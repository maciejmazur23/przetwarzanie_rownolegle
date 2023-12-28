#include <iostream>
#include <cmath>
#include <time.h>
#include <vector>
#include <cmath>
#include <omp.h>

// KOSTKA LICZB PIERWSZYCH
std::vector<int> seqCube(int start, int end) {
    std::vector<int> primes;
    std::vector<bool> prime(end + 1, true);
    prime[0] = prime[1] = false;

    for (int i = 2; i * i <= end; i++) {
        if (prime[i]) {
            for (int j = std::max(i * i, (start + i - 1) / i * i); j <= end; j += i)
                prime[j] = false;
        }
    }

    for (int i = start; i <= end; i++) {
        if (prime[i])
            primes.push_back(i);
    }

    return primes;
}

// SEKWENCYJNIE KLASYCZNIE
bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    int end = std::sqrt(n);
    for (int i = 3; i <= end; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

int calculatePrimes(int start, int end) {
    std::vector<int> primeList;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            primeList.push_back(i);
        }
    }

    return primeList.size();
}
/////////////////////////////

template<typename Func, typename... Args>
void measureExecutionTime(std::string message,Func func, Args... args) {
    clock_t spstart, spstop;
    spstart = clock();
    func(args...);
    spstop = clock();
    double time = (message, (double)(spstop - spstart)/CLOCKS_PER_SEC);
    std::cout<<message<<" "<<time<<" sekund"<<std::endl;
}

int main(int argc, char *argv[]) {

    int start = 2;
    int end = 50000000;


    // if(argc == 3) {
        // start = atoi(argv[1]);
        // end = atoi(argv[2]);
    // }
    // std::cout<<calculatePrimes(start, end)<<std::endl;
     measureExecutionTime("Po kolei", calculatePrimes ,start, end);
    measureExecutionTime("Sito", seqCube, start, end);
    return 0;
}
