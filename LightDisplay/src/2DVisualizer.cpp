#include <2DVisualizer.h>
#include <vector>
#include <algorithm>

template<typename K, typename V>
/// Requires v be at least size 1
inline std::enable_if_t<std::is_arithmetic_v<V>, std::pair<K, V>>
getMaxValues(const std::vector<std::pair<K, V>>& v)
{
	K nearMax = v[0].first;
	V max = v[0].second;
	for (auto i = decltype(v.size()){1}; i < v.size(); ++i) {
		if (v[i].second > max)
			max = v[i].second;
		if (std::abs(max - v[i].second) / static_cast<double>(max) > 0.8)
			nearMax = v[i].first;
	}
	return { nearMax, max };
}


inline float toNDC(double value, double max, double min = 0) {
	return static_cast<float>((value - min) / static_cast<double>(max - min) * 2.0 - 1.0);
}

std::vector<Visual> Visualizer2D::signalToVisuals(const std::vector<std::pair<double, double>>& signal)
{
	std::vector<Visual> visuals(signal.size());
	const auto localMaxFreq = signal[signal.size() - 1].first;
	const auto localMax = getMaxValues(signal);
	for (auto i = decltype(signal.size()){0}; i < signal.size(); ++i) {
		visuals[i].color = (*stradegy)(signal[i].first);
//		visuals[i].color.a = static_cast<unsigned char>((signal[i].second / localMax.second) * 255);
		visuals[i].ndcX = toNDC(signal[i].first, localMax.first);
			//toNDC(octaveUpToLight(signal[i].first), 700._thz, 400._thz);
		visuals[i].ndcY = toNDC(signal[i].second, localMax.second);
	}
	return visuals;
}
