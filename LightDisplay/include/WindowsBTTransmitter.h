#pragma once
#include <VisualTransmitter.h>
#include <Handle.h>
#include <WindowsBTImpl.h>

template<SocketPolicy sp>
class WindowsBT : public VisualTransmitter {
	Handle_t socket;
public:
	WindowsBT(typename sp::Address_t btAddr);
	WindowsBT();
	~WindowsBT();
	void transmit(const std::vector<Visual>& data) override;
};

using WindowsBTTransmitter = WindowsBT<BluetoothStreamPolicy>;
using WindowsTCPTransmitter = WindowsBT<Ipv4TcpPolicy>;
