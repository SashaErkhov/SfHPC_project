#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>

// matrix = [N x M]
// vec = [M x 1]
// res = [N x 1]
double foo(const std::vector<std::vector<int>>& matrix,
		const std::vector<int>& vec) {
	std::size_t N = matrix.size();
	std::size_t M = vec.size();
	std::vector<int> res(N);
	
	double start_time = omp_get_wtime();
	// parallelizm of outer loop
	#pragma omp parallel for default(none) shared(matrix, vec, res, N, M)
	for(std::size_t i=0; i<N; ++i) {
		int sum = 0; //private for each thread
		for (std::size_t j=0;j<M;++j) {
			sum += matrix[i][j] * vec[j];
		}
		res[i] = sum;
	}

	double end_time = omp_get_wtime();
	return (end_time-start_time);
}

int main() {
	std::size_t N = 10;
	while (N < 11000){
		std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 0));
		std::vector<int> vec(N,0);
		for(std::size_t i=0;i<N;++i) {
			for(std::size_t j=0;j<N;++j) {
				matrix[i][j] = std::rand() % 100;
			}
			vec[i] = std::rand()%100;
		}
		double average = 0.;
		for(std::size_t i=0;i<100;++i) {
			average += foo(matrix, vec);
		}
		average /= 100;
		std::cout<<"N = "<<N<<" Time = "<<average<<std::endl;
		N *= 2;

	}
	return 0;
}
