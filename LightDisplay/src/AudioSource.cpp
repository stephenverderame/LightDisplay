#include <AudioSource.h>
#include <WavAudioSource.h>

std::unique_ptr<AudioSource> makeStreamAudioSource(std::istream& is)
{
	return std::make_unique<WavAudioSource>(is);
}
