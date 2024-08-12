#include "base/mTileMapManager.h"
#include "platform/FileUtils.h"
#include <sstream> // 包含 std::istringstream 所需的头文件
// #include "base/mFileUtils.h" // 确保包含了 FileUtils 头文件

NS_AX_BEGIN

int TMapHeader::readHeader(std::istream& file) {
    file.read(reinterpret_cast<char*>(&wWidth), sizeof(wWidth));
    file.read(reinterpret_cast<char*>(&wHeight), sizeof(wHeight));
    file.read(reinterpret_cast<char*>(sTitle.data()), sTitle.size());
    file.read(reinterpret_cast<char*>(&UpdateDate), sizeof(UpdateDate));
    file.read(reinterpret_cast<char*>(&btVersion), sizeof(btVersion));
    file.read(reinterpret_cast<char*>(Reserved.data()), Reserved.size());

    int retversion = 1; // 默认版本号
    if (!(btVersion != 13) && (sTitle[14] == 13) && (sTitle[15] == 10)) {
        retversion = 3;
    } else if (static_cast<unsigned char>(btVersion) == 6) {
        retversion = 2;
    } else {
        retversion = 1;
    }
    return retversion;
}
TileMapManager* TileMapManager::create(){
    // TileMapManager* obj = new (std::nothrow) TileMapManager();
    // if (obj) {
    //     obj->autorelease();
    //     AXLOGD("TileMapManager::create obj:{}",fmt::ptr(obj));
    //     return obj;
    // }
    // AX_SAFE_RELEASE(obj);
    // AXLOGD("TileMapManager::create obj fail return nullptr");
    // return nullptr;
    TileMapManager* obj = new TileMapManager();
    obj->autorelease();
    return obj;    
}

bool TileMapManager::loadMapFile(const std::string& filename) {
    Data data = FileUtils::getInstance()->getDataFromFile(filename);
    if (data.isNull()) {
        // std::cerr << "Failed to read file: " << filename << std::endl;
        AXLOGW("loadMapFile 错误读入文件:{}", filename);
        return false;
    }

    // 从 Data 对象创建一个 std::string
    std::string fileContent(reinterpret_cast<const char*>(data.getBytes()), data.getSize());

    // 从 std::string 创建一个内存流
    std::istringstream file(fileContent);

    // 现在你可以使用 fileStream 来读取数据
    TMapHeader header;
    _mapDataType = header.readHeader(file);
    _Width=header.getWidth();
    _Height=header.getHeight();
    AXLOGD("loadMapFile _Width:{}  _Height:{}",_Width,_Height);
    //std::cout << "_mapDataType is: " << _mapDataType << std::endl;
    if (_mapDataType < 1 || _mapDataType > 3) {
        // std::cerr << "Unsupported map data type: " << _mapDataType << std::endl;
        AXLOGW("loadMapFile 不支持的文件版本类型:{}", _mapDataType);
        // file.close();
        return false;
    }
    const size_t numTiles = _Width * _Height;
    _mapInfoArray.resize(numTiles);
    //按顺序读入     
    for (int x = 0; x < _Width; ++x) {
        for (int y = 0; y < _Height; ++y) {
            size_t i = x * _Height + y;
            auto info = std::make_unique<TGMapInfo>();
            info->readInfo(file,_mapDataType);
            _mapInfoArray[i] = std::move(info);
        }
    }
    // file.close();
    return true;
}
// 在类实现中添加 GetTileInfo 函数定义
TGMapInfo* TileMapManager::GetTileInfo(size_t x, size_t y) {
    if (x >= _Width || y >= _Height) {
        // throw std::out_of_range("Tile coordinates are out of the map boundaries.");
        AXLOGW("GetTileInfo 在:[{},{}] 参数超界 {},{} ",x,y,_Width,_Height);
        return nullptr;
    }
    // 计算索引
    
    size_t i = x * _Height + y;
    // AXLOGD("GetTileInfo 在:[{},{}] 计算位置{}",x,y,i);
    // 检查索引是否有效
    if (i >= _mapInfoArray.size()) {
        // 如果索引超出范围，返回 nullptr 或者抛出异常
        // return nullptr; // 或者抛出 
        std::out_of_range("Index out of range");
        AXLOGW("GetTileInfo 在:[{},{}] 计算出位置:{} 超界 {}",x,y,i,_mapInfoArray.size());
        return nullptr;
    }
    // 返回对应的 TGMapInfo 指针

    return _mapInfoArray[i].get();
}

// TileMapManager 类的实现部分，继续之前的代码
// 新增的公开方法
uint16_t TileMapManager::GetBkImgIdx(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    // AXLOGD("GetTileInfo 在:[{},{}] 得到tileInfo:{}",x,y,fmt::ptr(tileInfo));
    // AXLOGD("lua_ax_base_TileMapManager_new tilemm:{}",fmt::ptr(tilemm));
    if (tileInfo) {
        return tileInfo->getwBkImg();
    } 
    // throw std::runtime_error("Failed to get tile info.");
    AXLOGW("GetBkImgIdx 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;
}

uint16_t TileMapManager::GetMidImgIdx(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    // AXLOGD("GetTileInfo 在:[{},{}] 得到tileInfo:{}",x,y,fmt::ptr(tileInfo));
    if (tileInfo) {
        return tileInfo->getwMidImg();
    }
    // throw std::runtime_error("Failed to get tile info.");
    AXLOGW("GetMidImgIdx 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;
}

uint16_t TileMapManager::GetFrImgIdx(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    // AXLOGD("GetTileInfo 在:[{},{}] 得到tileInfo:{}",x,y,fmt::ptr(tileInfo));
    if (tileInfo) {
        return tileInfo->getwFrImg();
    } 
    //    throw std::runtime_error("Failed to get tile info.");
    AXLOGW("GetFrImgIdx 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint16_t TileMapManager::GetAniImgIdx(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    // AXLOGD("GetTileInfo 在:[{},{}] 得到tileInfo:{}",x,y,fmt::ptr(tileInfo));
    if (tileInfo) {
        return tileInfo->getAniImg();
    } 
    // else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetAniImgIdx 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

// TileMapManager 类的实现部分，继续之前的代码

// 新增的公开方法
uint8_t TileMapManager::GetAniFrame(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getAniFrame();
    }
    //  else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetAniImgIdx 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetAniTick(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getAniTick();
    } 
    // else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetAniTick 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    

}

uint8_t TileMapManager::GetDoorIndex(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getDoorIndex();
    } 
    // else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetDoorIndex 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetDoorOffset(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getDoorOffset();
    }
    //  else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetDoorOffset 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetTilesArea(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getUnitBkImg();
    } 
    // else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetTilesArea 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetSmTilesArea(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getUnitMidImg();
    }
    //  else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetSmTilesArea 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetObjArea(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getUnitFrImg();
    } 
    // else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetObjArea 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}

uint8_t TileMapManager::GetLight(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->getLight();
    }
    //  else {
    //     throw std::runtime_error("Failed to get tile info.");
    // }
    AXLOGW("GetLight 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return 0;    
}


bool TileMapManager::isObstacle(size_t x, size_t y) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        return tileInfo->isObstacle();
    }
    AXLOGW("isObstacle 在:[{},{}] 无法获得 TGMapInfo",x,y);
    return false;
}

// 设置指定坐标的障碍物状态
void TileMapManager::setObstacle(size_t x, size_t y, bool bObstacle) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        tileInfo->setObstacle(bObstacle);
    } 
    // else {
    //     throw std::out_of_range("Tile coordinates are out of the map boundaries.");
    // }
    AXLOGW("setObstacle 在:[{},{}] 无法获得 TGMapInfo",x,y);
}

void TileMapManager::setDoorOffset(size_t x, size_t y, uint8_t DoorOffset) {
    TGMapInfo* tileInfo = GetTileInfo(x, y);
    if (tileInfo) {
        tileInfo->setDoorOffset(DoorOffset);
    } 
    // else {
    //     throw std::out_of_range("Tile coordinates are out of the map boundaries.");
    // }
    AXLOGW("setDoorOffset 在:[{},{}] 无法获得 TGMapInfo",x,y);    
}

NS_AX_END