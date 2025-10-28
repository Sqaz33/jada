#pragma once

#include <string>
#include <vector>


namespace attribute {

class QualifiedName { // TODO: expand interface
public:
    void push(const std::string& name);
    bool empty() const;
    void print() const;
    
    auto operator<=>(const QualifiedName&) const = default;

private:
    std::vector<std::string> fullName_;
};


class Attribute {
public:
    Attribute(QualifiedName left, const std::string& right);

    void print() const;
private:
    QualifiedName left_; 
    std::string right_;
};

} // namespace attribute

