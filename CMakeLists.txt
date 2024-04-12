cmake_minimum_required(VERSION 3.20)

project(draw_pixels)

add_library(draw_pixels
    draw_pixels.hpp
    draw_pixels.cpp)

add_executable(draw_pixels_test
    main.cpp)

set_target_properties(draw_pixels_test PROPERTIES CXX_STANDARD 23)
target_link_libraries(draw_pixels_test PUBLIC draw_pixels)
target_include_directories(draw_pixels PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
