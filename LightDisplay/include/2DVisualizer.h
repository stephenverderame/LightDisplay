#pragma once
#include <SignalVisualizer.h>
class Visualizer2D : public SignalVisualizer {
private:
	long minFreq, maxFreq;
public:
	std::vector<Visual> signalToVisuals(const std::vector<std::pair<double, double>>& signal) override;
	Visualizer2D(long minFrequency, long maxFrequency) : minFreq(minFrequency), maxFreq(maxFrequency) {};
};
