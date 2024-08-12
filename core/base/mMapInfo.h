#ifndef MTILE_MapInfo_H
#define MTILE_MapInfo_H
#include "platform/PlatformMacros.h"
#include "base/Logging.h"
// #include <fstream>
#include <istream> // 包含 std::istream 所需的头文件
#include <array>
#include <cstdint>

NS_AX_BEGIN
//通用格式的 内部存储结构
//处理 map 文件 结构读出，存放到 TGMapInfo 数组
class AX_DLL TGMapInfo  {
    private:
        uint16_t wBkImg;                    //背景瓦片索引
        uint16_t wMidImg;                   //中景瓦片索引
        uint16_t wFrImg;                    //前景对象瓦片索引
        uint16_t wAniImg;                   //动画索引
        uint8_t  btDoorIndex;               //门    
        uint8_t  btDoorOffset;              //门偏移
        uint8_t  btAniFrame;                //帧
        uint8_t  btAniTick;                 //帧间隔
        uint8_t  btLight;                   //照明

        uint8_t  btUnitBkImg;               //前景瓷砖集
        uint8_t  btUnitMidImg;              //中景瓷砖集
        uint8_t  btUnitFrImg;               //对象瓷砖集
        uint8_t  btUnitAniImg;              //动画瓷砖集
        uint8_t  btAniTilesFrame;           //动画帧
        uint8_t  btAniTilesTick;            //动画帧间隔
        uint8_t  btAniType;                 //动画类型
        
    public:
        TGMapInfo():
            wBkImg(0), wMidImg(0), wFrImg(0), btDoorIndex(0), btDoorOffset(0), btAniFrame(0), btAniTick(0), btUnitFrImg(0), btLight(0) 
            ,btUnitBkImg(0), btUnitMidImg(0), wAniImg(0),btAniTilesFrame(0), btAniTilesTick(0), btAniType(0){}

        void readMapInfo(std::istream& file) {
            file.read(reinterpret_cast<char*>(&wBkImg), sizeof(wBkImg));
            file.read(reinterpret_cast<char*>(&wMidImg), sizeof(wMidImg));
            file.read(reinterpret_cast<char*>(&wFrImg), sizeof(wFrImg));
            file.read(reinterpret_cast<char*>(&btDoorIndex), sizeof(btDoorIndex));
            file.read(reinterpret_cast<char*>(&btDoorOffset), sizeof(btDoorOffset));
            file.read(reinterpret_cast<char*>(&btAniFrame), sizeof(btAniFrame));
            file.read(reinterpret_cast<char*>(&btAniTick), sizeof(btAniTick));
            file.read(reinterpret_cast<char*>(&btUnitFrImg), sizeof(btUnitFrImg));
            file.read(reinterpret_cast<char*>(&btLight), sizeof(btLight));
        }

        void readNewMapInfo(std::istream& file) {
            readMapInfo(file);
            file.read(reinterpret_cast<char*>(&btUnitBkImg), sizeof(btUnitBkImg));
            file.read(reinterpret_cast<char*>(&btUnitMidImg), sizeof(btUnitMidImg));
        }

        void readReturnMapInfo(std::istream& file) {
            // readNewMapInfo(file);
            std::array<std::byte, 5> Bytes1;                //临时变量 占位 丢弃
            std::array<std::byte, 12> Bytes2;               //临时变量 占位 丢弃
            readNewMapInfo(file);
            file.read(reinterpret_cast<char*>(&wAniImg), sizeof(wAniImg));
            file.read(reinterpret_cast<char*>(Bytes1.data()), Bytes1.size());
            file.read(reinterpret_cast<char*>(&btAniTilesFrame), sizeof(btAniTilesFrame));
            file.read(reinterpret_cast<char*>(&btAniTilesTick), sizeof(btAniTilesTick));
            file.read(reinterpret_cast<char*>(&btAniType), sizeof(btAniType));
            file.read(reinterpret_cast<char*>(Bytes2.data()), Bytes2.size());
        }

        void readInfo(std::istream& file,int mmapmode){
            switch (mmapmode) {                                 //根据版本 决定 每一个元素如何读出
                case 3:
                    readReturnMapInfo(file);
                break;
                case 2:
                     readNewMapInfo(file);
                break;
                default:
                    readMapInfo(file);
                }
        }

        uint16_t getwBkImg() const { 
            // AXLOGD("getwBkImg {}",wBkImg);
            return wBkImg; }
        uint16_t getwMidImg() const {
            //  AXLOGD("getwMidImg {}",wMidImg);
             return wMidImg; }
        uint16_t getwFrImg() const { 
            // AXLOGD("getwFrImg {}",wFrImg);
            return wFrImg; }
        uint16_t getAniImg() const  { return wAniImg;} 


        uint8_t getAniFrame() const { return btAniFrame; }    
        uint8_t getDoorIndex() const { return btDoorIndex; } 
        uint8_t getDoorOffset() const { return btDoorOffset; } 
        uint8_t getAniTick() const { return btAniTick; } 
        uint8_t getLight() const { return btLight; } 
        uint8_t getUnitBkImg() const { return btUnitBkImg; } 
        uint8_t getUnitMidImg() const { return btUnitMidImg; } 
        uint8_t getUnitFrImg() const { return btUnitFrImg; }
        uint8_t getAniTilesFrame() const { return btAniTilesFrame; } 
        uint8_t getAniTilesTick() const { return btAniTilesTick; } 
        uint8_t getAniType() const { return btAniType; }     
        bool isObstacle() const {
            // 检查 btDoorIndex 的最高位是否为0
            if (((btDoorIndex & 0x80) == 0) | (btDoorOffset& 0x80 >0)) {
                // 检查 wBkImg 或 wMidImg 的最高位是否为1
                if (((wBkImg & 0x8000) > 0 ) | 
                    ((wMidImg & 0x8000) > 0) |
                    ((wFrImg & 0x8000) > 0 ) ) {
                    // 如果是，则该位置被认为是障碍物
                    return true;
                } 
                return false;
            } 
            // 如果 btDoorOffset 大于0，则该位置被认为是障碍物
            return false;
        }
        void setObstacle(bool bObstacle) {
            // 检查 btDoorIndex 的最高位是否为0
            if (bObstacle){
                wBkImg |= 0x8000;
                wMidImg |= 0x8000;
            } else
            {
                wBkImg &= 0x8000;
                wMidImg &=0x8000;
            }
        }
        void setDoorOffset(uint8_t DoorOffset) {
            // 检查 btDoorIndex 的最高位是否为0
            btDoorOffset=DoorOffset;
        }

};

NS_AX_END
#endif //MTILE_MapInfo_H