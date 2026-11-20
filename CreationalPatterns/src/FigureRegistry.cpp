// src/FigureRegistry.cpp
#include "../include/FigureRegistry.hpp"

bool FigureRegistry::register_shape(const std::string &name,
                                    std::vector<std::size_t> arg_counts,
                                    Creator creator)
{
    if (name.empty() || arg_counts.empty() || !creator)
        return false;

    mutable_entries()[name] = Entry{std::move(arg_counts), std::move(creator)};
    return true;
}

bool FigureRegistry::unregister_shape(const std::string &name)
{
    return mutable_entries().erase(name) > 0;
}

const FigureRegistry::Entry *FigureRegistry::find(const std::string &name)
{
    const auto &reg = entries();
    auto it = reg.find(name);
    return (it != reg.end()) ? &it->second : nullptr;
}

const std::map<std::string, FigureRegistry::Entry> &FigureRegistry::entries()
{
    return mutable_entries();
}

std::map<std::string, FigureRegistry::Entry> &FigureRegistry::mutable_entries()
{
    static std::map<std::string, Entry> instance;
    return instance;
}