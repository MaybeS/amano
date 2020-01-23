#pragma once

#include <iostream>
#include <chrono>
#include <functional>

template<typename T = std::chrono::milliseconds, size_t N = 10>
struct measure {
	template<typename F, typename ...Args>
	static auto execution(F&& f, Args&&... args) {
		auto start = std::chrono::steady_clock::now();

		for (size_t i = 0; i < N; i++) {
			std::forward<decltype(f)>(f)(std::forward<Args>(args)...);
		}

		return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - start);
	}

	static auto logging(const T& duration, const std::string& text) {
		std::cout << text << " (" << N << " times)" << std::endl;
		std::cout << "\tTotal: " << duration.count() << " ms" << std::endl;
		std::cout << "\tMean: " << duration.count() / (double)N << " ms" << std::endl;
	}
};
