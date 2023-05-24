#pragma once

#include <QString>

#include "lmms_export.h"

namespace lmms::PathUtil
{
	//! Make this path absolute. If a pointer to boolean is given
	//! it will indicate whether the path was converted successfully
	QString LMMS_EXPORT toAbsolute(const QString & input, bool* error = nullptr);
	//! Get the filename for a path, handling prefixed paths correctly
	QString LMMS_EXPORT cleanName(const QString & path);
}