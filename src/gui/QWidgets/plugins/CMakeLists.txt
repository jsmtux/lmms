set(QWIDGET_PLUGIN_SRCS
    QWidgetsDummyEffect.cpp
    QWidgetsDummyEffect.h
    QWidgetDummyInstrument.cpp
    QWidgetDummyInstrument.h
    DummyPlugin.h
    QWidgetExportFilter.h
    QWidgetImportFilter.h
    QWidgetInstrumentPlugin.h
    QWidgetToolPlugin.h
    interface
)

add_library(lmms_qwidget_plugins ${QWIDGET_PLUGIN_SRCS})

target_link_libraries(lmms_qwidget_plugins PRIVATE lmms_core_plugins lmms_gui)