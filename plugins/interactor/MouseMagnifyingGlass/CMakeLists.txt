INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipOGLInclude} ${TulipGUIInclude} ${GLEW_INCLUDE_DIR})

SET(LIB_SRCS
  MouseMagnifyingGlass.cpp
)

QTX_ADD_RESOURCES(LIB_RCC_SRCS
MouseMagnifyingGlassResource.qrc
)

ADD_LIBRARY(MouseMagnifyingGlass-${TulipVersion} SHARED ${LIB_SRCS} ${LIB_RCC_SRCS})
TARGET_LINK_LIBRARIES(MouseMagnifyingGlass-${TulipVersion} ${LibTulipCoreName} ${LibTulipOGLName} ${LibTulipGUIName} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

TULIP_INSTALL_PLUGIN(MouseMagnifyingGlass-${TulipVersion} ${TulipInteractorPluginsInstallDir})
