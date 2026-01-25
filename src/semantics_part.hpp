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

    std::string analyseVarDecl_(std::shared_ptr<node::VarDecl> var);
    
    std::pair<std::string, std::shared_ptr<node::IExpr>> 
    analyseExpr_(std::shared_ptr<node::IExpr> expr);

    std::pair<std::string, std::shared_ptr<node::IExpr>> 
    analyseOp_(std::shared_ptr<node::Op> op);
    
};

} // namespace semantics_part