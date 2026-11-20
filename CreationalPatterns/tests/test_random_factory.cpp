#include <gtest/gtest.h>
#include "../include/Factory.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

TEST(RandomFactoryTest, Constructor_ValidParameters_InitializesWithoutThrowing)
{
    EXPECT_NO_THROW(RandomFactory default_args);
    EXPECT_NO_THROW(RandomFactory custom_seed(42));
    EXPECT_NO_THROW(RandomFactory custom_range(42, 0.05, 500.0));
}

TEST(RandomFactoryTest, Constructor_InvalidRanges_ThrowsInvalidArgument)
{
    constexpr double inf = std::numeric_limits<double>::infinity();
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(RandomFactory(42, 0.0, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, 0.01, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, -5.0, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, 10.0, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, 20.0, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, 1.0, inf), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, -inf, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, nan, 10.0), std::invalid_argument);
    EXPECT_THROW(RandomFactory(42, 1.0, nan), std::invalid_argument);
}

TEST(RandomFactoryTest, Create_GeneratesValidRoundTrippableFigures)
{
    RandomFactory factory(42, 1.0, 100.0);

    for (int i = 0; i < 50; ++i)
    {
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_GT(fig->perimeter(), 0.0);

        const std::string serialized = fig->to_string();
        ASSERT_FALSE(serialized.empty());

        auto parsed = StringToFigure::create_from(serialized);
        ASSERT_NE(parsed, nullptr) << "Failed to parse generated figure: " << serialized;
        EXPECT_EQ(parsed->to_string(), serialized);
    }
}

TEST(RandomFactoryTest, Create_SeedDeterminismAndIndependence)
{
    RandomFactory f1(12345, 5.0, 50.0);
    RandomFactory f2(12345, 5.0, 50.0);
    RandomFactory f3(99999, 5.0, 50.0);

    int differences = 0;
    for (int i = 0; i < 20; ++i)
    {
        auto s1 = f1.create();
        auto s2 = f2.create();
        auto s3 = f3.create();

        ASSERT_NE(s1, nullptr);
        ASSERT_NE(s2, nullptr);
        ASSERT_NE(s3, nullptr);

        EXPECT_EQ(s1->to_string(), s2->to_string());

        if (s1->to_string() != s3->to_string())
            ++differences;
    }

    EXPECT_GT(differences, 0);
}

TEST(RandomFactoryTest, Create_ProducesUniformDistributionOfShapeTypes)
{
    constexpr int num_samples = 6000;
    constexpr double expected_count = num_samples / 3.0;
    constexpr double tolerance = expected_count * 0.10;

    RandomFactory factory(42, 1.0, 100.0);
    std::map<std::string, int> counts = {{"triangle", 0}, {"rectangle", 0}, {"circle", 0}};

    for (int i = 0; i < num_samples; ++i)
    {
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);

        std::istringstream iss(fig->to_string());
        std::string type;
        ASSERT_TRUE(iss >> type);
        ASSERT_TRUE(counts.count(type)) << "Unexpected shape type: " << type;
        ++counts[type];
    }

    for (const auto &[type, count] : counts)
    {
        EXPECT_NEAR(count, expected_count, tolerance) << "Skewed count for " << type;
    }
}

TEST(RandomFactoryTest, Create_DimensionsRespectBoundsAndUniformDistribution)
{
    constexpr int num_samples = 6000;
    constexpr double min_val = 10.0;
    constexpr double max_val = 50.0;
    constexpr double format_tol = 0.01;

    RandomFactory factory(12345, min_val, max_val);

    std::vector<double> uniform_dims;
    uniform_dims.reserve(num_samples * 2);

    for (int i = 0; i < num_samples; ++i)
    {
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);

        std::istringstream iss(fig->to_string());
        std::string type;
        ASSERT_TRUE(iss >> type);

        if (type == "circle")
        {
            double r = 0.0;
            ASSERT_TRUE(iss >> r);
            EXPECT_GE(r, min_val - format_tol);
            EXPECT_LE(r, max_val + format_tol);
            uniform_dims.push_back(r);
        }
        else if (type == "rectangle")
        {
            double a = 0.0, b = 0.0;
            ASSERT_TRUE(iss >> a >> b);
            EXPECT_GE(a, min_val - format_tol);
            EXPECT_LE(a, max_val + format_tol);
            EXPECT_GE(b, min_val - format_tol);
            EXPECT_LE(b, max_val + format_tol);
            uniform_dims.push_back(a);
            uniform_dims.push_back(b);
        }
        else if (type == "triangle")
        {
            double a = 0.0, b = 0.0, c = 0.0;
            ASSERT_TRUE(iss >> a >> b >> c);
            EXPECT_GE(a, min_val - format_tol);
            EXPECT_LE(a, max_val + format_tol);
            EXPECT_GE(b, min_val - format_tol);
            EXPECT_LE(b, max_val + format_tol);

            EXPECT_GT(c, std::abs(a - b) - format_tol);
            EXPECT_LT(c, (a + b) + format_tol);
            EXPECT_GT(a + b, c);
            EXPECT_GT(a + c, b);
            EXPECT_GT(b + c, a);

            uniform_dims.push_back(a);
            uniform_dims.push_back(b);
        }
    }

    ASSERT_FALSE(uniform_dims.empty());

    const double sum = std::accumulate(uniform_dims.begin(), uniform_dims.end(), 0.0);
    const double mean = sum / static_cast<double>(uniform_dims.size());
    EXPECT_NEAR(mean, (min_val + max_val) / 2.0, 0.5);

    std::array<int, 4> quartiles = {0, 0, 0, 0};
    const double quartile_width = (max_val - min_val) / 4.0;

    for (double val : uniform_dims)
    {
        int bucket = static_cast<int>((val - min_val) / quartile_width);
        bucket = std::max(0, std::min(3, bucket));
        ++quartiles[bucket];
    }

    const double expected_per_bucket = uniform_dims.size() / 4.0;
    for (int count : quartiles)
    {
        EXPECT_NEAR(count, expected_per_bucket, expected_per_bucket * 0.10);
    }
}