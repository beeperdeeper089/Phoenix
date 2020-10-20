set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/Window.hpp

	${currentDir}/Layer.hpp
	${currentDir}/LayerStack.hpp

	${currentDir}/Tooling/Timer.hpp
	${currentDir}/Tooling/OpenGLTools.hpp
	${currentDir}/Tooling/ImGuiExtensions.hpp

	${currentDir}/ShaderPipeline.hpp
	${currentDir}/Camera.hpp

	${currentDir}/ChunkMesher.hpp
	${currentDir}/ChunkRenderer.hpp

	${currentDir}/ChatBox.hpp

	PARENT_SCOPE
)
