FILE(GLOB Sources *.cpp)

add_library(lmms_note STATIC ${Sources})
target_include_directories(lmms_note PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_note PUBLIC lmms_base lmms_model lmms_track_interface)