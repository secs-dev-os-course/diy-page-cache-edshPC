cmake_minimum_required(VERSION 3.12)
project(app
        VERSION 0.1.0
        DESCRIPTION "Application module"
        LANGUAGES C CXX
)

cmake_policy(SET CMP0076 NEW) # avoid warning of relative paths translation

add_executable(app
        CacheTest_EmaSearchInt.cpp)
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(app "${CMAKE_BINARY_DIR}/CacheReader/CacheReader.lib")

target_sources(app
        PRIVATE
        app.cpp
)
