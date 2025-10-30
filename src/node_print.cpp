#include "node.hpp"

#include <unordered_map>

#define TAB 4

namespace node {

void Body::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Body:\n";
    for (auto ptr : stms_) {
        ptr->print(spc + TAB);
        std::cout << '\n';
    }
}

void DeclArea::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Decl Area:\n";
    for (auto ptr : decls_) {
        ptr->print(spc + TAB);
    }
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


void FuncDecl::printParam_(int spc, 
                           const FuncDecl::ParamType& param) const 
{ 
    static const std::unordered_map<ParamMode, std::string> modes {
        { ParamMode::IN, "IN" },
        { ParamMode::OUT, "OUT" },
        { ParamMode::IN_OUT, "IN_OUT" }
    };

    std::cout << std::string(spc, ' ') 
              << "Param: Mode: "
              << modes.at(param.second)
              << '\n';
    param.first->print(spc + TAB);
}

void FuncDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Func Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Return Type:\n";
    retType_->print(spc + TAB*2);
    if (!params_.empty()) {
        std::cout << std::string(spc + TAB, ' ') 
              << "Param list:\n";
        for (auto&& param: params_) {
            printParam_(spc + TAB*2, param);
        }
    }
    if (decls_) {
        decls_->print(spc + TAB); 
    }
    body_->print(spc + TAB);
}

void ProcDecl::printParam_(int spc, 
                           const ProcDecl::ParamType& param) const 
{ 
    static const std::unordered_map<ParamMode, std::string> modes {
        { ParamMode::IN, "IN" },
        { ParamMode::OUT, "OUT" },
        { ParamMode::IN_OUT, "IN_OUT" }
    };

    std::cout << std::string(spc, ' ') 
              << "Param: Mode:"
              << modes.at(param.second)
              << '\n';
    param.first->print(spc + TAB);
}

void ProcDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Func Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Return Type:\n";
    if (!params_.empty()) {
        std::cout << std::string(spc + TAB, ' ') 
              << "Param list:\n";
        for (auto&& param: params_) {
            printParam_(spc + TAB*2, param);
        }
    }
    if (decls_) {
        decls_->print(spc + TAB); 
    }
    body_->print(spc + TAB);
}

void PackDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Pack Decl: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ') 
              << "Public:\n";
    decls_->print(spc + TAB*2);
    if (privateDecls_) {
        std::cout << std::string(spc + TAB, ' ') 
                << "Private:\n";
        privateDecls_->print(spc + TAB*2);
    }
}

void UseDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Use Decl: Name: ";
    name_.print(spc);
}

void WithDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Use Decl: Name: ";
    name_.print(spc);
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
    for (auto var : decls_) {
        var->print(spc + TAB*2);
    }
}

void TypeAliasDecl::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Type Alias: Name: "
              << name_ << '\n';
    std::cout << std::string(spc + TAB, ' ')  
              << "Origin Type:\n";
    origin_->print(spc + TAB*2);
}

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

void StringType::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "Array Type:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Range: ("
              << range_.first << ", " << range_.second
              << ")\n";
}

void Aggregate::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Aggregate: \n";
    std::cout << std::string(spc + TAB, ' ')
              << "Values: \n";
    printInits_(spc + TAB*2);
}

void Aggregate::printInits_(int spc) const {
    for (auto lit : inits_) {
        std::cout << std::string(spc, ' ')
                  << "Value\n";
        lit->print(spc + TAB);
    }
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
        std::cout << std::string(spc + TAB, ' ')
                  << "Lhs:\n";
        lhs_->print(spc + TAB*2);
    }
    if (rhs_) {
        std::cout << std::string(spc + TAB, ' ')
                  << "Rhs:\n";
        rhs_->print(spc + TAB*2);
    }
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
    printValue_(spc + TAB);
}

void StringLiteral::print(int spc) const {
    std::cout << std::string(spc, ' ') 
              << "StringLiteral:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Type:\n";
    type_->print(spc + TAB*2);
    std::cout << std::string(spc + TAB, ' ') 
              << "Value: "
              << str_ 
              << '\n';
}

void If::printElsif_(std::pair<std::shared_ptr<IExpr>, 
                     std::shared_ptr<Body>> elsif, int spc) const 
{
    if (elsifs_.empty()) return;
    std::cout << std::string(spc, ' ') 
              << "Elsif:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Cond:\n"; 
    elsif.first->print(spc + TAB*2);
    elsif.second->print(spc + TAB);
}

void If::printElse_(int spc) const {
    if (!els_) return;
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

void While::print(int spc) const {
    std::cout <<  std::string(spc, ' ')
              << "While:\n";
    std::cout << std::string(spc + TAB, ' ') 
              << "Cond:\n";
    cond_->print(spc + TAB*2);
    body_->print(spc + TAB);
}

void CallOrIndexingOrVar::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Subporgram" 
                 " Call or Indexing or Variable:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Name:\n";
    name_.print(spc + TAB*2);
    attr_.print(spc + TAB*2);
    printArgs_(spc + TAB);
}

void CallOrIndexingOrVar::printArgs_(int spc) const {
    if (args_.empty()) return;
    std::cout << std::string(spc, ' ')
              << "Args:\n";
    for (auto arg : args_) {
        arg->print(spc + TAB);
    }
}

void TypeName::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Type Name:\n";
    std::cout << std::string(spc + TAB, ' ')
              << "Name:\n";
    if (!name_.empty()) {
        name_.print(spc + TAB*2);
        return;
    }
    attr_.print(spc + TAB*2);
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

void 
CallOrIndexingOrVarStm::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Unresolved Call Or "
                 "Indexing Or Var Statement\n";
    CIV_->print(spc + TAB*2);
}

void Return::print(int spc) const {
    std::cout << std::string(spc, ' ')
              << "Return Stm:\n";
    ret_->print(spc + TAB);
}

} // namespace node