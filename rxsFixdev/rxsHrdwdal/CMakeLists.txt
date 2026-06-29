# ===========================================================================
# rxsHrdwdal — Hardware Data Access Layer (Shared Library / DLL)
# Output: rxsHrdwdal.dll + rxsHrdwdal.lib
# Depends: SystemFrame
# ===========================================================================

# --- Source files ---
file(GLOB RXHRDWDAL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Auboi5/*.cpp"
)

file(GLOB RXHRDWDAL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Auboi5/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.def"
)

# --- Create shared library ---
add_library(rxsHrdwdal SHARED
    ${RXHRDWDAL_SOURCES}
    ${RXHRDWDAL_HEADERS}
)

# --- Module definition file ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rxsHrdwdal.def")
    set_target_properties(rxsHrdwdal PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/rxsHrdwdal.def\""
    )
endif()

# --- Dependencies ---
target_link_libraries(rxsHrdwdal
    PUBLIC SystemFrame
)

# --- Include directories ---
target_include_directories(rxsHrdwdal PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${RXS_FIXDEV_INCLUDE_DIRS}
)

# --- Compile definitions ---
target_compile_definitions(rxsHrdwdal PRIVATE
    RXSHRDWDAL_EXPORTS
    _WINDOWS
    _USRDLL
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
)

# --- Output ---
set_target_properties(rxsHrdwdal PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${RXS_FIXDEV_LIB_DIR}"
)

# --- MSVC settings ---
if(MSVC)
    target_compile_options(rxsHrdwdal PRIVATE /utf-8 /MP /W2)
endif()

message(STATUS "  [rxsFixdev] rxsHrdwdal (SHARED) → SystemFrame")
