FILE(GLOB Sources *.cpp)

add_library(lmms_base STATIC ${Sources})
target_include_directories(lmms_base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src)