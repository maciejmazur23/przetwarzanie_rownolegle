#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <algorithm> // Do sortowania

void printPrimes(const std::vector<int> &primes)
{
    for (size_t i = 0; i < primes.size(); ++i)
    {
        if (i > 0 && i % 10 == 0)
        {
            std::cout << std::endl;
        }
        std::cout << primes[i] << " ";
    }
    std::cout << "\nZnaleziono " << primes.size() << " liczb pierwszych." << std::endl;
}

void parallelCube(int M, int N, std::vector<int> &primes)
{
    std::vector<bool> isPrime(N + 1, true);
    isPrime[0] = isPrime[1] = false;
    int limit = std::sqrt(N);

    int NUMBER_OF_THREADS = 8;
    int DOMAIN_LENGTH = limit;
    for (int p = 2; p <= limit; ++p){
        if (isPrime[p]){
            for (int i = p * p; i <= N; i += p){
                isPrime[i] = false;
            }
        }
    }

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        int id = omp_get_thread_num();
        std::vector<int> localPrimes;
        
        #pragma omp for schedule(dynamic, DOMAIN_LENGTH)
        for (int i = M; i <= N; i++){
            if (isPrime[i]){
                localPrimes.push_back(i);
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }

}

int main(int argc, char* argv[])
{
      int M = std::stoi(argv[1]);
    int N = std::stoi(argv[2]);
    std::vector<int> primes;

   clock_t start, stop;
    start = clock();
    
    parallelCube(M, N, primes);
    
    stop = clock();
    double time = (double)(stop - start)/CLOCKS_PER_SEC;
    std::cout <<" Trwało " << time << " sekund" << std::endl;

    // printPrimes(primes);

    return 0;
}
