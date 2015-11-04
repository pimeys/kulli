cmake_minimum_required(VERSION 2.8)
project(kulli)

set(CMAKE_C_FLAGS "-std=c99 -Wall")

add_executable(kulli src/builtin_functions src/eval src/lval src/main src/mpc)

target_link_libraries(kulli -ledit -lm)
