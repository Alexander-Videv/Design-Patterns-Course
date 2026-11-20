#include <gtest/gtest.h>
#include "../include/Factory.hpp"

const bool registered = register_shapes();

TEST(StringToFigureTest, CreateFrom_ValidInputs_ReturnsExpectedFigures)
{
    auto t = StringToFigure::create_from("triangle 3 4 5");
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(t->to_string(), "triangle 3.00 4.00 5.00");

    auto r = StringToFigure::create_from("  rectangle   6.5   2.0  ");
    ASSERT_NE(r, nullptr);
    EXPECT_EQ(r->to_string(), "rectangle 6.50 2.00");

    auto c = StringToFigure::create_from("circle 3.14");
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->to_string(), "circle 3.14");
}

TEST(StringToFigureTest, CreateFrom_InvalidInputs_ReturnsNullptr)
{
    EXPECT_EQ(StringToFigure::create_from(""), nullptr);
    EXPECT_EQ(StringToFigure::create_from("    "), nullptr);
    EXPECT_EQ(StringToFigure::create_from("\t\n"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("square 4"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("TRIANGLE 3 4 5"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("Rectangle 3 4"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("CIRCLE 5"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("triangle 1 2 10"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("triangle 3 4"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("triangle 3 4 5 6"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("rectangle -2 5"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("rectangle 2"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("rectangle 2 3 4"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle 0"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle -3"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle 5 10"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle 5 abc"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle abc"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("circle"), nullptr);
}