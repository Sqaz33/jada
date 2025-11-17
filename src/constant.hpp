#pragma once 

#include <string>
#include <cstdint>
#include <ostream>
#include <memory>

#include "descriptor.hpp"

namespace constant {

enum class ConstantType : std::uint8_t {
    Utf8        = 1,
    Integer     = 3,
    Float       = 4,
    String      = 8,
    Class       = 7,
    Fieldref    = 9,
    Methodref   = 10,
    NameAndType = 12
};

} // namespace constant


namespace constant {

struct IConstant {
    IConstant(ConstantType type); // ConstantType
    
    virtual ~IConstant() = default;

public:
    virtual void printBytes(std::ostream& out) const = 0; 

protected:
    ConstantType type_; 
};

} // namespace constant

namespace constant {

class Utf8 : public IConstant {
public:
    Utf8(const std::string& text);

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::string text_;
};

class Integer : public IConstant {
public:
    Integer(int numb);

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    int numb_;
};

class Float : public IConstant {
public:
    Float(float numb);

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    union {
        float f_;
        std::uint32_t i_;
    };
};

class String : public IConstant {
public:
    String(std::uint16_t utf8); // constant_pool idx

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::uint16_t utf8_;
};

class NameAndType : public IConstant {
public:
    // utf8, utf8 - constant_pool idx
    NameAndType(std::uint16_t name, std::uint16_t descr); 

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::uint16_t name_;
    std::uint16_t descr_;
};

class Class : public IConstant {
public:
    Class(std::uint16_t name); // utf8 constant_pool idx

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::uint16_t name_;
};

class Fieldref : public IConstant {
public:
    // utf8, utf8 - constant_pool idx
    Fieldref(std::uint16_t cls, std::uint16_t nameNType); 

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:    
    std::uint16_t class_;
    std::uint16_t nameNType_;
};

class Methodref : public IConstant {
public:
    // utf8, utf8 - constant_pool idx
    Methodref(std::uint16_t cls, std::uint16_t nameNType); 

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:    
    std::uint16_t class_;
    std::uint16_t nameNType_;
};

class Descriptor : public IConstant {
public:
    Descriptor(std::unique_ptr<
        descriptor::JvmFieldDescriptor> fieldType);
    Descriptor(std::unique_ptr<
        descriptor::JvmMethodDescriptor> methodType);

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::unique_ptr<
        descriptor::JvmFieldDescriptor> fieldType_ = nullptr;
    std::unique_ptr<
        descriptor::JvmMethodDescriptor> methodType_ = nullptr;
};

} // namespace constant