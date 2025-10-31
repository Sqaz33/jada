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
    bool empty() const;
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const;
    
    auto operator<=>(const QualifiedName&) const = default;

private:
    std::vector<std::string> fullName_;
};

class Attribute {
public:
    Attribute(QualifiedName left, const std::string& right);
    Attribute() = default;
    
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const;
private:
    QualifiedName left_; 
    std::string right_;
};

} // namespace attribute

