#include "GuiApplication.h"


namespace lmms
{


namespace gui
{

IGuiApplication* getGUIInterface()
{
	return GuiApplication::instance();
}


std::unique_ptr<GuiApplication> GuiApplication::s_instance;

GuiApplication::GuiApplication() {}

GuiApplication* GuiApplication::instance()
{
	if (!s_instance) {
		s_instance = std::make_unique<GuiApplication>();
	}
	return s_instance.get();
}

}
}