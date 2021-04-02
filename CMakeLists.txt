cmake_minimum_required(VERSION 3.17.3)
project(AABB_Triangle_Intersection VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Debug)
ess_fetch_external(NAME Boost VERSION 1.66.0 FIND_PACKAGE COMPONENTS program_options iostreams thread filesystem)

add_subdirectory(source)
