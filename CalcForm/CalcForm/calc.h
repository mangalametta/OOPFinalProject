#pragma once
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>

typedef uint64_t BaseNum;

const BaseNum BaseMax = 1000000000000000000, subtrahend = BaseMax-1;
const uint32_t SizeMax = 1000, BaseLen = 18, ContainLenMax = SizeMax * BaseLen;

class Decimal;
class Complex;

class NumberObject{
    // root class
public:
    friend std::ostream& operator<<(std::ostream&, const NumberObject&);
    friend std::ostream& operator>>(std::ostream&, const NumberObject&);
    
    // Todo:在子類別實作
    virtual void Output(std::ostream&) const = 0;
    virtual void Input(std::istream&) = 0;
    virtual std::string ToString() const = 0;
};

class Integer: public NumberObject{
private:
    std::vector<BaseNum> _digi;
    uint32_t _sizeUsed;
    bool _sign;
public:
    Integer():_sizeUsed(1){
        _digi.resize(SizeMax);
    }
    Integer(const std::string&);
    Integer(BaseNum ,bool);
    
    friend const Integer operator+(const Integer&, const Integer&);
    friend const Integer operator-(const Integer&, const Integer&);
    friend const Integer operator*(const Integer&, const Integer&);
    friend const Integer operator/(const Integer&, const Integer&);
    friend const Integer operator%(const Integer&, const Integer&);
    friend const bool operator>(const Integer&, const Integer&);
    friend const bool operator<(const Integer&, const Integer&);
    friend const bool operator>=(const Integer&, const Integer&);
    friend const bool operator<=(const Integer&, const Integer&);
    friend const bool operator==(const Integer&, const Integer&);
    friend const Integer GCD(const Integer&, const Integer&);
    friend std::ostream& operator<<(std::ostream&, const Integer&);
    friend std::istream& operator>>(std::istream&, Integer&);
    
    Integer operator=(const std::string&);
    
    const Decimal operator+(const Decimal&);
    const Decimal operator-(const Decimal&);
    const Decimal operator*(const Decimal&);
    const Decimal operator/(const Decimal&);
    
    const Complex operator+(const Complex&);
    const Complex operator-(const Complex&);
    const Complex operator*(const Complex&);
    const Complex operator/(const Complex&);
    
    const Integer operator++();
    void Complete();
    void LeftShift();
    void RightShift();
    void SetSign(bool);
    const bool IsZero() const;
    const bool GetSign() const;
    
    virtual void Output(std::ostream&) const;
    virtual void Input(std::istream&);
    virtual std::string ToString() const;
    const Integer Add(const Integer&)const;
    const Integer operator=(const Integer&);
};

class Decimal:public NumberObject{
private:
    Integer _numerator, _denominator;
    bool _sign;
public:
    Decimal(){
        _numerator = Integer(0, false);
        _denominator = Integer(1, false);
    }
    Decimal(const std::string&);
    
    friend const Decimal operator+(const Decimal&, const Decimal&);
    friend const Decimal operator-(const Decimal&, const Decimal&);
    friend const Decimal operator*(const Decimal&, const Decimal&);
    friend const Decimal operator/(const Decimal&, const Decimal&);
    friend const bool operator==(const Decimal&, const Decimal&);
    friend const bool operator>(const Decimal&, const Decimal&);
    friend const bool operator<(const Decimal&, const Decimal&);
    friend const bool operator>=(const Decimal&, const Decimal&);
    friend const bool operator<=(const Decimal&, const Decimal&);
    friend std::ostream& operator<<(std::ostream&, const Decimal&);
    friend std::istream& operator>>(std::istream&, Decimal&);
    
    Decimal operator=(const std::string&);
    
    const Decimal operator+(const Integer&);
    const Decimal operator-(const Integer&);
    const Decimal operator*(const Integer&);
    const Decimal operator/(const Integer&);
    
    const Complex operator+(const Complex&);
    const Complex operator-(const Complex&);
    const Complex operator*(const Complex&);
    const Complex operator/(const Complex&);
    
    void Reduce();
    void SetSign(bool);
    const bool GetSign() const;
    const bool IsReduced();
    std::string ToString(int) const;
    
    static Decimal IntToDecimal(const Integer&);
    
    virtual void Output(std::ostream&) const;
    virtual void Input(std::istream&);
    virtual std::string ToString() const;
};

class Complex:public NumberObject{
private:
    Decimal _realPart, _imaginePart;
public:
    Complex(){}
    Complex(const std::string&);
    
    friend const Complex operator+(const Complex&, const Complex&);
    friend const Complex operator-(const Complex&, const Complex&);
    friend const Complex operator*(const Complex&, const Complex&);
    friend const Complex operator/(const Complex&, const Complex&);
    friend const bool operator==(const Complex&, const Complex&);
    friend std::ostream& operator<<(std::ostream&, const Complex&);
    friend std::istream& operator>>(std::istream&, Complex&);
    
    Complex operator=(const std::string&);
    
    const Complex operator+(const Integer&);
    const Complex operator-(const Integer&);
    const Complex operator*(const Integer&);
    const Complex operator/(const Integer&);
    
    const Complex operator+(const Decimal&);
    const Complex operator-(const Decimal&);
    const Complex operator*(const Decimal&);
    const Complex operator/(const Decimal&);
    
    static Complex IntToComplex(const Integer&);
    static Complex DecimalToComplex(const Decimal&);
    
    virtual void Output(std::ostream&) const;
    virtual void Input(std::istream&);
    virtual std::string ToString() const;
};