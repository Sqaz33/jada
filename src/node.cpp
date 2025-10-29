#include "node.hpp"

#include <algorithm>


// Stms
namespace node {

Body::Body(const std::vector<IStm*>& stms) :
    stms_(stms)
{}

Body::~Body() {
    for (auto* ptr : stms_) {
        delete ptr;
    }
}

} // namespace node 


// Decls 
namespace node {

// DeclArea
DeclArea::~DeclArea() {
    for (auto* ptr : decls_) {
        delete ptr;
    }
}

void DeclArea::addDecl(IDecl* decl) {
    decls_.push_back(decl);
}

// VarDecl
VarDecl::VarDecl(const std::string& name, IType* type, IExpr* rval) :
    name_(name)
    , type_(type)
    , rval_(rval)
{}

VarDecl::~VarDecl() {
    delete type_;
    delete rval_;
}

// FuncDecl
FuncDecl::FuncDecl(const std::string& name, 
                   const std::vector<FuncDecl::ParamType>& params,
                   IType* retType,
                   DeclArea* decls,
                   Body* body):
    name_(name)
    , params_(params)
    , retType_(retType)
    , decls_(decls)
    , body_(body)
{}

FuncDecl::~FuncDecl() {
    for (auto&& [param, _] : params_) {
        delete param;
    }
    delete retType_;
    delete decls_;
    delete body_;
}

// ProcDecl
ProcDecl::ProcDecl(const std::string& name, 
                   const std::vector<ParamType>& params,
                   DeclArea* decls,
                   Body* body):
    name_(name)
    , params_(params)
    , decls_(decls)
    , body_(body)
{}

ProcDecl::~ProcDecl() {
    for (auto&& [param, _] : params_) {
        delete param;
    }
    delete decls_;
    delete body_;
}

// ProcDecl
PackDecl::PackDecl(const std::string& name, 
                   DeclArea* decls,
                   DeclArea* privateDecls):
    name_(name)
    , decls_(decls)
    , privateDecls_(privateDecls)
{}

PackDecl::~PackDecl() {
    delete decls_;
}

// Use Decl 
UseDecl::UseDecl(attribute::QualifiedName name) :
    name_(std::move(name))
{}

// With Decl 
WithDecl::WithDecl(attribute::QualifiedName name) :
    name_(std::move(name))
{}

// RecordDecl
RecordDecl::RecordDecl(const std::string& name, 
                       const std::vector<VarDecl*>& decls, 
                       attribute::QualifiedName base, 
                       bool isTagged) :
    name_(name)
    , decls_(decls)
    , base_(std::move(base))
    , isTagged_(isTagged)
{ isInherits_ = !base.empty(); }

RecordDecl::~RecordDecl() {
    for (auto* decl : decls_) {
        delete decl;
    }
}

TypeAliasDecl::TypeAliasDecl(const std::string& name, IType* origin):
    name_(name)
    , origin_(origin)
{}

TypeAliasDecl::~TypeAliasDecl() {
    delete origin_;
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
                     IType* type) :
    ranges_(ranges)
    , type_(type)
{}
ArrayType::~ArrayType() { delete type_; }

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

// Aggregate
Aggregate::Aggregate(const std::vector<ILiteral*>& inits) :
    inits_(inits)
{}

Aggregate::~Aggregate() {
    for (auto* lit : inits_) {
        delete lit;
    }
}

} // namespace node 


// Exprs
namespace node {

Op::Op(IExpr* lhs, OpType opType, IExpr* rhs) :
    lhs_(lhs)
    , opType_(opType)
    , rhs_(rhs)
{}

Op::~Op() {
    delete rhs_;
    delete lhs_;
}

} // namespace node 


// Exprs - Literals
namespace node {

// SimpleLiteral
SimpleLiteral::~SimpleLiteral() {
    delete type_;
}

// StringLiteral
StringLiteral::StringLiteral(StringType* type, 
               const std::string& str) :
    str_(str)
    , type_(type)
{}

StringLiteral::~StringLiteral() {
    delete type_;
}

} // namespace node 

// Stms - Control Structure
namespace node {

If::If(IExpr* cond, 
       Body* body, 
       Body* els, 
       const std::vector<std::pair<IExpr*, Body*>>& elsifs):
    cond_(cond)
    , body_(body)
    , els_(els)
    , elsifs_(elsifs)
{}

If::~If() {
    delete cond_;
    delete body_;
    delete els_;
    for (auto&& [cond, body] : elsifs_) {
        delete cond; 
        delete body;
    }
}


// For
For::For(const std::string& init, 
         std::pair<IExpr*, IExpr*> range, 
         Body* body) :
    init_(init)
    , range_(range)
    , body_(body)
{}

For::~For() {
    delete body_;
    delete range_.first;
    delete range_.second;
}

// While
While::While(IExpr* cond, 
            Body* body) :
    cond_(cond)
    , body_(body)
{}

While::~While() {
    delete cond_;
    delete body_;
}

} // namespace node 

// Stms - Other
namespace node {

// CallOrIndexingOrVar:
CallOrIndexingOrVar::CallOrIndexingOrVar(
    attribute::QualifiedName name, 
    const std::vector<IExpr*>& args):
    name_(std::move(name))
    , args_(args)
{}

CallOrIndexingOrVar::CallOrIndexingOrVar(
    attribute::Attribute attr, 
    const std::vector<IExpr*>& args) :
    attr_(std::move(attr))
    , args_(args)
{}

CallOrIndexingOrVar::~CallOrIndexingOrVar() {
    for (auto* arg : args_) {
        delete arg;
    }
}

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
Assign::Assign(CallOrIndexingOrVar* lval,
               IExpr* rval) :
    lval_(lval)
    , rval_(rval)
{}

Assign::~Assign() {
    delete lval_;
    delete rval_;
}
    
// CallOrIndexingOrVarStm
CallOrIndexingOrVarStm::
CallOrIndexingOrVarStm(CallOrIndexingOrVar* CIV):
    CIV_(CIV)
{}

CallOrIndexingOrVarStm::
~CallOrIndexingOrVarStm() {
    delete CIV_;
}

// Return 
Return::Return(IExpr* ret) : ret_(ret) {}

Return::~Return() { delete ret_; }

} // namespace node