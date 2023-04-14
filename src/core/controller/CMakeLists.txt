FILE(GLOB Sources *.cpp)

add_library(lmms_controller STATIC ${Sources})
target_include_directories(lmms_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_controller PUBLIC lmms_base lmms_model)