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

dirt = {}
dirt.name = "Dirt"
dirt.id = "core.dirt"
dirt.textures = {"Assets/dirt.png"}
dirt.onPlace = function(x, y, z)

                end
dirt.onBreak = function(x, y, z)

                end
voxel.block.register(dirt)

block = {}
block.name = "Grass"
block.id = "core.grass"
block.textures = {"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_top.png",  "Assets/dirt.png"}
block.onPlace = function(x, y, z)

                end
block.onBreak = function(x, y, z)

                end
voxel.block.register(block)
