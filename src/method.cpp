#include "method.hpp"

#include "jvm_class.hpp"

#include <limits>
#include <cstdint>

namespace class_member {

using instr::OpCode;

JVMClassMethod::JVMClassMethod(  
    const std::string& name,
    const descriptor::JVMMethodDescriptor& type,   
    std::weak_ptr<jvm_class::JVMClass> cls,
    bool isStatic) :
    IJVMClassMember(
        cls.lock()->cp()->addUtf8Name(name), 
        cls.lock()->cp()->addMethodDescriptor(std::move(type)),
        true,
        cls.lock()->cp())
    , selfClass_(cls) 
    , name__(name)
    , type__(type)
{   
    if (isStatic) {
        addFlag(codegen::AccessFlag::ACC_STATIC);
    } 

    auto cp = cls.lock()->cp();
    code_.reset(
        new jvm_attribute::CodeAttr(cp));
    addAttr(code_);
    auto nameNType = cp->addNameAndType(
        this->name(), this->type());
    methodRef_ = cp->addMehodRef(
        cls.lock()->nameIdx(), nameNType);

    if (!isStatic) {
        createLocalRef("this");
    }
    for (auto [name, sz] : type__.params()) {
        code_->createLocal(name, sz);
    }
}

bb::SharedPtrBB JVMClassMethod::createBB() {
    return code_->createBB();
}

void JVMClassMethod::createPop(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::pop);
}

void JVMClassMethod::createPop2(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::pop2);
}

void JVMClassMethod::createDup(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup);
}

void JVMClassMethod::createDupX1(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup_x1);
}

void JVMClassMethod::createDupX2(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup_x2);
}

void JVMClassMethod::createDup2(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup2);
}

void JVMClassMethod::createDup2X1(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup2_x1);
}

void JVMClassMethod::createDup2X2(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dup2_x2);
}

void JVMClassMethod::createBipush(
    bb::SharedPtrBB bb, std::int8_t byte) 
{
    instr::Instr ins(OpCode::bipush);
    ins.pushByte(byte);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createSwap(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::swap);
}

void JVMClassMethod::createDconst(
    bb::SharedPtrBB bb, std::int8_t const_) {
    if (0 == const_) {
        code_->insertInstr(bb, OpCode::dconst_0);
    } 
    if (1 == const_) {
        code_->insertInstr(bb, OpCode::dconst_1);
    }
}

void JVMClassMethod::createFconst(
    bb::SharedPtrBB bb, std::int8_t const_) 
{
    switch (const_) {
        case 0: code_->insertInstr(bb, OpCode::fconst_0); break;
        case 1: code_->insertInstr(bb, OpCode::fconst_1); break;
        case 2: code_->insertInstr(bb, OpCode::fconst_2); break;
    }
}

void JVMClassMethod::createIconst(bb::SharedPtrBB bb, std::int8_t const_) {
    switch (const_) {
        case -1: code_->insertInstr(bb, OpCode::iconst_m1); break;
        case 0:  code_->insertInstr(bb, OpCode::iconst_0); break;
        case 1:  code_->insertInstr(bb, OpCode::iconst_1); break;
        case 2:  code_->insertInstr(bb, OpCode::iconst_2); break;
        case 3:  code_->insertInstr(bb, OpCode::iconst_3); break;
        case 4:  code_->insertInstr(bb, OpCode::iconst_4); break;
        case 5:  code_->insertInstr(bb, OpCode::iconst_5); break;
    }
}

void JVMClassMethod::createLconst(
    bb::SharedPtrBB bb, std::int8_t const_) 
{
    if (0 == const_) {
        code_->insertInstr(bb, OpCode::lconst_0);
    } 
    if (1 == const_) {
        code_->insertInstr(bb, OpCode::lconst_1);
    }
}

void JVMClassMethod::createLocalDouble(
    const std::string& name) 
{   
    code_->createLocal(name, 2);    
}

void JVMClassMethod::createLocalFloat(
    const std::string& name) {
    code_->createLocal(name, 1);    
}

void JVMClassMethod::createLocalInt(
    const std::string& name) 
{
    code_->createLocal(name, 1);    
}   

void JVMClassMethod::createLocalLong(
    const std::string& name)
{
    code_->createLocal(name, 2);    
}

void JVMClassMethod::createLocalRef(
    const std::string& name) 
{
    code_->createLocal(name, 1);       
}

void JVMClassMethod::createAload(
    bb::SharedPtrBB bb, const std::string& local) 
{   
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::aload_0 
                : idx == 1 ? OpCode::aload_1 
                : idx == 2 ? OpCode::aload_2
                : idx == 3 ? OpCode::aload_3
                : OpCode::aload; 

    if (OpCode::aload == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createDload(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::dload_0 
                : idx == 1 ? OpCode::dload_1 
                : idx == 2 ? OpCode::dload_2
                : idx == 3 ? OpCode::dload_3
                : OpCode::dload; 

    if (OpCode::dload == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createFload(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::fload_0 
                : idx == 1 ? OpCode::fload_1 
                : idx == 2 ? OpCode::fload_2
                : idx == 3 ? OpCode::fload_3
                : OpCode::fload; 

    if (OpCode::fload == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createIload(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::iload_0 
                : idx == 1 ? OpCode::iload_1 
                : idx == 2 ? OpCode::iload_2
                : idx == 3 ? OpCode::iload_3
                : OpCode::iload; 

    if (OpCode::iload == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createLload(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::lload_0 
                : idx == 1 ? OpCode::lload_1 
                : idx == 2 ? OpCode::lload_2
                : idx == 3 ? OpCode::lload_3
                : OpCode::lload;

    if (OpCode::lload == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createAstore(
    bb::SharedPtrBB bb, const std::string& local) 
{   
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::astore_0 
                : idx == 1 ? OpCode::astore_1 
                : idx == 2 ? OpCode::astore_2
                : idx == 3 ? OpCode::astore_3
                : OpCode::astore; 

    if (OpCode::astore == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createDstore(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::dstore_0 
                : idx == 1 ? OpCode::dstore_1 
                : idx == 2 ? OpCode::dstore_2
                : idx == 3 ? OpCode::dstore_3
                : OpCode::dstore; 

    if (OpCode::dstore == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createFstore(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::fstore_0 
                : idx == 1 ? OpCode::fstore_1 
                : idx == 2 ? OpCode::fstore_2
                : idx == 3 ? OpCode::fstore_3
                : OpCode::fstore; 

    if (OpCode::fstore == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createIstore(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::istore_0 
                : idx == 1 ? OpCode::istore_1 
                : idx == 2 ? OpCode::istore_2
                : idx == 3 ? OpCode::istore_3
                : OpCode::istore; 

    if (OpCode::istore == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createLstore(
    bb::SharedPtrBB bb, const std::string& local) 
{
    auto idx = code_->localIdx(local);
    auto op = idx == 0 ? OpCode::lstore_0 
                : idx == 1 ? OpCode::lstore_1 
                : idx == 2 ? OpCode::lstore_2
                : idx == 3 ? OpCode::lstore_3
                : OpCode::lstore; 

    if (OpCode::lstore == op) {
        code_->instertInstrWithLocal(
            bb, op, local
        );
    } else {
        code_->insertInstr(bb, op);
    }
}

void JVMClassMethod::createLdc(
    bb::SharedPtrBB bb, double numb) 
{   
    auto cp = selfClass_.lock()->cp();
    auto [ok, idx] = cp->getNumbConstIdx(numb);
    if (!ok) {
        idx = cp->addDouble(numb);
    }
    instr::Instr ins(OpCode::ldc2_w);
    ins.pushTwoBytes(idx);
    code_->insertInstr(bb, std::move(ins));
} 

void JVMClassMethod::createLdc(
    bb::SharedPtrBB bb, float numb) 
{
    auto cp = selfClass_.lock()->cp();
    auto [ok, idx] = cp->getNumbConstIdx(numb);
    if (!ok) {
        idx = cp->addFloat(numb);
    }
    std::unique_ptr<instr::Instr> ins;
    if (idx > std::numeric_limits<std::uint8_t>::max()) {
        ins.reset(new instr::Instr(OpCode::ldc_w));
        ins->pushTwoBytes(idx);
    } else {
        ins.reset(new instr::Instr(OpCode::ldc));
        ins->pushByte(
            static_cast<std::uint8_t>(idx));
    }
    code_->insertInstr(bb, *ins);
}   

void JVMClassMethod::createLdc(
    bb::SharedPtrBB bb, int numb) 
{
    auto cp = selfClass_.lock()->cp();
    auto [ok, idx] = cp->getNumbConstIdx(numb);
    if (!ok) {
        idx = cp->addInteger(numb);
    }
    std::unique_ptr<instr::Instr> ins;
    if (idx > std::numeric_limits<std::uint8_t>::max()) {
        ins.reset(new instr::Instr(OpCode::ldc_w));
        ins->pushTwoBytes(idx);
    } else {
        ins.reset(new instr::Instr(OpCode::ldc));
        ins->pushByte(
            static_cast<std::uint8_t>(idx));
    }
    code_->insertInstr(bb, *ins);
}   

void JVMClassMethod::createLdc(
    bb::SharedPtrBB bb, std::int64_t numb) 
{
    auto cp = selfClass_.lock()->cp();
    auto [ok, idx] = cp->getNumbConstIdx(numb);
    if (!ok) {
        idx = cp->addLong(numb);
    }
    instr::Instr ins(OpCode::ldc2_w);
    ins.pushTwoBytes(idx);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createLdc(bb::SharedPtrBB bb, 
    const std::string& string)
{
    auto cp = selfClass_.lock()->cp();
    auto [ok, idx] = cp->getStringIdx(string);
    if (!ok) {
        idx = cp->addString(string);
    }
    std::unique_ptr<instr::Instr> ins;
    if (idx > std::numeric_limits<std::uint8_t>::max()) {
        ins.reset(new instr::Instr(OpCode::ldc_w));
        ins->pushTwoBytes(idx);
    } else {
        ins.reset(new instr::Instr(OpCode::ldc));
        ins->pushByte(
            static_cast<std::uint8_t>(idx));
    }
    code_->insertInstr(bb, *ins);
}   

void JVMClassMethod::createDadd(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dadd);
}

void JVMClassMethod::createFadd(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fadd);
}

void JVMClassMethod::createIadd(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::iadd);
}

void JVMClassMethod::createLadd(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ladd);
}

void JVMClassMethod::createDsub(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dsub);
}

void JVMClassMethod::createFsub(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fsub);
}

void JVMClassMethod::createIsub(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::isub);
}

void JVMClassMethod::createLsub(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lsub);
}

void JVMClassMethod::createDmul(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dmul);
}

void JVMClassMethod::createFmul(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fmul);
}

void JVMClassMethod::createImul(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::imul);
}

void JVMClassMethod::createLmul(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lmul);
}

void JVMClassMethod::createDdiv(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ddiv);
}

void JVMClassMethod::createFdiv(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fdiv);
}

void JVMClassMethod::createIdiv(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::idiv);
}

void JVMClassMethod::createLdiv(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ldiv);
}

void JVMClassMethod::createDneg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dneg);
}

void JVMClassMethod::createFneg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fneg);
}

void JVMClassMethod::createIneg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ineg);
}

void JVMClassMethod::createLneg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lneg);
}

void JVMClassMethod::createDrem(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::drem);
}

void JVMClassMethod::createFrem(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::frem);
}

void JVMClassMethod::createIrem(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::irem);
}

void JVMClassMethod::createLrem(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lrem);
}

void JVMClassMethod::createIinc(
    bb::SharedPtrBB bb, 
    const std::string& local, 
    std::int8_t const_)
{
    code_->instertInstrWithLocal(
        bb,
        OpCode::iinc,
        local,
        { static_cast<std::uint8_t>(const_) }
    );
}

void JVMClassMethod::createIand(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::iand);
}

void JVMClassMethod::createLand(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::land);
}

void JVMClassMethod::createIor(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ior);
}

void JVMClassMethod::createLor(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lor);
}

void JVMClassMethod::createIxor(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ixor);
}

void JVMClassMethod::createLxor(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lxor);
}

void JVMClassMethod::createDcmpl(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dcmpl);
}

void JVMClassMethod::createDcmpg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dcmpg);
}

void JVMClassMethod::createFcmpl(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fcmpl);
}

void JVMClassMethod::createFcmpg(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fcmpg);
}

void JVMClassMethod::createIshl(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ishl);
}

void JVMClassMethod::createIshr(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ishr);
}

void JVMClassMethod::createLshl(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lshl);
}

void JVMClassMethod::createLshr(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lshr);
}

void JVMClassMethod::createReturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::return_);    
}

void JVMClassMethod::createAreturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::areturn);    
}

void JVMClassMethod::createDreturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dreturn);    
}

void JVMClassMethod::createFreturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::freturn);    
}

void JVMClassMethod::createIreturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::ireturn);    
}

void JVMClassMethod::createLreturn(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lreturn);    
}

void JVMClassMethod::createIfeq(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::ifeq, to);
}

void JVMClassMethod::createIfne(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::ifne, to);
}

void JVMClassMethod::createIflt(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::iflt, to);
}

void JVMClassMethod::createIfge(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::ifge, to);
}

void JVMClassMethod::createIfgt(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::ifgt, to);
}

void JVMClassMethod::createIfle(
    bb::SharedPtrBB from, bb::SharedPtrBB to) 
{
    code_->insertBranch(from, OpCode::ifle, to);
}

void JVMClassMethod::createIficmpeq(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmpeq, to);
}

void JVMClassMethod::createIficmpne(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmpne, to);
}

void JVMClassMethod::createIficmplt(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmplt, to);
}

void JVMClassMethod::createIficmpge(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmpge, to);
}

void JVMClassMethod::createIficmpgt(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmpgt, to);
}

void JVMClassMethod::createIficmple(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_icmple, to);
}

void JVMClassMethod::createIfnonull(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::ifnonnull, to);
}

void JVMClassMethod::createIfnull(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::ifnull, to);
}

void JVMClassMethod::createIfacmpeq(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_acmpeq, to);
}

void JVMClassMethod::createIfacmpne(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::if_acmpne, to);
}

void JVMClassMethod::createGoto(
    bb::SharedPtrBB from, bb::SharedPtrBB to)
{
    code_->insertBranch(from, OpCode::goto_, to);
}

void JVMClassMethod::createAnewarray(
    bb::SharedPtrBB bb, std::uint16_t type) 
{   
    instr::Instr ins(OpCode::anewarray);
    ins.pushTwoBytes(type);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createNewarray(
    bb::SharedPtrBB bb, codegen::ArrayType atype)
{
    instr::Instr ins(OpCode::anewarray);
    ins.pushByte(static_cast<std::uint8_t>(atype));
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createMultianewarray(
    bb::SharedPtrBB bb, 
    std::uint16_t type, 
    std::uint8_t demensions)
{
    instr::Instr ins(OpCode::multianewarray);
    ins.pushTwoBytes(type);
    ins.pushByte(demensions);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createArraylength(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::arraylength);
}

void JVMClassMethod::createAaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::aaload);
} 

void JVMClassMethod::createbaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::baload);
}

void JVMClassMethod::createCaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::caload);
}

void JVMClassMethod::createDaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::daload);
}

void JVMClassMethod::createFaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::faload);
}

void JVMClassMethod::createIaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::iaload);
}

void JVMClassMethod::createLaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::laload);
}

void JVMClassMethod::createSaload(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::saload);
}

void JVMClassMethod::createAastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::aastore);
} 

void JVMClassMethod::createbastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::bastore);
}

void JVMClassMethod::createCastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::castore);
}

void JVMClassMethod::createDastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::dastore);
}

void JVMClassMethod::createFastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::fastore);
}

void JVMClassMethod::createIastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::iastore);
}

void JVMClassMethod::createLastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::lastore);
}

void JVMClassMethod::createSastore(bb::SharedPtrBB bb) {
    code_->insertInstr(bb, OpCode::sastore);
}

void JVMClassMethod::createNew(
    bb::SharedPtrBB bb, std::uint16_t type) 
{
    instr::Instr ins(OpCode::new_);
    ins.pushTwoBytes(type);
    code_->insertInstr(bb, std::move(ins));
} 

void JVMClassMethod::createGetfield(
    bb::SharedPtrBB bb, std::shared_ptr<JVMClassField> field)
{
    auto f = field->ref(selfClass_);
    instr::Instr ins(OpCode::getfield);
    ins.pushTwoBytes(f);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createGetstatic(
    bb::SharedPtrBB bb, std::shared_ptr<JVMClassField> field)
{   
    if (!field->isStatic()) {
        throw std::logic_error("static getfield" 
                               " of a non-static field");
    }
    auto f = field->ref(selfClass_);
    instr::Instr ins(OpCode::getstatic);
    ins.pushTwoBytes(f);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createPutfield(
    bb::SharedPtrBB bb, std::shared_ptr<JVMClassField> field)
{
    auto f = field->ref(selfClass_);
    instr::Instr ins(OpCode::putfield);
    ins.pushTwoBytes(f);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createPutstatic(
    bb::SharedPtrBB bb, std::shared_ptr<JVMClassField> field)
{   
    auto f = field->ref(selfClass_);
    instr::Instr ins(OpCode::putstatic);
    ins.pushTwoBytes(f);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createInvokespecial(
    bb::SharedPtrBB bb, 
    std::shared_ptr<JVMClassMethod> method) 
{
    linkMethodNClass_(method);

    std::uint16_t ref;
    if (method->selfClass_.lock() == selfClass_.lock()) {
        ref = method->methodRef_;
    } else {
        ref = classes_[
            method->selfClass_.lock().get()];
    }

    instr::Instr ins(OpCode::invokespecial);
    ins.pushTwoBytes(ref);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createInvokevirtual(
    bb::SharedPtrBB bb, 
    std::shared_ptr<JVMClassMethod> method) 
{
    linkMethodNClass_(method);

    std::uint16_t ref;
    if (method->selfClass_.lock() == selfClass_.lock()) {
        ref = method->methodRef_;
    } else {
        ref = classes_[
            method->selfClass_.lock().get()];
    }

    instr::Instr ins(OpCode::invokevirtual);
    ins.pushTwoBytes(ref);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::createInvokestatic(
    bb::SharedPtrBB bb, 
    std::shared_ptr<JVMClassMethod> method) 
{
    if (!isStatic()) {
        throw std::logic_error("static invocation" 
                               " of a non-static method");
    }

    linkMethodNClass_(method);

    std::uint16_t ref;
    if (method->selfClass_.lock() == selfClass_.lock()) {
        ref = method->methodRef_;
    } else {
        ref = classes_[
            method->selfClass_.lock().get()];
    }

    instr::Instr ins(OpCode::invokestatic);
    ins.pushTwoBytes(ref);
    code_->insertInstr(bb, std::move(ins));
}

void JVMClassMethod::linkMethodNClass_(
    std::shared_ptr<JVMClassMethod> method) 
{
    auto otherClsLock = method->selfClass_.lock();
    auto clsLock = selfClass_.lock();

    if (otherClsLock == clsLock) {
        return;
    }

    if (!classes_.contains(otherClsLock.get())) {   
        auto cp = clsLock->cp();
        auto name = cp->addUtf8Name(method->name__);
        auto type = cp->addMethodDescriptor(method->type__);
        auto nameNType = cp->addNameAndType(name, type);

        auto [ok, otherClsName] = cp->getClassIdx(
                                    otherClsLock->name());
        if (!ok) {
            otherClsName = 
                cp->addClass(otherClsLock->name());
        }

        classes_[otherClsLock.get()] = 
            cp->addMehodRef(otherClsName, nameNType);
    }
}

} // namespace class_member