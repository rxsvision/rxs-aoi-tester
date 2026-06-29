# ===========================================================================
# rxsFixdal — Fixed Device Data Access Layer (Shared Library / DLL)
# Output: rxsFixdal.dll + rxsFixdal.lib
# Depends: SystemFrame, rxsHrdwdal, rxsImgProcess
# ===========================================================================

# --- Source files ---
file(GLOB RXSFIXDAL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB RXSFIXDAL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.def"
)

# --- Create shared library ---
add_library(rxsFixdal SHARED
    ${RXSFIXDAL_SOURCES}
    ${RXSFIXDAL_HEADERS}
)

# --- Module definition file ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rxsFixdal.def")
    set_target_properties(rxsFixdal PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/rxsFixdal.def\""
    )
endif()

# --- Dependencies ---
target_link_libraries(rxsFixdal
    PUBLIC SystemFrame
    PUBLIC rxsHrdwdal
    PUBLIC rxsImgProcess
)

# --- Include directories ---
target_include_directories(rxsFixdal PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${RXS_FIXDEV_INCLUDE_DIRS}
)

# --- Compile definitions ---
target_compile_definitions(rxsFixdal PRIVATE
    _WINDOWS
    _USRDLL
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
)

# --- Output ---
set_target_properties(rxsFixdal PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${RXS_FIXDEV_LIB_DIR}"
)

# --- MSVC settings ---
if(MSVC)
    target_compile_options(rxsFixdal PRIVATE /utf-8 /MP /W2)
endif()

message(STATUS "  [rxsFixdev] rxsFixdal (SHARED) → SystemFrame, rxsHrdwdal, rxsImgProcess")
