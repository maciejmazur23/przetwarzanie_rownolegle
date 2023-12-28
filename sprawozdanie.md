<div style="font-size: 20px;" align="center">
    <div style="font-weight: 500; font-size: 2em; line-height: 50px;"><bold>Obliczanie liczb pierwszych</bold></div>
    <br/>
    <p style="font-weight: 500;">Laboratorium z Przetwarzania Równoległego</p>
    <br/>
     Autor: Maciej Mazur 151853
    <br/><br/>
    Wymagany termin oddania: 30.12.2023
    <br/>
    Rzeczywisty termin oddania: 30.12.2023
    <br/><br/>
    Wersja 1
    <br/><br/>
    Email: maciej.mazur@student.put.poznan.pl
    <br/><br/><br/>
</div>
<div style="page-break-after: always;"></div>

## 1.1 Wstęp

Projekt zakłada analizę efektywności przetwarzania równoległego na komputerze z procesorem wielordzeniowym, ze skupieniem się na problemie znajdowania liczb pierwszych w podanym przedziale <M..N>. Przygotowane są różne wersje algorytmów sekwencyjnych i równoległych. Projekt obejmie także różne warianty zrównoleglenia, uwzględniając podejścia podziału domenowego i funkcyjnego.

Wszystkie obliczenia dokonywano na komputerze Lenovo Ideapad Gamping 3 z procesorem wielordzeniowym i kompilatorem obsługującym dyrektywy OpenMP.

Przygotowane wersje kodu pozwolą na przeprowadzenie eksperymentów dotyczących poprawności wyników i pomiaru czasu przetwarzania.
W ramach projektu przeprowadzone zostały testy poprawności, pomiary czasu i analiza efektywności przetwarzania równoległego, a wyniki eksperymentów szczegółowo przedstawiono w dalszej części sprawozdania.

## 1.2 Opis Wykorzystanego Systemu Obliczeniowego

1. **Procesor**:
   - **Oznaczenie**: AMD Ryzen 5 4600H with Radeon Graphics
   - **Liczba procesorów fizycznych**: 1 
   - **Liczba procesorów logicznych**: 6
   - **Liczba wątków**: 12
   - **Typ procesora**: procesor x64

2. **Dodatkowe specyfikacje**:
   - **Pamięć RAM**: 8Gb

3. **System operacyjny**:
   - **Wersja**: Windows 11 Home

4. **Oprogramowanie do kodowania i testów**:
   - **Nazwa**: Visual Studio Code

## 1.2 Znaczenie i cel ekspreymentu
Kluczowym elementem eksperymentu jest ocena algorytmów i strategii zrównoleglania w kontekście znajdowania liczb pierwszych. Projekt skupia się na analizie wydajności, równoważenia pracy procesorów, lokalności dostępu do danych i optymalizacji przetwarzania równoległego. Wykorzystanie dyrektyw OpenMP jest istotnym elementem, mającym na celu znalezienie optymalnych strategii dla danego problemu. Dodatkowo, eksperyment pozwala na zrozumienie wpływu tych elementów na skuteczność przetwarzania równoległego.
<div style="page-break-after: always;"></div>

## 2. Prezentacja Wariantów Kodów
**Wariant 1 - SeqClassic**
   ```cpp
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
   ```
    Ten sekwencyjny algorytm, jest najprostrzą wersją znajdowania liczb pierwszych. Przechodzi on przez wszystkie liczby w danym zakresie, od start do end, i dla każdej liczby sprawdza czy ma ona jakieś dzielniki, inne niż ona sama i 1. Z racji, że ten algorytm nie jest wielowątkowy, nie użyto w nim dyrektyw OpenMP i jest wolny od problemów związanych z wielowątkowością, takich jak wyścig czy False Sharing.

<br/><br/>

**Wariant 2 - SeqCube:**
  ```cpp
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
  ```

    Ten algorytm to sekwencyjna wersja Sita Eratostenesa, która znajduje liczby pierwsze. Algorytm ten wykorzystuje wektor boolowski prime do oznaczania liczb jako pierwszych lub złożonych. Początkowo wszystkie liczby są oznaczone jako potencjalnie pierwsze. Następnie dla każdej liczby pierwszej jej wielokrotności są oznaczane jako złożone. W efekcie, pozostają jedynie liczby pierwsze w danym zakresie. W kontekście wydajności, algorytm ten eliminuje potrzebę sprawdzania każdej liczby osobno, co przyspiesza proces znajdowania liczb pierwszych w dużych zakresach.

<br/><br/>

**Wariant 3 - ParallelClassicStatic:**
```cpp
bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    int limit = std::sqrt(num);
    for (int i = 3; i <= limit; i += 2) {
        if (num % i == 0) return false;
        
    }
    return true;
}

std::vector<int> parallelClassic(int start, int end) {
    std::vector<int> primes;

    int NUMBER_OF_THREADS = 3;
    
    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        std::vector<int> local_primes;
        #pragma omp for schedule(static)
        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                local_primes.push_back(i);
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), local_primes.begin(), local_primes.end());
        }
    }

    return primes;
}

```
Ten algorytm przechodzi przez wszystkie liczby, do pierwiastka końca danego przedziału, i sprawdza czy dane liczby mają dzielniki inne niż 1 i ona sama. Algorytm działa wielowątkowo. W powyższym algorytmie każdy wątek równolegle sprawdza liczby. Podział pracy jest równomierny, a dyrektywa OpenMP `#pragma omp for schedule(static)` zapewnia, że zadania są przydzielane równomiernie.Z astosowanie sekcji krytycznej `#pragma omp critical` eliminuje problem wyścigu, ale może wpływać na wydajność, gdyż tylko jeden wątek może w danym momencie wykonywać operacje w sekcji krytycznej. False sharing nie występuje, ponieważ operacje na danych wątków są wydzielone do wektora local_primes, co minimalizuje konflikty w dostępie do pamięci współdzielone. Wybrany sposób podziału pracy i przydziału zadań pozwala na zrównoważone wykorzystanie zasobów. 

<br/><br/>

**Wariant 4 - ParallelClassicDynamic:**
```cpp
bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    int limit = std::sqrt(num);
    for (int i = 3; i <= limit; i += 2) {
        if (num % i == 0) return false;
        
    }
    return true;
}

std::vector<int> parallelClassic(int start, int end) {
    std::vector<int> primes;

    int NUMBER_OF_THREADS = 3;
    
    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        std::vector<int> local_primes;
        #pragma omp for schedule(dynamic)
        for (int i = start; i <= end; i++) {
            if (isPrime(i)) {
                local_primes.push_back(i);
            }
        }

        #pragma omp critical
        {
            primes.insert(primes.end(), local_primes.begin(), local_primes.end());
        }
    }

    return primes;
}

```

Algorytm ten równolegle znajduje liczby pierwsze w zakresie od "start" do "end". Funkcja isPrime sprawdza, czy liczba jest liczbą pierwszą.
Główna funkcja parallelClassic używa dyrektyw OpenMP do równoległego przetwarzania. Pętla `#pragma omp parallel` tworzy określoną liczbę wątków, a pętla `#pragma omp for schedule(dynamic)` przydziela zadania każdemu wątkowi w sposób dynamiczny. Wątki zbierają wyniki w tablicy `primes`, a lokalne zliczenia są przechowywane w `local_primes`. Algorytm skupia się na efektywnym wykorzystaniu dostępnych wątków, minimalizując równocześnie koszty synchronizacji.

<br/><br/>

**Wariant 5 - ParallelDomainStatic:**
```cpp
    std::vector<bool> isPrime(end + 1, true);
    isPrime[0] = isPrime[1] = false;
    int limit = std::sqrt(end);

    int NUMBER_OF_THREADS = 5;
    int DOMAIN_LENGTH = limit;
    
    for (int p = 2; p <= limit; ++p){
        if (isPrime[p]){
            for (int i = p * p; i <= end; i += p){
                isPrime[i] = false;
            }
        }
    }

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        std::vector<int> localPrimes;
        
        #pragma omp for schedule(static, DOMAIN_LENGTH)
        for (int i = start; i <= end; i++){
            if (isPrime[i]){
                localPrimes.push_back(i);
            }
        }
  
        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }

```

Ten algorytm to domenowa wersja Sita Eratostenesa, działajśca równolegle. Algorytm znajduje liczby pierwsze w zakresie od start do end, dzieląc pracę równo między wątki. Wykorzystuje dyrektywy OpenMP, gdzie `#pragma omp parallel` tworzy obszar równoległy, a `#pragma omp for schedule(static, DOMAIN_LENGTH)` parallelizuje pętlę z równomiernym podziałem zadań. Każdy wątek dostaje taką samą liczbę liczb do sprawszenia, przekazaną w parametrze. Brak wyścigów, gdyż każdy wątek operuje na niezależnych obszarach tablicy isPrime. Minimalne ryzyko false sharing, a `#pragma omp critical` zapewnia bezpieczne łączenie wyników. Algorytm efektywnie wykorzystuje wątki do równoczesnego przetwarzania i minimalizuje synchronizację dla lepszej wydajności.

<br/><br/>


**Wariant 6 - ParallelDomainDynamic:**
```cpp
    std::vector<bool> isPrime(end + 1, true);
    isPrime[0] = isPrime[1] = false;
    int limit = std::sqrt(end);

    int NUMBER_OF_THREADS = 5;
    int DOMAIN_LENGTH = limit;

    for (int p = 2; p <= limit; ++p){
        if (isPrime[p]){
            for (int i = p * p; i <= end; i += p){
                isPrime[i] = false;
            }
        }
    }

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        std::vector<int> localPrimes;
        
        #pragma omp for schedule(dynamic, DOMAIN_LENGTH)
        for (int i = start; i <= end; i++){
            if (isPrime[i]){
                localPrimes.push_back(i);
            }
        }
  
        #pragma omp critical
        {
            primes.insert(primes.end(), localPrimes.begin(), localPrimes.end());
        }
    }

```

Podobnie jak poprzedni algorytm, ten to domenowa wersja Sita Eratostenesa, działajśca równolegle. Algorytm znajduje liczby pierwsze w zakresie od start do end, dzieląc pracę równo między wątki. Jedyna różnica w kodzie względem poprzedniego wariantu to zastosowanie dynamicznego przydzielania pracy do wątków. Sprawia to, że elastyczność przydzielania zadań w pętli równoległej jest większa. To ile zadań dostanie wątek będzie zależało od tego jak szybko będzie kończył poprzednią pracę. Brak wyścigów, gdyż każdy wątek operuje na niezależnych obszarach tablicy isPrime. Minimalne ryzyko false sharing, a `#pragma omp critical` zapewnia bezpieczne łączenie wyników. Algorytm efektywnie wykorzystuje wątki do równoczesnego przetwarzania i minimalizuje synchronizację dla lepszej wydajności.

<br/><br/>

**Wariant 7 - ParallelFunctionalDynamic:**
```cpp
  
    int NUMBER_OF_THREADS = 5;
    std::vector<int> isPrime(end+1, 1);
    isPrime[0] = 0;
    isPrime[1] = 0;
    int limit = static_cast<int>(sqrt(end));

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        #pragma omp for schedule(dynamic)
        for(int i = 2; i<=limit; i++){
            if(isPrime[i] == 1){
                findMultiple(isPrime, i, N);
            }
        }

        #pragma omp for
        for(int i = start; i<=end; i++){
            if(isPrime[i] == 1){
                #pragma omp critical
                primes.push_back(i);
            }
        }
    }

```
Ten algorytm to funkcyjne Sito Eratostenesa, który równolegle znajduje liczby pierwsze w przedziale od start do end. Wartości isPrime są aktualizowane dynamicznie w pętli, eliminując niepotrzebne obliczenia dla wielokrotności liczb pierwszych. Dyrektywy OpenMP, takie jak `#pragma omp parallel` i `#pragma omp for schedule(dynamic)`, umożliwiają efektywne wykorzystanie wielu wątków. Zastosowanie `#pragma omp critical` przy dodawaniu do wektora primes minimalizuje ryzyko wyścigów.

<br/><br/>


**Wariant 8 - ParallelFunctionalStatic:**
```cpp
    int NUMBER_OF_THREADS = 5;
    std::vector<int> isPrime(end+1, 1);
    isPrime[0] = 0;
    isPrime[1] = 0;
    int limit = static_cast<int>(sqrt(end));

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        #pragma omp for schedule(static)
        for(int i = 2; i<=limit; i++){
            if(isPrime[i] == 1){
                findMultiple(isPrime, i, N);
            }
        }

        #pragma omp for
        for(int i = start; i<=end; i++){
            if(isPrime[i] == 1){
                #pragma omp critical
                primes.push_back(i);
            }
        }
    }

```
Ten algorytm, tak jak poprzedni wariant, to funkcyjne Sito Eratostenesa, który równolegle znajduje liczby pierwsze w przedziale od start do end. W tym kodzie zadania są przydzielane do wątków statycznie. Może to prowadzić do mniej efektywnego wykorzystania zasobów i potencjalnie nierównomiernego obciążenia wątków, co może wpływać na ogólną wydajność programu. Dyrektywy OpenMP użyte w kodzie to `#pragma omp parallel` , `#pragma omp for schedule(static)`, `#pragma omp for` oraz `#pragma omp critical`. Zastosowanie `#pragma omp critical` przy dodawaniu do wektora primes minimalizuje ryzyko wyścigów.

<br/><br/>

## Punkt 3: Prezentacja Wyników i Omówienie Eksperymentu
**a) Testowane Wersje Kodów:**
<br/> 
- **SeqClassic**
- **SeqCube**
- **ParallelClassicStatic**
- **ParallelClassicDynamic**
- **ParallelDomainStatic**
- **ParallelDomainDynamic**
- **ParallelFunctionalStatic**
- **ParallelFunctionalDynamic**

**b) Tabela Wyników:**
- **SeqClassic**
<center>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.1406</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>4.26562</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>34.9531</td>
    </tr>
  </table>
</center>

- **SeqCube**
<center>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.014186</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>0.03125</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.328125</td>
    </tr>
  </table>
</center>

<br/>

***Przspieszenie w następnych wersjach kodu będzie liczone względem Klasycznej wersji Sekwencyjnej ,,SeqClassic"***

<br/>

- **ParallelClassicStatic**

<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.155975</td>
      <td>0.9</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>4.01875</td>
      <td>1.06</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>29.5244</td>
      <td>1.184</td>
    </tr>
  </table>
</center>

<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.093327</td>
      <td>1.51</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>2.67857</td>
      <td>1.593</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>21.0221</td>
      <td>1.663</td>
    </tr>
  </table>
</center>

<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.098006</td>
      <td>1.44</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>1.82882</td>
      <td>2.33</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>15.4302</td>
      <td>2.27</td>
    </tr>
  </table>
</center>

- **ParallelClassicDynamic**

<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.130231</td>
      <td>1.08</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>3.28964</td>
      <td>1.297</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>24.9798</td>
      <td>1.4</td>
    </tr>
  </table>
</center>

<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.090252</td>
      <td>1.558</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>2.20664</td>
      <td>1.933</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>17.9349</td>
      <td>1.95</td>
    </tr>
  </table>
</center>

<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.081394</td>
      <td>1.727</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>1.52364</td>
      <td>2.8</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>12.9103</td>
      <td>2.71</td>
    </tr>
  </table>
</center>


***Przspieszenie w następnych wersjach kodu będzie liczone względem Sekwencyjnej wersji Sita ,,SeqCube"***


- **ParallelDomainStatic**

<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.0625</td>
      <td>0.23</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>0.03125</td>
      <td>1</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.46875</td>
      <td>0.7</td>
    </tr>
  </table>
</center>

<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>0.03125</td>
      <td>1</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.453125</td>
      <td>0.72</td>
    </tr>
  </table>
</center>

<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>0.125</td>
      <td>0.25</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.4375</td>
      <td>0.75</td>
    </tr>
  </table>
</center>

- **ParallelDomainDynamic**
<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>0.046875</td>
      <td>0.668</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.34375</td>
      <td>0.95</td>
    </tr>
  </table>
</center>
<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>0.03125</td>
      <td>1</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.421875</td>
      <td>0.78</td>
    </tr>
  </table>
</center>
<br/>
<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.03125</td>
      <td>0.45</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>0.078125</td>
      <td>0.4</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>0.546875</td>
      <td>0.6</td>
    </tr>
  </table>
</center>

- **ParallelFunctionalStatic**

<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>0.546875</td>
      <td>0.05</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>1.8125</td>
      <td>0.18</td>
    </tr>
  </table>
</center>

<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>0.875</td>
      <td>0.04</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>3.20312</td>
      <td>0.1</td>
    </tr>
  </table>
</center>

<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.1875</td>
      <td>0.75</td>
    </tr>
    <tr>
      <td>0 - 10 000 000</td>
      <td>1.90625</td>
      <td>0.01</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>6.84375</td>
      <td>0.05</td>
    </tr>
  </table>
</center>

- **ParallelFunctionalDynamic**
<center>
<label>3 wątki</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>0.453125</td>
      <td>0.07</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>1.75</td>
      <td>0,19</td>
    </tr>
  </table>
</center>
<br/>
<center>
<label>5 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.015625</td>
      <td>0.9</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>1.0625</td>
      <td>0.02</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>3.875 </td>
      <td>0.084</td>
    </tr>
  </table>
</center>
<br/>
<center>
<label>8 wątków</label>
  <table>
    <tr>
      <th>Zakres</th>
      <th>Czas</th>
      <th>Przyspieszenie</th>
    </tr>
    <tr>
      <td>0 - 1 000 000</td>
      <td>0.15625</td>
      <td>0.9</td>
    </tr>
      <td>0 - 10 000 000</td>
      <td>1.875</td>
      <td>0.017</td>
    </tr>
    <tr>
      <td>0 - 50 000 000</td>
      <td>8.71875</td>
      <td>0.04</td>
    </tr>
  </table>
</center>

c) **Omówienie Wyników:**

W przypadku algorytmów klasycznych, zarówno te z statycznym, jak i dynamicznym przydziałem pracy, wersje równoległe osiągnęły lepszą prędkość przetwarzania w porównaniu do sekwencyjnej wersji klasycznej.

Natomiast w przypadku algorytmów implementujących Sito Eratostenesa, wersja sekwencyjna `SeqCube`, osiągnęła lepsze wyniki czasowe niż odpowiadające jej wersje równoległe dla wszystkich rozważanych zakresów danych. Ponadto algorytmy domenowe uzyskały lepsze przyspieszenie niż funkcyjne. Algorytmy funkcyjne charakteryzowały się znacznie niższą efektywnością, co sprawia, że są mniej atrakcyjne w kontekście prędkości przetwarzania w porównaniu do algorytmów domenowych. Wynik ten może sugerować, że dla tego konkretnego algorytmu, w określonych warunkach, sekwencyjna implementacja była bardziej efektywna niż jej równoległe odpowiedniki.


## Punkt 4: Wnioski

### Porównanie Podejść:

1. **SeqClassic vs. SeqCube**:
   - Algorytm `SeqClassic` był najwolniejszym ze wszystkich algorytmów i miał dużo gorsze wyniki od `SeqCube`(sekwencyjej wersji Sita Eratostenesa). Ponadto `SeqCube` osiągnął lepsze wyniki czasowe niż jej wersje równoległe dla wszystkich rozważanych zakresów danych. Algorytm ten efektywnie eliminuje niepotrzebne sprawdzanie każdej liczby, co przyczyniło się do jego lepszej wydajności.

2. **ParallelClassicStatic vs. ParallelClassicDynamic**:
   - Dla algorytmów klasycznych, wersja `ParallelClassicDynamic` (z dynamicznym przydzielaniem pracy) uzyskała często lepsze wyniki czasowe niż wersja `ParallelClassicStatic` (z statycznym przydzielaniem pracy). Wynika to z elastycznego przydzielania zadań w miarę dostępności, co pozwala uniknąć potencjalnych opóźnień związanych z nierównomiernym obciążeniem wątków.

3. **ParallelDomainStatic vs. ParallelDomainDynamic**:
   - W przypadku algorytmów domenowych, zarówno `ParallelDomainStatic` (statyczny podział domeny) jak i `ParallelDomainDynamic` (dynamiczny podział domeny) osiągnęły dobre wyniki, ale różnice były zazwyczaj niewielkie. Ostateczny wybór pomiędzy nimi może zależeć od konkretnych warunków pracy i dostępności zasobów.

4. **ParallelFunctionalStatic vs. ParallelFunctionalDynamic**:
   - Dla algorytmów funkcyjnych, `ParallelFunctionalDynamic` (dynamiczny podział funkcyjny) dla 3 wątków uzyskał zazwyczaj lepsze wyniki niż `ParallelFunctionalStatic` (statyczny podział funkcyjny). Dla 5 lub 8 miał gorsze wyniki niż jego statyczny odpowiednik. W porównaniu z wersjami domenowym, oba te algorytmy, były dużo wolniejsze.

### Podsumowanie Zrównoważenia Przetwarzania:

- Algorytmy domenowe, charakteryzują się lepszym zrównoważeniem pracy wątków. W przypadku podziału domenowego, każdy wątek równomiernie dostaje określony zakres liczb do sprawdzenia, minimalizując nierównomierną dystrybucję pracy.

- Algorytmy funkcyjne są bardziej elastyczne w dostosowaniu obciążenia pracy między dostępne wątki. To podejście może prowadzić do lepszego zrównoważenia przetwarzania, szczególnie w sytuacjach, gdy nie wszystkie zadania mają taką samą złożoność obliczeniową, jednak jest też mniej efektywne.

### Ocena Efektywności Skalowania:

- W przypadku algorytmów klasycznych, oba podejścia (statyczne i dynamiczne przydzielanie pracy) osiągnęły zazwyczaj dobre skalowanie, szczególnie dla większych zakresów danych. Im więcej wątków dostępnych, tym lepsza była efektywność przetwarzania.

- Algorytmy funkcyjne wykazywały niższą efektywność skalowania w porównaniu do algorytmów domenowych. Mimo że dynamiczne przydzielanie zadań mogło poprawić wyniki, ograniczenia efektywnościowe pozostają widoczne.

### Ograniczenia Efektywnościowe:

- Dominującym ograniczeniem w algorytmach funkcyjnych było wydajne wykorzystanie dostępnych wątków. Przydział pracy musi być dostosowany do tempa, w jakim poszczególne wątki kończą obliczenia, aby uniknąć zbędnej synchronizacji i opóźnień.

- W przypadku algorytmów klasycznych, niewielkie ograniczenia wynikały z wyboru statycznego lub dynamicznego przydzielania pracy. Dla małych zakresów danych statyczne przydzielanie było bardziej skuteczne, podczas gdy dynamiczne przydzielanie sprawdziło się lepiej w przypadku większych zakresów.

- Wersje równoległe algorytmu Sita Eratostenesa nie zawsze osiągały lepsze wyniki niż sekwencyjna wersja, co sugeruje, że w niektórych przypadkach prostota sekwencyjnej implementacji mogła przewyższać korzyści z równoległego przetwarzania.