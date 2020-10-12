add_subdirectory(Content)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/Save.cpp
	${currentDir}/Actor.cpp
	${currentDir}/Commander.cpp
	${currentDir}/Input.cpp
	${currentDir}/PlayerView.cpp

	PARENT_SCOPE
)
