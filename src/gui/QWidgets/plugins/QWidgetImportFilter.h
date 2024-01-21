#ifndef LMMS_GUI_QWIDGET_IMPORT_FILTER_H_
#define LMMS_GUI_QWIDGET_IMPORT_FILTER_H_

#include "PluginView.h"
#include "ImportFilter.h"

namespace lmms {

class QWidgetImportFilter : public ImportFilter {
public:
    QWidgetImportFilter( const QString & _file_name,
					const Descriptor * _descriptor )
        : ImportFilter(_file_name, _descriptor) {}
protected:
    virtual gui::PluginView* instantiateView( QWidget * ) = 0;
};

}


#endif /* LMMS_GUI_QWIDGET_IMPORT_FILTER_H_ */
