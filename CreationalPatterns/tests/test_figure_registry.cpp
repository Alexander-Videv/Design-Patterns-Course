#include <gtest/gtest.h>
#include "../include/Factory.hpp"
#include "../include/FigureRegistry.hpp"

#include <memory>
#include <vector>

namespace
{
    void restore_default_shapes()
    {
        Circle::register_shape();
        Rectangle::register_shape();
        Triangle::register_shape();
    }
} // namespace

class FigureRegistryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        restore_default_shapes();
    }

    void TearDown() override
    {
        TextFigureRegistry::unregister_shape({"square", 1});
        RandomFigureRegistry::unregister_shape("square");
        restore_default_shapes();
    }
};

TEST_F(FigureRegistryTest, DefaultShapes_AreRegisteredAtStartup)
{
    EXPECT_NE(TextFigureRegistry::find({"circle", 1}), nullptr);
    EXPECT_NE(TextFigureRegistry::find({"rectangle", 2}), nullptr);
    EXPECT_NE(TextFigureRegistry::find({"triangle", 3}), nullptr);

    EXPECT_NE(RandomFigureRegistry::find("circle"), nullptr);
    EXPECT_NE(RandomFigureRegistry::find("rectangle"), nullptr);
    EXPECT_NE(RandomFigureRegistry::find("triangle"), nullptr);
}

TEST_F(FigureRegistryTest, RegisterShape_RejectsNullCreator)
{
    EXPECT_FALSE(TextFigureRegistry::register_shape({"null_shape", 1}, nullptr));
    EXPECT_EQ(TextFigureRegistry::find({"null_shape", 1}), nullptr);

    EXPECT_FALSE(RandomFigureRegistry::register_shape("null_shape", nullptr));
    EXPECT_EQ(RandomFigureRegistry::find("null_shape"), nullptr);
}

TEST_F(FigureRegistryTest, RegisterAndUnregister_WorksDynamicallyAtRuntime)
{
    // Register a custom "square" alias on the fly
    ASSERT_TRUE(TextFigureRegistry::register_shape({"square", 1}, [](const std::vector<double> &a)
                                                   { return std::make_unique<Rectangle>(a[0], a[0]); }));

    auto sq = StringToFigure::create_from("square 5");
    ASSERT_NE(sq, nullptr);
    EXPECT_EQ(sq->to_string(), "rectangle 5.00 5.00");

    // Unregister it and verify it is no longer recognized
    EXPECT_TRUE(TextFigureRegistry::unregister_shape({"square", 1}));
    EXPECT_FALSE(TextFigureRegistry::unregister_shape({"square", 1})); // Already removed
    EXPECT_EQ(StringToFigure::create_from("square 5"), nullptr);
}

TEST_F(FigureRegistryTest, UnregisterFromRandomRegistry_ExcludesShapeFromGeneration)
{
    ASSERT_TRUE(RandomFigureRegistry::unregister_shape("triangle"));
    ASSERT_TRUE(RandomFigureRegistry::unregister_shape("rectangle"));

    RandomFactory factory(42, 1.0, 50.0);
    for (int i = 0; i < 30; ++i)
    {
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_EQ(fig->to_string().substr(0, 6), "circle");
    }
}

TEST_F(FigureRegistryTest, EmptyRandomRegistry_ReturnsNullptrCleanly)
{
    RandomFigureRegistry::unregister_shape("circle");
    RandomFigureRegistry::unregister_shape("rectangle");
    RandomFigureRegistry::unregister_shape("triangle");

    RandomFactory factory(42, 1.0, 50.0);
    EXPECT_EQ(factory.create(), nullptr);
}