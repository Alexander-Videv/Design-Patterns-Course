#include "../include/Factory.hpp"
#include "../include/FigureRegistry.hpp"

#include <sstream>
#include <fstream>
#include <algorithm>

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

    if (!(ss >> type_string))
        return nullptr;

    std::vector<double> args;

    double curr_val = 0.0;

    while (ss >> curr_val)
        args.push_back(curr_val);

    if (!ss.eof())
        return nullptr;

    const auto *entry = FigureRegistry::find(type_string);
    if (!entry)
        return nullptr;

    const auto &counts = entry->arg_counts;
    if (std::find(counts.begin(), counts.end(), args.size()) == counts.end())
        return nullptr;

    try
    {
        return entry->create(args);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
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
    const auto &reg = FigureRegistry::entries();
    if (reg.empty())
        return nullptr;

    std::uniform_int_distribution<std::size_t> shape_dist(0, reg.size() - 1);
    auto it = reg.begin();
    std::advance(it, shape_dist(rng));

    const auto &entry = it->second;
    if (entry.arg_counts.empty())
        return nullptr;

    std::uniform_int_distribution<std::size_t> count_dist(0, entry.arg_counts.size() - 1);
    std::uniform_real_distribution<double> dim_dist(min_val, max_val);

    constexpr int max_attempts = 1000;
    for (int attempt = 0; attempt < max_attempts; ++attempt)
    {
        const std::size_t arg_count = entry.arg_counts[count_dist(rng)];
        std::vector<double> args(arg_count);
        for (double &val : args)
            val = dim_dist(rng);

        try
        {
            if (auto fig = entry.create(args))
                return fig;
        }
        catch (const std::exception &)
        {
            // Invalid random geometry (e.g. triangle inequality) -> re-roll
        }
    }

    return nullptr;
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
