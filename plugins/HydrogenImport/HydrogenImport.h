#ifndef _HYDROGEN_IMPORT_H
#define _HYDROGEN_IMPORT_H

#include <QString>

#include "TrackContainer.h"

#include "plugins/QWidgetImportFilter.h"

namespace lmms
{


class HydrogenImport : public QWidgetImportFilter
{
public:
	HydrogenImport( const QString & _file );
        bool readSong();

	~HydrogenImport() override = default;

	gui::PluginView* instantiateView( QWidget * ) override
	{
		return( nullptr );
	}
private:
	bool tryImport( TrackContainer* tc ) override;
};


} // namespace lmms

#endif

