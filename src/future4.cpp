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
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };
    auto accumulate_future = std::async(std::launch::async, accumulate,
                                        numbers.begin(), numbers.end());
    std::cout << "result= " << accumulate_future.get() << std::endl;
    return 0;
}