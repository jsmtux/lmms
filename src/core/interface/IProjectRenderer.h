#pragma once

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
	} ;

    virtual ~IProjectRenderer() = default;
};

}