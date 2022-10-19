#include <iostream>
#include <string>
#include <concepts>

/// @brief struct Task
struct Task
{
    using value_type = std::string;
    int id = 0;
    std::string name;

    Task(const char *nm = "noname", int i = 0)
        : name(nm), id(i) {}
    const std::string &get_name() const { return name; }
    int get_id() const { return id; }

    int operator()(int d) const
    {
        std::cout << name << "\t" << id + d << std::endl;
        return id + d;
    }
};



// @brief the concept that can be called,
// and it is constrainted by the arguments-types and returned-type
template <class T, class U, class... Args>
concept Special = requires(T t, Args... args)
{
    {
        t(args...)
        } -> std::same_as<U>;
};

// @brief the more special concept than concept `Special`,
// it constraint that it must be a functional object
template <class T, class U, class... Args>
concept SpecialFunctor = Special<T, U, Args...> && !std::is_function_v<T>;

// @brief the more special concept than concept `Special`,
// it constraint that it must be a function
template <class T, class U, class... Args>
concept SpecialFunction = Special<T, U, Args...> && std::is_function_v<T>;

template<class R>
concept NoVoid = !std::is_void_v<R>;


// @brief test
// a normal function
int test(int id)
{
    std::cout << "using test..." << std::endl;
    return id;
}


// the main template class `Package` that would never be implemented
template <class...>
class Package;

// the branch template class `Package` that use for functional,
// and this template class accepts more than 2 template arguments
// the first template arg is functional-object-type
// the second template arg is the functional-object-type's returned-type
// the rest template args (if have) are the args that the functional-objet-type's functional-call requires.
template <class F, NoVoid R, NoVoid... Args>
requires SpecialFunctor<F, R, Args...>
class Package<F, R, Args...>
{
private:
    typedef F Fn;
    Fn _fn;

public:
    Package() = default;
    template<class Firtst, NoVoid... Rest>
    Package(Firtst f, Rest... res) : _fn(f, res...) {}

    R operator()(Args...args) {return _fn(args...); }

    template<NoVoid... Rest>
    static Package init(Rest... res) { return Package(res...); }
};

template <class F, NoVoid... Args>
requires SpecialFunctor<F, void, Args...>
class Package<F, Args...>
{
private:
    typedef F Fn;
    Fn _fn;

public:
    Package() = default;
    template<class First, NoVoid... Rest>
    Package(First f, Rest... res) : _fn(f, res...) {}

    void operator()(Args... args) { _fn(args...); }

    template<NoVoid... Rest>
    static Package init(Rest... res) { return Package(res...); }
};


template <NoVoid R, NoVoid... Args>
class Package<R(Args...)>
{
private:
    typedef R (*Fn)(Args...);
    Fn _fn;

public:
    explicit Package(Fn fn) : _fn(fn) {}

    template <class F>
    requires SpecialFunctor<F, R, Args...>
    explicit Package(F) : _fn([](Args... args) -> R
                              { F fn; return fn(args...); }) {}
    
    R operator()(Args... args) { return _fn(args...); }

    static Package init(Fn fn) { return Package(fn); }

    template <class F>
    requires SpecialFunctor<F, R, Args...>
    static Package init()
    {
        F f;
        return Package(f);
    }
};

template <NoVoid... Rest>
class Package<void(Rest...)>
{
private:
    typedef void (*Fn)(Rest...);
    Fn _fn;

public:
    explicit Package(Fn fn) : _fn(fn) {}

    template <class F>
    requires SpecialFunctor<F, void, Rest...>
    explicit Package(F) : _fn([](Rest... args) -> void
                              { F fn; fn(args...); }) {}

    void operator()(Rest... res) { _fn(res...); }

    static Package init(Fn fn)
    {
        return Package<void(Rest...)>(fn);
    }

    template <class F>
    requires SpecialFunctor<F, void, Rest...>
    static Package init()
    {
        F f;
        return Package(f);
    }
};


class Pkg
{
    // static createPackage
};


template <class R, NoVoid... Args>
using Handler = R (*)(Args...);


template <typename T, typename U, typename... Args>
requires SpecialFunctor<T, U, Args...>
    Handler<U, Args...> get_handler()
{
    return [](Args... args) -> U
    { T t; return t(args...); };
}


struct Functor
{
    int operator()(int id)
    {
        std::cout << "FN " << id << std::endl;
        return id;
    }
};

struct Functor_n
{
    int operator()()
    {
        std::cout << "FNN " << 0xff << std::endl;
        return 0xff;
    }
};

struct Functor_v
{
    void operator()(int id) { std::cout << "FN_V " << id << std::endl; }
};



int main()
{
    Task task("t1", 13);
    Package<int()> pkg1([]() -> int
                        { return 12; });
    Package<int(int)> pkg2([](int n) -> int
                           { return n; });
    Package<int(int)> pkg3(test);
    std::cout << pkg1() << std::endl;
    std::cout << pkg2(25) << std::endl;
    std::cout << pkg3(13) << std::endl;

    Package<void()> pkg4([]() -> void
                         { std::cout << "using pkg4..." << std::endl; });
    Package<void(int)> pkg5([](int n) -> void
                            { std::cout << "using pkg5...  " << n << std::endl; });
    pkg4();
    pkg5(55);

    if (SpecialFunction<int(int), int, int>)
        std::cout << "yes" << std::endl;
    else
        std::cout << "no" << std::endl;

    std::cout << sizeof(Package<int()>) << std::endl;

    Handler<int, int> handler = get_handler<Functor, int, int>();

    std::cout << handler << std::endl;
    handler(80);
    auto h = [&task](int id) -> int
    { return task(id); };
    h(13);

    Package<void(int)> pkg6([](int n)->void {Functor_v fv; fv(n); });
    pkg6(66);

    auto pkg7 = Package<void()>::init([]() -> void
                                       { std::cout << "using pkg7..." << std::endl; });
    auto pkg8 = Package<void(int)>::init<Functor_v>();
    pkg7();
    pkg8(120);

    auto pkg9 = Package<Task, int, int>::init("t9", 99);
    pkg9(11);

    auto pkg10 = Package<Functor, int, int>::init();
    pkg10(10);

    auto pkg11 = Package<Functor_v, int>::init();
    pkg11(11);

    auto pkg12 = Package<Functor_n, int>::init();
    pkg12();
}
