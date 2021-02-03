#pragma once
#include <SignalVisualizer.h>
class VisualReceiver {
public:
	virtual ~VisualReceiver() = default;
	/**
	 * Accepts a transmitter client
	 * @throw runtime_error on failure
	 */
	virtual void accept() = 0;
	/**
	 * Receives data from an accepted client
	 * Requires a connection be made by a successful call to accept
	 */
	virtual std::vector<Visual> recv() = 0;
};
