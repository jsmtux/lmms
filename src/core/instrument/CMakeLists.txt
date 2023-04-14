FILE(GLOB Sources *.cpp)

add_library(lmms_instrument_interface INTERFACE)
target_include_directories(lmms_instrument_interface INTERFACE interface)

add_library(lmms_instrument STATIC ${Sources})
target_include_directories(lmms_instrument PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_instrument PUBLIC lmms_track_interface lmms_instrument_interface lmms_base lmms_memory lmms_plugin)