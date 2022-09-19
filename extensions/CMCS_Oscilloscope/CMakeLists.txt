project(CMCS_Oscilloscope)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${PROJECT_NAME} ${DRAWINGUTILS_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_sources(${PROJECT_NAME} PRIVATE main.hpp)

target_sources(${PROJECT_NAME} PRIVATE ../CMCS_API.hpp)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf drawingUtils replxx)
