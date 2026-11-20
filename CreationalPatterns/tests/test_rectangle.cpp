#include <gtest/gtest.h>
#include "../include/Figure.hpp" // Adjust to your actual header filename
#include <stdexcept>

// --- Perimeter Tests ---
TEST(RectangleTest, Perimeter_StandardSides_CalculatesCorrectly)
{
    Rectangle r(3.0, 4.0);
    // Perimeter = 2*3 + 2*4 = 14.0
    EXPECT_NEAR(r.perimeter(), 14.0, 0.01);
}

TEST(RectangleTest, Perimeter_DecimalSides_CalculatesCorrectly)
{
    Rectangle r(3.14, 4.5);
    // Perimeter = 2*3.14 + 2*4.5 = 6.28 + 9.0 = 15.28
    EXPECT_NEAR(r.perimeter(), 15.28, 0.01);
}

// --- Clone Tests ---
TEST(RectangleTest, Clone_CreatesExactIndependentCopy)
{
    Rectangle original(3.0, 4.0);
    std::unique_ptr<Rectangle> cloned = original.clone();

    // Ensure the pointer is valid
    ASSERT_NE(cloned.get(), nullptr);

    // Ensure it's a new memory allocation (independent object)
    EXPECT_NE(cloned.get(), &original);

    // Ensure the state was copied correctly
    EXPECT_NEAR(cloned->perimeter(), original.perimeter(), 0.01);
    EXPECT_EQ(cloned->to_string(), original.to_string());
}

// --- to_string Tests ---
TEST(RectangleTest, ToString_FormatsSidesToTwoDecimalPlaces)
{
    // Using values that safely round to avoid floating-point binary issues
    // 3.006 rounds up to 3.01
    // 4.004 rounds down to 4.00
    Rectangle r(3.0051, 4.004);

    // Assumes your output format is "rectangle arg1 arg2"
    EXPECT_EQ(r.to_string(), "rectangle 3.01 4.00");
}

// --- Constructor / Validation Tests ---
TEST(RectangleTest, Constructor_NegativeSides_ThrowsException)
{
    EXPECT_THROW(Rectangle(-3.0, 4.0), std::invalid_argument);
    EXPECT_THROW(Rectangle(3.0, -4.0), std::invalid_argument);
    EXPECT_THROW(Rectangle(-3.0, -4.0), std::invalid_argument);
}

TEST(RectangleTest, Constructor_ZeroLengthSide_ThrowsException)
{
    // A rectangle with a side of length 0 is a line segment, which is invalid for 2D shapes
    EXPECT_THROW(Rectangle(0.0, 4.0), std::invalid_argument);
    EXPECT_THROW(Rectangle(3.0, 0.0), std::invalid_argument);
    EXPECT_THROW(Rectangle(0.0, 0.0), std::invalid_argument);
}