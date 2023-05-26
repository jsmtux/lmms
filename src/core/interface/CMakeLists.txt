ADD_LIBRARY(lmms_core_interface STATIC IRenderManager.cpp Model.cpp JournallingObject.cpp)

TARGET_INCLUDE_DIRECTORIES(lmms_core_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src/core)