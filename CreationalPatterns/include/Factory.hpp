#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "Figure.hpp"
#include <fstream>
#include <random>

enum class FIGURE_TYPE
{
    TRIANGLE,
    RECTANGLE,
    CIRCLE,
    COUNT,
    INVALID
};

class FigureFactory
{
public:
    virtual std::unique_ptr<Figure> create() = 0;
    virtual ~FigureFactory() = default;
};

class RandomFactory : public FigureFactory
{
public:
    RandomFactory(unsigned long seed = 0, double min = 1.0, double max = 100.0);

    std::unique_ptr<Figure> create() override;

private:
    double min_val = 1.0;
    double max_val = 100.0;
    std::mt19937 rng;
};

class StreamFigureFactory : public FigureFactory
{
public:
    virtual std::unique_ptr<Figure> create() override;

    StreamFigureFactory(std::istream &is);

    explicit StreamFigureFactory(std::ifstream &&fs);

private:
    std::ifstream file;
    std::istream &input_stream;
};

class StringToFigure
{
public:
    static std::unique_ptr<Figure> create_from(const std::string &str);
};

class AbstractFigureFactory
{
public:
    static std::unique_ptr<FigureFactory> create(const std::string &str);
};

#endif // FACTORY_HPP