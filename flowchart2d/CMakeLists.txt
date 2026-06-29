cmake_minimum_required(VERSION 3.5)


set(CurVersion 1.0.012.1)

#project(AoiTester VERSION 0.1 LANGUAGES CXX)
project(twodMeasurement LANGUAGES CXX VERSION ${CurVersion})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5AxContainer REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )



file(COPY ../rxsFixdev/SystemEntry/public.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../rxsFixdev/SystemEntry/rxsMainFrame.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../rxsFixdev/rxsHrdwdal/rxsHrdwdal.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../rxsFixdev/rxsImgProcess/imgProcess.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../rxsFixdev/rxsFixdal/rxsFixdal.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})



IF(CMAKE_CL_64)
    message("cur x64:")
    #if(EXISTS ../../rxsFixdev)
        file(COPY ../rxsFixdev/x64/Release/SystemFrame.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/SystemFrame.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsHrdwdal.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsHrdwdal.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsImgProcess.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsImgProcess.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsFixdal.lib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/x64/Release/rxsFixdal.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        #----以下共焦普相关dll----
        file(COPY ../rxsFixdev/LightE/x64/pthreadGC2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/pthreadVC2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/LEZlog_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/LEMotion_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/ConfocalDLL_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/KSJApi64u.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/EBDConfocal_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/EBDConfocalEx_x64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/gc_oem_ly.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/LightE/x64/nmc_lib20.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/KeyenceLib/x64/CL3_IF.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/lvm_sdk/x64/bin/nvt_lvm_sdk.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/Precitec/win/x64/CHRocodile.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/Precitec/win/x64/RecipeCompiler.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/sszn/SR_SdkDll/x64/SR7Link.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        #----以上共焦普相关dll----
        file(COPY ../rxsFixdev/zmotion/x64/zmotion.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/zmotion/x64/zauxdll.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/inkType/x64/inkType.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        file(COPY ../rxsFixdev/rxsToolkit/CzxTookit2D.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/rxsToolkit/czxToolkit.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ../rxsFixdev/rxsToolkit/conf_curve.czx DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

ELSE(CMAKE_CL_64)
    message("cur x86:")

ENDIF(CMAKE_CL_64)

#..自定义脚本----版本
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(MY_VERSIONINFO_RC "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc.in" "${MY_VERSIONINFO_RC}")
else()
    message(STATUS "no windows ${CurVersion}")
    SET_TARGET_PROPERTIES("${PROJECT_NAME}${ProductNameSufffix}" PROPERTIES VERSION ${CurVersion} SOVERSION 1)
endif()


set(TS_FILES twodMeasurement_zh_CN.ts)


# 流程图相关文件
include_directories(flowchart)
file(GLOB FLOWCHART_SOURCE "*.cpp" "*.h" "*.ui" "*.qrc")

set(PROJECT_SOURCES
        main.cpp
        ${TS_FILES}
        ${FLOWCHART_SOURCE}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(twodMeasurement
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(twodMeasurement SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(twodMeasurement
            ${PROJECT_SOURCES}
            ${MY_VERSIONINFO_RC}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(twodMeasurement PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/SystemFrame.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsHrdwdal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsImgProcess.lib
    ${CMAKE_CURRENT_BINARY_DIR}/rxsFixdal.lib
    Qt${QT_VERSION_MAJOR}::AxContainer
    Qt${QT_VERSION_MAJOR}::WinMain
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets)



if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.twodMeasurement)
endif()
set_target_properties(twodMeasurement PROPERTIES
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
install(TARGETS twodMeasurement
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(twodMeasurement)
endif()
