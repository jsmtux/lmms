FILE(GLOB Sources *.cpp)

add_library(lmms_model_interface INTERFACE)
target_include_directories(lmms_model_interface INTERFACE interface)

add_library(lmms_model STATIC ${Sources})
target_include_directories(lmms_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_model PUBLIC lmms_memory lmms_journalling)