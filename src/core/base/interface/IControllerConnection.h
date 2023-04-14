#ifndef EFDD5039_BB52_4A64_B2DE_A50BFF44E0D6
#define EFDD5039_BB52_4A64_B2DE_A50BFF44E0D6

namespace lmms {

class IController {
public:
	enum ControllerTypes
	{
		DummyController,
		LfoController,
		MidiController,
		PeakController,
		/*
		XYController,
		EquationController
		*/
		NumControllerTypes
	} ;

    virtual ~IController() {};
    virtual ControllerTypes type() const = 0;
	virtual bool isSampleExact() const = 0;
};


class IControllerConnection : public QObject, public JournallingObject{
	Q_OBJECT
public:
	static IControllerConnection* createNew(IController* _controller);
    virtual ~IControllerConnection() {};
    virtual IController * getController() = 0;
	virtual void saveSettings( QDomDocument& doc, QDomElement& element ) = 0;
	virtual void loadSettings( const QDomElement& element ) = 0;
	virtual float currentValue( int _offset ) = 0;
	virtual ValueBuffer * valueBuffer() = 0;
};

}

#endif /* EFDD5039_BB52_4A64_B2DE_A50BFF44E0D6 */
