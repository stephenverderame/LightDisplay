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
	return std::make_unique<Visualizer2D>(min_frequency, max_frequency);
}
