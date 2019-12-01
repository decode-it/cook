#ifndef HEADER_cook_model_GlobInfo_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_GlobInfo_hpp_ALREADY_INCLUDED

#include "cook/ingredient/File.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/file/system.hpp"
#include <string>
#include <optional>
#include <functional>
#include <algorithm>
#include <ostream>

namespace cook { namespace model {

    struct GlobInfo
    {
        enum Mode {Add, Remove };

        std::string dir;
        std::string pattern;
        Mode mode = Add;


        Language language = Language::Undefined;
        Type type = Type::Undefined;

        std::optional<Propagation> propagation;
        std::optional<Overwrite> overwrite;
        std::function<bool (LanguageTypePair &, ingredient::File &)> filter_and_adaptor;

        void stream(log::Importance imp = log::Importance{}) const
        {
            auto ss = log::scope("GlobInfo", imp, [&](auto &n){
                n.attr("dir", dir).attr("pattern", pattern).attr("mode", mode == Add ? "add" : "remove");
            });
        }
    };

    inline std::ostream &operator<<(std::ostream &os, const GlobInfo &globber)
    {
        os << "[GlobInfo](dir:" << globber.dir << ")(pattern: " << globber.pattern << ")(mode:" << (globber.mode == GlobInfo::Add ? "add" : "remove") << ")";
        return os;
    }

} }

#endif
