configure_file(base.h.in base.h)

find_package(OpenCV REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OpenCV_INCLUDE_DIR}
    )


add_executable(test_image test_image.cpp)
target_link_libraries(test_image sv ${OpenCV_LIBS})
add_test(image test_image)
