INCLUDE(BuildPlugin)

BUILD_PLUGIN(kicker KickerGui.cpp KickerGui.h MOCFILES KickerGui.h EMBEDDED_RESOURCES artwork.png logo.png)

TARGET_LINK_LIBRARIES(kicker kicker_core)