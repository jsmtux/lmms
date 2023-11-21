INCLUDE(BuildPlugin)

BUILD_PLUGIN(
    tripleoscillator
    TripleOscillator.cpp
    TripleOscillator.h
    QWidgetTripleOscillator.h
    QWidgetTripleOscillator.cpp
MOCFILES
    TripleOscillator.h
    QWidgetTripleOscillator.h
EMBEDDED_RESOURCES
    *.png)
