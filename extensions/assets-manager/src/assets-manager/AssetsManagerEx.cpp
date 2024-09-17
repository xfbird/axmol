/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "AssetsManagerEx.h"
#include "EventListenerAssetsManagerEx.h"
#include "base/UTF8.h"
#include "base/Director.h"

#include <stdio.h>

#ifdef MINIZIP_FROM_SYSTEM
#    include <minizip/unzip.h>
#else  // from our embedded sources
#    include "unzip.h"
#endif
#include <ioapi.h>

NS_AX_EXT_BEGIN

#define TEMP_FOLDERNAME            "_temp"
#define VERSION_FILENAME           "version.manifest"
#define TEMP_MANIFEST_FILENAME     "project.manifest.temp"
#define MANIFEST_FILENAME          "project.manifest"

#define BUFFER_SIZE                8192
#define MAX_FILENAME               512

#define DEFAULT_CONNECTION_TIMEOUT 45

#define SAVE_POINT_INTERVAL        0.1

const std::string AssetsManagerEx::VERSION_ID  = "@version";
const std::string AssetsManagerEx::MANIFEST_ID = "@manifest";

class AssetManagerExZipFileInfo
{
public:
    std::string zipFileName{};
};

// unzip overrides to support FileStream
long AssetManagerEx_tell_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
        return -1;

    auto* fs = (IFileStream*)stream;

    return fs->tell();
}

long AssetManagerEx_seek_file_func(voidpf opaque, voidpf stream, uint32_t offset, int origin)
{
    if (stream == nullptr)
        return -1;

    auto* fs = (IFileStream*)stream;

    return fs->seek(offset, origin) != -1 ? 0 : -1;  // must return 0 for success or -1 for error
}

voidpf AssetManagerEx_open_file_func(voidpf opaque, const char* filename, int mode)
{
    IFileStream::Mode fsMode;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
        fsMode = IFileStream::Mode::READ;
    else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        fsMode = IFileStream::Mode::APPEND;
    else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        fsMode = IFileStream::Mode::WRITE;
    else
        return nullptr;

    return FileUtils::getInstance()->openFileStream(filename, fsMode).release();
}

voidpf AssetManagerEx_opendisk_file_func(voidpf opaque, voidpf stream, uint32_t number_disk, int mode)
{
    if (stream == nullptr)
        return nullptr;

    const auto zipFileInfo   = static_cast<AssetManagerExZipFileInfo*>(opaque);
    std::string diskFilename = zipFileInfo->zipFileName;

    const auto pos = diskFilename.rfind('.', std::string::npos);

    if (pos != std::string::npos && pos != 0)
    {
        const size_t bufferSize = 5;
        char extensionBuffer[bufferSize];
        snprintf(&extensionBuffer[0], bufferSize, ".z%02u", number_disk + 1);
        diskFilename.replace(pos, std::min((size_t)4, zipFileInfo->zipFileName.size() - pos), extensionBuffer);
        return AssetManagerEx_open_file_func(opaque, diskFilename.c_str(), mode);
    }

    return nullptr;
}

uint32_t AssetManagerEx_read_file_func(voidpf opaque, voidpf stream, void* buf, uint32_t size)
{
    if (stream == nullptr)
        return (uint32_t)-1;

    auto* fs = (IFileStream*)stream;
    return fs->read(buf, size);
}

uint32_t AssetManagerEx_write_file_func(voidpf opaque, voidpf stream, const void* buf, uint32_t size)
{
    if (stream == nullptr)
        return (uint32_t)-1;

    auto* fs = (IFileStream*)stream;
    return fs->write(buf, size);
}

int AssetManagerEx_close_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
        return -1;

    auto* fs          = (IFileStream*)stream;
    const auto result = fs->close();  // 0 for success, -1 for error
    delete fs;
    return result;
}

// THis isn't supported by FileStream, so just check if the stream is null and open
int AssetManagerEx_error_file_func(voidpf opaque, voidpf stream)
{
    if (stream == nullptr)
    {
        return -1;
    }

    auto* fs = (IFileStream*)stream;

    if (fs->isOpen())
    {
        return 0;
    }

    return -1;
}
// End of Overrides

// Implementation of AssetsManagerEx

AssetsManagerEx::AssetsManagerEx(std::string_view manifestUrl,std::string_view storagePath,bool needinit)
:_manifestUrl(manifestUrl)                 
{
    // Init variables
    AXLOGD("AssetsManagerEx manifestUrl:{} storagePath:{}",manifestUrl,storagePath);
    _eventDispatcher    = Director::getInstance()->getEventDispatcher();
    std::string pointer = fmt::format("{}", fmt::ptr(this));
    _eventName          = EventListenerAssetsManagerEx::LISTENER_ID + pointer;
    _fileUtils          = FileUtils::getInstance();
    _totalDiffFileSize  = 0.0;
    _InterruptFlag      = false;

    network::DownloaderHints hints = {static_cast<uint32_t>(_maxConcurrentTask), DEFAULT_CONNECTION_TIMEOUT, ".tmp"};
    _downloader                    = std::shared_ptr<network::Downloader>(new network::Downloader(hints));
    _downloader->onTaskError = std::bind(&AssetsManagerEx::onError, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3, std::placeholders::_4);
    _downloader->onTaskProgress = [this](const network::DownloadTask& task) {
        this->onProgress(task.progressInfo.totalBytesExpected, task.progressInfo.totalBytesReceived, task.requestURL,
                         task.identifier);
    };
    _downloader->onFileTaskSuccess = [this](const network::DownloadTask& task) {
        this->onSuccess(task.requestURL, task.storagePath, task.identifier);
    };
    AXLOGD("AssetsManagerEx setStoragePath:{}",storagePath);
    setStoragePath(storagePath);

    _tempVersionPath   = _tempStoragePath + VERSION_FILENAME;
    AXLOGD("AssetsManagerEx _tempVersionPath:{}",_tempVersionPath);
    
    _cacheManifestPath = _storagePath + MANIFEST_FILENAME;
    AXLOGD("AssetsManagerEx _cacheManifestPath:{}",_cacheManifestPath);
    _tempManifestPath  = _tempStoragePath + TEMP_MANIFEST_FILENAME;
    AXLOGD("AssetsManagerEx _tempManifestPath:{}",_tempManifestPath);
    if (needinit){
        initManifests(manifestUrl);
    }
}

AssetsManagerEx::~AssetsManagerEx()
{
    _downloader->onTaskError       = (nullptr);
    _downloader->onFileTaskSuccess = (nullptr);
    _downloader->onTaskProgress    = (nullptr);
    AX_SAFE_RELEASE(_localManifest);
    // _tempManifest could share a ptr with _remoteManifest or _localManifest
    if (_tempManifest != _localManifest && _tempManifest != _remoteManifest)
        AX_SAFE_RELEASE(_tempManifest);
    AX_SAFE_RELEASE(_remoteManifest);
}

AssetsManagerEx* AssetsManagerEx::create(std::string_view manifestUrl, std::string_view storagePath)
{
    AXLOGD("AssetsManagerEx::create manifestUrl:{} storagePath:{}",manifestUrl,storagePath);
    AssetsManagerEx* ret = new AssetsManagerEx(manifestUrl, storagePath,true);    
    ret->autorelease();
    return ret;
}

// int a_localManifestPath, 
//int a_storagePath, 
//char a_ManualUpdat, 
//int a_tempManifestName, 
//char *a_tempDirName, 
//int a_resUrl, 
//int a_manifestUrl, 
//int a_versionUrl

AssetsManagerEx* AssetsManagerEx::create( std::string_view localManifestPath,
                                    std::string_view storagePath,
                                    bool manualUpdat,
                                    std::string_view resUrl,
                                    std::string_view manifestUrl,
                                    std::string_view versionUrl)
{
    AXLOGD("AssetsManagerEx::create localManifestPath:{}",localManifestPath);
    AXLOGD("AssetsManagerEx::create storagePath:{}",storagePath);
    // AXLOGD("AssetsManagerEx::create resUrl:{}",resUrl);
    // AXLOGD("AssetsManagerEx::create manifestUrl:{}",manifestUrl);
    // AXLOGD("AssetsManagerEx::create versionUrl:{}",versionUrl);
    //localManifestPath: mod_launcher_project.manifest 
    //storagePath: D:/MW/axn/axxxcs/build/bin/axxxcs/Debug/Content/cache/mod_launcher/stab/ 
    //resUrl:http://127.0.0.1:8082/launchers/yshj/mod_launcher/1/stab/ 
    //manifestUrl:http://127.0.0.1:8082/launchers/yshj/mod_launcher/1/stab/mod_launcher_project.manifest 
    //versionUrl:http://127.0.0.1:8082/launchers/yshj/mod_launcher/1/stab/mod_launcher_version.manifest
    AXLOGD("AssetsManagerEx::create Call Base NoInitialize");
    AssetsManagerEx* ret = new AssetsManagerEx(localManifestPath, storagePath,false);
    AXLOGD("AssetsManagerEx::create Call Ext Initialize setManualUpdate manualUpdat:{}",manualUpdat);
    ret->setManualUpdate(manualUpdat);              //设置 手动 模式
    AXLOGD("AssetsManagerEx::create 基础构建完成  resUrl:{}",resUrl);
    AXLOGD("AssetsManagerEx::create 基础构建完成  manifestUrl:{}",manifestUrl);
    AXLOGD("AssetsManagerEx::create 基础构建完成  versionUrl:{}",versionUrl);
    ret->exinitManifests(localManifestPath,resUrl,manifestUrl,versionUrl);
    ret->autorelease();
    return ret;
}

void AssetsManagerEx::initManifests(std::string_view localManifestPath)
{
    AXLOGD("initManifests manifestUrl:{}",localManifestPath);
    _inited = true;
    // Init and load local manifest
    AXLOGD("initManifests _localManifest = new Manifest()");
    _localManifest = new Manifest();

    AXLOGD("initManifests loadLocalManifest");
    loadLocalManifest(localManifestPath);

    // Init and load temporary manifest
    // AXLOGD("initManifests create _tempManifest = new Manifest()  ");
    _tempManifest = new Manifest();                                 //本地临时的。
    AXLOGD("initManifests create _tempManifest = new Manifest()->parse({})",_tempManifestPath);
    _tempManifest->parse(_tempManifestPath);
    // Previous update is interrupted
    if (_fileUtils->isFileExist(_tempManifestPath))
    {
        // Manifest parse failed, remove all temp files
        AXLOGD("initManifests create isFileExist:{}",_tempManifestPath);
        if (!_tempManifest->isLoaded())
        {
            _fileUtils->removeDirectory(_tempStoragePath);
            AX_SAFE_RELEASE(_tempManifest);
            _tempManifest = nullptr;
            AXLOGD("initManifests   _tempManifest = nullptr");
        }
    }

    // Init remote manifest for future usage
    AXLOGD("initManifests   _remoteManifest = new Manifest()");
    _remoteManifest = new Manifest();                               //创建一个远程的

    if (!_inited)
    {
        AX_SAFE_RELEASE(_localManifest);
        AX_SAFE_RELEASE(_tempManifest);
        AX_SAFE_RELEASE(_remoteManifest);
        _localManifest  = nullptr;
        _tempManifest   = nullptr;
        _remoteManifest = nullptr;
        AXLOGD("initManifests  _localManifest  = nullptr; _tempManifest   = nullptr;_remoteManifest = nullptr;");        
    }
}

void AssetsManagerEx::exinitManifests(std::string_view localManifestPath,std::string_view resUrl,std::string_view manifestUrl,std::string_view versionUrl)
{
    // initManifests(localManifestPath);
    // AXLOGD("initManifests localManifestPath:{}",localManifestPath);
    _inited = true;
    // Init and load local manifest
    AXLOGD("exinitManifests _localManifest = new Manifest()");
    _localManifest = new Manifest();

    AXLOGD("exinitManifests localManifestPath:{}",localManifestPath);
    loadLocalManifest(localManifestPath);

    // Init and load temporary manifest
    AXLOGD("exinitManifests _tempManifest = new Manifest()");
    _tempManifest = new Manifest();                                 //本地临时的。
    AXLOGD("exinitManifests  _tempManifest->parse({})",_tempManifestPath);
    _tempManifest->parse(_tempManifestPath);
    // Previous update is interrupted
    if (_fileUtils->isFileExist(_tempManifestPath))
    {
        // Manifest parse failed, remove all temp files
        if (!_tempManifest->isLoaded())
        {
            _fileUtils->removeDirectory(_tempStoragePath);
            AX_SAFE_RELEASE(_tempManifest);
            _tempManifest = nullptr;
            AXLOGD("exinitManifests   _tempManifest = nullptr");
        }
    }

    // Init remote manifest for future usage
    AXLOGD("exinitManifests   _remoteManifest = new Manifest()");
    _remoteManifest = new Manifest();                               //创建一个远程的
    if (_remoteManifest) {
        _remoteManifest->setManifestFileUrl(manifestUrl);
        AXLOGD("exinitManifests   setManifestFileUrl:{}",manifestUrl);
        _remoteManifest->setVersionFileUrl(versionUrl);
        AXLOGD("exinitManifests   setVersionFileUrl:{}",versionUrl);
        _remoteManifest->setpackageUrl(resUrl);
        AXLOGD("exinitManifests   setpackageUrl:{}",resUrl);        
    }

    if (!_inited)
    {
        AX_SAFE_RELEASE(_localManifest);
        AX_SAFE_RELEASE(_tempManifest);
        AX_SAFE_RELEASE(_remoteManifest);
        _localManifest  = nullptr;
        _tempManifest   = nullptr;
        _remoteManifest = nullptr;
        AXLOGD("exinitManifests  _localManifest  = nullptr; _tempManifest   = nullptr;_remoteManifest = nullptr;");        
    }
}


void AssetsManagerEx::prepareLocalManifest()
{
    // An alias to assets
    AXLOGD("prepareLocalManifest");
    _assets = &(_localManifest->getAssets());           //获得本地资产管理表
    AXLOGD("_localManifest->prependSearchPaths()");
    // Add search paths
    _localManifest->prependSearchPaths();               //准备 搜索路径
}

void AssetsManagerEx::loadLocalManifest(std::string_view manifestUrl /*manifestUrl*/)       //装载 本地的资产清单
{
    Manifest* cachedManifest = nullptr;
    // AXLOGD("loadLocalManifest  cachedManifest:nullptr _cacheManifestPath:{}",_cacheManifestPath);
    // Find the cached manifest file
    if (_fileUtils->isFileExist(_cacheManifestPath))                           //缓存的资产
    {
        cachedManifest = new Manifest();                                       //创建一个缓存资产 
        AXLOGD("loadLocalManifest  _cacheManifestPath:{} isFileExist Create,cachedManifest->parse(_cacheManifestPath)",_cacheManifestPath);
        cachedManifest->parse(_cacheManifestPath);                             //解析 缓存路径的配置文件 
        if (!cachedManifest->isLoaded())                                       //未转入 就删除文件     
        {
            _fileUtils->removeFile(_cacheManifestPath);
            AXLOGD("loadLocalManifest  remove _cacheManifestPath:{} cachedManifest = nullptr",_cacheManifestPath);
            AX_SAFE_RELEASE(cachedManifest);
            cachedManifest = nullptr;
        }
    }

    // Ensure no search path of cached manifest is used to load this manifest
    std::vector<std::string> searchPaths = _fileUtils->getSearchPaths();    //获得搜索路径
    if (cachedManifest)
    {
        std::vector<std::string> cacheSearchPaths = cachedManifest->getSearchPaths(); //获得搜索路径  
        std::vector<std::string> trimmedPaths     = searchPaths;                      //  
        for (const auto& path : cacheSearchPaths)
        {
            const auto pos = std::find(trimmedPaths.begin(), trimmedPaths.end(), path);
            if (pos != trimmedPaths.end())
            {
                trimmedPaths.erase(pos);
            }
        }
        AXLOGD("loadLocalManifest cachedManifest  setSearchPaths ");
        _fileUtils->setSearchPaths(trimmedPaths);
    }
    // Load local manifest in app package
    AXLOGD("loadLocalManifest _localManifest->parse({})",_manifestUrl);
    _localManifest->parse(_manifestUrl);                        //本地 解析 资产文件
    if (cachedManifest)
    {
        // Restore search paths
        _fileUtils->setSearchPaths(searchPaths);                //设置搜索目录
    }
    if (_localManifest->isLoaded())                             //本地 已经装入
    {
        // Compare with cached manifest to determine which one to use
        if (cachedManifest)
        {
            AXLOGD("loadLocalManifest CheckNewer _localManifest cachedManifest");
            bool localNewer = _localManifest->versionGreater(cachedManifest, _versionCompareHandle);    //版本大于的 进行 处理
            if (localNewer)                                                 //本地新，删除 目录。
            {
                // Recreate storage, to empty the content
                _fileUtils->removeDirectory(_storagePath);
                _fileUtils->createDirectory(_storagePath);
                AXLOGD("本地更新 loadLocalManifest isNewer removeDirectory({})",_storagePath);
                AXLOGD("本地更新 loadLocalManifest isNewer createDirectory({})",_storagePath);
                AXLOGD("本地更新 loadLocalManifest AX_SAFE_RELEASE(cachedManifest)",_storagePath);
                AX_SAFE_RELEASE(cachedManifest);
            }
            else
            {
                AXLOGD("缓存更新 loadLocalManifest AX_SAFE_RELEASE(_localManifest)");
                AX_SAFE_RELEASE(_localManifest);                            //释放 本地资产。
                AXLOGD("缓存更新 loadLocalManifest cachedManifest->>_localManifest ,用缓存 作为本地 清单");
                _localManifest = cachedManifest;                            //用缓存 作为本地资产
            }
        }
        AXLOGD("loadLocalManifest prepareLocalManifest");
        prepareLocalManifest();                                             //准备本地资产。
    }

    // Fail to load local manifest
    if (!_localManifest->isLoaded())
    {
        AXLOGD("AssetsManagerEx : No local manifest file found error.\n");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
    }
}

std::string_view AssetsManagerEx::basename(std::string_view path) const
{
    size_t found = path.find_last_of("/\\");

    if (std::string::npos != found)
    {
        return path.substr(0, found);
    }
    else
    {
        return path;
    }
}

std::string AssetsManagerEx::get(std::string_view key) const
{
    auto it = _assets->find(key);
    if (it != _assets->cend())
    {
        return _storagePath + it->second.path;
    }
    else
        return "";
}

const Manifest* AssetsManagerEx::getLocalManifest() const
{
    AXLOGD("getLocalManifest:{}",fmt::ptr(_localManifest));
    return _localManifest;
}

const Manifest* AssetsManagerEx::getRemoteManifest() const
{
    AXLOGD("getRemoteManifest:{}",fmt::ptr(_remoteManifest));
    return _remoteManifest;
}

std::string_view AssetsManagerEx::getStoragePath() const
{
    AXLOGD("getStoragePath:{}",_storagePath);
    return _storagePath;
}

void AssetsManagerEx::setStoragePath(std::string_view storagePath)
{
    AXLOGD("setStoragePath:{}",storagePath);
    _storagePath = storagePath;
    adjustPath(_storagePath);
    _fileUtils->createDirectories(_storagePath);

    _tempStoragePath = _storagePath;
    _tempStoragePath.append(TEMP_FOLDERNAME);
    adjustPath(_tempStoragePath);
    _fileUtils->createDirectories(_tempStoragePath);
}

void AssetsManagerEx::adjustPath(std::string& path)
{
    if (!path.empty() && path[path.size() - 1] != '/')
    {
        path.push_back('/');
    }
}

bool AssetsManagerEx::decompress(std::string_view zip)
{
    // Find root path for zip file
    size_t pos = zip.find_last_of("/\\");
    if (pos == std::string::npos)
    {
        AXLOGD("AssetsManagerEx : no root path specified for zip file {}\n", zip);
        return false;
    }
    const std::string_view rootPath = zip.substr(0, pos + 1);

    zlib_filefunc_def_s zipFunctionOverrides;
    fillZipFunctionOverrides(zipFunctionOverrides);

    AssetManagerExZipFileInfo zipFileInfo;
    zipFileInfo.zipFileName = zip;

    zipFunctionOverrides.opaque = &zipFileInfo;

    // Open the zip file
    unzFile zipfile = unzOpen2(zip.data(), &zipFunctionOverrides);
    if (!zipfile)
    {
        AXLOGD("AssetsManagerEx : can not open downloaded zip file {}\n", zip);
        return false;
    }

    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        AXLOGD("AssetsManagerEx : can not read file global info of {}\n", zip);
        unzClose(zipfile);
        return false;
    }

    // Buffer to hold data read from the zip file
    char readBuffer[BUFFER_SIZE];
    // Loop to extract all files.
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        // Get info about current file.
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile, &fileInfo, fileName, MAX_FILENAME, NULL, 0, NULL, 0) != UNZ_OK)
        {
            AXLOGD("AssetsManagerEx : can not read compressed file info\n");
            unzClose(zipfile);
            return false;
        }
        std::string fullPath{rootPath};
        fullPath += fileName;

        // Check if this entry is a directory or a file.
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength - 1] == '/')
        {
            // There are not directory entry in some case.
            // So we need to create directory when decompressing file entry
            if (!_fileUtils->createDirectories(basename(fullPath)))
            {
                // Failed to create directory
                AXLOGD("AssetsManagerEx : can not create directory {}\n", fullPath);
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            // Create all directories in advance to avoid issue
            std::string_view dir = basename(fullPath);
            if (!_fileUtils->isDirectoryExist(dir))
            {
                if (!_fileUtils->createDirectories(dir))
                {
                    // Failed to create directory
                    AXLOGD("AssetsManagerEx : can not create directory {}\n", fullPath);
                    unzClose(zipfile);
                    return false;
                }
            }
            // Entry is a file, so extract it.
            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                AXLOGD("AssetsManagerEx : can not extract file {}\n", fileName);
                unzClose(zipfile);
                return false;
            }

            // Create a file to store current file.
            auto fsOut = FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::WRITE);
            if (!fsOut)
            {
                AXLOGD("AssetsManagerEx : can not create decompress destination file {} (errno: {})\n", fullPath,
                      errno);
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }

            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0)
                {
                    AXLOGD("AssetsManagerEx : can not read zip file {}, error code is {}\n", fileName, error);
                    fsOut.reset();
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }

                if (error > 0)
                {
                    fsOut->write(readBuffer, error);
                }
            } while (error > 0);

            fsOut.reset();
        }

        unzCloseCurrentFile(zipfile);

        // Goto next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                AXLOGD("AssetsManagerEx : can not read next file for decompressing\n");
                unzClose(zipfile);
                return false;
            }
        }
    }

    unzClose(zipfile);
    return true;
}

void AssetsManagerEx::decompressDownloadedZip(std::string_view customId, std::string_view storagePath)
{
    struct AsyncData
    {
        std::string customId;
        std::string zipFile;
        bool succeed;
    };

    AsyncData* asyncData = new AsyncData;
    asyncData->customId  = customId;
    asyncData->zipFile   = storagePath;
    asyncData->succeed   = false;

    std::function<void(void*)> decompressFinished = [this](void* param) {
        auto dataInner = reinterpret_cast<AsyncData*>(param);
        if (dataInner->succeed)
        {
            fileSuccess(dataInner->customId, dataInner->zipFile);
        }
        else
        {
            std::string errorMsg = "Unable to decompress file " + dataInner->zipFile;
            // Ensure zip file deletion (if decompress failure cause task thread exit anormally)
            _fileUtils->removeFile(dataInner->zipFile);
            dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS, "", errorMsg);
            fileError(dataInner->customId, errorMsg);
        }
        delete dataInner;
    };

    Director::getInstance()->getJobSystem()->enqueue(
        [this, asyncData]() {
        // Decompress all compressed files
        if (decompress(asyncData->zipFile))
        {
            asyncData->succeed = true;
        }
        _fileUtils->removeFile(asyncData->zipFile);
    },
        [decompressFinished, asyncData]() { decompressFinished(asyncData); });
}

void AssetsManagerEx::dispatchUpdateEvent(EventAssetsManagerEx::EventCode code,
                                          std::string_view assetId /* = ""*/,
                                          std::string_view message /* = ""*/,
                                          int curle_code /* = CURLE_OK*/,
                                          int curlm_code /* = CURLM_OK*/)
{
    AXLOGD("dispatchUpdateEvent code:{} assetId:{} message:{} curle_code:{} curlm_code:{}",int(code),assetId,message,curle_code,curlm_code);
    
    switch (code)
    {
    case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
    case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
    case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
    case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
    case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
    case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
    case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
    case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
        _updateEntry = UpdateEntry::NONE;
        break;
    case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
        break;
    case EventAssetsManagerEx::EventCode::ASSET_UPDATED:
        break;
    case EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND:
        if (_updateEntry == UpdateEntry::CHECK_UPDATE)
        {
            _updateEntry = UpdateEntry::NONE;
        }
        break;
    default:
        break;
    }

    EventAssetsManagerEx event(_eventName, this, code, _percent, _percentByFile, assetId, message, curle_code,curlm_code);
    AXLOGD("dispatchUpdateEvent _updateEntry:{} _percent:{}  _percentByFile:{}",
           int(_updateEntry),_percent, _percentByFile);
    _eventDispatcher->dispatchEvent(&event);
}

AssetsManagerEx::State AssetsManagerEx::getState() const
{
    AXLOGD("getState :{}",int(_updateState));
    return _updateState;
}

void AssetsManagerEx::downloadVersion()
{
    AXLOGD("downloadVersion");
    if (_updateState > State::PREDOWNLOAD_VERSION)
        return;

    std::string_view  versionUrl=_remoteManifest->getVersionFileUrl();
    AXLOGD("downloadVersion _remoteManifest versionUrl:{}",versionUrl);
    if (versionUrl.empty()){
        versionUrl=_localManifest->getVersionFileUrl();
        AXLOGD("downloadVersion _localManifest versionUrl:{}",versionUrl);
    }

    AXLOGD("downloadVersion  versionUrl:{}",versionUrl);

    if (!versionUrl.empty())
    {
        _updateState = State::DOWNLOADING_VERSION;
        // Download version file asynchronously
        AXLOGD("createDownloadFileTask(versionUrl:{}, _tempVersionPath:{}, VERSION_ID:{})",versionUrl,_tempVersionPath,VERSION_ID);
        _downloader->createDownloadFileTask(versionUrl, _tempVersionPath, VERSION_ID);
    }
    // No version file found
    else
    {
        AXLOGW("AssetsManagerEx : No version file found, step skipped\n");
        _updateState = State::PREDOWNLOAD_MANIFEST;
        downloadManifest();
    }
}

void AssetsManagerEx::parseVersion()
{
    AXLOGD("parseVersion");
    if (_updateState != State::VERSION_LOADED)
        return;

    _remoteManifest->parseVersion(_tempVersionPath);

    if (!_remoteManifest->isVersionLoaded())
    {
        AXLOGD("AssetsManagerEx : Fail to parse version file, step skipped\n");
        _updateState = State::PREDOWNLOAD_MANIFEST;
        AXLOGD("downloadManifest");
        downloadManifest();
    }
    else
    {
        if (_localManifest->versionGreater(_remoteManifest, _versionCompareHandle))     //本地和远程比较版本
        {
            _updateState = State::UP_TO_DATE;                                           //本地 新
            _fileUtils->removeDirectory(_tempStoragePath);                              //删除 临时目录
            AXLOGD("callback ALREADY_UP_TO_DATE");
            dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE);   //通知
        }
        else
        {
            _updateState = State::NEED_UPDATE;                                          //本地 比 远程 旧 需要更新

            // Wait to update so continue the process
            if (_updateEntry == UpdateEntry::DO_UPDATE)                                 //设置更新状态 需要更新
            {
                // dispatch after checking update entry because event dispatching may modify the update entry
                AXLOGD("callback NEW_VERSION_FOUND");
                dispatchUpdateEvent(EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND);    //通知
                _updateState = State::PREDOWNLOAD_MANIFEST;                             //预下载资产
                AXLOGD("State::PREDOWNLOAD_MANIFEST  downloadManifest ");
                downloadManifest();                                                     //下载 资产
            }
            else
            {
                AXLOGD("callback NEW_VERSION_FOUND");
                dispatchUpdateEvent(EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND);
            }
        }
    }
}

void AssetsManagerEx::downloadManifest()
{
    AXLOGD("downloadManifest");
    if (_updateState != State::PREDOWNLOAD_MANIFEST)
        return;

    std::string manifestUrl;
    if (_remoteManifest->isVersionLoaded())                                             //远程版本 已经装载
    {
        manifestUrl = _remoteManifest->getManifestFileUrl();                            //获得 资产url
        AXLOGD("manifestUrl :{}",manifestUrl);
    }
    else
    {
        manifestUrl = _localManifest->getManifestFileUrl();                             //使用本地的 远程 资产url
        AXLOGD("manifestUrl :{}",manifestUrl);
    }

    if (!manifestUrl.empty())                                                          //url有效          
    {
        _updateState = State::DOWNLOADING_MANIFEST;
        // Download version file asynchronously
        AXLOGD("State::DOWNLOADING_MANIFEST  createDownloadFileTask(manifestUrl:{})",manifestUrl);
        AXLOGD("State::DOWNLOADING_MANIFEST  createDownloadFileTask(_tempManifestPath:{})",_tempManifestPath);
        AXLOGD("State::DOWNLOADING_MANIFEST  createDownloadFileTask(MANIFEST_ID:{})",MANIFEST_ID);
        _downloader->createDownloadFileTask(manifestUrl, _tempManifestPath, MANIFEST_ID);   //建立下载任务
    }
    // No manifest file found
    else
    {
        AXLOGW("AssetsManagerEx : No manifest file found, check update failed\n");
        AXLOGD("callback ERROR_DOWNLOAD_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST);  //通知 错误。 
        _updateState = State::UNCHECKED;
        AXLOGD("State::UNCHECKED");
    }
}

void AssetsManagerEx::parseManifest()
{
    AXLOGD("parseManifest");
    if (_updateState != State::MANIFEST_LOADED)
        return;

    _remoteManifest->parse(_tempManifestPath);                                          //远程 解析 临时 路径
    AXLOGD("_remoteManifest :{}",_tempManifestPath);                                    //

    if (!_remoteManifest->isLoaded())                                                   //如果远程 未加载
    {
        AXLOGW("AssetsManagerEx : Error parsing manifest file, {}", _tempManifestPath);
        AXLOGD("callback ERROR_PARSE_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST);     //出错通知
        _updateState = State::UNCHECKED;
        AXLOGD("State::UNCHECKED");
    }
    else
    {
        if (_localManifest->versionGreater(_remoteManifest, _versionCompareHandle))     //本地 与远程比较版本
        {
            _updateState = State::UP_TO_DATE;                                           //本地 新    
            AXLOGD(" _updateState = State::UP_TO_DATE removeDirectory(_tempStoragePath)");
            _fileUtils->removeDirectory(_tempStoragePath);                              //删除 临时目录
            AXLOGD("callback ALREADY_UP_TO_DATE");
            dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE);   //通知 已经更新完成
        }
        else
        {
            _updateState = State::NEED_UPDATE;                                          //需要 更新    
            AXLOGD("_updateState 设置为 State::NEED_UPDATE");
            if (_updateEntry == UpdateEntry::DO_UPDATE)
            {
              AXLOGD("NEED_UPDATE DO_UPDATE _ManualUpdate:{}",_ManualUpdate);
              if (!_ManualUpdate) {
                AXLOGD("自动模式 EventCode::NEW_VERSION_FOUND startUpdate");
                dispatchUpdateEvent(EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND);//通知 存在新版本
                startUpdate();                                                          //开始更新。
              }else
              {
                AXLOGD("手动模式 计算更新的变更 calcUpdateDiff  ");
                calcUpdateDiff();
                AXLOGD("EventCode::NEW_VERSION_FOUND");
                dispatchUpdateEvent(EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND);//通知 存在新版本
                AXLOGD("EventCode::NEW_VERSION_FOUND 通知完成");
              }
            }
        }
    }
}

void AssetsManagerEx::startUpdate()
{
    AXLOGD("AssetsManagerEx::startUpdate");
    if (_updateState != State::NEED_UPDATE)
        return;

    _updateState = State::UPDATING;                                                   //进入更新状态 8
    AXLOGD(" _updateState = State::UPDATING;");
    // Clean up before update
    _failedUnits.clear();                                                             //失败单元表
    _downloadUnits.clear();                                                           //下载单元表  
    _totalWaitToDownload = _totalToDownload = 0;                                      //总等待 下载 总下载  
    _nextSavePoint                          = 0;                                      //              
    _percent = _percentByFile = _sizeCollected = _totalSize = 0;                      //百分比          
    _totalDiffFileSize=0.0;                                                           //更新之前 清零                  
    _downloadedSize.clear();                                                          //下载尺寸 清零  
    _totalEnabled = false;

    // Temporary manifest exists, resuming previous download
    if (_tempManifest) {
        AXLOGD("_tempManifest->isLoaded() :{} _tempManifest->versionEquals(_remoteManifest)",int(_tempManifest->isLoaded()),int(_tempManifest->versionEquals(_remoteManifest)));
    }
    
    if (_tempManifest && _tempManifest->isLoaded() && 
        _tempManifest->versionEquals(_remoteManifest)) //临时目录 存在，并已经装载 与远程版本一致
    {
        AXLOGD("_tempManifest->isLoaded() and _tempManifest->versionEquals(_remoteManifest) saveToFile(_tempManifestPath):{}",_tempManifestPath);
        _tempManifest->saveToFile(_tempManifestPath);                                 //临时资产保存到 临时目录  
        _tempManifest->genResumeAssetsList(&_downloadUnits);                          //生成 继续资产表  
        _totalWaitToDownload = _totalToDownload = (int)_downloadUnits.size();         //

        AXLOGD("batchDownload");
        this->batchDownload();                                                        //执行批量下载  

        std::string msg = fmt::format(
            "Resuming from previous unfinished update, {} files remains to be finished.", _totalToDownload);
        AXLOGD("callback UPDATE_PROGRESSION :{}",msg);            
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, "", msg);  //通知状态
    }
    else
    {
        // Temporary manifest exists, but can't be parsed or version doesn't equals remote manifest (out of date)
        AXLOGD("_tempStoragePath  remove ");
        if (_tempManifest)                                                                //          
        {
            // Remove all temp files

            _fileUtils->removeDirectory(_tempStoragePath);                                //  
            AX_SAFE_RELEASE(_tempManifest);
            // Recreate temp storage path and save remote manifest
            _fileUtils->createDirectories(_tempStoragePath);
            _remoteManifest->saveToFile(_tempManifestPath);
            AXLOGD("saveToFile(_tempManifestPath):{}",_tempManifestPath);
        }

        // Temporary manifest will be used to register the download states of each asset,
        // in this case, it equals remote manifest.
        _tempManifest = _remoteManifest;
        AXLOGD("_tempManifest = _remoteManifest;");

        // Check difference between local manifest and remote manifest
        AXLOGD("_localManifest->genDiff(_remoteManifest)");
        hlookup::string_map<Manifest::AssetDiff> diff_map = _localManifest->genDiff(_remoteManifest);

        if (diff_map.empty())
        {
           AXLOGD("diff_map.empty() updateSucceed");
           updateSucceed();
        }
        else
        {
            // Generate download units for all assets that need to be updated or added
            std::string_view packageUrl = _remoteManifest->getPackageUrl();
            AXLOGD("packageUrl:{}",packageUrl);
            // Save current download manifest information for resuming
            _tempManifest->saveToFile(_tempManifestPath);
            AXLOGD("saveToFile(_tempManifestPath):{}",_tempManifestPath);
            // Preprocessing local files in previous version and creating download folders
            for (auto it = diff_map.begin(); it != diff_map.end(); ++it)
            {
                Manifest::AssetDiff diff = it->second;
                if (diff.type != Manifest::DiffType::DELETED)
                {
                    auto& path = diff.asset.path;
                    DownloadUnit unit;
                    unit.customId = it->first;
                    unit.srcUrl   = packageUrl;
                    unit.srcUrl += path;
                    unit.storagePath = _tempStoragePath + path;
                    unit.size        = diff.asset.size;
                    _totalDiffFileSize+=unit.size;                                            //不同的 文件尺寸 记录  
                    AXLOGD("DownloadUnit unit Add unit.srcUrl:{},unit.size:{} unit.customId:{}",unit.srcUrl,unit.size,unit.customId);
                    _downloadUnits.emplace(unit.customId, unit);
                    AXLOGD("UNSTARTED: key:{} DownloadState::UNSTARTED",it->first);
                    _tempManifest->setAssetDownloadState(it->first, Manifest::DownloadState::UNSTARTED);
                }
            }
            
            _totalWaitToDownload = _totalToDownload = (int)_downloadUnits.size();
            AXLOGD("callback UPDATE_PROGRESSION _totalWaitToDownload:{}",_totalWaitToDownload);
            this->batchDownload();

            std::string msg = fmt::format("Start to update {} files from remote package.", _totalToDownload);
            AXLOGD("callback UPDATE_PROGRESSION :{}",msg);
            dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, "", msg);
        }
    }
}
void AssetsManagerEx::startManualUpdate()
{
    AXLOGD("_updateEntry:{} _ManualUpdate:{}",int(_updateEntry),int(_ManualUpdate));
    // if ((_updateEntry == UpdateEntry::DO_UPDATE) && (_ManualUpdate)) {
    if ((_updateEntry == UpdateEntry::DO_UPDATE) && (_ManualUpdate)) {
        std::string msg = fmt::format("Start to update {} files manually.", _totalToDownload);
        AXLOGD("callback UPDATE_PROGRESSION :{}",msg);
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, "", msg);
        AXLOGD("call queueDowload");
        queueDowload();
    }
}
void AssetsManagerEx::calcUpdateDiff()
{
    
    if (_updateState != State::NEED_UPDATE)
    {
        AXLOGD("calcUpdateDiff _updateState != State::NEED_UPDATE: _updateState:{} 直接退出",int(_updateState));
        return;
    }
        

    _updateState = State::UPDATING;                                                   //进入更新状态 8
    AXLOGD("calcUpdateDiff _updateState = State::UPDATING");
    // Clean up before update
    AXLOGD("calcUpdateDiff _failedUnits.clear()");
    _failedUnits.clear();                                                             //失败单元表
    AXLOGD("calcUpdateDiff _downloadUnits.clear()");
    _downloadUnits.clear();                                                           //下载单元表  
    _totalWaitToDownload = _totalToDownload = 0;                                      //总等待 下载 总下载  
    _nextSavePoint                          = 0;                                      //              
    _percent = _percentByFile = _sizeCollected = _totalSize = 0;                      //百分比          
    _totalDiffFileSize=0.0;                                                           //更新之前 清零                  
    AXLOGD("calcUpdateDiff _downloadedSize.clear()");
    _downloadedSize.clear();                                                          //下载尺寸 清零  
    _totalEnabled = false;

    bool tempManifestPathSave =false;
    // Temporary manifest exists, resuming previous download
    if (_tempManifest) {
        AXLOGD("calcUpdateDiff _tempManifest->isLoaded() :{} _tempManifest->versionEquals(_remoteManifest) :{}",
               int(_tempManifest->isLoaded()),int(_tempManifest->versionEquals(_remoteManifest)));
    }
    
    if (_tempManifest && _tempManifest->isLoaded() && 
        _tempManifest->versionEquals(_remoteManifest)) //临时目录 存在，并已经装载 与远程版本一致
    {
        AXLOGD("calcUpdateDiff    _tempManifest 已经装载  版本一致");
        _tempManifest->saveToFile(_tempManifestPath);                                 //临时资产保存到 临时目录  
        tempManifestPathSave=true;
        _tempManifest->genResumeAssetsList(&_downloadUnits);                          //生成 继续资产表  
        _totalWaitToDownload = _totalToDownload = (int)_downloadUnits.size();         //
        AXLOGD("calcUpdateDiff 生成了 变化表  _totalWaitToDownload:{}",_totalWaitToDownload);
    }
    else
    {
        if (_tempManifest)                                                                //          
        {
            AXLOGD("calcUpdateDiff    _tempManifest 存在 删除 临时目录 删除 _tempManifest");
            _fileUtils->removeDirectory(_tempStoragePath);                                //  
            AX_SAFE_RELEASE(_tempManifest);
            AXLOGD("calcUpdateDiff    重建 临时目录 ");
            _fileUtils->createDirectory(_tempStoragePath);
            // AXLOGD("calcUpdateDiff   _remoteManifest 把文件存储到 临时目录:{}",_tempManifestPath);
            _remoteManifest->saveToFile(_tempManifestPath);
            tempManifestPathSave=true;
            AXLOGD("calcUpdateDiff _remoteManifest->saveToFile(_tempManifestPath):{} 完成",_tempManifestPath);
        }

        // Temporary manifest will be used to register the download states of each asset,
        // in this case, it equals remote manifest.
        _tempManifest = _remoteManifest;
        AXLOGD("calcUpdateDiff _tempManifest = _remoteManifest;");

        // Check difference between local manifest and remote manifest
        AXLOGD("calcUpdateDiff _localManifest->genDiff(_remoteManifest)");
        hlookup::string_map<Manifest::AssetDiff> diff_map = _localManifest->genDiff(_remoteManifest);

        if (diff_map.empty())
        {
           AXLOGD("calcUpdateDiff diff_map.empty() updateSucceed");
           updateSucceed();
        }
        else
        {
            // Generate download units for all assets that need to be updated or added
            std::string_view packageUrl = _remoteManifest->getPackageUrl();
            AXLOGD("calcUpdateDiff packageUrl:{}",packageUrl);
            // Save current download manifest information for resuming
            if (!tempManifestPathSave) {
                _tempManifest->saveToFile(_tempManifestPath);
                AXLOGD("calcUpdateDiff 需要保存 _tempManifest->saveToFile(_tempManifestPath):{}",_tempManifestPath);
                tempManifestPathSave=true;
            }
            // Preprocessing local files in previous version and creating download folders
            AXLOGD("calcUpdateDiff 遍历  diff_map ");
            for (auto it = diff_map.begin(); it != diff_map.end(); ++it)
            {
                Manifest::AssetDiff diff = it->second;
                if (diff.type != Manifest::DiffType::DELETED)
                {
                    auto& path = diff.asset.path;
                    DownloadUnit unit;
                    unit.customId = it->first;
                    unit.srcUrl   = packageUrl;
                    unit.srcUrl += path;
                    unit.storagePath = _tempStoragePath + path;
                    unit.size        = diff.asset.size;
                    _totalDiffFileSize+=unit.size;                                            //不同的 文件尺寸 记录  
                    AXLOGD("DownloadUnit calcUpdateDiff unit Add unit.srcUrl:{},unit.size:{} unit.customId:{}",unit.srcUrl,unit.size,unit.customId);
                    _downloadUnits.emplace(unit.customId, unit);
                    AXLOGD("calcUpdateDiff UNSTARTED: key:{} DownloadState::UNSTARTED",it->first);
                    _tempManifest->setAssetDownloadState(it->first, Manifest::DownloadState::UNSTARTED);
                    tempManifestPathSave=false;
                }
            }
            
            _totalWaitToDownload = _totalToDownload = (int)_downloadUnits.size();
            AXLOGD("calcUpdateDiff callback UPDATE_PROGRESSION _totalWaitToDownload:{}",_totalWaitToDownload);
            // this->batchDownload();
            if (!tempManifestPathSave) {
                _tempManifest->saveToFile(_tempManifestPath);
                AXLOGD("calcUpdateDiff 需要保存 _tempManifest->saveToFile(_tempManifestPath):{}",_tempManifestPath);
                // tempManifestPathSave=true;
            }
            // std::string msg = fmt::format("Start to update {} files from remote package.", _totalToDownload);
            AXLOGD("calcUpdateDiff  have download");
            // dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, "", msg);
        }
    };
    _queue.clear();    
    AXLOGD("calcUpdateDiff 清零 _queue 队列  遍历");
    for (const auto& iter : _downloadUnits)
    {
        const DownloadUnit& unit = iter.second;
        if (unit.size > 0)
        {
            _totalSize += unit.size;
            _sizeCollected++;
            _totalDiffFileSize=_totalDiffFileSize+ unit.size;   
            _queue.emplace_back(iter.first);
            AXLOGD("calcUpdateDiff customId:{} add _queue",unit.customId);
        }
    }
    // All collected, enable total size
    if (_sizeCollected == _totalToDownload)
    {
        _totalEnabled = true;
        AXLOGD("calcUpdateDiff _totalEnabled true");
    }
}

void AssetsManagerEx::updateSucceed()
{
    // Every thing is correctly downloaded, do the following
    // 1. rename temporary manifest to valid manifest
    AXLOGD("updateSucceed");
    std::string tempFileName = TEMP_MANIFEST_FILENAME;
    std::string fileName     = MANIFEST_FILENAME;
    _fileUtils->renameFile(_tempStoragePath, tempFileName, fileName);
    // 2. merge temporary storage path to storage path so that temporary version turns to cached version
    if (_fileUtils->isDirectoryExist(_tempStoragePath))
    {
        // Merging all files in temp storage path to storage path
        std::vector<std::string> files;
        _fileUtils->listFilesRecursively(_tempStoragePath, &files);
        int baseOffset = (int)_tempStoragePath.length();
        std::string relativePath, dstPath;
        for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
        {
            relativePath.assign((*it).substr(baseOffset));
            dstPath.assign(_storagePath + relativePath);
            // Create directory
            if (relativePath.back() == '/')
            {
                _fileUtils->createDirectories(dstPath);
            }
            // Copy file
            else
            {
                if (_fileUtils->isFileExist(dstPath))
                {
                    _fileUtils->removeFile(dstPath);
                }
                _fileUtils->renameFile(*it, dstPath);
            }
        }
        // Remove temp storage path
        _fileUtils->removeDirectory(_tempStoragePath);
    }
    // 3. swap the localManifest
    AX_SAFE_RELEASE(_localManifest);
    _localManifest = _remoteManifest;
    _localManifest->setManifestRoot(_storagePath);
    _remoteManifest = nullptr;
    // 4. make local manifest take effect
    AXLOGD("prepareLocalManifest");
    prepareLocalManifest();
    // 5. Set update state
    _updateState = State::UP_TO_DATE;
    AXLOGD("State::UP_TO_DATE");
    // 6. Notify finished event
    AXLOGD("callback UPDATE_FINISHED");
    dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_FINISHED);
}

void AssetsManagerEx::checkUpdate()
{
    AXLOGD("checkUpdate");
    if (_updateEntry != UpdateEntry::NONE)
    {
        AXLOGE("AssetsManagerEx::checkUpdate, updateEntry isn't NONE");
        return;
    }

    if (!_inited)
    {
        AXLOGD("AssetsManagerEx : Manifests uninited.\n");
        AXLOGD("callback ERROR_NO_LOCAL_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
        return;
    }
    if (!_localManifest->isLoaded())
    {
        AXLOGD("AssetsManagerEx : No local manifest file found error.\n");
        AXLOGD("callback ERROR_NO_LOCAL_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
        return;
    }

    _updateEntry = UpdateEntry::CHECK_UPDATE;

    switch (_updateState)
    {
    case State::UNCHECKED:
    case State::PREDOWNLOAD_VERSION:
    {
        AXLOGD("downloadVersion");
        downloadVersion();
    }
    break;
    case State::UP_TO_DATE:
    {
        AXLOGD("callback ALREADY_UP_TO_DATE");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE);
    }
    break;
    case State::FAIL_TO_UPDATE:
    case State::NEED_UPDATE:
    {
        AXLOGD("callback NEW_VERSION_FOUND");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::NEW_VERSION_FOUND);
    }
    break;
    default:
        break;
    }
}

void AssetsManagerEx::update()
{
    AXLOGD("AssetsManagerEx::update");
    if (_updateEntry != UpdateEntry::NONE)
    {
        AXLOGE("AssetsManagerEx::update, updateEntry isn't NONE");
        return;
    }

    AXLOGD("AssetsManagerEx::update _updateEntry == UpdateEntry::NONE");
    if (!_inited)
    {
        AXLOGW("AssetsManagerEx : Manifests uninited.\n");
        AXLOGD("callback  ERROR_NO_LOCAL_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
        return;
    }
    AXLOGD("AssetsManagerEx::update _inited");
    if (!_localManifest->isLoaded())
    {
        AXLOGW("AssetsManagerEx : No local manifest file found error.\n");
        AXLOGD("callback  ERROR_NO_LOCAL_MANIFEST");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
        return;
    }

    _updateEntry = UpdateEntry::DO_UPDATE;
    AXLOGD("AssetsManagerEx::update UpdateEntry::DO_UPDATE;");
    switch (_updateState)
    {
            case State::UNCHECKED:
                {
                    _updateState = State::PREDOWNLOAD_VERSION;
                    AXLOGD("AssetsManagerEx::update _updateState PREDOWNLOAD_VERSION "); 
                }
            case State::PREDOWNLOAD_VERSION:
                {
                    AXLOGD("AssetsManagerEx::update _updateState downloadVersion "); 
                    downloadVersion();                
                }
            break;
            case State::VERSION_LOADED:
                {
                    AXLOGD("AssetsManagerEx::update _updateState parseVersion "); 
                    parseVersion();
                    
                }
            break;
            case State::PREDOWNLOAD_MANIFEST:
                {
                    AXLOGD("AssetsManagerEx::update _updateState downloadManifest "); 
                    downloadManifest();
                }
            break;
            case State::MANIFEST_LOADED:
                {
                    AXLOGD("AssetsManagerEx::update _updateState parseManifest "); 
                    parseManifest();
                }
            break;
            case State::FAIL_TO_UPDATE:
            case State::NEED_UPDATE:
                {
                    // Manifest not loaded yet
                    if (!_remoteManifest->isLoaded())
                    {
                        _updateState = State::PREDOWNLOAD_MANIFEST;
                        AXLOGD("AssetsManagerEx::update _updateState PREDOWNLOAD_MANIFEST downloadManifest "); 
                        downloadManifest();
                    }
                    else
                    {
                        AXLOGD("AssetsManagerEx::update _updateState startUpdate "); 
                        startUpdate();
                    }
                }
            break;
            case State::UP_TO_DATE:
            case State::UPDATING:
            case State::UNZIPPING:
                AXLOGD("AssetsManagerEx::update _updateState NONE "); 
                _updateEntry = UpdateEntry::NONE;
                break;
            default:
                break;
    }
}

void AssetsManagerEx::updateAssets(const DownloadUnits& assets)
{
    if (!_inited)
    {
        AXLOGD("AssetsManagerEx : Manifests uninited.\n");
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST);
        return;
    }

    if (_updateState != State::UPDATING && _localManifest->isLoaded() && _remoteManifest->isLoaded())
    {
        _updateState = State::UPDATING;
        _downloadUnits.clear();
        _downloadedSize.clear();
        _percent = _percentByFile = _sizeCollected = _totalSize = 0;
        _totalWaitToDownload = _totalToDownload = (int)assets.size();
        _nextSavePoint                          = 0;
        _totalEnabled                           = false;
        if (_totalToDownload > 0)
        {
            _downloadUnits = assets;
            this->batchDownload();
        }
        else if (_totalToDownload == 0)
        {
            onDownloadUnitsFinished();
        }
    }
}

const DownloadUnits& AssetsManagerEx::getFailedAssets() const
{
    return _failedUnits;
}

void AssetsManagerEx::downloadFailedAssets()
{
    AXLOGD("AssetsManagerEx : Start update {} failed assets.\n", static_cast<unsigned long>(_failedUnits.size()));
    updateAssets(_failedUnits);
}

void AssetsManagerEx::fileError(std::string_view identifier,
                                std::string_view errorStr,
                                int errorCode,
                                int errorCodeInternal)
{
    AXLOGD("fileError: identifier:{}  errorStr:{}  errorCode:{} errorCodeInternal:{}",identifier,errorStr,errorStr,errorCode,errorCodeInternal);
    auto unitIt = _downloadUnits.find(identifier);
    // Found unit and add it to failed units
    if (unitIt != _downloadUnits.end())
    {
        _totalWaitToDownload--;

        DownloadUnit unit = unitIt->second;
        _failedUnits.emplace(unit.customId, unit);
    }
    dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_UPDATING, identifier, errorStr, errorCode,
                        errorCodeInternal);
    AXLOGD("fileError: key:{} DownloadState::UNSTARTED",identifier);
    _tempManifest->setAssetDownloadState(identifier, Manifest::DownloadState::UNSTARTED);

    _currConcurrentTask = MAX(0, _currConcurrentTask - 1);
    queueDowload();
}

void AssetsManagerEx::fileSuccess(std::string_view customId, std::string_view storagePath)
{
    // Set download state to SUCCESSED
    AXLOGD("fileSuccess: setAssetDownloadState customId:{}  storagePath:{} SUCCESSED",customId,storagePath);
    _tempManifest->setAssetDownloadState(customId, Manifest::DownloadState::SUCCESSED);

    auto unitIt = _failedUnits.find(customId);
    // Found unit and delete it
    if (unitIt != _failedUnits.end())
    {
        // Remove from failed units list
        _failedUnits.erase(unitIt);
    }

    unitIt = _downloadUnits.find(customId);
    if (unitIt != _downloadUnits.end())
    {
        // Reduce count only when unit found in _downloadUnits
        _totalWaitToDownload--;

        _percent = _percentByFile = 100 * (float)(_totalToDownload - _totalWaitToDownload) / _totalToDownload;
        // Notify progression event
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, "");
    }
    // Notify asset updated event
    dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ASSET_UPDATED, customId);

    _currConcurrentTask = MAX(0, _currConcurrentTask - 1);
    queueDowload();
}

void AssetsManagerEx::onError(const network::DownloadTask& task,
                              int errorCode,
                              int errorCodeInternal,
                              std::string_view errorStr)
{
    // Skip version error occurred
    AXLOGD("onError: errorCode:{}  errorCodeInternal:{}  errorStr:{}",errorCode,errorCodeInternal,errorStr);
    if (task.identifier == VERSION_ID)
    {
        AXLOGD("AssetsManagerEx : Fail to download version file, step skipped\n");
        _updateState = State::PREDOWNLOAD_MANIFEST;
        downloadManifest();
    }
    else if (task.identifier == MANIFEST_ID)
    {
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST, task.identifier, errorStr,
                            errorCode, errorCodeInternal);
        _updateState = State::FAIL_TO_UPDATE;
    }
    else
    {
        fileError(task.identifier, errorStr, errorCode, errorCodeInternal);
    }
}

void AssetsManagerEx::onProgress(double total, double downloaded, std::string_view url, std::string_view customId)
{
    AXLOGD("onProgress: customId:{}  url:{}",customId,url);
    if (customId == VERSION_ID || customId == MANIFEST_ID)
    {
        _percent = 100 * downloaded / total;
        // Notify progression event
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, customId);
        return;
    }
    else
    {
        // Calcul total downloaded
        bool found             = false;
        double totalDownloaded = 0;
        for (auto it = _downloadedSize.begin(); it != _downloadedSize.end(); ++it)
        {
            if (it->first == customId)
            {
                it->second = downloaded;
                found      = true;
            }
            totalDownloaded += it->second;
        }
        // Collect information if not registed
        if (!found)
        {
            // Set download state to DOWNLOADING, this will run only once in the download process
            AXLOGD("onProgress: customId:{}  url:{}  DownloadState::DOWNLOADING",customId,url);
            _tempManifest->setAssetDownloadState(customId, Manifest::DownloadState::DOWNLOADING);
            // Register the download size information
            _downloadedSize.emplace(customId, downloaded);
            // Check download unit size existance, if not exist collect size in total size
            if (_downloadUnits[customId].size == 0)
            {
                _totalSize += total;
                _sizeCollected++;
                _totalDiffFileSize=_totalDiffFileSize+total;
                // All collected, enable total size
                if (_sizeCollected == _totalToDownload)
                {
                    _totalEnabled = true;
                }
            }
        }

        if (_totalEnabled && _updateState == State::UPDATING)
        {
            float currentPercent = 100 * totalDownloaded / _totalSize;
            // Notify at integer level change
            if ((int)currentPercent != (int)_percent)
            {
                _percent = currentPercent;
                // Notify progression event
                dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION, customId);
            }
        }
    }
}

void AssetsManagerEx::onSuccess(std::string_view srcUrl, std::string_view storagePath, std::string_view customId)
{
   AXLOGD("onSuccess: srcUrl:{}  storagePath:{}  customId:{}",srcUrl,storagePath,customId);
    if (customId == VERSION_ID)
    {
        _updateState = State::VERSION_LOADED;
        parseVersion();
    }
    else if (customId == MANIFEST_ID)
    {
        _updateState = State::MANIFEST_LOADED;
        parseManifest();
    }
    else
    {
        bool ok      = true;
        auto& assets = _remoteManifest->getAssets();
        auto assetIt = assets.find(customId);
        if (assetIt != assets.end())
        {
            Manifest::Asset asset = assetIt->second;
            if (_verifyCallback != nullptr)
            {
                ok = _verifyCallback(storagePath, asset);
            }
        }

        if (ok)
        {
            bool compressed = assetIt != assets.end() ? assetIt->second.compressed : false;
            if (compressed)
            {
                decompressDownloadedZip(customId, storagePath);
            }
            else
            {
                fileSuccess(customId, storagePath);
            }
        }
        else
        {
            fileError(customId, "Asset file verification failed after downloaded");
        }
    }
}

void AssetsManagerEx::destroyDownloadedVersion()
{
    _fileUtils->removeDirectory(_storagePath);
    _fileUtils->removeDirectory(_tempStoragePath);
}

void AssetsManagerEx::batchDownload()
{
    _queue.clear();
    _totalDiffFileSize=0;
    for (const auto& iter : _downloadUnits)
    {
        const DownloadUnit& unit = iter.second;
        if (unit.size > 0)
        {
            _totalSize += unit.size;
            _sizeCollected++;
            _totalDiffFileSize=_totalDiffFileSize+ unit.size;   
        }

        _queue.emplace_back(iter.first);
    }
    // All collected, enable total size
    if (_sizeCollected == _totalToDownload)
    {
        _totalEnabled = true;
    }

    queueDowload();
}

void AssetsManagerEx::queueDowload()
{
    AXLOGD("queueDowload _totalWaitToDownload:{}",_totalWaitToDownload);
    if (_totalWaitToDownload == 0)
    {
        this->onDownloadUnitsFinished();
        return;
    }
    AXLOGD("queueDowload _InterruptFlag:{}",_InterruptFlag);
    if (!_InterruptFlag) {
        AXLOGD("queueDowload _currConcurrentTask:{} _maxConcurrentTask:{}",_currConcurrentTask,_maxConcurrentTask);
        while (_currConcurrentTask < _maxConcurrentTask && !_queue.empty())
        {
            std::string key = _queue.back();
            _queue.pop_back();

            _currConcurrentTask++;
            DownloadUnit& unit = _downloadUnits[key];
            _fileUtils->createDirectory(basename(unit.storagePath));
            _downloader->createDownloadFileTask(unit.srcUrl, unit.storagePath, unit.customId);
            AXLOGD("onProgress: key:{} DownloadState::DOWNLOADING",key);
            _tempManifest->setAssetDownloadState(key, Manifest::DownloadState::DOWNLOADING);
        }
        if (_percentByFile / 100 > _nextSavePoint)
        {
            // Save current download manifest information for resuming
            _tempManifest->saveToFile(_tempManifestPath);
            _nextSavePoint += SAVE_POINT_INTERVAL;
        }
    }

}

void AssetsManagerEx::onDownloadUnitsFinished()
{
    // Finished with error check
    if (!_failedUnits.empty())
    {
        // Save current download manifest information for resuming
        _tempManifest->saveToFile(_tempManifestPath);

        _updateState = State::FAIL_TO_UPDATE;
        dispatchUpdateEvent(EventAssetsManagerEx::EventCode::UPDATE_FAILED);
    }
    else if (_updateState == State::UPDATING)
    {
        updateSucceed();
    }
}

void AssetsManagerEx::fillZipFunctionOverrides(zlib_filefunc_def_s& zipFunctionOverrides)
{
    zipFunctionOverrides.zopen_file     = AssetManagerEx_open_file_func;
    zipFunctionOverrides.zopendisk_file = AssetManagerEx_opendisk_file_func;
    zipFunctionOverrides.zread_file     = AssetManagerEx_read_file_func;
    zipFunctionOverrides.zwrite_file    = AssetManagerEx_write_file_func;
    zipFunctionOverrides.ztell_file     = AssetManagerEx_tell_file_func;
    zipFunctionOverrides.zseek_file     = AssetManagerEx_seek_file_func;
    zipFunctionOverrides.zclose_file    = AssetManagerEx_close_file_func;
    zipFunctionOverrides.zerror_file    = AssetManagerEx_error_file_func;
    zipFunctionOverrides.opaque         = nullptr;
}

NS_AX_EXT_END
