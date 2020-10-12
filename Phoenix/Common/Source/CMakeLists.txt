add_subdirectory(Math)
add_subdirectory(Network)
add_subdirectory(Voxels)
add_subdirectory(Game)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/Settings.cpp
	${currentDir}/Logger.cpp

	PARENT_SCOPE
)
