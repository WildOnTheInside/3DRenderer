cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-seh-rt_v5-rev0/mingw64/bin/g++.exe")
set(SFML_DIR "C:/Program Files/SFML/SFML-2.5.1/lib/cmake/SFML")
set(glm_DIR "C:/Program Files/glm-0.9.9.8/cmake/glm")
set(CMAKE_BUILD_TYPE Debug)

project(renderer)

include_directories("../")
add_executable(renderer main.cpp)

find_package(SFML 2.5.1 COMPONENTS window graphics REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(renderer sfml-window sfml-graphics glm::glm)

install(TARGETS renderer DESTINATION ../bin)