#include <gtest/gtest.h>
#include "../include/Factory.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

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

// --- Constructor Tests ---

TEST(StreamFigureFactoryTest, Constructor_BadIstream_ThrowsInvalidArgument)
{
    std::istringstream bad_stream;
    bad_stream.setstate(std::ios::failbit);

    EXPECT_THROW(StreamFigureFactory factory(bad_stream), std::invalid_argument);
}

TEST(StreamFigureFactoryTest, Constructor_UnopenedMovedIfstream_ThrowsInvalidArgument)
{
    std::ifstream unopened_file("non_existent_file_xyz_987.txt");
    EXPECT_THROW(StreamFigureFactory factory(std::move(unopened_file)), std::invalid_argument);
}

TEST(StreamFigureFactoryTest, Constructor_ValidMovedIfstream_ReadsSuccessfully)
{
    TempFile temp("test_stream_factory_move.txt",
                  "triangle 3 4 5\n"
                  "circle 2.5\n");

    std::ifstream in(temp.path);
    ASSERT_TRUE(in.is_open());

    StreamFigureFactory factory(std::move(in));

    auto f1 = factory.create();
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->to_string(), "triangle 3.00 4.00 5.00");

    auto f2 = factory.create();
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->to_string(), "circle 2.50");

    EXPECT_EQ(factory.create(), nullptr);
}

// --- Positive Creation Tests ---

TEST(StreamFigureFactoryTest, Create_ValidSingleShapes_ReturnsExpectedFigures)
{
    {
        std::istringstream iss("triangle 3.0 4.0 5.0");
        StreamFigureFactory factory(iss);
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_EQ(fig->to_string(), "triangle 3.00 4.00 5.00");
        EXPECT_NEAR(fig->perimeter(), 12.0, 0.01);
    }
    {
        std::istringstream iss("rectangle 3.5 4.5");
        StreamFigureFactory factory(iss);
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_EQ(fig->to_string(), "rectangle 3.50 4.50");
        EXPECT_NEAR(fig->perimeter(), 16.0, 0.01);
    }
    {
        std::istringstream iss("circle 10.0");
        StreamFigureFactory factory(iss);
        auto fig = factory.create();
        ASSERT_NE(fig, nullptr);
        EXPECT_EQ(fig->to_string(), "circle 10.00");
    }
}

TEST(StreamFigureFactoryTest, Create_MultipleShapesAndBlankLines_ReadsSequentiallyUntilEOF)
{
    std::istringstream iss(
        "\n"
        "   circle 44   \n"
        "\n"
        "rectangle 18 19\n"
        "triangle 5 12 13\n"
        "\n");
    StreamFigureFactory factory(iss);

    EXPECT_EQ(factory.create(), nullptr); // Leading blank line

    auto f1 = factory.create();
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->to_string(), "circle 44.00");

    EXPECT_EQ(factory.create(), nullptr); // Middle blank line

    auto f2 = factory.create();
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->to_string(), "rectangle 18.00 19.00");

    auto f3 = factory.create();
    ASSERT_NE(f3, nullptr);
    EXPECT_EQ(f3->to_string(), "triangle 5.00 12.00 13.00");

    EXPECT_EQ(factory.create(), nullptr); // Trailing blank line
    EXPECT_EQ(factory.create(), nullptr); // EOF
}

// --- Negative & Recovery Tests ---

TEST(StreamFigureFactoryTest, Create_EmptyStream_ReturnsNullptr)
{
    std::istringstream iss("");
    StreamFigureFactory factory(iss);

    EXPECT_EQ(factory.create(), nullptr);
}

TEST(StreamFigureFactoryTest, Create_UnknownShapeKeyword_SkipsRestOfLineAndContinues)
{
    std::istringstream iss(
        "tringl tringl lil star\n"
        "hexagon 1 2 3 4 5 6\n"
        "circle 32\n"
        "skibidi.\n");
    StreamFigureFactory factory(iss);

    EXPECT_EQ(factory.create(), nullptr);
    EXPECT_EQ(factory.create(), nullptr);

    auto valid = factory.create();
    ASSERT_NE(valid, nullptr);
    EXPECT_EQ(valid->to_string(), "circle 32.00");

    EXPECT_EQ(factory.create(), nullptr);
    EXPECT_EQ(factory.create(), nullptr); // EOF
}

TEST(StreamFigureFactoryTest, Create_InvalidGeometryParameters_ReturnsNullptrAndRecovers)
{
    std::istringstream iss(
        "triangle 1 2 10\n"
        "rectangle -4 5\n"
        "circle 0\n"
        "rectangle 2 3\n");
    StreamFigureFactory factory(iss);

    EXPECT_EQ(factory.create(), nullptr);
    EXPECT_EQ(factory.create(), nullptr);
    EXPECT_EQ(factory.create(), nullptr);

    auto valid = factory.create();
    ASSERT_NE(valid, nullptr);
    EXPECT_EQ(valid->to_string(), "rectangle 2.00 3.00");
}

TEST(StreamFigureFactoryTest, Create_MissingOrNonNumericArguments_RecoversNextLine)
{
    std::istringstream iss(
        "triangle 3 4\n"
        "rectangle 18 19\n"
        "circle not_a_number\n"
        "circle 7\n");
    StreamFigureFactory factory(iss);

    EXPECT_EQ(factory.create(), nullptr);

    auto rect = factory.create();
    ASSERT_NE(rect, nullptr);
    EXPECT_EQ(rect->to_string(), "rectangle 18.00 19.00");

    EXPECT_EQ(factory.create(), nullptr);

    auto circ = factory.create();
    ASSERT_NE(circ, nullptr);
    EXPECT_EQ(circ->to_string(), "circle 7.00");

    EXPECT_EQ(factory.create(), nullptr); // EOF
}

TEST(StreamFigureFactoryTest, Create_ExtraTrailingTokens_ConsumesWholeLineAndMovesOn)
{
    std::istringstream iss(
        "rectangle 18 18 18 18 18\n"
        "circle 5\n");
    StreamFigureFactory factory(iss);

    auto rect = factory.create();
    ASSERT_NE(rect, nullptr);
    EXPECT_EQ(rect->to_string(), "rectangle 18.00 18.00");

    auto circ = factory.create();
    ASSERT_NE(circ, nullptr);
    EXPECT_EQ(circ->to_string(), "circle 5.00");

    EXPECT_EQ(factory.create(), nullptr); // EOF
}