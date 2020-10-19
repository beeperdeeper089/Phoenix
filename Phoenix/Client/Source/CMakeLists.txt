add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${graphicsSources}
        ${audioSources}

        ${currentDir}/InputQueue.cpp

        ${currentDir}/Client.cpp
        ${currentDir}/Game.cpp
        ${currentDir}/Crosshair.cpp
        ${currentDir}/EscapeMenu.cpp
        ${currentDir}/InputMap.cpp
        ${currentDir}/Network.cpp

        ${currentDir}/DebugOverlay.cpp
        ${currentDir}/GameTools.cpp

        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )
