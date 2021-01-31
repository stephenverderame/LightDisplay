#include <VisualTransmitter.h>
#include <WindowsBTTransmitter.h>
std::unique_ptr<VisualTransmitter> getTransmitter()
{
	return std::make_unique<WindowsBT>();
}
