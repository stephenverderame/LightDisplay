#pragma once
#include <AudioSource.h>
#include <iosfwd>
class WavAudioSource : public AudioSource {
private:
	std::vector<int16_t> audioSignal;
	decltype(audioSignal)::const_iterator ptr;
	uint16_t sampleRate;
public:
	signal_t sample(std::chrono::milliseconds duration) override;
	double samplingFrequency() const override;
	bool ready() const override;
	WavAudioSource(std::istream& wavFile);

};
