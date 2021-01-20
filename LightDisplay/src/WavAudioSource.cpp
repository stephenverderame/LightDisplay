#include <WavAudioSource.h>
#include <istream>
signal_t WavAudioSource::sample(std::chrono::milliseconds duration)
{
	const auto sampleNum = nextPowerOf2(
		static_cast<long long>(duration.count() / 1000.0 * sampleRate));
	signal_t result;
	result.reserve(sampleNum);
	if (ptr != audioSignal.end()) {
		result.insert(result.end(), ptr, audioSignal.end() - ptr > sampleNum ?
			ptr + sampleNum : audioSignal.end());
		if (result.size() < static_cast<unsigned long long>(sampleNum)) {
			result.resize(sampleNum, 0);
			ptr = audioSignal.end();
		}
		else
			ptr += sampleNum;
	}
	return result;
}

double WavAudioSource::samplingFrequency() const
{
	return this->sampleRate;
}

bool WavAudioSource::ready() const
{
	return ptr < audioSignal.end();
}

void assertStreamIsWav(std::istream& wavFile) {
	char header[5];
	wavFile.read(header, 4);
	header[4] = '\0';
	if (strcmp(header, "RIFF") != 0) throw std::invalid_argument("Bad file format for Wav file reader");
	wavFile.seekg(4, std::ios::cur);
	wavFile.read(header, 4);
	if (strcmp(header, "WAVE") != 0) throw std::invalid_argument("Bad wav file format");
}
template<typename T>
inline std::enable_if_t<std::is_integral_v<T>, T>
readNum(std::streampos offset, std::istream& str) {
	str.seekg(offset);
	T sr = 0;
	str.read(reinterpret_cast<char*>(&sr), sizeof(sr));
	return sr;
}
inline void assertWavStreamUncompressed(std::istream& wavFile) {
	if (readNum<int16_t>(20, wavFile) != 1) 
		throw std::invalid_argument("Wav file cannot be compressed");
}

inline uint16_t readSampleRate(std::istream& wavFile) {
	return readNum<uint16_t>(24, wavFile);
}
inline uint16_t readChannelNum(std::istream& wavFile) {
	return readNum<uint16_t>(22, wavFile);
}
inline uint16_t readBps(std::istream& wavFile) {
	return readNum<uint16_t>(34, wavFile);
}

template<typename T>
std::vector<T> loadData(std::istream& wavFile) {
	wavFile.seekg(0, std::ios::end);
	auto end = wavFile.tellg();
	auto bytes = end - std::streampos(44);
	wavFile.seekg(44);
	std::vector<T> data(bytes / sizeof(T));
	wavFile.read(reinterpret_cast<char*>(&data[0]), bytes);
	return data;
}
template<typename T>
std::vector<int16_t> mergeChannels(std::vector<T>&& data, int channels) {
	if (channels == 1) return { data.begin(), data.end() };
	std::vector<int16_t> result(data.size() / channels);
	for (decltype(data.size()) i = 0; i < result.size(); ++i) {
		int res = 0;
		for (auto j = 0; j < channels; ++j) {
			res += data[i * channels + j];
		}
		result[i] = static_cast<int16_t>(res);
	}
	return result;

}
inline bool isValidBps(uint16_t bitsPerSample) {
	return bitsPerSample == 8 || bitsPerSample == 16;
}

WavAudioSource::WavAudioSource(std::istream& wavFile)
{
	assertStreamIsWav(wavFile);
	assertWavStreamUncompressed(wavFile);
	auto channelNum = readChannelNum(wavFile);
	sampleRate = readSampleRate(wavFile);
	auto bitsPerSample = readBps(wavFile);
	if (bitsPerSample == 8) {
		audioSignal = mergeChannels(loadData<unsigned char>(wavFile), channelNum);
	}
	else if (bitsPerSample == 16) {
		audioSignal = mergeChannels(loadData<int16_t>(wavFile), channelNum);
	}
	else {
		throw std::invalid_argument("Invalid bits per sample");
	}
	ptr = audioSignal.begin();
}
