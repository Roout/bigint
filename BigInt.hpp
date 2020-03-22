#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cassert>
#include <string_view>

namespace helper {
    class Tests;
}

class BigInt final {
public:

    BigInt(const std::string& number = "") {
        if( number.empty() ) {
            m_digits.push_back(0);
            m_isPositive = true;
        }
        else {
            this->ParseNonEmptyString(number);
        }
    }

    void operator-() noexcept {
        m_isPositive = static_cast<int>(m_isPositive) ^ 1;
    }

    bool IsPositive() const noexcept {
        return m_isPositive;
    }

    void operator += (const BigInt& rhs) {
        if( m_isPositive && rhs.m_isPositive) {
            this->AddPositiveInteger(rhs);
        }
        else if( !m_isPositive && !rhs.m_isPositive) {
            auto right { rhs };
            -*this, -right;
            this->AddPositiveInteger(right);
            -*this;
        }
        else if(m_isPositive) { // rhs is negative
            // (a + b), where a >= 0, b < 0 =>  
            // a + (-b) = a - b, where a >= 0, b > 0 
            auto right { rhs };
            -right;
            this->SubstractPositiveInteger(right);
        }
        else { // lhs < 0, rhs >= 0
            // (a + b), a < 0, b >= 0 => 
            // (-a + b) = b - a, where a > 0, b >= 0 
            auto right { rhs };
            -*this;
            right.SubstractPositiveInteger(*this);
            *this = std::move(right);
        }
    }

    void operator -= (const BigInt& rhs) {
        if( m_isPositive && rhs.m_isPositive ) {
            this->SubstractPositiveInteger(rhs);
        }
        else if( !m_isPositive && !rhs.m_isPositive ) {
             // (a - b), a < 0, b < 0 => (-a - (-b)) => (-a + b) => b - a
            auto right { rhs };
            -*this, -right;
            right.SubstractPositiveInteger(*this);
            *this = std::move(right);
        }
        else if(m_isPositive) { // rhs is negative
            // (a - b), a >= 0, b < 0 =>  a - (-b) = a + b, where a >= 0, b > 0 
            auto right { rhs };
            -right;
            this->AddPositiveInteger(right);
        }
        else { // lhs < 0, rhs >= 0
            // (a - b), a < 0, b >= 0 => 
            // (-a - b) = -(a + b), where a > 0, b >= 0 
            auto right { rhs };
            -*this;
            this->AddPositiveInteger(right);
            -*this;
        }
    }

    // Time compexity: O(n*n)
    void operator *= (const BigInt& rhs) {
        std::vector<int> res (rhs.m_digits.size() + m_digits.size() + 1, 0U);
        for(size_t i = 0; i < rhs.m_digits.size(); i++) {
            for(size_t j = 0, carry = 0; j < m_digits.size() || carry; j++) {
                const auto cur { 1ULL * rhs.m_digits[i] * (j < m_digits.size()? m_digits[j]: 0LL ) + carry + res[i + j] };
                res[i + j] = cur % RADIX;
                carry = cur / RADIX;
            }
        }
        // remove leading zeroes
        while(res.size() > 1 && !res.back()) {
            res.pop_back();
        }
        // set up sign
        m_isPositive = !static_cast<bool>(m_isPositive ^ rhs.m_isPositive);
        m_digits = std::move(res);
    }

    void operator /= (const BigInt& rhs) {
        *this = this->DivMod(rhs).first;
    }

    // Reminder, NOT modulo! Answer can be negative.
    void operator %= (const BigInt& rhs) {
        *this = this->DivMod(rhs).second;
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
            if( carry ) m_digits[i] -= RADIX;
        }
    }

    /** @brief
     * Expect only positive integers as passed parametr. 
     * Caller object must be also positive.
     * @note 
     * It will modify caller object.
     * Restrictions:
     * - rhs must be smaller or equel *this big integer;
     * - *this >= 0
     * - ths >= 0 
     */
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

    /** @brief
     * Expect only positive integers as passed parametr. 
     * Caller object must be also positive.
     * @note 
     * It will modify caller object  
     */
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
            -copy;
            *this = std::move(copy);
        }
    }

    std::pair<BigInt, BigInt> DivMod(const BigInt& rhs) const {
        BigInt div, mod;
        
        return {div, mod};
    }

    void ParseNonEmptyString(const std::string& number) {
        // TODO: 
        // - add exceptons for parsing, e.g. if first char is letter etc.
        std::string_view sv { number };
        // set up number sign
        m_isPositive = number.front() == '-'? false : true;

        // remove leading and ending non-number characters such as: \t\n etc
        sv.remove_prefix(sv.find_first_of("0123456789"));
        sv.remove_suffix(sv.size() - sv.find_last_of("0123456789") - 1);

        const auto size = static_cast<int>(sv.size());
        // allocate contiguous memory at once 
        m_digits.reserve(size / BASE + 1);
        for (int i = size; i > 0; i -= BASE) {
            if (i <= BASE) {
                const std::string sub(sv.begin(), sv.begin() + i);
                m_digits.emplace_back(std::stoi(sub));
            }
            else {
                const std::string sub(sv.begin() + i - BASE, sv.begin() + i);
                m_digits.emplace_back(std::stoi(sub));
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
    x += rhs;
    return x;
}

BigInt operator- (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    x -= rhs;
    return x;
}

BigInt operator* (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    x *= rhs;
    return x;
}

BigInt operator/ (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    x /= rhs;
    return  x;
}

BigInt operator% (const BigInt& lhs, const BigInt& rhs) {
    auto x { lhs };
    x %= rhs;
    return x;
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
