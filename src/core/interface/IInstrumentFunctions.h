#pragma once

#include <Qstring>
#include <QVector>
#include <QObject>

#include "IModels.h"

namespace lmms
{


class IInstrumentFunctionNoteStacking
{
public:
	static const int MAX_CHORD_POLYPHONY = 13;
	static const int NUM_CHORD_TABLES = 95;
	using ChordSemiTones = std::array<int8_t, MAX_CHORD_POLYPHONY>;
	struct Chord
	{
	private:
		QString m_name;
		ChordSemiTones m_semiTones;
		int m_size;

	public:
		Chord() : m_size( 0 ) {}

		Chord( const char * n, const ChordSemiTones & semi_tones ) :
            m_name( QObject::tr( n ) )
        {
            for( m_size = 0; m_size < MAX_CHORD_POLYPHONY; m_size++ )
            {
                if( semi_tones[m_size] == -1 )
                {
                    break;
                }

                m_semiTones[m_size] = semi_tones[m_size];
            }
        }

		int size() const
		{
			return m_size;
		}

		bool isScale() const
		{
			return size() > 6;
		}

		bool isEmpty() const
		{
			return size() == 0;
		}

		bool hasSemiTone( int8_t semiTone ) const
        {
            for( int i = 0; i < size(); ++i )
            {
                if( semiTone == m_semiTones[i] )
                {
                    return true;
                }
            }
            return false;
        }

		int8_t last() const
		{
			return m_semiTones[size() - 1];
		}

		const QString & getName() const
		{
			return m_name;
		}

		int8_t operator [] ( int n ) const
		{
			return m_semiTones[n];
		}
	};


	struct ChordTable : public QVector<Chord>
	{
	private:
		ChordTable() :
            QVector<Chord>()
        {
            for (const auto& chord : s_initTable)
            {
                push_back(Chord(chord.m_name, chord.m_semiTones));
            }
        }

		struct Init
		{
			const char * m_name;
			ChordSemiTones m_semiTones;
		};

		static std::array<Init, NUM_CHORD_TABLES> s_initTable;

	public:
		static const ChordTable & getInstance()
		{
			static ChordTable inst;
			return inst;
		}

		const Chord & getByName( const QString & name, bool is_scale = false ) const
        {
            for( int i = 0; i < size(); i++ )
            {
                if( at( i ).getName() == name && is_scale == at( i ).isScale() )
                    return at( i );
            }

            static Chord empty;
            return empty;
        }


		const Chord & getScaleByName( const QString & name ) const
		{
			return getByName( name, true );
		}

		const Chord & getChordByName( const QString & name ) const
		{
			return getByName( name, false );
		}
	};

	virtual IBoolAutomatableModel* chordsEnabledModel() = 0;
	virtual IComboBoxModelWrapper* chordsModel() = 0;
	virtual IFloatAutomatableModel* chordRangeModel() = 0;
	virtual Model* model() = 0;
};

class IInstrumentFunctionArpeggio
{
public:
	virtual IBoolAutomatableModel* arpEnabledModel() = 0;
	virtual IComboBoxModelWrapper* arpModel() = 0;
	virtual IFloatAutomatableModel* arpRangeModel() = 0;
	virtual IFloatAutomatableModel* arpRepeatsModel() = 0;
	virtual IFloatAutomatableModel* arpCycleModel() = 0;
	virtual IFloatAutomatableModel* arpSkipModel() = 0;
	virtual IFloatAutomatableModel* arpMissModel() = 0;
	virtual ITempoSyncKnobModelWrapper* arpTimeModel() = 0;
	virtual IFloatAutomatableModel* arpGateModel() = 0;
	virtual IComboBoxModelWrapper* arpDirectionModel() = 0;
	virtual IComboBoxModelWrapper* arpModeModel() = 0;
	virtual Model* model() = 0;
};

}