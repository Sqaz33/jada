#include "ada_codegen.hpp"

#include <ranges>

#include "node.hpp"

#include "descriptor.hpp"

namespace codegen {

void gen(std::vector<std::shared_ptr<mdl::Module>>& program) {
    std::vector<std::shared_ptr<node::IDecl>> decls;

    for (auto&& mod : program) {
        auto unit = mod->unit().lock();
        auto space = std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto spaceUnit = space->unit();
        decls.push_back(spaceUnit);
    }

    int idx = 0;
    for (auto&& d : decls | std::views::drop(1)) {
        if (idx == 0) {
            std::dynamic_pointer_cast<node::ProcBody>(d)->setJavaMain();
        }
        ++idx;
        d->pregen(InnerSubprograms, nullptr);
    }

    for (auto&& d : decls | std::views::drop(1)) {
        d->codegen(nullptr);
    }

    for (auto&& d : decls | std::views::drop(1)) {
        d->printClass();
    }

    cg.printClass(InnerSubprograms);
}

jvm_class::SharedPtrJVMClass InnerSubprograms;

jvm_class::SharedPtrJVMClass StringBuiler;
jvm_class::SharedPtrJVMClass PrintStream; 
jvm_class::SharedPtrJVMClass AtomicInteger;
jvm_class::SharedPtrJVMClass AdaUtility; 
jvm_class::SharedPtrJVMClass JavaObject;
jvm_class::SharedPtrJVMClass JavaString;

class_member::SharedPtrMethod AdaUtilityInitArrayElements;

class_member::SharedPtrMethod AdaUtilityDeepCopy;
class_member::SharedPtrMethod AdaUtilityDeepCopyArray;
class_member::SharedPtrMethod AdaUtilityCopyStringBuilder;

class_member::SharedPtrMethod AdaUtilityToAtomicBoolean;
class_member::SharedPtrMethod AdaUtilityToAtomicInt;
class_member::SharedPtrMethod AdaUtilityToAtomicFloat;
class_member::SharedPtrMethod AdaUtilityToAtomicChar;

class_member::SharedPtrMethod AdaUtilityFromAtomicBoolean;
class_member::SharedPtrMethod AdaUtilityFromAtomicInt;
class_member::SharedPtrMethod AdaUtilityFromAtomicFloat;
class_member::SharedPtrMethod AdaUtilityFromAtomicChar;

class_member::SharedPtrMethod AdaUtilitySetAtomicBoolean;
class_member::SharedPtrMethod AdaUtilitySetAtomicInt;
class_member::SharedPtrMethod AdaUtilitySetAtomicFloat;
class_member::SharedPtrMethod AdaUtilitySetAtomicChar;

class_member::SharedPtrMethod AdaUtilitySetCharAt;
class_member::SharedPtrMethod AdaUtilityCharAt;
class_member::SharedPtrMethod AdaUtilityConcat;
class_member::SharedPtrMethod AdaUtilityFromStringLiteral;

class_member::SharedPtrMethod AdaUtilityImageFromChar;
class_member::SharedPtrMethod AdaUtilityImageFromInt;
class_member::SharedPtrMethod AdaUtilityImageFromBool;
class_member::SharedPtrMethod AdaUtilityImageFromFloat;

class_member::SharedPtrMethod AdaUtilityPrintStringBuilder;

void initAdaUtilityNames() {
    using namespace descriptor;

    // ------------------ классы ------------------
    InnerSubprograms = cg.createClass(
        attribute::QualifiedName("inner_subprograms"));

    StringBuiler = cg.createClass(
        attribute::QualifiedName({"java", "lang", "StringBuilder"}));

    PrintStream = cg.createClass(
        attribute::QualifiedName({"java", "io", "PrintStream"}));

    AtomicInteger = cg.createClass(
        attribute::QualifiedName({"java", "util", "concurrent", "atomic", "AtomicInteger"}));

    AdaUtility = cg.createClass(
        attribute::QualifiedName({"AdaUtility"}));

    JavaObject = cg.createClass(
        attribute::QualifiedName({"java", "lang", "Object"}));

    InnerSubprograms->setParent(JavaObject);
    JavaString = 
        cg.createClass(attribute::QualifiedName({"java", "lang", "String"}));

    // ------------------ методы ------------------
    AdaUtilityInitArrayElements = AdaUtility->addMethod(
        "initArrayElements",
        JVMMethodDescriptor::createVoidParamsVoidReturn());

    AdaUtilityDeepCopy = AdaUtility->addMethod(
        "deepCopy",
        JVMMethodDescriptor::create(
            {{"src", JVMFieldDescriptor::createObject(JavaObject->name())}},
            JVMFieldDescriptor::createObject(JavaObject->name())
        )
    );

    AdaUtilityDeepCopyArray = AdaUtility->addMethod(
        "deepCopyArray",
        JVMMethodDescriptor::create(
            {{"srcArray", JVMFieldDescriptor::createObject(JavaObject->name())}},
            JVMFieldDescriptor::createObject(JavaObject->name())
        )
    );

    AdaUtilityCopyStringBuilder = AdaUtility->addMethod(
        "copyStringBuilder",
        JVMMethodDescriptor::create(
            {{"src", JVMFieldDescriptor::createObject(StringBuiler->name())}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    // ---------- atomic ----------
    AdaUtilityToAtomicBoolean = AdaUtility->addMethod(
        "toAtomic",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::BOOLEAN)}},
            JVMFieldDescriptor::createObject(AtomicInteger->name())
        )
    );

    AdaUtilityToAtomicInt = AdaUtility->addMethod(
        "toAtomic",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)}},
            JVMFieldDescriptor::createObject(AtomicInteger->name())
        )
    );

    AdaUtilityToAtomicFloat = AdaUtility->addMethod(
        "toAtomic",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::FLOAT)}},
            JVMFieldDescriptor::createObject(AtomicInteger->name())
        )
    );

    AdaUtilityToAtomicChar = AdaUtility->addMethod(
        "toAtomic",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)}},
            JVMFieldDescriptor::createObject(AtomicInteger->name())
        )
    );

    // ---------- fromAtomic ----------
    AdaUtilityFromAtomicBoolean = AdaUtility->addMethod(
        "fromAtomic",
        JVMMethodDescriptor::create(
            {{"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())}},
            JVMFieldDescriptor::createFundamental(codegen::FundamentalType::BOOLEAN)
        )
    );

    AdaUtilityFromAtomicInt = AdaUtility->addMethod(
        "fromAtomic",
        JVMMethodDescriptor::create(
            {{"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())}},
            JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)
        )
    );

    AdaUtilityFromAtomicFloat = AdaUtility->addMethod(
        "fromAtomic",
        JVMMethodDescriptor::create(
            {{"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())}},
            JVMFieldDescriptor::createFundamental(codegen::FundamentalType::FLOAT)
        )
    );

    AdaUtilityFromAtomicChar = AdaUtility->addMethod(
        "fromAtomicChar",
        JVMMethodDescriptor::create(
            {{"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())}},
            JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)
        )
    );

    // ---------- setAtomic ----------
    AdaUtilitySetAtomicBoolean = AdaUtility->addMethod(
        "setAtomic",
        JVMMethodDescriptor::createVoidRetun({
            {"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())},
            {"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::BOOLEAN)}
        })
    );

    AdaUtilitySetAtomicInt = AdaUtility->addMethod(
        "setAtomic",
        JVMMethodDescriptor::createVoidRetun({
            {"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())},
            {"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)}
        })
    );

    AdaUtilitySetAtomicFloat = AdaUtility->addMethod(
        "setAtomic",
        JVMMethodDescriptor::createVoidRetun({
            {"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())},
            {"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::FLOAT)}
        })
    );

    AdaUtilitySetAtomicChar = AdaUtility->addMethod(
        "setAtomicChar",
        JVMMethodDescriptor::createVoidRetun({
            {"atomic", JVMFieldDescriptor::createObject(AtomicInteger->name())},
            {"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)}
        })
    );

    // ---------- StringBuilder ----------
    AdaUtilitySetCharAt = AdaUtility->addMethod(
        "setCharAt",
        JVMMethodDescriptor::createVoidRetun({
            {"sb", JVMFieldDescriptor::createObject(StringBuiler->name())},
            {"index", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)},
            {"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)}
        })
    );

    AdaUtilityCharAt = AdaUtility->addMethod(
        "charAt",
        JVMMethodDescriptor::create(
            {{"sb", JVMFieldDescriptor::createObject(StringBuiler->name())},
             {"index", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)}},
            JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)
        )
    );

    AdaUtilityConcat = AdaUtility->addMethod(
        "concat",
        JVMMethodDescriptor::create(
            {{"sb1", JVMFieldDescriptor::createObject(StringBuiler->name())},
             {"sb2", JVMFieldDescriptor::createObject(StringBuiler->name())}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    AdaUtilityFromStringLiteral = AdaUtility->addMethod(
        "fromStringLiteral",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createObject(JavaString->name())}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    // ---------- image ----------
    AdaUtilityImageFromChar = AdaUtility->addMethod(
        "imageFromChar",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::CHAR)}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    AdaUtilityImageFromInt = AdaUtility->addMethod(
        "imageFromInt",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::INT)}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    AdaUtilityImageFromBool = AdaUtility->addMethod(
        "imageFromBool",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::BOOLEAN)}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    AdaUtilityImageFromFloat = AdaUtility->addMethod(
        "imageFromFloat",
        JVMMethodDescriptor::create(
            {{"value", JVMFieldDescriptor::createFundamental(codegen::FundamentalType::FLOAT)}},
            JVMFieldDescriptor::createObject(StringBuiler->name())
        )
    );

    AdaUtilityPrintStringBuilder = AdaUtility->addMethod(
        "printStringBuilder",
        descriptor::JVMMethodDescriptor::createVoidRetun(
            {{"sb", descriptor::JVMFieldDescriptor::createObject(StringBuiler->name())}}
        )
    );
}

} // namespace codegen
