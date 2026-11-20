#include "../include/Figure.hpp"
#include "../include/FigureRegistry.hpp"

#include <string_view>
#include <iomanip>
#include <exception>
#include <cmath>

const double EPSILON = 0.01;

std::string format_double(double value, unsigned int precision = 2)
{
    std::ostringstream stream;

    stream << std::fixed << std::setprecision(precision) << value;

    return stream.str();
}

std::ostream &operator<<(std::ostream &os, const Figure &figure)
{
    os << figure.to_string();
    return os;
}

//---------- Triangle impl

Triangle::Triangle(double a, double b, double c)
    : a(a), b(b), c(c)
{
    if (a < EPSILON || b < EPSILON || c < EPSILON)
        throw std::invalid_argument("Side cant be 0 or negative!");

    if (a + b <= c || a + c <= b || b + c <= a)
        throw std::invalid_argument("A,B and C dont for a triangle!");
}

Triangle *Triangle::clone_impl() const
{
    return new Triangle(*this);
}

double Triangle::perimeter() const
{
    return a + b + c;
}

std::unique_ptr<Triangle> Triangle::clone() const
{
    return std::unique_ptr<Triangle>(clone_impl());
}

std::string Triangle::to_string() const
{
    return "triangle " +
           format_double(a) + " " +
           format_double(b) + " " +
           format_double(c);
}

void Triangle::register_shape()
{
    // 1. How to build a Triangle from text arguments
    TextFigureRegistry::register_shape({"triangle", 3}, [](const std::vector<double> &args) -> std::unique_ptr<Figure>
                                       { return std::make_unique<Triangle>(args[0], args[1], args[2]); });

    // 2. How to build a valid random Triangle in one shot (Las Vegas, zero retries)
    RandomFigureRegistry::register_shape("triangle", [](std::mt19937 &rng, double min, double max) -> std::unique_ptr<Figure>
                                         {
        constexpr double eps = 0.01;
        std::uniform_real_distribution<double> dist(min, max);
        double a = dist(rng);
        double b = dist(rng);

        std::uniform_real_distribution<double> c_dist(std::abs(a - b) + eps, (a + b) - eps);
        return std::make_unique<Triangle>(a, b, c_dist(rng)); });
}

//---------- Rectangle impl

Rectangle::Rectangle(double a, double b)
    : a(a), b(b)
{
    if (a < EPSILON || b < EPSILON)
        throw std::invalid_argument("Sides cant be 0 or negative");
}

double Rectangle::perimeter() const
{
    return 2 * (a + b);
}

Rectangle *Rectangle::clone_impl() const
{
    return new Rectangle(*this);
}

std::unique_ptr<Rectangle> Rectangle::clone() const
{
    return std::unique_ptr<Rectangle>(clone_impl());
}

std::string Rectangle::to_string() const
{
    return "rectangle " +
           format_double(a) + " " +
           format_double(b);
}

void Rectangle::register_shape()
{
    TextFigureRegistry::register_shape({"rectangle", 2}, [](const std::vector<double> &args) -> std::unique_ptr<Figure>
                                       { return std::make_unique<Rectangle>(args[0], args[1]); });

    RandomFigureRegistry::register_shape("rectangle", [](std::mt19937 &rng, double min, double max) -> std::unique_ptr<Figure>
                                         {
        std::uniform_real_distribution<double> dist(min, max);
        return std::make_unique<Rectangle>(dist(rng), dist(rng)); });
}

//---------- Circle impl

Circle::Circle(double radius)
    : radius(radius)
{
    if (radius < EPSILON)
        throw std::invalid_argument("Radius cant be 0 or negative");
}

double Circle::perimeter() const
{
    return 2 * M_PI * radius;
}

Circle *Circle::clone_impl() const
{
    return new Circle(*this);
}

std::unique_ptr<Circle> Circle::clone() const
{
    return std::unique_ptr<Circle>(clone_impl());
}

std::string Circle::to_string() const
{
    return "circle " +
           format_double(radius);
}

std::unique_ptr<Cloneable> Cloneable::clone() const
{
    return std::unique_ptr<Cloneable>(clone_impl());
}

void Circle::register_shape()
{
    TextFigureRegistry::register_shape({"circle", 1}, [](const std::vector<double> &args) -> std::unique_ptr<Figure>
                                       { return std::make_unique<Circle>(args[0]); });

    RandomFigureRegistry::register_shape("circle", [](std::mt19937 &rng, double min, double max) -> std::unique_ptr<Figure>
                                         {
        std::uniform_real_distribution<double> dist(min, max);
        return std::make_unique<Circle>(dist(rng)); });
}
