#include "cook/chai/toolchain/serialize/Standard.hpp"

namespace cook { namespace chai { namespace toolchain { namespace serialize {

    void standard_config(std::ostream & oss)
    {
        oss << "cook.toolchain.configure(1, \"single key translations\", fun(e,k,v,b) {" << std::endl;
        oss << "    if (v != \"true\") {" << std::endl;
        oss << "        return false" << std::endl;
        oss << "    }" << std::endl;
        oss << "" << std::endl;
        oss << "    if (false) {" << std::endl;
        oss << "    } else if (k == \"x86\" || k == \"x64\" || k == \"armv7\" || k == \"arm64\") {" << std::endl;
        oss << "        b.add_config(\"arch\", k);" << std::endl;
        oss << "    } else if (k == \"x32\") {" << std::endl;
        oss << "        b.add_config(\"x86\")" << std::endl;
        oss << "    } else if (k == \"debug\" || k == \"release\" || k == \"rtc\" || k == \"profile\") {" << std::endl;
        oss << "        b.add_config(\"config\", k)" << std::endl;
        oss << "    } else {" << std::endl;
        oss << "        return false" << std::endl;
        oss << "    }" << std::endl;
        oss << "" << std::endl;
        oss << "    return true" << std::endl;
        oss << "})" << std::endl;
        oss << std::endl;
        oss << std::endl;
        oss << "cook.toolchain.configure(10, \"standard toolchain config\", fun(e, k, v, b) {" << std::endl;
        oss << "" << std::endl;
        oss << "    if (false) {" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Compile) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else if (k == \"config\" && v == \"debug\") {" << std::endl;
        oss << "            b.add_config(\"debug_symbols\")" << std::endl;
        oss << "            b.add_config(\"config\", \"rtc\")" << std::endl;
        oss << "        }" << std::endl;
        oss << "        else if (k == \"config\" && v == \"release\") {" << std::endl;
        oss << "            b.add_config(\"optimization\", \"max_speed\")" << std::endl;
        oss << "            e.key_values.append(Part.Define, \"NDEBUG\")" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Link) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else if (k == \"config\" && v == \"debug\") {" << std::endl;
        oss << "            b.add_config(\"config\", \"rtc\")" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Archive) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else  {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else {" << std::endl;
        oss << "        return false" << std::endl;
        oss << "    }" << std::endl;
        oss << "" << std::endl;
        oss << "    return true" << std::endl;
        oss << "})" << std::endl;
        oss << std::endl;
    }

} } } }


