#ifndef HEADER_cook_model_Menu_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Menu_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace model {

class Menu
{
    enum ConstructionResult
    {
        OK                      = 0b000,
        UnresolvedDependencies  = 0b001,
        CyclicDependencies      = 0b010,
        InternalError           = 0b111
    };

public:
    explicit Menu(Book * root) : root_(root){}

    void clear();

    template <typename It> Result construct(gubg::Range<It> root_recipes)
    {
        MSS_BEGIN(Result);
        clear();
        root_recipes_.assign(RANGE(root_recipes));

        MSS(construct_());
        MSS_END();
    }

    bool all_dependencies_resolved() const;
    bool is_acyclic() const;


    bool topological_order(Recipe * root, std::list<Recipe *> & order) const;
    const std::list<Recipe *> &topological_order() const;
    const std::list<Recipe *> & root_recipes() const { return root_recipes_; }
    Book * root_book() const { return root_; }

    static bool topological_suborder(model::Recipe * new_root, const std::list<Recipe*> & topological_order, std::list<Recipe *> & new_order);

private:
    Result construct_();
    Result resolve_recursive_dependencies_(bool & all_resolved);
    Result sort_topologically_(bool & acyclic);

    std::list<Recipe *> root_recipes_;
    std::list<Recipe *> topological_order_;
    ConstructionResult result_;
    Book * root_;
};


} }

#endif
