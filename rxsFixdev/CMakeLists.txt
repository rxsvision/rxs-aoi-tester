# ===========================================================================
# rxsFixdev — Fixed-device Data Access Layer (DAL) Framework
#
# Build dependency chain:
#   SystemFrame (STATIC) ← rxsHrdwdal (SHARED)
#                        ← rxsImgProcess (SHARED) ← rxsFixdal (SHARED)
#                                                  ← Tester (EXE)
# ===========================================================================

# --- Path constants ---
set(RXS_FIXDEV_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# --- Collect all third-party SDK include directories ---
set(RXS_FIXDEV_INCLUDE_DIRS
    "${RXS_FIXDEV_DIR}"
    "${RXS_FIXDEV_DIR}/SystemEntry"
    "${RXS_FIXDEV_DIR}/rxsHrdwdal"
    "${RXS_FIXDEV_DIR}/rxsImgProcess"
    "${RXS_FIXDEV_DIR}/rxsFixdal"
    "${RXS_FIXDEV_DIR}/Precitec/include"
    "${RXS_FIXDEV_DIR}/KeyenceLib/include"
    "${RXS_FIXDEV_DIR}/LightE/inc"
    "${RXS_FIXDEV_DIR}/SizectorAPI"
    "${RXS_FIXDEV_DIR}/auboi5/inc"
    "${RXS_FIXDEV_DIR}/lvm_sdk/x86/include"
    "${RXS_FIXDEV_DIR}/sszn/SR_SdkDll"
    "${RXS_FIXDEV_DIR}/sszn/SR_SdkDll/x86"
    "${RXS_FIXDEV_DIR}/zmotion/x86"
    "${RXS_FIXDEV_DIR}/3trd/libredwg-0.13.3.7187-win64/include"
    "${RXS_FIXDEV_DIR}/inkType"
)

# --- Output directories ---
set(RXS_FIXDEV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(RXS_FIXDEV_LIB_DIR    "${CMAKE_BINARY_DIR}/lib")

# --- Build order (dependency chain) ---
add_subdirectory(SystemEntry)    # 1. SystemFrame (static lib)
add_subdirectory(rxsHrdwdal)     # 2. Hardware DAL (DLL)
add_subdirectory(rxsImgProcess)  # 3. Image process (DLL)
add_subdirectory(rxsFixdal)      # 4. Fix DAL (DLL)
add_subdirectory(Tester)         # 5. Test executables

message(STATUS "")
message(STATUS "=== rxsFixdev Build Chain ===")
message(STATUS "  1. SystemFrame (STATIC)")
message(STATUS "  2. rxsHrdwdal (SHARED) → depends: SystemFrame")
message(STATUS "  3. rxsImgProcess (SHARED) → depends: SystemFrame, rxsHrdwdal")
message(STATUS "  4. rxsFixdal (SHARED) → depends: SystemFrame, rxsHrdwdal, rxsImgProcess")
message(STATUS "  5. Tester (EXE) → depends: all above")
message(STATUS "============================")
