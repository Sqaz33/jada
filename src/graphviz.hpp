#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>


namespace graphviz {
    
class GraphViz {
public:
    using VertexType = Agnode_t*;

public:
    GraphViz(bool isTree, bool isDirected, const std::string& name);

    void printDOT(std::ostream& out);
    
    VertexType addVertex(const std::string& name);
    VertexType addVertex(const std::string& name, 
                         const std::vector<std::string>& desc); // auto style
    void addEdge(VertexType v, VertexType u); // auto style

private:
    struct GraphDeleter {
        void operator()(Agraph_t* g);
    };
    std::unique_ptr<Agraph_t, GraphDeleter> graph_;

    struct CpDeleter {
        void operator()(char* cp);
    };
    using CharCp_ = std::unique_ptr<char, CpDeleter>; 
};

} // namespace graphviz