#include <iostream>
#include <vector>
#include <mpi.h>
#include <cstdlib>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  std::vector<int> sizes;
  sizes.resize(8);
  for(std::size_t i=0, x = 100; i<8;++i, x*=2) {
    sizes[i] = x;
  }
  double start, end;
  for (int N : sizes) {
    // Распределяем строки и элементы матрицы по процессам
    std::vector<int> send_counts(size);
    std::vector<int> displacements(size);
    std::vector<int> recv_counts(size);
    std::vector<int> recv_displacements(size);
    int rem = N % size;
    int matrix_offset = 0;
    int vector_offset = 0;
    for (int i = 0; i < size; ++i) {
      int rows = N / size + (i < rem ? 1 : 0);
      send_counts[i] = rows * N;
      displacements[i] = matrix_offset; 
      matrix_offset += send_counts[i];
      recv_counts[i] = rows;
      recv_displacements[i] = vector_offset;
      vector_offset += recv_counts[i];
    }

    //Главный процесс отвечает за инициализацию
    std::vector<int> matrix;
    std::vector<int> vec(N);
    std::vector<int> result;
    if (rank == 0) {
      matrix.resize(N * N);
      result.resize(N);
      for (int i = 0; i < N * N; ++i) {
        matrix[i] = std::rand() % 100;
      }
      for (int i = 0; i < N; ++i) {
        vec[i] = std::rand() % 100;
      }
    }

    // Рассылка вектора-множителя всем процессам целиком
    MPI_Bcast(vec.data(), N, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальные массивы для каждого процесса
    int local_elements = send_counts[rank];
    int local_rows = recv_counts[rank];
    std::vector<int> local_matrix(local_elements);
    std::vector<int> local_result(local_rows, 0);

    //Синхронизация перед таймером
    MPI_Barrier(MPI_COMM_WORLD);
    
    start = MPI_Wtime();

    // Рассылка полос матрицы процессам
    MPI_Scatterv(matrix.data(), send_counts.data(), displacements.data(), MPI_INT,
                local_matrix.data(), local_elements, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальное умножение матрицы на вектор
    for (int i = 0; i < local_rows; ++i) {
      for (int j = 0; j < N; ++j) {
        local_result[i] += local_matrix[i * N + j] * vec[j];
      }
    }

    // Сборка кусков результирующего вектора на главном процессе
    MPI_Gatherv(local_result.data(), local_rows, MPI_INT,
                result.data(), recv_counts.data(), recv_displacements.data(), MPI_INT,
                0, MPI_COMM_WORLD);
    
    end = MPI_Wtime();
    if (rank == 0) {
        std::cout << "| " << N << " | " << end - start << " |" << std::endl;
    }
  }
  MPI_Finalize();
  return 0;
}
