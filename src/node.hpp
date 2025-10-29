/*
 * // TODO: return stm
 *
 *
*/


#pragma once

#include "inode.hpp"
#include "attribute.hpp"

#include <vector>
#include <string>
#include <variant>

// enums
namespace node {

enum class OpType {
    EQ,
    NEQ,
    MORE,
    LESS,
    GTE,
    LTE,
    AMPER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    UMINUS
};

enum class SimpleType {
    INTEGER, 
    BOOL, 
    CHAR,
    FLOAT 
};

enum class ParamMode {
    IN, OUT, IN_OUT
};

} // namespace node

namespace node {

// Stms
// #########################################
class IStm : public INode { /* ... */ };

class Body : public INode {
public:
    Body(const std::vector<IStm*>& stms);
    
    ~Body();

    void addStm(IStm* stm);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<IStm*> stms_;
};

} // namespace node

// Decls 
namespace node {

class IDecl : public INode { /*...*/ };
 
class DeclArea : public INode {
public:
    ~DeclArea();

    void addDecl(IDecl* decl);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<IDecl*> decls_;
};

struct IType : INode { 
    virtual bool compare(const IType* rhs) const = 0;
};

class IExpr : public INode { 
public:
    virtual bool compareTypes(const IExpr* rhs) const = 0;
};

class VarDecl : public IDecl {
public:
    VarDecl(const std::string& name, IType* type, IExpr* rval = nullptr);
    
    ~VarDecl();

public:
    bool compareTypes(IType* rhs) const {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string name_;
    IType* type_;
    IExpr* rval_;
};

class FuncDecl : public IDecl {
public:
    using ParamType = std::pair<VarDecl*, ParamMode>;

    FuncDecl(const std::string& name, 
             const std::vector<ParamType>& params ,
             IType* retType,
             DeclArea* decls,
             Body* body);

    ~FuncDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO
    

private:
    void printParam_(int spc, const ParamType& param) const;

private:
    std::string name_;
    std::vector<ParamType> params_;
    IType* retType_;
    DeclArea* decls_;
    Body* body_;
};

class ProcDecl : public IDecl {
public:
    using ParamType = std::pair<VarDecl*, ParamMode>;
    ProcDecl(const std::string& name, 
             const std::vector<ParamType>& params,
             DeclArea* decls,
             Body* body);

    ~ProcDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printParam_(int spc, const ParamType& param) const;
    // void setIsOver(bool over) noexcept; TODO: do we want it?

private:
    std::string name_;
    std::vector<ParamType> params_;
    DeclArea* decls_;
    Body* body_;
    // bool over_ = false; TODO
};

class PackDecl : public IDecl {
public:
    PackDecl(const std::string& name, 
             DeclArea* decls, 
             DeclArea* privateDecls = nullptr);
    
    ~PackDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string name_;
    DeclArea* decls_;
    DeclArea* privateDecls_;
};

class UseDecl : public IDecl {
public:
    UseDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    attribute::QualifiedName name_;
};

class WithDecl : public IDecl{
public:
    WithDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    attribute::QualifiedName name_;
};

class RecordDecl : public IDecl {
public:
    RecordDecl(const std::string& name, 
               const std::vector<VarDecl*>& decls, 
               attribute::QualifiedName base = {}, 
               bool isTagged = false);

    ~RecordDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string name_;
    std::vector<VarDecl*> decls_;
    attribute::QualifiedName base_;
    bool isInherits_;
    bool isTagged_;
};

class TypeAliasDecl : public IDecl {
public:
    TypeAliasDecl(const std::string& name, IType* type); 

    ~TypeAliasDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string name_;
    IType* origin_;
};

} // namespace node
 

// Typeinfo
namespace node {

class SimpleLiteralType : public IType {
public:
    SimpleLiteralType(SimpleType type);

public:
    SimpleType type() const noexcept;

public: // IType interface
    bool compare(const IType* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    SimpleType type_;
};

class ArrayType : public IType {
public:
    ArrayType(const std::vector<std::pair<int, int>>& ranges, 
             IType* type);
    
    ~ArrayType();

public: // IType interface
    bool compare(const IType* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<std::pair<int, int>> ranges_; 
    IType* type_;
};

class StringType : public IType {
public:
    StringType(std::pair<int, int> range);

public: // IType interface
    bool compare(const IType* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::pair<int, int> range_; 
};

} // namespace node


// Exprs
namespace node {

class Op : public IExpr {
public:
    Op(IExpr* lhs, OpType opType, IExpr* rhs);
    ~Op();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {}; 

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    IExpr* lhs_;
    OpType opType_;
    IExpr* rhs_;
};

} // namespace node


// Exprs - Literals
namespace node {

class ILiteral : public IExpr { /*...*/ };

class SimpleLiteral : public ILiteral {
public:
    template <class T>
    SimpleLiteral(SimpleLiteralType* type, T&& value):
        type_(type)
        , value_(std::forward<T>(value))
    {}

    ~SimpleLiteral();

    template <class T>
    T get() const {
        return std::get<T>(value_);
    }

    SimpleType type() const noexcept;

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printValue_(int spc) const;

private:
    SimpleLiteralType* type_;
    std::variant<int, bool, char, float> value_; 
};

class StringLiteral : public ILiteral {
public:
    StringLiteral(StringType* type, 
           const std::string& str);

    ~StringLiteral();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO
    
private:
    std::string str_; 
    StringType* type_;
};

class Aggregate : public ILiteral {
public:
    Aggregate(const std::vector<ILiteral*>& inits);

    ~Aggregate();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printInits_(int spc) const;

private:
    std::vector<ILiteral*> inits_;
};

} // namespace node


// Stms - Control Structure
namespace node {
class If : public IStm {
public:
    If(IExpr* cond, 
       Body* body, 
       Body* els = nullptr, 
       const std::vector<std::pair<IExpr*, Body*>>& elsifs = {});

    ~If();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printElsif_(std::pair<IExpr*, Body*> elsif, int spc) const;
    void printElse_(int spc) const; 

private:
    IExpr* cond_;
    Body* body_;
    Body* els_;
    std::vector<std::pair<IExpr*, Body*>> elsifs_;
};

class For : public IStm {
public:
    For(const std::string& init, 
        std::pair<IExpr*, IExpr*> range, 
        Body* body);

    ~For();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string init_;
    std::pair<IExpr*, IExpr*> range_; 
    Body* body_;
};

class While : public IStm {
public:
    While(IExpr* cond, 
          Body* body);

    ~While();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO


private:
    IExpr* cond_;
    Body* body_;
};

} // namespace node


// Stms - Other
namespace node {

class CallOrIndexingOrVar : public IExpr {
public:
    CallOrIndexingOrVar(attribute::QualifiedName name, 
                             const std::vector<IExpr*>& args = {});
    CallOrIndexingOrVar(attribute::Attribute attr, 
                             const std::vector<IExpr*>& args = {});
    ~CallOrIndexingOrVar();
    
public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printArgs_(int spc) const;

private:
    attribute::QualifiedName name_;
    attribute::Attribute attr_;
    std::vector<IExpr*> args_;
};

} // namespace node

// Typeinfo - Other
namespace node {
class TypeName : public IType {
public:
    TypeName(attribute::QualifiedName name);
    TypeName(attribute::Attribute attr);

public: // IType interface
    bool compare(const IType* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    attribute::QualifiedName name_;
    attribute::Attribute attr_;
};

} // namespace node

// Stms - Ops 
namespace node {

class Assign : public IStm {
public:
    Assign(CallOrIndexingOrVar* lval,
           IExpr* rval);
    
    ~Assign();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    CallOrIndexingOrVar* lval_;
    IExpr* rval_;
};

class CallOrIndexingOrVarStm : public IStm {
public:
    CallOrIndexingOrVarStm(CallOrIndexingOrVar* CIV);
    ~CallOrIndexingOrVarStm();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    CallOrIndexingOrVar* CIV_;
};

class Return : public IStm {
public:
    Return(IExpr* ret = nullptr);
    ~Return();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    IExpr* ret_;
};

} // namespace node
