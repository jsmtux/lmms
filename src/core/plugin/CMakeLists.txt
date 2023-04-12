FILE(GLOB Sources *.cpp)

add_library(lmms_plugin STATIC ${Sources})
target_include_directories(lmms_plugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_plugin PUBLIC lmms_serialization lmms_memory)