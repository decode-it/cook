#include "cook/algo/Book.hpp"
#include <stack>

namespace cook { namespace algo {


void list_all_recipes(model::Book * book, std::list<model::Recipe *> & result)
{
    std::stack<model::Book *> todo;
    todo.push(book);

    while(!todo.empty())
    {
        model::Book * b = todo.top();
        todo.pop();

        result.insert(result.end(), RANGE(b->recipes()));

        for(model::Book * cb : b->books())
            todo.push(cb);
    }

}


} }
