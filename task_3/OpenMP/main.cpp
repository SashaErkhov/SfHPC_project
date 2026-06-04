#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>

double foo(const std::vector<long long>& arr) {
	double start_time = omp_get_wtime();
	long long sum = 0;
	std::size_t N = arr.size();
	
	#pragma omp parallel for reduction(+:sum)
	for(std::size_t i=0; i<N; ++i) {
		sum += arr[i];
	}

	double end_time = omp_get_wtime();
	return (end_time-start_time);
}

int main() {
	std::size_t N = 10;
	std::cout << "N = " << N << " Time = ";
	std::vector<long long> arr(N);
	for (std::size_t i=0;i<N;++i) {
		arr[i] = std::rand() % 100; // just example
	}
	std::cout << foo(arr) << std::endl;
	N = 1000;
	arr.resize(N);
	for (std::size_t i=0;i<N;++i) {
		arr[i] = std::rand() % 100;
	}
	std::cout << "N = " << N << " Time = " << foo(arr) << std::endl;
	N = 10000000;
	arr.resize(N);
	for (std::size_t i=0;i<N;++i) {
		arr[i] = std::rand() % 100;
	}
	std::cout << "N = " << N << " Time = " << foo(arr) << std::endl;
	return 0;
}
