FILE(GLOB Sources *.cpp)

add_library(lmms_track_interface INTERFACE)
target_include_directories(lmms_track_interface INTERFACE interface)

add_library(lmms_track STATIC ${Sources})
target_include_directories(lmms_track PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_track PUBLIC lmms_base lmms_model lmms_automation)