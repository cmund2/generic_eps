cmake_minimum_required(VERSION 3.16)
project(wrappertest_generic_eps)

set(CMAKE_CXX_STANDARD 17)

# Your wrapper test sources
set(SOURCES
        wrappertest_generic_eps.cpp
        i2c_integration_stub.cpp
        ../../../sim/src/generic_eps_hardware_model.cpp
        ../../../sim/src/generic_eps_static_data_provider.cpp
        ../../shared/generic_eps_device.c
)

# Include directories - mimic unit test, add sim includes as needed
include_directories(
        ../../../sim/inc
        ../../shared
        ../src                   # cFE flight software headers (for cfe.h)
        ${PROJECT_SOURCE_DIR}/../unit-test/inc    # unit test helpers
        ${hwlib_MISSION_DIR}/fsw/public_inc       # mission-wide cFE public headers
        ${PROJECT_SOURCE_DIR}/../../../../fsw/osal/ut_assert/inc  # UT-assert framework
        ${Boost_INCLUDE_DIRS}     # Boost for sim
)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(wrappertest_generic_eps ${SOURCES})

target_link_libraries(wrappertest_generic_eps
        ${Boost_LIBRARIES}
        ut_core_api_stubs    # if used by unit tests, link these as well
        ut_assert
)







