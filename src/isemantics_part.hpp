#pragma once 

#include <memory>
#include <string>

#include "module.hpp"

namespace semantics_part {

struct ISemanticsPart {
    virtual ~ISemanticsPart() = default;

    void setTail(
        std::weak_ptr<ISemanticsPart> tail);

    virtual std::string analyse(
                const std::vector<
                    std::shared_ptr<mdl::Module>>& program) = 0; 

protected:
    std::string analyseNext(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program);

private:
    std::weak_ptr<ISemanticsPart> next_;
};

using SharedPtr = std::shared_ptr<ISemanticsPart>;
using WeakPtr = std::weak_ptr<ISemanticsPart>;

} // namespace semantics_part