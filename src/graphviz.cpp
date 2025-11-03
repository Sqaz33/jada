#include "graphviz.hpp"

#include <string.h>
#include <stdexcept>
#include <sstream>

namespace graphviz {


void GraphViz::CpDeleter::operator()(char* cp) {
    free(cp);
}

GraphViz::GraphViz(bool isTree, 
                   bool isDirected,
                   const std::string& name) {   
    auto type = isDirected ? 
                   Agstrictdirected 
                :  Agstrictundirected;
    GraphViz::CharCp_ nameCp(strdup(name.c_str()));
    auto* g = agopen(nameCp.get(), type, nullptr);
    if (!g) {
        throw std::runtime_error(
            "It is impossible to create a graph");
    }
    graph_.reset(g);
    if (isTree) {
        GraphViz::CharCp_ rankdir(strdup("rankdir"));
        GraphViz::CharCp_ dir(strdup("TB"));
        agset(graph_.get(), rankdir.get(), dir.get());
    }
}

void GraphViz::GraphDeleter::operator()(Agraph_t* g) {
    agclose(g);
}

void GraphViz::printDOT(std::ostream& out) {
    struct ctx_deleter {
        void operator()(GVC_t* ctx) {
            gvFreeContext(ctx);
        }
    };

    std::unique_ptr<GVC_t, ctx_deleter> ctx(gvContext());
    char* res = nullptr;
    unsigned len = 0;
    if (gvLayout(ctx.get(), graph_.get(), "dot") != 0) {
        throw std::runtime_error(
            "It is impossible to print a graph");
    }
    if (gvRenderData(ctx.get(), graph_.get(), "dot", &res, &len) != 0) {
        gvFreeLayout(ctx.get(), graph_.get());
        throw std::runtime_error(
            "It is impossible to print a graph");
    }

    out.write(res, len);
    
    gvFreeRenderData(res);
    gvFreeLayout(ctx.get(), graph_.get());
}

VertexType GraphViz::addVertex(const std::string& name, 
                               const std::vector<std::string>& desc) 
{
    std::stringstream ss;
    ss << name << '\n';
    for (int i = 0; i < desc.size() - 1; ++i) {
        ss << desc[i] << '\n';
    }
    ss << desc.back();
    return addVertex(ss.str());
}


VertexType GraphViz::addVertex(const std::string& name) {
    VertexType v;
    auto id = std::to_string(id_++);
    GraphViz::CharCp_ idCp(strdup(id.c_str()));
    if (!(v = agnode(graph_.get(), idCp.get(), true))) {
        throw std::runtime_error(
            "It is impossible to create a vertex");
    }
    GraphViz::CharCp_ nameCp(strdup(name.c_str()));
    GraphViz::CharCp_ label(strdup("label"));
    GraphViz::CharCp_ empty(strdup(""));
    agsafeset(v, label.get(), nameCp.get(), empty.get());
    return v;
}

void GraphViz::addEdge(VertexType v, VertexType u) {
    GraphViz::CharCp_ nameCp = nullptr;
    if (edgeName_.size()) {
        nameCp.reset(strdup(edgeName_.c_str()));
        edgeName_.clear();
    }
    if (!agedge(graph_.get(), v, u, nameCp.get(), true)) {
        throw std::runtime_error(
            "It is impossible to create a edge");
    }
}

void GraphViz::addEdge(const std::string& name,
                       VertexType v, VertexType u) 
{
    GraphViz::CharCp_ nameCp(strdup(name.c_str()));
    if (!agedge(graph_.get(), v, u, nameCp.get(), true)) {
        throw std::runtime_error(
            "It is impossible to create a edge");
    }
}

void GraphViz::nameNextEdge(const std::string& name) {
    edgeName_ = name;
}


} // namespace graphviz