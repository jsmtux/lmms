ADD_DEFINITIONS(-DFFTW3)
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    ${FFTW3F_INCLUDE_DIRS}
                    "${CMAKE_BINARY_DIR}")
LINK_DIRECTORIES(${FFTW3F_LIBRARY_DIRS})
LINK_LIBRARIES(-lfftw3f)
FILE(GLOB PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

FOREACH(_item ${PLUGIN_SOURCES})
	GET_FILENAME_COMPONENT(_plugin "${_item}" NAME_WE)

	ADD_LIBRARY("${_plugin}" MODULE "${_item}")
	INSTALL(TARGETS "${_plugin}" LIBRARY DESTINATION "${PLUGIN_DIR}/ladspa")
	SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES PREFIX "")
	SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES COMPILE_FLAGS "-O3 -Wall -fomit-frame-pointer -fstrength-reduce -funroll-loops -ffast-math -c -fno-strict-aliasing")
	IF(LMMS_BUILD_WIN32)
		ADD_CUSTOM_COMMAND(TARGET "${_plugin}" POST_BUILD COMMAND "${STRIP}" "\"${CMAKE_CURRENT_BINARY_DIR}/${_plugin}.dll\"")
	ELSE(LMMS_BUILD_WIN32)
		SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC -DPIC")
	ENDIF(LMMS_BUILD_WIN32)
	IF(LMMS_BUILD_APPLE)
		SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES LINK_FLAGS "${LINK_FLAGS} -Bsymbolic -lm")
	ELSE(LMMS_BUILD_APPLE)
		SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES LINK_FLAGS "${LINK_FLAGS} -shared -Wl,-no-undefined -Wl,-Bsymbolic -lm")
	ENDIF(LMMS_BUILD_APPLE)
	IF(LMMS_BUILD_LINUX)
		SET_TARGET_PROPERTIES("${_plugin}" PROPERTIES LINK_FLAGS "${LINK_FLAGS} -nostartfiles")
	ENDIF(LMMS_BUILD_LINUX)
ENDFOREACH(_item ${PLUGIN_SOURCES})


IF(LMMS_BUILD_WIN32)
	SET(PIC_FLAGS "")
ELSE(LMMS_BUILD_WIN32)
	SET(PIC_FLAGS "-fPIC")
ENDIF(LMMS_BUILD_WIN32)

ADD_LIBRARY(iir STATIC util/iir.c)
SET_TARGET_PROPERTIES(iir PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(bandpass_a_iir_1893 iir)
TARGET_LINK_LIBRARIES(bandpass_iir_1892 iir)
TARGET_LINK_LIBRARIES(butterworth_1902 iir)
TARGET_LINK_LIBRARIES(highpass_iir_1890 iir)
TARGET_LINK_LIBRARIES(lowpass_iir_1891 iir)
TARGET_LINK_LIBRARIES(notch_iir_1894 iir)

FILE(GLOB GSM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gsm/*.c")
ADD_LIBRARY(gsm STATIC ${GSM_SOURCES})
SET_TARGET_PROPERTIES(gsm PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(gsm_1215 gsm)

ADD_LIBRARY(gverb STATIC gverb/gverb.c gverb/gverbdsp.c)
SET_TARGET_PROPERTIES(gverb PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(gverb_1216 gverb)

ADD_LIBRARY(blo STATIC util/blo.c)
SET_TARGET_PROPERTIES(blo PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(hermes_filter_1200 blo)

ADD_LIBRARY(rms STATIC util/rms.c)
ADD_LIBRARY(db STATIC util/db.c)
SET_TARGET_PROPERTIES(rms PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
SET_TARGET_PROPERTIES(db PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(sc1_1425 rms db)
TARGET_LINK_LIBRARIES(sc2_1426 rms db)
TARGET_LINK_LIBRARIES(sc3_1427 rms db)
TARGET_LINK_LIBRARIES(sc4_1882 rms db)
TARGET_LINK_LIBRARIES(sc4m_1916 rms db)
TARGET_LINK_LIBRARIES(se4_1883 rms db)

ADD_LIBRARY(pitchscale STATIC util/pitchscale.c)
SET_TARGET_PROPERTIES(pitchscale PROPERTIES COMPILE_FLAGS "${PIC_FLAGS}")
TARGET_LINK_LIBRARIES(pitch_scale_1193 pitchscale)
TARGET_LINK_LIBRARIES(pitch_scale_1194 pitchscale)