#include "semantics.hpp"

namespace semantics {
    
void ADASementics::addPart(
    semantics_part::SharedPtr part) 
{
    if (head_.expired()) {
        head_ = part;
        return;
    }
    head_.lock()->setTail(part);
}

std::pair<bool, std::string>
ADASementics::analyse(
    const std::vector<mdl::Module>& program)
{
    auto msg = head_.lock()->analyse(program);
    return {msg.empty(), msg};
}

}