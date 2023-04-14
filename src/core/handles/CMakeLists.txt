FILE(GLOB Sources *.cpp)

add_library(lmms_handles STATIC ${Sources})
target_include_directories(lmms_handles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_handles PUBLIC lmms_base lmms_memory lmms_instrument_interface lmms_filter)