add_library(lmms_scale STATIC Scale.cpp)
target_include_directories(lmms_scale PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lmms_scale PUBLIC lmms_serialization)