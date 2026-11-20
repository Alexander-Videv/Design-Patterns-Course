#include "../include/Factory.hpp"

#include <sstream>
#include <fstream>

const double EPSILON = 0.01;
double min_val = 0.01;
double max_val = static_cast<double>(RAND_MAX);

FIGURE_TYPE type_from_string(const std::string &str)
{
    if (str == "triangle")
        return FIGURE_TYPE::TRIANGLE;

    else if (str == "rectangle")
        return FIGURE_TYPE::RECTANGLE;

    else if (str == "circle")
        return FIGURE_TYPE::CIRCLE;

    else
        return FIGURE_TYPE::INVALID;
}

std::unique_ptr<Figure> read_triangle(std::istream &is)
{
    double a = 0;
    double b = 0;
    double c = 0;

    is >> a >> b >> c;

    try
    {
        return std::make_unique<Triangle>(a, b, c);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}

std::unique_ptr<Figure> read_rectangle(std::istream &is)
{
    double a = 0;
    double b = 0;

    is >> a >> b;

    try
    {
        return std::make_unique<Rectangle>(a, b);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}

std::unique_ptr<Figure> read_circle(std::istream &is)
{
    double radius = 0;

    is >> radius;

    try
    {
        return std::make_unique<Circle>(radius);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}

double random_double(double min, double max)
{
    double scale = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
    return min + scale * (max - min);
}

std::unique_ptr<Figure> StringToFigure::create_from(const std::string &str)
{
    std::istringstream ss(str);

    std::string type_string;

    ss >> type_string;

    FIGURE_TYPE type = type_from_string(type_string);

    switch (type)
    {
    case FIGURE_TYPE::TRIANGLE:
        return read_triangle(ss);

    case FIGURE_TYPE::RECTANGLE:
        return read_rectangle(ss);

    case FIGURE_TYPE::CIRCLE:
        return read_circle(ss);

    default:
        return nullptr;
    }
}

//---------- Stream Factory impl

std::unique_ptr<Figure> StreamFigureFactory::create()
{
    std::string line;
    if (!std::getline(input_stream, line))
        return nullptr;

    return StringToFigure::create_from(line);
}

StreamFigureFactory::StreamFigureFactory(std::istream &is)
    : input_stream(is)
{
    if (!input_stream.good())
        throw std::invalid_argument("Bad stream");
}

StreamFigureFactory::StreamFigureFactory(std::ifstream &&fs)
    : file(std::move(fs)), input_stream(file)
{
    if (!input_stream.good())
        throw std::invalid_argument("Bad stream");
}

//---------- Random Factory impl

RandomFactory::RandomFactory(unsigned long seed, double min, double max)
    : min_val(min), max_val(max)
{
    if (!(min_val > EPSILON && min_val < max_val && max_val < 1e9))
        throw std::invalid_argument("Invalid range for RandomFactory");

    if (seed == 0)
        seed = std::random_device{}();

    rng.seed(seed);
}

// Suggested by Gemini
std::unique_ptr<Figure> RandomFactory::create()
{

    std::uniform_int_distribution<int> rand_type(0, static_cast<int>(FIGURE_TYPE::COUNT) - 1);
    std::uniform_real_distribution<double> rand_double(min_val, max_val);

    FIGURE_TYPE type = static_cast<FIGURE_TYPE>(rand_type(rng));

    switch (type)
    {
    case FIGURE_TYPE::TRIANGLE:
    {
        double a = rand_double(rng);
        double b = rand_double(rng);

        // Satisfy triangle inequality: |a - b| < c < a + b
        double min_c = std::abs(a - b) + EPSILON;
        double max_c = (a + b) - EPSILON;

        std::uniform_real_distribution<double> rand_c(min_c, max_c);

        double c = rand_c(rng);

        return std::make_unique<Triangle>(a, b, c);
    }

    case FIGURE_TYPE::RECTANGLE:
    {
        double a = rand_double(rng);
        double b = rand_double(rng);
        return std::make_unique<Rectangle>(a, b);
    }

    case FIGURE_TYPE::CIRCLE:
    {
        double radius = rand_double(rng);
        return std::make_unique<Circle>(radius);
    }

    default:
        return nullptr;
    }
}

//---------- Abstract Figure Factory impl

std::unique_ptr<FigureFactory> AbstractFigureFactory::create(const std::string &str)
{
    std::istringstream line(str);

    std::string fact_type;

    line >> fact_type;

    try
    {
        if (fact_type == "random")
        {
            double min_val = 1.0;
            double max_val = 100.0;

            // Optional: parse "random <min> <max>" if provided
            if (line >> min_val)
            {
                if (!(line >> max_val))
                    return nullptr; // Provided min without max -> invalid command
            }

            return std::make_unique<RandomFactory>(0, min_val, max_val);
        }

        if (fact_type == "console")
            return std::make_unique<StreamFigureFactory>(std::cin);

        if (fact_type == "file")
        {
            std::string file_name;

            if (!(line >> file_name))
                return nullptr;

            std::ifstream fs(file_name);

            return std::make_unique<StreamFigureFactory>(std::move(fs));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return nullptr;
}
