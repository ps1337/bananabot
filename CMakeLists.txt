cmake_minimum_required(VERSION 3.0.0)
project(bananabot VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(${PROJECT_SOURCE_DIR})
add_executable(bananabot main.cpp globalsettings.h mem.cpp util.cpp game.cpp madmath.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
