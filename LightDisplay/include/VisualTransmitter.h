#pragma once
#include <SignalVisualizer.h>
class VisualTransmitter {
public:
	virtual ~VisualTransmitter() = default;
	virtual void transmit(const std::vector<Visual>& data) = 0;
};
std::unique_ptr<VisualTransmitter> getTransmitter();
