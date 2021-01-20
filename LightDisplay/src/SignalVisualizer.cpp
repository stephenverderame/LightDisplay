#include <SignalVisualizer.h>
#include <2DVisualizer.h>
#include <memory>
constexpr long min_frequency = 20;
constexpr long max_frequency = 10000;
const Color Color::RED{ 255, 0, 0, 255 };
const Color Color::BLUE{ 0, 0, 255, 255 };
const Color Color::GREEN{ 0, 255, 0, 255 };
const Color Color::WHITE{ 255, 255, 255, 255 };
const Color Color::BLACK{ 0, 0, 0, 255 };

std::unique_ptr<SignalVisualizer> getSignalVisualizer()
{
	return std::make_unique<Visualizer2D>(min_frequency, max_frequency, 
		std::make_unique<SynthesiaFreqToColor>());
}

inline double lightFreqToWavelength(double freq) {
	return 299792458 / freq; // v = f(wavelength)
}
double octaveUpToLight(double f) {
	while (f > 0 && f < 400._thz) f *= 2;
	return f;
}
/**
 * Determines the fraction of the distance x is from lo to hi
 * Fraction represented as a byte (so frac * 255)
 * Requies lo <= x <= hi
 * @return number representing how close x is to hi from lo, where 255 means x == hi and 0 means x == lo
 */
inline unsigned char fracBetweenToByte(double lo, double hi, double x) {
	return static_cast<unsigned char>((x - lo) / (hi - lo) * 255);
}
double intensityFactorFromWavelength(double wavelength) {
	// Intensity factor goes through the range:
	// 0.1 (350-420 nm) 1.0 (420-645 nm) 1.0 (645-780 nm) 0.2
	double factor = 0;

	if (wavelength >= 350 && wavelength < 420) {
		factor = 0.1 + 0.9 * (wavelength - 350) / (420.0 - 350);
	}
	else if (wavelength >= 420 && wavelength < 645) {
		factor = 1.0;
	}
	else if (wavelength >= 645 && wavelength <= 780) {
		factor = 0.2 + 0.8 * (780 - wavelength) / (780.0 - 645);
	}
	return factor;
}
inline unsigned char adjustColorComponentForIntensity(unsigned char component, double intensityFactor) {
	constexpr auto maxIntensity = 255;
	constexpr auto gamma = 1;
	if (component == 0) return 0;
	return static_cast<unsigned char>(round(maxIntensity * 
		pow((static_cast<double>(component) / maxIntensity) * intensityFactor, gamma)));
}
inline Color setColorIntensity(Color c, double wavelength) {
	double intensityFac = intensityFactorFromWavelength(wavelength);
	c.r = adjustColorComponentForIntensity(c.r, intensityFac);
	c.g = adjustColorComponentForIntensity(c.g, intensityFac);
	c.b = adjustColorComponentForIntensity(c.b, intensityFac);
	return c;
}
/**
 * Converts a wavelength in nm to a color representation
 * Does not set the alpha parameter of color
 */
Color wavelengthToColor(double wavelength) {
	Color c = Color::BLACK;
	if (wavelength >= 350 && wavelength < 440) {
		// From Purple (1, 0, 1) to Blue (0, 0, 1), with increasing intensity (set below)
		c.r = fracBetweenToByte(350, 440, wavelength);
		c.b = 255;
	}
	else if (wavelength >= 440 && wavelength < 490) {
		// From Blue (0, 0, 1) to Cyan (0, 1, 1) 
		c.g = fracBetweenToByte(440, 490, wavelength);
		c.b = 255;
	}
	else if (wavelength >= 490 && wavelength < 510) {
		// From  Cyan (0, 1, 1)  to  Green (0, 1, 0) 
		c.g = 255;
		c.b = fracBetweenToByte(490, 510, wavelength);
	}
	else if (wavelength >= 510 && wavelength < 580) {
		// From  Green (0, 1, 0)  to  Yellow (1, 1, 0)
		c.r = fracBetweenToByte(510, 580, wavelength);
		c.g = 255;
	}
	else if (wavelength >= 580 && wavelength < 645) {
		// From  Yellow (1, 1, 0)  to  Red (1, 0, 0)
		c.r = 255;
		c.g = fracBetweenToByte(580, 645, wavelength);
	}
	else if (wavelength >= 645 && wavelength <= 780) {
		// Solid Red (1, 0, 0), with decreasing intensity (set below)
		c.r = 255 - fracBetweenToByte(645, 780, wavelength);
	}
	return setColorIntensity(c, wavelength);
}

Color SynthesiaFreqToColor::operator()(double freq)
{
	const auto lightFreq = octaveUpToLight(freq);
	const auto wavelength = lightFreqToWavelength(lightFreq) / 1._nm;
	return wavelengthToColor(wavelength);
}
