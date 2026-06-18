#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <limits>

void foo(const std::vector<int>& arr, int& resMax, int& resCount) {
  std::size_t n = arr.size();
  if (n==0) {
    resMax = 0;
    resCount = 0;
    return;
  }
  resMax = std::numeric_limits<int>::min();
  resCount = 0;
  
  #pragma omp parallel
  {
    int localMax = std::numeric_limits<int>::min();
    int localCount = 0;
    
    //параметр nowait отключает синхронизацию в конце цикла, сразу в блок critical
    #pragma omp for nowait
    for (std::size_t i = 0; i < n; ++i) {
      if (arr[i] > localMax) {
        localMax = arr[i];
        localCount = 1;
      } else if (arr[i] == localMax) {
        ++localCount;
      }
    }

    #pragma omp critical
    {
      if (localMax > resMax) {
        resMax = localMax;
        resCount = localCount;
      } else if (localMax == resMax) {
        resCount += localCount;
      }
    }
  }
}

int main() {
  for (std::size_t N = 10; N<=1000000; N*=10) {
    std::vector<int> arr(N);
    for (std::size_t i=0;i<N;++i) {
      arr[i] = std::rand() % 100;
    }
    double time = 0.;
    double start = 0.;
    double end = 0.;
    int res1, res2;
    for (std::size_t i=0;i<100;++i) {
      start = omp_get_wtime();
      foo(arr, res1, res2);
      end = omp_get_wtime();
      time += end-start;
    }
    time /= 100;
    std::cout << "| "<<N<<" | "<<time<<" |"<<std::endl;
  }
  return 0;
}
