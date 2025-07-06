cmake_minimum_required(VERSION 3.16)
project(simple_eps_sim_test)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
        model-wrapper.cpp
        ../src/generic_eps_hardware_model.cpp
        ../src/generic_eps_static_data_provider.cpp
)

include_directories(
        ../inc
        ${Boost_INCLUDE_DIRS}
)

find_package(Boost REQUIRED COMPONENTS system filesystem)
add_executable(model-wrapper ${SOURCES})
target_link_libraries(model-wrapper ${Boost_LIBRARIES})