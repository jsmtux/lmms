#ifndef LMMS_UI_QWIDGET_EXPORT_FILTER_H_
#define LMMS_UI_QWIDGET_EXPORT_FILTER_H_

#include "ExportFilter.h"

namespace lmms {

class QWidgetExportFilter : public ExportFilter {
public:
    QWidgetExportFilter( const Descriptor * _descriptor ) : ExportFilter( _descriptor ) {}
    virtual gui::PluginView* instantiateView(QWidget *) = 0;
};

}

#endif /* LMMS_UI_QWIDGET_EXPORT_FILTER_H_ */
