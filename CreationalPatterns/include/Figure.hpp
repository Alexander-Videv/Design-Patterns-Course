#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <iostream>
#include <string>
#include <memory>

class Stringable
{
public:
    virtual std::string to_string() const = 0;
    virtual ~Stringable() = default;
};

// За да мога да използвам unique_ptr в Cloneable, Gemini предложи следния интерфейс.

class Cloneable
{
public:
    std::unique_ptr<Cloneable> clone() const;
    virtual ~Cloneable() = default;

protected:
    virtual Cloneable *clone_impl() const = 0;
};

class Figure : public Cloneable, public Stringable
{
public:
    virtual double perimeter() const = 0;
    virtual ~Figure() = default;

    friend std::ostream &operator<<(std::ostream &os, const Figure &figure);

protected:
    Figure *clone_impl() const override = 0;
};

class Triangle : public Figure
{
public:
    virtual std::string to_string() const;

    std::unique_ptr<Triangle> clone() const;

    virtual double perimeter() const;

    Triangle(double a, double b, double c);

    static void register_shape();

protected:
    virtual Triangle *clone_impl() const override;

private:
    double a;
    double b;
    double c;
};

class Rectangle : public Figure
{
public:
    virtual std::string to_string() const;

    std::unique_ptr<Rectangle> clone() const;

    virtual double perimeter() const;

    Rectangle(double a, double b);

    static void register_shape();

protected:
    virtual Rectangle *clone_impl() const override;

private:
    double a;
    double b;
};

class Circle : public Figure
{
public:
    virtual std::string to_string() const;

    std::unique_ptr<Circle> clone() const;

    virtual double perimeter() const;

    Circle(double radius);

    static void register_shape();

protected:
    virtual Circle *clone_impl() const override;

private:
    double radius;
};

static bool register_shapes()
{
    Circle::register_shape();
    Rectangle::register_shape();
    Triangle::register_shape();
    return true;
}

#endif