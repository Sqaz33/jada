#include "node.hpp"

#include <algorithm>
#include <unordered_map>

#define TAB 4

// Stms
namespace node {

Body::~Body() {
    for (auto* ptr : stms_) {
        delete ptr;
    }
}

void Body::addStm(Stm* stm) {
    stms_.push_back(stm);
}

void Body::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Body:\n";
    for (auto* ptr : stms_) {
        ptr->print(spc + TAB);
        std::cout << '\n';
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

void DeclArea::addDecl(Decl* decl) {
    decls_.push_back(decl);
}

void DeclArea::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Decl Area:\n";
    for (auto* ptr : decls_) {
        ptr->print(spc + TAB);
    }
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

void VarDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Var Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Type:\n";
    type_->print(spc + TAB*2);
    if (rval_) {
        std::cout << std::string(spc + TAB, ' ') 
                  << "Init Value:\n";
        rval_->print(spc + TAB*2);
    }
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
    for (auto&& [_, param] : params_) {
        delete param;
    }
    delete retType_;
    delete decls_;
    delete body_;
}

void FuncDecl::printParam_(int spc, 
                           const FuncDecl::ParamType& param) const 
{ 
    static const std::unordered_map<ParamMode, std::string> modes {
        { ParamMode::IN, "IN" },
        { ParamMode::OUT, "OUT" },
        { ParamMode::IN_OUT, "IN_OUT" }
    };

    std::cout << std::string(spc, ' ') 
              << "Param: Mode:"
              << modes.at(param.first)
              << '\n';
    param.second->print(spc + TAB);
}

void FuncDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Func Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Return Type:\n";
    retType_->print(spc + TAB*2);
    std::cout << std::string(spc + TAB, ' ') 
              << "Param list:\n";
    for (auto&& param: params_) {
        printParam_(spc + TAB*2, param);
    }
    decls_->print(spc + TAB);
    body_->print(spc + TAB);
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
    for (auto&& [_, param] : params_) {
        delete param;
    }
    delete decls_;
    delete body_;
}

void ProcDecl::printParam_(int spc, 
                           const FuncDecl::ParamType& param) const 
{ 
    static const std::unordered_map<ParamMode, std::string> modes {
        { ParamMode::IN, "IN" },
        { ParamMode::OUT, "OUT" },
        { ParamMode::IN_OUT, "IN_OUT" }
    };

    std::cout << std::string(spc, ' ') 
              << "Param: Mode:"
              << modes.at(param.first)
              << '\n';
    param.second->print(spc + TAB);
}

void ProcDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Func Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Return Type:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Param list:\n";
    for (auto&& param: params_) {
        printParam_(spc + TAB*2, param);
    }
    decls_->print(spc + TAB);
    body_->print(spc + TAB);
}

// ProcDecl
PackDecl::PackDecl(const std::string& name, 
                   DeclArea* decls):
    name_(name)
    , decls_(decls)
{}

PackDecl::~PackDecl() {
    delete decls_;
}

void PackDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Pack Decl: Name: "
              << name_ << '\n';
    decls_->print(spc + TAB);
}

// Use Decl 
UseDecl::UseDecl(attribute::QualifiedName name) :
    name_(std::move(name))
{}

void UseDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Use Decl: Name: ";
    name_.print(spc);
}

// With Decl 
WithDecl::WithDecl(attribute::QualifiedName name) :
    name_(std::move(name))
{}

void WithDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Use Decl: Name: ";
    name_.print(spc);
}

// RecordDecl
RecordDecl::RecordDecl(const std::string& name, 
                       const std::vector<VarDecl*>& decls, 
                       attribute::QualifiedName base = {}, 
                       bool isTagged = false) :
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

void RecordDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Type Record Decl: Name: "
              << name_ << '\n';
    std::cout << std::boolalpha;
    std::cout << std::string(spc + TAB, ' ') 
              << "Is Tagged: " 
              << isTagged_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Is Inherits: "
              << isInherits_ << '\n';
    std::cout << std::noboolalpha;
    base_.print(spc + TAB);
    std::cout << std::string(spc + TAB, ' ') 
            << "Var Decls:\n";
    for (auto* var : decls_) {
        var->print(spc + TAB*2);
    }
}

TypeAliasDecl::TypeAliasDecl(const std::string& name, IType* origin):
    name_(name)
    , origin_(origin)
{}

void TypeAliasDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Type Alias: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ')  
              << "Origin Type:\n";
    origin_->print(spc + TAB*2);
}

} // namespace node 

// Typeinfo
namespace node {

// SimpleLiteralType
SimpleLiteralType::SimpleLiteralType(SimpleType type) :
    type_(type)
{}

void SimpleLiteralType::print(int spc) const {
    static const std::unordered_map<SimpleType, std::string> types {
        { SimpleType::INTEGER, "Integer" },
        { SimpleType::CHAR, "Character" },
        { SimpleType::BOOL, "Boolean" },
        { SimpleType::FLOAT, "Float" }
    };
    std::cout << std::string(spc, ' ') 
              << "Symple Literal Type: " 
              << types.at(type_)
              << '\n';
}

SimpleType SimpleLiteralType::type() const noexcept {
    return type_;
}

// ArrayType
ArrayType::ArrayType(const std::vector<std::pair<int, int>>& ranges, 
                     IType* type) :
    ranges_(ranges)
    , type_(type)
{}

void ArrayType::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Array Type:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Ranges: ( ";
    for (auto&& [l, r] : ranges_) {
        std::cout << '(' << l << ", " << r << ") ";
    }
    std::cout << ")\n";
}

// StringType
StringType::StringType(std::pair<int, int> range) :
    range_(range)
{}

void StringType::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Array Type:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Range: ("
              << range_.first << ", " << range_.second
              << ")\n";
}

// Aggregate
Aggregate::~Aggregate() {
    for (auto&& [_, lit] : namedInits_) {
        delete lit;
    } 
    for (auto&& [_, lit] : idxInits_) {
        delete lit;
    }
    for (auto* lit : inits_) {
        delete lit;
    }
}

void Aggregate::addNamedInit(const std::string& name, ILiteral* lit) {
    namedInits_.push_back(std::make_pair(name, lit));
}

void Aggregate::addIndexingInit(int idx, ILiteral* lit) {
    idxInits_.push_back(std::make_pair(idx, lit));
}

void Aggregate::addInit(ILiteral* lit) {
    inits_.push_back(lit);
}

void Aggregate::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Aggregate: \n";
    std::cout << std::string(spc + TAB, ' ')
              << "Values: \n";
    printNamed_(spc + TAB*2);
    printIdx_(spc + TAB*2);
    printInits_(spc + TAB*2);
}

void Aggregate::printNamed_(int spc) const {
    for (auto&& [name, lit] : namedInits_) {
        std::cout << std::string(spc, ' ')
                  << "Name: " 
                  << name 
                  << '\n';
        std::cout << std::string(spc, ' ')
                  << "Value\n";
        lit->print(spc + TAB);
    }
}

void Aggregate::printIdx_(int spc) const {
    for (auto&& [idx, lit] : idxInits_) {
        std::cout << std::string(spc, ' ')
                  << "Idx: " 
                  << idx 
                  << '\n';
        std::cout << std::string(spc, ' ')
                  << "Value\n";
        lit->print(spc + TAB);
    }
}
void Aggregate::printInits_(int spc) const {
    for (auto* lit : inits_) {
        std::cout << std::string(spc, ' ')
                  << "Value\n";
        lit->print(spc + TAB);
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

void Op::print(int spc) const {
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
        { OpType::UMINUS, "UMINUS" }
    };

    std::cout << std::string(spc, ' ') 
              << "Op: Type: " 
              << ops.at(opType_)
              << '\n';
    if (lhs_) {
        std::cout << std::string(spc, ' ' + TAB)
                  << "Lhs:\n";
        lhs_->print(spc + TAB*2);
    }
    if (rhs_) {
        std::cout << std::string(spc, ' ' + TAB)
                  << "Rhs:\n";
        rhs_->print(spc + TAB*2);
    }
}

} // namespace node 


// Exprs - Literals
namespace node {

// SimpleLiteral
SimpleLiteral::~SimpleLiteral() {
    delete type_;
}

void SimpleLiteral::printValue_(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Value: ";
    switch (type_->type()) {
        case SimpleType::INTEGER: 
            std::cout << get<int>();
            break;
        case SimpleType::BOOL: 
            std::cout << get<bool>();
            break;
        case SimpleType::CHAR: 
            std::cout << get<char>();
            break;
        case SimpleType::FLOAT: 
            std::cout << get<float>();
            break;
    }
    std::cout << '\n';
}

void SimpleLiteral::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Simple Literal:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Type:\n";
    type_->print(spc + TAB*2);
    printValue_(spc + TAB*2);
}

// String
String::String(StringType* type, 
               const std::string& str) :
    str_(str)
    , type_(type)
{}

String::~String() {
    delete type_;
}

void String::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "String:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Type:\n";
    type_->print(spc + TAB*2);
    std::cout << std::string(spc + TAB*2, ' ') 
              << "Value: "
              << str_ 
              << '\n';
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

void If::printElsif_(std::pair<IExpr*, Body*> elsif, int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Elsif:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Cond:\n"; 
    elsif.first->print(spc + TAB*2);
    elsif.second->print(spc + TAB);
}

void If::printElse_(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Else:\n";
    els_->print(spc + TAB);
}

void If::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "If:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Cond:\n";
    cond_->print(spc + TAB*2);
    body_->print(spc + TAB);
    
    for (auto&& elsif : elsifs_) {
        printElsif_(elsif, spc);
    }
    
    printElse_(spc + TAB);
}

For::For(const std::string& init, 
         std::pair<IExpr*, IExpr*> range, 
         Body* body) :
    init_(init)
    , range_(range)
    , body_(body)
{}


void For::print(int spc) const {
    std::cout <<  std::string(spc, ' ')
              << "For init: " << init_
              << " in\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Range\n";
    std::cout << std::string(spc + TAB*2, ' ')
              << "Left:\n";
    range_.first->print(spc + TAB*3);
    std::cout << std::string(spc + TAB*2, ' ')
              << "Right:\n";
    range_.second->print(spc + TAB*3);
    body_->print(spc + TAB*2);
}

While::While(IExpr* cond, 
            Body* body) :
    cond_(cond)
    , body_(body)
{}

void While::print(int spc) const {
    std::cout <<  std::string(spc, ' ')
              << "While:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Cond:\n";
    cond_->print(spc + TAB*2);
    body_->print(spc + TAB);
}

} // namespace node 

// Stms - Other
namespace node {

// DummyCallOrIndexingOrVar:
DummyCallOrIndexingOrVar::DummyCallOrIndexingOrVar(
    attribute::QualifiedName name, 
    const std::vector<IExpr*>& args):
    name_(std::move(name))
    , args_(args)
{}

DummyCallOrIndexingOrVar::DummyCallOrIndexingOrVar(
    attribute::Attribute attr, 
    const std::vector<IExpr*>& args) :
    attr_(std::move(attr))
    , args_(args)
{}

DummyCallOrIndexingOrVar::~DummyCallOrIndexingOrVar() {
    for (auto* arg : args_) {
        delete arg;
    }
}

void DummyCallOrIndexingOrVar::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Subporgram" 
                 " Call or Indexing or Variable:\n";
    printArgs_(spc + TAB);
}

void DummyCallOrIndexingOrVar::printArgs_(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Args";
    for (auto* arg : args_) {
        arg->print(spc + TAB);
    }
}

} // namespace node 


// Typeinfo - Other
namespace node {

// DummyType
DummyType::DummyType(attribute::QualifiedName name) :
    name_(std::move(name))
{}

DummyType::DummyType(attribute::Attribute attr) :
    attr_(std::move(attr))
{}

void DummyType::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Type Name:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Name: ";
    if (!name_.empty()) {
        name_.print(spc);
        return;
    }
    attr_.print(spc);
} 

} // namespace node 


// Stms - Ops 
namespace node {

// Assign
Assign::Assign(DummyCallOrIndexingOrVar* lval,
               IExpr* rval) :
    lval_(lval)
    , rval_(rval)
{}

Assign::~Assign() {
    delete lval_;
    delete rval_;
}
    
void Assign::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Assign:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Lval:\n";
    lval_->print(spc + TAB*2);
    std::cout << std::string(spc + TAB, ' ')
              << "Rval:\n";
    rval_->print(spc + TAB*2);
}

// DummyCallOrIndexingOrVarStm
DummyCallOrIndexingOrVarStm::
DummyCallOrIndexingOrVarStm(DummyCallOrIndexingOrVar* CIV):
    CIV_(CIV)
{}

DummyCallOrIndexingOrVarStm::
~DummyCallOrIndexingOrVarStm() {
    delete CIV_;
}

void 
DummyCallOrIndexingOrVarStm::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Call Or "
                 "Indexing Or Var Statement\n";
    CIV_->print(spc + TAB*2);
}


} // namespace node