FILE(GLOB Sources *.cpp)

add_library(lmms_engine STATIC ${Sources})
target_include_directories(lmms_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_engine PUBLIC lmms_base lmms_memory lmms_handles)