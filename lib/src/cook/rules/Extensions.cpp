#include "cook/rules/Extensions.hpp"
#include "gubg/mss.hpp"
#include <vector>
#include <string>

namespace cook { namespace rules { 

    namespace details { 
        bool fill(TypeMap &tm, Language lang)
        {
            MSS_BEGIN(bool);
            using Exts = std::vector<std::string>;

            if (false) {}
            else if (lang == Language::CXX)
            {
                for (auto ext: Exts{ ".cpp", ".cxx", ".CPP", ".CXX" })
                    tm[ext] = Type::Source;
                for (auto ext: Exts{ ".hpp", ".hxx", ".HPP", ".HXX" })
                    tm[ext] = Type::Header;
            }
            else if (lang == Language::ObjectiveCXX)
            {
                for (auto ext: Exts{ ".mm", ".MM" })
                    tm[ext] = Type::Source;
            }
            else if (lang == Language::C)
            {
                for (auto ext: Exts{ ".c", ".cc", ".C", ".CC" })
                    tm[ext] = Type::Source;
                for (auto ext: Exts{ ".h" })
                    tm[ext] = Type::Header;
            }
            else if (lang == Language::ObjectiveC)
            {
                for (auto ext: Exts{ ".m", ".M" })
                    tm[ext] = Type::Source;
            }
            else if (lang == Language::ASM)
            {
                for (auto ext: Exts{ ".asm", ".S", ".s", ".ASM" })
                    tm[ext] = Type::Source;
            }
            else if (lang == Language::Resource)
            {
                for (auto ext: Exts{ ".rc" })
                    tm[ext] = Type::Source;
            }
            else if (lang == Language::Binary)
            {
                for (auto ext: Exts{ ".o", ".obj", ".res" })
                    tm[ext] = Type::Object;
                for (auto ext: Exts{ ".a", ".so", ".lib", ".dll" })
                    tm[ext] = Type::Library;
            }
            else if (lang == Language::Definition)
            {
                for (auto ext: Exts{ ".def" })
                    tm[ext] = Type::Export;
            }
            else if (lang == Language::PropertyList)
            {
                for (auto ext: Exts{ ".plist" })
                    tm[ext] = Type::Bundle;
            }
            else if (lang == Language::Icon)
            {
                for (auto ext: Exts{ ".icns" })
                    tm[ext] = Type::Bundle;
            }
            else
            {
                MSS(false);
            }
            MSS_END();
        }
    } 

} } 
