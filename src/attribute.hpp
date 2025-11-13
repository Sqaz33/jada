#pragma once

#include <string>
#include <vector>

#include "graphviz.hpp"

namespace attribute {

class QualifiedName { // TODO: expand interface
public:
    QualifiedName() = default;
    QualifiedName(const std::string& base);

public:
    void push(const std::string& name);
    bool empty() const noexcept; 
    std::string toSring() const;  // TODO: name/name/name/obj 
    const std::string& first() const noexcept; 
    
    auto operator<=>(const QualifiedName&) const = default;
private:
    std::vector<std::string> fullName_;
};

class Attribute {
public:
    Attribute(QualifiedName left, const std::string& right);
    Attribute() = default;
    
    std::string toString() const;
    
private:
    QualifiedName left_; 
    std::string right_;
};

} // namespace attribute

