add_subdirectory(Math)
add_subdirectory(Utility)
add_subdirectory(Network)
add_subdirectory(Voxels)
add_subdirectory(Game)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp
	${currentDir}/Logger.hpp
	${currentDir}/Registry.hpp
	${currentDir}/Settings.hpp

	PARENT_SCOPE
)
