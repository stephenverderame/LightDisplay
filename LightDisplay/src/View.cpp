#include <View.h>
#include <ScreenView.h>

std::unique_ptr<SignalView> getView()
{
	return std::make_unique<ScreenView>();
}
