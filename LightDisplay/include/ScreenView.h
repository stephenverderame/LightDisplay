#pragma once
#include <View.h>
class ScreenView : public SignalView {
	struct impl;
	std::unique_ptr<impl> pimpl;
public:
	ScreenView();
	void draw(const std::vector<Visual>& visuals) override;
	bool isActive() override;
	~ScreenView();
};
