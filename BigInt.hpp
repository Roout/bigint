#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cassert>

namespace helper {
    class Tests;
}

class BigInt final {
public:
    BigInt():
        m_digits {0},
        m_isPositive{ true } 
    { // def ctor
    }

    BigInt(const std::string& number) {
        if( number.empty() ) {
            m_digits.push_back(0);
        }
        else {
            this->ParseNonEmptyString(number);
        }
    }

    // inverse the sign: + to -, - to +;
    // i.e., same as multiply it by -1.
    const BigInt& Negate() {
        m_isPositive = static_cast<int>(m_isPositive) ^ 1;
        return *this;
    }

    bool IsPositive() const {
        return m_isPositive;
    }

    const BigInt& operator + (const BigInt& rhs) {
        if( m_isPositive && rhs.m_isPositive) {
            this->AddPositiveInteger(rhs);
            return *this;
        }
        else if( !m_isPositive && !rhs.m_isPositive) {
            auto right { rhs };
            this->Negate(), right.Negate();
            this->AddPositiveInteger(right);
            this->Negate();
            return *this;
        }
        else if(m_isPositive) { // rhs is negative
            // (a + b), where a >= 0, b < 0 =>  
            // a + (-b) = a - b, where a >= 0, b > 0 
            auto right { rhs };
            right.Negate();
            this->SubstractPositiveInteger(right);
            return *this;
        }
        else { // lhs < 0, rhs >= 0
            // (a + b), a < 0, b >= 0 => 
            // (-a + b) = b - a, where a > 0, b >= 0 
            auto right { rhs };
            this->Negate();
            right.SubstractPositiveInteger(*this);
            *this = std::move(right);
            return *this;
        }
    }

    const BigInt& operator - (const BigInt& rhs) {
        if( m_isPositive && rhs.m_isPositive ) {
            this->SubstractPositiveInteger(rhs);
            return *this;
        }
        else if( !m_isPositive && !rhs.m_isPositive ) {
             // (a - b), a < 0, b < 0 => (-a - (-b)) => (-a + b) => b - a
            auto right { rhs };
            this->Negate(), right.Negate();
            right.SubstractPositiveInteger(*this);
            *this = std::move(right);
            return *this;
        }
        else if(m_isPositive) { // rhs is negative
            // (a - b), a >= 0, b < 0 =>  a - (-b) = a + b, where a >= 0, b > 0 
            auto right { rhs };
            right.Negate();
            this->AddPositiveInteger(right);
            return *this;
        }
        else { // lhs < 0, rhs >= 0
            // (a - b), a < 0, b >= 0 => 
            // (-a - b) = -(a + b), where a > 0, b >= 0 
            auto right { rhs };
            this->Negate();
            this->AddPositiveInteger(right);
            this->Negate();
            return *this;
        }
    }

    const BigInt& operator * (const BigInt& rhs) {

        return rhs;
    }

    const BigInt& operator / (const BigInt& rhs) {

        return rhs;
    }

    // Reminder, NOT modulo! Answer can be negative.
    const BigInt& operator % (const BigInt& rhs) {

        return rhs;
    }
    
    // Modulo, NOT reminder! Answer >= 0;
    const BigInt& Mod (const BigInt& rhs) {

        return rhs;
    }

    friend BigInt operator+ (const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator- (const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator* (const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator/ (const BigInt& lhs, const BigInt& rhs);
    friend BigInt operator% (const BigInt& lhs, const BigInt& rhs);

    friend bool operator<   (const BigInt& lhs, const BigInt& rhs);
    friend bool operator>   (const BigInt& lhs, const BigInt& rhs);
    friend bool operator!=  (const BigInt& lhs, const BigInt& rhs);
    friend bool operator==  (const BigInt& lhs, const BigInt& rhs);

    /// simple pod type as one operand

    friend std::ostream& operator<<(std::ostream& os, const BigInt& x);

private:

    friend class helper::Tests;

     /** @brief
     * Expect only positive integers as passed parametr. 
     * Caller object must be also positive.
     * @note 
     * It will modify caller object  
     */
    void AddPositiveInteger(
        const BigInt& rhs
    ) {
        assert(m_isPositive && rhs.m_isPositive);
        
        const auto size = std::max(m_digits.size(), rhs.m_digits.size());

        auto carry { 0 };
        for(size_t i = 0; i < size || carry; i++) {
            if ( i == m_digits.size() ) { 
                m_digits.push_back( 0 );
            }
            m_digits[i] += carry + (i < rhs.m_digits.size()? rhs.m_digits[i] : 0);
            carry = m_digits[i] >= RADIX;
            m_digits[i] %= RADIX;
        }
    }


    // TODO: change isPositive to isNegative: 
    //       zero isn't positive but I use it!
    // RESTRICTION: 
    // - rhs must be smaller or equel *this big integer;
    // - *this >= 0
    // - ths >= 0 
    void SubstractSmallerPositiveInteger(
        const BigInt& rhs
    ) {
        assert(m_isPositive && rhs.m_isPositive && !(*this < rhs));
        
        // m_digits.size() >= rhs.m_digits.size() due to restrictions
        for(size_t i = 0; i < m_digits.size(); i++) {
            if( i < rhs.m_digits.size() ) {
                m_digits[i] -= rhs.m_digits[i];
            }
            if ( m_digits[i] < 0) {
                m_digits[i] += RADIX;
                m_digits[i + 1]--;
            }
        }
    }

    void SubstractPositiveInteger(
        const BigInt& rhs
    ) {
        assert(m_isPositive && rhs.m_isPositive);
        
        if (!(*this < rhs)) {
            this->SubstractSmallerPositiveInteger(rhs);
        }
        else {
            auto copy { rhs };
            copy.SubstractSmallerPositiveInteger(*this);
            copy.Negate();
            std::swap(copy, *this);
        }
    }

    void ParseNonEmptyString(const std::string& number) {
        // TODO: 
        // - add exceptons for parsing, e.g. if first char is letter etc.

        // ignore sign if exist
        const int lastLeftDigit = isdigit(number.front()) ? 0 : 1;
        // set up number sign
        m_isPositive = number.front() == '-'? false : true;

        const auto size = static_cast<int>(number.length());
        // allocate contiguous memory at once 
        m_digits.reserve(size / BASE + 1);
        for (int i = size; i > lastLeftDigit; i -= BASE) {
            if (i <= BASE) {
                m_digits.emplace_back(stoi(number.substr(lastLeftDigit, i - lastLeftDigit)));
            }
            else {
                m_digits.emplace_back(stoi(number.substr(i - BASE, BASE)));
            }
        }
    }

    void Print(std::ostream& os) const {
        for(auto cell = m_digits.crbegin(); cell != m_digits.crend(); ++cell ) {
            if( cell == m_digits.crbegin()) {
                // last number to be printed without leading zeros
                // but with minus if it's negative
                if( !m_isPositive )  os << '-';
                os << *cell;
            }
            else {
                os.width(BASE);
                os.fill('0');
                os << std::right << *cell;
            }
        }
    }

private:

    static constexpr int BASE = 9; 
    static constexpr int RADIX = 1'000'000'000;
    
    std::vector<int> m_digits;
    bool m_isPositive;
};


BigInt operator+ (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    return x + rhs;
}

BigInt operator- (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    return x - rhs;
}

BigInt operator* (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    return x * rhs;
}

BigInt operator/ (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    return x / rhs;
}

BigInt operator% (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    return x % rhs;
}

bool operator< (const BigInt& lhs, const BigInt& rhs) {
    bool isLesser { false };
    // positive always greater negative
    if ( lhs.m_isPositive && !rhs.m_isPositive ) {
        return false;
    }
    else if ( !lhs.m_isPositive && rhs.m_isPositive ) {
        return true;
    }

    // reacheable for only positive or negative integers
    const auto lSize { static_cast<int>(lhs.m_digits.size()) };
    const auto rSize { static_cast<int>(rhs.m_digits.size()) };
    
    // number with higher number of digits is greater
    if ( lSize > rSize ) {
        isLesser = (lhs.m_isPositive && rhs.m_isPositive)? false: true;
    }
    else if(lSize < rSize) {
        isLesser = (lhs.m_isPositive && rhs.m_isPositive)? true: false;
    }
    else if ( lSize == rSize ) { 
        isLesser = false; // assume that integers are equel by default
        // Go from the highest digit number to lowest: 
        for(int i = lSize - 1; i >= 0; i-- ) {
            if( lhs.m_digits[i] > rhs.m_digits[i] ) {
                isLesser = (lhs.m_isPositive && rhs.m_isPositive)? false: true;
                break;
            } 
            else if( lhs.m_digits[i] < rhs.m_digits[i] ) {
                isLesser = (lhs.m_isPositive && rhs.m_isPositive)? true: false;
                break;
            }
        }
    }
    return isLesser;
}

bool operator> (const BigInt& lhs, const BigInt& rhs) {
    return !(lhs < rhs) && lhs != rhs;
}

bool operator!= (const BigInt& lhs, const BigInt& rhs) {
    return !(lhs == rhs);
}

bool operator== (const BigInt& lhs, const BigInt& rhs) {
    bool isEquel { true };

    const auto lSize { lhs.m_digits.size() };
    const auto rSize { rhs.m_digits.size() };
    
    if( lhs.m_isPositive != rhs.m_isPositive ||
        lSize != rSize 
    ) {
        isEquel = false;
    }

    for(size_t i = 0; i < lSize && isEquel; i++) {
        if( lhs.m_digits[i] != rhs.m_digits[i] ) {
            isEquel = false;
        }
    }

    return isEquel;
}

std::ostream& operator<<(std::ostream& os, const BigInt& x) {
    x.Print(os);
    return os;
}
