#ifndef HEADER_cook_util_TreeNode_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_TreeNode_hpp_ALREADY_INCLUDED

namespace cook { namespace util {

    class TreeNode 
    {
    public:
        TreeNode(std::ostream & stream, const std::string & name, std::size_t indent)
                : str_(&stream),
                indent_(indent),
                has_children_(false)
        {
            indented_() << "[" << name << "]";
        }
        
        ~TreeNode()
        {
            if(str_ && has_children_)
            {
                (*str_) << std::endl;
                indented_() << "}";
            }
        }
        
        TreeNode(TreeNode && rhs) : 
            str_(rhs.str_),
            indent_(rhs.indent_),
            has_children_(rhs.has_children_)
        {
            rhs.str_ = nullptr;
        }
        
        TreeNode & operator=(TreeNode && rhs)
        {
            str_=  rhs.str_;
            indent_ = rhs.indent_;
            has_children_ = rhs.has_children_;
            rhs.str_ = nullptr;
            
            return *this;
        }
        
        TreeNode open(const std::string & name)
        {
            if (!has_children_)
            {
                (*str_) << " {";
                has_children_= true;
            }
            
            (*str_) << std::endl;
         
            return TreeNode(*str_, name, indent_+1);
        }
        
        template <typename Key, typename Value>
        TreeNode & attr(const Key & key, const Value & value)
        {
            (*str_) << "(" << key << ":" << value << ")";
            return *this;
        }
        
        template <typename Key, typename Value>
        TreeNode & attr(const Key & key)
        {
            (*str_) << "(" << key << ")";
            return *this;
        }
        
    private:
        TreeNode & operator=(const TreeNode &) = delete;
        TreeNode(const TreeNode &) = delete;
        
        std::ostream & indented_() 
        {
            std::fill_n(std::ostream_iterator<char>(*str_), indent_*2, ' ');
            return *str_;
        }
        
        std::ostream * str_;
        unsigned int indent_;
        bool has_children_;
        bool dying_;
    };
    
    inline TreeNode make_tree_node(std::ostream & stream, const std::string & name)
    {
        return TreeNode(stream, name, 0);
    }
} }


#endif
