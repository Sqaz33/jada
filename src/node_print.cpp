#include "node.hpp"

#include <unordered_map>
#include <sstream>

namespace node {

void Body::print(graphviz::GraphViz& gv, 
                 graphviz::VertexType par) const 
{
    auto v = gv.addVertex("body");
    gv.addEdge(par, v);
    for (auto stm : stms_) {
        stm->print(gv, v);
    }
}

void DeclArea::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{   
    auto v = gv.addVertex("decl area");
    gv.addEdge(par, v);
    for (auto decl : decls_) {
        decl->print(gv, v);
    }
}

void VarDecl::print(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const 
{
    auto v = gv.addVertex("var decl", 
                    {"Name:", name_});
    gv.addEdge(par, v);
    gv.nameNextEdge("type");
    type_->print(gv, v);
    if (rval_) {
        gv.nameNextEdge("=");
        rval_->print(gv, v);
    }
}

void ProcBody::printParam_(const std::shared_ptr<VarDecl> param, 
                           graphviz::GraphViz& gv, 
                           graphviz::VertexType par) const
{ 
    param->print(gv, par);
}

void ProcBody::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{
    std::string name = "Proc Decl";
    if (dynamic_cast<const FuncBody*>(this)) {
        name = "Func Decl";
    }

    auto v = gv.addVertex(name, {"Name: " + name_});
    gv.addEdge(par, v);

    for (auto&& param: params_) {
        printParam_(param, gv, v);
    }

    if (decls_) {
        decls_->print(gv, v); 
    }

    body_->print(gv, v);

    if (auto self = 
            dynamic_cast<const FuncBody*>(this)) {
        gv.nameNextEdge("ret");
        self->retType_->print(gv, v);
    }
}

void FuncBody::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{ ProcBody::print(gv, par); }

void PackDecl::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{   
    auto v = gv.addVertex("Pack Decl", 
                          {"Name: " + name_});
    gv.addEdge(par, v);

    if (decls_) {
        gv.nameNextEdge("public");
        decls_->print(gv, v);
    }
}

void Use::print(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Use", 
                          {"Name: " + name_.toString('.')});
    gv.addEdge(par, v);
}

void With::print(graphviz::GraphViz& gv, 
                 graphviz::VertexType par) const 
{
    auto v = gv.addVertex("With", 
                          {"Name: " + name_.toString('.')});
    gv.addEdge(par, v);
}

void RecordDecl::print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const 
{
    std::vector<std::string> desc;
    desc.push_back("Name: " + name_);
    desc.push_back("Is Tagged: " 
                    + std::to_string(isTagged_));
    desc.push_back("Is Inherits: " 
                    + std::to_string(isInherits_));
    desc.push_back("Base Name: " 
                    + base_.toString('.'));
    auto v = gv.addVertex("Type Record Decl", desc);
    gv.addEdge(par, v);
    decls_->print(gv, v);
}

void TypeAliasDecl::print(graphviz::GraphViz& gv, 
                          graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Type Alias", 
                          {"Name: " + name_});
    gv.addEdge(par, v);
    gv.nameNextEdge("orig");
    origin_->print(gv, v);
}

void SimpleLiteralType::print(graphviz::GraphViz& gv, 
                              graphviz::VertexType par) const 
{
    static const std::unordered_map<SimpleType, std::string> types {
        { SimpleType::INTEGER, "Integer" },
        { SimpleType::CHAR, "Character" },
        { SimpleType::BOOL, "Boolean" },
        { SimpleType::FLOAT, "Float" }
    };
    auto v = gv.addVertex(types.at(type_));
    gv.addEdge(par, v);
}

void ArrayType::print(graphviz::GraphViz& gv, 
                      graphviz::VertexType par) const 
{   
    std::stringstream ss;    
    ss << "Ranges: (";
    for (auto&& [l, r] : ranges_) {
        ss << '(' << l << ", " << r << ")";
    }
    ss << ")";
    auto v = gv.addVertex("Array Type", {ss.str()});
    gv.addEdge(par, v);
}

void StringType::print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const 
{
    std::stringstream ss;    
    ss << "Range: ("
       << range_.first << ", " << range_.second
       << ")";
    auto v = gv.addVertex("String Type", {ss.str()});
    gv.addEdge(par, v);
}

void Aggregate::print(graphviz::GraphViz& gv, 
                      graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Aggregate");
    gv.addEdge(par, v);
    printInits_(gv, v);
}

void Aggregate::printInits_(graphviz::GraphViz& gv, 
                            graphviz::VertexType par) const 
{
    for (auto lit : inits_) {
        gv.nameNextEdge("val");
        lit->print(gv, par);
    }
}

void Op::print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const 
{
    static const std::unordered_map<OpType, std::string> ops = {
        { OpType::EQ, "EQ" },
        { OpType::NEQ, "NEQ" },
        { OpType::MORE, "MORE" },
        { OpType::LESS, "LESS" },
        { OpType::GTE, "GTE" },
        { OpType::LTE, "LTE" },
        { OpType::AMPER, "AMPER" },
        { OpType::PLUS, "PLUS" },
        { OpType::MINUS, "MINUS" },
        { OpType::MUL, "MUL" },
        { OpType::DIV, "DIV" },
        { OpType::MOD, "MOD" },
        { OpType::UMINUS, "UMINUS" },
        { OpType::DOT, "DOT"}
    };

    auto v = gv.addVertex("Op", 
                {"Type: " + ops.at(opType_)});
    gv.addEdge(par, v);

    if (lhs_) {
        gv.nameNextEdge("lhs");
        lhs_->print(gv, v);
    }
    if (rhs_) {
        gv.nameNextEdge("rhs");
        rhs_->print(gv, v);
    }
}

void NameExpr::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Name expr", 
                            {"Name: ", name_});
    gv.addEdge(par, v);
}

void AttributeExpr::print(graphviz::GraphViz& gv, 
                          graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Attribute expr", 
                        {"Attr: ", attr_.toString()});
    gv.addEdge(par, v);
}

void CallOrIdxExpr::print(graphviz::GraphViz& gv, 
                          graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Call or idx expr");
    gv.addEdge(par, v);
    gv.nameNextEdge("name");
    name_->print(gv, v);
    printArgs_(gv, v);
}

void CallOrIdxExpr::printArgs_(graphviz::GraphViz& gv, 
                               graphviz::VertexType par) const 
{
    for (auto arg : args_) {
        gv.nameNextEdge("arg");
        arg->print(gv, par);
    }
}


std::string
SimpleLiteral::stringifyValue_() const 
{
    std::stringstream ss;
    switch (type_->type()) {
        case SimpleType::INTEGER: 
            ss << get<int>();
            break;
        case SimpleType::BOOL: 
            ss << get<bool>();
            break;
        case SimpleType::CHAR: 
            ss << get<char>();
            break;
        case SimpleType::FLOAT: 
            ss << get<float>();
            break;
    }
    return ss.str();
}

void SimpleLiteral::print(graphviz::GraphViz& gv, 
                          graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Literal", 
                {"Value: " + stringifyValue_()});
    gv.addEdge(par, v);
    type_->print(gv, v);
}

void StringLiteral::print(graphviz::GraphViz& gv, 
                          graphviz::VertexType par) const 
{                            
    auto v = gv.addVertex("String Liteal", 
            {"Value: " + str_});
    gv.addEdge(par, v);
    type_->print(gv, v);
}

void If::printElsif_(std::pair<std::shared_ptr<IExpr>, 
                              std::shared_ptr<Body>> elsif, 
                     graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{
        
    gv.nameNextEdge("elsif cond");
    elsif.first->print(gv, par);
    gv.nameNextEdge("elsif body");
    elsif.second->print(gv, par);
}

void If::printElse_(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const 
{
    if (!els_) return;
    gv.nameNextEdge("else");
    els_->print(gv, par);
}

void If::print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const 
{
    auto v = gv.addVertex("If");
    gv.addEdge(par, v);

    gv.nameNextEdge("cond");
    cond_->print(gv, v);
    gv.nameNextEdge("body");
    body_->print(gv, v);
    
    for (auto&& elsif : elsifs_) {
        printElsif_(elsif, gv, v);
    }
    
    printElse_(gv, v);
}

void For::print(graphviz::GraphViz& gv, 
                graphviz::VertexType par) const 
{
    auto v = gv.addVertex("For", 
                {"Var: " + init_});
    gv.addEdge(par, v);
    gv.nameNextEdge("left");
    range_.first->print(gv, v);
    gv.nameNextEdge("right");
    range_.second->print(gv, v);
    gv.nameNextEdge("body");
    body_->print(gv, v);
}

void While::print(graphviz::GraphViz& gv, 
                  graphviz::VertexType par) const 
{
    auto v = gv.addVertex("While");
    gv.addEdge(par, v);
    gv.nameNextEdge("cond");
    cond_->print(gv, v);
    gv.nameNextEdge("body");
    body_->print(gv, v);
}

void TypeName::print(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const 
{
    auto name = name_.empty() ? 
                  attr_.toString() : 
                  name_.toString('.');
    auto v = gv.addVertex("Type Name", 
                    {"Name: " + name});
    gv.addEdge(par, v);
} 

void Assign::print(graphviz::GraphViz& gv, 
                  graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Assign");
    gv.addEdge(par, v);
    gv.nameNextEdge("lval");
    lval_->print(gv, v);
    gv.nameNextEdge("rval");
    rval_->print(gv, v);
}

void MBCall::print(graphviz::GraphViz& gv, 
                   graphviz::VertexType par) const 
{
    auto v = gv.addVertex("MB call");
    gv.addEdge(par, v);
    call_->print(gv, v);
}

void Return::print(graphviz::GraphViz& gv, 
                   graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Return stm");
    gv.addEdge(par, v);
    if (retVal_) {
        retVal_->print(gv, v);
    }
}

} // namespace node