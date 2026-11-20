#include <gtest/gtest.h>
#include "../include/Factory.hpp"

#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{

    struct TempFile
    {
        std::string path;

        TempFile(std::string filename, const std::string &content)
            : path(std::move(filename))
        {
            std::ofstream out(path);
            out << content;
        }

        ~TempFile()
        {
            std::remove(path.c_str());
        }
    };

} // namespace

// --- Console Factory Creation ---

TEST(AbstractFigureFactoryTest, Create_ConsoleCommand_ReturnsWorkingStreamFactory)
{
    auto console_fact = AbstractFigureFactory::create("console");
    ASSERT_NE(console_fact, nullptr);

    auto padded_fact = AbstractFigureFactory::create("   \tconsole\n  ");
    ASSERT_NE(padded_fact, nullptr);
}

// --- Random Factory Creation ---

TEST(AbstractFigureFactoryTest, Create_RandomDefaultCommand_ProducesValidFigures)
{
    auto random_fact = AbstractFigureFactory::create("random");
    ASSERT_NE(random_fact, nullptr);

    auto padded_fact = AbstractFigureFactory::create("  random   ");
    ASSERT_NE(padded_fact, nullptr);

    for (int i = 0; i < 15; ++i)
    {
        auto fig = random_fact->create();
        ASSERT_NE(fig, nullptr);
        EXPECT_GT(fig->perimeter(), 0.0);
    }
}

TEST(AbstractFigureFactoryTest, Create_RandomWithCustomRange_AppliesBoundsAndSupportsScientificNotation)
{
    auto valid_fact = AbstractFigureFactory::create("random 20.0 30.0");
    ASSERT_NE(valid_fact, nullptr);

    for (int i = 0; i < 25; ++i)
    {
        auto fig = valid_fact->create();
        ASSERT_NE(fig, nullptr);

        std::istringstream iss(fig->to_string());
        std::string type;
        double first_dim = 0.0;
        ASSERT_TRUE(iss >> type >> first_dim);
        EXPECT_GE(first_dim, 19.99);
        EXPECT_LE(first_dim, 30.01);
    }

    auto sci_fact = AbstractFigureFactory::create("  random   1e1   2e1  ");
    ASSERT_NE(sci_fact, nullptr);
    auto fig = sci_fact->create();
    ASSERT_NE(fig, nullptr);
    EXPECT_GT(fig->perimeter(), 0.0);
}

TEST(AbstractFigureFactoryTest, Create_RandomWithInvalidRangeArguments_ReturnsNullptr)
{
    EXPECT_EQ(AbstractFigureFactory::create("random 20.0"), nullptr);      // Missing max
    EXPECT_EQ(AbstractFigureFactory::create("random 20.0   "), nullptr);   // Missing max with trailing spaces
    EXPECT_EQ(AbstractFigureFactory::create("random 30.0 20.0"), nullptr); // min > max
    EXPECT_EQ(AbstractFigureFactory::create("random 15.0 15.0"), nullptr); // min == max
    EXPECT_EQ(AbstractFigureFactory::create("random 0.01 10.0"), nullptr); // min == eps
    EXPECT_EQ(AbstractFigureFactory::create("random 0.0 10.0"), nullptr);  // min <= eps
    EXPECT_EQ(AbstractFigureFactory::create("random -5.0 10.0"), nullptr); // Negative min
    EXPECT_EQ(AbstractFigureFactory::create("random 1.0 inf"), nullptr);   // Infinite max
    EXPECT_EQ(AbstractFigureFactory::create("random -inf 10.0"), nullptr); // Infinite min
    EXPECT_EQ(AbstractFigureFactory::create("random nan 10.0"), nullptr);  // NaN min
    EXPECT_EQ(AbstractFigureFactory::create("random 1.0 nan"), nullptr);   // NaN max
    EXPECT_EQ(AbstractFigureFactory::create("random abc 10.0"), nullptr);  // Non-numeric min
    EXPECT_EQ(AbstractFigureFactory::create("random 1.0 xyz"), nullptr);   // Non-numeric max
}

// --- File Factory Creation ---

TEST(AbstractFigureFactoryTest, Create_ValidFileCommand_CreatesWorkingFileFactory)
{
    TempFile temp("test_abstract_factory.txt",
                  "circle 9.0\n"
                  "invalid_line\n"
                  "rectangle 4.0 5.0\n");

    auto file_fact = AbstractFigureFactory::create("file " + temp.path);
    ASSERT_NE(file_fact, nullptr);

    auto f1 = file_fact->create();
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->to_string(), "circle 9.00");

    EXPECT_EQ(file_fact->create(), nullptr); // Skips "invalid_line"

    auto f2 = file_fact->create();
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->to_string(), "rectangle 4.00 5.00");

    EXPECT_EQ(file_fact->create(), nullptr); // EOF
}

TEST(AbstractFigureFactoryTest, Create_FileCommandWithExtraWhitespaceOrEmptyFile_WorksCleanly)
{
    TempFile empty_temp("test_abstract_empty.txt", "");

    auto file_fact = AbstractFigureFactory::create("   file    " + empty_temp.path + "   ");
    ASSERT_NE(file_fact, nullptr);
    EXPECT_EQ(file_fact->create(), nullptr); // Immediately at EOF
}

// --- Unknown / Malformed Commands ---

TEST(AbstractFigureFactoryTest, Create_InvalidCommandsOrBadFiles_ReturnsNullptr)
{
    EXPECT_EQ(AbstractFigureFactory::create(""), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("   \t\n  "), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("unknown"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("stream"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("RANDOM"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("Random 1 10"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("CONSOLE"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("FILE test.txt"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("file"), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("file   \t  "), nullptr);
    EXPECT_EQ(AbstractFigureFactory::create("file /non/existent/path/shapes_12345.txt"), nullptr);
}

// --- Polymorphic Usage ---

TEST(AbstractFigureFactoryTest, PolymorphicFactories_CanBeStoredAndInvokedUniformly)
{
    TempFile temp("test_abstract_poly.txt", "triangle 3 4 5\n");

    std::vector<std::unique_ptr<FigureFactory>> factories;
    factories.push_back(AbstractFigureFactory::create("file " + temp.path));
    factories.push_back(AbstractFigureFactory::create("random 2.0 8.0"));

    for (const auto &factory : factories)
    {
        ASSERT_NE(factory, nullptr);
        auto fig = factory->create();
        ASSERT_NE(fig, nullptr);
        EXPECT_GT(fig->perimeter(), 0.0);
    }
}