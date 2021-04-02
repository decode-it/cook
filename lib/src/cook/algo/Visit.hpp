#ifndef HEADER_cook_algo_Visit_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_Visit_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include <stack>
#include <unordered_set>
#include <type_traits>

namespace cook { namespace algo {

template <typename InitializationFunctor, typename ProcessFunctor>
auto visit(InitializationFunctor && initialize, ProcessFunctor && process)
{
    using Stack = std::stack<model::Recipe*>;
    using ResultType = typename std::invoke_result_t<ProcessFunctor, model::Recipe *, Stack &>;

    MSS_BEGIN(ResultType);

    Stack todo;
    initialize(todo);

    std::unordered_set<model::Recipe*> seen;
    while(!todo.empty())
    {
        model::Recipe * cur = todo.top();
        todo.pop();

        if (!seen.insert(cur).second)
            continue;

        MSS(process(cur, todo));
    }

    MSS_END();
}


} }

#endif
