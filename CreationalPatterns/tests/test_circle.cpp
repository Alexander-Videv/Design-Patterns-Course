#include <gtest/gtest.h>
#include "../include/Figure.hpp"
#include <stdexcept>
#include <cmath>

// --- Perimeter Tests ---
TEST(CircleTest, Perimeter_StandardRadius_CalculatesCorrectly)
{
    Circle c(3.0);
    // Perimeter (circumference) = 2 * pi * radius
    double expected = 2.0 * M_PI * 3.0;
    EXPECT_NEAR(c.perimeter(), expected, 0.01);
}

TEST(CircleTest, Perimeter_DecimalRadius_CalculatesCorrectly)
{
    Circle c(4.5);
    double expected = 2.0 * M_PI * 4.5;
    EXPECT_NEAR(c.perimeter(), expected, 0.01);
}

// --- Clone Tests ---
TEST(CircleTest, Clone_CreatesExactIndependentCopy)
{
    Circle original(3.0);
    std::unique_ptr<Circle> cloned = original.clone();

    // Ensure the pointer is valid
    ASSERT_NE(cloned.get(), nullptr);

    // Ensure it's a new memory allocation (independent object)
    EXPECT_NE(cloned.get(), &original);

    // Ensure the state was copied correctly
    EXPECT_NEAR(cloned->perimeter(), original.perimeter(), 0.01);
    EXPECT_EQ(cloned->to_string(), original.to_string());
}

// --- to_string Tests ---
TEST(CircleTest, ToString_FormatsRadiusToTwoDecimalPlaces)
{
    // 3.006 safely rounds up to 3.01
    Circle c(3.006);

    // Assumes your output format is "circle arg1"
    EXPECT_EQ(c.to_string(), "circle 3.01");
}

TEST(CircleTest, ToString_TrailingZerosAreKept)
{
    // 4.0 should format to 4.00
    Circle c(4.0);
    EXPECT_EQ(c.to_string(), "circle 4.00");
}

// --- Constructor / Validation Tests ---
TEST(CircleTest, Constructor_NegativeRadius_ThrowsException)
{
    EXPECT_THROW(Circle(-3.0), std::invalid_argument);
}

TEST(CircleTest, Constructor_ZeroRadius_ThrowsException)
{
    // A circle with a radius of 0 is just a point, invalid as a 2D figure
    EXPECT_THROW(Circle(0.0), std::invalid_argument);
}