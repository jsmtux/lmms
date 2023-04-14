FILE(GLOB Sources *.cpp)

add_library(lmms_journalling STATIC ${Sources})
target_include_directories(lmms_journalling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_journalling PUBLIC lmms_base lmms_journaling_interface)