#pragma once 

#include <string>
#include <cstdint>
#include <ostream>

namespace constant {

namespace ConstantType {
    constexpr std::uint8_t Utf8        = 1;
    constexpr std::uint8_t Integer     = 3;
    constexpr std::uint8_t Float       = 4;
    constexpr std::uint8_t String      = 8;
    constexpr std::uint8_t Class       = 7;
    constexpr std::uint8_t Fieldref    = 9;
    constexpr std::uint8_t Methodref   = 10;
    constexpr std::uint8_t NameAndType = 12;
    constexpr std::uint8_t MethodType  = 16;
} // namespace ConstantType

} // namespace constant


namespace constant {

struct IConstant {
    IConstant(std::uint8_t type); // ConstantType
    
    virtual ~IConstant() = default;

public:
    virtual void printBytes(std::ostream& out) const = 0; 

protected:
    std::uint8_t type_; 
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
    float numb_;
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

public:
    int name() const noexcept; // utf8

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

class MethodType : public IConstant {
public:
    MethodType(std::uint16_t descr); // utf8 - constant_pool idx

public: // IConstant interface
    void printBytes(std::ostream& out) const override;

private:
    std::uint16_t descr_;
};


} // namespace constant