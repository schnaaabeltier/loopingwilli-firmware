cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp-idf-lib/components)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wno-error)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(looping_willi)

