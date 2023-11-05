#pragma once

#include "IPlugin.h"

namespace lmms
{

namespace gui {
class PluginView;
} // namespace gui

class IQWidgetInstrumentPlugin : public IGUISpecificPlugin
{
public:
    virtual ~IQWidgetInstrumentPlugin() = default;
    virtual gui::PluginView * createView( QWidget * parent ) = 0;
};

} // namespace lmms