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
        FigureRegistry::unregister_shape("square");
        FigureRegistry::unregister_shape("impossible");
        restore_default_shapes();
    }
};

TEST_F(FigureRegistryTest, DefaultShapes_AreRegisteredWithExpectedArgCounts)
{
    const auto *circle = FigureRegistry::find("circle");
    ASSERT_NE(circle, nullptr);
    EXPECT_EQ(circle->arg_counts, (std::vector<std::size_t>{1}));

    const auto *rect = FigureRegistry::find("rectangle");
    ASSERT_NE(rect, nullptr);
    EXPECT_EQ(rect->arg_counts, (std::vector<std::size_t>{2}));

    const auto *tri = FigureRegistry::find("triangle");
    ASSERT_NE(tri, nullptr);
    EXPECT_EQ(tri->arg_counts, (std::vector<std::size_t>{3}));

    EXPECT_EQ(FigureRegistry::find("unknown_shape"), nullptr);
}

TEST_F(FigureRegistryTest, RegisterShape_RejectsInvalidArguments)
{
    auto dummy_creator = [](const std::vector<double> &a)
    {
        return std::make_unique<Circle>(a[0]);
    };

    EXPECT_FALSE(FigureRegistry::register_shape("", {1}, dummy_creator));
    EXPECT_FALSE(FigureRegistry::register_shape("empty_args", {}, dummy_creator));
    EXPECT_FALSE(FigureRegistry::register_shape("null_creator", {1}, nullptr));

    EXPECT_EQ(FigureRegistry::find(""), nullptr);
    EXPECT_EQ(FigureRegistry::find("empty_args"), nullptr);
    EXPECT_EQ(FigureRegistry::find("null_creator"), nullptr);
}

TEST_F(FigureRegistryTest, RegisterAndUnregister_WorksDynamicallyAcrossFactories)
{
    // Register a custom "square" alias at runtime
    ASSERT_TRUE(FigureRegistry::register_shape("square", {1}, [](const std::vector<double> &a)
                                               { return std::make_unique<Rectangle>(a[0], a[0]); }));

    // Works in StringToFigure
    auto sq = StringToFigure::create_from("square 5");
    ASSERT_NE(sq, nullptr);
    EXPECT_EQ(sq->to_string(), "rectangle 5.00 5.00");
    EXPECT_EQ(StringToFigure::create_from("square 5 6"), nullptr);

    // Unregister and verify removal
    EXPECT_TRUE(FigureRegistry::unregister_shape("square"));
    EXPECT_FALSE(FigureRegistry::unregister_shape("square")); // Already removed
    EXPECT_EQ(FigureRegistry::find("square"), nullptr);
    EXPECT_EQ(StringToFigure::create_from("square 5"), nullptr);
}

TEST_F(FigureRegistryTest, UnregisterShape_ExcludesItFromRandomGeneration)
{
    ASSERT_TRUE(FigureRegistry::unregister_shape("triangle"));
    ASSERT_TRUE(FigureRegistry::unregister_shape("rectangle"));

    RandomFactory factory(42, 1.0, 50.0);
    for (int i = 0; i < 30; ++i)
    {
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_EQ(fig->to_string().substr(0, 6), "circle");
    }
}

TEST_F(FigureRegistryTest, EmptyRegistry_RandomFactoryReturnsNullptrCleanly)
{
    FigureRegistry::unregister_shape("circle");
    FigureRegistry::unregister_shape("rectangle");
    FigureRegistry::unregister_shape("triangle");

    RandomFactory factory(42, 1.0, 50.0);
    EXPECT_EQ(factory.create(), nullptr);
}

TEST_F(FigureRegistryTest, UnsatisfiableShape_RandomFactoryHitsRetryCapAndReturnsNullptr)
{
    FigureRegistry::unregister_shape("circle");
    FigureRegistry::unregister_shape("rectangle");
    FigureRegistry::unregister_shape("triangle");

    ASSERT_TRUE(FigureRegistry::register_shape("impossible", {1}, [](const std::vector<double> &) -> std::unique_ptr<Figure>
                                               { return nullptr; }));

    RandomFactory factory(42, 1.0, 50.0);
    EXPECT_EQ(factory.create(), nullptr);
}