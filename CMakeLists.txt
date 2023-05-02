cmake_minimum_required(VERSION 3.23)
project(C51MicroControllerSimulator)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

FetchContent_Declare(replxx
        GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
        )

FetchContent_Declare(SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.26.5
        )

FetchContent_Declare(SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
        GIT_TAG release-2.20.2
        )

FetchContent_MakeAvailable(replxx)
FetchContent_MakeAvailable(SDL2 SDL2_ttf)

include_directories(${CMAKE_PROJECT_NAME} ${SDL2_INCLUDE_DIR})
include_directories(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/extensions)

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE sources/simul_main.cpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/simul_main.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE sources/simul_core.cpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/simul_core.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE sources/simul_ext.cpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/simul_ext.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE sources/simul_math.cpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/simul_main.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/C_shareableData.inl)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/C_shareableData.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE sources/simul_C8051_opcode.cpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE includes/simul_C8051_opcode.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE extensions/C_bit.inl)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE extensions/C_bit.hpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE extensions/CMCS_API.hpp)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE extensions/simul_device.hpp)

target_link_libraries(${CMAKE_PROJECT_NAME} mingw32 SDL2main SDL2 SDL2_ttf replxx)

add_subdirectory(drawingUtils)
set(DRAWINGUTILS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/drawingUtils/includes)

#init .txt
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/init.txt"
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

#extensions data
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/extensions/CMCS_GUI/data"
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/data)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/extensions/CMCS_Oscilloscope/data"
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/data)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/data"
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/data)

#DLLs
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<IF:$<TARGET_EXISTS:replxx>,$<$<TARGET_EXISTS:replxx>:$<TARGET_FILE:replxx>>,"${PROJECT_SOURCE_DIR}/dummy">
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<IF:$<TARGET_EXISTS:SDL2>,$<$<TARGET_EXISTS:SDL2>:$<TARGET_FILE:SDL2>>,"${PROJECT_SOURCE_DIR}/dummy">
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<IF:$<TARGET_EXISTS:SDL2_ttf>,$<$<TARGET_EXISTS:SDL2_ttf>:$<TARGET_FILE:SDL2_ttf>>,"${PROJECT_SOURCE_DIR}/dummy">
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<IF:$<TARGET_EXISTS:drawingUtils>,$<$<TARGET_EXISTS:drawingUtils>:$<TARGET_FILE:drawingUtils>>,"${PROJECT_SOURCE_DIR}/dummy">
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

#Extensions
add_subdirectory(extensions/CMCS_GUI)
add_subdirectory(extensions/CMCS_Oscilloscope)