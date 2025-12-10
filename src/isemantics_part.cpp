#include "isemantics_part.hpp"

namespace semantics_part  {

void ISemanticsPart::setTail(
        std::weak_ptr<ISemanticsPart> tail) 
{
    if (next_.expired()) {
        next_ = tail;
        return;
    }
    next_.lock()->setTail(tail);
}

std::string ISemanticsPart::analyseNext(
        const std::vector<std::shared_ptr<mdl::Module>>& program) 
{
    if (!next_.expired()) {
        return next_.lock()->analyse(program);
    }
    return "";
}

} // namespace semantics_part