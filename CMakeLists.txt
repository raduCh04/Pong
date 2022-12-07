cmake_minimum_required(VERSION 3.20)

project(Pong)

add_executable(Pong main.c)

target_link_libraries(Pong -lSDL2)
