cmake_minimum_required(VERSION 3.13)
project(nottCourseServer)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(SQLiteCpp)

add_executable(nottCourseServer main.cpp util.cpp util.h)

find_package(Crow)
target_link_libraries(nottCourseServer PUBLIC Crow::Crow SQLiteCpp)