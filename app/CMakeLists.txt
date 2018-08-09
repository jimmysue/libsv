
find_package(OpenCV REQUIRED)
file(GLOB SOURCES
    "./*.c*"
)
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(driver ${SOURCES})

target_link_libraries(driver
    sv
    yuv
    ${OPENGL_LIBRARY}
    ${OpenCV_LIBS}
)