--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

]]

-- >= lua-5.4
if (math.pow == nil) then
    -- AXLOG_WITH_LEVEL(1, "加载 实现 math.pow")
    math.pow = function(x, y)
        return x ^ y
    end
else
    -- AXLOG_WITH_LEVEL(1, "存在 math.pow 无需实现")
    -- [L]存在 math.pow 无需实现
end


-- < lua-5.4
if (unpack) then
    -- AXLOG_WITH_LEVEL(1, "存在 unpack 存储到 table.unpack")
    table.unpack = unpack
else
    -- AXLOG_WITH_LEVEL(1, "不存在 unpack ")
    -- [L]不存在 unpack
end

-- < axmol-1.0
if not cc then 
    cc = ax
    cc.Director.setDisplayStats = ax.Director.setStatsDisplay
    cc.Sprite3D = ax.MeshRenderer
    ccexp = axexp
    ccui = axui
    ccb = axb
    ax.UserData = ax.UserDataEx;    
end

require "axmol.core.Axmol"
require "axmol.core.Constants"
require "axmol.core.functions"
-- deprecated.lua
require "axmol.core.deprecated"

cc.Director.getOpenGLView = cc.Director.getGLView

if not __G__TRACKBACK__ then 
    __G__TRACKBACK__ = function(msg)
        local msg = debug.traceback(msg, 3)
        print(msg)
        return msg
    end
end
-- opengl
--渲染器变更后，不能再用 Opengl 的指令
-- require "axmol.core.Opengl"
-- require "axmol.core.OpenglConstants"
-- audio
-- require "cocos.cocosdenshion.AudioEngine"

-- cocosstudio
if nil ~= ccs then
    AXLOGD("加载 axmol.cocostudio.CocoStudio")
    --实际 已经加载
    require "axmol.cocostudio.CocoStudio"
end
-- ui
if nil ~= ccui then
    AXLOGD("加载 axmol.ui.GuiConstants")
    --实际 已经加载
    require "axmol.ui.GuiConstants"
end

-- extensions
AXLOGD("加载 axmol.extension.ExtensionConstants")
require "axmol.extension.ExtensionConstants"
-- network
AXLOGD("加载 axmol.network.NetworkConstants")
require "axmol.network.NetworkConstants"
-- Spine
if nil ~= sp then
    AXLOGD("加载 axmol.spine.SpineConstants")
    --实际 已经加载
    require "axmol.spine.SpineConstants"
end

-- Lua extensions
AXLOGD("加载 axmol.core.bitExtend")
require "axmol.core.bitExtend"

-- physics3d
if AX_USE_PHYSICS3D then
    AXLOGD("加载 axmol.physics3d.physics3d-constants")
    --实际 已经加载
    require "axmol.physics3d.physics3d-constants"
end


if AX_USE_FRAMEWORK then
    AXLOGD("加载 axmol.framework.init")
    --实际 已经加载
    require "axmol.framework.init"
end
