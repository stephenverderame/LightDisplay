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
