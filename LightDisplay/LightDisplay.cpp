#include <iostream>
#include <View.h>
#include <AudioSource.h>
#include <fstream>
#include <SignalVisualizer.h>
#include <FFT.h>
#include <thread>
#include <filesystem>
#include <VisualTransmitter.h>
using namespace std::chrono_literals;
int main()
{
	auto transmitter = getTransmitter();
	auto view = getView();
	std::ifstream input("res/test-sound.wav", std::ios::binary);
	if (!input.is_open()) throw std::invalid_argument("Could not find file");
	auto audio = makeLoopbackAudioSource(); // makeStreamAudioSource(input);
	auto visualizer = getSignalVisualizer();
	std::this_thread::sleep_for(10ms);
	while (view->isActive() && audio->ready()) {
		auto start = std::chrono::system_clock::now();
		FFT::Signal signal(audio->sample(20ms), audio->samplingFrequency());
		auto v = visualizer->signalToVisuals(signal.amplitudeList());
		view->draw(v);
		std::this_thread::sleep_until(start + signal.getDuration());
	}
	return 0;
}
