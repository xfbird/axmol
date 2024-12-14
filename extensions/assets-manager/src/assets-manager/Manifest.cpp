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

#include "Manifest.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <fstream>
#include <stdio.h>

#define KEY_VERSION "version"
#define KEY_PACKAGE_URL "packageUrl"
#define KEY_MANIFEST_URL "remoteManifestUrl"
#define KEY_VERSION_URL "remoteVersionUrl"
#define KEY_GROUP_VERSIONS "groupVersions"
#define KEY_ENGINE_VERSION "engineVersion"
#define KEY_ASSETS "assets"
#define KEY_COMPRESSED_FILES "compressedFiles"
#define KEY_SEARCH_PATHS "searchPaths"

#define KEY_PATH "path"
#define KEY_MD5 "md5"
#define KEY_GROUP "group"
#define KEY_COMPRESSED "compressed"
#define KEY_SIZE "size"
#define KEY_COMPRESSED_FILE "compressedFile"
#define KEY_DOWNLOAD_STATE "downloadState"

NS_AX_EXT_BEGIN

static int cmpVersion(std::string_view v1, std::string_view v2)
{
    int i;
    int oct_v1[4] = {0}, oct_v2[4] = {0};
    int filled1 = std::sscanf(v1.data(), "%d.%d.%d.%d", &oct_v1[0], &oct_v1[1], &oct_v1[2], &oct_v1[3]);
    int filled2 = std::sscanf(v2.data(), "%d.%d.%d.%d", &oct_v2[0], &oct_v2[1], &oct_v2[2], &oct_v2[3]);

    if (filled1 == 0 || filled2 == 0)
    {
        return v1 != v2;  // strcmp(v1.data(), v2.data());
    }
    for (i = 0; i < 4; i++)
    {
        if (oct_v1[i] > oct_v2[i])
            return 1;
        else if (oct_v1[i] < oct_v2[i])
            return -1;
    }
    return 0;
}

Manifest::Manifest(std::string_view manifestUrl /* = ""*/)
    : _versionLoaded(false)
    , _loaded(false)
    , _manifestRoot("")
    , _packageUrl("")
    , _remoteManifestUrl("")
    , _remoteVersionUrl("")
    , _version("")
    , _engineVer("")
{
    // Init variables
    _fileUtils = FileUtils::getInstance();
    if (!manifestUrl.empty())
        parse(manifestUrl);                 //如果本地文件存在 ，就清除所有的 并将文件读入，放入_json
}

// Manifest(std::string_view manifestUrl = "") 
//         : _versionLoaded(false)
//         , _loaded(false)
//         , _fileUtils(nullptr)
//         , _manifestRoot("")
//         , _packageUrl("")
//         , _remoteManifestUrl("")
//         , _remoteVersionUrl("")
//         , _version("")
//         , _groups()
//         , _groupVer()
//         , _engineVer()
//         , _assets()
//         , _searchPaths()
//         , _json() 
//     {
//         if (!manifestUrl.empty()) {
//             parse(manifestUrl);
//         }
//     }
// NS_AX_EXT_END


void Manifest::loadJson(std::string_view url)
{
    clear();
    std::string content;
    if (_fileUtils->isFileExist(url))
    {
        // Load file content
        content = _fileUtils->getStringFromFile(url);

        if (content.empty())
        {
            AXLOGW("Fail to retrieve local file content: {}\n", url);
        }
        else
        {
            // Parse file with rapid json
            // AXLOGS("Manifest::loadJson LoadFile:{} Content {}",url,content);
            _json.Parse<0>(content.c_str());
            // Print error
            if (_json.HasParseError())
            {
                size_t offset = _json.GetErrorOffset();
                if (offset > 0)
                    offset--;
                std::string errorSnippet = content.substr(offset, 10);
                AXLOGS("File parse error {} at <{}>\n",  static_cast<int>(_json.GetParseError()), errorSnippet);
            }
        }
    }
}

void Manifest::parseVersion(std::string_view versionUrl)
{
    AXLOGS("parseVersion loadJson versionUrl:{}",versionUrl);
    loadJson(versionUrl);

    if (_json.IsObject())
    {
        AXLOGS("parseVersion loadVersion");
        loadVersion(_json);
    }
}

void Manifest::parse(std::string_view manifestUrl)
{
    AXLOGS("parse  manifestUrl:{}",manifestUrl);
    loadJson(manifestUrl);                  //文件存在 读出 并用json 解析。

    if (!_json.HasParseError() && _json.IsObject())         //没有错误
    {
        // Register the local manifest root
        size_t found = manifestUrl.find_last_of("/\\");
        if (found != std::string::npos)
        {
            _manifestRoot = manifestUrl.substr(0, found + 1);
            AXLOGS("parse _manifestRoot:{}",_manifestRoot);
        }
        AXLOGS("loadManifest json");                //取出路径部分
        loadManifest(_json);                        //装载 json  数据装入内存，并 建立装载标志
    }
}

bool Manifest::isVersionLoaded() const
{
    AXLOGS("isVersionLoaded:{}",_versionLoaded);
    return _versionLoaded;
}
bool Manifest::isLoaded() const
{
    AXLOGS("_loaded:{}",_loaded);
    return _loaded;
}

bool Manifest::versionEquals(const Manifest* b) const
{
    // Check manifest version
    AXLOGS("Check _version:{} vs Call B__version:{}",
                   _version,b->getVersion());
    if (_version != b->getVersion())
    {
        AXLOGS("versionEquals:false");
        return false;
    }
    // Check group versions
    else
    {
        std::vector<std::string> bGroups = b->getGroups();
        auto& bGroupVer                  = b->getGroupVerions();
        // Check group size
        AXLOGS("Check b->getGroups() size:{} vs _groups.size():{}",
                    bGroups.size(),_groups.size());
        if (bGroups.size() != _groups.size())
        {
            AXLOGS("versionEquals size no:false");
            return false;
        }
         

        // Check groups version
        for (unsigned int i = 0; i < _groups.size(); ++i)
        {
            std::string gid = _groups[i];
            // Check group name
            if (gid != bGroups[i])
            {
                
                AXLOGS("versionEquals gid no :false");
                return false;
            }   
            // Check group version
            if (_groupVer.at(gid) != bGroupVer.at(gid))
            {
                AXLOGS("versionEquals at gid no :false");
                return false;
            }   
        }
    }
    // AXLOGS("versionEquals:{}",true);
    AXLOGS("versionEquals:true");
    return true;
}

bool Manifest::versionGreater(
    const Manifest* b,
    const std::function<int(std::string_view versionA, std::string_view versionB)>& handle) const
{
    std::string_view localVersion = getVersion();
    std::string_view bVersion     = b->getVersion();
    AXLOGS("versionGreater localVersion:{}  cmpv:{}",localVersion,bVersion);    
    bool greater;
    if (handle)
    {
        greater = handle(localVersion, bVersion) > 0;
    }
    else
    {
        greater = cmpVersion(localVersion, bVersion) > 0;
    }

    AXLOGS("versionGreater return:{}",greater);    
    return greater;
}

hlookup::string_map<Manifest::AssetDiff> Manifest::genDiff(const Manifest* b) const
{
   
    AXLOGS("genDiff self _remoteManifestUrl:{}",_remoteManifestUrl);
    AXLOGS("genDiff b _remoteManifestUrl:{}",b->getManifestFileUrl());
    hlookup::string_map<AssetDiff> diff_map;
    auto& bAssets = b->getAssets();
    
    std::string key;
    Asset valueA;
    Asset valueB;

    hlookup::string_map<Asset>::const_iterator valueIt, it;
    for (it = _assets.begin(); it != _assets.end(); ++it)
    {
        key    = it->first;
        valueA = it->second;

        // AXLOGS("genDiff  Check key:{} have ValueB ",key);
        // Deleted
        valueIt = bAssets.find(key);
        if (valueIt == bAssets.cend())
        {
            AXLOGS("genDiff  self.Key:{} not in b need Delete Add To Diff",key);
            AssetDiff diff;
            diff.asset = valueA;
            diff.type  = DiffType::DELETED;
            diff_map.emplace(key, diff);
            // AXLOGS("genDiff DiffType::DELETED  diff_map.emplace(key{}, diff) ",key);
            continue;
        }

        // Modified
        valueB = valueIt->second;
        // AXLOGS("genDiff  self.Key:{}   self.md5:{}  b:md5:{}",key,valueA.md5,valueB.md5);
        if (valueA.md5 != valueB.md5)
        {
            AXLOGS("genDiff  self.Key:{} in b and self.md5 != b.md5 need Modify Add To Diff",key);
            AssetDiff diff;
            diff.asset = valueB;
            diff.type  = DiffType::MODIFIED;
            diff_map.emplace(key, diff);
            // AXLOGS("genDiff DiffType::MODIFIED  diff_map.emplace(key{}, diff) ",key);
        }
    }

    for (it = bAssets.begin(); it != bAssets.end(); ++it)
    {
        key    = it->first;
        valueB = it->second;

        // AXLOGS("genDiff  CheckB key:{} have ValueB ",key);
        // Added
        valueIt = _assets.find(key);
        if (valueIt == _assets.cend())
        {
            AXLOGS("genDiff  b.Key:{} not in self  need AddEd Add To Diff",key);
            AssetDiff diff;
            diff.asset = valueB;
            diff.type  = DiffType::ADDED;
            diff_map.emplace(key, diff);
            // AXLOGS("genDiff DiffType::ADDED  diff_map.emplace(key{}, diff) ",key);
        }
    }

    return diff_map;
}

void Manifest::genResumeAssetsList(DownloadUnits* units) const
{
    AXLOGS("genResumeAssetsList ");    
    for (auto it = _assets.begin(); it != _assets.end(); ++it)
    {
        Asset asset = it->second;
        AXLOGS("genResumeAssetsList  asset.path:{},asset.size:{} asset.downloadState:{}",asset.path,asset.size,int(asset.downloadState));    
        if (asset.downloadState != DownloadState::SUCCESSED && asset.downloadState != DownloadState::UNMARKED)
        {
            DownloadUnit unit;
            unit.customId    = it->first;
            unit.srcUrl      = _packageUrl + asset.path;
            unit.storagePath = _manifestRoot + asset.path;
            unit.size        = asset.size;
            units->emplace(unit.customId, unit);
            AXLOGS("genResumeAssetsList unit customId:{} srcUrl:{} storagePath:{} size:{}",unit.customId,unit.srcUrl,unit.storagePath,unit.size);    
        }
    }
}

std::vector<std::string> Manifest::getSearchPaths() const
{
    std::vector<std::string> searchPaths;
    searchPaths.emplace_back(_manifestRoot);

    for (int i = (int)_searchPaths.size() - 1; i >= 0; i--)
    {
        std::string path = _searchPaths[i];
        if (!path.empty() && path[path.size() - 1] != '/')
            path.push_back('/');
        path = _manifestRoot + path;
        searchPaths.emplace_back(path);
    }
    return searchPaths;
}

void Manifest::prependSearchPaths()
{
    std::vector<std::string> searchPaths    = FileUtils::getInstance()->getSearchPaths();
    std::vector<std::string>::iterator iter = searchPaths.begin();
    bool needChangeSearchPaths              = false;
    if (std::find(searchPaths.begin(), searchPaths.end(), _manifestRoot) == searchPaths.end())
    {
        searchPaths.insert(iter, _manifestRoot);
        AXLOGS("Insert _manifestRoot:{}",_manifestRoot);
        needChangeSearchPaths = true;
    }

    for (int i = (int)_searchPaths.size() - 1; i >= 0; i--)
    {
        std::string path = _searchPaths[i];
        if (!path.empty() && path[path.size() - 1] != '/')
            path.push_back('/');
        path = _manifestRoot + path;
        iter = searchPaths.begin();
        searchPaths.insert(iter, path);
        AXLOGS("searchPaths.insert path:{}",path);
        needChangeSearchPaths = true;
    }
    if (needChangeSearchPaths)
    {
        FileUtils::getInstance()->setSearchPaths(searchPaths);
    }
}

std::string_view Manifest::getPackageUrl() const
{
    AXLOGS("_packageUrl :{}",_packageUrl);
    return _packageUrl;
}

std::string_view Manifest::getManifestFileUrl() const
{
    AXLOGS("_remoteManifestUrl :{}",_remoteManifestUrl);
    return _remoteManifestUrl;
}

std::string_view Manifest::getVersionFileUrl() const
{
    AXLOGS("Manifest::getVersionFileUrl :{}",_remoteVersionUrl);
    return _remoteVersionUrl;
}

std::string_view Manifest::getVersion() const
{
    AXLOGS("getVersion _version :{}",_version);
    return _version;
}

const std::vector<std::string>& Manifest::getGroups() const
{
    // AXLOGS("_groups :{}",_groups);
    return _groups;
}

const hlookup::string_map<std::string>& Manifest::getGroupVerions() const
{
    // AXLOGS("_groups :{}",_groups);
    return _groupVer;
}

std::string_view Manifest::getGroupVersion(std::string_view group) const
{
    return _groupVer.at(group);
}

const hlookup::string_map<Manifest::Asset>& Manifest::getAssets() const
{
    return _assets;
}

void Manifest::setAssetDownloadState(std::string_view key, const Manifest::DownloadState& state)
{
    auto valueIt = _assets.find(key);
    AXLOGS("setAssetDownloadState set:{}  state:{}",key,(int)state);
    if (valueIt != _assets.end())
    {
        valueIt->second.downloadState = state;

        // Update json object
        if (_json.IsObject())
        {
            if (_json.HasMember(KEY_ASSETS))
            {
                rapidjson::Value& assets = _json[KEY_ASSETS];
                if (assets.IsObject())
                {
                    if (assets.HasMember(key.data()))
                    {
                        rapidjson::Value& entry = assets[key.data()];
                        if (entry.HasMember(KEY_DOWNLOAD_STATE) && entry[KEY_DOWNLOAD_STATE].IsInt())
                        {
                            entry[KEY_DOWNLOAD_STATE].SetInt((int)state);
                        }
                        else
                        {
                            entry.AddMember<int>(KEY_DOWNLOAD_STATE, (int)state, _json.GetAllocator());
                        }
                    }
                }
            }
        }
    }
}

void Manifest::clear()
{
    if (_versionLoaded || _loaded)
    {
        _groups.clear();
        _groupVer.clear();

        _remoteManifestUrl = "";
        _remoteVersionUrl  = "";
        _version           = "";
        _engineVer         = "";

        _versionLoaded = false;
    }

    if (_loaded)
    {
        _assets.clear();
        _searchPaths.clear();
        _loaded = false;
    }
}

Manifest::Asset Manifest::parseAsset(std::string_view path, const rapidjson::Value& json)
{
    Asset asset;
    asset.path = path;

    if (json.HasMember(KEY_MD5) && json[KEY_MD5].IsString())
    {
        asset.md5 = json[KEY_MD5].GetString();
    }
    else
        asset.md5 = "";

    if (json.HasMember(KEY_PATH) && json[KEY_PATH].IsString())
    {
        asset.path = json[KEY_PATH].GetString();
    }

    if (json.HasMember(KEY_COMPRESSED) && json[KEY_COMPRESSED].IsBool())
    {
        asset.compressed = json[KEY_COMPRESSED].GetBool();
    }
    else
        asset.compressed = false;

    if (json.HasMember(KEY_SIZE) && json[KEY_SIZE].IsInt())
    {
        asset.size = json[KEY_SIZE].GetInt();
    }
    else
        asset.size = 0;

    if (json.HasMember(KEY_DOWNLOAD_STATE) && json[KEY_DOWNLOAD_STATE].IsInt())
    {
        asset.downloadState = (json[KEY_DOWNLOAD_STATE].GetInt());
    }
    else
        asset.downloadState = DownloadState::UNMARKED;

    return asset;
}

void Manifest::loadVersion(const rapidjson::Document& json)
{
    // Retrieve remote manifest url
    // AXLOGS("loadVersion json:{}",json.GetString());
    //对于 json 取出 相关的 信息，如果存在的化。如果存在 就认为 版本信息已经加载

    // #define KEY_MANIFEST_URL "remoteManifestUrl"             远程url
    if (json.HasMember(KEY_MANIFEST_URL) && json[KEY_MANIFEST_URL].IsString())
    {
        _remoteManifestUrl = json[KEY_MANIFEST_URL].GetString();
        AXLOGS("loadVersion read:{} set _remoteManifestUrl:{}",KEY_MANIFEST_URL,_remoteManifestUrl);
    }

    // Retrieve remote version url
    // #define KEY_VERSION_URL "remoteVersionUrl"               远程版本url
    if (json.HasMember(KEY_VERSION_URL) && json[KEY_VERSION_URL].IsString())
    {
        _remoteVersionUrl = json[KEY_VERSION_URL].GetString();
        AXLOGS("loadVersion read:{} set _remoteVersionUrl:{}",KEY_VERSION_URL,_remoteVersionUrl);
    }

    // Retrieve local version
    // #define KEY_VERSION "version"                            版本的key
    if (json.HasMember(KEY_VERSION) && json[KEY_VERSION].IsString())
    {
        _version = json[KEY_VERSION].GetString();
        AXLOGS("loadVersion read:{} set _version:{}",KEY_VERSION,_version);
    }

    // Retrieve local group version
    // #define KEY_GROUP_VERSIONS "groupVersions"               组版本
    if (json.HasMember(KEY_GROUP_VERSIONS))
    {
        const rapidjson::Value& groupVers = json[KEY_GROUP_VERSIONS];
        if (groupVers.IsObject())
        {
            for (rapidjson::Value::ConstMemberIterator itr = groupVers.MemberBegin(); itr != groupVers.MemberEnd();
                 ++itr)
            {
                std::string group   = itr->name.GetString();
                std::string version = "0";
                if (itr->value.IsString())
                {
                    version = itr->value.GetString();
                }
                _groups.emplace_back(group);
                _groupVer.emplace(group, version);
            }
        }
    }

    // Retrieve local engine version
    // #define KEY_ENGINE_VERSION "engineVersion"               引擎版本
    if (json.HasMember(KEY_ENGINE_VERSION) && json[KEY_ENGINE_VERSION].IsString())
    {
        _engineVer = json[KEY_ENGINE_VERSION].GetString();
        AXLOGS("loadVersion read set _engineVer:{}",KEY_ENGINE_VERSION,_engineVer);
    }

    _versionLoaded = true;
}

void Manifest::loadManifest(const rapidjson::Document& json)
{
    AXLOGS("loadVersion json");                     //基于 主文件 加载 版本信息
    loadVersion(json);

    // Retrieve package url
    // #define KEY_PACKAGE_URL "packageUrl"
    if (json.HasMember(KEY_PACKAGE_URL) && json[KEY_PACKAGE_URL].IsString())
    {
        _packageUrl = json[KEY_PACKAGE_URL].GetString();
        // Append automatically "/"
        if (!_packageUrl.empty() && _packageUrl[_packageUrl.size() - 1] != '/')
        {
            _packageUrl.push_back('/');             //包Url 
        }
    }

    // Retrieve all assets
    // #define KEY_ASSETS "assets"
    if (json.HasMember(KEY_ASSETS))
    {
        const rapidjson::Value& assets = json[KEY_ASSETS];
        if (assets.IsObject())
        {
            for (rapidjson::Value::ConstMemberIterator itr = assets.MemberBegin(); itr != assets.MemberEnd(); ++itr)
            {
                std::string key = itr->name.GetString();
                Asset asset     = parseAsset(key, itr->value);
                _assets.emplace(key, asset);         //加入所有的 assets 成员
            }
        }
    }

    // Retrieve all search paths
    // #define KEY_SEARCH_PATHS "searchPaths"  搜索路径
    if (json.HasMember(KEY_SEARCH_PATHS))
    {
        const rapidjson::Value& paths = json[KEY_SEARCH_PATHS];
        if (paths.IsArray())
        {
            for (rapidjson::SizeType i = 0; i < paths.Size(); ++i)
            {
                if (paths[i].IsString())
                {
                    _searchPaths.emplace_back(paths[i].GetString());
                }
            }
        }
    }

    //装载 完成 
    _loaded = true;
}

void Manifest::saveToFile(std::string_view filepath)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    _json.Accept(writer);

    FileUtils::getInstance()->writeStringToFile(buffer.GetString(), filepath);
}

NS_AX_EXT_END
