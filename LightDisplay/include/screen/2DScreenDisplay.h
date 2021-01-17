#pragma once
#include "ScreenDisplay.h"
class ScreenDisplay2D : public ScreenDisplay {
private:
	struct impl;
	std::unique_ptr<impl> pimpl;
public:
	void render(const std::vector<Visual>& visuals) override;
	void resize(int width, int height) override;
	~ScreenDisplay2D();
	ScreenDisplay2D();
private:
	void updateShaderValues();
};
