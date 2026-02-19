#pragma once

#include <string>
#include <vector>

namespace attribute {
    
class QualifiedName {
public:
    QualifiedName() = default;
    QualifiedName(const std::string& base);

public:
    void push(const std::string& name);
    bool empty() const noexcept; 
    std::string toString(char delim = '/') const;
    const std::string& first() const noexcept; 
    const std::string& last() const noexcept;
    std::size_t size() const noexcept;
    std::vector<std::string>::const_iterator begin() const;
    std::vector<std::string>::const_iterator end() const;
    void clear();
    
    auto operator<=>(const QualifiedName&) const = default;
private:
    std::vector<std::string> fullName_;
};

class Attribute {
public:
    Attribute(QualifiedName left, const std::string& right);
    Attribute() = default;
    
    std::string toString() const;

    const QualifiedName& left() const noexcept;
    const std::string& right() const noexcept;
    
private:
    QualifiedName left_; 
    std::string right_;
};

} // namespace attribute

