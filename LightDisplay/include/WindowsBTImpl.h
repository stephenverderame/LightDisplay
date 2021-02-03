#pragma once
#include <WinSock2.h>
#include <ws2bth.h>
#include <Handle.h>
#include <vector>
#include <type_traits>
#include <stdexcept>
/// Plain-old data
template<typename T>
concept POD = std::is_standard_layout_v<std::remove_reference_t<T>> &&
	(std::is_trivial_v<std::remove_reference_t<T>> || 
		std::is_default_constructible_v<std::remove_reference_t<T>>);
/**
 * Guaruntees the sending of data entirely, or an error
 * @param totalSize the size of the data of ptr
 * @return 0 or a WSA error code
 */
inline auto safeSend(Handle_t sock, const char * ptr, int totalSize) {
	int err = 0;
	decltype(totalSize) sent = 0;
	do {
		err = send(sock, ptr + sent,
			totalSize - sent, 0);
		if (err > 0) sent += err;
		else {
			err = -WSAGetLastError();
		}
	} while (sent < totalSize && err > 0);
	return err < 0 ? -err : 0;
}
/**
 * Guaruntees the sending of data entirely, or an error
 * @return 0 or a WSA error code
 * @{
 */
template<POD T>
inline int sendAll(Handle_t sock, const std::vector<T>& data) {
	const auto totalSize = static_cast<int>(data.size() * sizeof(T));
	return safeSend(sock, reinterpret_cast<const char*>(&data[0]), totalSize);
}
template<POD T>
inline int sendAll(Handle_t sock, T&& data) {
	const char* ptr = reinterpret_cast<const char*>(&data);
	const auto totalSize = static_cast<int>(sizeof(std::remove_reference_t<T>));
	return safeSend(sock, ptr, totalSize);

}
/// @}
/**
 * Guaruntees the receiving of data entirely, or an error
 * @param data the buffer to store the data which must fit totalSize bytes
 * @param totalSize the size of the data of ptr
 * @return 0 or a WSA error code
 */
inline auto safeRecv(Handle_t sock, char* data, int totalSize) {
	int err = 0;
	decltype(totalSize) got = 0;
	do {
		err = recv(sock, data + got,
			totalSize - got, 0);
		if (err > 0) got += err;
		else {
			err = -WSAGetLastError();
		}
	} while (got < totalSize && err > 0);
	return err < 0 ? -err : 0;
}
/**
 * Gets all the data of the plain old data type
 * @param <T> POD type to read
 * @param sock the sock to read on
 */
template<POD T>
T getAll(Handle_t sock) {
	T t{};
	int err = safeRecv(sock, reinterpret_cast<char*>(&t), sizeof(T));
	if (err != 0) {
		fprintf(stderr, "Recv failure: %d\n", err);
		throw std::runtime_error("Recv fail");
	}
	return t;
}
/**
 * Creation Policy to create sockets and socket address structures
 */
template<typename T>
concept SocketPolicy = requires(T p) {
	{std::declval<typename T::Address_t>()};
	{T::createSocket() };
	{T::createAddr(std::declval<unsigned short>(), std::declval<unsigned>())};
};
class BluetoothStreamPolicy {
public:
	using Address_t = BTH_ADDR;
	using SockAddr_t = SOCKADDR_BTH;
	static Handle_t createSocket() {
		auto res = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (res == INVALID_SOCKET)
			throw std::runtime_error("Cannot create socket");
		return res;
	}
	static SOCKADDR_BTH createAddr(unsigned short port, Address_t addr = 0) {
		SOCKADDR_BTH addrInfo;
		ZeroMemory(&addrInfo, sizeof(addrInfo));
		addrInfo.port = htons(port);
		addrInfo.addressFamily = AF_BTH;
		addrInfo.btAddr = addr;
		return addrInfo;
	}
};
class Ipv4TcpPolicy {
public:
	using Address_t = decltype(inet_addr(""));
	using SockAddr_t = SOCKADDR_IN;

	static Handle_t createSocket() {
		auto res = ::socket(AF_INET, SOCK_STREAM, 0);
		if (res == INVALID_SOCKET) {
			fprintf(stderr, "Socket create fail %d\n", WSAGetLastError());
			throw std::runtime_error("Cannot create socket");
		}
		return res;
	}
	static SOCKADDR_IN createAddr(unsigned short port, Address_t addr = INADDR_ANY) {
		SOCKADDR_IN addrInfo;
		ZeroMemory(&addrInfo, sizeof(addrInfo));
		addrInfo.sin_port = htons(port);
		addrInfo.sin_family = AF_INET;
		addrInfo.sin_addr.s_addr = addr;
		return addrInfo;
	}
};
