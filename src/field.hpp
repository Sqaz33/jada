#include "class_member.hpp"

#include "jvm_class.hpp"

namespace class_member {

class JVMClassField : private IJVMClassMember {
public:
    JVMClassField(
        const std::string& name,
        descriptor::JVMFieldDescriptor type,   
        std::weak_ptr<jvm_class::JVMClass> cls);

    JVMClassField(const JVMClassField&) = delete;
    JVMClassField(JVMClassField&&) = default;

public:
    using IJVMClassMember::addFlag;
    using IJVMClassMember::addAttr;
    using IJVMClassMember::isStatic;
    using IJVMClassMember::printBytes;

public:
    std::uint16_t ref(
        std::weak_ptr<jvm_class::JVMClass> cls);

private:
    void linkWithClass_(
        std::weak_ptr<jvm_class::JVMClass> cls);

private:
    std::weak_ptr<jvm_class::JVMClass> selfClass_;
    std::uint16_t fieldRef_;
    std::map<jvm_class::JVMClass*, 
        std::uint16_t> classes_;
    std::string name__;
    descriptor::JVMFieldDescriptor type__;
};

using SharedPtrField = std::shared_ptr<JVMClassField>;

} // namespace class_member