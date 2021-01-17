#pragma once
#include <SignalVisualizer.h>
class ScreenDisplay {
public:
	virtual ~ScreenDisplay() = default;
	virtual void render(const std::vector<Visual>& visuals) = 0;
	virtual void resize(int width, int height) = 0;
};