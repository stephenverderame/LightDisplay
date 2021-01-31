#pragma once
#include <functional>
#include <type_traits>
template<typename T, typename Arg>
concept Functor = requires(T a, Arg arg) {
	{ a(arg) };
};

template<typename T>
concept Handle = std::is_integral_v<T> || std::is_pointer_v<T>;

/**
 * Class to ensure the call of a specified one argument functor when it goes out of scope
 */
template<typename T, Functor<T> F> requires Handle<T>
	class ResourceWrapper {
	private:
		T t;
		F f;
	public:
		/**
		 * Constructs a resource wrapper to wrap t and call f on destruction
		 */
		ResourceWrapper(T t, F&& f) : t(t),
			f(std::forward<std::remove_reference_t<F>>(f)) {};
		~ResourceWrapper() { f(t); }
		ResourceWrapper(const ResourceWrapper&) = delete;
		ResourceWrapper& operator=(const ResourceWrapper&) = delete;
		T operator->() { return t; }
		T get() { return t; }
	};
