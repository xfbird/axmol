#ifndef MTILE_MapManager_H
#define MTILE_MapManager_H
#include "base/mMapInfo.h"
#include <iostream>
// #include <istream> // 包含 std::istream 所需的头文件
#include <unordered_map>
#include <stdexcept> // For std::runtime_error

NS_AX_BEGIN

class TMapHeader {
    private:
        uint16_t wWidth;
        uint16_t wHeight;
        std::array<uint8_t, 16> sTitle;
        double UpdateDate;
        uint8_t btVersion;
        std::array<uint8_t, 23> Reserved;


    public:
        TMapHeader() : wWidth(0), wHeight(0), UpdateDate(0.0), btVersion(0) {}
        int readHeader(std::istream& file);
        // int readHeader(std::istream& file) {
        //     file.read(reinterpret_cast<char*>(&wWidth), sizeof(wWidth));
        //     file.read(reinterpret_cast<char*>(&wHeight), sizeof(wHeight));
        //     file.read(reinterpret_cast<char*>(sTitle.data()), sTitle.size());
        //     file.read(reinterpret_cast<char*>(&UpdateDate), sizeof(UpdateDate));
        //     file.read(reinterpret_cast<char*>(&btVersion), sizeof(btVersion));
        //     file.read(reinterpret_cast<char*>(Reserved.data()), Reserved.size());

        //     int retversion = 1; // 默认版本号
        //     if (!(btVersion != 13) && (sTitle[14] == 13) && (sTitle[15] == 10)) {
        //         retversion = 3;
        //     } else if (static_cast<unsigned char>(btVersion) == 6) {
        //         retversion = 2;
        //     } else {
        //         retversion = 1;
        //     }
        //     return retversion;
        // }

        uint16_t getWidth() const { return wWidth; }
        uint16_t getHeight() const { return wHeight; }
        double getUpdateDate() const { return UpdateDate; }
        uint8_t getVersion() const { return btVersion; }
        // Additional methods for TMapHeader can be added here
};

using TGMapInfoVector = std::vector<std::unique_ptr<TGMapInfo>>;

class AX_DLL TileMapManager : public Object {
    private:
        uint32_t _Width;
        uint32_t _Height;
        TGMapInfoVector _mapInfoArray; // 局部变量
        int _mapDataType;

        // static TileMapManager* _tmmInstance; // 静态实例指针
        //TileMapManager(); 
        // TileMapManager(){};// 私有构造函数
        // TileMapManager(const TileMapManager&) = delete; // 禁用拷贝构造函数
        // TileMapManager& operator=(const TileMapManager&) = delete; // 禁用赋值操作符
    public:
        TileMapManager() : _Width(0), _Height(0), _mapDataType(0) {} // 公开构造函数

        // 允许拷贝构造和赋值操作符，如果需要
        TileMapManager(const TileMapManager&) = default;
        TileMapManager& operator=(const TileMapManager&) = default;

        ~TileMapManager() = default;
        // static void destroyInstance();
        // 重命名 destroyInstance 为 destroy
        static void destroy(TileMapManager* tmm) {
            AX_SAFE_RELEASE(tmm);            
        }        
        static TileMapManager* create();


        //int getMapFileversion(const std::string& filename);
        bool loadMapFile(const std::string& filename);
        TGMapInfo* GetTileInfo(size_t x, size_t y);
        uint16_t GetBkImgIdx(size_t x, size_t y);
        uint16_t GetMidImgIdx(size_t x, size_t y);
        uint16_t GetFrImgIdx(size_t x, size_t y);
        uint16_t GetAniImgIdx(size_t x, size_t y);
        uint8_t GetAniFrame(size_t x, size_t y);
        uint8_t GetAniTick(size_t x, size_t y);
        uint8_t GetDoorIndex(size_t x, size_t y);
        uint8_t GetDoorOffset(size_t x, size_t y);
        uint8_t GetTilesArea(size_t x, size_t y);
        uint8_t GetSmTilesArea(size_t x, size_t y);
        uint8_t GetObjArea(size_t x, size_t y);
        uint8_t GetLight(size_t x, size_t y);
        // 检查指定坐标的障碍物状态
        bool isObstacle(size_t x, size_t y);
        // 设置指定坐标的障碍物状态
        void setObstacle(size_t x, size_t y, bool bObstacle);
        void setDoorOffset(size_t x, size_t y, uint8_t DoorOffset);
        uint32_t getWidth() const {return _Width;}
        uint32_t getVersion() const {return _mapDataType;}
        // 返回地图高度的函数
        uint32_t getHeight() const {return _Height;}

};
NS_AX_END
#endif //MTILE_VecEx_H