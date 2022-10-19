#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

int res = 0;
std::mutex mu;
std::condition_variable cond;

void accumulate(std::vector<int>::iterator first,
                std::vector<int>::iterator last)
{
    int sum = std::accumulate(first, last, 0);
    std::unique_lock<std::mutex> locker(mu);
    res = sum;
    locker.unlock();
    cond.notify_one();
}

int main()
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::thread work_thread(accumulate, numbers.begin(), numbers.end());

    std::unique_lock<std::mutex> locker(mu);
    cond.wait(locker, [](){ return res; });
    std::cout << "result= " << res << std::endl;
    locker.unlock();
    work_thread.join();

    return 0;
}