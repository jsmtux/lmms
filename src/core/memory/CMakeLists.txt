add_library(lmms_memory STATIC BufferManager.cpp)
target_include_directories(lmms_memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_memory PUBLIC lmms_base)