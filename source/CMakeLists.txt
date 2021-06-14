cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_COMPILER "path/to/compiler")
set(SFML_DIR "../lib/SFML/SFML-2.5.1/lib/cmake/SFML")
set(glm_DIR "../lib/glm-0.9.9.8/cmake/glm")
set(CMAKE_BUILD_TYPE Release)

project(renderer)

include_directories("../")
add_executable(renderer main.cpp)

find_package(SFML 2.5.1 COMPONENTS window graphics REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(renderer sfml-window sfml-graphics glm::glm)

install(TARGETS renderer DESTINATION ../bin)
