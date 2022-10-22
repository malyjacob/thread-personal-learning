#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

std::mutex lck;

int sum = 0;

void func()
{
    for (size_t i = 0; i < 100000; i++)
    {
        lck.lock();
        ++sum;
        // std::cout << "sum =\t" << ++sum << "\tin thread["
        //           << std::this_thread::get_id() <<"]" << std::endl;
        lck.unlock();
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    for(int i = 0; i < 4; i++)
        threads.push_back(std::thread(func));

    for(int i = 0; i < 4; i++)
        threads[i].join();

    auto end = std::chrono::high_resolution_clock::now();
    double dr_ms=std::chrono::duration<double,std::milli>(end - start).count();

    std::cout << sum << std::endl;
    std::cout << dr_ms << std::endl;
    return 0;
}