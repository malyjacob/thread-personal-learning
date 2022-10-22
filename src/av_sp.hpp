#ifndef __AV_SP
#define __AV_SP

#include <shared_mutex>

template <unsigned Limit>
class Lock;

template <unsigned Limit>
class Flag
{
private:
    unsigned sgnl = 0;
    mutable std::shared_mutex sh_mtx;

public:
    Flag() = default;
    Flag(const Flag &) = delete;
    Flag &operator=(const Flag &) = delete;

    unsigned get_sgnl() const
    {
        sh_mtx.lock_shared();
        unsigned tmp = sgnl;
        sh_mtx.unlock_shared();
        return tmp;
    }

    void incre()
    {
        sh_mtx.lock();
        sgnl = ++sgnl == Limit ? 0 : sgnl;
        sh_mtx.unlock();
    }

    bool test(unsigned expct) const
    {
        return expct == get_sgnl() ? true : false;
    }

    std::shared_mutex& get_mutex() { return sh_mtx; }

    friend class Lock<Limit>;
};

class BoolFlag
{
private:
    bool sgnl = false;
    mutable std::shared_mutex sh_mtx;

public:
    BoolFlag() = default;
    BoolFlag(const BoolFlag &) = delete;
    BoolFlag &operator=(const BoolFlag &) = delete;

    BoolFlag(bool bl) : sgnl(bl), sh_mtx{} {}
    bool get_signl() const
    {
        sh_mtx.lock_shared();
        bool tmp = sgnl;
        sh_mtx.unlock_shared();
        return tmp;
    }

    void set(bool desired)
    {
        if (desired == get_signl())
            return;
        sh_mtx.lock();
        sgnl = desired;
        sh_mtx.unlock();
    }
};

template <unsigned Limit>
class Lock
{
private:
    static Flag<Limit> flag;
    static Flag<Limit> num;
    static BoolFlag full;

private:
    unsigned id;

public:
    Lock() : id(0)
    {
        if(full.get_signl())
            throw;
        num.sh_mtx.lock();
        id = num.sgnl;
        num.sgnl = ++num.sgnl == Limit ? 0 : num.sgnl;
        if(Limit == id + 1)
            full.set(true);
        num.sh_mtx.unlock();
    }

    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;

    Lock(Lock&& other) : id(other.id)
    {
        other.id = Limit;
    }

    void lock()
    {
        while (!flag.test(id))
        {
        }
    }

    void unlock()
    {
        flag.incre();
    }

    operator bool()
    {
        return (id < Limit) ? true : false;
    }
};

template <unsigned Limit>
Flag<Limit> Lock<Limit>::flag = Flag<Limit>();

template <unsigned Limit>
Flag<Limit> Lock<Limit>::num = Flag<Limit>();

template <unsigned Limit>
BoolFlag Lock<Limit>::full = BoolFlag();

#endif