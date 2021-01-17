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
std::vector<int16_t> signalFromMonoChannel(std::istream& wavFile) {
	std::vector<int16_t> sig;
	T c = 0;
	wavFile.seekg(0, std::ios::end);
	auto end = wavFile.tellg();
	wavFile.seekg(44);
	while (wavFile.tellg() != end) {
		wavFile.read(reinterpret_cast<char*>(&c), sizeof(c));
		sig.push_back(c);
	}
	return sig;
}
template<typename T>
std::vector<int16_t> signalFromDualChannel(std::istream& wavFile) {
	std::vector<int16_t> sig;
	T c1 = 0, c2 = 0;
	wavFile.seekg(0, std::ios::end);
	auto end = wavFile.tellg();
	wavFile.seekg(44);
	while (wavFile.tellg() != end) {
		wavFile.read(reinterpret_cast<char*>(&c1), sizeof(T));
		wavFile.read(reinterpret_cast<char*>(&c2), sizeof(T));
		sig.push_back(static_cast<int16_t>(((int)c1 + c2) / 2));
	}
	return sig;
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
	if (channelNum == 1) {
		audioSignal = bitsPerSample == 8 ? signalFromMonoChannel<unsigned char>(wavFile) :
			signalFromMonoChannel<int16_t>(wavFile);
	}
	else if (channelNum == 2) {
		audioSignal = bitsPerSample == 8 ? signalFromDualChannel<unsigned char>(wavFile) :
			signalFromDualChannel<int16_t>(wavFile);
	}
	ptr = audioSignal.begin();
}
