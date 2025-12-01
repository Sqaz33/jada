#pragma once

#include <memory>

#include "class_member.hpp"
#include "jvm_attribute.hpp"
#include "descriptor.hpp"
#include "jvm_class.hpp"
#include "basic_block.hpp"
#include "field.hpp"

namespace class_member {

class JVMClassMethod : private IJVMClassMember {
public:
    JVMClassMethod(  
        const std::string& name,
        descriptor::JVMMethodDescriptor type,   
        std::weak_ptr<jvm_class::JVMClass> cls);

    JVMClassMethod(const JVMClassMethod&) = delete;
    JVMClassMethod(JVMClassMethod&&) = default;

public:
    using IJVMClassMember::addFlag;
    using IJVMClassMember::addAttr;
    using IJVMClassMember::isStatic;
    using IJVMClassMember::printBytes;

public:
    bb::SharedPtrBB createBB();

public: 
    // stack
    void createPop(bb::SharedPtrBB bb);
    void createPop2(bb::SharedPtrBB bb);
    void createDup(bb::SharedPtrBB bb);
    void createDupX1(bb::SharedPtrBB bb);
    void createDupX2(bb::SharedPtrBB bb);
    void createDup2(bb::SharedPtrBB bb);
    void createDup2X1(bb::SharedPtrBB bb);
    void createDup2X2(bb::SharedPtrBB bb);

    void createBipush(bb::SharedPtrBB bb, std::int8_t byte);

    void createSwap(bb::SharedPtrBB bb);

    // == 0.0 or == 1.0
    void createDconst(bb::SharedPtrBB bb, std::int8_t const_); 
    // == 0.0 or == 1.0 or == 2.0
    void createFconst(bb::SharedPtrBB bb, std::int8_t const_); 
    // -1 >= const_ <= 5
    void createIconst(bb::SharedPtrBB bb, std::int8_t const_); 
    // == 1 or == 0
    void createLconst(bb::SharedPtrBB bb, std::int8_t const_); 

    // load, store
    void createLocalDouble(const std::string& name);
    void createLocalFloat(const std::string& name);
    void createLocalInt(const std::string& name);     // for bool, char ...
    void createLocalLong(const std::string& name);
    void createLocalRef(const std::string& name);

    void createAload(bb::SharedPtrBB bb, const std::string& local);
    void createDload(bb::SharedPtrBB bb, const std::string& local);
    void createFload(bb::SharedPtrBB bb, const std::string& local);
    void createIload(bb::SharedPtrBB bb, const std::string& local);
    void createLload(bb::SharedPtrBB bb, const std::string& local);

    void createAstore(bb::SharedPtrBB bb, const std::string& local);
    void createDstore(bb::SharedPtrBB bb, const std::string& local);
    void createFstore(bb::SharedPtrBB bb, const std::string& local);
    void createIstore(bb::SharedPtrBB bb, const std::string& local);
    void createLstore(bb::SharedPtrBB bb, const std::string& local);

    void createLdc(bb::SharedPtrBB bb, double numb);       //   \ auto ldc, ldc_w, ldc2_w and cp interaction
    void createLdc(bb::SharedPtrBB bb, float numb);        //   /
    void createLdc(bb::SharedPtrBB bb, int numb);          //  /
    void createLdc(bb::SharedPtrBB bb, std::int64_t numb); // /

    // math
    void createDadd(bb::SharedPtrBB bb);
    void createFadd(bb::SharedPtrBB bb);
    void createIadd(bb::SharedPtrBB bb);
    void createLadd(bb::SharedPtrBB bb);

    void createDsub(bb::SharedPtrBB bb);
    void createFsub(bb::SharedPtrBB bb);
    void createIsub(bb::SharedPtrBB bb);
    void createLsub(bb::SharedPtrBB bb);

    void createDmul(bb::SharedPtrBB bb);
    void createFmul(bb::SharedPtrBB bb);
    void createImul(bb::SharedPtrBB bb);
    void createLmul(bb::SharedPtrBB bb);

    void createDdiv(bb::SharedPtrBB bb);
    void createFdiv(bb::SharedPtrBB bb);
    void createIdiv(bb::SharedPtrBB bb);
    void createLdiv(bb::SharedPtrBB bb);

    void createDneg(bb::SharedPtrBB bb);
    void createFneg(bb::SharedPtrBB bb);
    void createIneg(bb::SharedPtrBB bb);
    void createLneg(bb::SharedPtrBB bb);

    void createDrem(bb::SharedPtrBB bb);
    void createFrem(bb::SharedPtrBB bb);
    void createIrem(bb::SharedPtrBB bb);
    void createLrem(bb::SharedPtrBB bb);

    void createIinc(
        bb::SharedPtrBB bb, 
        const std::string& local, 
        std::int8_t const_);

    // logic
    void createIand(bb::SharedPtrBB bb);
    void createLand(bb::SharedPtrBB bb);

    void createIor(bb::SharedPtrBB bb);
    void createLor(bb::SharedPtrBB bb);

    void createIxor(bb::SharedPtrBB bb);
    void createLxor(bb::SharedPtrBB bb);

    void createDcmpl(bb::SharedPtrBB bb);
    void createDcmpg(bb::SharedPtrBB bb);
    void createFcmpl(bb::SharedPtrBB bb);
    void createFcmpg(bb::SharedPtrBB bb);

    // bit manipulation
    void createIshl(bb::SharedPtrBB bb);
    void createIshr(bb::SharedPtrBB bb);
    void createLshl(bb::SharedPtrBB bb);
    void createLshr(bb::SharedPtrBB bb);

    // return
    void createReturn(bb::SharedPtrBB bb);
    void createAreturn(bb::SharedPtrBB bb);
    void createDreturn(bb::SharedPtrBB bb);
    void createFreturn(bb::SharedPtrBB bb);
    void createIreturn(bb::SharedPtrBB bb);
    void createLreturn(bb::SharedPtrBB bb);

    // branch    
    void createIfeq(bb::SharedPtrBB from, bb::SharedPtrBB to); // == 0
    void createIfne(bb::SharedPtrBB from, bb::SharedPtrBB to); // != 0
    void createIflt(bb::SharedPtrBB from, bb::SharedPtrBB to); // < 0
    void createIfge(bb::SharedPtrBB from, bb::SharedPtrBB to); // <= 0
    void createIfgt(bb::SharedPtrBB from, bb::SharedPtrBB to); // > 0
    void createIfle(bb::SharedPtrBB from, bb::SharedPtrBB to); // >= 0

    void createIficmpeq(bb::SharedPtrBB from, bb::SharedPtrBB to); // == 
    void createIficmpne(bb::SharedPtrBB from, bb::SharedPtrBB to); // != 
    void createIficmplt(bb::SharedPtrBB from, bb::SharedPtrBB to); // < 
    void createIficmpge(bb::SharedPtrBB from, bb::SharedPtrBB to); // <= 
    void createIficmpgt(bb::SharedPtrBB from, bb::SharedPtrBB to); // > 
    void createIficmple(bb::SharedPtrBB from, bb::SharedPtrBB to); // >= 

    void createIfnonull(bb::SharedPtrBB from, bb::SharedPtrBB to);
    void createIfnull(bb::SharedPtrBB from, bb::SharedPtrBB to);

    void createIfacmpeq(bb::SharedPtrBB from, bb::SharedPtrBB to);
    void createIfacmpne(bb::SharedPtrBB from, bb::SharedPtrBB to);

    void createGoto(bb::SharedPtrBB from, bb::SharedPtrBB to); 

    // array
    void createAnewarray(bb::SharedPtrBB bb, std::uint16_t type);
    void createNewarray(bb::SharedPtrBB bb, codegen::ArrayType atype);
    void createMultianewarray(
        bb::SharedPtrBB bb, 
        std::uint16_t type, 
        std::uint8_t demensions);

    void createArraylength(bb::SharedPtrBB bb);

    void createAaload(bb::SharedPtrBB bb); 
    void createbaload(bb::SharedPtrBB bb);
    void createCaload(bb::SharedPtrBB bb);
    void createDaload(bb::SharedPtrBB bb);
    void createFaload(bb::SharedPtrBB bb);
    void createIaload(bb::SharedPtrBB bb);
    void createLaload(bb::SharedPtrBB bb);
    void createSaload(bb::SharedPtrBB bb);

    void createAastore(bb::SharedPtrBB bb); 
    void createbastore(bb::SharedPtrBB bb);
    void createCastore(bb::SharedPtrBB bb);
    void createDastore(bb::SharedPtrBB bb);
    void createFastore(bb::SharedPtrBB bb);
    void createIastore(bb::SharedPtrBB bb);
    void createLastore(bb::SharedPtrBB bb);
    void createSastore(bb::SharedPtrBB bb);

    // object
    void createNew(bb::SharedPtrBB bb, std::uint16_t type);

    void createGetfield(bb::SharedPtrBB bb, 
                        std::shared_ptr<JVMClassField> field);

    void createGetstatic(bb::SharedPtrBB bb,                         
                         std::shared_ptr<JVMClassField> field);
    
    void createPutfield(bb::SharedPtrBB bb,                         
                        std::shared_ptr<JVMClassField> field);

    void createPutstatic(bb::SharedPtrBB bb,                         
                         std::shared_ptr<JVMClassField> field);
    
    void createInvokespecial(
        bb::SharedPtrBB bb, 
        std::shared_ptr<JVMClassMethod> method);

    void createInvokestatic(
        bb::SharedPtrBB bb, 
        std::shared_ptr<JVMClassMethod> method);

    void createInvokevirtual(
        bb::SharedPtrBB bb, 
        std::shared_ptr<JVMClassMethod> method);

private:
    void linkMethodNClass_(
        std::shared_ptr<JVMClassMethod> method);

private:
    std::shared_ptr<jvm_attribute::CodeAttr> code_;
    std::weak_ptr<jvm_class::JVMClass> selfClass_;
    std::uint16_t methodRef_;
    std::map<jvm_class::JVMClass*, 
        std::uint16_t> classes_;
    
    std::string name__;
    descriptor::JVMMethodDescriptor type__;
};

using SharedPtrMethod = std::shared_ptr<JVMClassMethod>;

} // namespace class_member