set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
        ${Headers}

	${currentDir}/Window.hpp
	${currentDir}/InputState.hpp

	${currentDir}/RenderStage.hpp
        ${currentDir}/RenderPipeline.hpp

        PARENT_SCOPE
)
