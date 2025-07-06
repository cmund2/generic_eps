cmake_minimum_required(VERSION 3.16)
project(wrappert_generic_eps)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        wrappert_generic_eps.cpp
        ../../sim/src/generic_eps_hardware_model.cpp
        ../../sim/src/generic_eps_static_data_provider.cpp
        ../../sim/src/generic_static_data_provider.cpp
        ../../shared/generic_eps_device.c
)

include_directories(
        ../../sim/inc
        ../../shared
        ../../cfs/src
)

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(wrappert_generic_eps ${SOURCES})

target_link_libraries(wrappert_generic_eps ${Boost_LIBRARIES})




