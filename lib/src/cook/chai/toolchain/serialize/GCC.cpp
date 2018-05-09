#include "cook/chai/toolchain/serialize/GCC.hpp"
#include "cook/chai/toolchain/serialize/Standard.hpp"

namespace cook { namespace chai { namespace toolchain { namespace serialize {

    void gcc_config(std::ostream & oss, const std::map<cook::Language, std::string> & compilers, const std::string & linker)
    {
        standard_config(oss);

        oss << "cook.toolchain.configure(100, \"specific toolchain config\", fun(e,k,v,b) {" << std::endl;
        oss << "" << std::endl;
        oss << "    auto & kv = e.key_values();" << std::endl;
        oss << "    auto & tm = e.translators();" << std::endl;
        oss << "" << std::endl;
        oss << "" << std::endl;
        oss << "    if (false) {" << std::endl;
        oss << "    } else if (e.type == ElementType.Compile) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else if (k == \"debug_symbols\" && v == \"true\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-g\")" << std::endl;
        oss << "        } else if (k == \"optimization\" && v == \"max_speed\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-O3\")" << std::endl;
        oss << "        } else if (k == \"config\" && v == \"rtc\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-fsanitize\", \"address\")" << std::endl;
        oss << "            kv.append(Part.Pre, \"-fno-omit-frame-pointer\")" << std::endl;
        oss << "        } else if (k == \"config\" && v == \"profile\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-g\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x86\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-m32\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x64\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-m64\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"armv7\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"armv7\")" << std::endl;
        oss << "        } else if (k == \"c++-standard\" && e.language == Language.CXX) {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-std\", \"c++${v}\")" << std::endl;
        oss << "        } else if (k == \"c-standard\" && e.language == Language.C) {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-std\", \"c${v}\")" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false;" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.type == ElementType.Link) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x86\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-m32\", \"\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"x64\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-m64\")" << std::endl;
        oss << "        } else if (k == \"arch\" && v == \"armv7\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-arch armv7\", \"\")" << std::endl;
        oss << "        } else if (k == \"config\" && v == \"rtc\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-fsanitize\", \"address\") " << std::endl;
        oss << "        } else if (k == \"config\" && v == \"profile\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-pg\", \"\") " << std::endl;
        oss << "        } else if (k == \"c++-standard\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-std\", \"c++${v}\") " << std::endl;
        oss << "        } else if (k == \"c-standard\") {" << std::endl;
        oss << "            kv.append(Part.Pre, \"-std\", \"c${v}\")" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else if (e.type == ElementType.Archive) {" << std::endl;
        oss << "        if (false) {" << std::endl;
        oss << "        } else {" << std::endl;
        oss << "            return false" << std::endl;
        oss << "        }" << std::endl;
        oss << "    } else {" << std::endl;
        oss << "        return false" << std::endl;
        oss << "    }" << std::endl;
        oss << "    return true" << std::endl;
        oss << "})" << std::endl;
        oss << std::endl;

        oss << "for( s: [";
        for(auto it = compilers.begin(); it != compilers.end(); ++it)
            oss << (it != compilers.begin() ? ", " : "") << "[Language." << it->first << ", \"" << it->second << "\"]";
        oss << "]) {" << std::endl;

        oss << "    var compiler = cook.toolchain.element(ElementType.Compile, s[0])" << std::endl;
        oss << "    var & kv = compiler.key_values()" << std::endl;
        oss << "    var & tm = compiler.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return k } else { return \"${k}=${v}\" } }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return \"-o ${k}\" }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.DepFile]        = fun(k,v) { return \"-MMD -MF ${k}\" }" << std::endl;
        oss << "    tm[Part.Define]         = fun(k,v) { if (v.empty) { return \"-D${k}\" } else { return \"-D${k}=${v}\" } }" << std::endl;
        oss << "    tm[Part.IncludePath]    = fun(k,v) { if (k.empty) { return \"-I./\" } else { return \"-I${k}\" } }" << std::endl;
        oss << "    tm[Part.ForceInclude]   = fun(k,v) { return \"-include${k}\" }" << std::endl;
        oss << "" << std::endl;
        oss << "    kv.append(Part.Cli, s[1])" << std::endl;
        oss << "    kv.append(Part.Pre, \"-c\")" << std::endl;
        oss << "    kv.append(Part.Pre, \"-fPIC\")" << std::endl;
        oss << "}" << std::endl;
        oss << std::endl;

        oss << "{" << std::endl;
        oss << "    var linker = cook.toolchain.element(ElementType.Link, Language.Binary)" << std::endl;
        oss << "    var & kv = linker.key_values()" << std::endl;
        oss << "    var & tm = linker.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    " << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { if (v.empty) { return k } else { return \"${k}=${v}\" } }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return \"-o ${k}\" }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Library]        = fun(k,v) { return \"-l${k}\" }" << std::endl;
        oss << "    tm[Part.LibraryPath]    = fun(k,v) { if (k.empty) { return \"-L./\" } else { return \"-L${k}\" } }" << std::endl;
        oss << "    tm[Part.ForceInclude]   = fun(k,v) { return \"-include${k}\" }" << std::endl;
        oss << "" << std::endl;
        oss << "    kv.append(Part.Cli, \"" << linker << "\")" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "{" << std::endl;
        oss << "    var archiver = cook.toolchain.element(ElementType.Archive, Language.Binary)" << std::endl;
        oss << "    var & kv = archiver.key_values()" << std::endl;
        oss << "    var & tm = archiver.translators()" << std::endl;
        oss << "" << std::endl;
        oss << "    kv.append(Part.Cli, \"ar\")" << std::endl;
        oss << "    tm[Part.Cli]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Pre]            = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Output]         = fun(k,v) { return k }" << std::endl;
        oss << "    tm[Part.Input]          = fun(k,v) { return k }" << std::endl;
        oss << "    if (my(OS) == OS.MacOS) {" << std::endl;
        oss << "        kv.append(Part.Pre, \"crs\")" << std::endl;
        oss << "    } else {" << std::endl;
        oss << "        kv.append(Part.Pre, \"crf\")" << std::endl;
        oss << "    }" << std::endl;
        oss << "}" << std::endl;
    }

} } } }
