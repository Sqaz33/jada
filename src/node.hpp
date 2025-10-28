#pragma once

#include "inode.hpp"
#include "attribute.hpp"

#include <vector>
#include <string>
#include <variant>

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
    INTEGER, BOOL, CHAR
};

enum class ParamMode {
    IN, OUT, IN_OUT
};

} // namespace node

namespace node {

// Stms
// #########################################
class Stm : public INode { /*...*/ };

class Body : public INode {
public:
    ~Body();

    void addStm(Stm* stm);

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::vector<Stm*> stms_;
};

// Decls 
// #########################################
class Decl : public INode { /*...*/ };
 
class DeclArea : public INode {
public:
    ~DeclArea();

    void addDecl(Decl* decl);

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::vector<Decl*> decls_;
};

struct IType : INode { 
    virtual bool compare(const IType* rhs) const = 0;
};

// Types

class IExpr : public INode { 
public:
    ~IExpr();
    virtual bool compareTypes(const IExpr* rhs) const = 0;

protected:
    IType* type_;
};

class VarDecl : public Decl {
public:
    VarDecl(const std::string& name, IType* type, IExpr* expr);
    VarDecl() = default;
    
    ~VarDecl();

public:
    // TODO: reduce interface ? 
    const std::string& name() const noexcept;
    void setName(const std::string& name);
    IExpr* expr() noexcept;
    void setExpr(IExpr* expr) noexcept;           
    IType* type() noexcept;
    void setType(IType* type) noexcept;
    bool compareTypes(IType* rhs) const;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    IExpr* val_;
    IType* type_;
};

class FuncDecl : public Decl {
public:
    FuncDecl(const std::string& name, 
             std::vector<std::pair<ParamMode, VarDecl*>> params = {},
             IType* retType,
             DeclArea* decls,
             Body* body);

    ~FuncDecl();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    std::vector<std::pair<ParamMode, VarDecl*>> params_;
    IType* retType_;
    DeclArea* decls_;
    Body* body_;
};

class ProcDecl : public Decl {
public:
    ProcDecl(const std::string& name, 
             std::vector<std::pair<ParamMode, VarDecl*>> params = {},
             DeclArea* decls,
             Body* body);

    ~ProcDecl();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    std::vector<std::pair<ParamMode, VarDecl*>> params_;
    DeclArea* decls;
    Body* body_;
};

class PackDecl : public Decl {
public:
    PackDecl(const std::string& name, 
             DeclArea* decls);
    
    ~PackDecl();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    DeclArea* decls_;
};

class RecordDecl : public Decl {
public:
    RecordDecl(const std::string& name, 
               std::vector<VarDecl*> decls, 
               attribute::QualifiedName base = {}, 
               bool isTagged = false);

    ~RecordDecl();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    std::vector<VarDecl*> decls_;
    attribute::QualifiedName base_;
    bool isInherits_;
    bool isTagged_;
};

class TypeAliasDecl : public Decl {
public:
    TypeAliasDecl(const std::string& name, 
                  attribute::QualifiedName origin);


public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string name_;
    IType* origin_;
};

// Typeinfo
// #########################################
class SimpleLiteralType : public IType {
public:
    SimpleLiteralType(SimpleType type);

public:
    SimpleType type() const noexcept;

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    SimpleType type_;
};

class ArrayType : public IType {
public:
    ArrayType(std::vector<std::pair<int, int>> ranges, 
             IType* type);

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::vector<std::pair<int, int>> ranges_; 
    IType* type_;
};

class StringType : public IType {
public:
    StringType(std::pair<int, int> range);

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::pair<int, int> range_; 
};

// Exprs
// #########################################
class Op : public IExpr {
public:
    Op(IExpr* lhs, OpType opType, IExpr* rhs);
    ~Op();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override; 

public: // INode interface
    void print() const override;
    void* codegen() override {} // TODO

private:
    OpType opType_;
    IExpr* lhs_;
    IExpr* rhs_;
};


// Exprs - Literals
// #########################################
class ILiteral : public IExpr { /*...*/ };

class SimpleLiteral : public ILiteral {
public:
    template <class T>
    SimpleLiteral(T&& value, SimpleType* type):
        value_(std::forward<T>(value))
        , type_(type)
    {}

    ~SimpleLiteral();

    template <class T>
    T get() const {
        return std::get<T>(value_);
    }

    SimpleType type() const noexcept;

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::variant<int, bool, char> value_;
    SimpleLiteralType* type_;
};

class String : public ILiteral {
public:
    String(StringType* type, 
           const std::string& str);

    ~String();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO
    
private:
    std::string str_; 
    StringType* type_;
};

class Aggregate : ILiteral {
public:
    ~Aggregate();

    void addInit(ILiteral* lit, const std::string& name = "");

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::vector<std::pair<std::string, ILiteral*>> inits_;
};

class Array : ILiteral {
public:
    Array(ArrayType* type,
          Aggregate* init);

    ~Array();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO
    
private:
    Aggregate* value_;
    StringType* type_;
};

// Stms - Control Structure
// #########################################
class If : public Stm {
public:
    If(IExpr* cond, 
       Body* body, 
       Body* els, 
       std::vector<std::pair<IExpr*, Body*>> elsifs);

    ~If();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    IExpr* cond_;
    Body* body_;
    Body* els_;
    std::vector<std::pair<IExpr*, Body*>> elsifs_;
};

class For : public Stm {
public:
    For(const std::string& init, 
        std::pair<IExpr*, IExpr*> range, 
        Body* body);

    ~For();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::string init_;
    std::pair<IExpr*, IExpr*> range_; 
    Body* body_;
};

class While : public Stm {
public:
    While(IExpr* cond, 
       Body* body);

    ~While();

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    IExpr* cond_;
    Body* body_;
};

// Stms - Other
// #########################################
class DummyCallOrIndexingOrVar : IExpr {
    DummyCallOrIndexingOrVar(attribute::QualifiedName name, 
                             std::vector<IExpr*> args = {});
    DummyCallOrIndexingOrVar(attribute::Attribute attr, 
                             std::vector<IExpr*> args = {});
    ~DummyCallOrIndexingOrVar();
    
public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    attribute::QualifiedName name_;
    attribute::Attribute attr_;
    std::vector<IExpr*> args_;
};


// Typeinfo - Other
// #########################################
class DummyType : IType {
    DummyType(attribute::QualifiedName name);
    DummyType(attribute::Attribute attr);

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    attribute::QualifiedName name_;
    attribute::Attribute attr_;
};

} // namespace node