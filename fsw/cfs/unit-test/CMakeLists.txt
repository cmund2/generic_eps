##################################################################
#
# Coverage Unit Test build recipe for generic_eps
#
# This CMake file is invoked from the parent CMakeLists.txt
# when ENABLE_UNIT_TESTS is ON.
#
##################################################################

#--- 1) build a stub library that fakes out device and HW calls ---#
add_cfe_coverage_stubs(generic_eps-internal
        stubs/generic_eps_device_stubs.c
        stubs/libi2c_stubs.c          # remove or replace if EPS doesn't use I2C
)
target_link_libraries(coverage-generic_eps-internal-stubs
        ut_core_api_stubs
        ut_assert
)
target_include_directories(coverage-generic_eps-internal-stubs
        PUBLIC $<TARGET_PROPERTY:generic_eps,INCLUDE_DIRECTORIES>
)

#--- 2) pull in all the headers we need to compile private code ---#
# allow direct inclusion of your src/.c files
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
# local test helpers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
# pull in the common headers under coveragetest/
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/coveragetest)
# mission-wide cFE public headers
include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)
# UT-assert framework
include_directories(${PROJECT_SOURCE_DIR}/../../../../fsw/osal/ut_assert/inc)

#--- 3) coverage test for your app logic ---#
add_cfe_coverage_test(generic_eps ALL
        "coveragetest/coveragetest_generic_eps_app.c"
        "../src/generic_eps_app.c"
)

#--- 4) coverage test for your device layer ---#
add_cfe_coverage_test(generic_eps DEVICE
        "coveragetest/coveragetest_generic_eps_device.c"
        "../../shared/generic_eps_device.c"
        "stubs/libi2c_stubs.c"
)

#--- 5) make sure the “ALL” test links against our stubs library ---#
add_cfe_coverage_dependency(generic_eps ALL generic_eps-internal)
