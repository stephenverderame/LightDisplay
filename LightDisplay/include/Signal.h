#pragma once
#include <vector>
#include <complex>
using signal_t = std::vector<std::complex<double>>;
using cmplx_t = std::complex<double>;

template<typename T>
inline std::enable_if_t<std::is_integral_v<T>, T>
nextPowerOf2(T n)
{
	T i = 1;
	const T two = 2;
	while ((two << i) < n) ++i;
	return (two << i);
}
