#include "node.hpp"

#include <algorithm>

// INode
namespace node {

void INode::setParent(INode* parent) {
    parent_ = parent;
}

INode* INode::parent() noexcept {
    return parent_;
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
    std::shared_ptr<IDecl> requester)
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
                    (parent_)->reachable(name, 
                            std::dynamic_pointer_cast<node::IDecl>(self()));
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
        std::shared_ptr<IDecl> requester)
{ return; }

const std::string& VarDecl::name() const noexcept {
    return name_;
}

std::shared_ptr<IType> VarDecl::type() {
    return type_;
}

void VarDecl::resetType(std::shared_ptr<IType> type) {
    type_ = type;
}

bool VarDecl::in() const noexcept {
    return in_;
}

void VarDecl::setIn(bool in) noexcept {
    in_ = in;
}

bool VarDecl::out() const noexcept {
    return out_;
}

void VarDecl::setOut(bool out) noexcept {
    out_ = out;
}

// ProcBody
ProcBody::ProcBody(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body) :
    name_(name)
    , params_(params)
    , decls_(decls ? decls : std::make_shared<DeclArea>())
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

const std::string& ProcBody::name() const noexcept {
    return name_;
}

std::shared_ptr<DeclArea> ProcBody::decls() {
    return decls_;
}

const std::vector<std::shared_ptr<VarDecl>>& 
ProcBody::params() const noexcept {
    return params_;
}

void ProcBody::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) 
{
    if (it == end) return;

    bool insert = false;
    if (std::distance(it, end) == 1) {
        for (auto& param : params_) {
            if (param->name() == *it) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(param);
            }
        }
    }

    for (auto decl : *decls_) {
        if (decl->name() == *it) {
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
        if (decl == requester) {
            break;
        }
    }
}

// ProcDecl
ProcDecl::ProcDecl(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params) :
    ProcBody(name, params, 
             std::make_shared<DeclArea>(), 
             std::make_shared<Body>(std::vector<std::shared_ptr<IStm>>()))
{}

void ProcDecl::setBody(std::shared_ptr<ProcBody> body) {
    body_ = body;
}

// FuncBody
FuncBody::FuncBody(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params,
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<Body> body,
                   std::shared_ptr<IType> retType) :
    ProcBody(name, params, decls, body)
    , retType_(retType)
{ retType_->setParent(this); }


std::shared_ptr<IType> FuncBody::retType() {
    return retType_;
}

void FuncBody::resetRetType(std::shared_ptr<IType> type) {
    retType_ = type;
}

// FuncDecl 
FuncDecl::FuncDecl(const std::string& name, 
                   const std::vector<std::shared_ptr<VarDecl>>& params,
                   std::shared_ptr<IType> retType) :
    FuncBody(name, params, 
             std::make_shared<DeclArea>(), 
             std::make_shared<Body>(std::vector<std::shared_ptr<IStm>>()),
             retType)
{}

void FuncDecl::setBody(std::shared_ptr<ProcBody> body) {
    body_ = body;
}

// PackDecl
PackDecl::PackDecl(const std::string& name, 
                   std::shared_ptr<DeclArea> decls,
                   std::shared_ptr<DeclArea> privateDecls):
    name_(name)
    , decls_(decls)
    , privateDecls_(privateDecls)
{
    if (!decls_) {
        decls_ = std::make_shared<node::DeclArea>();
    }
    decls_->setParent(this);
    if (privateDecls_) {
        privateDecls_->setParent(this);
    }
}

const std::string& PackDecl::name() const noexcept {
    return name_;
}

void PackDecl::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) 
{
    if (it == end) return;

    bool insert = false;
    for (auto decl : *decls_) {
        if (decl->name() == *it || (requester->parent() == this && *it == name_)) {
            if (requester->parent() == this && *it == name_) {
                ++it;
            }
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
        if (decl == requester) {
            break;
        }
    }
}

void PackDecl::reachableForPackBody_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    std::shared_ptr<IDecl> requester) 
{
    if (it == end) return;

    reachable_(res, it, end, requester);

    if (!privateDecls_) {
        return;
    }
    
    bool insert = false;
    for (auto decl : *privateDecls_) {
        if (decl->name() == *it) {
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
        if (decl == requester) {
            break;
        }
    }
}

std::shared_ptr<DeclArea> PackDecl::decls() {
    return decls_;
}

std::shared_ptr<DeclArea> PackDecl::privateDecls() {
    return privateDecls_;
}

void PackDecl::setPackBody(std::shared_ptr<PackBody> body) {
    packBody_ = body;
}

std::weak_ptr<PackBody> PackDecl::packBody() {
    return packBody_;
}

// PackBody
PackBody::PackBody(const std::string& name, 
                   std::shared_ptr<DeclArea> decls) :
    PackDecl(name, decls)
{}

std::vector<
    std::vector<std::shared_ptr<IDecl>>> 
PackBody::reachable(
    const attribute::QualifiedName& name, 
    std::shared_ptr<IDecl> requester) 
{
    if (requester->parent() != this) {
        return {};
    }

    return IDecl::reachable(name, requester);
}

void PackBody::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    std::shared_ptr<IDecl> requester) 
{
    if (requester->parent() != this) {
        return;
    } 
    bool insert = false;
    for (auto decl : *decls_) {
        if (decl->name() == *it) {
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
        if (decl == requester) {
            break;
        }
    }
    auto pack = packDecl_.lock();
    assert(pack && "no pack decl for pack body");
    pack->reachableForPackBody_(res, it, end, requester);
}

void PackBody::setPackDecl(std::shared_ptr<PackDecl> decl) {
    packDecl_ = decl;
}

// GlobalSpace
GlobalSpace::GlobalSpace(std::shared_ptr<IDecl> unit) :
    unit_(unit)
{ unit->setParent(this); }

void GlobalSpace::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    std::shared_ptr<IDecl> requester) 
{
    if (it == end) return;

    bool insert = false;

    if (std::distance(it, end) == 1 && *it == unit_->name()) {
        insert = true;
        res.emplace_back();
        res.back().push_back(unit_);
    } else if (*it == unit_->name()) {
        unit_->reachable_(
            res, std::next(it), end, requester);
    }

    for (auto decl : imports_) {
        if (decl->name() == *it) {
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
    }
}

void GlobalSpace::addImport(std::shared_ptr<IDecl> decl) {
    imports_.push_back(decl);
}

const std::vector<std::shared_ptr<IDecl>>& 
GlobalSpace::imports() const noexcept
{ return imports_; }

const std::string& GlobalSpace::name() const noexcept {
    static const std::string name__ = "global";
    return name__; 
}

std::shared_ptr<IDecl> 
GlobalSpace::unit() {
    return unit_;
}

// Use 
Use::Use(attribute::QualifiedName name) :
    name_(std::move(name))
{}

const attribute::QualifiedName& 
Use::name() const noexcept {
    return name_;
}

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
    , isInherits_(!base_.empty())
{ 
    decls_->setParent(this);
}

const std::string& RecordDecl::name() const noexcept {
    return name_;
}

bool RecordDecl::compare(const std::shared_ptr<IType> rhs) const {
    if (rhs.get() == this) {
        return true;
    }
    return false;
}

std::shared_ptr<DeclArea> RecordDecl::decls() {
    return decls_;
}

void RecordDecl::setBase(std::shared_ptr<RecordDecl> base) {
    baseRecord_ = base;
    base_ = base->name();
}

std::weak_ptr<RecordDecl> RecordDecl::base() {
    return baseRecord_;
}

const attribute::QualifiedName& 
RecordDecl::baseName() const noexcept {
    return base_;
}

bool RecordDecl::isInherits() const noexcept {
    return isInherits_;
}

void RecordDecl::setTagged() noexcept {
    isTagged_ = true;
}

bool RecordDecl::isTagged() const noexcept {
    return isTagged_;
}

std::weak_ptr<ClassDecl> RecordDecl::cls() {
    return class_;
}

void RecordDecl::setClass(std::shared_ptr<ClassDecl> cls) {
    class_ = cls;    
}

void RecordDecl::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    std::shared_ptr<IDecl> requester)
{   
    bool insert = false;
    for (auto decl : *decls_) {
        if (decl->name() == *it) {
            if (std::distance(it, end) == 1) {
                if (!insert) {
                    res.emplace_back();
                    insert = true;
                }
                res.back().push_back(decl);
            } else if (!std::dynamic_pointer_cast<ProcBody>(decl)) {
                decl->reachable_(
                    res, std::next(it), end, requester);
            }   
        }
        if (decl == requester) {
            break;
        }
    }
    if (auto base = baseRecord_.lock()) {
        base->reachable_(res, it, end, requester);
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

bool TypeAliasDecl::compare(const std::shared_ptr<IType> rhs) const {
    auto cur = std::dynamic_pointer_cast<TypeAliasDecl>(origin_);
    auto next = origin_;
    while (cur) {
        next = cur->origin_;
        cur = std::dynamic_pointer_cast<TypeAliasDecl>(next);
    }
    return rhs->compare(next);
}

std::shared_ptr<IType> 
TypeAliasDecl::origin() {
    return origin_;
}

void TypeAliasDecl::resetOrigin(
    std::shared_ptr<IType> newOrigin)
{
    origin_ = newOrigin;
}

void TypeAliasDecl::reachable_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    std::shared_ptr<IDecl> requester) 
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

bool SimpleLiteralType::compare(const std::shared_ptr<IType> rhs) const {
    if (auto lit = std::dynamic_pointer_cast<SimpleLiteralType>(rhs)) {
        return lit->type_ == type_;
    }
    return false;
}

// ArrayType
ArrayType::ArrayType(const std::vector<std::pair<int, int>>& ranges, 
                     std::shared_ptr<IType> type) :
    ranges_(ranges)
    , type_(type)
{
    type_->setParent(this);
}

bool ArrayType::compare(const std::shared_ptr<IType> rhs) const {
    if (auto arr = std::dynamic_pointer_cast<ArrayType>(rhs)) {
        return arr->type_->compare(type_) && ranges_ == arr->ranges_;
    }
    return false;
}


std::shared_ptr<IType> ArrayType::type() {
    return type_;
}

void ArrayType::resetType(std::shared_ptr<IType> newType) {
    type_ = newType;
}

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

bool StringType::compare(const std::shared_ptr<IType> rhs) const {
    if (auto str = std::dynamic_pointer_cast<StringType>(rhs)) {
        return range_ == str->range_;
    }
    return false;
}

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
    if (lhs) {
        lhs_->setParent(this);
    }
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
    , hasName_(true)
{}

TypeName::TypeName(attribute::Attribute attr) :
    attr_(std::move(attr))
{}

bool TypeName::compare(const std::shared_ptr<IType> rhs) const {
    assert(false);
}


const attribute::QualifiedName& 
TypeName::name() const noexcept {
    return name_;
}

const attribute::Attribute& 
TypeName::attribute() const noexcept {
    return attr_;
}

bool TypeName::hasName() const noexcept {
    return hasName_;
}

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
    if (retVal) {
        retVal_->setParent(this);
    }
}

} // namespace node

namespace node {

// ClassDecl
ClassDecl::ClassDecl(std::shared_ptr<RecordDecl> record) :
    record_(record),
    name_(record->name() + "Class")
{}

const std::string& ClassDecl::name() const noexcept {
    return name_;
}

void ClassDecl::setBase(std::weak_ptr<ClassDecl> base) {
    base_ = base;
}

bool ClassDecl::isDerivedOf(std::shared_ptr<ClassDecl> cls) {
    std::weak_ptr<ClassDecl> cur = std::dynamic_pointer_cast<ClassDecl>(self());
    while (!cur.expired()) {
        auto lock = cur.lock();
        if (lock == cls) {
            return true;
        }
        cur = lock->base_;
    }
    return false;
}

void ClassDecl::addMethod(std::shared_ptr<ProcBody> method) {
    if (auto func = std::dynamic_pointer_cast<FuncBody>(method)) {
        funcs_.emplace_back(func);
        return;
    }
    procs_.push_back(method);
}

std::shared_ptr<ProcBody> ClassDecl::containsMethod(
    const std::string& name, 
    const std::vector<std::shared_ptr<IType>>& params,
    bool proc)
{   
    auto eqParams = [&params] (auto proc) { 
        auto&& procParams = proc->params();
        if (procParams.size() != params.size()) {
            return false;
        }
        for (std::size_t i = 0; i < params.size(); ++i) {
            auto type = procParams[i]->type();
            if (!type->compare(params[i])) {
                return false;
            }
        }
        return true;
    };
    if (proc) {
        for (auto p : procs_) {
            auto lock = p.lock();
            if (lock->name() == name && eqParams(lock)) {
                return lock;
            }
        }
    } else {
        for (auto f : funcs_) {
            auto lock = f.lock();
            if (lock->name() == name && eqParams(lock)) {
                return lock;
            }
        }
    }
    if (base_.expired()) {
        return nullptr;
    } else {
        auto lock = base_.lock();
        return lock->containsMethod(name, params, proc);
    }
}

// SuperclassReference
SuperclassReference::SuperclassReference(
    const attribute::Attribute& ref) :
    ref_(ref)
{}

const attribute::Attribute& 
SuperclassReference::ref() const noexcept {
    return ref_;
}

const std::shared_ptr<ClassDecl>& 
SuperclassReference::cls() const noexcept {
    return class_;
}

void SuperclassReference::setClass(
    std::shared_ptr<ClassDecl> cls) 
{
    class_ = cls;
}

bool SuperclassReference::compare(
    const std::shared_ptr<IType> rhs) const 
{
    if (auto r = std::dynamic_pointer_cast<SuperclassReference>(rhs)) {
        return class_ && r->class_ && class_ == r->class_;
    }
}

}