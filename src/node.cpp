#include "node.hpp"

#include <algorithm>

// INode
namespace node {

void INode::setParent(INode* parent) {
    parent_ = parent;
}

std::shared_ptr<INode> INode::self() {
    return shared_from_this();
}

} 

// Stms
namespace node {

Body::Body(const std::vector<std::shared_ptr<IStm>>& stms) :
    stms_(stms)
{}

} // namespace node 

// Decls 
namespace node {

// IDecl 

std::vector<
    std::vector<std::shared_ptr<IDecl>>>  
IDecl::reachable(
    const attribute::QualifiedName& name, 
    const std::string& requester) 
{   
    if (dynamic_cast<VarDecl*>(this) || 
         dynamic_cast<TypeAliasDecl*>(this)) 
    {
        return {};
    }

    std::vector<
        std::vector<std::shared_ptr<IDecl>>> res;

    reachable_(res, name.begin(), name.end(), requester);
    
    if (parent_) {
        auto buf = dynamic_cast<IDecl*>
                    (parent_)->reachable(name, this->name());
        res.insert(res.end(), buf.begin(), buf.end());
    }

    return res;
}

// DeclArea
void DeclArea::addDecl(std::shared_ptr<IDecl> decl) {
    decls_.push_back(decl);
    decl->setParent(parent_);
}

void DeclArea::replaceDecl(
    const std::string& name, 
    std::shared_ptr<IDecl> decl) 
{
    for (auto&& d : decls_) {
        if (d->name() == name) {
            d.swap(decl);
            d->setParent(this);
            return;
        }
    }
    throw std::logic_error("There is no declaration" 
                           " with that name" 
                           " in this declaration area: " 
                           + name);   
}

std::shared_ptr<IDecl> 
DeclArea::findDecl(const std::string& name, 
                   const std::string& requester)
{
    auto itDecl = std::find_if(
        decls_.begin(), 
        decls_.end(), 
        [&name](auto decl) 
        { return decl->name() == name; });

    auto itReq = std::find_if(
        decls_.begin(), 
        decls_.end(), 
        [&requester](auto decl) 
        { return decl->name() == requester; });

    if (itDecl != decls_.end() && 
        (itReq == decls_.end() || itDecl <= itReq )) 
    {
        return *itDecl;
    } 
    return nullptr;
}

std::vector<std::shared_ptr<IDecl>>::iterator 
DeclArea::begin() {
    return decls_.begin();
}

std::vector<std::shared_ptr<IDecl>>::iterator 
DeclArea::end() {
    return decls_.end();
}

void DeclArea::setParent(INode* parent) {
    INode::setParent(parent);
    std::for_each(decls_.begin(), decls_.end(), 
        [this](auto decl) { decl->setParent(parent_); });
}

// VarDecl
VarDecl::VarDecl(const std::string& name, 
                 std::shared_ptr<IType> type, 
                 std::shared_ptr<IExpr> rval) :
    name_(name)
    , type_(type)
    , rval_(rval)
{
    type_->setParent(this);
    if (rval_) {
        rval_->setParent(this);
    }
}

void VarDecl::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        const std::string& requester)
{ return; }

const std::string& VarDecl::name() const noexcept {
    return name_;
}

// ProcDecl
ProcDecl::ProcDecl(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body) :
    name_(name)
    , params_(params)
    , decls_(decls)
    , body_(body)
{
    for (auto param : params_) {
        param->setParent(this);
    }
    if (decls_) {
        decls_->setParent(this);
    }
    body_->setParent(this);
}

const std::string& ProcDecl::name() const noexcept {
    return name_;
}

std::shared_ptr<VarDecl> 
ProcDecl::findParam(const std::string& name) {
    auto r = std::find_if(
        params_.begin(), 
        params_.end(), 
        [&name] (auto par) 
        { return par->name() == name; });

    if (r != params_.end()) {
        return *r;
    }

    return nullptr;
}

std::shared_ptr<IDecl> 
ProcDecl::findDecl(
    const std::string& name, 
    const std::string& requester) 
{
    return decls_->findDecl(name, requester);
}

void ProcDecl::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        const std::string& requester) 
{
    if (it == end) return;

    bool emplace = false;
    if (std::distance(it, end) == 1) {
        for (auto& param : params_) {
            if (param->name() == *it) {
                if (!emplace) {
                    res.emplace_back();
                    emplace = true;
                }
                res.back().push_back(param);
            }
        }
    }

    auto declsBegin = decls_->begin();
    auto declsEnd = decls_->end();

    for (; declsBegin != declsEnd; ++declsBegin) {
        if ((*declsBegin)->name() == *it) {
            auto decl = *declsBegin;
            if (std::distance(it, end) == 1) {
                if (!emplace) {
                    res.emplace_back();
                    emplace = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcDecl>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
    }
}

// FuncDecl
FuncDecl::FuncDecl(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body,
                   std::shared_ptr<IType> retType) :
    ProcDecl(name, params, decls, body)
    , retType_(retType)
{ retType_->setParent(this); }

// PackDecl
PackDecl::PackDecl(const std::string& name, 
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<DeclArea> privateDecls):
    name_(name)
    , decls_(decls)
    , privateDecls_(privateDecls)
{
    decls_->setParent(this);
    if (privateDecls_) { 
        privateDecls_->setParent(this);
    }
}

const std::string& PackDecl::name() const noexcept {
    return name_;
}

std::shared_ptr<IDecl> 
PackDecl::findDecl(const std::string& name,  
                   const std::string& requester)
{
    return decls_->findDecl(name, requester);
}

void PackDecl::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        const std::string& requester) 
{
    if (it == end) return;

    auto declsBegin = decls_->begin();
    auto declsEnd = decls_->end();

    bool emplace = false;
    for (; declsBegin != declsEnd; ++declsBegin) {
        if ((*declsBegin)->name() == *it) {
            auto decl = *declsBegin;
            if (std::distance(it, end) == 1) {
                if (!emplace) {
                    res.emplace_back();
                    emplace = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcDecl>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
    }
}

// Use Decl 
Use::Use(attribute::QualifiedName name) :
    name_(std::move(name))
{}

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
                       std::shared_ptr<DeclArea> decls, 
                       attribute::QualifiedName base, 
                       bool isTagged) :
    name_(name)
    , decls_(decls)
    , base_(std::move(base))
    , isTagged_(isTagged)
{ 
    isInherits_ = !base.empty(); 
    decls_->setParent(this);
}

const std::string& RecordDecl::name() const noexcept {
    return name_;
}

std::shared_ptr<IDecl> 
RecordDecl::findDecl(const std::string& name,  
                     const std::string& requester)
{
    return decls_->findDecl(name, requester); 
}

void RecordDecl::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    const std::string& requester)
{
    auto declsBegin = decls_->begin();
    auto declsEnd = decls_->end();
    
    bool emplace = false;
    for (; declsBegin != declsEnd; ++declsBegin) {
        if ((*declsBegin)->name() == *it) {
            auto decl = *declsBegin;
            if (std::distance(it, end) == 1) {
                if (!emplace) {
                    res.emplace_back();
                    emplace = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcDecl>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
    }
}

//TypeAliasDecl
TypeAliasDecl::TypeAliasDecl(const std::string& name, 
                            std::shared_ptr<IType> origin):
    name_(name)
    , origin_(origin)
{
    origin_->setParent(this);
}

const std::string& TypeAliasDecl::name() const noexcept {
    return name_;
}

void TypeAliasDecl::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    const std::string& requester) 
{ return; }

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
{
    type_->setParent(this);
}

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

// Aggregate
Aggregate::Aggregate(const std::vector<
                        std::shared_ptr<ILiteral>>& inits) :
    inits_(inits)
{
    for (auto i : inits_) {
        i->setParent(this);
    }
}

} // namespace node 

// Exprs
namespace node {

Op::Op(std::shared_ptr<IExpr> lhs, 
       OpType opType, 
       std::shared_ptr<IExpr> rhs) :
    lhs_(lhs)
    , opType_(opType)
    , rhs_(rhs)
{
    lhs_->setParent(this);
    rhs_->setParent(this);
}

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
{
    name_->setParent(this);
    for (auto arg : args) {
        arg->setParent(this);
    }
}

} // namespace node 

// Exprs - Literals
namespace node {

// StringLiteral
StringLiteral::StringLiteral(std::shared_ptr<StringType> type, 
                             const std::string& str) :
    str_(str)
    , type_(type)
{
    type_->setParent(this);
}

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
{
    cond_->setParent(this);
    body_->setParent(this);
    if (els_) {
        els_->setParent(this);
    }
    for (auto [cnd, bdy] : elsifs_) {
        cnd->setParent(this);
        bdy->setParent(this);
    }
}

// For
For::For(const std::string& init, 
         std::pair<std::shared_ptr<IExpr>, std::shared_ptr<IExpr>> range, 
         std::shared_ptr<Body> body) :
    init_(init)
    , range_(range)
    , body_(body)
{
    range_.first->setParent(this);
    range_.second->setParent(this);
    body_->setParent(this);
}

// While
While::While(std::shared_ptr<IExpr> cond, 
            std::shared_ptr<Body> body) :
    cond_(cond)
    , body_(body)
{
    cond_->setParent(this);
    body_->setParent(this);
}

} // namespace node 

// Stms - Other
namespace node {

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
{
    lval_->setParent(this);
    rval_->setParent(this);
}

// Return 
Return::Return(std::shared_ptr<IExpr> retVal) : 
    retVal_(retVal) 
{
    retVal_->setParent(this);
}

} // namespace node