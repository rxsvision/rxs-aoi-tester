# ===========================================================================
# SystemEntry — System Framework (Static Library)
# Output: SystemFrame.lib
# ===========================================================================

# --- Source files ---
file(GLOB SYSTEMFRAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB SYSTEMFRAME_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# --- Create static library ---
add_library(SystemFrame STATIC
    ${SYSTEMFRAME_SOURCES}
    ${SYSTEMFRAME_HEADERS}
)

# --- Include directories ---
target_include_directories(SystemFrame PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${RXS_FIXDEV_INCLUDE_DIRS}
)

# --- Compile definitions ---
target_compile_definitions(SystemFrame PUBLIC
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
    _MYX86
)

# --- Output ---
set_target_properties(SystemFrame PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${RXS_FIXDEV_LIB_DIR}"
    OUTPUT_NAME "SystemFrame"
)

# --- PCH support ---
if(MSVC)
    target_compile_options(SystemFrame PRIVATE /utf-8 /MP)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
        target_precompile_headers(SystemFrame PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/pch.h"
        )
    endif()
endif()

message(STATUS "  [rxsFixdev] SystemFrame (STATIC)")
