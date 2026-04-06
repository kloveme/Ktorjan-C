cmake_minimum_required(VERSION 3.15)
project(KTorjan C)

set(HEADERS 
  ../Folder/include/
)
set(SOURCES 
   ../Folder/lib/ktorjan_lib.c
   ../Folder/lib/kvictim_lib.c
)
add_executable(ktorjan ../Folder/src/ktorjan.c   
  ../Folder/lib/ktorjan_lib.c)

add_executable(kvictim ../Folder/src/kvictim.c
  ../Folder/lib/kvictim_lib.c)

find_package(Threads REQUIRED)
target_link_libraries(kvictim Threads::Threads)
