qt_add_library(lmms2widgets STATIC)

qt_add_qml_module(lmms2widgets
    URI "lmms2widgets"
    VERSION 1.0
    QML_FILES
        TaggedDial.qml
        Dial.qml
    RESOURCES
        dial.png
    RESOURCE_PREFIX /
)
