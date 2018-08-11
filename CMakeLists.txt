cmake_minimum_required(VERSION 2.8)

project(libsv)

set(LibSV_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(LibSV_BINARY_DIR ${CMAKE_BINARY_DIR})


include("cmake/sv_utils.cmake")

add_subdirectory(3rdparty/libyuv)
add_subdirectory(3rdparty/libjpeg-turbo)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

sv_debug_message("--- ${SV_INCLUDE_DIR}")

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${SV_INCLUDE_DIR}"
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
   jpeg-static
)

# demo

add_subdirectory(app)

enable_testing()
add_subdirectory(test)