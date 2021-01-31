#pragma once
#include "Signal.h"
#include <iosfwd>
#include <chrono>
class AudioSource {
public:
	/**
	 * Retrieves the next sample from the audio source and removes it from the stream
	 * Will always retrive a sampled wave of the same length (zero-padded if necessary)
	 * and same sampling frequency for the same duration
	 * Requires the source to be in the ready state
	 * @param duration the minimum duration of the sample length. The result may not actually have this duration
	 * @return a signal in time-amplitude domain of a power of 2 length
	 */
	virtual signal_t sample(std::chrono::milliseconds duration) = 0;
	virtual double samplingFrequency() const = 0;
	virtual ~AudioSource() = default;
	/**
	 * @return true if a successive call to sample will read some meaningful data
	 */
	virtual bool ready() const = 0;

};
std::unique_ptr<AudioSource> makeStreamAudioSource(std::istream& is);
AudioSource* makeLoopbackAudioSource();
