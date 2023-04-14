FILE(GLOB Sources *.cpp)

add_library(lmms_config STATIC ${Sources})
target_include_directories(lmms_config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_config PUBLIC lmms_base lmms_model)