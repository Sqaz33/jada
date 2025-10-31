#include "attribute.hpp"

#include <iostream>

namespace attribute {

QualifiedName::QualifiedName(const std::string& base) :
    fullName_({base})
{}

void QualifiedName::push(const std::string& name) {
    fullName_.push_back(name);
}

bool QualifiedName::empty() const {
    return fullName_.empty();    
}

void QualifiedName::print(graphviz::GraphViz& gv, 
                       VertexType par) const {
    if (empty()) return;
    std::cout << std::string(spc, ' ')
              << "Qualified name: ";
    for (int i = 0; i < fullName_.size() - 1; ++i) {
        std::cout << fullName_[i] << '.';
    }
    std::cout << fullName_.back();
    std::cout << '\n';
}

Attribute::Attribute(QualifiedName left, const std::string& right) :
    left_(std::move(left))
    , right_(right)
{}

void Attribute::print(graphviz::GraphViz& gv, 
                       VertexType par) const {
    if (left_.empty() || right_.empty()) return;
    std::cout << std::string(spc, ' ')
              << "Attribute:\n";
    left_.print(spc + 4);            
    std::cout << std::string(spc + 4, ' ')
              << "Attribute Val: "
              << right_
              << '\n';
}

} // namespace attribute