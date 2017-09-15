#ifndef HEADER_cook_work_TreeWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_TreeWriter_hpp_ALREADY_INCLUDED

#include "cook/work/TopologicalOrder.hpp"
#include "cook/util/TreeNode.hpp"

namespace cook { namespace work {
    
    class TreeWriter
    {
    public:
        bool operator()(const TopologicalOrder & order);
        
    private:
        bool write_book_(const structure::Book & book, util::TreeNode & node);
        bool write_recipe_(const structure::Recipe & recipe, util::TreeNode & node);
    };
    
} }

#endif
