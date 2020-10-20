add_subdirectory(Overlays)
add_subdirectory(Systems)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/MainMenu.hpp
	${currentDir}/Game.hpp
	${currentDir}/InputMap.hpp
	${currentDir}/InputQueue.hpp
	${currentDir}/Network.hpp

        PARENT_SCOPE
)
