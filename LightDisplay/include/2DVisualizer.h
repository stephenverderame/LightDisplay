#pragma once
#include <SignalVisualizer.h>
class Visualizer2D : public SignalVisualizer {
private:
	long minFreq, maxFreq;
	std::unique_ptr<FreqToColorStradegy> stradegy;
public:
	std::vector<Visual> signalToVisuals(const std::vector<std::pair<double, double>>& signal) override;
	Visualizer2D(long minFrequency, long maxFrequency,
		std::unique_ptr<FreqToColorStradegy> && colorStrat) : 
		minFreq(minFrequency), maxFreq(maxFrequency), stradegy(std::move(colorStrat)) {};
};
