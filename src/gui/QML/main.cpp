/*
 * main.cpp - just main.cpp which is starting up app...
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2012-2013 Paul Giblock    <p/at/pgiblock.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include "lmmsconfig.h"
#include "lmmsversion.h"
#include "versioninfo.h"
#include "Engine.h"
#include "denormals.h"

#include "IPlugin.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include <QTextStream>
#include <QQmlContext>

#ifdef LMMS_BUILD_WIN32
#include <windows.h>
#endif

#ifdef LMMS_HAVE_SCHED_H
#include "sched.h"
#endif

#ifdef LMMS_HAVE_PROCESS_H
#include <process.h>
#endif

#ifdef LMMS_HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <csignal>

#include "ICoreApplication.h"
#include "OutputSettings.h"
#include "GuiApplication.h"

#include "DummyEffect.h"
#include "Instrument.h"
#include "DummyInstrument.h"



inline void loadTranslation( const QString & tname,
	const QString & dir)
{
	auto t = new QTranslator(QCoreApplication::instance());
	QString name = tname + ".qm";

	if (t->load(name, dir))
	{
		QCoreApplication::instance()->installTranslator(t);
	}
}

namespace lmms
{

IEffect* InstantiateDummyEffect(IEffectChain* _parent, const QDomElement& originalPluginData)
{
	return new DummyEffect(_parent, originalPluginData);
}


IInstrument* InstantiateDummyInstrument(IInstrumentTrack* _instrument_track)
{
	return new DummyInstrument(_instrument_track);
}
}

void SetRealTime()
{
	// try to set realtime priority
#if defined(LMMS_BUILD_LINUX) || defined(LMMS_BUILD_FREEBSD)
#ifdef LMMS_HAVE_SCHED_H
#ifndef __OpenBSD__
	struct sched_param sparam;
	sparam.sched_priority = ( sched_get_priority_max( SCHED_FIFO ) +
				sched_get_priority_min( SCHED_FIFO ) ) / 2;
	if( sched_setscheduler( 0, SCHED_FIFO, &sparam ) == -1 )
	{
		printf( "Notice: could not set realtime priority.\n" );
	}
#endif
#endif // LMMS_HAVE_SCHED_H
#endif

#ifdef LMMS_BUILD_WIN32
	if( !SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS ) )
	{
		printf( "Notice: could not set high priority.\n" );
	}
#endif

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_SIGINFO;
	if ( sigemptyset( &sa.sa_mask ) )
	{
		fprintf( stderr, "Signal initialization failed.\n" );
	}
	if ( sigaction( SIGPIPE, &sa, nullptr ) )
	{
		fprintf( stderr, "Signal initialization failed.\n" );
	}
#endif
}

int main( int argc, char * * argv )
{
	using namespace lmms;

	ICoreApplication* coreApplication = getCoreApplication();
	IConfigManager* configManager = coreApplication->getConfigManager();

	coreApplication->init();

	// intialize RNG
	srand( getpid() + time( 0 ) );

	disable_denormals();

	SetRealTime();

	QString renderOut, profilerOutputFile, configFile;

    QApplication app(argc, argv);

	IAudioEngine::qualitySettings qs( IAudioEngine::qualitySettings::Mode_HighQuality );
	OutputSettings os( 44100, OutputSettings::BitRateSettings(160, false), OutputSettings::Depth_16Bit, OutputSettings::StereoMode_JointStereo );
	IProjectRenderer::ExportFileFormats eff = IProjectRenderer::WaveFile;

	configManager->loadConfigFile(configFile);

	// Hidden settings
	coreApplication->setNaNHandler( configManager->value( "app",
						"nanhandler", "1" ).toInt() );

	// set language
	QString pos = configManager->value( "app", "language" );
	if( pos.isEmpty() )
	{
		pos = QLocale::system().name().left( 2 );
	}

	// load actual translation for LMMS
	loadTranslation( pos, configManager->localeDir() );

	// load translation for Qt-widgets/-dialogs
#ifdef QT_TRANSLATIONS_DIR
	// load from the original path first
	loadTranslation(QString("qt_") + pos, QT_TRANSLATIONS_DIR);
#endif
	// override it with bundled/custom one, if exists
	loadTranslation(QString("qt_") + pos, configManager->localeDir());

	Engine::init(false);

    QQmlApplicationEngine qCoreApplicationEngine;
    qCoreApplicationEngine.addImportPath(":/imports");

	auto qml_context = qCoreApplicationEngine.rootContext();

	lmms::gui::SongModel::RegisterInQml();
	lmms::gui::SongModel song_model(IEngine::Instance()->getSongInterface());
	qml_context->setContextProperty("songModel", &song_model);

    qCoreApplicationEngine.load(QUrl(QStringLiteral("qrc:/lmms2.qml")));

	IEngine::Instance()->getSongInterface()->setExportLoop( false );

	// create renderer
	auto renderer = createRenderManager(qs, os, eff, renderOut);
	QCoreApplication::instance()->connect( renderer.get(),
			SIGNAL(finished()), SLOT(quit()));

	// timer for progress-updates
	auto t = new QTimer(renderer.get());
	renderer->connect( t, SIGNAL(timeout()),
			SLOT(updateConsoleProgress()));
	t->start( 200 );

	if( profilerOutputFile.isEmpty() == false )
	{
		IEngine::Instance()->getAudioEngineInterface()->setProfilerOutputFile( profilerOutputFile );
	}

	lmms::gui::GuiApplication guiApplication;

	IEngine::Instance()->getSongInterface()->loadProject("../data/projects/shorties/sv-DnB-Startup.mmpz");
	
	IConfigManager::Instance()->setWorkingDir("../data");
	if ( !IConfigManager::Instance()->hasWorkingDir() )
	{
		IConfigManager::Instance()->createWorkingDir();
	}
	qWarning() << IConfigManager::Instance()->workingDir() << Qt::endl;

	const int ret = app.exec();

	coreApplication->free();

	return ret;
}