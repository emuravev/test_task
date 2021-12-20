cmake_minimum_required(VERSION 3.1)

project(test_task)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(client client.cpp)
add_executable(server server.cpp)
target_link_libraries(server PRIVATE Threads::Threads)
