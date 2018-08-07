cmake_minimum_required(VERSION 2.8)

project(sjz-vision)

set(DEPS_INCLUDE_DIRS "" cache)
set(var "" cache)
# third party
add_subdirectory(3rdparty/libyuv)


list(APPEND CMAKE_CXX_FLAGS "-std=c++11")


include_directories(
    "${CMAKE_SOURCE_DIR}/3rdparty/libyuv/include"
    "${CMAKE_SOURCE_DIR}/include"
 )

file(GLOB SV_SOURCES
    src/*.c*
)

file(GLOB SV_HEADERS
    src/*.h*
)


add_library(sv ${SV_SOURCES} ${SV_HEADERS})

target_link_libraries(
   sv
   yuv
)

# demo

add_subdirectory(app)