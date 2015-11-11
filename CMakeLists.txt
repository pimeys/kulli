cmake_minimum_required(VERSION 2.8)
project(musti)

set(CMAKE_C_FLAGS "-std=c99 -Wall")

add_executable(musti src/builtin_functions src/eval src/lval src/main src/mpc)

target_link_libraries(musti -ledit -lm)
execute_process(COMMAND "rm -rf ./lib")
file(GLOB STD_MUSTI "lib/*.musti")
file(COPY ${STD_MUSTI} DESTINATION "./lib")
