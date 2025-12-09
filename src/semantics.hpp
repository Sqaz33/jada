#pragma once

#include <vector>
#include <utility>

#include "module.hpp"
#include "isemantics_part.hpp"

namespace semantics {

class ADASementics {
public:
    void addPart(semantics_part::SharedPtr part);
    std::pair<bool, std::string> analyse(
        const std::vector<mdl::Module>& program);

private:
    semantics_part::WeakPtr head_;
};

} // namespace semantics