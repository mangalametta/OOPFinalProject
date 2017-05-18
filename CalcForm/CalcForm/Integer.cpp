#include "Integer.h"
#include "Decimal.h"
#include "Complex.h"
std::vector<BaseNum> Integer::primes;
//
//
// Integer below
//
//

Integer::Integer(const std::string& numStr){
    std::string numTmp = numStr;
    this->_digi.resize(SizeMax);
    //防呆
    if(numTmp[0] == '+')numTmp = numTmp.substr(1);
    if(numTmp == "")numTmp = "0";
    //處理負號
    this->_sign = numStr[0]=='-' ? true:false;
    numTmp = this->_sign ? numTmp.substr(1):numTmp;
    //計算使用量
    this->_sizeUsed = static_cast<uint32_t>(numTmp.length()) / BaseLen + 1;
    //字串補0方便轉換
    numTmp.insert(0, ContainLenMax-numTmp.length(),'0');
    //轉換
    for(int i=0;i<SizeMax;++i){
        BaseNum tmp = 0;
        for(int j=0;j<BaseLen;++j){
            tmp += static_cast<BaseNum>(pow(10,BaseLen-j-1))*(numTmp[i*BaseLen+j]-'0');
        }
        this->_digi[i] = tmp;
    }
}

Integer::Integer(BaseNum val, bool sign){
    //用數字來建構比較小的
    this->_digi.resize(SizeMax);
    this->_sign = sign;
    this->_digi[SizeMax-1] = val;
    this->_sizeUsed = 1;
    if(this->_digi[SizeMax-1]/BaseMax >0){
        this->_digi[SizeMax-1] = this->_digi[SizeMax-1]%BaseMax;
        this->_digi[SizeMax-2] = val/BaseMax;
        this->_sizeUsed = 2;
    }
    
}

void Integer::Output(std::ostream& stream) const{
    stream << this->ToString();
}

void Integer::Input(std::istream& stream){
    std::string digis;
    stream >> digis;
    *this = Integer(digis);
}

void Integer::Complete(){
    //將數字轉成10補數
    BaseNum temp;
    uint8_t carry = 1;
    for(int i=0;i<SizeMax;++i){
        this->_digi[i] = subtrahend - this->_digi[i];
    }
    for(int i=SizeMax-1;i>=0;--i){
        if(!carry)break;
        temp = this->_digi[i] + carry;
        this->_digi[i] = temp % BaseMax;
        carry = temp / BaseMax;
    }
}

std::string Integer::ToString() const{
    bool leading = true;
    std::string num = "", tmp;
    for(int i=SizeMax-this->_sizeUsed;i<SizeMax;++i){
        if(!this->_digi[i] && leading)continue;
        tmp = std::to_string(this->_digi[i]);
        if(!leading)tmp.insert(0, BaseLen - tmp.length(), '0');
        num += tmp;
        if(this->_digi[i])leading = false;
    }
    if(leading)num = "0";//如果全部都是0就輸出0
    return (this->_sign?"-":"") + num;
}

const Integer Integer::operator++(){
    //prefix ++ ex:++integer
    uint8_t carry = 1;
    BaseNum temp;
    for(int i=SizeMax-1;i>=0;--i){
        if(!carry)break;
        temp = this->_digi[i] + carry;
        this->_digi[i] = temp % BaseMax;
        carry = temp / BaseMax;
    }
    return *this;
}

Integer Integer::operator=(const std::string& numSrt){
    *this = Integer(numSrt);
    return *this;
}

const Integer Integer::operator=(const Integer& rhs){
    this->_sign = rhs._sign;
    this->_sizeUsed = rhs._sizeUsed;
    this->_digi = rhs._digi;
    return *this;
}

const Integer Integer::Add(const Integer& rhs) const{
    Integer ltmp = *this, rtmp = rhs;
    if(ltmp._sign)ltmp.Complete();
    if(rtmp._sign)rtmp.Complete();
    BaseNum carry = 0,temp;
    //加到ltmp，並假設不會溢位
    for(int i=SizeMax-1;i>=0;--i){
        temp = ltmp._digi[i] + rtmp._digi[i] + carry;
        ltmp._digi[i] = temp % BaseMax;
        carry = temp / BaseMax;
    }
    ltmp._sign = false;
    //相加結果是負數(最高為不是0，假設不會滿位)
    if(ltmp._digi[0]/(BaseMax/10)!=0){
        ltmp.Complete();
        ltmp._sign = true;
    }
    //重新計算使用量
    ltmp._sizeUsed = 1;
    for(int i = 0;i<SizeMax;++i){
        if(ltmp._digi[i]){
            ltmp._sizeUsed = SizeMax - i;
            break;
        }
    }
    return ltmp;
    
}

const Integer operator+(const Integer& lhs, const Integer& rhs){
    return lhs.Add(rhs);
}

const Integer operator-(const Integer& lhs, const Integer& rhs){
    Integer ltmp = lhs, rtmp = rhs;
    rtmp._sign = !rtmp._sign;
    return ltmp + rtmp;
}

Integer Mul(const Integer& lhs, const Integer& rhs){
    Integer result(0, false), rtmp , itmp;
    for(int j=SizeMax-1;j>=SizeMax-lhs._sizeUsed;--j){
        rtmp = rhs;
        if(lhs._digi[j] == 0)continue;
        int count = 0;
        while(!rtmp.IsZero()){
            itmp =Integer(lhs._digi[j] * rtmp._digi[SizeMax-1],false);
            rtmp.RightShiftUnit();
            for(int k=0;k<count + (SizeMax-j-1);k++){
                itmp.LeftShiftUnit();
            }count++;
            result = result+itmp;
        }
    }
    result._sign = lhs._sign ^ rhs._sign;
    return result;
}

Integer Karatsuba(const Integer& lhs, const Integer& rhs){
    if(lhs.IsZero()||rhs.IsZero())return Integer(0,false);
    std::string lstr = lhs.ToString(), rstr = rhs.ToString();
    if(lstr[0] == '-')lstr = lstr.substr(1);
    if(rstr[0] == '-')rstr = rstr.substr(1);
    int len = (int)std::max(lstr.length(),rstr.length()) , mid=len/2;
    if(len < 325){
        return Mul(lhs, rhs);
    }
    if(lstr.length()<len)lstr.insert(0,len-lstr.length(),'0');
    if(rstr.length()<len)rstr.insert(0,len-rstr.length(),'0');
    Integer high1(lstr.substr(0,mid)),low1(lstr.substr(mid,lstr.length()-mid));
    Integer high2(rstr.substr(0,mid)),low2(rstr.substr(mid,rstr.length()-mid));
    Integer z0 = Karatsuba(low1,low2);
    Integer z1 = Karatsuba((low1+high1),(low2+high2));
    Integer z2=Karatsuba(high1,high2);
    z1 = z1-z2-z0;
    for(int i=0;i<2*(len-mid);++i){
        z2.LeftShift();
    }
    for(int i=0;i<(len-mid);++i){
        z1.LeftShift();
    }
    return z0+z1+z2;
}

const Integer operator*(Integer lhs,Integer rhs){
    bool sign = lhs._sign ^ rhs._sign;
    lhs._sign=false;rhs._sign=false;
    Integer result = Karatsuba(lhs, rhs);
    result._sign = sign;
    return result;
}


const Integer operator/(const Integer& lhs,const Integer& rhs){
    Integer dividend = lhs, divisor = rhs, result(0,false), oriDivisor = rhs;
    dividend._sign = false;divisor._sign = false;
    if(dividend < divisor)return Integer(0,false);
    if(dividend == divisor)return Integer(1,false);
    if(divisor == Integer(1,false))return dividend;
    while(dividend >= divisor)
        divisor.LeftShift();
    divisor.RightShift();
    while(divisor >= oriDivisor){
        result.LeftShift();
        while(dividend >= divisor){
            ++result;
            //現在不知道是調用乘法快還是用減法快，待測試
            dividend = dividend - divisor;//to do -=
        }
        divisor.RightShift();
    }
    return result;
}

const Integer operator%(const Integer& lhs, const Integer& rhs){
    Integer dividend = lhs, divisor = rhs, result(0,false), oriDivisor = rhs;
    dividend._sign = false;divisor._sign = false;
    if(dividend < divisor)return dividend;
    if(dividend == divisor)return Integer(0,false);
    if(divisor == Integer(1,false))return Integer(0,false);
    while(dividend >= divisor)
        divisor.LeftShift();
    divisor.RightShift();
    while(divisor >= oriDivisor){
        result.LeftShift();
        while(dividend >= divisor){
            ++result;
            //現在不知道是調用乘法快還是用減法快，待測試
            dividend = dividend - divisor;//to do -=
        }
        divisor.RightShift();
    }
    return dividend;
}

const bool operator>(const Integer& lhs, const Integer& rhs){
    if(!(lhs._sign^rhs._sign)){
        if(lhs._sizeUsed > rhs._sizeUsed)return true;
        else if(lhs._sizeUsed < rhs._sizeUsed)return false;
        else{
            std::string lstr = lhs.ToString(), rstr = rhs.ToString();
            if(lhs._sign){
                lstr = lstr.substr(1);
                rstr = rstr.substr(1);
            }
            uint32_t maxNow = static_cast<uint32_t>(lstr.length());
            if(rstr.length()>maxNow)maxNow = static_cast<uint32_t>(rstr.length());
            lstr.insert(0, maxNow - lstr.length(), '0');
            rstr.insert(0, maxNow - rstr.length(), '0');
            return lstr > rstr;
        }
    }
    return !lhs._sign;
}

const bool operator<(const Integer& lhs, const Integer& rhs){
    if(!(lhs._sign^rhs._sign)){
        if(lhs._sizeUsed > rhs._sizeUsed)return false;
        else if(lhs._sizeUsed < rhs._sizeUsed)return true;
        else{
            std::string lstr = lhs.ToString(), rstr = rhs.ToString();
            if(lhs._sign){
                lstr = lstr.substr(1);
                rstr = rstr.substr(1);
            }
            uint32_t maxNow = static_cast<uint32_t>(lstr.length());
            if(rstr.length()>maxNow)maxNow = static_cast<uint32_t>(rstr.length());
            lstr.insert(0, maxNow - lstr.length(), '0');
            rstr.insert(0, maxNow - rstr.length(), '0');
            return lstr < rstr;
        }
    }
    return lhs._sign;
}

const bool operator>=(const Integer& lhs, const Integer& rhs){
    return !(lhs < rhs);
}

const bool operator<=(const Integer& lhs, const Integer& rhs){
    return !(lhs > rhs);
}
const bool operator==(const Integer& lhs, const Integer& rhs){
    if(!(lhs._sign^rhs._sign)){
        if(lhs._sizeUsed != rhs._sizeUsed) return false;
        else{
            std::string lstr = lhs.ToString(), rstr = rhs.ToString();
            return lstr == rstr;
        }
    }
    return false;
}

const bool Integer::IsZero() const{
    for(int i=SizeMax-1;i>=SizeMax-this->_sizeUsed;--i){
        if(this->_digi[i])return false;
    }
    return true;
}

const Integer GCD(const Integer& lhs, const Integer& rhs){
    Integer ltmp = lhs, rtmp = rhs, tmp;
    ltmp._sign = false;rtmp._sign = false;
    while(!rtmp.IsZero()){
        tmp = rtmp;
        rtmp = ltmp%rtmp;
        ltmp = tmp;
    }
    return ltmp;
}
void Integer::LeftShiftUnit(){
    for(int i=SizeMax-this->_sizeUsed;i<=SizeMax-1;++i){
        this->_digi[i-1] = this->_digi[i];
    }
    this->_digi[SizeMax-1] = 0;
    this->_sizeUsed++;
}

void Integer::RightShiftUnit(){
    for(int i = SizeMax-1;i>=SizeMax-this->_sizeUsed;--i){
        this->_digi[i] = this->_digi[i-1];
    }
    this->_sizeUsed--;
}

void Integer::LeftShift(){
    BaseNum carry = 0, tmp, origin;
    uint32_t newSize = 0;
    for(int i=SizeMax-1;i>=SizeMax-this->_sizeUsed || carry;--i){
        origin = this->_digi[i];
        tmp = this->_digi[i]*9 + carry;//先乘9
        this->_digi[i] = tmp % BaseMax;
        carry = tmp / BaseMax;
        tmp = this->_digi[i] + origin;//再加一次
        this->_digi[i] = tmp % BaseMax;
        carry += tmp / BaseMax;
        newSize = SizeMax - i;
    }
    this->_sizeUsed = newSize;
}

void Integer::RightShift(){
    for(int i=SizeMax-1;i>=SizeMax-this->_sizeUsed;--i){
        this->_digi[i] /= 10;
        this->_digi[i] += this->_digi[i-1]%10 * (BaseMax/10);
    }
    if(!this->_digi[SizeMax - this->_sizeUsed]){
        this->_sizeUsed -= 1;
    }
}

const bool Integer::GetSign() const{
    return this->_sign;
}

void Integer::SetSign(bool sign){
    this->_sign = sign;
}

const Integer Integer::Power(const Integer& rhs){
    Integer one(1, false), two(2, false), tmp;
    //end Condition
    if(rhs.IsZero()){
        // return square root
    }else if((rhs - one).IsZero()){
        return *this;
    }
    //
    if((rhs%two).IsZero()){
        tmp = this->Power(rhs / two);
        return tmp * tmp;
    }else{
        tmp = this->Power(rhs / two);
        return tmp * tmp * *this;
    }
}

std::ostream& operator<<(std::ostream& stream, const Integer& rhs){
    rhs.Output(stream);
    return stream;
}

std::istream& operator>>(std::istream& stream, Integer& rhs){
    rhs.Input(stream);
    return stream;
}

const Decimal Integer::operator+(const Decimal& rhs){
    return Decimal::IntToDecimal(*this) + rhs;
}

const Decimal Integer::operator-(const Decimal& rhs){
    return Decimal::IntToDecimal(*this) - rhs;
}

const Decimal Integer::operator*(const Decimal& rhs){
    return Decimal::IntToDecimal(*this) * rhs;
}

const Decimal Integer::operator/(const Decimal& rhs){
    return Decimal::IntToDecimal(*this) / rhs;
}

const Complex Integer::operator+(const Complex& rhs){
    return Complex::IntToComplex(*this) + rhs;
}

const Complex Integer::operator-(const Complex& rhs){
 return Complex::IntToComplex(*this) - rhs;
}

const Complex Integer::operator*(const Complex& rhs){
    return Complex::IntToComplex(*this) * rhs;
}

const Complex Integer::operator/(const Complex& rhs){
    return Complex::IntToComplex(*this) / rhs;
}

void Integer::LoadPrime(){
    std::ifstream fin("primes.txt");
    BaseNum tmp;
    primes.clear();
    while(fin >> tmp){
        primes.push_back(tmp);
    }
    fin.close();
}

Integer Integer::Factorial(Integer rhs){
    Integer result(1,false), one(1,false);
    // end condition
    if(rhs < one){
        return one;
    }
    //
    if(rhs > Integer(primes[primes.size()-1],false)){
        return rhs * Factorial(rhs - one);
    }else{
        BaseNum tmp = 2, tmp2,power, rhsTmp = rhs._digi[SizeMax-1];
        for(int i=1;tmp<=rhsTmp;++i){
            power = 0;
            tmp2 = 1;
            while(true){
                tmp2 = tmp2 * tmp;
                if(tmp2 > rhsTmp)break;
                power = power + rhsTmp/tmp2;
            }
            result = result * Integer(tmp,false).Power(Integer(power,0));
            tmp = primes[i];
        }
    }
    return result;
}