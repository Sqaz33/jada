#pragma once

#include "isemantics_part.hpp"

#include <set>

namespace semantics_part {

class EntryPointCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class ModuleNameCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class OneLevelWithCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class SelfImportCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class ExistingModuleImportCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class GlobalSpaceCreation : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
private:
    std::pair<bool, std::shared_ptr<node::With>> 
        addImportsPtrs_(std::shared_ptr<mdl::Module> mdl,
                        const std::vector<std::shared_ptr<node::IDecl>>& units);

    std::pair<bool,std::shared_ptr<node::Use>>
        addReduceImportPtrs_(std::shared_ptr<mdl::Module> mdl);
};

class CircularImportCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    bool checkSpace_(
        node::GlobalSpace* cur, 
        std::vector<std::shared_ptr<node::IDecl>>& onStack);
};

class NameConflictCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

public:
    std::string analyseDecl_(std::shared_ptr<node::IDecl> decl);
};

class PackBodyNDeclLinking : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

public:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);

    std::string analyseProgram_(
        const std::vector<
            std::shared_ptr<mdl::Module>>& program);
};

class TypeNameToRealType : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
    std::string analyseDecl_(std::shared_ptr<node::IDecl> decl);
    std::string analyseArrayType_(
        std::shared_ptr<node::ArrayType> atype, 
        node::IDecl* space,
        std::shared_ptr<node::IDecl> parent);
    std::string analyseRecord_(
        std::shared_ptr<node::RecordDecl> decl, 
        std::shared_ptr<node::RecordDecl> derivee = nullptr);
    std::string analyseParam_(std::shared_ptr<node::VarDecl> decl);  
};

class InheritsVarNameConlflicCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
};

class OverloadCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
};

class SubprogBodyNDeclLinking : public ISemanticsPart {
public:
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);

};

class CreateClassDeclaration : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(node::IDecl* decl);
};

class OneClassInSubprogramCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
};

class SetClassForRefs : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
};

// body 
class LinkExprs : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;

private:
    std::string analyseContainer_(std::shared_ptr<node::IDecl> decl);
    
    std::string analyseBody_(
        std::shared_ptr<node::Body> body, 
        std::shared_ptr<node::DeclArea> parDecls,
        const std::vector<std::shared_ptr<node::VarDecl>>& args = {}); 

    std::pair<std::string, std::shared_ptr<node::IExpr>> 
    analyseExpr_(std::shared_ptr<node::IExpr> expr, 
                 attribute::QualifiedName& base); // * 3

    std::pair<std::string, std::shared_ptr<node::IExpr>> 
    analyseOp_(std::shared_ptr<node::Op> op); // * 3 

    std::string analyseRecord_(
        std::shared_ptr<node::DotOpExpr> left, 
        std::shared_ptr<node::IExpr> right);

    std::string analyseOpExprErr_(std::shared_ptr<node::IExpr> expr); // * 1

    std::string analyseArgsExpr_(std::shared_ptr<node::IExpr> expr); // * 2

    std::string analyseInOutRvalLvalNoVal_( // * 4
        const std::vector<std::shared_ptr<node::VarDecl>>& args, 
        std::shared_ptr<node::IExpr> expr, bool lhs, bool noValue, 
        bool first = true);
};

// asg и vardecl rhs - равные типы слева и справа, и не могут быть агрегаты
// asg и vardecl rhs: без вложенных 
// op: равные типы у left и right
// for: типы у и l ренджа - Integer
// if и elsifs: тип выражения - Boolean

class TypeCheck : public ISemanticsPart {

};

} // namespace semantics_part