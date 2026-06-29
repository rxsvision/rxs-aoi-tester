cmake_minimum_required(VERSION 3.5)


set(CurVersion 1.0.012.1)

#project(AoiTester VERSION 0.1 LANGUAGES CXX)
project(AoiTester LANGUAGES CXX VERSION ${CurVersion})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5AxContainer REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(VM4X_ROOT E:/VisonMaster4.3/VisionMaster4.3.0/Development/V4.x)
include( CMakePolicies NO_POLICY_SCOPE )

include_directories( ${CC_FBO_LIB_SOURCE_DIR}/include )
include_directories( ${CC_CORE_LIB_SOURCE_DIR}/include )
include_directories( ${QCC_DB_LIB_SOURCE_DIR} )
include_directories( ${QCC_DB_LIB_SOURCE_DIR}/msvc )
include_directories( ${QCC_IO_LIB_SOURCE_DIR} )
include_directories( ${QCC_GL_LIB_SOURCE_DIR} )
include_directories( ${CloudComparePlugins_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../common )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qcustomplot )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common ${CMAKE_CURRENT_BINARY_DIR}/common)


file(COPY ../../rxsFixdev/SystemEntry/public.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../../rxsFixdev/SystemEntry/rxsMainFrame.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../../rxsFixdev/rxsHrdwdal/rxsHrdwdal.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../../rxsFixdev/rxsImgProcess/imgProcess.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../../rxsFixdev/rxsFixdal/rxsFixdal.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory( pluginManager )


IF(CMAKE_CL_64)
    message("cur x64:")
    #if(EXISTS ../../rxsFixdev)
        file(COPY ../../rxsFixdev/x64/Release/SystemFrame.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/SystemFrame.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsHrdwdal.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsHrdwdal.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsImgProcess.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsImgProcess.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsFixdal.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsFixdal.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        file(COPY ../../rxsFixdev/x64/Release/SystemFrame.pdb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsHrdwdal.pdb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsImgProcess.pdb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/x64/Release/rxsFixdal.pdb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        #----以下共焦普相关dll----
        file(COPY ../../rxsFixdev/LightE/x64/pthreadGC2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/pthreadVC2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/LEZlog_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/LEMotion_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/ConfocalDLL_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/KSJApi64u.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/EBDConfocal_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/EBDConfocalEx_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/gc_oem_ly.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/LightE/x64/nmc_lib20.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/KeyenceLib/x64/CL3_IF.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/KeyenceLib/ljx8000A/x64/LJX8_IF.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/lvm_sdk/x64/bin/nvt_lvm_sdk.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/Precitec/win/x64/CHRocodile.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/Precitec/win/x64/RecipeCompiler.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/sszn/SR_SdkDll/x64/SR7Link.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        #----以上共焦普相关dll----
        file(COPY ../../rxsFixdev/zmotion/x64/zmotion.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/zmotion/x64/zauxdll.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/inkType/x64/inkType.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        file(COPY ../../rxsFixdev/rxsToolkit/LaptopDLL-V18.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/rxsToolkit/LaptopDLL.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/rxsToolkit/Registration2D.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/rxsToolkit/czxToolkit2D.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/rxsToolkit/czxToolkit.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../../rxsFixdev/rxsToolkit/conf_curve.czx DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        file(COPY ../../rxsFixdev/dmtx/libdmtx.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

ELSE(CMAKE_CL_64)
    message("error cur x86:")
ENDIF(CMAKE_CL_64)

#..自定义脚本----版本
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(MY_VERSIONINFO_RC "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc.in" "${MY_VERSIONINFO_RC}")
else()
    message(STATUS "no windows ${CurVersion}")
    SET_TARGET_PROPERTIES("${PROJECT_NAME}${ProductNameSufffix}" PROPERTIES VERSION ${CurVersion} SOVERSION 1)
endif()


set(TS_FILES AoiTester_zh_CN.ts)

# 流程图相关文件
include_directories(flowchart)
file(GLOB FLOWCHART_SOURCE "flowchart/*.cpp" "flowchart/*.h" "flowchart/*.ui" "flowchart/*.qrc")

file(GLOB CUR_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

file( GLOB qrc_list *.qrc )



qt5_add_resources( generated_qrc_list ${qrc_list} )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AoiTester
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(AoiTester SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(AoiTester
            ${CUR_SOURCE}
            ${FLOWCHART_SOURCE}
            ${TS_FILES}
            ${MY_VERSIONINFO_RC}
            ${header_list}
            ${source_list}
            ${ui_list}
            ${generated_qrc_list}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(AoiTester PRIVATE
    CC_FBO_LIB
    CC_CORE_LIB
    QCC_DB_LIB
    QCC_IO_LIB
    QCC_GL_LIB
    qcustomplot
    ${CMAKE_CURRENT_BINARY_DIR}/SystemFrame.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsHrdwdal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsImgProcess.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsFixdal.lib
    ${VM4X_ROOT}/Libraries/win64/C/iMVS-6000PlatformSDK.lib
    Qt${QT_VERSION_MAJOR}::AxContainer
    Qt${QT_VERSION_MAJOR}::WinMain
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::PrintSupport)



if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AoiTester)
endif()
set_target_properties(AoiTester PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if( WIN32 )
        if (MSVC)
                set_target_properties( ${PROJECT_NAME} PROPERTIES LINK_FLAGS " /MANIFEST:NO" )
        endif()
endif()

include(GNUInstallDirs)
install(TARGETS AoiTester
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AoiTester)
endif()
