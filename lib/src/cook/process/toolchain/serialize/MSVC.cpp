#include "cook/process/toolchain/serialize/GCC.hpp"
#include "cook/process/toolchain/serialize/Standard.hpp"

namespace cook { namespace process { namespace toolchain { namespace serialize {

    void msvc_config(std::ostream & oss)
    {
        standard_config(oss);

        oss << "cook.toolchain.configure(100, \"specific toolchain config\", fun(e,k,v,b) {" << std::endl;
        oss << "" << std::endl;
        oss << "    auto & kv = e.key_values();" << std::endl;
        oss << "    auto & tm = e.translators();" << std::endl;
        oss << "" << std::endl;
        oss << "    if (false) {" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Compile) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else if (k == \"debug_symbols\" && v == \"true\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"Zi\")" << std::endl;
        oss << "        } else if (k == \"optimization\" && v == \"max_speed\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"O2\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x86\") {" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x64\") {" << std::endl;
        oss << "        } else if (k == \"position_independent_code\" && v == \"true\") {" << std::endl;
        oss << "        } else if (k == \"c++.std\" && e.language == Language.CXX && v == \"17\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"std\", \"c++latest\")" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false;" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Link) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.element_type == ElementType.Archive) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else {" << std::endl;
        oss << "        return false" << std::endl;
        oss << "    }" << std::endl;
        oss << "    return true" << std::endl;
        oss << "})" << std::endl;
        oss << "" << std::endl;
        oss << "" << std::endl;
        oss << "for( lang : [Language.C, Language.CXX, Language.ASM]) {" << std::endl;
        oss << "" << std::endl;
        oss << "    var compiler = cook.toolchain.element(ElementType.Compile, lang, TargetType.Object)" << std::endl;
        oss << "    var & kv = compiler.key_values()" << std::endl;
        oss << "    var & tm = compiler.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return \"/${k}\" } else { return \"/${k}:${v}\" } }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return \"/Fo:${k}\" }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Deps]           = fun(k,v) { return \"/showIncludes\" }" << std::endl;
        oss << "    tm[Part.Define]         = fun(k,v) { if (v.empty) { return \"/D${k}\" } else { return \"/D${k}=${v}\" } }" << std::endl;
        oss << "    tm[Part.IncludePath]    = fun(k,v) { if (k.empty) { return \"/I./\" } else { return \"/I${k}\" } }" << std::endl;
        oss << "    tm[Part.ForceInclude]   = fun(k,v) { return \"/FI${k}\" }" << std::endl;
        oss << "" << std::endl;
        oss << "    if (lang == Language.C) {" << std::endl;
        oss << "        kv.append(Part.Pre, \"TC\")" << std::endl;
        oss << "    }" << std::endl;
        oss << "    kv.append(Part.Cli, \"cl\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"nologo\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"EHsc\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"bigobj\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"FS\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"c\")" << std::endl;
        oss << "    kv.append(Part.Define, \"NOMINMAX\")" << std::endl;
        oss << "    kv.append(Part.Deps, \"msvc\")" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "{" << std::endl;
        oss << "    var archiver = cook.toolchain.element(ElementType.Archive, Language.Binary, TargetType.Archive)" << std::endl;
        oss << "    var & kv = archiver.key_values()" << std::endl;
        oss << "    var & tm = archiver.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return \"/OUT:${k}\" }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "" << std::endl;
        oss << "    kv.append(Part.Cli, \"lib\")" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "for(s : [TargetType.Executable, TargetType.SharedLibrary]){" << std::endl;
        oss << "    var linker = cook.toolchain.element(ElementType.Link, Language.Binary, s)" << std::endl;
        oss << "    var & kv = linker.key_values()" << std::endl;
        oss << "    var & tm = linker.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return \"/${k}\" } else { return \"/${k}:${v}\" } }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return \"/OUT:${k}\" }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Library]        = fun(k,v) { return \"${k}.lib\" }" << std::endl;
        oss << "    tm[Part.LibraryPath]    = fun(k,v) { if (k.empty) { return \"/LIBPATH:./\" } else { return \"/LIBPATH:${k}\" } }" << std::endl;
        oss << "    " << std::endl;
        oss << "    kv.append(Part.Cli, \"link\")" << std::endl;
        oss << "    if (s == TargetType.SharedLibrary){" << std::endl;
        oss << "        tm[Part.Export]     = fun(k,v) { return \"/DEF:${k}\" }" << std::endl;
        oss << "        kv.append(Part.Pre, \"DLL\")" << std::endl;
        oss << "    }" << std::endl;
        oss << "}" << std::endl;
    }

} } } }
