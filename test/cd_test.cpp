#include "codegen.hpp"

int main() {
    codegen::JavaBCCodegen cd(49, 0);
    attribute::QualifiedName name("Main");
    auto cls = cd.createClass(name);
    cls->addAccesFlag(codegen::AccessFlag::ACC_PUBLIC);


    auto initType =  descriptor::JVMMethodDescriptor::createVoidParamsVoidreturn();

    attribute::QualifiedName objName("java/lang/Object");
    auto obj = cd.createClass(objName);
    auto objInit = obj->addMethod("<init>", initType);
    cls->setParent(obj);

    auto init = cls->addMethod("<init>", initType);
    init->addFlag(codegen::AccessFlag::ACC_PUBLIC);
    auto initBB = init->createBB();
    init->createAload(initBB, "this");
    init->createInvokespecial(initBB, objInit);
    init->createReturn(initBB);
    
    
    attribute::QualifiedName strName("java/lang/String");
    auto strArrType = descriptor::JVMFieldDescriptor::createObject(strName);
    strArrType.addDimension();
    std::pair<std::string, descriptor::JVMFieldDescriptor> p(std::string("F"), strArrType);
    auto funcType = descriptor::JVMMethodDescriptor::createVoidRetun({p});
    auto func = cls->addMethod("main", funcType);
    func->addFlag(codegen::AccessFlag::ACC_PUBLIC);

    auto I = descriptor::JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT);
    auto myField = cls->addField("myField", I);
    myField->addFlag(codegen::AccessFlag::ACC_STATIC);

    func->createLocalInt("x");
    auto bb1 = func->createBB();
    
    func->addFlag(codegen::AccessFlag::ACC_PUBLIC);
    func->addFlag(codegen::AccessFlag::ACC_STATIC);

    func->createLdc(bb1, 1024);
    func->createDup(bb1);
    func->createPutstatic(bb1, myField);
    func->createIstore(bb1, "x");

    attribute::QualifiedName sysName("java/lang/System");
    auto sysClass = cd.createClass(sysName);
    attribute::QualifiedName outTypeName("java/io/PrintStream");
    auto outType = descriptor::JVMFieldDescriptor::createObject(outTypeName);
    auto outField = sysClass->addField("out", outType);
    outField->addFlag(codegen::AccessFlag::ACC_STATIC);

    attribute::QualifiedName printName("java/io/PrintStream");
    auto printClass = cd.createClass(printName);
    auto strType = descriptor::JVMFieldDescriptor::createObject(strName);
    std::pair<std::string, descriptor::JVMFieldDescriptor> p1(std::string("F"), strType);
    auto printLnType = descriptor::JVMMethodDescriptor::createVoidRetun({p1});
    auto printLn = printClass->addMethod("println", printLnType);

    func->createGetstatic(bb1, outField);
    func->createLdc(bb1, "Hello, World!");
    func->createInvokevirtual(bb1, printLn);
    func->createGoto(bb1, bb1);
    
    func->createReturn(bb1);

    cd.printClass(cls);
}