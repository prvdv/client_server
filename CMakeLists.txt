cmake_minimum_required(VERSION 3.10)

project(Project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(client client.cpp)
target_link_libraries(client Threads::Threads)

add_executable(server server.cpp)
