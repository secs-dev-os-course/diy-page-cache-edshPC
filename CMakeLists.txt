cmake_minimum_required(VERSION 3.12)

project(LFUCacheableReader
        VERSION 0.1.0
        DESCRIPTION "OS Lab2"
        LANGUAGES C CXX
)

#set(CMAKE_C_STANDARD 23)
#set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(app)
add_subdirectory(CacheReader)
