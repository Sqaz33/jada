#pragma once

#include <memory>

#include "class_member.hpp"
#include "jvm_attribute.hpp"
#include "descriptor.hpp"
#include "basic_block.hpp"
#include "field.hpp"

namespace jvm_class {

class JVMClass;

} // namespace jvm_class 

namespace class_member {

class JVMClassMethod : private IJVMClassMember {
public: 
    JVMClassMethod(  
        const std::string& name,
        const descriptor::JVMMethodDescriptor& type,   
        std::weak_ptr<jvm_class::JVMClass> cls,
        bool isStatic = false);

    JVMClassMethod(const JVMClassMethod&) = delete;
    JVMClassMethod(JVMClassMethod&&) = default;

public:
    using IJVMClassMember::addFlag;
    using IJVMClassMember::addAttr;
    using IJVMClassMember::isStatic;
    using IJVMClassMember::printBytes;

public:
    bb::BasicBlock*createBB();

    std::uint16_t selfClassRef() const noexcept;
    jvm_class::SharedPtrJVMClass cls();

    const std::string& methodName() const noexcept;
    const descriptor::JVMMethodDescriptor& methodType() const noexcept; 
    
public: 
    // stack
    void createPop(bb::BasicBlock*bb);
    void createPop2(bb::BasicBlock*bb);
    void createDup(bb::BasicBlock*bb);
    void createDupX1(bb::BasicBlock*bb);
    void createDupX2(bb::BasicBlock*bb);
    void createDup2(bb::BasicBlock*bb);
    void createDup2X1(bb::BasicBlock*bb);
    void createDup2X2(bb::BasicBlock*bb);

    void createBipush(bb::BasicBlock*bb, std::int8_t byte);

    void createSwap(bb::BasicBlock*bb);

    // == 0.0 or == 1.0
    void createDconst(bb::BasicBlock*bb, std::int8_t const_); 
    // == 0.0 or == 1.0 or == 2.0
    void createFconst(bb::BasicBlock*bb, std::int8_t const_); 
    // -1 >= const_ <= 5
    void createIconst(bb::BasicBlock*bb, std::int8_t const_); 
    // == 1 or == 0
    void createLconst(bb::BasicBlock*bb, std::int8_t const_); 

    // load, store
    void createLocalDouble(const std::string& name);
    void createLocalFloat(const std::string& name);
    void createLocalInt(const std::string& name);     // for bool, char ...
    void createLocalLong(const std::string& name);
    void createLocalRef(const std::string& name);

    void createAload(bb::BasicBlock*bb, const std::string& local);
    void createDload(bb::BasicBlock*bb, const std::string& local);
    void createFload(bb::BasicBlock*bb, const std::string& local);
    void createIload(bb::BasicBlock*bb, const std::string& local);
    void createLload(bb::BasicBlock*bb, const std::string& local);

    void createAstore(bb::BasicBlock*bb, const std::string& local);
    void createDstore(bb::BasicBlock*bb, const std::string& local);
    void createFstore(bb::BasicBlock*bb, const std::string& local);
    void createIstore(bb::BasicBlock*bb, const std::string& local);
    void createLstore(bb::BasicBlock*bb, const std::string& local);

    void createLdc(bb::BasicBlock*bb, double numb);                //    \ auto ldc, ldc_w, ldc2_w and cp interaction
    void createLdc(bb::BasicBlock*bb, float numb);                 //    /
    void createLdc(bb::BasicBlock*bb, int numb);                   //   /
    void createLdc(bb::BasicBlock*bb, std::int64_t numb);          //  /
    void createLdc(bb::BasicBlock*bb, const std::string& string);  // /

    // math
    void createDadd(bb::BasicBlock*bb);
    void createFadd(bb::BasicBlock*bb);
    void createIadd(bb::BasicBlock*bb);
    void createLadd(bb::BasicBlock*bb);

    void createDsub(bb::BasicBlock*bb);
    void createFsub(bb::BasicBlock*bb);
    void createIsub(bb::BasicBlock*bb);
    void createLsub(bb::BasicBlock*bb);

    void createDmul(bb::BasicBlock*bb);
    void createFmul(bb::BasicBlock*bb);
    void createImul(bb::BasicBlock*bb);
    void createLmul(bb::BasicBlock*bb);

    void createDdiv(bb::BasicBlock*bb);
    void createFdiv(bb::BasicBlock*bb);
    void createIdiv(bb::BasicBlock*bb);
    void createLdiv(bb::BasicBlock*bb);

    void createDneg(bb::BasicBlock*bb);
    void createFneg(bb::BasicBlock*bb);
    void createIneg(bb::BasicBlock*bb);
    void createLneg(bb::BasicBlock*bb);

    void createDrem(bb::BasicBlock*bb);
    void createFrem(bb::BasicBlock*bb);
    void createIrem(bb::BasicBlock*bb);
    void createLrem(bb::BasicBlock*bb);

    void createIinc(
        bb::BasicBlock*bb, 
        const std::string& local, 
        std::int8_t const_);

    // logic
    void createIand(bb::BasicBlock*bb);
    void createLand(bb::BasicBlock*bb);

    void createIor(bb::BasicBlock*bb);
    void createLor(bb::BasicBlock*bb);

    void createIxor(bb::BasicBlock*bb);
    void createLxor(bb::BasicBlock*bb);

    void createDcmpl(bb::BasicBlock*bb);
    void createDcmpg(bb::BasicBlock*bb);
    void createFcmpl(bb::BasicBlock*bb);
    void createFcmpg(bb::BasicBlock*bb);

    // bit manipulation
    void createIshl(bb::BasicBlock*bb);
    void createIshr(bb::BasicBlock*bb);
    void createLshl(bb::BasicBlock*bb);
    void createLshr(bb::BasicBlock*bb);

    // return
    void createReturn(bb::BasicBlock*bb);
    void createAreturn(bb::BasicBlock*bb);
    void createDreturn(bb::BasicBlock*bb);
    void createFreturn(bb::BasicBlock*bb);
    void createIreturn(bb::BasicBlock*bb);
    void createLreturn(bb::BasicBlock*bb);

    // branch    
    void createIfeq(bb::BasicBlock*from, bb::BasicBlock*to); // == 0
    void createIfne(bb::BasicBlock*from, bb::BasicBlock*to); // != 0
    void createIflt(bb::BasicBlock*from, bb::BasicBlock*to); // < 0
    void createIfge(bb::BasicBlock*from, bb::BasicBlock*to); // <= 0
    void createIfgt(bb::BasicBlock*from, bb::BasicBlock*to); // > 0
    void createIfle(bb::BasicBlock*from, bb::BasicBlock*to); // >= 0

    void createIficmpeq(bb::BasicBlock*from, bb::BasicBlock*to); // == 
    void createIficmpne(bb::BasicBlock*from, bb::BasicBlock*to); // != 
    void createIficmplt(bb::BasicBlock*from, bb::BasicBlock*to); // < 
    void createIficmpge(bb::BasicBlock*from, bb::BasicBlock*to); // <= 
    void createIficmpgt(bb::BasicBlock*from, bb::BasicBlock*to); // > 
    void createIficmple(bb::BasicBlock*from, bb::BasicBlock*to); // >= 

    void createIfnonull(bb::BasicBlock*from, bb::BasicBlock*to);
    void createIfnull(bb::BasicBlock*from, bb::BasicBlock*to);

    void createIfacmpeq(bb::BasicBlock*from, bb::BasicBlock*to);
    void createIfacmpne(bb::BasicBlock*from, bb::BasicBlock*to);

    void createGoto(bb::BasicBlock*from, bb::BasicBlock*to); 

    // array
    void createAnewarray(bb::BasicBlock*bb, std::uint16_t type);
    void createNewarray(bb::BasicBlock*bb, codegen::ArrayType atype);
    void createMultianewarray(
        bb::BasicBlock*bb, 
        std::uint16_t type, 
        std::uint8_t demensions);

    void createArraylength(bb::BasicBlock*bb);

    void createAaload(bb::BasicBlock*bb); 
    void createbaload(bb::BasicBlock*bb);
    void createCaload(bb::BasicBlock*bb);
    void createDaload(bb::BasicBlock*bb);
    void createFaload(bb::BasicBlock*bb);
    void createIaload(bb::BasicBlock*bb);
    void createLaload(bb::BasicBlock*bb);
    void createSaload(bb::BasicBlock*bb);

    void createAastore(bb::BasicBlock*bb); 
    void createbastore(bb::BasicBlock*bb);
    void createCastore(bb::BasicBlock*bb);
    void createDastore(bb::BasicBlock*bb);
    void createFastore(bb::BasicBlock*bb);
    void createIastore(bb::BasicBlock*bb);
    void createLastore(bb::BasicBlock*bb);
    void createSastore(bb::BasicBlock*bb);

    // object
    void createNew(bb::BasicBlock*bb, 
                   jvm_class::SharedPtrJVMClass cls);

    void createGetfield(bb::BasicBlock*bb, 
                        std::shared_ptr<JVMClassField> field);

    void createGetstatic(bb::BasicBlock*bb,                         
                         std::shared_ptr<JVMClassField> field);
    
    void createPutfield(bb::BasicBlock*bb,                         
                        std::shared_ptr<JVMClassField> field);

    void createPutstatic(bb::BasicBlock*bb,                         
                         std::shared_ptr<JVMClassField> field);
    
    void createInvokespecial(
        bb::BasicBlock*bb, 
        std::shared_ptr<JVMClassMethod> method);

    void createInvokestatic(
        bb::BasicBlock*bb, 
        std::shared_ptr<JVMClassMethod> method);

    void createInvokevirtual(
        bb::BasicBlock*bb, 
        std::shared_ptr<JVMClassMethod> method);

private:
    std::shared_ptr<jvm_attribute::CodeAttr> code_;
    std::weak_ptr<jvm_class::JVMClass> selfClass_;
    std::uint16_t methodRef_;
    
    std::string name__;
    descriptor::JVMMethodDescriptor type__;
};

using SharedPtrMethod = std::shared_ptr<JVMClassMethod>;

} // namespace class_member