#include <iostream>

template<typename R, typename A>
struct function
{
    using call_func_t = R(*)(void*, A);
    using delete_func_t = void(*)(void*);
    
    template<typename O>
    struct function_helper
    {
        using mem_func_t = R(O::*)(A); 
        function_helper(mem_func_t mf, O* o) : mf_(mf), o_(o)
        {
        }
    
        R operator()(A a)
        {
            return (o_->*mf_)(a);
        }

        mem_func_t mf_;
        O* o_;
    };
    
    template<typename O>
    static inline R call_func_util(void* o, A a)
    {
        O* ro = static_cast<O*>(o);

        return (*ro)(a);
    }

    template<typename O>
    static inline void delete_func_util(void* o)
    {
        O* ro = static_cast<O*>(o);

        delete(ro);
    }

    template<typename O>
    function(R(O::*mf)(A), O* o)
    {
        using func_helper_t = function_helper<O>;
        hfunc_ = new func_helper_t(mf, o);          
        call_func_ = &call_func_util<func_helper_t>;
        delete_func_ = &delete_func_util<func_helper_t>;
    }

    inline R operator()(A a)
    {
        return call_func_(hfunc_, a);
    }
    
    ~function()
    {
        delete_func_(hfunc_);
    }

    void* hfunc_;
    call_func_t call_func_;
    delete_func_t delete_func_;
};

struct TestObject
{
    void work(int& i)
    {
        ++i;
    }
};

int main()
{
    TestObject to;
    int i = 42;
    function<void, int&> f(&TestObject::work, &to);
    f(i);

    std::cout << i << std::endl;

    return 0;
}
