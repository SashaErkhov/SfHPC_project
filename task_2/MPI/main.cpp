#include <iostream>
#include <vector>
#include <mpi.h>
#include <cstdlib>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  std::vector<int> sizes = {10, 1000, 10000000};
  double start, end;
  for(int N: sizes) {

    //Распределяем части массива процессам
    std::vector<int> send_counts(size);
    std::vector<int> displacements(size);
    int rem = N % size;
    int offset = 0;
    for(int i = 0;i < size; ++i) {
      send_counts[i] = N / size + (i < rem ? 1 : 0);
      displacements[i] = offset;
      offset += send_counts[i];
    }

    //Главный процесс отвечает за инициализацию массива
    std::vector<int> arr;
    if(rank == 0) {
      arr.resize(N);
      for(std::size_t i=0;i<N;++i) {
        arr[i] = std::rand() % 100;
      }
    }

    //Локальный массив для каждого процесса
    std::vector<int> local_arr(send_counts[rank]);

    //Синхронизация перед таймером
    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    //Рассылка частей массива всем процессам
    MPI_Scatterv(arr.data(), send_counts.data(), displacements.data(), MPI_INT,
                 local_arr.data(), send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = 0;
    for (int i = 0; i < send_counts[rank]; ++i) {
      local_sum += local_arr[i];
    }


    //Сборка локальных сумм в глобальную на главном процессе
    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end = MPI_Wtime();
    if(rank == 0) {
      std::cout << "| "<<N<<" | "<<end-start<<" |"<<std::endl;
    }
  }
  MPI_Finalize();
  return 0;
}
