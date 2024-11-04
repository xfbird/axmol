#include "LuaBridgeControl.h"
#include "base/Logging.h"
#include "LuaScriptHandlerMgr.h"
#include "platform/FileUtils.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/lua_module_register.h"
#include "base/Configuration.h"
#include "lua-bindings/manual/tolua_fix.h"

NS_AX_BEGIN
// 初始化静态成员变量
LuaBridgeControl* LuaBridgeControl::_Instance = nullptr;
// std::mutex LuaBridgeControl::_Mutex;

// 私有构造函数
LuaBridgeControl::LuaBridgeControl() {
    // 初始化代码可以放在这里
      SetModulesSwitch(4,Configuration::getInstance()->supportsETC2());
      SetModulesSwitch(5,1);
      SetModulesSwitch(7,1);
}

// 私有析构函数
LuaBridgeControl::~LuaBridgeControl() {
    // 清理代码可以放在这里
}

// 获取单例对象的静态方法
LuaBridgeControl* LuaBridgeControl::GetInstance() {
    // AXLOGD("LuaBridgeControl::GetInstance");
    // std::lock_guard<std::mutex> lock(_Mutex);
    if (_Instance == nullptr) {
        _Instance = new LuaBridgeControl();
        AXLOGD("LuaBridgeControl::GetInstance need new :{}",fmt::ptr(_Instance));
    }
    AXLOGD("LuaBridgeControl::GetInstance ret:{}",fmt::ptr(_Instance));
    return _Instance;
}

// 销毁单例对象的静态方法
void LuaBridgeControl::DestroyInstance() {
    // std::lock_guard<std::mutex> lock(_Mutex);
    delete _Instance;
    _Instance = nullptr;
}

// _Init3rdparty 的 getter
bool LuaBridgeControl::GetInit3rdparty() const {
    return _Init3rdparty;
}

// _Init3rdparty 的 setter
void LuaBridgeControl::SetInit3rdparty(bool init3rdparty) {
    _Init3rdparty = init3rdparty;
}

// _ModulesSwitch 的 getter
int LuaBridgeControl::GetModulesSwitch(int index) const {
    if (index >= 0 && index < 100) {
        return _ModulesSwitch[index];
    }
    // throw std::out_of_range("Index out of range");
    AXLOGW("Index out of range :{}",index);
    return 0;
}

// _ModulesSwitch 的 setter
void LuaBridgeControl::SetModulesSwitch(int index, int value) {
    if (index >= 0 && index < 100) {
        _ModulesSwitch[index] = value;
    } else {
        // throw std::out_of_range("Index out of range");
        AXLOGW("Index out of range :{}",index);
    }
}
bool LuaBridgeControl::GameMain()
{
    AXLOGD("LuaBridgeControl::GameMain in");
    auto engine = LuaEngine::getInstance();
    lua_State* L = engine->getLuaStack()->getLuaState();
    
    AXLOGD("lua_module_register(L)");
    lua_module_register(L);
   // //LuaStack* stack = engine->getLuaStack();
   if (true)
    {
       auto fui = FileUtils::getInstance();
       // 1. 获取默认资源根路径，并在其后面加上 "cache/"
       std::string resourceRootPath = fui->getDefaultResourceRootPath();
       resourceRootPath += "cache/";
       AXLOGD("设置当前写入 路径: {}", resourceRootPath);
       // 2. 设置当前写入目录
       fui->setWritablePath(resourceRootPath);
       // 3. 获取当前的搜索路径列表
       // const std::vector<std::string>& cursearchdirs = fui->getSearchPaths();
       // 4. 清空搜索路径列表
       // cursearchdirs.clear();
       std::string writablePath = fui->getWritablePath();
       AXLOGD("读出写入 路径: {}", writablePath);
       std::vector<std::string> newSearchDirs = fui->getSearchPaths();
       newSearchDirs.clear();
       // newSearchDirs();
       // 5. 获取当前可写路径，并在其后面加上 "mod_launcher"，然后将其添加到新的搜索路径列表
       writablePath += "mod_launcher";
       // AXLOGD("设置 可写目录添加 mod_launcher: {} 加入到搜索目录",writablePath);
       newSearchDirs.push_back(writablePath);
       // 6. 将 "mod_launcher" 目录添加到新的搜索路径列表
       newSearchDirs.push_back("mod_launcher");
       // AXLOGD("设置 可写目录添加 mod_launcher: {} 加入到搜索目录","mod_launcher");
       newSearchDirs.push_back("mod_launcher/stab/");
       // AXLOGD("设置 可写目录添加 mod_launcher: {} 加入到搜索目录","mod_launcher");
       //  7. 设置新的搜索顺序
       fui->setSearchPaths(newSearchDirs);
       for (const auto& dir : newSearchDirs)
       {
           AXLOGD("fileutils SearchPaths :{}", dir);
       };
        AXLOGD("启动脚本运行");
        // FileUtils::getInstance()->addSearchPath("res");
        engine->addSearchPath("scripts");
        if (engine->executeScriptFile("scripts/main.lua"))
        //if (engine->executeScriptFile("main.lua"))
        {
            return false;
        }
   }else {
    // // register custom function
    // // LuaStack* stack = engine->getLuaStack();
    // // register_custom_function(stack->getLuaState());
    // // auto lSearchResolutions=fui->getSearchResolutionsOrder();
    // engine->addSearchPath("src");
    // //engine->addSearchPath("scripts");
    engine->addSearchPath("src");
    FileUtils::getInstance()->addSearchPath("res");
    if (engine->executeString("require 'main'"))
    {
        return false;
    }
  }         
    return true;
}


void  LuaBridgeControl::CreateNewState(){
    AXLOGD("LuaBridgeControl::CreateNewState 执行处理");
    ScriptHandlerMgr::destroyInstance();
   
    // ScriptEngineManager
    // LuaEngine::DestroyInstance();

    auto engine = LuaEngine::getInstance();
    AXLOGD("LuaEngine::getInstance() engine:{}",fmt::ptr(engine));
    engine->init();
    AXLOGD("CreateNewState Call GameMain");
    LuaBridgeControl::GetInstance()->GameMain();
}


NS_AX_END
