#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>

int fib(int n)
{
    if(n < 3) return 1;
    else return fib(n - 1) + fib(n - 2);
}

int main()
{
    std::vector<std::future<int>> future_pool;
    for(int i = 0; i < 20; i++)
    {
        auto f = std::async(std::launch::async, [=](){
            return fib(25 + i);
        });
        future_pool.push_back(std::move(f));
    }

    std::cout << "waiting...." << std::endl;
    const auto start = std::chrono::system_clock::now();

    for(int i = 0; i < 20; i++)
        future_pool[i].wait();
    
    const auto diff = std::chrono::system_clock::now() - start;

    std::cout << std::chrono::duration<double>(diff).count() << "seconds\n";

    for(int i = 0; i < 20; i++)
        std::cout << "future_pool[" << i << "] :\t\t" << future_pool[i].get() << std::endl;
}