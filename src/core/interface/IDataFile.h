#pragma once

#include <QTextStream>
#include <QString>
#include <QDomDocument>

namespace lmms {

class IDataFile : public QDomDocument
{
public:
	enum Types
	{
		UnknownType,
		SongProject,
		SongProjectTemplate,
		InstrumentTrackSettings,
		DragNDropData,
		ClipboardData,
		JournalData,
		EffectSettings,
		MidiClip,
		TypeCount
	} ;
	IDataFile() {}
	IDataFile(const QString& name) : QDomDocument( name ) {}
    virtual ~IDataFile() = default;

	virtual void write( QTextStream& strm ) = 0;
	virtual bool writeFile(const QString& fn, bool withResources = false) = 0;
	virtual bool validate( QString extension ) = 0;
	virtual QDomElement& content() = 0;
	virtual QDomElement& head() = 0;
};

std::unique_ptr<IDataFile> createDataFile(const QString& fileName);
std::unique_ptr<IDataFile> createDataFile( IDataFile::Types type );

}