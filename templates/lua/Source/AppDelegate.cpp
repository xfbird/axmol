/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "AppDelegate.h"
#include "yasio/singleton.hpp"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/lua_module_register.h"

#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "audio/AudioEngine.h"
#endif

using namespace ax;
using namespace std;
class SampleLogOutput : public ILogOutput
{
public:
    SampleLogOutput()
    {
        auto fu   = FileUtils::getInstance();
        _filePath = fu->getWritablePath() + "game.log";
        _fs       = fu->openFileStream(_filePath, ax::IFileStream::Mode::APPEND);

        ax::setLogFmtFlag(ax::LogFmtFlag::Level | ax::LogFmtFlag::TimeStamp | ax::LogFmtFlag::Colored |ax::LogFmtFlag::SourceFn|ax::LogFmtFlag::SourceFl);
        AXLOGW("further log message will write to {}", _filePath);
        ax::setLogOutput(this);
    }

    ~SampleLogOutput() override
    {
        ax::setLogOutput(nullptr);
        _fs->close();
    }
    // virtual void write(std::string_view message, LogLevel) override
    virtual void write(LogItem& item, const char* tag) override
    {
        if (_fs->size() > _threshold)
            _fs->resize(0);
        auto sv = item.message();
        _fs->write(sv.data(), static_cast<int>(sv.size()));
        // std::string(message).c_str(), static_cast<int>(message.size()));
    }

private:
    std::string _filePath;
    std::unique_ptr<IFileStream> _fs;
    int64_t _threshold{1024 * 1024 * 10};  // 10MB
};

AppDelegate::AppDelegate()
{
    yasio::singleton<SampleLogOutput>::instance();    
}

AppDelegate::~AppDelegate() {}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    // glContextAttrs.vsync = false
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // set default FPS
    Director::getInstance()->setAnimationInterval(1.0 / 60.0f);

    // register lua module
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* L = engine->getLuaStack()->getLuaState();
    lua_module_register(L);
    LuaStack* stack = engine->getLuaStack();

    auto fui = FileUtils::getInstance();
    // 1. 获取默认资源根路径，并在其后面加上 "cache/"
    std::string resourceRootPath = fui->getDefaultResourceRootPath();
    resourceRootPath += "cache/";
    AXLOGD("设置当前写入 路径: {}",resourceRootPath);
    // 2. 设置当前写入目录
    fui->setWritablePath(resourceRootPath);

    // 3. 获取当前的搜索路径列表
    // const std::vector<std::string>& cursearchdirs = fui->getSearchPaths();
    // 4. 清空搜索路径列表
    // cursearchdirs.clear();
    std::string writablePath = fui->getWritablePath();
    AXLOGD("读出写入 路径: {}",writablePath);
    std::vector<std::string> newSearchDirs=fui->getSearchPaths();
    newSearchDirs.clear();
    // newSearchDirs();
    // 5. 获取当前可写路径，并在其后面加上 "mod_launcher"，然后将其添加到新的搜索路径列表
    writablePath += "mod_launcher";
    AXLOGD("设置 可写目录添加 mod_launcher: {} 加入到搜索目录",writablePath);
    newSearchDirs.push_back(writablePath);

    // 6. 将 "mod_launcher" 目录添加到新的搜索路径列表
    // AXLOGD("设置 可写目录添加 mod_launcher: {} 加入到搜索目录",writablePath);
    newSearchDirs.push_back("mod_launcher");
    newSearchDirs.push_back("mod_launcher/stab/");

    // 7. 设置新的搜索顺序
    fui->setSearchPaths(newSearchDirs);

    // register custom function
    // LuaStack* stack = engine->getLuaStack();
    // register_custom_function(stack->getLuaState());
    // auto lSearchResolutions=fui->getSearchResolutionsOrder();
    // stack->addSearchPath("src");
    engine->addSearchPath("scripts");
    // FileUtils::getInstance()->addSearchPath("res");
    if (engine->executeScriptFile("scripts/main.lua"))
    {
        return false;
    }

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
