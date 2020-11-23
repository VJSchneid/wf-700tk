cmake_minimum_required(VERSION 3.13)

project(wf-700tk LANGUAGES CXX)

add_library(config_wf700tk INTERFACE)

find_package(Boost REQUIRED)
find_package(Threads)

target_link_libraries(config_wf700tk INTERFACE Boost::boost Threads::Threads)
target_compile_features(config_wf700tk INTERFACE cxx_std_17)
target_include_directories(config_wf700tk INTERFACE include/)

if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(COVERAGE)
    target_compile_options(config_wf700tk INTERFACE --coverage)
    target_link_options(config_wf700tk INTERFACE --coverage)
endif()

add_subdirectory(src)

option(ENABLE_TESTING "build unit tests" 0)
if(ENABLE_TESTING)
    add_subdirectory(test)
endif()

option(ENABLE_EXAMPLES "build examples" 0)
if(ENABLE_EXAMPLES)
    add_subdirectory(example)
endif()
