#pragma once

#include <chrono>
#include <iostream>

class Benchmark {
private:
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
public:
	Benchmark() {
		this->start = std::chrono::high_resolution_clock::now();
	}

	long long end_benchmark(bool show_time = true) {
		end = std::chrono::high_resolution_clock::now();

		auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		
		if (show_time)
			std::cout << ms << "us\n";

		return ms;
	}
};