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

enum class SimpleLiteralType {
    INTEGER,
    BOOL,
    CHAR
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

// Exprs
// #########################################
class Op : public IExpr {
public:
    Op(IExpr* lhs, OpType opType, IExpr* rhs);
    ~Op();

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    OpType opType_;
    IExpr* lhs_;
    IExpr* rhs_;
};

class ILiteral : public IExpr { /*...*/ };

// Exprs - Literals
// #########################################
class SimpleLiteral : public ILiteral {
public:
    template <class T>
    SimpleLiteral(T&& value, SimpleLiteralType type):
        value_(std::forward<T>(value))
        , type_(type)
    {}

    template <class T>
    T get() const {
        return std::get<T>(value_);
    }

    SimpleLiteralType type() const noexcept;

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::variant<int, bool, char> value_;
    SimpleLiteralType type_;
};

class String : public ILiteral {
public:
    String(std::pair<int, int> range, 
           const std::string& str);

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO
    
private:
    std::pair<int, int> range_;
    std::string str_;
};

class Aggregate : ILiteral {
public:
    ~Aggregate();

    void addInit(ILiteral* lit, const std::string& = "");

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print() const override ;
    void* codegen() override {} // TODO

private:
    std::vector<std::pair<std::string, ILiteral*>> inits_;
};

} // namespace node