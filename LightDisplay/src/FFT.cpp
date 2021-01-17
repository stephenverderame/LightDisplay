#define _USE_MATH_DEFINES
#include <FFT.h>
#include <future>
using namespace FFT;
const static auto pi2 = acos(-1) * 2;
/// Requires n > 0
inline bool isPowerOf2(size_t n) {
	return !(n & (n - 1));
	//power of 2 will have one bit set.
	//power of 2 - 1 will have all bits less than the bit for the power of 2 set
	//therefore n & (n - 1) == 0 if n is a power of 2
}
/// Requires s.size() > 0
inline void assertLengthPowerOf2(const signal_t& s) {
	if (!isPowerOf2(s.size())) throw std::invalid_argument("Must have a power of 2 length");
}
/// @return least number of bits needed to represent n
unsigned char FFT::numBits(size_t n) {
	unsigned char c = 0;
	while (n > 0) {
		n >>= 1;
		++c;
	}
	return c;
}
/**
 * Performs a single butterfly of the specified size
 * Requires the butterfly of the previous power of 2 already be performed on s
 * @param s the signal to comput. Effect: s is modified in-place
 * @param butterflySize the amount of elements in the butterfly
 * @param halfSize butterflySize / 2
 * @param twiddleFactor
 * @param startIndex the index of S to perform the butterfly. Should be aligned so that the previous butterfly ends at startIndex
 */
void innerButterfly(signal_t& s, size_t butterflySize, size_t halfSize, 
	cmplx_t twiddleFactor, size_t startIndex) {
	for (auto i = decltype(butterflySize){0}; i < halfSize; ++i) {
		const auto wn = pow(twiddleFactor, i);
		const auto temp = s[startIndex + i];
		s[startIndex + i] = s[startIndex + i] + wn * s[startIndex + halfSize + i];
		s[startIndex + halfSize + i] = temp - wn * s[startIndex + halfSize + i];

	}
}
/**
 * Performs the butterfly step for the FFT
 * Repeatedly performs the specific butterfly on the input
 * Requires the butterfly of the previous power of 2 size already be performed on the input
 * Requires s be a power of 2
 * @param s signal to perform the butterfly on
 * Effect: s is modified in-place
 */
inline void butterfly(signal_t& s, size_t butterflySize) {
	const auto w = exp(cmplx_t{ 0, -pi2 / butterflySize });
	const auto hMergeSize = butterflySize >> 1; //half of butterfly size
	const auto sz = s.size();
	for (auto j = decltype(sz){0}; j < sz; j += butterflySize) {// for each butterfly chunk in the list
		for (auto i = decltype(butterflySize){0}; i < hMergeSize; ++i) {
			const auto wn = pow(w, i);
			const auto temp = s[j + i];
			s[j + i] = s[j + i] + wn * s[j + hMergeSize + i];
			s[j + hMergeSize + i] = temp - wn * s[j + hMergeSize + i];

		}
	}
}

signal_t FFT::fft(signal_t&& s)
{
	//s.size() is a power of 2
	s = bitReverse(std::move(s));
	size_t mergeSize = 2;
	const auto sz = s.size();
	while (sz >= mergeSize) {
		butterfly(s, mergeSize);
		mergeSize <<= 1;
	}
	return s;

}

size_t FFT::bitReverse(size_t n, unsigned char numBits) {
	size_t res = 0;
	for (auto i = decltype(numBits){0}; i < numBits; n >>= 1, ++i) {
		res = (res << 1) | (n & 1);

	}
	return res;
}

signal_t FFT::bitReverse(signal_t&& s)
{
	assertLengthPowerOf2(s);
	const auto bits = numBits(s.size() - 1);
	for (size_t i = 1; i < s.size() - 1; ++i) {
		const auto bitReverseIndex = bitReverse(i, bits);
		if(bitReverseIndex > i)
			std::swap(s[i], s[bitReverseIndex]);
	}
	return std::move(s);

}

FFT::Signal::Signal(signal_t&& s, double samplingFrequency)
	: s(FFT::fft(std::move(s))), samplingFrequency(samplingFrequency), average(-10)

{
}
/**
 * Linear interpolation between p1 and p2
 * Requires p1.first < p2.first and the first value in the point be in the same domain as x
 */
double lerp(std::pair<double, double> p1, std::pair<double, double> p2, double x) {
	return (x - p1.first) / (p2.first - p1.first) * (p2.second - p1.second) + p1.second;
}

double FFT::Signal::amplitudeAt(double frequency) const
{
	if (frequency > samplingFrequency / 2) throw std::invalid_argument("Frequency above nyquist limit");
	const auto bin = frequency * s.size() / samplingFrequency;
	const auto low = static_cast<size_t>(floor(bin));
	const auto high = static_cast<size_t>(ceil(bin));
	const auto magLo = magnitudeOfIndex(low);
	const auto magHi = magnitudeOfIndex(high);
	return lerp({ static_cast<double>(low), magLo }, { static_cast<double>(high), magHi }, bin);

}

std::vector<std::pair<double, double>> FFT::Signal::amplitudeList() const
{
	std::vector<std::pair<double, double>> ret;
	ret.reserve(s.size());
	for (auto i = decltype(s.size()){0}; i < s.size() / 2; ++i) {
		ret.emplace_back(static_cast<double>(i) * samplingFrequency / s.size(), magnitudeOfIndex(i));
	}
	return ret;
}

double FFT::Signal::avgMagnitude() const
{
	if (average < 0) {
		average = 0;
		for (auto i = decltype(s.size()){0}; i < s.size() / 2; ++i) {
			average += magnitudeOfIndex(i);
		}
		average /= (s.size() / 2);
	}
	return average;
}
