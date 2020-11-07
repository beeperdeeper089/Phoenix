core.log_info("Load mod 1")

function hello (args)
    if args[1] == "there" then
        print("General Kenobi")
    elseif args[1] == "world" then
        print("World says hi")
    else
        print("with you, the force is not")
    end
end

--core.command.register("Hello", "Master the arts of the Jedi you must", hello)

core.setting.register({name = "Test1", key = "Mod1.test1", default = 50, max = 100, min = 0})
core.setting.register({name = "Test2", key = "Mod1.test2", default = 50, max = 100, min = 0})
core.setting.register({name = "Test3", key = "Mod1.test3", default = 50, max = 100, min = 0})

local dirtAudioRef = audio.loadMP3("core.dirt_place", "Modules/mod1/Assets/Audio/dirt_place.mp3");

dirt = {}
dirt.name = "Dirt"
dirt.id = "core.dirt"
dirt.textures = {"Assets/dirt.png"}
dirt.onPlace = function(x, y, z)
                    source = {}
                    source.id = dirtAudioRef
                    source.position = {}
                    source.position.x = x
                    source.position.y = y
                    source.position.z = z
                    audio.play(source)
                end
dirt.onBreak = function(x, y, z)
                    source = {}
                    source.id = dirtAudioRef
                    source.position = {}
                    source.position.x = x
                    source.position.y = y
                    source.position.z = z
                    audio.play(source)
                end
voxel.block.register(dirt)

block = {}
block.name = "Grass"
block.id = "core.grass"
block.textures = {"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_top.png",  "Assets/dirt.png"}
block.onPlace = function(x, y, z)
                    source = {}
                    source.id = dirtAudioRef
                    source.position = {}
                    source.position.x = x
                    source.position.y = y
                    source.position.z = z
                    audio.play(source)
                end
block.onBreak = function(x, y, z)
                    source = {}
                    source.id = dirtAudioRef
                    source.position = {}
                    source.position.x = x
                    source.position.y = y
                    source.position.z = z
                    audio.play(source)
                end
voxel.block.register(block)

dirtStair = {}
dirtStair.name = "Dirt Stair"
dirtStair.id = "core.dirt_stair"
dirtStair.model = "Stair"
dirtStair.textures= {"Assets/dirt.png"}
voxel.block.register(dirtStair)

xpanel = {}
xpanel.name = "Test XPanel"
xpanel.id = "core.test_xpanel"
xpanel.model = "XPanel"
xpanel.textures= {"Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/4.png"}
voxel.block.register(xpanel)

xpanelBlock = {}
xpanelBlock.name = "Test XPanelBlock"
xpanelBlock.id = "core.test_xpanel_block"
xpanelBlock.model = "XPanelCube"
xpanelBlock.textures= {"Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/4.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png"}
voxel.block.register(xpanelBlock)

slope = {}
slope.name = "Test Slope"
slope.id = "core.test_slope"
slope.model = "Slope"
slope.textures= {"Assets/xpanel.png", "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/dirt.png"}
voxel.block.register(slope)
