#pragma once 

#include <string>

namespace constant {

namespace ConstantType {
    constexpr int Utf8 = 1;
    constexpr int Integer = 3;
    constexpr int Float = 4;
    constexpr int String = 8;
    constexpr int Class = 7;
    constexpr int FieldRef = 9;
    constexpr int Methodref = 10;
    constexpr int NameAndType = 12;
} // namespace ConstantType

} // namespace constant


namespace constant {

struct IConstant {
    IConstant(int type); //  // ConstantType
    
    virtual ~IConstant() = default;

public:
    virtual int type() const noexcept = 0; // ConstantType

protected:
    int type_; 
};

} // namespace constant


namespace constant {

class Utf8 : public IConstant {
public:
    Utf8(const std::string& text);

public: // IConstant interface
    int type() const noexcept override;

public:
    const std::string& text() const noexcept;

private:
    std::string text_;
};

class Integer : public IConstant {
public:
    Integer(int numb);

public: // IConstant interface
    int type() const noexcept override;

public:
    int number() const noexcept;

private:
    int numb_;
};

class Float : public IConstant {
public:
    Float(float numb);

public: // IConstant interface
    int type() const noexcept override;

public:
    float number() const noexcept;

private:
    float numb_;
};

class String : public IConstant {
public:
    String(int utf8);

public: // IConstant interface
    int type() const noexcept override;

public:
    int utf8() const noexcept;

private:
    int utf8_;
};

class NameAndType : public IConstant {
public:
    NameAndType(int name, int descr); // utf8, utf8

public: // IConstant interface
    int type() const noexcept override;

public:
    int descriptor() const noexcept; // utf8
    int name() const noexcept; // utf8

private:
    int name_;
    int descr_;
};

class Class : public IConstant {
public:
    Class(int name); // utf8

public: // IConstant interface
    int type() const noexcept override;

public:
    int name() const noexcept; // utf8

private:
    int name_;
};

// class Fieldref : public IConstant {
// public:
//     Fieldref(int )

// };



} // namespace constant