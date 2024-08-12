# -----------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.6)

# -----------------------------------------------------------------------------------------
set(lib_name luasocket)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
    asynconnect.cpp
    buffer.c
    io.c
    timeout.c
    select.c
    udp.c
    mime.c
    tcp.c
    auxiliar.c
    inet.c
    luasocket.c
    luasocket_scripts.c
    except.c
    options.c
)

# file(GLOB_RECURSE bullet_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)
if(WINDOWS)
    list(APPEND ${target_name}_src
        wsocket.c
    )
else()
    list(APPEND ${target_name}_src
        unix.c
        usocket.c
        serial.c
    )
endif()

add_library(${target_name} STATIC ${${target_name}_src})

if(LINUX)
    target_compile_definitions(${target_name} PRIVATE _GNU_SOURCE)
endif()

# message(STATUS "xx===23 AX_LUA_VERSION=${AX_LUA_VERSION}")
if(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
    target_compile_definitions(${target_name}
        PUBLIC LUA_COMPAT_APIINTCASTS=1
        PUBLIC LUA_COMPAT_MATHLIB=1
    )
elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
    target_compile_definitions(${target_name}
        PUBLIC LUA_COMPAT_5_1=1
        PUBLIC LUA_COMPAT_5_2=1
        PUBLIC LUA_COMPAT_MODULE=1
    )
elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
    target_compile_definitions(${target_name}
        PUBLIC LUA_COMPAT_ALL=1
        PUBLIC LUA_COMPAT_MODULE=1
    )
endif()

get_filename_component(parent_dir "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(parent_dir "${parent_dir}" DIRECTORY)

# 构造完整的路径
# 最终设置 include directories
set(include_path "${parent_dir}/${AX_LUA_ENGINE}")

if(NOT AX_USE_LUAJIT)
    target_include_directories(${target_name} PRIVATE ${include_path})
    message(STATUS "luasocket include_directories PRIVATE ==${include_path}")
else()
    target_include_directories(${target_name} PRIVATE ${include_path}/include)
    message(STATUS "luasocket include_directories PRIVATE ==${include_path}include")
endif()

# get_filename_component(core_dir "${parent_dir}" DIRECTORY)
# get_filename_component(core_dir "${core_dir}" DIRECTORY)
# set(core_include_path "${core_dir}/core")

# message(STATUS "xx===24 luasocket parent_dir_directories PRIVATE ==${core_include_path}")
# target_include_directories(${target_name} PRIVATE ${core_include_path})
target_include_directories(${target_name} PUBLIC .)

target_include_directories(${target_name} INTERFACE ${include_path})

# message(STATUS "luasocket include_directories INTERFACE ==${include_path}")
# message(STATUS "LUASOCKET OUTDir =${CMAKE_BINARY_DIR}/lib")
# target_link_libraries(${target_name} PRIVATE axmol.lib)

# set_target_properties(${target_name}
# PROPERTIES
# ARCHIVE_OUTPUT_DIRECTORY " ${CMAKE_BINARY_DIR}/lib "
# LIBRARY_OUTPUT_DIRECTORY " ${CMAKE_BINARY_DIR}/lib "
# FOLDER " 3rdparty "
# )
