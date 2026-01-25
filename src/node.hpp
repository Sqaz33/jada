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
    DOT,
    AND,
    OR,
    XOR,
    NOT
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

class ProcBody;
class PackDecl;
class PackBody;
class RecordDecl;
class GlobalSpace;
class IDecl : virtual public INode { 
public: 
    virtual const std::string& name() const noexcept = 0;

    virtual std::vector<
        std::vector<std::shared_ptr<IDecl>>> 
    reachable(
        const attribute::QualifiedName& name, 
        std::shared_ptr<IDecl> requester = nullptr);

protected:
    friend class ProcBody;
    friend class PackDecl;
    friend class PackBody;
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

struct IExpr : INode { 
    virtual bool compareTypes(
        const std::shared_ptr<IType> rhs) = 0;
    virtual std::shared_ptr<IType> type() = 0;

    void setInBrackets();
    bool inBrackets() const noexcept;
private:
    bool inBrackets_ = false;
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
    std::shared_ptr<IType> type();
    void resetType(std::shared_ptr<IType> type);

    bool in() const noexcept;
    void setIn(bool in) noexcept;
    bool out() const noexcept;
    void setOut(bool out) noexcept;

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
    bool in_ = true;
    bool out_ = true;
};
// TODO:
// 1. при объявлении и функции и процедуры с одним именим - если rhs в assign - функция
//      если просто вызов - процедура
// 2. разные проверки перегрузки
// 3. можно объявлять функции и процедуры с одним именем в одном спейсе
class ProcBody : public IDecl {
public:
    ProcBody(const std::string& name, 
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

class ProcDecl : public ProcBody {
public:
    ProcDecl(const std::string& name, 
             const std::vector<std::shared_ptr<VarDecl>>& params = {});

public: // INode interface
    void* codegen() override { return nullptr; } // TODO

public:
    void setBody(std::shared_ptr<ProcBody> body);

private:
    std::weak_ptr<ProcBody> body_;
};

class FuncBody : public ProcBody {
public:
    FuncBody(const std::string& name, 
             const std::vector<std::shared_ptr<VarDecl>>& params ,
             std::shared_ptr<DeclArea> decls,
             std::shared_ptr<Body> body,
             std::shared_ptr<IType> retType);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

    using ProcBody::name;

    std::shared_ptr<IType> retType();
    void resetRetType(std::shared_ptr<IType> type);

private:
    std::shared_ptr<IType> retType_;

    friend void ProcBody::print(graphviz::GraphViz& gv, 
                                graphviz::VertexType par) const;
};

class FuncDecl : public FuncBody {
public:
    FuncDecl(const std::string& name, 
             const std::vector<std::shared_ptr<VarDecl>>& params,
             std::shared_ptr<IType> retType);

public: // INode interface
    void* codegen() override { return nullptr; } // TODO

public:
    void setBody(std::shared_ptr<ProcBody> body);
    
private:
    std::weak_ptr<ProcBody> body_;
};

class PackDecl : public IDecl {
public:
    PackDecl(const std::string& name, 
             std::shared_ptr<DeclArea> decls,
             std::shared_ptr<DeclArea> privateDecls = nullptr);
    
public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    std::shared_ptr<DeclArea> decls();
    std::shared_ptr<DeclArea> privateDecls();

public: // IDecl interface
    const std::string& name() const noexcept override;

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

protected:
    friend class PackBody;
    void reachableForPackBody_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester);

public:
    void setPackBody(std::shared_ptr<PackBody> body);
    std::weak_ptr<PackBody> packBody();

protected:
    std::string name_;
    std::shared_ptr<DeclArea> decls_;
    std::shared_ptr<DeclArea> privateDecls_;
    std::weak_ptr<PackBody> packBody_;
};

// + разделение - объявление подпрог. в декле пака, тело в боди пака *
// + нужно слинковать боди пак и декл пак  *
// + проверить что в декле/боди нет боди/декла подпрог. *
// + если в одном спейсе - то на одном уровне сначала декл потом боди *
// + для поиска имен из боди пака нужно вызывать отедльную функцию из декла пака *
// + переопределить reachable для боди *
// + боди пак наследуется от декла пака  *

// + проверка переопределения имен в боди пака из декла пака
// + проверка наличия боди подпрог. в боди пака для деклов подпрог. из декла пака  

// + декл подпрог наследуется от боди и вызывает его методы * 
class PackBody : public PackDecl {
public:
    PackBody(const std::string& name, 
             std::shared_ptr<DeclArea> decls);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override {};
    void* codegen() override { return nullptr; } // TODO

public:
    std::vector<
        std::vector<std::shared_ptr<IDecl>>> 
    reachable(
        const attribute::QualifiedName& name, 
        std::shared_ptr<IDecl> requester = nullptr) override;

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override;

public:
    void setPackDecl(std::shared_ptr<PackDecl> decl);

private:
    std::weak_ptr<PackDecl> packDecl_;
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

class ClassDecl;

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
            const std::shared_ptr<IType> rhs) const override;

            
public: 
    void setBase(std::shared_ptr<RecordDecl> base);

    std::weak_ptr<RecordDecl> base();
    
    const attribute::QualifiedName& baseName() const noexcept;

    std::shared_ptr<DeclArea> decls();

    bool isInherits() const noexcept;

    void setTagged() noexcept;
    bool isTagged() const noexcept;

    std::weak_ptr<ClassDecl> cls();
    void setClass(std::shared_ptr<ClassDecl> cls);

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
    std::weak_ptr<ClassDecl> class_;
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
            const std::shared_ptr<IType> rhs) const override;
            
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
    bool compare(const std::shared_ptr<IType> rhs) const override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    SimpleType type_;
};

class AggregateType : public IType {
public:
    AggregateType(std::vector<SimpleType> type);

public: // IType interface
    bool compare(const std::shared_ptr<IType> rhs) const override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override {}
    void* codegen() override { return nullptr; } // TODO

private:
    std::vector<SimpleType> type_;
};

class ArrayType : public IType {
public:
    ArrayType(const std::vector<std::pair<int, int>>& ranges, 
              std::shared_ptr<IType> type);
    
public: // IType interface
    bool compare(const std::shared_ptr<IType> rhs) const override;

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
    bool compare(const std::shared_ptr<IType> rhs) const override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

public:
    void setInf() noexcept;

    std::pair<int, int> range() const;
private:
    std::pair<int, int> range_; 
    bool inf_;
};

} // namespace node

// Exprs
namespace node {

// res string type
// with ada.text_io;
// procedure main is 
//    S1 : String := "H";
//    S2 : String := "W";
//    S3 : String (1..2);
// begin
//    S3 := S1 & S2;
// end main;


// with ada.text_io; 
// procedure main is 

//    procedure p(s: string) is
//    begin
//       ada.text_io.put_line(s);
//    end p;

// begin
//    p("hi"); // но здесь все ок
// end main;


class Op : public IExpr {
public:
    Op(std::shared_ptr<IExpr> lhs, 
       OpType opType, 
       std::shared_ptr<IExpr> rhs);

public: // IExpr interface
    bool compareTypes(const std::shared_ptr<IType> rhs) override;
    std::shared_ptr<IType> type() override;


public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    std::shared_ptr<IExpr> lhs_;
    OpType opType_;
    std::shared_ptr<IExpr> rhs_;
};

class DotOpExpr : public IExpr {
    void* codegen() override { return nullptr; } // TODO

public:
    void setLeft(std::shared_ptr<DotOpExpr> l);
    void setRight(std::shared_ptr<DotOpExpr> r);

    void setTail(std::shared_ptr<DotOpExpr> tail);
    std::shared_ptr<DotOpExpr> tail();

    std::shared_ptr<DotOpExpr> left();
    std::shared_ptr<DotOpExpr> right();

    virtual bool lhs() = 0; // нужно чтобы проверить всё выражение на lhs и rhs 
    virtual bool rhs() = 0; 
    virtual bool container() = 0;

public: // IExpr interface
    bool compareTypes(const std::shared_ptr<IType> rhs) override;

protected:
    std::weak_ptr<DotOpExpr> left_;
    std::shared_ptr<DotOpExpr> right_;
};

class GetVarExpr : public DotOpExpr {
public:
    GetVarExpr(
        std::shared_ptr<IDecl> owner, 
        std::shared_ptr<VarDecl> var);
public:    
    bool lhs() override;
    bool rhs() override;
    bool container() override;

public: // IExpr interface
    std::shared_ptr<IType> type() override;

private:
    std::shared_ptr<IDecl> owner_; 
    std::shared_ptr<VarDecl> var_;
    bool lhs_;
    bool rhs_;
    bool container_;
};

class GetArrElementExpr : public DotOpExpr {
public:
    GetArrElementExpr(
        std::shared_ptr<IDecl> owner, 
        std::shared_ptr<VarDecl> arr,
        const std::vector<std::shared_ptr<IExpr>>& idxs);
        
public:    
    bool lhs() override;
    bool rhs() override;
    bool container() override;

public: // IExpr interface
    std::shared_ptr<IType> type() override;

private:
    std::shared_ptr<IDecl> owner_; 
    std::shared_ptr<VarDecl> arr_;
    std::vector<std::shared_ptr<IExpr>> idxs_;
    bool container_;
    bool lhs_;
    bool rhs_;
};

class CallExpr : public DotOpExpr {
public:
    CallExpr(
        std::shared_ptr<IDecl> owner, 
        std::shared_ptr<ProcBody> proc,
        std::shared_ptr<FuncBody> func,
        const std::vector<std::shared_ptr<IExpr>>& params = {});

public:    
    bool lhs() override;
    bool rhs() override;
    bool container() override;

public:
    bool setNoValue();

public: // IExpr interface
    std::shared_ptr<IType> type() override;

private:
    std::shared_ptr<IDecl> owner_; 
    std::shared_ptr<ProcBody> proc_;
    std::shared_ptr<FuncBody> func_;
    std::vector<std::shared_ptr<IExpr>> params_;
    bool container_;
    bool noValue_ = false;
};

class CallMethodExpr : public DotOpExpr {
public:
    CallMethodExpr(
        std::shared_ptr<ClassDecl> owner, 
        std::shared_ptr<ProcBody> proc,
        std::shared_ptr<FuncBody> func,
        const std::vector<std::shared_ptr<IExpr>>& params = {});

public:    
    bool lhs() override;
    bool rhs() override;
    bool container() override;

public:
    bool setNoValue();

public: // IExpr interface
    std::shared_ptr<IType> type() override;

private:
    std::shared_ptr<ClassDecl> owner_;
    std::shared_ptr<ProcBody> proc_;
    std::shared_ptr<FuncBody> func_;
    std::vector<std::shared_ptr<IExpr>> params_;
    bool noValue_ = false;
    bool container_;
};

class ImageCallExpr : public DotOpExpr {
public:
    ImageCallExpr( 
        std::shared_ptr<ProcDecl> owner,
        SimpleType type,
        std::shared_ptr<IExpr> param) :
        owner_(owner)
        , type_(type)
        , param_(param)
    {}

public:    
    bool lhs() override;
    bool rhs() override;
    bool container() override;

public: // IExpr interface
    std::shared_ptr<IType> type() override;

private:
    std::shared_ptr<ProcDecl> owner_;
    SimpleType type_;
    std::shared_ptr<IExpr> param_;
};

class NameExpr : public IExpr {
public:
    NameExpr(const std::string& name);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO


public: // IExpr interface
    bool compareTypes(const std::shared_ptr<IType> rhs) override 
    { assert(false); return false; }

    std::shared_ptr<IType> type() override 
    { assert(false); return nullptr; }

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
    bool compareTypes(const std::shared_ptr<IType> rhs) override 
    { assert(false); return false; }

    std::shared_ptr<IType> type() override 
    { assert(false); return nullptr; }

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
    bool compareTypes(const std::shared_ptr<IType> rhs) override 
    { assert(false); return false; }

    std::shared_ptr<IType> type() override 
    { assert(false); return nullptr; }

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
    
    SimpleType literalType() const noexcept;

public: // IExpr interface
    bool compareTypes(const std::shared_ptr<IType> rhs) override;
    std::shared_ptr<IType> type() override;

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
    bool compareTypes(const std::shared_ptr<IType> rhs) override;
    std::shared_ptr<IType> type() override;

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
    bool compareTypes(const std::shared_ptr<IType> rhs) override;
    std::shared_ptr<IType> type() override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override;
    void* codegen() override { return nullptr; } // TODO

private:
    void printInits_(graphviz::GraphViz& gv, 
                     graphviz::VertexType par) const;

private:
    std::shared_ptr<AggregateType> type_;
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
    bool compare(const std::shared_ptr<IType> rhs) const override;

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

// 1. содержит методы (фунции и процедуры)
// 2. содержит рекорд
// 3. поиск метода для точечной нотации 
// 4. является ли класс производным от того, что в сслыке
// 5. каждый тагед рекорд содержит ссылку на cвой ClassDecl (находясь в пакете)
class ClassDecl : public IDecl {
public:
    ClassDecl(std::shared_ptr<RecordDecl> record);

public: // IDecl interface
    const std::string& name() const noexcept override;

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override { assert(false); }; // TODO

    void* codegen() override { return nullptr; } // TODO

public:
    void setBase(std::weak_ptr<ClassDecl> base);

    void addDerived(std::shared_ptr<ClassDecl> derived);

    void addMethod(std::shared_ptr<ProcBody> method);

    bool isDerivedOf(std::shared_ptr<ClassDecl> cls);

    std::shared_ptr<ProcBody> containsMethod(
        const std::string& name, 
        const std::vector<std::shared_ptr<IType>>& params,
        bool proc);
        
private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) override {};

private:
    std::shared_ptr<RecordDecl> record_;
    std::vector<std::weak_ptr<ProcBody>> procs_;
    std::vector<std::weak_ptr<FuncBody>> funcs_;

    std::weak_ptr<ClassDecl> base_;
    std::string name_;
};

class SuperclassReference : public IType {
public:
    SuperclassReference(const attribute::Attribute& ref);

public: // INode interface
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const override { assert(false); }; // TODO
    void* codegen() override { return nullptr; } // TODO

private:
    void reachable_(
        std::vector<
            std::vector<std::shared_ptr<IDecl>>>& res,
        std::vector<std::string>::const_iterator it,
        std::vector<std::string>::const_iterator end,
        std::shared_ptr<IDecl> requester) { assert(false); } 

public: // IType interface
    bool compare(const std::shared_ptr<IType> rhs) const override;

public:
    const attribute::Attribute& ref() const noexcept;
    const std::shared_ptr<ClassDecl>& cls() const noexcept;
    void setClass(std::shared_ptr<ClassDecl> cls);

private:
    attribute::Attribute ref_;
    std::shared_ptr<ClassDecl> class_;
};

} // namespace node
