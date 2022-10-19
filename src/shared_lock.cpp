#define __cplusplus 202004L
#include <iostream>
#include <mutex>        //unique_lock
#include <shared_mutex> //shared_mutex shared_lock
#include <thread>
#include <chrono>

int main()
{
    int pub_int = 0;
    std::mutex pub_mtx;
    std::shared_mutex sh_mtx;

    auto read = [&](int id, int limit, int pending)
    {
        std::shared_lock<std::shared_mutex> sh_lck(sh_mtx, std::defer_lock);
        while (true)
        {
            sh_lck.lock();
            if (pub_int < limit)
            {
                pub_mtx.lock();
                std::cout << "reader #" << id << "\tpub_int= " << pub_int << std::endl;
                pub_mtx.unlock();
            }
            else
            {
                sh_lck.unlock();
                break;
            }
            sh_lck.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(pending * 250));
        }
    };

    auto write = [&](int &pi, int limit, int pending)
    {
        std::unique_lock<std::shared_mutex> un_lck(sh_mtx, std::defer_lock);
        while (true)
        {
            un_lck.lock();
            pi++;
            if (pub_int >= limit)
            {
                un_lck.unlock();
                break;
            }
            un_lck.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(pending * 250));
        }
    };

    auto test = [&](int wn, int rn, int limit)
    {
        std::thread w_ths[wn];
        for (int i = 0; i < wn; i++)
            w_ths[i] = std::thread(write, std::ref(pub_int), limit, i + 1);
        std::thread r_ths[rn];
        for (int i = 0; i < rn; i++)
            r_ths[i] = std::thread(read, i + 1, limit, i + 1);

        for (int i = 0; i < wn; i++)
            w_ths[i].join();
        for (int i = 0; i < rn; i++)
            r_ths[i].join();
    };

    test(3, 5, 50);
    return 0;
}
