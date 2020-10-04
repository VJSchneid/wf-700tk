cmake_minimum_required(VERSION 3.13)

project(wf-700tk LANGUAGES CXX)

add_library(config_wf700tk INTERFACE)

target_compile_features(config_wf700tk INTERFACE cxx_std_17)
target_include_directories(config_wf700tk INTERFACE include/)

if(COVERAGE)
    target_compile_options(config_wf700tk INTERFACE --coverage)
    target_link_options(config_wf700tk INTERFACE --coverage)
endif()

add_subdirectory(src)
add_subdirectory(test)
