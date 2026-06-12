#include <iostream>
#include <omp.h>
#include <vector>
#include <random>
#include <cmath>

double foo(std::size_t total) {
	std::size_t in = 0;
	#pragma omp parallel reduction(+:in)
	{
		unsigned seed = 12345 + omp_get_thread_num();
		// Обеспечивает разные последовательности случайных чисел у разных потоков
		
		std::mt19937 gen(seed);
		std::uniform_real_distribution<double> dist(0.,1.);

		#pragma omp for
		for(std::size_t i=0;i<total;++i) {
			double x = dist(gen);
			double y = dist(gen);
			if (x*x + y*y <= 1) {
				++in;
			}
		}
	}
	return 4. * static_cast<double>(in)/static_cast<double>(total);
}

int main() {
	const double PI = 3.14159265358979323846;
	std::size_t N = 10000;
	while (N < 100000001) {
		double time = 0.;
		double pi = 0.;
		double start = 0.;
		double end = 0.;
		for(std::size_t i=0;i<100;++i) {
			start = omp_get_wtime();
			pi = foo(N);
			end = omp_get_wtime();
			time += end-start;
		}
		time /= 100;
		std::cout<<"N = "<<N<<" Time = "<<time<<" abs = "<<std::abs(pi-PI)<<std::endl;
		N *= 10;
	}
	return 0;
}
