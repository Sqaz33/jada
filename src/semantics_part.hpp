#pragma once

#include "isemantics_part.hpp"

namespace semantics_part {

class EntryPointCheck : public ISemanticsPart { 
public:
    std::string analyse(
            std::vector<mdl::Module> program) override;
};


} // namespace semantics_part