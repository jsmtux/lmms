#pragma once

#include <cstdio>
#include <memory>

#include "IAudioEngine.h"

namespace lmms {

class IProjectRenderer {
public:
	enum ExportFileFormats: int
	{
		WaveFile,
		FlacFile,
		OggFile,
		MP3File,
		NumFileFormats
	};

	struct IFileEncodeDevice
	{
		virtual ~IFileEncodeDevice() = default;
		virtual bool isAvailable() const = 0;

		ExportFileFormats m_fileFormat;
		const char * m_description;
		const char * m_extension;
	} ;

    virtual ~IProjectRenderer() = default;
	virtual const IFileEncodeDevice* getFileEncodeDeviceInterface(std::size_t index) const = 0;

	static QString getFileExtensionFromFormat( ExportFileFormats fmt );
};

class OutputSettings;

std::unique_ptr<IProjectRenderer> createProjectRenderer(const IAudioEngine::qualitySettings & _qs,
				const OutputSettings & _os,
				IProjectRenderer::ExportFileFormats _file_format,
				const QString & _out_file);

}