#pragma once
#include <VisualTransmitter.h>
template<int sz>
struct Handle {};
template<>
struct Handle<4> {
	using val = unsigned long;
};
template<>
struct Handle<8> {
	using val = unsigned long long;
};
using Handle_t = typename Handle<sizeof(void*)>::val;

class WindowsBT : public VisualTransmitter {
	static unsigned refCount;
	Handle_t socket;
public:
	WindowsBT();
	~WindowsBT();
	void transmit(const std::vector<Visual>& data) override;
};
