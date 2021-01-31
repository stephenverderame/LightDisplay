#include <AudioSource.h>
#include <WavAudioSource.h>
#include <PlaybackAudioSource.h>

std::unique_ptr<AudioSource> makeStreamAudioSource(std::istream& is)
{
	return std::make_unique<WavAudioSource>(is);
}

AudioSource* makeLoopbackAudioSource()
{
	return PlaybackAudioSource::get();
}
