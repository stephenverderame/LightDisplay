#include <gtest/gtest.h>
#include <FFT.h>
#include <chrono>
#include <math.h>
#include "FFTCmpImpl.h"
#include <algorithm>
using namespace testing;
using namespace std::chrono_literals;

const auto pi2 = acos(-1) * 2;



signal_t generateSignal(double freq, double amplitude, double samplingFreq, std::chrono::microseconds duration,
	bool enforcePowerOf2 = false) {
	const double durSeconds = duration.count() / pow(10, 6);
	signal_t s;
	const auto sampleCount = [&]() {
		if (enforcePowerOf2) {
			return nextPowerOf2(static_cast<size_t>(durSeconds * samplingFreq));
		}
		else
			return static_cast<size_t>(durSeconds * samplingFreq);
	}();

	s.reserve(sampleCount);
	for (size_t i = 0; i < sampleCount; ++i) {
		s.push_back(amplitude * cos(pi2 * freq * (i / samplingFreq)));
	}
	return s;
}
signal_t randomVector(size_t len, bool enforcePowerOf2 = true) {
	if (enforcePowerOf2) {
		len = nextPowerOf2(len);
	}
	signal_t ret;
	for (auto i = decltype(len){0}; i < len; ++i)
		ret.push_back(rand());
	return ret;
}
inline double randNormal() {
	return rand() / static_cast<double>(RAND_MAX);
}
TEST(FFTTest, bitReverseNum) {
	for (auto i = 0; i < 32; ++i) {
		const auto num = (1ull << i);
		ASSERT_EQ(FFT::numBits(num), i + 1);
	}
	const auto iMax = 1ull << 20;
	const auto bitNum = FFT::numBits(iMax);
	for (auto i = decltype(iMax){0}; i < iMax; ++i) {
		ASSERT_EQ(i, FFT::bitReverse(FFT::bitReverse(i, bitNum), bitNum));
	}
	ASSERT_EQ(FFT::bitReverse(1, 3), 4);
	ASSERT_EQ(FFT::bitReverse(3, 3), 6);
	ASSERT_EQ(FFT::bitReverse(1, 7), 64);
	ASSERT_EQ(FFT::bitReverse(2, 7), 32);
}
TEST(FFTTest, bitReverse) {
	for (auto i = 0; i < 1000; ++i) {
		const auto freq = rand() % 100 + 1;
		const auto samplingFreq = rand() % 20 + 2 * freq + 1;
		auto sig = generateSignal(freq, 1, samplingFreq, 1s, true);
		const auto sigCpy = sig;
		sig = FFT::bitReverse(std::move(sig));

		const auto bitNum = FFT::numBits(sigCpy.size() - 1);
		for (auto j = decltype(sigCpy.size()){0}; j < sigCpy.size(); ++j) {
			ASSERT_EQ(sigCpy[FFT::bitReverse(j, bitNum)], sig[j]);
		}
	}
}
bool eqEnough(const signal_t& a, const signal_t& b) {
	if (a.size() != b.size()) return false;
	for (auto i = decltype(a.size()){0}; i < a.size(); ++i) {
		if (abs(a[i].real() - b[i].real()) > 0.00001) {
			EXPECT_EQ(a, b); 
			std::cout << "Vector equality fail at index " << i << ": " << a[i] << " != " << b[i] << "\n";
			return false;
		}
		if (abs(a[i].imag() - b[i].imag()) > 0.00001) {
			EXPECT_EQ(a, b);
			std::cout << "Vector equality fail at index " << i << ": " << a[i] << " != " << b[i] << "\n";
			return false;
		}
	}
	return true;
}
TEST(FFTTest, fft)
{
	constexpr auto test_num = 100;
	constexpr auto test_size = 100; //reduced after successful tests to keep the entire test suite short
	signal_t start = { 5, 3, 2, 1 };
	signal_t res = { 11, {3, -2}, 3, {3, 2} };
	ASSERT_EQ(FFT::fft(std::move(start)), res);
	start = { 1, 2, 3, 4, 5, 6, 7, 8 };
	res = { 36, {-4.000000,9.656854}, {-4.000000,4.000000}, {-4.000000,1.656854},
		{-4.000000,0.000000}, {-4.000000,-1.656854}, {-4.000000,-4.000000},
		{-4.000000,-9.656854} };
	ASSERT_TRUE(eqEnough(FFT::fft(std::move(start)), res));
	for (auto i = 0; i < test_num; ++i) {
		auto sig = randomVector(rand() % test_size);
		auto sigCpy = sig;
		auto sigRes = sig;
		fft(sigCpy.begin(), sigRes.begin(), static_cast<int>(log2(sigCpy.size())));
		ASSERT_TRUE(eqEnough(FFT::fft(std::move(sig)), sigRes));
	}
}
TEST(FFTTest, signalRepresentation) {
	for (int i = 0; i < 500; ++i) {
		const auto freq = rand() % 1000 + 1;
		const auto samplingFreq = rand() % 2000 + 2 * freq + 1;
		const auto amplitude = rand();
		auto signal = generateSignal(freq, amplitude, samplingFreq, 1s, true);
		FFT::Signal sig(std::move(signal), samplingFreq);
		const auto freqAmp = sig.amplitudeList();
		std::pair<double, double> max = { 0, INT_MIN };
		for (auto& p : freqAmp) {
			if (p.second > max.second)
				max = p;
		}
		ASSERT_LE(abs(max.first - freq), 1);
		ASSERT_LE(abs(max.second - amplitude), 0.4 * amplitude);
	}
}

int main(int argc, char** argv) {
	srand(clock());
	InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}