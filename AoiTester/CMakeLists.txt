cmake_minimum_required(VERSION 3.0)

project( myAoi_cc )

option(BUILD_TESTING "" OFF)

set( CMAKE_DEBUG_POSTFIX "d" )


list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" )

include( CMakePolicies )
include( CMakeSetCompilerOptions )


# One shouldn't generate the BUILD project directly in the SOURCES folder!
if ( ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
	if ( NOT SAME_BUILD_AND_SOURCE_FOLDER_WARNING_ALREADY_ISSUED )
		message(FATAL_ERROR "It is not advised to BUILD the binaries directly in the SOURCE folder!\n If you want to proceed with this option, just CONFIGURE the project once again" )
		set( SAME_BUILD_AND_SOURCE_FOLDER_WARNING_ALREADY_ISSUED TRUE )
	endif()
endif()



# Quad buffer stereo support
OPTION( OPTION_GL_QUAD_BUFFER_SUPPORT "Check to compile CloudCompare and ccViewer with Quad Buffer support" OFF )


if(WIN32 OR APPLE)
        set( CLOUDCOMPARE_DEST_FOLDER CloudCompare )

        if (OPTION_GL_QUAD_BUFFER_SUPPORT)
                set ( CLOUDCOMPARE_DEST_FOLDER ${CLOUDCOMPARE_DEST_FOLDER}Stereo )

        endif()
else()
        set( CLOUDCOMPARE_DEST_FOLDER bin )

endif()


set( INSTALL_DESTINATIONS ${CLOUDCOMPARE_DEST_FOLDER} )# just need qCC only


# Load advanced scripts
include( CMakeInclude )

add_subdirectory( CC )

# Propagate the choice to use doubles for ScalarType
if ( ${USE_SCALAR_TYPE_DOUBLE} )
	add_definitions( -DSCALAR_TYPE_DOUBLE )
else()
	add_definitions( -DSCALAR_TYPE_FLOAT )
endif()

# Add external libraries
include( CMakeExternalLibs )

# Contrib. libraries (mainly for I/O)
include( contrib/AllSupport.cmake )

# Libs requested by both qCC & ccViewer
add_subdirectory( libs/CCFbo )
add_subdirectory( libs/qCC_db ) # must always be included after CCFbo (dependency)
add_subdirectory( libs/qCC_io ) # must always be included after qCC_db (dependency)
add_subdirectory( libs/qCC_glWindow ) # must always be included after qCC_db (dependency)
add_subdirectory( libs/qcustomplot )

# load plugins (potentially used by qCC and ccViewer)
add_subdirectory( plugins )

add_subdirectory( Aoi_ui )


