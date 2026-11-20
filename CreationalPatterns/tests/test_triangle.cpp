#include <gtest/gtest.h>
#include "../include/Figure.hpp" // Adjust to your actual header filename
#include <stdexcept>

// --- Constructor / Validation Tests ---
TEST(TriangleTest, Constructor_InequalityOnDifferentSides_ThrowsException)
{
    // Side C is too long (a + b <= c)
    EXPECT_THROW(Triangle(2.0, 3.0, 10.0), std::invalid_argument);

    // Side B is too long (a + c <= b)
    EXPECT_THROW(Triangle(2.0, 10.0, 3.0), std::invalid_argument);

    // Side A is too long (b + c <= a)
    EXPECT_THROW(Triangle(10.0, 2.0, 3.0), std::invalid_argument);

    // a + b == c
    EXPECT_THROW(Triangle(2.0, 3.0, 5.0), std::invalid_argument);
}

TEST(TriangleTest, Constructor_ZeroLengthSide_ThrowsException)
{
    EXPECT_THROW(Triangle(0.0, 4.0, 5.0), std::invalid_argument);
    EXPECT_THROW(Triangle(3.0, 0.0, 5.0), std::invalid_argument);
    EXPECT_THROW(Triangle(3.0, 4.0, 0.0), std::invalid_argument);
}

TEST(TriangleTest, Constructor_NegativeSides_ThrowsException)
{
    // ACTION REQUIRED: Uncomment if your constructor validates against negative sides
    EXPECT_THROW(Triangle(-3.0, 4.0, 5.0), std::invalid_argument);
}

// --- Perimeter Tests ---
TEST(TriangleTest, Perimeter_StandardSides_CalculatesCorrectly)
{
    Triangle t(3.0, 4.0, 5.0);
    EXPECT_NEAR(t.perimeter(), 12.0, 0.01);
}

TEST(TriangleTest, Perimeter_DecimalSides_CalculatesCorrectly)
{
    Triangle t(3.14, 4.5, 5.891);
    EXPECT_NEAR(t.perimeter(), 13.531, 0.01);
}

// --- Clone Tests ---
TEST(TriangleTest, Clone_CreatesExactIndependentCopy)
{
    Triangle original(3.0, 4.0, 5.0);
    std::unique_ptr<Triangle> cloned = original.clone();

    // Ensure the pointer is valid
    ASSERT_NE(cloned.get(), nullptr);

    // Ensure it's a new memory allocation (independent object)
    EXPECT_NE(cloned.get(), &original);

    // Ensure the state was copied correctly
    EXPECT_DOUBLE_EQ(cloned->perimeter(), original.perimeter());
    EXPECT_EQ(cloned->to_string(), original.to_string());
}

// --- to_string Tests ---
TEST(TriangleTest, ToString_FormatsSidesToTwoDecimalPlaces)
{
    // Testing with integer, single decimal, and multi-decimal inputs
    Triangle t(3, 4.1, 5.12345);

    // ACTION REQUIRED: Adjust the expected string to match your exact layout.
    // I am assuming a layout like "Triangle(3.00, 4.10, 5.12)" here.
    std::string expected = "triangle 3.00 4.10 5.12";

    EXPECT_EQ(t.to_string(), expected);
}

TEST(TriangleTest, ToString_RoundsCorrectlyToTwoDecimals)
{
    // If your rounding logic rounds .005 up to .01
    Triangle t(3.0051, 4.004, 5.999);

    // Expected behavior: 3.005 -> 3.01, 4.004 -> 4.00, 5.999 -> 6.00
    EXPECT_EQ(t.to_string(), "triangle 3.01 4.00 6.00");
}