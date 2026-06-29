# ===========================================================================
# Tester — Test Executables
# Depends: SystemFrame, rxsHrdwdal, rxsImgProcess, rxsFixdal
#
# Note: Each test file has its own main(), so we create separate executables.
# Original vcxproj listed: FixDalTest.cpp, HrdwDalTest.cpp, ImgProcTest.cpp
# mainTester.cpp also exists as a standalone test.
# ===========================================================================

# --- Common test dependencies ---
set(TEST_LIBS SystemFrame rxsHrdwdal rxsImgProcess rxsFixdal)

# --- Individual test executables ---
# Each .cpp file has its own int main(), so they must be separate targets.

set(TEST_SOURCES
    FixDalTest
    HrdwDalTest
    ImgProcTest
    mainTester
)

foreach(test_name ${TEST_SOURCES})
    set(test_file "${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.cpp")

    if(EXISTS "${test_file}")
        add_executable(Tester_${test_name} "${test_file}")

        target_link_libraries(Tester_${test_name} PRIVATE ${TEST_LIBS})

        target_include_directories(Tester_${test_name} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
            ${RXS_FIXDEV_INCLUDE_DIRS}
        )

        target_compile_definitions(Tester_${test_name} PRIVATE
            _CONSOLE
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
            _SCL_SECURE_NO_WARNINGS
        )

        set_target_properties(Tester_${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${RXS_FIXDEV_OUTPUT_DIR}"
        )

        if(MSVC)
            target_compile_options(Tester_${test_name} PRIVATE /utf-8 /MP /W2)
        endif()

        message(STATUS "  [rxsFixdev] Tester_${test_name} (EXE) → all DAL libs")
    endif()
endforeach()
