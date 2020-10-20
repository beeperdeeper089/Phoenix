add_subdirectory(Graphics)
add_subdirectory(Game)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${Sources}

        ${currentDir}/Client.cpp
        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )
