
set(lib_name LogSave)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
    LogSave.cpp
)

target_include_directories(${target_name} PUBLIC .)
