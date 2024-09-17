#ifndef LUABRIDGECONTROL_H
#define LUABRIDGECONTROL_H

#include "platform/PlatformMacros.h"
#include "base/Macros.h"
#include "base/Object.h"
// #include <string>
// #include <cstdint> // For fixed-width integer types
#include <stdexcept> // 引入标准异常头文件，用于异常处理
#include <mutex>

NS_AX_BEGIN

class AX_DLL LuaBridgeControl : public Object 
{
private:
    static LuaBridgeControl* _Instance; // 单例实例
    // static std::mutex _Mutex; // 用于同步的互斥锁
    bool _Init3rdparty = false; // 初始化第三方库的标志
    int _ModulesSwitch[100] = {}; // 模块开关数组

    // 私有构造函数和析构函数
    LuaBridgeControl();
    ~LuaBridgeControl();

    // 禁止拷贝构造和赋值操作
    LuaBridgeControl(const LuaBridgeControl&) = delete;
    LuaBridgeControl& operator=(const LuaBridgeControl&) = delete;

public:
    // 获取单例对象的静态方法
    static LuaBridgeControl* GetInstance();
    
    // 销毁单例对象的静态方法
    static void DestroyInstance();

    // _Init3rdparty 的 getter 和 setter
    bool GetInit3rdparty() const;
    void SetInit3rdparty(bool init3rdparty);

    void CreateNewState();
    bool GameMain();

    // _ModulesSwitch 的 getter 和 setter
    int GetModulesSwitch(int index) const;
    void SetModulesSwitch(int index, int value);
};

NS_AX_END

#endif // LUABRIDGECONTROL_H
