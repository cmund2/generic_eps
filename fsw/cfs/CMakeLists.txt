cmake_minimum_required(VERSION 2.6.4)
project(CFS_GENERIC_EPS_APP C)

# pull in the standard NOS3 component settings
include(../../../ComponentSettings.cmake)

# public headers
include_directories(public_inc)
# mission-level headers
include_directories(mission_inc)
# platform headers
include_directories(platform_inc)
# your EPS sources
include_directories(src)

# any shared code (device implementation, etc.)
include_directories(../shared)
# common hwlib mission headers
include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

# if you use extra platform-wide includes, uncomment:
# include_directories(${APPLICATION_PLATFORM_INC_LIST})

# collect all .c files under src/
aux_source_directory(src EPS_SRC_FILES)

# build the cFE app module named "generic_eps"
add_cfe_app(generic_eps
        ${EPS_SRC_FILES}
        ../shared/generic_eps_device.c
)

# hook up your unit tests when ENABLE_UNIT_TESTS=ON
if(ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif()

add_subdirectory(component_wrappers)
