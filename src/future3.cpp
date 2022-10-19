#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

int accumulate(std::vector<int>::iterator first,
               std::vector<int>::iterator last)
{
    int sum = std::accumulate(first, last, 0);
    return sum;
}

int main()
{
    std::vector<int> numbers{1, 2, 3, 4, 5, 6, 7};
    std::packaged_task<int(std::vector<int>::iterator,
                           std::vector<int>::iterator)>
        accumulate_task(accumulate);
    std::future<int> accumute_future = accumulate_task.get_future();
    std::thread work_thread(std::move(accumulate_task),
                            numbers.begin(), numbers.end());
    accumute_future.wait();
    std::cout << "result= " << accumute_future.get() << std::endl;

    work_thread.join();
    return 0;
}