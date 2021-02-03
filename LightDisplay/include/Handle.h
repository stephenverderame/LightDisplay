#pragma once
template<int sz>
struct HandleHolder {};
template<>
struct HandleHolder<2> {
	using val = unsigned short;
};
template<>
struct HandleHolder<4> {
	using val = unsigned long;
};
template<>
struct HandleHolder<8> {
	using val = unsigned long long;
};
/// Integral type that can store an address
using Handle_t = typename HandleHolder<sizeof(void*)>::val;
