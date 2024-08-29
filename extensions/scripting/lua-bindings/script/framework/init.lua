--[[

Copyright (c) 2014-2017 Chukong Technologies Inc.

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

if type(DEBUG) ~= "number" then DEBUG = 0 end

-- load framework
printInfo("")
printInfo("# DEBUG                        = " .. DEBUG)
printInfo("#")

device     = require("axmol.framework.device")
display    = require("axmol.framework.display")
transition = require("axmol.framework.transition")

require("axmol.framework.extends.NodeEx")   --扩展cc.Node
require("axmol.framework.extends.SpriteEx") --扩展 cc.Sprite
require("axmol.framework.extends.LayerEx")  --扩展cc.Layer
require("axmol.framework.extends.MenuEx")   --扩展 cc.MenuItem

if ccui then
    require("axmol.framework.extends.UIWidget")   --扩展ccui.Widget
    require("axmol.framework.extends.UICheckBox") --扩展 ccui.CheckBox
    require("axmol.framework.extends.UIEditBox")
    require("axmol.framework.extends.UIListView")
    require("axmol.framework.extends.UIPageView")
    require("axmol.framework.extends.UIScrollView")
    require("axmol.framework.extends.UISlider")
    require("axmol.framework.extends.UITextField")
end

require("axmol.framework.package_support")

-- register the build-in packages
cc.register("event", require("axmol.framework.components.event"))

-- export global variable
local __g = _G
cc.exports = {}
setmetatable(cc.exports, {
    __newindex = function(_, name, value)
        rawset(__g, name, value)
    end,

    __index = function(_, name)
        return rawget(__g, name)
    end
})

-- disable create unexpected global variable
function cc.disable_global()
    setmetatable(__g, {
        __newindex = function(_, name, value)
            error(string.format("USE \" cc.exports.%s = value \" INSTEAD OF SET GLOBAL VARIABLE", name), 0)
        end
    })
end

-- if AX_DISABLE_GLOBAL then --不允许 直接使用 global 全局变量。需要通过 cc.exports.xxxx = value 方式来使用
--     cc.disable_global()
-- end
