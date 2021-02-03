#pragma once
#include <VisualReceiver.h>
#include <Handle.h>
#include <WindowsBTImpl.h>

template<SocketPolicy sp>
class WindowsBTReceiver : public VisualReceiver {
	Handle_t server, client; ///< Invalid_Socket iff not open
	typename sp::SockAddr_t addr;
public:
	WindowsBTReceiver();
	~WindowsBTReceiver();
	void accept() override;
	std::vector<Visual> recv() override;
};
