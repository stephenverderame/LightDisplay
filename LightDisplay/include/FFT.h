#pragma once
#include <vector>
#include <complex>
#include <chrono>
#include <Signal.h>
namespace FFT {

	/**
	 * Arranges s in bit reversed order
	 * Requires s
	 * .s[0] <-> s[0], s[1] <-> s[4]
	 */
	signal_t bitReverse(signal_t&& s);

	size_t bitReverse(size_t n, unsigned char numBits);

	
	unsigned char numBits(size_t n);

	/**
	 * Performs the FFT on the signal
	 * Requires s be a power of 2 size
	 * @param s the signal to compute. Modified in-place
	 * @return the FFT of the signal
	 */
	signal_t fft(signal_t&& s);

	class Signal {
	private:
		signal_t s; ///< Signal after the application of the FFT
		double samplingFrequency;
		mutable double average; ///< average magnitude or < 0 if it has yet to be calculated
	public:
		/**
		 * @param s the raw signal received from a wave source. Required to have a power of 2 size
		 * @param samplingFrequency sr of the signal
		 */
		Signal(signal_t&& s, double samplingFrequency);
		double amplitudeAt(double frequency) const;
		/**
		 * Returns a list of {frequency, amplitude} pairs for the signal
		 */
		std::vector<std::pair<double, double>> amplitudeList() const;
		double avgMagnitude() const;
	private:
		inline double magnitudeOfIndex(size_t index) const {
			return sqrt(s[index].imag() * s[index].imag() + s[index].real() * s[index].real()) 
				/ s.size() * 2; //normalize magnitudes
		}
	};
}