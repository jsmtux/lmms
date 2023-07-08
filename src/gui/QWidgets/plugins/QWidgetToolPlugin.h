#ifndef Q_WIDGET_TOOL_PLUGIN_H_
#define Q_WIDGET_TOOL_PLUGIN_H_

#include "ToolPluginView.h"
#include "ToolPlugin.h"

namespace lmms::gui {

class QWidgetToolPlugin: public ToolPlugin{
public:
	QWidgetToolPlugin( const Descriptor * _descriptor, Model * _parent ) :
		ToolPlugin(_descriptor, _parent)
	{}

	//! Create a view for the model
	ToolPluginView * createView( QWidget * parent )
	{
		ToolPluginView * pv = instantiateView( parent );
		return pv;        
	}

protected:
	//! Create a view for the model
	virtual ToolPluginView* instantiateView( QWidget * ) = 0;
};

}

#endif // Q_WIDGET_TO_PLUGIN_H_
