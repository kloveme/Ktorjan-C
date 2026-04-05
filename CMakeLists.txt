cmake_minimum_required(VERSION 3.15)
project(KTorjan C)

set(HEADERS 
  ../Your_folder/include/
)
set(SOURCES 
   ../Your_folder/lib/server_lib.c
)
add_executable(ktorjan ../Your_folder/src/ktorjan.c   
  ../Your_folder/lib/server_lib.c)


add_executable(kvictim ../Your_folder/src/kvictim.c)
find_package(Threads REQUIRED)
target_link_libraries(kvictim Threads::Threads)
