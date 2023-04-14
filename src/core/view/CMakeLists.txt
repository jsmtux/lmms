FILE(GLOB Sources *.cpp)

add_library(lmms_view STATIC ${Sources})
target_include_directories(lmms_view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_view PUBLIC lmms_journalling lmms_memory lmms_model)