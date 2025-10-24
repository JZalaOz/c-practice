cmake_minimum_required(VERSION 4.0)
project(cstuff C)

set(CMAKE_C_STANDARD 17)

add_executable(cstuff
        src/main.c
        src/linkedlist/linkedlist.c
        src/hashmap/hashmap.c
        src/vector/vector.c
        src/allocation/allocation.c
)

target_link_libraries(cstuff PRIVATE m)
