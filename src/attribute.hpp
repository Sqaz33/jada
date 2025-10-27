#pragma once

#include <string>
#include <vector>


namespace attribute {

class QualifiedName { // TODO: expand interface
public:
    void push(const std::string& name);
    void print() const;
    
    auto operator<=>(const QualifiedName&) const = default;

private:
    std::vector<std::string> fullName_;
};

} // namespace attribute

