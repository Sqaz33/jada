#pragma once

#include "module.hpp"
#include "codegen.hpp"

namespace codegen {

void gen(std::vector<std::shared_ptr<mdl::Module>>& program);

// prikols
extern jvm_class::SharedPtrJVMClass InnerSubprograms;
extern jvm_class::SharedPtrJVMClass StringBuiler;
extern jvm_class::SharedPtrJVMClass PrintStream; 
extern jvm_class::SharedPtrJVMClass AtomicInteger;
extern jvm_class::SharedPtrJVMClass AdaUtility; 
extern jvm_class::SharedPtrJVMClass JavaObject; 
extern jvm_class::SharedPtrJVMClass JavaString; 

// init 
extern class_member::SharedPtrMethod AdaUtilityInitArrayElements;
extern class_member::SharedPtrMethod AdaUtilityJavaObjectInit;

// copy
extern class_member::SharedPtrMethod AdaUtilityDeepCopy;
extern class_member::SharedPtrMethod AdaUtilityDeepCopyArray;
extern class_member::SharedPtrMethod AdaUtilityCopyStringBuilder;

// atomic
extern class_member::SharedPtrMethod AdaUtilityToAtomicBoolean;
extern class_member::SharedPtrMethod AdaUtilityToAtomicInt;
extern class_member::SharedPtrMethod AdaUtilityToAtomicFloat;
extern class_member::SharedPtrMethod AdaUtilityToAtomicChar;

extern class_member::SharedPtrMethod AdaUtilityFromAtomicBoolean;
extern class_member::SharedPtrMethod AdaUtilityFromAtomicInt;
extern class_member::SharedPtrMethod AdaUtilityFromAtomicFloat;
extern class_member::SharedPtrMethod AdaUtilityFromAtomicChar;

extern class_member::SharedPtrMethod AdaUtilitySetAtomicBoolean;
extern class_member::SharedPtrMethod AdaUtilitySetAtomicInt;
extern class_member::SharedPtrMethod AdaUtilitySetAtomicFloat;
extern class_member::SharedPtrMethod AdaUtilitySetAtomicChar;

// string builder
extern class_member::SharedPtrMethod AdaUtilitySetCharAt;
extern class_member::SharedPtrMethod AdaUtilityCharAt;
extern class_member::SharedPtrMethod AdaUtilityConcat;
extern class_member::SharedPtrMethod AdaUtilityFromStringLiteral;

// image
extern class_member::SharedPtrMethod AdaUtilityImageFromChar;
extern class_member::SharedPtrMethod AdaUtilityImageFromInt;
extern class_member::SharedPtrMethod AdaUtilityImageFromBool;
extern class_member::SharedPtrMethod AdaUtilityImageFromFloat;

// io
extern class_member::SharedPtrMethod AdaUtilityPrintStringBuilder;

void initAdaUtilityNames();

} // namespace codegen