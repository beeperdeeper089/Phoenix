add_subdirectory(Content)
add_subdirectory(Components)
add_subdirectory(Systems)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/NetworkEvents.hpp

	PARENT_SCOPE
)
