# ===========================================================================
# rxsImgProcess — Image Processing Framework (Shared Library / DLL)
# Output: rxsImgProcess.dll + rxsImgProcess.lib
# Depends: SystemFrame, rxsHrdwdal
# ===========================================================================

# --- Source files ---
file(GLOB RXSIMGPROC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB RXSIMGPROC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.def"
)

# --- Create shared library ---
add_library(rxsImgProcess SHARED
    ${RXSIMGPROC_SOURCES}
    ${RXSIMGPROC_HEADERS}
)

# --- Module definition file ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgProcess.def")
    set_target_properties(rxsImgProcess PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/imgProcess.def\""
    )
endif()

# --- Dependencies ---
target_link_libraries(rxsImgProcess
    PUBLIC SystemFrame
    PUBLIC rxsHrdwdal
)

# --- Include directories ---
target_include_directories(rxsImgProcess PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${RXS_FIXDEV_INCLUDE_DIRS}
)

# --- Compile definitions ---
target_compile_definitions(rxsImgProcess PRIVATE
    _WINDOWS
    _USRDLL
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
)

# --- OpenCV (image processing dependency) ---
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    target_include_directories(rxsImgProcess PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(rxsImgProcess PRIVATE ${OpenCV_LIBS})
endif()

# --- Output ---
set_target_properties(rxsImgProcess PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${RXS_FIXDEV_LIB_DIR}"
)

# --- MSVC settings ---
if(MSVC)
    target_compile_options(rxsImgProcess PRIVATE /utf-8 /MP /W2)
endif()

message(STATUS "  [rxsFixdev] rxsImgProcess (SHARED) → SystemFrame, rxsHrdwdal")
