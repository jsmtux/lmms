#ifndef Q_WIDGET_TOOL_PLUGIN_H_
#define Q_WIDGET_TOOL_PLUGIN_H_

#include "ToolPlugin.h"
#include "PluginView.h"

namespace lmms::gui {

class QWidgetToolPlugin: public ToolPlugin{
public:
    QWidgetToolPlugin( const Descriptor * _descriptor, Model * _parent ) :
        ToolPlugin(_descriptor, _parent)
    {}
    //! Create a view for the model
	PluginView * createView( QWidget * parent ) {
        gui::PluginView * pv = instantiateView( parent );
        if( pv != nullptr )
        {
            pv->setModel( this );
        }
        return pv;        
    }

protected:
	//! Create a view for the model
	virtual gui::PluginView* instantiateView( QWidget * ) = 0;
};

}

#endif // Q_WIDGET_TO_PLUGIN_H_