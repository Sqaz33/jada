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

class Decl : public INode { /*...*/ };
 
class DeclArea : public INode {
public:
    ~DeclArea();

    void addDecl(Decl* decl);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<Decl*> decls_;
};

struct IType : INode { 
    virtual bool compare(const IType* rhs) const = 0;
};

class IExpr : public INode { 
public:
    virtual bool compareTypes(const IExpr* rhs) const = 0;
};

class VarDecl : public Decl {
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

class FuncDecl : public Decl {
public:
    using ParamType = std::pair<ParamMode, VarDecl*>;

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

class ProcDecl : public Decl {
public:
    using ParamType = std::pair<ParamMode, VarDecl*>;
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

private:
    std::string name_;
    std::vector<ParamType> params_;
    DeclArea* decls_;
    Body* body_;
};

class PackDecl : public Decl {
public:
    PackDecl(const std::string& name, 
             DeclArea* decls);
    
    ~PackDecl();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string name_;
    DeclArea* decls_;
};

class UseDecl : public Decl {
public:
    UseDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    attribute::QualifiedName name_;
};

class WithDecl : public Decl{
public:
    WithDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    attribute::QualifiedName name_;
};

class RecordDecl : public Decl {
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

class TypeAliasDecl : public Decl {
public:
    TypeAliasDecl(const std::string& name, IType* type); 


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
    ~Aggregate();

    void addInit(const std::string& name, ILiteral* lit);
    void addInit(int idx, ILiteral* lit);
    void addInit(ILiteral* lit);

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override {};

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printNamed_(int spc) const;
    void printIdx_(int spc) const;
    void printInits_(int spc) const;

private:
    std::vector<std::pair<std::string, ILiteral*>> namedInits_;
    std::vector<std::pair<int, ILiteral*>> idxInits_;
    std::vector<ILiteral*> inits_;
};

// class Array : ILiteral {
// public:
//     Array(ArrayType* type,
//           Aggregate* init);

//     ~Array();

// public: // IExpr interface
//     bool compareTypes(const IExpr* rhs) const override;

// public: // INode interface
//     void print(int spc) const override;
//     void* codegen() override { return nullptr; } // TODO
    
// private:
//     Aggregate* value_;
//     StringType* type_;
// };

} // namespace node


// Stms - Control Structure
namespace node {
class If : public IStm {
public:
    If(IExpr* cond, 
       Body* body, 
       Body* els, 
       const std::vector<std::pair<IExpr*, Body*>>& elsifs);

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
class TypeName : IType {
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

class Assign : IStm {
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

class CallOrIndexingOrVarStm : IStm {
public:
    CallOrIndexingOrVarStm(CallOrIndexingOrVar* CIV);
    ~CallOrIndexingOrVarStm();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    CallOrIndexingOrVar* CIV_;
};

} // namespace node
