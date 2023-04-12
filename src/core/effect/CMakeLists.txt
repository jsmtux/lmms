FILE(GLOB Sources *.cpp)

add_library(lmms_effect STATIC ${Sources})
target_include_directories(lmms_effect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_effect PUBLIC lmms_serialization)