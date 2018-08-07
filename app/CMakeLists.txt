file(GLOB SOURCES
    "./*.c*"
)

add_executable(driver ${SOURCES})

target_link_libraries(driver
    sv
    yuv
)