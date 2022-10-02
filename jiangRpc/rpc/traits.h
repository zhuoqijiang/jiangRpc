#include <functional>
#include <tuple>

namespace jiangRpc {
template<typename T>
struct function_traits;


template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{
    enum {arg_size = sizeof...(Args)};
    using return_type = ReturnType;
    using function_type = ReturnType(Args...);
    using functional_type = std::function<function_type>;
    template<size_t I>
    struct args 
    {   
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };  
};

template<typename ReturnType, typename... Args>
struct function_traits<std::function<ReturnType(Args...)>>: function_traits<ReturnType(Args...)> {}; 


template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)>: function_traits<ReturnType(Args...)>{};

template<typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>:function_traits<ReturnType(Args...)>{};

template<typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>:function_traits<ReturnType(Args...)>{};

template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {}; 
}
