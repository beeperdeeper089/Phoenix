set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/Window.cpp
	${currentDir}/LayerStack.cpp
	${currentDir}/Camera.cpp
	${currentDir}/ImGuiExtensions.cpp

	${currentDir}/ShaderPipeline.cpp

	${currentDir}/ChatBox.cpp

	${currentDir}/ChunkMesher.cpp
	${currentDir}/ChunkRenderer.cpp

	PARENT_SCOPE
)
