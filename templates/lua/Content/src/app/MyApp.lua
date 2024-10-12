
local MyApp = class("MyApp", cc.load("mvc").AppBase)

function MyApp:onCreate()
    math.randomseed(os.time())
    AXStackDumpEx("MyApp Create")
end

return MyApp
