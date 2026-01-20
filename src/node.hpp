#pragma once

#include "attribute.hpp"
#include "location.hh"
#include "graphviz.hpp"

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <map>

// inteface
namespace node {    

struct INode : std::enable_shared_from_this<INode> {
    virtual void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const = 0;
    virtual void* codegen() = 0; // TODO
    virtual ~INode() = default;

    void setLocation(const yy::location& loc);

    virtual void setParent(INode* parent);
    INode* parent() noexcept;

    std::shared_ptr<INode> self(); 

protected:
    INode* parent_ = nullptr;
    
protected:
    yy::location loc;
};

} // namespace node

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
    UMINUS,
    DOT
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

class IStm : public INode { /* ... */ };

class ProcDecl;
class PackDecl;
class RecordDecl;
class GlobalSpace;
class IDecl : virtual public INode { 
public: 
    virtual const std::string& name() const noexcept = 0;

    std::vector<
        std::vector<std::shared_ptr<IDecl>>> 
    reachable(
        const attribute::QualifiedName& name, 
        std::shared_ptr<IDecl> requester = nullptr);

protected:
    friend class ProcDecl;
    friend class PackDecl;
    friend class RecordDecl;
    friend class GlobalSpace;
    virtual void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) = 0;
};

struct IType : virtual INode { 
    virtual bool compare(
        const std::shared_ptr<IType> rhs) const = 0;
};

class IExpr : public INode { 
public:
    virtual bool compareTypes(
        const std::shared_ptr<IExpr> rhs) const = 0;
};

class ILiteral : public IExpr { /*...*/ };

} // namespace node

// Stms
// #########################################
namespace node {

class Body : public INode {
public:
    Body(const std::vector<std::shared_ptr<IStm>>& stms);
    
public: // INode interface
    void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<std::shared_ptr<IStm>> stms_;
};

} // namespace node

// Decls 
namespace node { 
class DeclArea : public INode {
public:
    void addDecl(std::shared_ptr<IDecl> decl);

    void replaceDecl(
        const std::string& name, 
        std::shared_ptr<IDecl> decl);

    std::vector<std::shared_ptr<IDecl>>::iterator begin();
    std::vector<std::shared_ptr<IDecl>>::iterator end();

    void setParent(INode* parent) override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<std::shared_ptr<IDecl>> decls_;
};

class VarDecl : public IDecl {
public:
    VarDecl(const std::string& name, 
            std::shared_ptr<IType> type, 
            std::shared_ptr<IExpr> rval = nullptr);
    
public:
    bool compareTypes(std::shared_ptr<IType> rhs) const {};

    std::shared_ptr<IType> type();
    void resetType(std::shared_ptr<IType> type);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public: // IDecl interface
    const std::string& name() const noexcept override;

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

private:
    std::string name_;
    std::shared_ptr<IType> type_;
    std::shared_ptr<IExpr> rval_;
};

class ProcDecl : public IDecl {
public:
    ProcDecl(const std::string& name, 
             const std::vector<std::shared_ptr<VarDecl>>& params,
             std::shared_ptr<DeclArea> decls,
             std::shared_ptr<Body> body);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public: // IDecl interface
    const std::string& name() const noexcept override;

public:
    std::shared_ptr<DeclArea> decls();
    const std::vector<std::shared_ptr<VarDecl>>& params() const noexcept;

private:
    void printParam_(const std::shared_ptr<VarDecl> param, 
                     graphviz::GraphViz& gv, 
                     graphviz::VertexType v) const;
protected:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

protected:
    std::string name_;
    std::vector<std::shared_ptr<VarDecl>> params_;
    std::shared_ptr<DeclArea> decls_;
    std::shared_ptr<Body> body_;
};

class FuncDecl : public ProcDecl {
public:
    FuncDecl(const std::string& name, 
             const std::vector<std::shared_ptr<VarDecl>>& params ,
             std::shared_ptr<DeclArea> decls,
             std::shared_ptr<Body> body,
             std::shared_ptr<IType> retType);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

    using ProcDecl::name;

private:
    std::shared_ptr<IType> retType_;

    friend void ProcDecl::print(graphviz::GraphViz& gv, 
                                graphviz::VertexType par) const;
};

class PackDecl : public IDecl {
public:
    PackDecl(const std::string& name, 
             std::shared_ptr<DeclArea> decls);
    
public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    std::shared_ptr<DeclArea> decls();

public: // IDecl interface
    const std::string& name() const noexcept override;

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

private:
    std::string name_;
    std::shared_ptr<DeclArea> decls_;
};

class GlobalSpace : public IDecl {
public:
    GlobalSpace(std::shared_ptr<IDecl> unit);

public:
    const std::string& name() const noexcept override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override { }; // TODO

    void* codegen() override { return nullptr; } // TODO

public:
    void addImport(std::shared_ptr<IDecl> decl);

    const std::vector<std::shared_ptr<IDecl>>& 
        imports() const noexcept;

    std::shared_ptr<IDecl> unit();

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;
    
private:
    std::shared_ptr<IDecl> unit_;
    std::vector<std::shared_ptr<IDecl>> imports_;
};

class Use : public INode {
public:
    Use(attribute::QualifiedName name);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    const attribute::QualifiedName& name() const noexcept;

private:
    attribute::QualifiedName name_;
};

class With : public INode {
public:
    With(attribute::QualifiedName name);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    const attribute::QualifiedName& name() const noexcept;

private:
    attribute::QualifiedName name_;
};

class RecordDecl : 
    public IDecl 
    , public IType
{
public:
    RecordDecl(const std::string& name, 
               std::shared_ptr<DeclArea> decls, 
               attribute::QualifiedName base = {}, 
               bool isTagged = false);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public: // IDecl interface
    const std::string& name() const noexcept override;

public: // IType interface
    bool compare(
            const std::shared_ptr<IType> rhs) const override { return false; }; // TODO

            
public: 
    void setBase(std::shared_ptr<RecordDecl> base);
    
    std::shared_ptr<DeclArea> decls();

    const attribute::QualifiedName& baseName() const noexcept;

    bool isInherits() const noexcept;

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

private:
    std::weak_ptr<RecordDecl> baseRecord_;
    std::string name_;
    std::shared_ptr<DeclArea> decls_;
    attribute::QualifiedName base_;
    bool isInherits_;
    bool isTagged_;
};

class TypeAliasDecl : 
    public IDecl 
    , public IType
{
public:
    TypeAliasDecl(const std::string& name, 
                  std::shared_ptr<IType> type); 

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public: // IDecl interface
    const std::string& name() const noexcept override;

public: // IType interface
    bool compare(
            const std::shared_ptr<IType> rhs) const override { return false; }; // TODO
            
public: 
    std::shared_ptr<IType> origin();
    void resetOrigin(std::shared_ptr<IType> newOrigin);

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

private:
    std::string name_;
    std::shared_ptr<IType> origin_;
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
    bool compare(const std::shared_ptr<IType> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    SimpleType type_;
};

class ArrayType : public IType {
public:
    ArrayType(const std::vector<std::pair<int, int>>& ranges, 
              std::shared_ptr<IType> type);
    
public: // IType interface
    bool compare(const std::shared_ptr<IType> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;

    void* codegen() override { return nullptr; } // TODO

public:
    std::shared_ptr<IType> type();

    void resetType(std::shared_ptr<IType> newType);

private:
    std::vector<std::pair<int, int>> ranges_; 
    std::shared_ptr<IType> type_;
};

class StringType : public IType {
public:
    StringType(std::pair<int, int> range);

public: // IType interface
    bool 
    compare(const std::shared_ptr<IType> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::pair<int, int> range_; 
};

} // namespace node

// Exprs
namespace node {

class Op : public IExpr {
public:
    Op(std::shared_ptr<IExpr> lhs, 
       OpType opType, 
       std::shared_ptr<IExpr> rhs);

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {}; 

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::shared_ptr<IExpr> lhs_;
    OpType opType_;
    std::shared_ptr<IExpr> rhs_;
};

class NameExpr : public IExpr {
public:
    NameExpr(const std::string& name);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

private:
    std::string name_;
};

class AttributeExpr : public IExpr {
public:
    AttributeExpr(const attribute::Attribute& attr);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO 

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

private:
    attribute::Attribute attr_;
};

class CallOrIdxExpr : public IExpr {
    using ArgsType_ = std::vector<std::shared_ptr<IExpr>>;

public:
    CallOrIdxExpr(std::shared_ptr<IExpr> name, 
                    const std::vector<std::shared_ptr<node::IExpr>>& args);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO  

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

private:
    void printArgs_(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const;

private:
    std::shared_ptr<IExpr> name_;
    ArgsType_ args_;
};

} // namespace node

// Exprs - Literals
namespace node {

class SimpleLiteral : public ILiteral {
public:
    template <class T>
    SimpleLiteral(std::shared_ptr<SimpleLiteralType> type, T&& value):
        type_(type)
        , value_(std::forward<T>(value))
    {}

    template <class T>
    T get() const {
        return std::get<T>(value_);
    }

    SimpleType type() const noexcept;

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string stringifyValue_() const;

private:
    std::shared_ptr<SimpleLiteralType> type_;
    std::variant<int, bool, char, float> value_; 
};

class StringLiteral : public ILiteral {
public:
    StringLiteral(std::shared_ptr<StringType> type, 
                  const std::string& str);

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO
    
private:
    std::string str_; 
    std::shared_ptr<StringType> type_;
};

class Aggregate : public ILiteral {
public:
    Aggregate(const std::vector<std::shared_ptr<ILiteral>>& inits);

public: // IExpr interface
    bool 
    compareTypes(const std::shared_ptr<IExpr> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printInits_(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const;

private:
    std::vector<std::shared_ptr<ILiteral>> inits_;
};

} // namespace node

// Stms - Control Structure
namespace node {
class If : public IStm {
public:
    If(std::shared_ptr<IExpr> cond, 
       std::shared_ptr<Body> body, 
       std::shared_ptr<Body> els = nullptr, 
       const std::vector<std::pair<std::shared_ptr<IExpr>, 
                         std::shared_ptr<Body>>>& elsifs = {});

public: // INode interface
    void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printElsif_(std::pair<std::shared_ptr<IExpr>, 
                               std::shared_ptr<Body>> elsif, 
                     graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const;
    void printElse_(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const; 

private:
    std::shared_ptr<IExpr> cond_;
    std::shared_ptr<Body> body_;
    std::shared_ptr<Body> els_;
    std::vector<std::pair<std::shared_ptr<IExpr>, 
                    std::shared_ptr<Body>>> elsifs_;
};

class For : public IStm {
public:
    For(const std::string& init, 
        std::pair<std::shared_ptr<IExpr>,
                     std::shared_ptr<IExpr>> range, 
        std::shared_ptr<Body> body);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::string init_;
    std::pair<std::shared_ptr<IExpr>,
                 std::shared_ptr<IExpr>> range_; 
    std::shared_ptr<Body> body_;
};

class While : public IStm {
public:
    While(std::shared_ptr<IExpr> cond, 
          std::shared_ptr<Body> body);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO


private:
    std::shared_ptr<IExpr> cond_;
    std::shared_ptr<Body> body_;
};

} // namespace node

// Typeinfo - Other
namespace node {
class TypeName : public IType {
public:
    TypeName(attribute::QualifiedName name);
    TypeName(attribute::Attribute attr);

public: // IType interface
    bool compare(const std::shared_ptr<IType> rhs) const override {};

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    const attribute::QualifiedName& name() const noexcept;

    const attribute::Attribute& attribute() const noexcept;

    bool hasName() const noexcept;

private:
    attribute::QualifiedName name_;
    bool hasName_ = false;
    attribute::Attribute attr_;
};

} // namespace node

// Stms - Ops 
namespace node {

class Assign : public IStm {
public:
    Assign(std::shared_ptr<IExpr> lval,
           std::shared_ptr<IExpr> rval);
    
public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::shared_ptr<IExpr> lval_;
    std::shared_ptr<IExpr> rval_;
};
class MBCall : public IStm {
public:
    MBCall(std::shared_ptr<IExpr> call);

public:
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODOW

private:
    std::shared_ptr<IExpr> call_;
};

class Return : public IStm {
public:
    Return(std::shared_ptr<IExpr> retVal = nullptr);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::shared_ptr<IExpr> retVal_;
};

class ClassDecl; // TODO

class SuperclassReference :
    public IDecl,
    public IType
{
public:
    SuperclassReference(const attribute::Attribute& ref);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override {}; // TODO
    void* codegen() override { return nullptr; } // TODO

public: // IDecl interface
    const std::string& name() const noexcept override {}; // TODO

private:
void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) { } 

public: // IType interface
    bool compare(
            const std::shared_ptr<IType> rhs) const override { return false; }; // TODO

public:
    const attribute::Attribute& ref() const noexcept;
    const std::shared_ptr<ClassDecl>& cls() const noexcept;
    void setClass(std::shared_ptr<ClassDecl> cls);



private:
    attribute::Attribute ref_;
    std::shared_ptr<ClassDecl> class_;
};

} // namespace node
