#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cassert>
#include <string_view>
#include <algorithm>

namespace helper {
    class Tests;
}

class BigInt final {
public:

    BigInt(const std::string& number = "") {
        if( number.empty() ) {
            m_coefficients.push_back(0);
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
        while(m_coefficients.size() > 1u && !m_coefficients.back()) {
            m_coefficients.pop_back();
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
        while(m_coefficients.size() > 1u && !m_coefficients.back()) {
            m_coefficients.pop_back();
        }
    }

    // Time compexity: O(n*n)
    void operator *= (const BigInt& rhs) {
        std::vector<int> res (rhs.m_coefficients.size() + m_coefficients.size() + 1, 0U);
        for(size_t i = 0; i < rhs.m_coefficients.size(); i++) {
            for(size_t j = 0, carry = 0; j < m_coefficients.size() || carry; j++) {
                const auto cur { 1ULL * rhs.m_coefficients[i] * (j < m_coefficients.size()? m_coefficients[j]: 0LL ) + carry + res[i + j] };
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
        m_coefficients = std::move(res);
    }

    void operator /= (const BigInt& rhs) {
        *this = this->DivMod(rhs).first;
    }

    // Reminder, NOT modulo! Answer can be negative.
    void operator %= (const BigInt& rhs) {
        *this = this->DivMod(rhs).second;
    }

    // Time complexity: O(n^(1.585))
    friend BigInt PositiveKaratsubaMultiplication(const BigInt& lhs, const BigInt& rhs);
    friend BigInt KaratsubaMultiplication(const BigInt& lhs, const BigInt& rhs);
    
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

    /**
     * Works like binary >> (just pop_front coefficient) 
     */
    BigInt ShiftRight(size_t rank) const {
        BigInt copy {};
        copy.m_isPositive = this->m_isPositive;
        auto currentRank { m_coefficients.size() };
        if (currentRank > rank) {
            copy.m_coefficients.resize(currentRank - rank);
            std::copy(m_coefficients.cbegin() + rank, m_coefficients.cend(), copy.m_coefficients.begin());
        }
        return copy;
    }

    /**
     * Works like binary << (just push_front zero coefficients) 
     */
    BigInt ShiftLeft(size_t rank) const {
        BigInt copy {};
        copy.m_isPositive = this->m_isPositive;
        if(!m_coefficients.empty()) {
            copy.m_coefficients.resize(m_coefficients.size() + rank);
            std::copy(m_coefficients.cbegin(), m_coefficients.cend(), copy.m_coefficients.begin() + rank);
        }
        return copy;       
    }

    /**
     * Just pop_back coefficients 
     * (these are highest rank one so the calue will become smaller)
     */
    BigInt CutOffRank(size_t rank) const {
        BigInt copy {};
        copy.m_isPositive = this->m_isPositive;
        auto currentRank { m_coefficients.size() };
        if (currentRank > rank) {
            copy.m_coefficients.resize(currentRank - rank);
            std::copy(m_coefficients.cbegin(), m_coefficients.cbegin() + currentRank - rank, copy.m_coefficients.begin());
            // remove leading zeros:
            while(copy.m_coefficients.size() > 1 && copy.m_coefficients.back() == 0) {
                copy.m_coefficients.pop_back();
            }
        }
        return copy;
    }

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
        
        const auto size = std::max(m_coefficients.size(), rhs.m_coefficients.size());

        auto carry { 0 };
        for(size_t i = 0; i < size || carry; i++) {
            if ( i == m_coefficients.size() ) { 
                m_coefficients.push_back( 0 );
            }
            m_coefficients[i] += carry + (i < rhs.m_coefficients.size()? rhs.m_coefficients[i] : 0);
            carry = m_coefficients[i] >= RADIX;
            if( carry ) m_coefficients[i] -= RADIX;
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
        
        // m_coefficients.size() >= rhs.m_coefficients.size() due to restrictions
        for(size_t i = 0; i < m_coefficients.size(); i++) {
            if( i < rhs.m_coefficients.size() ) {
                m_coefficients[i] -= rhs.m_coefficients[i];
            }
            if ( m_coefficients[i] < 0) {
                m_coefficients[i] += RADIX;
                m_coefficients[i + 1]--;
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
        // TODO: add exceptons for parsing, e.g. if first char is letter etc.
        std::string_view sv { number };
        // set up number sign
        m_isPositive = number.front() == '-'? false : true;

        // remove leading and ending non-number characters such as: \t\n etc
        sv.remove_prefix(sv.find_first_of("0123456789"));
        sv.remove_suffix(sv.size() - sv.find_last_of("0123456789") - 1);

        const auto size = static_cast<int>(sv.size());
        // reserve contiguous memory at once 
        m_coefficients.reserve(size / DIGIT_COUNT + 1);
        for (int i = size; i > 0; i -= DIGIT_COUNT) {
            if (i <= DIGIT_COUNT) {
                const std::string sub(sv.begin(), sv.begin() + i);
                m_coefficients.emplace_back(std::stoi(sub));
            }
            else {
                const std::string sub(sv.begin() + i - DIGIT_COUNT, sv.begin() + i);
                m_coefficients.emplace_back(std::stoi(sub));
            }
        }
    }

    void Print(std::ostream& os) const {
        for(auto cell = m_coefficients.crbegin(); cell != m_coefficients.crend(); ++cell) {
            if (cell == m_coefficients.crbegin()) {
                // last number to be printed without leading zeros
                // but with minus if it's negative
                if( !m_isPositive )  os << '-';
                os << *cell;
            }
            else {
                os.width(DIGIT_COUNT);
                os.fill('0');
                os << std::right << *cell;
            }
        }
    }

private:

    static constexpr int DIGIT_COUNT = 9; // max number of digits in one cell
    static constexpr int RADIX = 1'000'000'000;
    
    // Contains coefficients; from left to right starting from 0..
    // N = m_coefficients[0] * RADIX ^ 0 + m_coefficients[1] * RADIX^1 + ... .
    std::vector<int> m_coefficients;
    bool m_isPositive;
};

BigInt PositiveKaratsubaMultiplication(const BigInt& lhs, const BigInt& rhs) {
    /**
     * Algorithm is fairy simple:
     * A = ax + b
     * B = cx + d
     * A * B = (ax + b)(cx + d) = acxx + x(ad + cb) + bd 
     * AND
     * (ad + cb) = (a + b)(c + d) - ac - bd
     * SO
     * A * B = ac * xx + x * ((a + b)(c + d) - ac - bd) + bd
    */
    auto degree = std::max(lhs.m_coefficients.size(), rhs.m_coefficients.size());
    if(degree <= 1u) return lhs * rhs;

    degree = (degree&1u) + (degree >> 1u);
    // Split lhs and rhs into 2 equal parts:

    // works like binary shift operator >> (pop_front coefficient)
    auto a = lhs.ShiftRight(degree);
    // cut off rank from right to left (from highest to lowest) (pop_back coefficients)
    // min(size(), size() - degree)
    auto b = lhs.CutOffRank(lhs.m_coefficients.size() > degree? lhs.m_coefficients.size() - degree: 0u); 
    // works like binary shift operator >>
    auto c = rhs.ShiftRight(degree); 
    // cut off rank from right to left (from highest to lowest)
    auto d = rhs.CutOffRank(rhs.m_coefficients.size() > degree? rhs.m_coefficients.size() - degree: 0u); 

    // Compute the subproblems:
    auto ac = PositiveKaratsubaMultiplication(a, c);
    auto bd = PositiveKaratsubaMultiplication(b, d);
    auto abcd = PositiveKaratsubaMultiplication(a + b, c + d);
    return (abcd - ac - bd).ShiftLeft(degree) + ac.ShiftLeft(degree << 1u) + bd; 
}

BigInt KaratsubaMultiplication(const BigInt& lhs, const BigInt& rhs) { 
    BigInt result = PositiveKaratsubaMultiplication(lhs, rhs);
    result.m_isPositive = rhs.m_isPositive == lhs.m_isPositive;
    return result;
}

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
    const auto lSize { static_cast<int>(lhs.m_coefficients.size()) };
    const auto rSize { static_cast<int>(rhs.m_coefficients.size()) };
    
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
            if( lhs.m_coefficients[i] > rhs.m_coefficients[i] ) {
                isLesser = (lhs.m_isPositive && rhs.m_isPositive)? false: true;
                break;
            } 
            else if( lhs.m_coefficients[i] < rhs.m_coefficients[i] ) {
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

    const auto lSize { lhs.m_coefficients.size() };
    const auto rSize { rhs.m_coefficients.size() };
    
    if( lhs.m_isPositive != rhs.m_isPositive ||
        lSize != rSize 
    ) {
        isEquel = false;
    }

    for(size_t i = 0; i < lSize && isEquel; i++) {
        if( lhs.m_coefficients[i] != rhs.m_coefficients[i] ) {
            isEquel = false;
        }
    }

    return isEquel;
}

std::ostream& operator<<(std::ostream& os, const BigInt& x) {
    x.Print(os);
    return os;
}
