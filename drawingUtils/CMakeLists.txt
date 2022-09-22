project(drawingUtils)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
include_directories(${PROJECT_NAME} includes)

add_library(${PROJECT_NAME} SHARED)

target_compile_definitions(${PROJECT_NAME} PRIVATE CMCS_DRAWINGUTILS_EXPORTS)

target_sources(${PROJECT_NAME} PRIVATE sources/C_font.cpp)
target_sources(${PROJECT_NAME} PRIVATE includes/C_font.hpp)
target_sources(${PROJECT_NAME} PRIVATE sources/C_sprite.cpp)
target_sources(${PROJECT_NAME} PRIVATE includes/C_sprite.hpp)
target_sources(${PROJECT_NAME} PRIVATE sources/C_text.cpp)
target_sources(${PROJECT_NAME} PRIVATE includes/C_text.hpp)
target_sources(${PROJECT_NAME} PRIVATE sources/C_texture.cpp)
target_sources(${PROJECT_NAME} PRIVATE includes/C_texture.hpp)
target_sources(${PROJECT_NAME} PRIVATE sources/C_surface.cpp)
target_sources(${PROJECT_NAME} PRIVATE includes/C_surface.hpp)

target_sources(${PROJECT_NAME} PRIVATE includes/drawingUtilsExtern.hpp)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)
