#ifndef __ResourceAudio_h__
#define __ResourceAudio_h__

#include "Resource.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

class ResourceAudio :
	public Resource
{
public:
	ResourceAudio(unsigned uid);
	ResourceAudio(const ResourceAudio& resource);
	~ResourceAudio();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;
	void LoadConfigFromLibraryMeta() override;

	void DrawLoadSettings() override;

public:
	bool streamed = false;

	SoLoud::Wav* wavFX = nullptr;
	SoLoud::WavStream* wavstream = nullptr;
};

#endif __ResourceAudio_h__