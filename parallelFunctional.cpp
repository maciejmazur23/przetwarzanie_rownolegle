#include <omp.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

void findMultiple(std::vector<int>& isPrime, int prime, int end){
    for(int i = prime*prime; i<=end; i+= prime){
        isPrime[i] = 0;
    }
}

void parallelFunction(int M, int N, std::vector<int>&primes){
    int NUMBER_OF_THREADS = 8;

    std::vector<int> isPrime(N+1, 1);
    isPrime[0] = 0;
    isPrime[1] = 0;
    int limit = static_cast<int>(sqrt(N));


    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        #pragma omp for schedule(dynamic)
        for(int i = 2; i<=limit; i++){
            if(isPrime[i] == 1){
                findMultiple(isPrime, i, N);
            }
        }

        #pragma omp for
        for(int i = M; i<=N; i++){
            if(isPrime[i] == 1){
                #pragma omp critical
                primes.push_back(i);
            }
        }

        
    }
}

int main(int argc, char* argv[]){
    int M = std::stoi(argv[1]);
    int N = std::stoi(argv[2]);
    std::vector<int> primes;
    
    clock_t start, stop;
    start = clock();
    
    parallelFunction(M, N, primes);
    
    stop = clock();
    double time = (double)(stop - start)/CLOCKS_PER_SEC;
    std::cout <<" Trwało " << time << " sekund" << std::endl;
/*
    std::cout << "Primes: ";
 
    for(int i : primes){
        std::cout<<i<<" ";
    }
    std::cout<<std::endl;
*/

    
    return 0;
}



    //   <td>0.014186</td>
    //   <td>0.03125</td>
    //   <td>0.328125</td>