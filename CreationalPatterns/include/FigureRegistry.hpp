// include/FigureRegistry.hpp
#ifndef FIGURE_REGISTRY_HPP
#define FIGURE_REGISTRY_HPP

#include "Figure.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class FigureRegistry
{
public:
    using Creator = std::function<std::unique_ptr<Figure>(const std::vector<double> &)>;

    struct Entry
    {
        std::vector<std::size_t> arg_counts;
        Creator create;
    };

    static bool register_shape(const std::string &name,
                               std::vector<std::size_t> arg_counts,
                               Creator creator);

    static bool unregister_shape(const std::string &name);

    static const Entry *find(const std::string &name);
    static const std::map<std::string, Entry> &entries();

private:
    static std::map<std::string, Entry> &mutable_entries();
};

#endif // FIGURE_REGISTRY_HPP