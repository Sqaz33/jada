#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>


namespace graphviz {
    
class GraphViz;

GraphViz* createGraphViz(
    bool isTree, bool isDirected, const std::string& name);

using VertexType = Agnode_t*;

class GraphViz {
public:
    GraphViz(const GraphViz&) = delete;
    GraphViz& operator=(const GraphViz&) = delete;

    friend GraphViz*
    createGraphViz(
        bool isTree, bool isDirected, const std::string& name);

public:
    void printDOT(std::ostream& out);
    
    VertexType addVertex(const std::string& name); // auto style
    VertexType addVertex(const std::string& name, 
                         const std::vector<std::string>& desc); // auto style
    void addEdge(VertexType v, VertexType u); // auto style
    void addEdge(const std::string& name, 
                 VertexType v, VertexType u); // auto style
    void nameNextEdge(const std::string& name);


private:
    GraphViz(bool isTree, bool isDirected, const std::string& name);

private:
    struct GraphDeleter {
        void operator()(Agraph_t* g);
    };
    std::unique_ptr<Agraph_t, GraphDeleter> graph_;

    struct CpDeleter {
        void operator()(char* cp);
    };
    using CharCp_ = std::unique_ptr<char, CpDeleter>; 
    std::string edgeName_;
    int id_ = 0;
};

} // namespace graphviz