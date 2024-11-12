cmake_minimum_required(VERSION 3.15)
project(SuperEmu)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SDL2_PATH libs/sdl)

# OPEN GL
find_package(OpenGL REQUIRED)
link_directories(${SDL2_LIB})
include_directories(${OPENGL_INCLUDE_DIRS} ${SDL2_PATH}/include)

# COMMON
set(COMMON_PATH libs/common)
include_directories(${COMMON_PATH}/include)
add_library(common STATIC ${COMMON_PATH}/src/vmem.cc ${COMMON_PATH}/src/ctx.cc ${COMMON_PATH}/src/video.cc)

# MOS
set(6502_PATH libs/mos/6502)
set(656x_PATH libs/mos/656x)
include_directories(${6502_PATH} ${656x_PATH})
add_library(6502 STATIC ${6502_PATH}/6502.cc ${656x_PATH}/6560.cc)
target_link_libraries(6502 PRIVATE common)

# PROJECT
add_executable(superemu src/main.cc)
include_directories(superemu src/)
target_link_libraries(superemu PRIVATE common 6502)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_LIB libs/sdl/bin/win/amd64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SDL2_LIB libs/sdl/bin/win/i386)
    endif()
    target_link_directories(superemu PRIVATE ${SDL2_LIB})
    target_link_libraries(superemu PRIVATE SDL2 SDL2main ${OPENGL_LIBRARIES})
    target_link_directories(common PRIVATE ${SDL2_LIB})
    target_link_libraries(common PRIVATE SDL2 SDL2main ${OPENGL_LIBRARIES})
    
else()
    target_link_libraries(superemu PRIVATE SDL2 SDL2main ${OPENGL_LIBRARIES})
    target_link_libraries(common PRIVATE SDL2 SDL2main ${OPENGL_LIBRARIES})
endif()