if(LMMS_HAVE_CARLA OR LMMS_HAVE_WEAKCARLA)
	ADD_DEFINITIONS(-DCARLA_PLUGIN_RACK -DCARLA_PLUGIN_SYNTH)
	INCLUDE(BuildPlugin)
	include_directories(SYSTEM ${CARLA_INCLUDE_DIRS})
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../CarlaBase")
	LINK_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/../CarlaBase"
                          ${CARLA_LIBRARY_DIRS})
	LINK_LIBRARIES(carlabase)
	BUILD_PLUGIN(carlarack CarlaRack.cpp EMBEDDED_RESOURCES logo.png)
endif()
