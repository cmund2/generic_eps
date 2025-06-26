##################################################################
#
# Coverage Unit Test build recipe for GENERIC_EPS
#
##################################################################

# 1) Declare a stub-only target for internal linkage
add_cfe_coverage_stubs(eps-internal
        stubs/generic_eps_device_stubs.c
        stubs/libi2c_stubs.c
)
target_link_libraries(eps-internal
        ut_core_api_stubs
        ut_assert
)
target_include_directories(eps-internal PUBLIC
        $<TARGET_PROPERTY:generic_eps,INCLUDE_DIRECTORIES>
)

# 2) Pull in cFE and your include dirs
include_directories(
        ${PROJECT_SOURCE_DIR}/fsw/src          # app source
        ${CMAKE_CURRENT_SOURCE_DIR}/inc        # your ut headers
        ${hwlib_MISSION_DIR}/fsw/public_inc   # hwlib stubs
)

# 3) App-level coverage test
add_cfe_coverage_test(generic_eps ALL
        "coveragetest/coveragetest_generic_eps_app.c"
        "../src/generic_eps_app.c"
)
add_cfe_coverage_dependency(generic_eps ALL eps-internal)

# 4) Device‐layer coverage test
add_cfe_coverage_test(generic_eps DEVICE
        "coveragetest/coveragetest_generic_eps_device.c"
        "../src/generic_eps_device.c"
)
add_cfe_coverage_dependency(generic_eps DEVICE eps-internal)