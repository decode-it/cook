//#include "cook/algo/Book.hpp"
//#include "cook/algo/Recipe.hpp"
//#include <stack>

//namespace cook { namespace algo {


//void list_all_recipes(model::Book * book, std::list<model::Recipe *> & result)
//{
//    std::stack<model::Book *> todo;
//    todo.push(book);

//    while(!todo.empty())
//    {
//        model::Book * b = todo.top();
//        todo.pop();

//        result.insert(result.end(), RANGE(b->recipes()));

//        for(model::Book * cb : b->books())
//            todo.push(cb);
//    }

//}

//Result resolve_all_dependencies(model::Book * book)
//{
//    std::stack<model::Book * book> todo;
//    todo.push(book);

//    while(todo.empty())
//    {

//    }


//Result resolve_dependency(model::Recipe *&result, const model::Uri & uri, model::Book * current_book, model::Book * root_book);
//}


//} }
