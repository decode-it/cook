#include "cook/process/toolchain/serialize/Standard.hpp"
#include "cook/OS.hpp"

namespace cook { namespace process { namespace toolchain { namespace serialize {

    namespace {

        void serialize_naming(std::ostream & oss)
        {
            if(get_os() == OS::Windows)
            {
                oss << R"%(cook.toolchain.set_target_namer(fun(r) {
    if (false) {
    } else if (r.type == TargetType.Executable) {
        return "${r.name}.exe"
    } else if (r.type == TargetType.SharedLibrary) {
        return "${r.name}.dll"
    } else if(r.type == TargetType.Archive) {
        return "${r.name}.lib"
    } else {
        return ""
    }
})
)%";
             }
             else
             {
                 oss << R"%(cook.toolchain.set_target_namer(fun(r) {
    if (false) {
    } else if (r.type == TargetType.Executable) {
        return "${r.name}"
    } else if (r.type == TargetType.SharedLibrary) {
        return "lib${r.name}.so"
    } else if(r.type == TargetType.Archive) {
        return "lib${r.name}.a"
    } else {
        return ""
    }
})
)%";
             }
        }
    }

    void standard_config(std::ostream & oss)
    {
        serialize_naming(oss);
        oss << R"%(
cook.toolchain.configure(1, "single key translations", fun(e,k,v,b) {
    if (v != "true") {
        return false
    }

    if (false) {
    } else if (k == "x86" || k == "x64" || k == "armv7" || k == "arm64" || k == "a53" || k == "a5") {
        b.add_config("arch", k);
    } else if (k == "x32") {
        b.add_config("x86")
    } else if (k == "debug" || k == "release" || k == "rtc" || k == "profile") {
        b.add_config("config", k)
    } else if (k == "pic") {
        b.add_config("position_independent_code", "true")
    } else if (k == "md") {
        b.add_config("c++.runtime", "dynamic")
    } else if (k == "mt") {
        b.add_config("c++.runtime", "static")
    } else {
        return false
    }

    return true
})

cook.toolchain.configure(10, "standard toolchain config", fun(e, k, v, b) {
    if (false) {
    } else if (e.element_type == ElementType.Compile) {
        if (false) {
        } else if (k == "config" && v == "debug") {
            b.add_config("debug_symbols")
        }
        else if (k == "config" && v == "release") {
            b.add_config("optimization", "max_speed")
            e.key_values.append(Part.Define, "NDEBUG")
        } else {
            return false
        }
    } else if (e.element_type == ElementType.Link) {
        if (false) {
        } else {
            return false
        }
    } else if (e.element_type == ElementType.Archive) {
        if (false) {
        } else  {
            return false
        }
    } else {
        return false
    }

    return true
})

)%";
    }

} } } }


