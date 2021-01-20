#include <gtest/gtest.h>
#include <AudioSource.h>
#include <fstream>
#include <FFT.h>
using namespace testing;
using namespace std::chrono_literals;
std::vector<int16_t> readAudioSource(const char* file) {
	std::ifstream f(file, std::ios::binary);
	if (!f.is_open()) throw std::invalid_argument(file);
	f.seekg(0, std::ios::end);
	auto end = f.tellg();
	f.seekg(22);
	int16_t channels = 0;
	f.read(reinterpret_cast<char*>(&channels), sizeof(channels));
	EXPECT_TRUE(channels == 1 || channels == 2);
	if (channels <= 0 || channels > 2) throw std::invalid_argument(std::to_string(channels));
	f.seekg(44, std::ios::beg);
	int res = 0;
	std::vector<int16_t> result(end - std::streampos(44));
	f.read((char*)result.data(), result.size() * sizeof(int16_t));
	if (channels == 1) return result;
	std::vector<int16_t> combined(result.size() / channels);
	for (auto i = 0; i < combined.size(); ++i) {
		for (auto j = 0; j < channels; ++j) {
			res += result[(long long)channels * i + j];
		}
		res /= channels;
		combined[i] = (int16_t)res;
		res = 0;
	}
	return combined;
}
void assertCorrectSample(const std::vector<int16_t>& real, const signal_t& read, size_t & ptr) {
	auto i = decltype(read.size()){0};
	for (; i < read.size() && ptr < real.size(); ++ptr, ++i) {
		ASSERT_EQ(real[ptr], (int16_t)read[i].real());
		ASSERT_EQ(0.0, read[i].imag());
	}
	if (i < read.size()) {
		for (; i < read.size(); ++i) {
			ASSERT_EQ(0, (int16_t)read[i].real());
			ASSERT_EQ(0, (int)read[i].imag());
		}
	}

}
TEST(AudioTest, loadWav) {
	const auto wav = readAudioSource("res/sine-tone-440hz.wav");
	size_t wavCounter = 0;
	std::ifstream in("res/sine-tone-440hz.wav", std::ios::binary);
	ASSERT_TRUE(in.is_open());
	auto audio = makeStreamAudioSource(in);
	ASSERT_EQ(audio->samplingFrequency(), 44100);
	double lastHighestFreq = -1;
	while (audio->ready()) {
		auto sig = audio->sample(20ms);
		assertCorrectSample(wav, sig, wavCounter);
		FFT::Signal signal(std::move(sig), audio->samplingFrequency());
		std::pair<double, double> max = { 0, 0 };
		auto amps = signal.amplitudeList();
		for (auto& p : amps)
			if (p.second > max.second) max = p;
		if (lastHighestFreq >= 0) {
			ASSERT_LE(std::abs(lastHighestFreq - max.first), 1);
			ASSERT_LE(std::abs(lastHighestFreq - 440), 440 * 0.1);
		}
		lastHighestFreq = max.first;
	}
}

int main(int argc, char** argv) {
	InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
