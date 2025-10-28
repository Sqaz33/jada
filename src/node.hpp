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
class Stm : public INode { /*...*/ };

class Body : public INode {
public:
    ~Body();

    void addStm(Stm* stm);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    std::vector<Stm*> stms_;
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
    VarDecl(const std::string& name, IType* type, IExpr* rval = nullptr);
    
    ~VarDecl();

public:
    // TODO: reduce interface ? 
    // const std::string& name() const noexcept;
    // void setName(const std::string& name);
    // IExpr* expr() noexcept;
    // void setExpr(IExpr* expr) noexcept;           
    // IType* type() noexcept;
    // void setType(IType* type) noexcept;
    bool compareTypes(IType* rhs) const;

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    std::string name_;
    IExpr* rval_;
    IType* type_;
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
    void* codegen() override {} // TODO
    

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
    void* codegen() override {} // TODO

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
    void* codegen() override {} // TODO

private:
    std::string name_;
    DeclArea* decls_;
};

class UseDecl : public Decl {
public:
    UseDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    attribute::QualifiedName name_;
};

class WithDecl : public Decl{
public:
    WithDecl(attribute::QualifiedName name);

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

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
    TypeAliasDecl(const std::string& name, IType* type); 


public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

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
    bool compare(const IType* rhs) const override;


public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    SimpleType type_;
};

class ArrayType : public IType {
public:
    ArrayType(const std::vector<std::pair<int, int>>& ranges, 
             IType* type);

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print(int spc) const override;
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
    void print(int spc) const override;
    void* codegen() override {} // TODO

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
    bool compareTypes(const IExpr* rhs) const override; 

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    OpType opType_;
    IExpr* lhs_;
    IExpr* rhs_;
};

} // namespace node


// Exprs - Literals
namespace node {

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
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    void printValue_(int spc) const;

private:
    std::variant<int, bool, char, float> value_; 
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
    void print(int spc) const override;
    void* codegen() override {} // TODO
    
private:
    std::string str_; 
    StringType* type_;
};

class Aggregate : ILiteral {
public:
    ~Aggregate();

    void addNamedInit(const std::string& name, ILiteral* lit);
    void addIndexingInit(int idx, ILiteral* lit);
    void addInit(ILiteral* lit);

public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

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
//     void* codegen() override {} // TODO
    
// private:
//     Aggregate* value_;
//     StringType* type_;
// };

} // namespace node


// Stms - Control Structure
namespace node {
class If : public Stm {
public:
    If(IExpr* cond, 
       Body* body, 
       Body* els, 
       const std::vector<std::pair<IExpr*, Body*>>& elsifs);

    ~If();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    void printElsif_(std::pair<IExpr*, Body*> elsif, int spc) const;
    void printElse_(int spc) const; 

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
    void print(int spc) const override;
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
    void print(int spc) const override;
    void* codegen() override {} // TODO


private:
    IExpr* cond_;
    Body* body_;
};

} // namespace node


// Stms - Other
namespace node {

class DummyCallOrIndexingOrVar : IExpr {
public:
    DummyCallOrIndexingOrVar(attribute::QualifiedName name, 
                             const std::vector<IExpr*>& args = {});
    DummyCallOrIndexingOrVar(attribute::Attribute attr, 
                             const std::vector<IExpr*>& args = {});
    ~DummyCallOrIndexingOrVar();
    
public: // IExpr interface
    bool compareTypes(const IExpr* rhs) const override;

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

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
class DummyType : IType {
    DummyType(attribute::QualifiedName name);
    DummyType(attribute::Attribute attr);

public: // IType interface
    bool compare(const IType* rhs) const override;

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    attribute::QualifiedName name_;
    attribute::Attribute attr_;
};

} // namespace node

// Stms - Ops 
namespace node {

class Assign : Stm {
public:
    Assign(DummyCallOrIndexingOrVar* lval,
           IExpr* rval);
    
    ~Assign();

public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    DummyCallOrIndexingOrVar* lval_;
    IExpr* rval_;
};

class DummyCallOrIndexingOrVarStm : Stm {
public:
    DummyCallOrIndexingOrVarStm(DummyCallOrIndexingOrVar* CIV);
    ~DummyCallOrIndexingOrVarStm();
    
public: // INode interface
    void print(int spc) const override;
    void* codegen() override {} // TODO

private:
    DummyCallOrIndexingOrVar* CIV_;
};

} // namespace node
