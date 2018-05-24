#include "cook/process/toolchain/serialize/Standard.hpp"
#include "cook/OS.hpp"

namespace cook { namespace process { namespace toolchain { namespace serialize {

    namespace {

        void serialize_naming(std::ostream & oss)
        {
            if(get_os() == OS::Windows)
            {
                oss << "cook.toolchain.set_target_namer(fun(r) {" << std::endl;
                oss << "    if (false) {" << std::endl;
                oss << "    } else if (r.type == TargetType.Executable) {" << std::endl;
                oss << "        return \"${r.name}.exe\"" << std::endl;
                oss << "    } else if (r.type == TargetType.SharedLibrary) {" << std::endl;
                oss << "        return \"${r.name}.dll\"" << std::endl;
                oss << "    } else if(r.type == TargetType.Archive) {" << std::endl;
                oss << "        return \"${r.name}.lib\"" << std::endl;
                oss << "    } else {" << std::endl;
                oss << "        return \"\"" << std::endl;
                oss << "    }" << std::endl;
                oss << "})" << std::endl;
            }
            else
            {
                oss << "cook.toolchain.set_target_namer(fun(r) {" << std::endl;
                oss << "    if (false) {" << std::endl;
                oss << "    } else if (r.type == TargetType.Executable) {" << std::endl;
                oss << "        return \"${r.name}\"" << std::endl;
                oss << "    } else if (r.type == TargetType.SharedLibrary) {" << std::endl;
                oss << "        return \"lib${r.name}.so\"" << std::endl;
                oss << "    } else if(r.type == TargetType.Archive) {" << std::endl;
                oss << "        return \"lib${r.name}.a\"" << std::endl;
                oss << "    } else {" << std::endl;
                oss << "        return \"\"" << std::endl;
                oss << "    }" << std::endl;
                oss << "})" << std::endl;
            }
        }
    }

    void standard_config(std::ostream & oss)
    {
        serialize_naming(oss);
        oss << std::endl;

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
        oss << "    } else if (k == \"pic\") {" << std::endl;
        oss << "        b.add_config(\"position_independent_code\", \"true\")" << std::endl;
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


