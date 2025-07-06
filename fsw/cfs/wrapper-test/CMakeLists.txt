cmake_minimum_required(VERSION 3.16)
project(wrappertest_generic_eps)

set(CMAKE_CXX_STANDARD 17)

# Main sources for the wrapper test
set(SOURCES
        wrappertest_generic_eps.cpp
        i2c_integration_stub.cpp
        ../../../sim/src/generic_eps_hardware_model.cpp
        ../../../sim/src/generic_eps_static_data_provider.cpp
        ../../shared/generic_eps_device.c
)

# Include directories for all relevant headers
include_directories(
        ../../sim/inc
        ../../shared
        ../../cfs/src
)

# Boost (needed by sim and property_tree)
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(wrappertest_generic_eps ${SOURCES})

target_link_libraries(wrappertest_generic_eps ${Boost_LIBRARIES})





