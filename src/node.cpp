#include "node.hpp"

#include <algorithm>

#include "ada_codegen.hpp"

static auto getOrigin(std::shared_ptr<node::IType> type) {
    auto alias = std::dynamic_pointer_cast<node::TypeAliasDecl>(type);
    if (alias) {
        return alias->origin();
    }
    return type;
}

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

void IExpr::setInBrackets() {
    inBrackets_ = true;
}

bool IExpr::inBrackets() const noexcept {
    return inBrackets_;
}

void IExpr::setVarDecl(VarDecl* var) noexcept {
    varDecl_ = var;
}

VarDecl* IExpr::varDecl() noexcept {
    return varDecl_;
}

} 

// Stms
namespace node {

Body::Body(const std::vector<std::shared_ptr<IStm>>& stms) :
    stms_(stms)
{}

std::vector<std::shared_ptr<IStm>>::iterator Body::begin() {
    return stms_.begin();
}

std::vector<std::shared_ptr<IStm>>::iterator Body::end() {
    return stms_.end();
}

void Body::setParent(INode* parent)  {
    INode::setParent(parent);
    for (auto& stm : stms_) {
        stm->setParent(parent);
    }
}

// codegen
bb::BasicBlock* Body::codegen(
    class_member::SharedPtrMethod method, 
    bb::BasicBlock* bb) 
{
    for (auto&& stm : stms_) {
        bb = stm->codegen(bb, method);
    }
    return bb;
}

} // namespace node 

// Decls 
namespace node {

// IDecl 

std::vector<
    std::vector<std::shared_ptr<IDecl>>>  
IDecl::reachable(
    const attribute::QualifiedName& name, 
    IDecl* requester)
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
        auto buf = dynamic_cast<IDecl*>(parent_)->reachable(name, this);
        res.insert(res.end(), buf.begin(), buf.end());
    }

    return res;
}

// IType
descriptor::JVMFieldDescriptor 
IType::descriptor(bool out) {
    auto desc = 
    descriptor::JVMFieldDescriptor::createFundamental(
        codegen::java_bytecode_codegen::FundamentalType::BOOLEAN);
    return desc;
}

// DeclArea
void DeclArea::addDecl(std::shared_ptr<IDecl> decl) {
    decls_.push_back(decl);
    decl->setParent(parent_);
}

void DeclArea::removeDecl(std::shared_ptr<IDecl> decl) {
    auto it = std::find(decls_.begin(), decls_.end(), decl);
    if (it != decls_.end()) {
        decls_.erase(it);
    }
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
        IDecl* requester)
{ return; }

const std::string& VarDecl::name() const noexcept {
    return name_;
}

std::shared_ptr<IType> VarDecl::type() {
    auto type = getOrigin(type_);
    return type;
}

void VarDecl::resetType(std::shared_ptr<IType> type) {
    type_ = getOrigin(type);
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

std::shared_ptr<node::IExpr> VarDecl::rval() {
    return rval_;
}

void VarDecl::setRval(std::shared_ptr<node::IExpr> expr) {
    rval_ = expr;
}

// codegen
void VarDecl::pregen(
    jvm_class::SharedPtrJVMClass cls, 
    class_member::SharedPtrMethod method,
    bool isStatic)
{
    if (cls) {
        javaField_ = cls->addField(name_, type_->descriptor());
        isStatic_ = isStatic;
        if (isStatic) {
            javaField_->addFlag(
                codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
        }
    } else {
        if (std::dynamic_pointer_cast<RecordDecl>(type_) || 
            std::dynamic_pointer_cast<StringType>(type_) || 
            std::dynamic_pointer_cast<ArrayType>(type_)) 
        {
            method->createLocalRef(name_);
        } 
        else if (auto p = std::dynamic_pointer_cast<SimpleLiteralType>(type_))
        {
            switch (p->type()) {
                case SimpleType::BOOL: case SimpleType::CHAR: case SimpleType::INTEGER:
                    method->createLocalInt(name_); 
                    break;
                case SimpleType::FLOAT:
                    method->createLocalFloat(name_);
                    break;
            }
        } 
        else 
        {
            assert(false);
        }
    }
}


static void cgCreateArray(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    std::shared_ptr<ArrayType> arr)
{
    for (auto [l, r] : arr->ranges()) {
        method->createLdc(bb, l - r + 1);
    }
    auto arrDesc = arr->descriptor();
    auto dem = static_cast<std::uint8_t>(arr->ranges().size());
    method->createMultianewarray(bb, arrDesc, dem);
    method->createDup(bb);
    method->createInvokestatic(bb, codegen::AdaUtilityInitArrayElements);
}

void VarDecl::codegen(
        bb::BasicBlock* bb,
        class_member::SharedPtrMethod method)  
 {
    auto rec = std::dynamic_pointer_cast<node::RecordDecl>(type_);
    auto arr = std::dynamic_pointer_cast<node::ArrayType>(type_);
    auto str = std::dynamic_pointer_cast<node::StringType>(type_);
    // если есть инициализация
    if (rval_) {
        bb = rval_->codegen(bb, method);
        nextBB_ = bb;
        if (rec) {
            method->createInvokestatic(bb, codegen::AdaUtilityDeepCopy);
        } else if (arr) {
            if (auto arrTy = std::dynamic_pointer_cast<node::ArrayType>(rval_->type())) {
                // дублирование массива
                method->createInvokestatic(bb, codegen::AdaUtilityDeepCopyArray);
                createStore(bb, method);
            } else if (auto aggrTy = std::dynamic_pointer_cast<AggregateType>(rval_->type())) {
                // работа с агрегатом
                cgCreateArray(bb, method, arr);
                createStore(bb, method);

                for (int i = aggrTy->size() - 1; i >= 0; --i) {
                    createLoad(bb, method);
                    method->createLdc(bb, i);
                    method->createDup2X1(bb);
                    method->createPop(bb);
                    method->createPop(bb);
                    switch (aggrTy->type()) {
                        case SimpleType::BOOL:
                            method->createBastore(bb);
                            break;
                        case SimpleType::CHAR:
                            method->createCastore(bb);
                            break;
                        case SimpleType::FLOAT:
                            method->createFastore(bb);
                            break;
                        case SimpleType::INTEGER:
                            method->createIastore(bb);
                            break;
                    } 
                } 
            } 
        } else if (str) {
            if (std::dynamic_pointer_cast<StringLiteral>(rval_)) {
                method->createInvokestatic(bb, codegen::AdaUtilityFromStringLiteral);
            } else {
                method->createInvokestatic(bb, codegen::AdaUtilityCopyStringBuilder);
            }
        } else {
            createLoad(bb, method);
        }
    // если нет
    } else {
        if (rec) {
            method->createNew(bb, rec->javaClass());
            method->createInvokespecial(bb, rec->init());
            createStore(bb, method);
        } else if (arr) {
            cgCreateArray(bb, method, arr);
            createStore(bb, method);
        } else if (str) {
            method->createNew(bb, codegen::StringBuiler);
            createStore(bb, method);
        }
    }
}

void VarDecl::createLoad(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    if (javaField_ && isStatic_) {
        method->createGetstatic(bb, javaField_);
    } else if (javaField_) {
        method->createGetfield(bb, javaField_);
    } else {
        auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(type_);
        if (sTy && !param_ && !out_) {
            switch (sTy->type()) {
                case SimpleType::BOOL: case SimpleType::CHAR: case SimpleType::INTEGER:
                    method->createIload(bb, name_);
                    break;
                case (SimpleType::FLOAT):
                    method->createFload(bb, name_);
                    break;
            }
        } else {
            method->createAload(bb, name_);
        }
    }
}
    
void VarDecl::createStore(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    if (javaField_ && isStatic_) {
        method->createPutstatic(bb, javaField_);
    } else if (javaField_) {
        method->createPutfield(bb, javaField_);
    } else {
        auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(type_);
        if (sTy && !param_ && !out_) {
            switch (sTy->type()) {
                case SimpleType::BOOL: case SimpleType::CHAR: case SimpleType::INTEGER:
                    method->createIstore(bb, name_);
                    break;
                case (SimpleType::FLOAT):
                    method->createFstore(bb, name_);
                    break;
            }
        } else {
            method->createAstore(bb, name_);
        }
    }
}

// codegen
const std::vector<std::string> atomicRefs({
    "AtomicRef1",
    "AtomicRef2",
    "AtomicRef3",
    "AtomicRef4",
    "AtomicRef5",
    "AtomicRef6",
    "AtomicRef7",
    "AtomicRef8",
    "AtomicRef9",
    "AtomicRef10",
});

int curRef = 0;
int curRefForLoad = 0;

void VarDecl::createRef(bb::BasicBlock* bb, 
                class_member::SharedPtrMethod method)
{
    auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(type_);
    assert(sTy);

    auto&& ref = atomicRefs[curRef++];
    curRefForLoad = 0;

    assert(curRef >= 0);
    if (curRef >= atomicRefs.size()) {
        throw std::runtime_error("ref params > 10");
    }

    createLoad(bb, method);
    switch (sTy->type()) {
        case SimpleType::BOOL:
            method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicBoolean);
            break;
        case SimpleType::CHAR:
            method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicChar);
            break;
        case SimpleType::FLOAT:
            method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicFloat);
            break;
        case SimpleType::INTEGER:
            method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicInt);
            break;
    }

    method->createAstore(bb, ref);
}

void VarDecl::loadFromRef(bb::BasicBlock* bb,
                    class_member::SharedPtrMethod method)
{
    auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(type_);
    assert(sTy);

    auto&& ref = atomicRefs[curRefForLoad++];
    curRef = 0;

    method->createAload(bb, ref);
    switch (sTy->type()) {
        case SimpleType::BOOL:
            method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicBoolean);
            break;
        case SimpleType::CHAR:
            method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicChar);
            break;
        case SimpleType::FLOAT:
            method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicFloat);
            break;
        case SimpleType::INTEGER:
            method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicInt);
            break;
    }  

    createStore(bb, method);
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

std::shared_ptr<node::Body> ProcBody::body() {
    return body_;
}

void ProcBody::reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        IDecl* requester) 
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
        if (decl.get() == requester) {
            break;
        }
    }
}

// codegen
void ProcBody::createCall(
    bb::BasicBlock* bb,
    class_member::SharedPtrMethod method) 
{
    if (isStatic_) {
        method->createInvokestatic(bb, javaMethod_);
    } else {
        method->createInvokevirtual(bb, javaMethod_);
    }
}

descriptor::JVMMethodDescriptor ProcBody::desc() {
    using namespace descriptor;

    assert(!(dynamic_cast<node::ProcDecl*>(this) || 
             dynamic_cast<node::FuncDecl*>(this)));

    std::vector<std::pair<std::string, JVMFieldDescriptor>> paramsDescr;
    auto it = params_.begin(); 
    if (!cls_.expired()) ++it;
    for (; it != params_.end(); ++it) {
        auto var = *it;
        paramsDescr.emplace_back(
            var->name(), var->type()->descriptor(var->out() && var->param()));
    } 

    if (paramsDescr.empty()) {
        return JVMMethodDescriptor::createVoidParamsVoidReturn();
    } else {
        return JVMMethodDescriptor::createVoidRetun(paramsDescr);
    }
}

void ProcBody::pregen(
    jvm_class::SharedPtrJVMClass cls, 
    class_member::SharedPtrMethod method,
    bool isStatic) 
{
    if (dynamic_cast<node::ProcDecl*>(this) || 
        dynamic_cast<node::FuncDecl*>(this)) 
    { return; }

    auto d = desc();
    if (cls) {
        javaMethod_ = cls->addMethod(name_, d);
        isStatic_ = false;
    } else {
        javaMethod_ = codegen::InnerSubprograms->addMethod(name_, d);
        javaMethod_->addFlag(
            codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
        isStatic_ = true;
    }
    javaMethod_->addFlag(
        codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    
    for (auto&& r : atomicRefs) {
        javaMethod_->createLocalRef(r);
    }

    for (auto&& d : *decls_) {
        d->pregen(nullptr, javaMethod_);
    }
}

void ProcBody::codegen(
    bb::BasicBlock* bb,
    class_member::SharedPtrMethod method)  
{
    auto* _ = body_->codegen(javaMethod_, method->createBB());
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

// codegen
void ProcDecl::createCall(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method) 
{
    body_.lock()->createCall(bb, method);
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
    auto type = getOrigin(retType_);
    return type;
}

void FuncBody::resetRetType(std::shared_ptr<IType> type) {
    retType_ = getOrigin(type);
}

descriptor::JVMMethodDescriptor FuncBody::desc() {
    using namespace descriptor;
    
    assert(!(dynamic_cast<node::ProcDecl*>(this) || 
             dynamic_cast<node::FuncDecl*>(this)));

    std::vector<std::pair<std::string, JVMFieldDescriptor>> paramsDescr;
    auto it = params_.begin(); 
    if (!cls_.expired()) ++it;
    for (; it != params_.end(); ++it) {
        auto var = *it;
        paramsDescr.emplace_back(
            var->name(), var->type()->descriptor(var->out() && var->param()));
    } 

    auto retDesc = retType_->descriptor();
    if (paramsDescr.empty()) {
        return JVMMethodDescriptor::createVoidParams(retDesc);
    } else {
        return JVMMethodDescriptor::create(paramsDescr, retDesc);
    }
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

// codegen
void FuncDecl::createCall(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method) 
{
    assert(!body_.expired());
    body_.lock()->createCall(bb, method);
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
        IDecl* requester) 
{
    if (it == end) return;

    bool insert = false;
    for (auto decl : *decls_) {
        if (decl->name() == *it /* || (requester && requester->parent() == this && *it == name_) */) {
            // if (requester && requester->parent() == this && *it == name_) {
            //     ++it;
            // }
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
        if (decl.get() == requester) {
            break;
        }
    }
}

void PackDecl::reachableForPackBody_(
    std::vector<
        std::vector<std::shared_ptr<IDecl>>>& res,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator end,
    IDecl* requester) 
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
        if (decl.get() == requester) {
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

// codegen 
void PackDecl::pregen(
    jvm_class::SharedPtrJVMClass cls, 
    class_member::SharedPtrMethod method,
    bool isStatic) 
{
    if (dynamic_cast<PackBody*>(this)) return;

    javaClass_ = codegen::cg.createClass(fullName_.toString('_'));
    javaClass_->setParent(codegen::JavaObject);
    javaClass_->addAccesFlag(codegen::AccessFlag::ACC_PUBLIC);

    for (auto&& var : *decls_) {
        var->pregen(javaClass_, nullptr, true);
    }

    if (auto body = packBody_.lock()) {
        for (auto&& var : *(body->decls())) {
            var->pregen(javaClass_, nullptr, true);
        }
    }

    auto initDesc = 
        descriptor::JVMMethodDescriptor::createVoidParamsVoidReturn();
    clinit_ = javaClass_->addMethod("<clinit>", initDesc);
}

void PackDecl::codegen(
    bb::BasicBlock* bb,
    class_member::SharedPtrMethod method) 
{
    if (dynamic_cast<PackBody*>(this)) return;

    auto* clinitBB = clinit_->createBB();
    for (auto&& d : *decls_) {
        d->codegen(clinitBB);
        if (auto var = std::dynamic_pointer_cast<VarDecl>(d)) {
            clinitBB = var->nextBB();
        }
    }

    if (auto body = packBody_.lock()) {
        for (auto&& d : *(body->decls())) {
            d->codegen(clinitBB);
            if (auto var = std::dynamic_pointer_cast<VarDecl>(d)) {
                clinitBB = var->nextBB();
            }
        }
    }
}

void PackDecl::printClass() {
    if (dynamic_cast<PackBody*>(this)) return;
    codegen::cg.printClass(javaClass_);
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
    IDecl* requester) 
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
    IDecl* requester) 
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
        if (decl.get() == requester) {
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
    IDecl* requester) 
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
    auto orig = getOrigin(rhs);
    if (orig.get() == this) {
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
    IDecl* requester)
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
        if (decl.get() == requester) {
            break;
        }
    }
    if (auto base = baseRecord_.lock()) {
        base->reachable_(res, it, end, requester);
    }
}

descriptor::JVMFieldDescriptor 
RecordDecl::descriptor(bool out) {
    auto desc = 
        descriptor::JVMFieldDescriptor::createObject(fullName_.toString('_'));
    return desc;
}

void RecordDecl::pregen(
    jvm_class::SharedPtrJVMClass cls, 
    class_member::SharedPtrMethod method,
    bool isStatic) 
{
    createJavaClass_();
    if (deriveRecord_) {
        deriveRecord_->setJavaClassParrent(javaClass_);
    }

    if (auto cls = class_.lock()) {
        for (auto&& p : cls->procs()) {
            p.lock()->pregen(javaClass_);
        }

        for (auto&& f : cls->funcs()) {
            f.lock()->pregen(javaClass_);
        }
    }

    for (auto&& var : *decls_) {
        var->pregen(javaClass_);
    }

    auto initDesc = 
        descriptor::JVMMethodDescriptor::createVoidParamsVoidReturn();
    init_ = javaClass_->addMethod("<init>", initDesc);
}

void RecordDecl::codegen(
    bb::BasicBlock* bb,
    class_member::SharedPtrMethod method) 
{
    auto* initBB = init_->createBB();
    for (auto&& var : *decls_) {
        var->codegen(initBB);
        auto v = std::dynamic_pointer_cast<VarDecl>(var);
        initBB = v->nextBB();
    }
}

void RecordDecl::printClass() {
    codegen::cg.printClass(javaClass_);
}

void RecordDecl::setJavaClassParrent(
    jvm_class::SharedPtrJVMClass parent) 
{
    createJavaClass_();
    javaClass_->setParent(parent);   
}

void RecordDecl::createJavaClass_() {
    if (javaClass_) return;
    javaClass_ = codegen::cg.createClass(fullName_.toString('_'));
    javaClass_->setParent(codegen::JavaObject);
    javaClass_->addAccesFlag(codegen::AccessFlag::ACC_PUBLIC);
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
    IDecl* requester) 
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
    auto orig = getOrigin(rhs);
    if (auto lit = std::dynamic_pointer_cast<SimpleLiteralType>(orig)) {
        return lit->type_ == type_;
    }
    return false;
}
// codegen
descriptor::JVMFieldDescriptor 
SimpleLiteralType::descriptor(bool out) {
    using namespace codegen;
    using namespace descriptor;
    if (!out) {
        switch (type_) {
            case SimpleType::BOOL: 
                return JVMFieldDescriptor::createFundamental(FundamentalType::BOOLEAN);
            case SimpleType::INTEGER: 
                return JVMFieldDescriptor::createFundamental(FundamentalType::INT);
            case SimpleType::FLOAT: 
                return JVMFieldDescriptor::createFundamental(FundamentalType::FLOAT);
            case SimpleType::CHAR: 
                return JVMFieldDescriptor::createFundamental(FundamentalType::CHAR);
        }
    } else {
        switch (type_) {
            case SimpleType::BOOL: 
                return JVMFieldDescriptor::createObject({"java/util/concurrent/atomic/AtomicBoolean"});
            case SimpleType::INTEGER: 
                return JVMFieldDescriptor::createObject({"java/util/concurrent/atomic/AtomicInteger"});
            case SimpleType::FLOAT: 
                return JVMFieldDescriptor::createObject({"java/util/concurrent/atomic/AtomicReference"}); 
            case SimpleType::CHAR: 
                return JVMFieldDescriptor::createObject({"java/util/concurrent/atomic/AtomicInteger"}); //TODO: mb err here
        }
    }

    assert(false);
}

// AggregateType
AggregateType::AggregateType(std::vector<SimpleType> type) :
    type_(std::move(type))
{}

bool AggregateType::compare(const std::shared_ptr<IType> rhs) const {
    auto orig = getOrigin(rhs);
    if (auto aggr = std::dynamic_pointer_cast<AggregateType>(orig)) {
        auto rtype = aggr->type_;
        if (type_.size() == rtype.size()) {
            for (std::size_t i = 0; i < type_.size(); ++i) {
                if (type_[i] != rtype[i]) {
                    return false;
                }
            }
            return true;
        }
    } else if (auto arrTy = std::dynamic_pointer_cast<ArrayType>(orig)) {
        auto&& range = arrTy->ranges();
        if (range.size() == 1) {
            auto&& r = range[0];
            if (r.second - r.first + 1 == type_.size()) {
                auto elTy = getOrigin(arrTy->type());
                for (auto&& s : type_) {
                    auto agrElType = std::make_shared<node::SimpleLiteralType>(s);
                    if (!agrElType->compare(elTy)) {
                        return false;
                    }
                }
                return true;
            }
        }
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
    auto orig = getOrigin(rhs);
    if (auto arr = std::dynamic_pointer_cast<ArrayType>(orig)) {
        return getOrigin(arr->type_)->compare(getOrigin(type_)) && ranges_ == arr->ranges_;
    }
    return false;
}

std::shared_ptr<IType> ArrayType::type() {
    auto type = getOrigin(type_);
    return type;
}

void ArrayType::resetType(std::shared_ptr<IType> newType) {
    type_ = getOrigin(newType);
}

descriptor::JVMFieldDescriptor 
ArrayType::descriptor(bool out) {
    auto desc = type_->descriptor();
    for (auto&& _ : ranges_) {
        desc.addDimension();
    }
    return desc;
}

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

bool StringType::compare(const std::shared_ptr<IType> rhs) const {
    auto orig = getOrigin(rhs);
    if (auto str = std::dynamic_pointer_cast<StringType>(orig)) {
        return range_ == str->range_ || inf_;
    }
    return false;
}

void StringType::setInf() noexcept {
    inf_ = true;
}

std::pair<int, int> StringType::range() const {
    return range_;
}

descriptor::JVMFieldDescriptor StringType::descriptor(bool out) {
    static attribute::QualifiedName builder;
    if (builder.size() == 0) {
        builder.push("java");
        builder.push("lang");
        builder.push("StringBuilder");
    }
    auto desc = descriptor::JVMFieldDescriptor::createObject(builder);
    return desc;
}

// Aggregate
Aggregate::Aggregate(const std::vector<std::shared_ptr<ILiteral>>& inits) :
    inits_(inits)
{   
    std::vector<SimpleType> types;
    for (auto i : inits_) {
        if (auto litTy = std::dynamic_pointer_cast<SimpleLiteral>(i)) {
            types.push_back(std::dynamic_pointer_cast<SimpleLiteralType>(litTy->type())->type());
        } else {
            throw std::logic_error(
                    "In this implementation, aggregate" 
                    " initialization by an aggregate" 
                    " consisting only of primitive" 
                    " type literals is available.");        
        }
        i->setParent(this);
    }
    type_ = std::make_shared<AggregateType>(std::move(types));
}

bool Aggregate::compareTypes(const std::shared_ptr<IType> rhs) {
    return type_->compare(rhs);
}

std::shared_ptr<IType> Aggregate::type() {
    auto type = getOrigin(type_);
    return type;
}

bb::BasicBlock* Aggregate::codegen(
        bb::BasicBlock* bb, 
        class_member::SharedPtrMethod method, 
        bool lhs,
        int callStage) 
{
    for (auto&& e : inits_) {
        bb = e->codegen(bb, method, lhs, callStage);
    }
    return bb;
}

} // namespace node 

// Exprs
namespace node {

// Op
Op::Op(std::shared_ptr<IExpr> lhs, 
       OpType opType, 
       std::shared_ptr<IExpr> rhs) :
    lhs_(lhs)
    , opType_(opType)
    , rhs_(rhs)
{}

void Op::setParent(INode* parent) {
    INode::setParent(parent);
    if (lhs_) {
        lhs_->setParent(parent);
    }
    if (rhs_) {
        rhs_->setParent(parent);
    }
}

std::shared_ptr<IExpr> Op::left() { 
    return lhs_; 
}

std::shared_ptr<IExpr> Op::right() { 
    return rhs_; 
}

OpType Op::op() { 
    return opType_; 
}

void Op::setLeft(std::shared_ptr<IExpr> left) {
    lhs_ = left;
    lhs_->setParent(parent_);
}

void Op::setRight(std::shared_ptr<IExpr> right) {
    rhs_ = right;
    rhs_->setParent(parent_);
}

bool Op::compareTypes(const std::shared_ptr<IType> comp) {
    if (std::dynamic_pointer_cast<node::ArrayType>(rhs_->type())) {
        return false;
    }
    auto ty = type();
    if (ty) {
        if (auto sTy = 
            std::dynamic_pointer_cast<node::SimpleLiteralType>(comp))
        {
            if (sTy->type() == SimpleType::BOOL && 
                (opType_ == OpType::MORE || 
                 opType_ == OpType::LESS || 
                 opType_ == OpType::LTE || 
                 opType_ == OpType::GTE || 
                 opType_ == OpType::EQ || 
                 opType_ == OpType::NEQ)) 
            { return true; }
        }
        return ty->compare(comp);
    } else {
        return false;
    }
}

bb::BasicBlock* Op::codegen(
    bb::BasicBlock* bb,
    class_member::SharedPtrMethod method,
    bool lhs,
    int callStage)
{
    if (lhs_) {
        bb = lhs_->codegen(bb, method, lhs, callStage);
    }

    bb = rhs_->codegen(bb, method, lhs, callStage);

    assert(opType_ != OpType::DOT);

    static auto BOOL_TY    = std::make_shared<SimpleLiteralType>(SimpleType::BOOL);
    static auto CHAR_TY    = std::make_shared<SimpleLiteralType>(SimpleType::CHAR);
    static auto FLOAT_TY   = std::make_shared<SimpleLiteralType>(SimpleType::FLOAT);
    static auto INTEGER_TY = std::make_shared<SimpleLiteralType>(SimpleType::INTEGER);
    static auto STRING_TY  = std::make_shared<StringType>(std::make_pair(-1, -1));
    STRING_TY->setInf();

    auto makeBoolResult = [&](OpType op, bool isFloat)
    {
        auto trueBB  = method->createBB();
        auto falseBB = method->createBB();
        auto endBB = method->createBB();
        bb = endBB;

        if (isFloat) {
            method->createFcmpl(bb);
            switch (op) {
                case OpType::EQ:  method->createIfeq(bb, trueBB); break;
                case OpType::NEQ: method->createIfne(bb, trueBB); break;
                case OpType::MORE:method->createIfgt(bb, trueBB); break;
                case OpType::LESS:method->createIflt(bb, trueBB); break;
                case OpType::GTE: method->createIfge(bb, trueBB); break;
                case OpType::LTE: method->createIfle(bb, trueBB); break;
                default: break;
            }
        } else {
            switch (op) {
                case OpType::EQ:  method->createIficmpeq(bb, trueBB); break;
                case OpType::NEQ: method->createIficmpne(bb, trueBB); break;
                case OpType::MORE:method->createIficmpgt(bb, trueBB); break;
                case OpType::LESS:method->createIficmplt(bb, trueBB); break;
                case OpType::GTE: method->createIficmpge(bb, trueBB); break;
                case OpType::LTE: method->createIficmple(bb, trueBB); break;
                default: break;
            }
        }

        method->createGoto(bb, falseBB); // mb err

        method->createLdc(trueBB, 1);
        method->createGoto(trueBB, endBB);

        method->createLdc(falseBB, 0);
        method->createGoto(falseBB, endBB);
    };

    switch (opType_)
    {
        // ===== арифметика =====

        case OpType::PLUS:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createIadd(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFadd(bb);
            break;

        case OpType::MINUS:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createIsub(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFsub(bb);
            break;

        case OpType::MUL:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createImul(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFmul(bb);
            break;

        case OpType::DIV:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createIdiv(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFdiv(bb);
            break;

        case OpType::MOD:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createIrem(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFrem(bb);
            break;

        case OpType::UMINUS:
            if (INTEGER_TY->compare(type()) || CHAR_TY->compare(type()))
                method->createIneg(bb);
            else if (FLOAT_TY->compare(type()))
                method->createFneg(bb);
            break;

        // ===== логика =====

        case OpType::AND:
            method->createIand(bb);
            break;

        case OpType::OR:
            method->createIor(bb);
            break;

        case OpType::XOR:
            method->createIxor(bb);
            break;

        case OpType::NOT:
            //  x -> x ^ 1
            method->createLdc(bb, 1);
            method->createIxor(bb);
            break;

        // ===== сравнения =====

        case OpType::EQ:
        case OpType::NEQ:
        case OpType::MORE:
        case OpType::LESS:
        case OpType::GTE:
        case OpType::LTE:
        {
            bool isFloat = FLOAT_TY->compare(lhs_->type());
            makeBoolResult(opType_, isFloat);
            break;
        }
        case OpType::AMPER:
            assert(STRING_TY->compare(type()));
            method->createInvokestatic(bb, codegen::AdaUtilityConcat);

        default:
            assert(false && "Unsupported OpType");
    }

    return bb;
}

// >,<,=,!= только с float, bool, integer, char
// not and or xor только с bool 
std::shared_ptr<IType> Op::type() { 
    if (lhs_) {
        auto ltype = lhs_->type();
        auto rtype = rhs_->type();
        if (!ltype || !rtype) {
            return nullptr;
        }
        auto str1 = std::dynamic_pointer_cast<StringType>(ltype);
        auto str2 = std::dynamic_pointer_cast<StringType>(rtype);
        if (str1 && str2 && opType_ == OpType::AMPER) {
            auto [d1, lim1] = str1->range();
            auto [d2, lim2] = str2->range(); 
            return std::make_shared<StringType>(std::make_pair(1, lim1 + lim2));
        } else if (str1 && str2) {
            return nullptr;
        }

        auto lrec = std::dynamic_pointer_cast<RecordDecl>(ltype);
        auto rrec = std::dynamic_pointer_cast<RecordDecl>(rtype);
        if (lrec || rrec) {
            return nullptr;
        }

        auto larr = std::dynamic_pointer_cast<node::ArrayType>(ltype);
        auto rarr = std::dynamic_pointer_cast<node::AggregateType>(rtype);
        if (larr || rarr) {
            return nullptr;
        }

        if (ltype->compare(rtype)) {
            auto sTy = std::dynamic_pointer_cast<node::SimpleLiteralType>(rhs_->type());
            SimpleType s = sTy ? sTy->type() : SimpleType::CHAR;
            // только бул для этих операторов
            if (s != SimpleType::BOOL && ( 
                 opType_ == OpType::NOT || 
                 opType_ == OpType::XOR || 
                 opType_ == OpType::OR  || 
                 opType_ == OpType::AND))
            { return nullptr; }

            if (s != SimpleType::FLOAT || 
                s != SimpleType::INTEGER && 
                (opType_ == OpType::MORE ||
                 opType_ == OpType::LESS ||
                 opType_ == OpType::GTE ||
                 opType_ == OpType::LTE ||
                 opType_ == OpType::PLUS ||
                 opType_ == OpType::MINUS ||
                 opType_ == OpType::MUL ||
                 opType_ == OpType::DIV ||
                 opType_ == OpType::MOD ||
                 opType_ == OpType::UMINUS ))
            { return nullptr; }
            return rhs_->type();
        }
        
        return nullptr;
    } else {
        return rhs_->type();
    }
    return nullptr;
}

// DotOpExpr
void DotOpExpr::setLeft(std::shared_ptr<DotOpExpr> l) {
    left_ = l;
}

void DotOpExpr::setRight(std::shared_ptr<DotOpExpr> r) {
    // r->setLeft(std::dynamic_pointer_cast<node::DotOpExpr>(self())); 
    right_ = r;
}

void DotOpExpr::setTail(std::shared_ptr<DotOpExpr> tail) {
    if (!right_) {
        setRight(tail);
        return;
    }
    right_->setTail(tail);
}

std::shared_ptr<DotOpExpr> DotOpExpr::tail() {
    if (right_) {
        return right_->tail();
    }
    return std::dynamic_pointer_cast<DotOpExpr>(self());
}

std::shared_ptr<DotOpExpr> DotOpExpr::left() {
    return left_.lock();
}

std::shared_ptr<DotOpExpr> DotOpExpr::right() {
    return right_;
}

bool DotOpExpr::compareTypes(const std::shared_ptr<IType> rhs) {
    auto ty = type();
    if (ty) {
        return ty->compare(rhs);
    }
    return false;
}

// GetVarExpr
GetVarExpr::GetVarExpr(
    std::shared_ptr<VarDecl> var, 
    std::shared_ptr<VarDecl> recordInst):
    var_(var)
    , recordInst_(recordInst)
    , container_(std::dynamic_pointer_cast<RecordDecl>(var_->type()) || 
                 std::dynamic_pointer_cast<SuperclassReference>(var_->type()))
    , lhs_(var->out())
    , rhs_(var->in())
{}

bool GetVarExpr::lhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return lhs_;
}

bool GetVarExpr::rhs() {
    if (left_.expired() && right_) {  
        return tail()->rhs();
    }
    return rhs_;
}

bool GetVarExpr::container() {
    if (left_.expired() && right_) {  
        return tail()->container();
    }
    return container_;
}

std::shared_ptr<IType> GetVarExpr::type() {
    if (left_.expired() && right_) {
        return tail()->type();
    } 
    return var_->type();
}

std::shared_ptr<VarDecl> GetVarExpr::var() {
    return var_;
}

bb::BasicBlock* GetVarExpr::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs, 
    int callStage) 
{
    // если lhs и !right_ - лоад, иначе стор
    // если out и param и примитивный тип то - лоад через fromAtomic, стор через setAtomic

    auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(var_->type());
    const int PRE_CALL = 1;
    const int POST_CALL = 2; 
    if (PRE_CALL == callStage && !right_) {
        var_->createRef(bb, method);
        return bb;
    } else if (POST_CALL == callStage && !right_) {
        var_->loadFromRef(bb, method);
        return bb;
    }

    if (lhs && !right_) {
        if (sTy && var_->out() && var_->param()) {
            // ... rhs, lhs_atomic_ref -> ... lhs_atomic_ref, rhs 
            var_->createLoad(bb, method); 
            method->createSwap(bb); 
            switch (sTy->type()) {
                case SimpleType::BOOL:
                    method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicBoolean);
                    break;
                case SimpleType::CHAR:
                    method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicChar);
                    break;
                case SimpleType::FLOAT:
                    method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicFloat);
                    break;
                case SimpleType::INTEGER:
                    method->createInvokestatic(bb, codegen::AdaUtilitySetAtomicInt);
                    break;
            }
        } else {
            var_->createStore(bb, method);
        } 
    } else if (!right_) {
        if (sTy && var_->out() && var_->param()) {
            var_->createLoad(bb, method); 
            switch (sTy->type()) {
                case SimpleType::BOOL:
                    method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicBoolean);
                    break;
                case SimpleType::CHAR:
                    method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicChar);
                    break;
                case SimpleType::FLOAT:
                    method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicFloat);
                    break;
                case SimpleType::INTEGER:
                    method->createInvokestatic(bb, codegen::AdaUtilityFromAtomicInt);
                    break;
            }
        } else {
            var_->createLoad(bb, method);
        } 
    } else {
        var_->createLoad(bb, method);
        return right_->codegen(bb, method, lhs, callStage);
    }

    return bb;                         
} 

// PackNamePart
PackNamePart::PackNamePart(std::shared_ptr<PackDecl> pack) :
    pack_(pack)
{}

bool PackNamePart::lhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return false;
}

bool PackNamePart::rhs() {
    if (left_.expired() && right_) {  
        return tail()->rhs();
    }
    return false;
}

bool PackNamePart::container() {
    if (left_.expired() && right_) {  
        return tail()->container();
    }
    return true;
}

std::string PackNamePart::packName() const {
    return pack_->name();
}

std::shared_ptr<IType> PackNamePart::type() {
    if (left_.expired() && right_) {  
        return tail()->type();
    }
    assert(false);
    return nullptr;
}

bb::BasicBlock* PackNamePart::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs,
    int callStage)
{
    assert(right_);
    return right_->codegen(bb, method, lhs, callStage);
}

// GetArrElementExpr
GetArrElementExpr::GetArrElementExpr(
    std::shared_ptr<IDecl> owner, 
    std::shared_ptr<VarDecl> arr,
    const std::vector<std::shared_ptr<IExpr>>& idxs) :
    owner_(owner)
    , arr_(arr)
    , idxs_(idxs)
    , container_(
        std::dynamic_pointer_cast<RecordDecl>(arr_->type()) || 
        std::dynamic_pointer_cast<PackDecl>(arr_->type()))
    , lhs_(arr->out())
    , rhs_(arr->in())
{}

bool GetArrElementExpr::lhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return lhs_;
}

bool GetArrElementExpr::rhs() {
    if (left_.expired() && right_) {  
        return tail()->rhs();
    }
    return rhs_;
}

bool GetArrElementExpr::container() {
    if (left_.expired() && right_) {  
        return tail()->container();
    }
    return container_;
}

std::shared_ptr<IType> GetArrElementExpr::type() {
    if (left_.expired() && right_) {  
        return tail()->type();
    }
    auto type = getOrigin(arr_->type());
    if (auto ty = std::dynamic_pointer_cast<ArrayType>(type)) {
        return ty->type();
    } else if (auto str = std::dynamic_pointer_cast<StringType>(type)) {
        return std::make_shared<SimpleLiteralType>(SimpleType::CHAR);
    }
    assert(false);
}

std::shared_ptr<VarDecl> GetArrElementExpr::arr() {
    return arr_;
}

bb::BasicBlock* GetArrElementExpr::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs,
    int callStage) 
{
    if (auto arrTy = std::dynamic_pointer_cast<ArrayType>(arr_->type())) {
        // проходим по всем измерениям
        arr_->createLoad(bb, method);
        for (int i = 0; i < idxs_.size() - 1; ++i) {
            auto&& idx = idxs_[i];
            bb = idx->codegen(bb, method, false);
            method->createIconst(bb, 1);
            method->createIsub(bb);
            method->createAaload(bb);
        }
        // val, ref, idx -> ref, idx, var
        // получаем последний индекс
        bb = idxs_.back()->codegen(bb, method);
        method->createIconst(bb, 1);
        method->createIsub(bb);
        // загружаем или выгружаем
        auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(arrTy->type());
        if (lhs && !right_) {
            if (sTy) {
                method->createDup2X1(bb);
                method->createPop(bb);
                method->createPop(bb);
                switch (sTy->type()) {
                    case SimpleType::BOOL:
                        method->createBastore(bb);
                        break;
                    case SimpleType::CHAR:
                        method->createCastore(bb);
                        break;
                    case SimpleType::FLOAT:
                        method->createFastore(bb);
                        break;
                    case SimpleType::INTEGER:
                        method->createIastore(bb);
                        break;
                } 
            } else {
                method->createAastore(bb);
            }
        } else {
            if (sTy) {
                switch (sTy->type()) {
                    case SimpleType::BOOL:
                        method->createBaload(bb);
                        break;
                    case SimpleType::CHAR:
                        method->createCaload(bb);
                        break;
                    case SimpleType::FLOAT:
                        method->createFaload(bb);
                        break;
                    case SimpleType::INTEGER:
                        method->createIaload(bb);
                        break;
                } 
            } else {
                method->createAaload(bb);
            }
        }
    } else if (auto strTy = std::dynamic_pointer_cast<StringType>(arr_->type())) {
        arr_->createLoad(bb, method);
        bb = idxs_[0]->codegen(bb, method, false);
        method->createIconst(bb, 1);
        method->createIsub(bb);
        if (lhs && !right_) {
            method->createDup2X1(bb);
            method->createPop(bb);
            method->createPop(bb);
            method->createInvokestatic(bb, codegen::AdaUtilitySetCharAt);
        } else {
            method->createInvokestatic(bb, codegen::AdaUtilityCharAt);
        }
    }

    if (right_) {
        return right_->codegen(bb, method, lhs, callStage);
    }

    return bb;
}

// CallExpr
CallExpr::CallExpr(
    std::shared_ptr<IDecl> owner, 
    std::shared_ptr<ProcBody> proc,
    std::shared_ptr<FuncBody> func,
    const std::vector<std::shared_ptr<IExpr>>& params) :
    owner_(owner)
    , proc_(proc)
    , func_(func)
    , params_(params)
    , container_(func)
    , noValue_(!func_)
{}

bool CallExpr::lhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return false;
}

bool CallExpr::rhs() {
    if (left_.expired() && right_) {  
        return tail()->rhs();
    }
    return !noValue_;
}

bool CallExpr::container() {
    if (right_ && right_) {  
        return tail()->container();
    }
    return container_;
}

bool CallExpr::setNoValue() {
    if (func_ && !proc_) {
        return false;
    }
    container_ = false;
    noValue_ = true;
    return true;
}

const std::vector<std::shared_ptr<IExpr>>& 
CallExpr::params() const noexcept {
    return params_;
}

std::shared_ptr<ProcBody> CallExpr::proc() {
    return proc_;
}

std::shared_ptr<FuncBody> CallExpr::func() {
    return func_;
}

std::shared_ptr<IType> CallExpr::type() {
    if (right_) {
        return right_->type();
    } 
    if (!noValue_ && func_) {
        return func_->retType();
    }
    return nullptr;
}

bb::BasicBlock* CallExpr::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs, 
    int callStage)
{
    assert(!lhs || (lhs && right_));

    std::vector<std::shared_ptr<node::VarDecl>> params;
    if (noValue_) {
        params = proc_->params();
    } else {
        params = func_->params();
    }

    if (params_.size() != params.size()) {
        params.erase(params.begin());
    }
    // обертка в атомик
    for (int i = 0; i < params.size(); ++i) {
        auto&& p = params_[i];
        auto&& var = params[i];
        int callSt = var->out() ? -1 : 1;
        bb = p->codegen(bb, method, callSt);
    }

    if (noValue_) {
        proc_->createCall(bb, method);
    } else {
        func_->createCall(bb, method);
    }

    // загрузка из атомика
    for (int i = 0; i < params.size(); ++i) {
        auto&& p = params_[i];
        auto&& var = params[i];
        if (auto dotOp = std::dynamic_pointer_cast<node::DotOpExpr>(p)) {
            if (std::dynamic_pointer_cast<node::GetVarExpr>(dotOp->tail())) {
                int callSt = var->out() ? -1 : 2;
                bb = p->codegen(bb, method, callSt);
            }
        }
    }

    if (right_) {
        return right_->codegen(bb, method, lhs, callStage);
    }
    
    return bb;
} 

// CallMethodExpr
CallMethodExpr::CallMethodExpr(
    std::shared_ptr<ClassDecl> owner, 
    std::shared_ptr<ProcBody> proc,
    std::shared_ptr<FuncBody> func,
    const std::vector<std::shared_ptr<IExpr>>& params) :
    owner_(owner)
    , proc_(proc)
    , func_(func)
    , params_(params)
    , container_(func)
    , noValue_(!func_)
{}

bool CallMethodExpr::lhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return false;
}

bool CallMethodExpr::rhs() {
    if (left_.expired() && right_) {  
        return tail()->lhs();
    }
    return !noValue_;
}

bool CallMethodExpr::container() {
    if (right_ && right_) {  
        return tail()->container();
    }
    return container_;
}

bool CallMethodExpr::setNoValue() {
    if (func_ && !proc_) {
        return false;
    }
    container_ = false;
    noValue_ = true;
    return true;
}

const std::vector<std::shared_ptr<IExpr>>& 
CallMethodExpr::params() const noexcept {
    return params_;
}

std::shared_ptr<ProcBody> CallMethodExpr::proc() {
    return proc_;
}

std::shared_ptr<FuncBody> CallMethodExpr::func() {
    return func_;
}

std::shared_ptr<IType> CallMethodExpr::type() {
    if (right_) {
        return right_->type();
    } 
    if (!noValue_ && func_) {
        return func_->retType();
    }
    return nullptr;
}

bb::BasicBlock* CallMethodExpr::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs, 
    int callStage)
{
    assert(!lhs || (lhs && right_));

    std::vector<std::shared_ptr<node::VarDecl>> params;
    if (noValue_) {
        params = proc_->params();
    } else {
        params = func_->params();
    }

    if (params_.size() != params.size()) {
        params.erase(params.begin());
    }
    // обертка в атомик
    for (int i = 0; i < params.size(); ++i) {
        auto&& p = params_[i];
        auto&& var = params[i];
        int callSt = var->out() ? -1 : 1;
        bb = p->codegen(bb, method, callSt);
    }

    if (noValue_) {
        proc_->createCall(bb, method);
    } else {
        func_->createCall(bb, method);
    }

    // загрузка из атомика
    for (int i = 0; i < params.size(); ++i) {
        auto&& p = params_[i];
        auto&& var = params[i];
        if (auto dotOp = std::dynamic_pointer_cast<node::DotOpExpr>(p)) {
            if (std::dynamic_pointer_cast<node::GetVarExpr>(dotOp->tail())) {
                int callSt = var->out() ? -1 : 2;
                bb = p->codegen(bb, method, callSt);
            }
        }
    }

    if (right_) {
        return right_->codegen(bb, method, lhs, callStage);
    }
    
    return bb;
} 

// ImageCallExpr
ImageCallExpr::ImageCallExpr(        
    std::shared_ptr<IExpr> param, 
    std::shared_ptr<SimpleLiteralType> imageType
) :
    param_(param)
    , imageType_(imageType)
    , stringType_(std::make_shared<StringType>(std::make_pair(1, 1)))
{
    stringType_->setInf();
}

std::shared_ptr<IType> ImageCallExpr::type() {
    return stringType_;
}

std::shared_ptr<IExpr> ImageCallExpr::param() {
    return param_;
}

std::shared_ptr<SimpleLiteralType> ImageCallExpr::imageType() {
    return imageType_;
}

bool ImageCallExpr::compareTypes(
    const std::shared_ptr<IType> rhs) 
{
    return stringType_->compare(rhs);
}

bb::BasicBlock* ImageCallExpr::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method, 
    bool lhs,
    int callStage) 
{
    bb = param_->codegen(bb, method);
    switch (imageType_->type()) {
        case SimpleType::CHAR:
            method->createInvokestatic(bb, codegen::AdaUtilityImageFromChar);
            break;
        case SimpleType::BOOL:
            method->createInvokestatic(bb, codegen::AdaUtilityImageFromBool);
            break;
        case SimpleType::INTEGER:
            method->createInvokestatic(bb, codegen::AdaUtilityImageFromInt);
            break;
        case SimpleType::FLOAT:
            method->createInvokestatic(bb, codegen::AdaUtilityImageFromFloat);
            break;
    }
    return bb;
}

// NameExpr
NameExpr::NameExpr(const std::string& name) :
    name_(name)
{}

// AttributeExpr
AttributeExpr::AttributeExpr(const attribute::Attribute& attr) :
    attr_(attr)
{}

// CallOrIdxExpr 
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

// SimpleLiteral
bool SimpleLiteral::compareTypes(const std::shared_ptr<IType> rhs) {
    return type_->compare(rhs);
}

std::shared_ptr<IType> SimpleLiteral::type() { 
    return type_;
}

// codegen
bb::BasicBlock* SimpleLiteral::codegen(
        bb::BasicBlock* bb, 
        class_member::SharedPtrMethod method, 
        bool lhs,
        int callStage) 
{   
    switch (type_->type()) {
        case SimpleType::INTEGER: 
            method->createLdc(bb, get<int>());
            break;
        case SimpleType::BOOL: 
            method->createLdc(bb, get<bool>());
            break;
        case SimpleType::CHAR: 
            method->createLdc(bb, get<char>());
            break;
        case SimpleType::FLOAT: 
            method->createLdc(bb, get<float>());
            break;
    }
    return bb;
}

// StringLiteral
StringLiteral::StringLiteral(std::shared_ptr<StringType> type, 
                             const std::string& str) :
    str_(str)
    , type_(type)
{
    type_->setParent(this);
}

bool StringLiteral::compareTypes(const std::shared_ptr<IType> rhs) {
    return type_->compare(rhs);
}

std::shared_ptr<IType> StringLiteral::type() { 
    return type_;
}

// codegen
bb::BasicBlock* StringLiteral::codegen(
        bb::BasicBlock* bb, 
        class_member::SharedPtrMethod method, 
        bool lhs,
        int callStage) 
{
    method->createLdc(bb, str_);   
    return bb;
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

void If::setParent(INode* parent) {
    INode::setParent(parent);
    cond_->setParent(parent);
    body_->setParent(parent);
    if (els_) {
        els_->setParent(parent);
    }
    for (auto&& [cond, body] : elsifs_) {
        cond->setParent(parent);
        body->setParent(parent);
    } 
}

// codegen
bb::BasicBlock* If::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    bb = cond_->codegen(bb, method);
    
    auto* bodyBB = method->createBB();

    std::vector<bb::BasicBlock*> bodyNextBBs;
    bodyNextBBs.push_back(body_->codegen(method, bodyBB));

    auto* nextBB = method->createBB();
    method->createIfne(bb, bodyBB);
    method->createGoto(bb, nextBB);

    bb = nextBB;

    for (auto&& [c, b] : elsifs_) {
        bb = c->codegen(bb, method);
        auto* bodyBB = method->createBB();
        bodyNextBBs.push_back(b->codegen(method, bodyBB));
        auto* nextBB = method->createBB();
        method->createIfne(bb, bodyBB);
        method->createGoto(bb, nextBB);
        bb = nextBB;
    }

    bodyNextBBs.push_back(els_->codegen(method, bb));

    auto* endBB = method->createBB();

    for (auto* bb : bodyNextBBs) {
        method->createGoto(bb, endBB);
    }

    return endBB;
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

void For::setParent(INode* parent) {
    INode::setParent(parent);
    range_.first->setParent(parent);
    range_.second->setParent(parent);
    body_->setParent(parent);
}

bb::BasicBlock* For::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    iter_->pregen(nullptr, method);

    bb = range_.first->codegen(bb, method);
    iter_->createStore(bb, method);

    std::string right = "right12345";
    method->createLocalInt(right);
    bb = range_.second->codegen(bb, method);
    method->createIstore(bb, right);
    
    // создаем бб конда и тела
    auto* condBB = method->createBB();
    auto* bodyBB = method->createBB();

    // наполняем тело
    auto* bodyNext = body_->codegen(method, bodyBB);
    method->createIinc(bodyNext, iter_->name(), 1);
    method->createGoto(bodyNext, condBB);

    // создаем следующий бб, наполняем конд бб 
    auto nextBB = method->createBB();
    iter_->createLoad(condBB, method);
    method->createIload(condBB, right);
    // условие цикла
    method->createIficmple(condBB, bodyBB);
    method->createGoto(condBB, nextBB);

    return nextBB;
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

void While::setParent(INode* parent) {
    INode::setParent(parent);
    cond_->setParent(parent);
    body_->setParent(parent);
}

bb::BasicBlock* While::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    auto* condBB = method->createBB();
    cond_->codegen(condBB, method); 
    
    auto* bodyBB = method->createBB();
    auto bodyNextBB = body_->codegen(method, bodyBB);

    auto* nextBB = method->createBB();
    // заполняется cond
    method->createIfne(condBB, bodyBB);
    method->createGoto(condBB, nextBB);

    method->createGoto(bodyNextBB, condBB);

    return nextBB;
}

} // namespace node 

// Stms - Other
namespace node {

MBCall::MBCall(std::shared_ptr<IExpr> call) :
    call_(call)
{}

void MBCall::setParent(INode* parent) {
    INode::setParent(parent);
    call_->setParent(parent);
}

std::shared_ptr<IExpr> MBCall::call() {
    return call_;
}

void MBCall::setCall(std::shared_ptr<IExpr> expr) {
    call_ = expr;
}

// codegen
bb::BasicBlock* MBCall::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    call_->codegen(bb, method);
    return bb;
}

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
{}

void Assign::setParent(INode* parent) {
    INode::setParent(parent);
    lval_->setParent(parent);
    rval_->setParent(parent);
}

std::shared_ptr<IExpr> Assign::lval() {
    return lval_;
}

void Assign::setLval(std::shared_ptr<IExpr> lval) {
    lval_ = lval;
}

std::shared_ptr<IExpr> Assign::rval() {
    return rval_;
}

void Assign::setRval(std::shared_ptr<IExpr> rval) {
    rval_ = rval;
}

bb::BasicBlock* Assign::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{

    // дип копи: справа массив, строка, объект
    // спрвава агрегат, копирование агретага

    auto rec = std::dynamic_pointer_cast<RecordDecl>(rval_->type());
    auto arr = std::dynamic_pointer_cast<ArrayType>(rval_->type());
    auto str = std::dynamic_pointer_cast<StringType>(rval_->type());
    auto agr = std::dynamic_pointer_cast<AggregateType>(rval_->type());

    rval_->codegen(bb, method);
    if (rec) {
        method->createInvokestatic(bb, codegen::AdaUtilityDeepCopy);
    } else if (arr) {
        method->createInvokestatic(bb, codegen::AdaUtilityDeepCopyArray);
    } else if (str) {
        method->createInvokestatic(bb, codegen::AdaUtilityCopyStringBuilder);
    }

    if (!agr) {
        lval_->codegen(bb, method, true);
    } else {
         // работа с агрегатом
        lval_->codegen(bb, method, false);
        for (int i = agr->size() - 1; i >= 0; --i) {
            lval_->codegen(bb, method, false);
            method->createLdc(bb, i);
            method->createDup2X1(bb);
            method->createPop(bb);
            method->createPop(bb);
            switch (agr->type()) {
                case SimpleType::BOOL:
                    method->createBastore(bb);
                    break;
                case SimpleType::CHAR:
                    method->createCastore(bb);
                    break;
                case SimpleType::FLOAT:
                    method->createFastore(bb);
                    break;
                case SimpleType::INTEGER:
                    method->createIastore(bb);
                    break;
            } 
        } 
    }

    return bb;
}

// Return 
Return::Return(std::shared_ptr<IExpr> retVal) : 
    retVal_(retVal) 
{}

void Return::setParent(INode* parent) {
    INode::setParent(parent);
    if (retVal_) {
        retVal_->setParent(parent);
    }
}

bb::BasicBlock* Return::codegen(
    bb::BasicBlock* bb, 
    class_member::SharedPtrMethod method)
{
    retVal_->codegen(bb, method);
    auto rec = std::dynamic_pointer_cast<RecordDecl>(retVal_->type());
    auto arr = std::dynamic_pointer_cast<ArrayType>(retVal_->type());
    auto str = std::dynamic_pointer_cast<StringType>(retVal_->type());
    if (rec) {
        method->createInvokestatic(bb, codegen::AdaUtilityDeepCopy);
    } else if (arr) {
        method->createInvokestatic(bb, codegen::AdaUtilityDeepCopyArray);
    } else if (str) {
        method->createInvokestatic(bb, codegen::AdaUtilityCopyStringBuilder);
    }

    if (rec || arr || str) {
        method->createAreturn(bb);
    } else if (auto sTy = std::dynamic_pointer_cast<SimpleLiteralType>(retVal_->type())) {
        switch (sTy->type()) {
            case SimpleType::BOOL: case SimpleType::CHAR: case SimpleType::INTEGER:
                method->createIreturn(bb);
                break;
            case SimpleType::FLOAT:
                method->createFreturn(bb);
                break;
        }
    }

    return bb;
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
            auto rec1 = std::dynamic_pointer_cast<RecordDecl>(procParams[i]->type());
            auto cls1 = rec1 ? rec1->cls().lock() : nullptr;
            auto rec2 = std::dynamic_pointer_cast<RecordDecl>(params[i]);
            auto cls2 = rec2 ? rec2->cls().lock() : nullptr;
            if (cls1 && cls2 && !cls2->isDerivedOf(cls1)) {
                return false;
            } else if (cls1 && cls2) {
                continue;
            }
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

std::shared_ptr<ProcBody> ClassDecl::proc(const std::string& name) {
    auto it = std::find_if(procs_.begin(), procs_.end(), 
    [&name] (auto&& p) { return p.lock()->name() == name;} );
    
    return it == procs_.end() ? nullptr : it->lock();
}  

std::shared_ptr<FuncBody> ClassDecl::func(const std::string& name) {
    auto it = std::find_if(funcs_.begin(), funcs_.end(), 
    [&name] (auto&& p) { return p.lock()->name() == name;});
    
    return it == funcs_.end() ? nullptr : it->lock();
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
    auto cur = cls; 
    while (!cur->base().expired()) {
        cur = cur->base().lock();
    }
    class_ = cur;
}

bool SuperclassReference::compare(
    const std::shared_ptr<IType> rhs) const 
{
    if (auto r = std::dynamic_pointer_cast<SuperclassReference>(rhs)) {
        return class_ && r->class_ && class_ == r->class_;
    }
    return false;
}

descriptor::JVMFieldDescriptor
SuperclassReference::descriptor(bool out) {
    return class_->record()->descriptor();
}

}