CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(TULIP_DIR "%TulipPath%")
SET(CMAKE_MODULE_PATH "%TulipPath%/share/tulip")

SET(PROJECT_NAME %ProjectName:l%)
PROJECT(${PROJECT_NAME})

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
FIND_PACKAGE(TULIP REQUIRED)

INCLUDE_DIRECTORIES(${TULIP_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

SET(PROJECT_SRCS
  main.cpp
# Add source files here
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${TULIP_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)