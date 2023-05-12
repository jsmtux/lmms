#include "GuiApplication.h"


namespace lmms
{


namespace gui
{

IGuiApplication* getGUIInterface()
{
	return GuiApplication::instance();
}


std::unique_ptr<GuiApplication*> GuiApplication::s_instance;

GuiApplication* GuiApplication::instance()
{
	return *s_instance.get();
}

}
}