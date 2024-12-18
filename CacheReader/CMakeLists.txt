cmake_minimum_required(VERSION 3.12)
project(CacheReader
        VERSION 0.0.0
        DESCRIPTION "DLL module"
        LANGUAGES C CXX
)

cmake_policy(SET CMP0076 NEW) # avoid warning of relative paths translation

add_compile_options(/utf-8)

add_library(CacheReader SHARED ""
        CacheReaderAPI.h
        CacheReaderAPI.cpp
        Global.h
        LFUCache.cpp)
target_include_directories(CacheReader PRIVATE ${CMAKE_SOURCE_DIR})

target_sources(CacheReader PRIVATE
        CacheReader.cpp
        CacheReader.h
)

target_compile_definitions(CacheReader PRIVATE
        CACHE_READER_EXPORTS)

add_custom_command(TARGET CacheReader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_BINARY_DIR}/CacheReader.dll"
        "${CMAKE_BINARY_DIR}/app")
