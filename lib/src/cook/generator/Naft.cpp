#include "cook/generator/Naft.hpp"
#include "cook/util/File.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/Context.hpp"


namespace cook { namespace generator {

Result Naft::set_option(const std::string & option)
{
    set_filename(option);
}

bool Naft::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Naft::process(std::ostream & oss, const Context & context)
{

}

} }
