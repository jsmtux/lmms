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

#ifdef LMMS_DEBUG_FPE
#include <execinfo.h> // For backtrace and backtrace_symbols_fd
#include <unistd.h> // For STDERR_FILENO
#include <csignal> // To register the signal handler
#endif


#ifdef LMMS_DEBUG_FPE
void signalHandler( int signum ) {

	// Get a back trace
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	backtrace_symbols_fd(array, size, STDERR_FILENO);

	// cleanup and close up stuff here
	// terminate program

	exit(signum);
}
#endif


#ifdef LMMS_BUILD_WIN32
// Workaround for old MinGW
#ifdef __MINGW32__
extern "C" _CRTIMP errno_t __cdecl freopen_s(FILE** _File,
	const char *_Filename, const char *_Mode, FILE *_Stream);
#endif

// For qInstallMessageHandler
void consoleMessageHandler(QtMsgType type,
	const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData());
}
#endif // LMMS_BUILD_WIN32


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

void printVersion( char *executableName )
{
	printf( "LMMS %s\n(%s %s, Qt %s, %s)\n\n"
		"Copyright (c) %s\n\n"
		"This program is free software; you can redistribute it and/or\n"
		"modify it under the terms of the GNU General Public\n"
		"License as published by the Free Software Foundation; either\n"
		"version 2 of the License, or (at your option) any later version.\n\n"
		"Try \"%s --help\" for more information.\n\n", LMMS_VERSION,
		LMMS_BUILDCONF_PLATFORM, LMMS_BUILDCONF_MACHINE, QT_VERSION_STR, LMMS_BUILDCONF_COMPILER_VERSION,
		LMMS_PROJECT_COPYRIGHT, executableName );
}




void printHelp()
{
	printf( "LMMS %s\n"
		"Copyright (c) %s\n\n"
		"Usage: lmms [global options...] [<action> [action parameters...]]\n\n"
		"Actions:\n"
		"  <no action> [options...] [<project>]  Start LMMS in normal GUI mode\n"
		"  dump <in>                             Dump XML of compressed file <in>\n"
		"  compress <in>                         Compress file <in>\n"
		"  render <project> [options...]         Render given project file\n"
		"  rendertracks <project> [options...]   Render each track to a different file\n"
		"  upgrade <in> [out]                    Upgrade file <in> and save as <out>\n"
		"                                        Standard out is used if no output file\n"
		"                                        is specified\n"
		"  makebundle <in> [out]                 Make a project bundle from the project\n"
		"                                        file <in> saving the resulting bundle\n"
		"                                        as <out>\n"
		"\nGlobal options:\n"
		"      --allowroot                Bypass root user startup check (use with\n"
		"          caution).\n"
		"  -c, --config <configfile>      Get the configuration from <configfile>\n"
		"  -h, --help                     Show this usage information and exit.\n"
		"  -v, --version                  Show version information and exit.\n"
		"\nOptions if no action is given:\n"
		"      --geometry <geometry>      Specify the size and position of\n"
		"          the main window\n"
		"          geometry is <xsizexysize+xoffset+yoffsety>.\n"
		"      --import <in> [-e]         Import MIDI or Hydrogen file <in>.\n"
		"          If -e is specified lmms exits after importing the file.\n"
		"\nOptions for \"render\" and \"rendertracks\":\n"
		"  -a, --float                    Use 32bit float bit depth\n"
		"  -b, --bitrate <bitrate>        Specify output bitrate in KBit/s\n"
		"          Default: 160.\n"
		"  -f, --format <format>         Specify format of render-output where\n"
		"          Format is either 'wav', 'flac', 'ogg' or 'mp3'.\n"
		"  -i, --interpolation <method>   Specify interpolation method\n"
		"          Possible values:\n"
		"            - linear\n"
		"            - sincfastest (default)\n"
		"            - sincmedium\n"
		"            - sincbest\n"
		"  -l, --loop                     Render as a loop\n"
		"  -m, --mode                     Stereo mode used for MP3 export\n"
		"          Possible values: s, j, m\n"
		"            s: Stereo\n"
		"            j: Joint Stereo\n"
		"            m: Mono\n"
		"          Default: j\n"
		"  -o, --output <path>            Render into <path>\n"
		"          For \"render\", provide a file path\n"
		"          For \"rendertracks\", provide a directory path\n"
		"          If not specified, render will overwrite the input file\n"
		"          For \"rendertracks\", this might be required\n"
		"  -p, --profile <out>            Dump profiling information to file <out>\n"
		"  -s, --samplerate <samplerate>  Specify output samplerate in Hz\n"
		"          Range: 44100 (default) to 192000\n"
		"  -x, --oversampling <value>     Specify oversampling\n"
		"          Possible values: 1, 2, 4, 8\n"
		"          Default: 2\n\n",
		LMMS_VERSION, LMMS_PROJECT_COPYRIGHT );
}




void fileCheck( QString &file )
{
	QFileInfo fileToCheck( file );

	if( !fileToCheck.size() )
	{
		printf( "The file %s does not have any content.\n",
				file.toUtf8().constData() );
		exit( EXIT_FAILURE );
	}
	else if( fileToCheck.isDir() )
	{
		printf( "%s is a directory.\n",
				file.toUtf8().constData() );
		exit( EXIT_FAILURE );
	}
}

int usageError(const QString& message)
{
	qCritical().noquote() << QString("\n%1.\n\nTry \"%2 --help\" for more information.\n\n")
				   .arg( message ).arg( qApp->arguments()[0] );
	return EXIT_FAILURE;
}

int noInputFileError()
{
	return usageError( "No input file specified" );
}

int main( int argc, char * * argv )
{
	using namespace lmms;

#ifdef LMMS_DEBUG_FPE
	// Enable exceptions for certain floating point results
	// FE_UNDERFLOW is disabled for the time being
	feenableexcept( FE_INVALID   |
			FE_DIVBYZERO |
			FE_OVERFLOW  /*|
			FE_UNDERFLOW*/);

	// Install the trap handler
	// register signal SIGFPE and signal handler
	signal(SIGFPE, signalHandler);
#endif

#ifdef LMMS_BUILD_WIN32
	// Don't touch redirected streams here
	// GetStdHandle should be called before AttachConsole
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
	FILE *fIn, *fOut, *fErr;
	// Enable console output if available
	if (AttachConsole(ATTACH_PARENT_PROCESS))
	{
		if (!hStdIn)
		{
			freopen_s(&fIn, "CONIN$", "r", stdin);
		}
		if (!hStdOut)
		{
			freopen_s(&fOut, "CONOUT$", "w", stdout);
		}
		if (!hStdErr)
		{
			freopen_s(&fErr, "CONOUT$", "w", stderr);
		}
	}
	// Make Qt's debug message handlers work
	qInstallMessageHandler(consoleMessageHandler);
#endif
	ICoreApplication* coreApplication = getCoreApplication();
	IConfigManager* configManager = coreApplication->getConfigManager();

	coreApplication->init();

	// intialize RNG
	srand( getpid() + time( 0 ) );

	disable_denormals();

	bool coreOnly = false;
	bool allowRoot = false;
	bool renderLoop = false;
	// bool renderTracks = false;
	QString fileToLoad, fileToImport, renderOut, profilerOutputFile, configFile;

	// first of two command-line parsing stages
	for( int i = 1; i < argc; ++i )
	{
		QString arg = argv[i];

		if( arg == "--help"    || arg == "-h" ||
		    arg == "--version" || arg == "-v" ||
		    arg == "render" || arg == "--render" || arg == "-r" )
		{
			coreOnly = true;
		}
		else if( arg == "rendertracks" || arg == "--rendertracks" )
		{
			coreOnly = true;
			// renderTracks = true;
		}
		else if( arg == "--allowroot" )
		{
			allowRoot = true;
		}
		else if( arg == "--geometry" || arg == "-geometry")
		{
			if( arg == "--geometry" )
			{
				// Delete the first "-" so Qt recognize the option
				strcpy(argv[i], "-geometry");
			}
		}
	}

#if !defined(LMMS_BUILD_WIN32) && !defined(LMMS_BUILD_HAIKU)
	if ( ( getuid() == 0 || geteuid() == 0 ) && !allowRoot )
	{
		printf( "LMMS cannot be run as root.\nUse \"--allowroot\" to override.\n\n" );
		return EXIT_FAILURE;
	}
#endif

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
	// auto& engine = coreApplication->createEngine(false);

	// qDebug() << "Loading projec!t... %s\n" <<  fileToLoad << "\n";
	// IEngine::Instance()->getSongInterface()->loadProject( fileToLoad );
	// if( IEngine::Instance()->getSongInterface()->trackContainerInterface()->isEmpty() )
	// {
	// 	printf("The project %s is empty, aborting!\n", fileToLoad.toUtf8().constData() );
	// 	exit( EXIT_FAILURE );
	// }
	// printf( "Done\n" );

	Engine::init(false);

    QQmlApplicationEngine qCoreApplicationEngine;
    qCoreApplicationEngine.addImportPath(":/imports");

	auto qml_context = qCoreApplicationEngine.rootContext();

	lmms::gui::SongModel::RegisterInQml();
	lmms::gui::SongModel song_model(IEngine::Instance()->getSongInterface());
	qml_context->setContextProperty("songModel", &song_model);

    qCoreApplicationEngine.load(QUrl(QStringLiteral("qrc:/lmms2.qml")));

	IEngine::Instance()->getSongInterface()->setExportLoop( renderLoop );

	// when rendering multiple tracks, renderOut is a directory
	// otherwise, it is a file, so we need to append the file extension
	// if ( !renderTracks )
	// {
	// 	renderOut = baseName( renderOut ) +
	// 		ProjectRenderer::getFileExtensionFromFormat(eff);
	// }


	// create renderer
	auto r = createRenderManager(qs, os, eff, renderOut);
	QCoreApplication::instance()->connect( r.get(),
			SIGNAL(finished()), SLOT(quit()));

	// timer for progress-updates
	auto t = new QTimer(r.get());
	r->connect( t, SIGNAL(timeout()),
			SLOT(updateConsoleProgress()));
	t->start( 200 );

	if( profilerOutputFile.isEmpty() == false )
	{
		IEngine::Instance()->getAudioEngineInterface()->setProfilerOutputFile( profilerOutputFile );
	}

	lmms::gui::GuiApplication guiApplication;

	// If no recovery file, no macOS request, not last open project, then:
	// IEngine::Instance()->getSongInterface()->createNewProject();
	// IEngine::Instance()->getSongInterface()->loadProject(configManager->factoryProjectsDir() + "/shorties/sv-Trance-Startup.mmpz");
	qDebug() << "Will run stuff\n";
	IEngine::Instance()->getSongInterface()->loadProject("../data/projects/shorties/sv-DnB-Startup.mmpz");

	const int ret = app.exec();

	// ProjectRenderer::updateConsoleProgress() doesn't return line after render
	if( coreOnly )
	{
		printf( "\n" );
	}

#ifdef LMMS_BUILD_WIN32
	// Cleanup console
	HWND hConsole = GetConsoleWindow();
	if (hConsole)
	{
		SendMessage(hConsole, WM_CHAR, (WPARAM)VK_RETURN, (LPARAM)0);
		FreeConsole();
	}
#endif

	coreApplication->free();

	return ret;
}