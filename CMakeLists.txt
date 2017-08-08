cmake_minimum_required(VERSION 3.7)

project(erasure-codes)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(galois)
add_subdirectory(galois-test)
