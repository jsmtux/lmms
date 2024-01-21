#include "ExportFilter.h"


namespace lmms {

IExportFilter* InstantiateMidiExportFilter() {
	return static_cast<ExportFilter*>(Plugin::instantiate("midiexport", nullptr, nullptr));
}

}
