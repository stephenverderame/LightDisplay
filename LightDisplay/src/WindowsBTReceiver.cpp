#include <WindowsBTReceiver.h>
#include <WindowsBTImpl.h>
#include <WSA.h>
#include <ws2bth.h>
#include <stdexcept>

template<SocketPolicy sp>
WindowsBTReceiver<sp>::WindowsBTReceiver() : client(INVALID_SOCKET)
{
	WSA::init();
	this->server = sp::createSocket();
	addr = sp::createAddr(8008);
	if (bind(this->server, (sockaddr*)&addr, sizeof(addr)) != 0) {
		fprintf(stderr, "Failed to bind! %d\n", WSAGetLastError());
		throw std::runtime_error("Failed to bind!");
	}
	if (listen(this->server, SOMAXCONN) != 0) {
		fprintf(stderr, "Failed to listen! %d\n", WSAGetLastError());
		throw std::runtime_error("Failed to listen");
	}
}
template<SocketPolicy sp>
WindowsBTReceiver<sp>::~WindowsBTReceiver()
{
	if(server != INVALID_SOCKET) closesocket(this->server);
	if (client != INVALID_SOCKET) closesocket(client);
	WSA::cleanup();
}
template<SocketPolicy sp>
void WindowsBTReceiver<sp>::accept()
{
	int sz = static_cast<int>(sizeof(addr));
	client = ::accept(server, (sockaddr*)&addr, &sz);
	if (client == INVALID_SOCKET) {
		fprintf(stderr, "Error accepting client: %d\n", WSAGetLastError());
		throw std::runtime_error("Failed to accept");
	}
}

template<SocketPolicy sp>
std::vector<Visual> WindowsBTReceiver<sp>::recv()
{
	const auto sz = getAll<unsigned long long>(client);
	std::vector<Visual> visuals(sz / sizeof(Visual));
	int err = safeRecv(client, reinterpret_cast<char*>(&visuals[0]), static_cast<int>(sz));
	if (err != 0) {
		fprintf(stderr, "Could not recv %d\n", err);
		throw std::runtime_error("Could not recv data");
	}
	return visuals;
}

template WindowsBTReceiver<BluetoothStreamPolicy>;
template WindowsBTReceiver<Ipv4TcpPolicy>;
