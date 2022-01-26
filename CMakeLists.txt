cmake_minimum_required(VERSION 3.5)

project(utilchan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (API_KEY)
    add_compile_definitions(API_KEY="${API_KEY}")
else()
    message( FATAL_ERROR "Please set your api key using API_KEY!" )
endif()


add_executable(utilchan
    main.cpp
    systemd.cpp
    systemd.h
    utilites.h
    utilites.cpp
    utilchanbot.h
    utilchanbot.cpp
)


#Add these two lines
add_subdirectory(dependencies/sleepy-discord)
target_link_libraries(utilchan sleepy-discord)

# First, find sdbus-c++
find_package(sdbus-c++ REQUIRED)

# Use the sdbus-c++ target in SDBusCpp namespace
target_link_libraries(utilchan SDBusCpp::sdbus-c++)
