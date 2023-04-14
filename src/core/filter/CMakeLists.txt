FILE(GLOB Sources *.cpp)

add_library(lmms_filter STATIC ${Sources})
target_include_directories(lmms_filter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_filter PUBLIC lmms_base lmms_memory)