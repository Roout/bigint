#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

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

    /// Big integer as one operand
    BigInt& operator + (const BigInt& rhs) {

        return *this;
    }

    BigInt& operator - (const BigInt& rhs) {

        return *this;
    }

    BigInt& operator * (const BigInt& rhs) {

        return *this;
    }

    BigInt& operator / (const BigInt& rhs) {

        return *this;
    }

    // Reminder, NOT modulo! Answer can be negative.
    BigInt& operator % (const BigInt& rhs) {

        return *this;
    }
    
    // Modulo, NOT reminder! Answer >= 0;
    BigInt& Mod (const BigInt& rhs) {

        return *this;
    }

    friend BigInt operator + (const BigInt& lhs, const BigInt& rhs) {
        return BigInt{lhs} + rhs;
    }

    friend BigInt operator - (const BigInt& lhs, const BigInt& rhs) {
        return BigInt{lhs} - rhs;
    }

    friend BigInt operator * (const BigInt& lhs, const BigInt& rhs) {
        return BigInt{lhs} * rhs;
    }

    friend BigInt operator / (const BigInt& lhs, const BigInt& rhs) {
        return BigInt{lhs} / rhs;
    }

    friend BigInt operator % (const BigInt& lhs, const BigInt& rhs) {
        return BigInt{lhs} % rhs;
    }
    /// simple pod type as one operand
    
    /// output
    friend std::ostream& operator<<(std::ostream& os, const BigInt& x) {
        x.Print(os);
        return os;
    }

private:

    // -1'234567890'654323453'346786543'005675432_
    // 1'234567890'654323453'346786543'005675432
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