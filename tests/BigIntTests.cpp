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

TEST_F(BigIntTest, IsLesserComparable)
{
    // data preparation:
    constexpr size_t size = 6; 
    std::array<std::string, size> viewLeft = {
        "0",
        "1809274982374918237627129802120320130210301240100000",
        "1809274982374918237627129802120320130210301240100000",
        "-8901001001",
        "18092749823890100100174918890100100123762712980212089010089010010011089000000000000001001001018901001001320130210301240100000",
        "-718437942374632742384324234213412342432432432"
    }, viewRight = {
        "0",
        "1809274982374918237627129802120320130210301240100000",
        "-1809274982374918237627129802120320130210301240100000",
        "-234324328901001001",
        "18292749823890100109994918890100100123762712980212089010089010010011089000000000000001001001018901001001320130210301240100000",
        "7832894234717826381254635412784623746329872395462352347234932423743294324"
    };

    std::array<BigInt, size> lhs, rhs;
    for(size_t i = 0; i < size; i++) {
        lhs[i] = BigInt{ viewLeft[i] };
        rhs[i] = BigInt{ viewRight[i] };
    }

    std::array<bool, size> comparisonResult {
        false,
        false,
        false,
        false,
        true,
        true
    };

    // tests
    for(size_t i = 0; i < size; i++) {
        EXPECT_EQ(lhs[i] < rhs[i], comparisonResult[i])
            <<  lhs[i] << " < " << rhs[i] << " :=> i = " << i;
    }
} 

// TDOO: add other comparison operator tests!

TEST_F(BigIntTest, SubstractSmallerPositiveInteger) 
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        if( lhs[i].IsPositive() && rhs[i].IsPositive() && !(lhs[i] < rhs[i]) ) {
            std::stringstream ss;
            ss << helper::Tests(&lhs[i]).SubstractSmallerPositiveInteger(rhs[i]);
            EXPECT_EQ(ss.str(), m_resultView[Operators::MINUS][i]) << "i = " << i;
        }
    }
}

TEST_F(BigIntTest, SubstractPositiveInteger) 
{
    // data preparation:
    std::array<BigInt, SIZE> lhs, rhs;
    for(size_t i = 0; i < SIZE; i++) {
        lhs[i] = BigInt{ m_op1View[i] };
        rhs[i] = BigInt{ m_op2View[i] };
    }
    // tests
    for(size_t i = 0; i < SIZE; i++) {
        if( lhs[i].IsPositive() && rhs[i].IsPositive()) {
            std::stringstream ss;
            ss << helper::Tests(&lhs[i]).SubstractPositiveInteger(rhs[i]);
            EXPECT_EQ(ss.str(), m_resultView[Operators::MINUS][i]) 
                << m_op1View[i] << " - " <<  m_op2View[i] << " :==> i = " << i;
        }
    }
}

TEST_F(BigIntTest, SubstractWithBigIntOperand)
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

TEST(SimpleMultiplicationTest, CustomMadeTestsPass)
{
    constexpr auto size = 9U;
    std::array<BigInt, size> args = {
        BigInt{"0"},
        BigInt{"-1"},
        BigInt{"1"},
        BigInt{"1000000000000"},
        BigInt{"101010101010"},
        BigInt{"100000000"},
        BigInt{"1000000000"},
        BigInt{"9999"},
        BigInt{"99999"}
    };
    std::array<BigInt, size> res = {
        BigInt{"0"},
        BigInt{"1"},
        BigInt{"1"},
        BigInt{"1000000000000000000000000"},
        BigInt{"10203040506050403020100‬"},
        BigInt{"10000000000000000"},
        BigInt{"1000000000000000000"},
        BigInt{"99980001‬"},
        BigInt{"9999800001"}
    };
    for(size_t i = 0; i < size; i++) {
        EXPECT_EQ(args[i] * args[i], res[i] ) 
            << "\n\t:=> i = " << i;
    }

    EXPECT_EQ(BigInt{"10000"} * BigInt{"100001"}, BigInt{"1000010000"} )
        << " fail multiplication of 100001 * 10000";; 
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

        //EXPECT_EQ(ss.str(), m_resultView[Operators::DIV][i]) << "i = " << i;
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

        //EXPECT_EQ(ss.str(), m_resultView[Operators::MOD][i]) << "i = " << i;
    }
}

/// Zero division exception

/// Negative arguments