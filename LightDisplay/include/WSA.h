#pragma once
#include <WinSock2.h>
#include <stdexcept>
#include <atomic>
class WSA {
	static std::atomic<unsigned> refCount;
public:
	WSA() = delete;
	/**
	 * Initializes winsock if it has not been done so already
	 * Throws bad_alloc on fail
	 */
	static void init() {
		if (refCount++ == 0) {
			WSAData data;
			if (WSAStartup(MAKEWORD(2, 1), &data) != 0)
				throw std::bad_alloc();
		}
	}
	/**
	 * Cleans up winsock if nobody is using it
	 */
	static void cleanup() {
		if (--refCount == 0) {
			WSACleanup();
		}
	}
};
