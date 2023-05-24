

class DummyEffect : public Effect
{
public:
	DummyEffect( Model * _parent, const QDomElement& originalPluginData ) :
		Effect( nullptr, _parent, nullptr )
	{ }

	EffectControls * controls() override
	{
		return nullptr;
	}

	bool processAudioBuffer( sampleFrame *, const fpp_t ) override
	{
		return false;
	}
};

class DummyInstrument : public Instrument
{
public:
	DummyInstrument( InstrumentTrack * _instrument_track ) :
		Instrument( _instrument_track, nullptr )
	{
	}

	~DummyInstrument() override = default;

	void playNote( NotePlayHandle *, sampleFrame * buffer ) override
	{
		memset( buffer, 0, sizeof( sampleFrame ) *
			getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->framesPerPeriod() );
	}

	void saveSettings( QDomDocument &, QDomElement & ) override
	{
	}

	void loadSettings( const QDomElement & ) override
	{
	}

	QString nodeName() const override
	{
		return "dummyinstrument";
	}
};