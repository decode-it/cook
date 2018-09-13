#include "cook/process/toolchain/serialize/GCC.hpp"
#include "cook/process/toolchain/serialize/Standard.hpp"

namespace cook { namespace process { namespace toolchain { namespace serialize {

    void msvc_config(std::ostream & oss)
    {
        standard_config(oss);

        oss << R"%(cook.toolchain.configure(100, "specific toolchain config", fun(e,k,v,b) {
    auto & kv = e.key_values();
    auto & tm = e.translators();

    if (false) {
    } else if (e.element_type == ElementType.Compile) {
        if (false) {
        } else if (k == "debug_symbols" && v == "true") {
            kv.append(Part.Pre, "Zi")
            tm[Part.Runtime] = fun(k,v) { return "/${k}d" }
        } else if (k == "optimization" && v == "max_speed") {
            kv.append(Part.Pre, "O2")
        } else if (k == "arch" && v == "x86") {
        } else if (k == "arch" && v == "x64") {
        } else if (k == "position_independent_code" && v == "true") {
        } else if (k == "c++.runtime") {
            if (false) {}
            else if (v == "dynamic") { kv.append(Part.Runtime, "MD") }
            else if (v == "static") { kv.append(Part.Runtime, "MT") }
        } else if (k == "c++.std" && e.language == Language.CXX) {
            kv.append(Part.Pre, "std", "c++${v}")
        } else {
            return false;
        }
    } else if (e.element_type == ElementType.Link) {
        if (false) {
        } else {
            return false
        }
    } else if (e.element_type == ElementType.Archive) {
        if (false) {
        } else {
            return false
        }
    } else {
        return false
    }
    return true
})


for( lang : [Language.C, Language.CXX, Language.ASM, Language.Resource]) {

    var compiler = cook.toolchain.element(ElementType.Compile, lang, TargetType.Object)
    var & kv = compiler.key_values()
    var & tm = compiler.translators()

    tm[Part.Cli]            = fun(k,v) { return k }
    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return "/${k}" } else { return "/${k}:${v}" } }
    tm[Part.Runtime]        = fun(k,v) { return "/${k}" }
    tm[Part.Output]         = fun(k,v) { return "/Fo${k}" }
    tm[Part.Input]          = fun(k,v) { return k }
    tm[Part.Deps]           = fun(k,v) { return "/showIncludes" }
    tm[Part.Define]         = fun(k,v) { if (v.empty) { return "/D${k}" } else { return "/D${k}=${v}" } }
    tm[Part.IncludePath]    = fun(k,v) { if (k.empty) { return "/I./" } else { return "/I\"${k}\"" } }
    tm[Part.ForceInclude]   = fun(k,v) { return "/FI${k}" }
    tm[Part.Resource]       = fun(k,v) { return "" }

    if (lang == Language.C) {
        kv.append(Part.Pre, "TC")
    }
    if (lang == Language.Resource) {
        kv.append(Part.Cli, "rc")
        tm[Part.Pre]        = fun(k,v) { return "" }
        tm[Part.Runtime]    = fun(k,v) { return "" }
        tm[Part.Deps]       = fun(k,v) { return "" }
        tm[Part.Resource]   = fun(k,v) { return k }
    } else {
        kv.append(Part.Cli, "cl")
    }
    kv.append(Part.Pre, "EHsc")
    kv.append(Part.Pre, "bigobj")
    kv.append(Part.Pre, "FS")
    kv.append(Part.Pre, "c")
    kv.append(Part.Pre, "nologo")
    kv.append(Part.Define, "NOMINMAX")
    kv.append(Part.Deps, "msvc")
}

{
    var archiver = cook.toolchain.element(ElementType.Archive, Language.Binary, TargetType.Archive)
    var & kv = archiver.key_values()
    var & tm = archiver.translators()

    tm[Part.Cli]            = fun(k,v) { return k }
    tm[Part.Pre]            = fun(k,v) { return k }
    tm[Part.Response]       = fun(k,v) { return "@${k}" }
    tm[Part.Output]         = fun(k,v) { return "/OUT:${k}" }
    tm[Part.Input]          = fun(k,v) { return k }

    kv.append(Part.Cli, "lib")
}

for(s : [TargetType.Executable, TargetType.SharedLibrary]){
    var linker = cook.toolchain.element(ElementType.Link, Language.Binary, s)
    var & kv = linker.key_values()
    var & tm = linker.translators()

    tm[Part.Cli]            = fun(k,v) { return k }
    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return "/${k}" } else { return "/${k}:${v}" } }
    tm[Part.Response]       = fun(k,v) { return "@${k}" }
    tm[Part.Output]         = fun(k,v) { return "/OUT:${k}" }
    tm[Part.Input]          = fun(k,v) { return k }
    tm[Part.Library]        = fun(k,v) { return "${k}.lib" }
    tm[Part.LibraryPath]    = fun(k,v) { if (k.empty) { return "/LIBPATH:./" } else { return "/LIBPATH:\"${k}\"" } }
    
    kv.append(Part.Cli, "link")
    if (s == TargetType.SharedLibrary){
        tm[Part.Export]     = fun(k,v) { return "/DEF:${k}" }
        kv.append(Part.Pre, "DLL")
    }
}

)%";
    }

} } } }
