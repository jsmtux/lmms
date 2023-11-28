INCLUDE(BuildPlugin)

BUILD_PLUGIN(
    tripleoscillator
    ../core/TripleOscillator.cpp
    ../core/TripleOscillator.h
    QWidgetTripleOscillator.h
    QWidgetTripleOscillator.cpp
MOCFILES
    ../core/TripleOscillator.h
    ../core/QWidgetTripleOscillator.h
EMBEDDED_RESOURCES
    ../core/*.png)
