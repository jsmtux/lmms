TEMPLATE = app
TARGET = lmms2

QT += quick quickcontrols2 widgets

SOURCES += main.cpp

RESOURCES += \
    qtquickcontrols2.conf \
    lmms2.qml \
    imports/Flat/Button.qml \
    imports/Flat/HeaderButton.qml \
    imports/Flat/StatusButton.qml \
    imports/Flat/CheckBox.qml \
    imports/Flat/qmldir \
    imports/Flat/Switch.qml \
    imports/Flat/TabBar.qml \
    imports/Flat/TabButton.qml \
    imports/Theme/Theme.qml \
    imports/Theme/qmldir \
    icons/big_button/save.png \
    icons/big_button/help.png \
    icons/status_button/play.png \
    icons/status_button/play_record.png \
    icons/status_button/record.png \
    icons/status_button/stop.png \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/imports

target.path = $$[QT_INSTALL_EXAMPLES]/quickcontrols/flatstyle
INSTALLS += target
