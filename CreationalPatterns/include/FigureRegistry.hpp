#ifndef FIGURE_REGISTRIES_HPP
#define FIGURE_REGISTRIES_HPP

#include "Figure.hpp"
#include "Registry.hpp"

#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

using TextKey = std::pair<std::string, std::size_t>;
using TextFigureRegistry = Registry<TextKey, std::unique_ptr<Figure>(const std::vector<double> &)>;

using RandomFigureRegistry = Registry<std::string, std::unique_ptr<Figure>(std::mt19937 &, double min, double max)>;

#endif // FIGURE_REGISTRIES_HPP