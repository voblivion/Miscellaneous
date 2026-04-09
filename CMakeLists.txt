cmake_minimum_required(VERSION 3.20)
project(Miscellaneous VERSION 0.1.0 LANGUAGES CXX)

add_library(Miscellaneous INTERFACE)
add_library(vob::misc ALIAS Miscellaneous)

target_include_directories(Miscellaneous INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(Miscellaneous INTERFACE cxx_std_23)