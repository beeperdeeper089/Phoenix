set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${Sources}

        ${currentDir}/Overlays/Crosshair.cpp
        ${currentDir}/Overlays/DebugOverlay.cpp
        ${currentDir}/Overlays/EscapeMenu.cpp
        ${currentDir}/Overlays/GameTools.cpp

	${currentDir}/Game.cpp
	${currentDir}/InputMap.cpp
	${currentDir}/InputQueue.cpp
	${currentDir}/Network.cpp

        PARENT_SCOPE
)
