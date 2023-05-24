#pragma once

#include "IAudioDevice.h"
#include "IConfigManager.h"

namespace lmms
{

class IMidiClient {
public:
    virtual bool isRaw() const = 0;
    virtual QStringList readablePorts() const = 0;
};

class IMidiJack {
public:
	static QString probeDevice()
    {
        QString jid = IConfigManager::Instance()->value( "midijack", "lmms" );
        if( jid.isEmpty() )
        {
            return "lmms";
        }
        return jid;
    }


	inline static QString name()
	{
		return MidiJackName();
	}

	inline static QString configSection()
	{
		return "MidiJack";
	}
};

class IMidiApple {
public:
	inline static QString probeDevice()
	{
		return QString(); // no midi device name
	}

	inline static QString name()
	{
		return QT_TRANSLATE_NOOP( "MidiSetupWidget", "Apple MIDI" );
	}
	inline static QString configSection()
	{
		return QString(); // no configuration settings
	}
};

class IMidiDummy {
public:
	inline static QString name()
	{
		return( QT_TRANSLATE_NOOP( "MidiSetupWidget",
			"Dummy (no MIDI support)" ) );
	}

	inline static QString probeDevice()
	{
		return QString(); // no midi device name
	}

	inline static QString configSection()
	{
		return QString(); // no configuration settings
	}
};

} // namespace lmms
