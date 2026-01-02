#include "node.hpp"

#include <algorithm>


// Stms
namespace node {

Body::Body(const std::vector<std::shared_ptr<IStm>>& stms) :
    stms_(stms)
{}

} // namespace node 

// Decls 
namespace node {

// DeclArea
void DeclArea::addDecl(std::shared_ptr<IDecl> decl) {
    decls_.push_back(decl);
}

// VarDecl
VarDecl::VarDecl(const std::string& name, 
                 std::shared_ptr<IType> type, 
                std::shared_ptr<IExpr> rval) :
    name_(name)
    , type_(type)
    , rval_(rval)
{}

const std::string& VarDecl::name() const noexcept {
    return name_;
}

// FuncDecl
FuncDecl::FuncDecl(const std::string& name, 
                   const std::vector<FuncDecl::ParamType>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body,
                   std::shared_ptr<IType> retType) :
    ProcDecl(name, params, decls, body)
    , retType_(retType)
{}

// ProcDecl
ProcDecl::ProcDecl(const std::string& name, 
                   const std::vector<ParamType>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body) :
    name_(name)
    , params_(params)
    , decls_(decls)
    , body_(body)
{}

const std::string& ProcDecl::name() const noexcept {
    return name_;
}

// ProcDecl
PackDecl::PackDecl(const std::string& name, 
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<DeclArea> privateDecls):
    name_(name)
    , decls_(decls)
    , privateDecls_(privateDecls)
{}

const std::string& PackDecl::name() const noexcept {
    return name_;
}

// Use Decl 
Use::Use(attribute::QualifiedName name) :
    name_(std::move(name))
{}

// const std::string& Use::name() const noexcept {
//     assert(false && "Use name");
//     return *static_cast<std::string*>(nullptr);
// }

// With 
With::With(attribute::QualifiedName name) :
    name_(std::move(name))
{}

const attribute::QualifiedName& 
With::name() const noexcept {
    return name_;
}


// RecordDecl
RecordDecl::RecordDecl(const std::string& name, 
                       const std::vector<std::shared_ptr<VarDecl>>& decls, 
                       attribute::QualifiedName base, 
                       bool isTagged) :
    name_(name)
    , decls_(decls)
    , base_(std::move(base))
    , isTagged_(isTagged)
{ isInherits_ = !base.empty(); }

const std::string& RecordDecl::name() const noexcept {
    return name_;
}

//TypeAliasDecl
TypeAliasDecl::TypeAliasDecl(const std::string& name, 
                            std::shared_ptr<IType> origin):
    name_(name)
    , origin_(origin)
{}

const std::string& TypeAliasDecl::name() const noexcept {
    return name_;
}

} // namespace node 

// Typeinfo
namespace node {

// SimpleLiteralType
SimpleLiteralType::SimpleLiteralType(SimpleType type) :
    type_(type)
{}

SimpleType SimpleLiteralType::type() const noexcept {
    return type_;
}

// ArrayType
ArrayType::ArrayType(const std::vector<std::pair<int, int>>& ranges, 
                     std::shared_ptr<IType> type) :
    ranges_(ranges)
    , type_(type)
{}

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

// Aggregate
Aggregate::Aggregate(const std::vector<
                        std::shared_ptr<ILiteral>>& inits) :
    inits_(inits)
{}


} // namespace node 

// Exprs
namespace node {

Op::Op(std::shared_ptr<IExpr> lhs, 
       OpType opType, 
       std::shared_ptr<IExpr> rhs) :
    lhs_(lhs)
    , opType_(opType)
    , rhs_(rhs)
{}

NameExpr::NameExpr(const std::string& name) :
    name_(name)
{}

AttributeExpr::AttributeExpr(const attribute::Attribute& attr) :
    attr_(attr)
{}

CallOrIdxExpr::CallOrIdxExpr(
    std::shared_ptr<IExpr> name, 
    const std::vector<std::shared_ptr<node::IExpr>>& args) :
    name_(name)
    , args_(args)
{}

} // namespace node 

// Exprs - Literals
namespace node {

// StringLiteral
StringLiteral::StringLiteral(std::shared_ptr<StringType> type, 
                             const std::string& str) :
    str_(str)
    , type_(type)
{}

} // namespace node 

// Stms - Control Structure
namespace node {

If::If(std::shared_ptr<IExpr> cond, 
       std::shared_ptr<Body> body, 
       std::shared_ptr<Body> els, 
       const std::vector<std::pair<std::shared_ptr<IExpr>, 
        std::shared_ptr<Body>>>& elsifs):
    cond_(cond)
    , body_(body)
    , els_(els)
    , elsifs_(elsifs)
{}

// For
For::For(const std::string& init, 
         std::pair<std::shared_ptr<IExpr>, std::shared_ptr<IExpr>> range, 
         std::shared_ptr<Body> body) :
    init_(init)
    , range_(range)
    , body_(body)
{}

// While
While::While(std::shared_ptr<IExpr> cond, 
            std::shared_ptr<Body> body) :
    cond_(cond)
    , body_(body)
{}

} // namespace node 

// Stms - Other
namespace node {

// // CallOrIndexingOrVar
// void CallOrIndexingOrVar::addPart(const CallOrIndexingOrVar::NamePart& part) {
//     fullName_.push_back(part);
// }

MBCall::MBCall(std::shared_ptr<IExpr> call) :
    call_(call)
{}

} // namespace node 

// Typeinfo - Other
namespace node {

// TypeName
TypeName::TypeName(attribute::QualifiedName name) :
    name_(std::move(name))
{}

TypeName::TypeName(attribute::Attribute attr) :
    attr_(std::move(attr))
{}

} // namespace node 

// Stms - Ops 
namespace node {

// Assign
Assign::Assign(std::shared_ptr<IExpr> lval,
               std::shared_ptr<IExpr> rval) :
    lval_(lval)
    , rval_(rval)
{}

// // CallOrIndexingOrVarStm
// CallOrIndexingOrVarStm::
// CallOrIndexingOrVarStm(std::shared_ptr<CallOrIndexingOrVar> CIV):
//     CIV_(CIV)
// {}

// Return 
Return::Return(std::shared_ptr<IExpr> ret) : ret_(ret) {}

} // namespace node