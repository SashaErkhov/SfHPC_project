#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>

// matrix = [N x M]
// vec = [M x 1]
// res = [N x 1]
double foo(const std::vector<std::vector<int>>& matrix,
		const std::vector<int>& vec) {
	double start_time = omp_get_wtime();
	std::size_t N = matrix.size();
	std::size_t M = vec.size();
	std::vector<int> res(N);
	
	// parallelizm of outer loop
	#pragma omp parallel for default(none) shared(matrix, vec, result, N, M)
	for(std::size_t i=0; i<N; ++i) {
		double sum = 0; //private for each thread
		for (std::size_t j=0;j<M;++j) {
			sum += matrix[i][j] * vec[j]
		}
		res[i] = sum;
	}

	double end_time = omp_get_wtime();
	return (end_time-start_time);
}

int main() {
	return 0;
}
