#include "BigIntTests.hpp"
#include <sstream>


TEST_F(BigIntTest, ConstructWithAllIntegers) 
{
    // data preparation:
    std::array<BigInt, SIZE> integers1, integers2;
    for(size_t i = 0; i < SIZE; i++) {
        integers1[i] = BigInt{ m_op1View[i] };
        integers2[i] = BigInt{ m_op2View[i] };
    }
    // tests
    
    for(size_t i = 0; i < SIZE; i++) {
        {
            std::stringstream ss;
            ss << integers1[i];
            EXPECT_EQ(ss.str(), m_op1View[i]) << "i = " << i;
        }
        {
            std::stringstream ss;
            ss << integers2[i];
            EXPECT_EQ(ss.str(), m_op2View[i]) << "i = " << i;
        }
        
    }
}

TEST_F(BigIntTest, AddPositiveIntegers) 
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        if( lhs[i].IsPositive() && rhs[i].IsPositive() ) {
            std::stringstream ss;
            ss << helper::Tests(&lhs[i]).AddPositiveInteger(rhs[i]);
            EXPECT_EQ(ss.str(), m_resultView[Operators::PLUS][i]) << "i = " << i;
        }
    }
}

TEST_F(BigIntTest, SubstructPositiveIntegers) 
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        if( lhs[i].IsPositive() && rhs[i].IsPositive() ) {
            std::stringstream ss;
            ss << helper::Tests(&lhs[i]).SubstructPositiveInteger(rhs[i]);
            EXPECT_EQ(ss.str(), m_resultView[Operators::MINUS][i]) << "i = " << i;
        }
    }
}

TEST_F(BigIntTest, MultiplyWithBigIntOperand)
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        std::stringstream ss;
        ss << lhs[i] * rhs[i];

        EXPECT_EQ(ss.str(), m_resultView[Operators::MULT][i]) << "i = " << i;
    }
}

TEST_F(BigIntTest, SubtractWithBigIntOperand)
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        std::stringstream ss;
        ss << lhs[i] - rhs[i];

        EXPECT_EQ(ss.str(), m_resultView[Operators::MINUS][i]) << "i = " << i;
    }
}

TEST_F(BigIntTest, DivideWithBigIntOperand)
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        std::stringstream ss;
        ss << lhs[i] / rhs[i];

        EXPECT_EQ(ss.str(), m_resultView[Operators::DIV][i]) << "i = " << i;
    }
}

TEST_F(BigIntTest, AddWithBigIntOperand)
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        std::stringstream ss;
        ss << lhs[i] + rhs[i];

        EXPECT_EQ(ss.str(), m_resultView[Operators::PLUS][i]) << "i = " << i;
    }
}

// in fact, it's remainder i.e. works as operator% for buildin integers
TEST_F(BigIntTest, ModWithBigIntOperand)
{
   // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        std::stringstream ss;
        ss << lhs[i] % rhs[i];

        EXPECT_EQ(ss.str(), m_resultView[Operators::MOD][i]) << "i = " << i;
    }
}

/// Zero division exception

/// Negative arguments