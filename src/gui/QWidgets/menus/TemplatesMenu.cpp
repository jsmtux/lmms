#include "TemplatesMenu.h"

#include "IConfigManager.h"
#include "embed.h"
#include "GuiApplication.h"
#include "IEngine.h"
#include "MainWindow.h"
#include "ISong.h"

namespace lmms::gui
{


TemplatesMenu::TemplatesMenu(QWidget *parent) :
	QMenu(tr("New from template"), parent)
{
	setIcon(embed::getIconPixmap("project_new"));

	connect( this, SIGNAL(aboutToShow()), SLOT(fillTemplatesMenu()));
	connect( this, SIGNAL(triggered(QAction*)),
		SLOT(createNewProjectFromTemplate(QAction*)));
}




void TemplatesMenu::createNewProjectFromTemplate(QAction * _action)
{
	if( getGUI()->mainWindow()->mayChangeProject(true) )
	{
		const QString& templateFilePath = _action->data().toString();
		IEngine::Instance()->getSongInterface()->createNewProjectFromTemplate(templateFilePath);
	}
}





void TemplatesMenu::fillTemplatesMenu()
{
	clear();

	addTemplatesFromDir(IConfigManager::Instance()->userTemplateDir());
	addTemplatesFromDir(IConfigManager::Instance()->factoryProjectsDir() + "templates");
}




void TemplatesMenu::addTemplatesFromDir( const QDir& dir ) {
	QFileInfoList templates = dir.entryInfoList( QStringList( "*.mpt" ),
		QDir::Files | QDir::Readable );

	if (!templates.empty() && !actions().isEmpty())
	{
		addSeparator();
	}

	auto projectFileIcon = embed::getIconPixmap( "project_file" );

	for(const QFileInfo& templateFile : templates)
	{
		auto action = addAction(projectFileIcon,
			templateFile.completeBaseName().replace("&", "&&"));
		action->setData(templateFile.absoluteFilePath());
#ifdef LMMS_BUILD_APPLE
		action->setIconVisibleInMenu(false); // QTBUG-44565 workaround
		action->setIconVisibleInMenu(true);
#endif
	}
}


} // namespace lmms::gui
