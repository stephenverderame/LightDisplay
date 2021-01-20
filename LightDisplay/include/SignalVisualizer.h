#pragma once
#include <vector>
#include <memory>
struct Color {
	unsigned char r, g, b, a;
	Color() : r(0), g(0), b(0), a(0) {}
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
		r(r), g(g), b(b), a(a) {}
	
	const static Color RED, WHITE, BLUE, GREEN, BLACK;

};
struct Visual {
	Color color;
	float ndcX, ndcY; ///< Range from -1 to 1
	Visual() : color(), ndcX(0), ndcY(0) {}
	/// Requires ndcX and ndcY range from -1 to 1
	Visual(Color c, float ndcX, float ndcY) :
		color(c), ndcX(ndcX), ndcY(ndcY) {}
};
class SignalVisualizer {
public:
	/**
	 * Visualizes a signal
	 * @param signal a signal in the frequency-amplitude domain
	 */
	virtual std::vector<Visual> signalToVisuals(const std::vector<std::pair<double, double>>& signal) = 0;
	virtual ~SignalVisualizer() = default;
};
std::unique_ptr<SignalVisualizer> getSignalVisualizer();

class FreqToColorStradegy {
public:
	/**
	 * Converts the given frequency to a color
	 * @return Color whose r,g, and b values correspond to a particular mapping from the frequency. 
	 * Alpha of the color may or may not be meaningful
	 */
	virtual Color operator()(double frequency) = 0;
	virtual ~FreqToColorStradegy() = default;
};

class SynthesiaFreqToColor : public FreqToColorStradegy {
public:
	Color operator()(double freq) override;
};
/// Tera hertz
constexpr double operator "" _thz(long double d) {
	return static_cast<double>(d * 1000000000000);
}
/// Nanomemters
constexpr double operator "" _nm(long double d) {
	return static_cast<double>(d * 0.000000001);
}

/**
 * Converts a lower frequency to a sound frequency by incrementing the octave of the representative frequency
 */
double octaveUpToLight(double f);
