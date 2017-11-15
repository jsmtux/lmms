INCLUDE(BuildPlugin)

INCLUDE_DIRECTORIES(exprtk)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_sc_andor")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_return_statement")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_break_continue")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_comments")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_string_capabilities")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_rtl_io_file")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dexprtk_disable_rtl_vecops")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WERROR_FLAGS} -fexceptions")

# See https://github.com/ArashPartow/exprtk/pull/9
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	EXECUTE_PROCESS(COMMAND patch exprtk/exprtk.hpp exprtk.hpp.patch
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_QUIET)
ENDIF()

IF(LMMS_BUILD_WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj -Dexprtk_disable_enhanced_features")
ENDIF()

BUILD_PLUGIN(xpressive
	Xpressive.cpp
	ExprSynth.cpp
	Xpressive.h
	exprtk/exprtk.hpp
	MOCFILES Xpressive.h
	EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png"
)

