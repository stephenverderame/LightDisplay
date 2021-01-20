#pragma once
#include <SignalVisualizer.h>
#include <memory>
class SignalView {
public:
	virtual void draw(const std::vector<Visual>& visuals) = 0;
	virtual ~SignalView() = default;
	/// @return true if the view is still active, false if something on the view end shut it down
	virtual bool isActive() = 0;
};
std::unique_ptr<SignalView> getView();
