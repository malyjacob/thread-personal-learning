#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void thread_function(int n)
{
    std::thread::id this_id = std::this_thread::get_id();
    for (int i = 0; i < 5; i++)
    {
        std::cout << "sub-thread " << this_id << "running: "
            << i + 1 << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(n));
    }
}

class Thread_functor
{
public:
    void operator()(int n)
    {
        std::thread::id this_id = std::this_thread::get_id();
        for(int i = 0; i < 5; i++)
        {
            std::cout << "sub-thread functor " << this_id
                << "running: " << i + 1 << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(n));
        }
    }
};

int main()
{
    thread mythread1(thread_function, 1);      // 传递初始函数作为线程的参数
    if (mythread1.joinable())                  //判断是否可以成功使用join()或者detach()，返回true则可以，返回false则不可以
        mythread1.join();                     // 使用join()函数阻塞主线程直至子线程执行完毕

    Thread_functor thread_functor;             //函数对象实例化一个对象
    thread mythread2(thread_functor, 3);     // 传递初始函数作为线程的参数
    if (mythread2.joinable())
        mythread2.detach();                  // 使用detach()函数让子线程和主线程并行运行，主线程也不再等待子线程

    auto thread_lambda = [](int n) {            //lambda表达式格式：[捕获列表]（参数列表）可变异常->返回类型{函数体}
        std::thread::id this_id = std::this_thread::get_id();
        for (int i = 0; i < 5; i++)
        {
            cout << "子lambda线程" << this_id << " 运行: " << i + 1 << endl;
            std::this_thread::sleep_for(std::chrono::seconds(n));   //进程睡眠n秒
        }
    };

    thread mythread3(thread_lambda, 4);     // 传递初始函数作为线程的参数
    if (mythread3.joinable())
        mythread3.join();                     // 使用join()函数阻塞主线程直至子线程执行完毕

    std::thread::id this_id = std::this_thread::get_id();
    for (int i = 0; i < 5; i++) {
        cout << "主线程" << this_id << " 运行: " << i + 1 << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    getchar();
    return 0;
}