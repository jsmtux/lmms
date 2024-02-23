qt_add_library(lmms_widgets STATIC)

qt_add_qml_module(lmms_widgets
    URI LmmsWidgets
    VERSION 1.0
    QML_FILES
        TaggedDial.qml
        LmmsDial.qml
    RESOURCE_PREFIX "/qt/qml/"
)
