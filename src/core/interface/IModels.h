#ifndef LMMS_I_MODELS_H
#define LMMS_I_MODELS_H

#include <QDomDocument>
#include <QDomElement>

#include <memory>

#include "embed.h"
#include "Model.h"
#include "lmms_basics.h"

namespace lmms
{

class IControllerConnection;

class IAutomatableModelBase {
public:
    virtual ~IAutomatableModelBase() = default;
	virtual void saveSettings( QDomDocument& doc, QDomElement& element, const QString& name ) = 0;
    virtual void loadSettings( const QDomElement& element, const QString& name ) = 0;
    virtual void setJournalling( const bool _sr ) = 0;
    virtual Model* model() = 0;
    virtual const Model* model() const = 0;
    virtual jo_id_t id() const = 0;
    virtual void reset() = 0;
    virtual IControllerConnection* controllerConnection() const = 0;
    virtual void setControllerConnection(std::unique_ptr<IControllerConnection>&& connection) = 0;
    virtual bool hasLinkedModels() const = 0;
    virtual void unlinkAllModels() = 0;
    virtual float inverseScaledValue( float value ) const = 0;
    virtual void setUseControllerValue(bool b = true) = 0;
    virtual void setAutomatedValue( const float value ) = 0;
    virtual bool useControllerValue() = 0;

    virtual float valueAsFloat( int frameOffset = 0 ) const = 0;
    virtual float initValueAsFloat() const = 0;
    virtual void setValueAsFloat( const float value ) = 0;
	virtual float minValueAsFloat() const = 0;
	virtual float maxValueAsFloat() const = 0;
    virtual void setRangeAsFloat( const float min, const float max, const float step = 1 ) = 0;
    virtual float stepAsFloat() const = 0;
	virtual QString displayValue( const float val ) const = 0;
    virtual float scaledValue( float value ) const = 0;
    virtual void incValue( int steps ) = 0;
    virtual void addJournalCheckPoint() = 0;
    virtual void saveJournallingState( const bool newState ) = 0;
    virtual void restoreJournallingState() = 0;
    virtual float centerValue() const = 0;
    virtual bool isScaleLogarithmic() const = 0;
    virtual void setScaleLogarithmic( bool setToTrue = true ) = 0;
    virtual float range() const = 0;

	//! link @p m1 and @p m2, let @p m1 take the values of @p m2
	static void linkModelInterfaces( IAutomatableModelBase* m1, IAutomatableModelBase* m2 );
	static void unlinkModelInterfacses( IAutomatableModelBase* m1, IAutomatableModelBase* m2 );
};

class JournallingObject;
IAutomatableModelBase* getAutomatableModelFromJournallingObject(JournallingObject*);

template<typename T> class IAutomatableModel : public IAutomatableModelBase
{
public:
    virtual ~IAutomatableModel() = default;
    virtual T value( int frameOffset = 0 ) const = 0;
    virtual T initValue() const = 0;
    virtual void setValue( const T value ) = 0;
	virtual T minValue() const = 0;
	virtual T maxValue() const = 0;
    virtual void setRange( const T min, const T max, const T step = 1 ) = 0;
    virtual void setInitValue( const T value ) = 0;
    virtual T step() const = 0;


    float getRoundedValue() const { return qRound( valueAsFloat() / step() ) * step(); }
    int getDigitCount() const {
        auto steptemp = step();
        int digits = 0;
        while ( steptemp < 1 )
        {
            steptemp = steptemp * 10.0f;
            digits++;
        }
        return digits;
    }
private:
    float valueAsFloat( int frameOffset = 0 ) const override {
        return static_cast<float>(value(frameOffset));
    }
    float initValueAsFloat() const override {
        return static_cast<float>(initValue());
    }
    void setValueAsFloat( const float value ) override {
        setValue(static_cast<float>(value));
    }
	float minValueAsFloat() const override {
        return static_cast<float>(minValue());
    }
	float maxValueAsFloat() const override {
        return static_cast<float>(maxValue());
    }
    void setRangeAsFloat( const float min, const float max, const float step = 1 ) override {
        setRange(static_cast<float>(min), static_cast<float>(max), static_cast<float>(step));
    }
};

using IFloatAutomatableModel = IAutomatableModel<float>;
using IIntAutomatableModel = IAutomatableModel<int>;
using IBoolAutomatableModel = IAutomatableModel<bool>;

class ValueBuffer;

class IComboBoxModelWrapper {
public:
    virtual ~IComboBoxModelWrapper() = default;
    virtual IIntAutomatableModel* wrappedModel() = 0;
    virtual QString currentText() const = 0;
    virtual void addItem( QString item, std::unique_ptr<PixmapLoader> loader = nullptr ) = 0;
    virtual void replaceItem(std::size_t index, QString item, std::unique_ptr<PixmapLoader> loader = nullptr) = 0;
    virtual void clear() = 0;
    virtual void setValue(int value) = 0;
    virtual int findText( const QString& txt ) const = 0;
    virtual int size() const = 0;
    virtual const PixmapLoader* itemPixmap( int i ) const = 0;
    virtual const QString & itemText( int i ) const = 0;
    virtual const PixmapLoader* currentData() const = 0;
};

class IMeterModel;

class TempoSyncKnobSignals : public QObject
{
    Q_OBJECT
signals:
	void syncModeChanged();
	void scaleChanged( float _new_scale );
};

class ITempoSyncKnobModelWrapper {
public:
	enum TempoSyncMode
	{
		SyncNone,
		SyncDoubleWholeNote,
		SyncWholeNote,
		SyncHalfNote,
		SyncQuarterNote,
		SyncEighthNote,
		SyncSixteenthNote,
		SyncThirtysecondNote,
		SyncCustom
	} ;

    virtual ~ITempoSyncKnobModelWrapper() = default;
    virtual TempoSyncKnobSignals* modelSignals() = 0;
    // TODO: find better name
    virtual const IFloatAutomatableModel* wrappedModel() const = 0;
    virtual IFloatAutomatableModel* wrappedModel() = 0;

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _this, const QString& name ) = 0;
	virtual void loadSettings( const QDomElement & _this, const QString& name ) = 0;

    virtual void setRange( const float min, const float max, const float ) = 0;
    virtual float initValue() = 0;
    virtual void setInitValue( const float value ) = 0;
    virtual void setScaleLogarithmic( bool setToTrue) = 0;
    virtual ValueBuffer * valueBuffer() = 0;
    virtual float scale() const = 0;
    virtual TempoSyncMode tempoSyncMode() = 0;
    virtual IMeterModel* custom() = 0;
    virtual void setTempoSync( int _note_type ) = 0;
    virtual void linkModel(ITempoSyncKnobModelWrapper* another) = 0;
    virtual void unlinkModel(ITempoSyncKnobModelWrapper* another) = 0;
};

class IMeterModel {
public:
    virtual ~IMeterModel() = default;
    virtual IIntAutomatableModel & getNumeratorModel() = 0;
	virtual IIntAutomatableModel & getDenominatorModel() = 0;
    virtual const IIntAutomatableModel & getNumeratorModel() const = 0;
	virtual const IIntAutomatableModel & getDenominatorModel() const = 0;
};

class MFact{
public:
    template<typename T>
    static IAutomatableModel<T>* create(
        T val = 0, T min = 0, T max = 0, T step = 0,
        QObject * parent = nullptr,
        const QString& displayName = QString()
    );

    template<>
    IAutomatableModel<float>* create(
        float val, float min, float max, float step,
        QObject * parent,
        const QString& displayName);

    static IAutomatableModel<int>* createIntModel(
        int val, int min, int max,
        QObject * parent,
        const QString& displayName = "");

    template<>
    IAutomatableModel<bool>* create(
        bool val, bool min, bool max, bool step,
        QObject * parent,
        const QString& displayName);

    static IAutomatableModel<bool>* create(
        bool val,
        QObject * parent,
        const QString& displayName = "")
    {
        return create<bool>(val, false, true, 1, parent, displayName);
    }

    static ITempoSyncKnobModelWrapper* create(
        float val, float min, float max, float step, float scale,
        QObject * parent,
        const QString& displayName);
    
    static IComboBoxModelWrapper* createComboBox(QObject* parent = nullptr,
					const QString& displayName = QString());
};

}

#endif /* LMMS_I_MODELS_H */
