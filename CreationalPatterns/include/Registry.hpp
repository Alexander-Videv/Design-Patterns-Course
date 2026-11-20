#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <functional>
#include <map>

template <typename Key, typename Signature>
class Registry
{
public:
    using Creator = std::function<Signature>;

    static bool register_shape(const Key &key, Creator creator)
    {
        if (!creator)
            return false;

        mutable_entries()[key] = std::move(creator);
        return true;
    }

    static bool unregister_shape(const Key &key)
    {
        return mutable_entries().erase(key) > 0;
    }

    static const Creator *find(const Key &key)
    {
        const auto &reg = entries();
        auto it = reg.find(key);
        return (it != reg.end()) ? &it->second : nullptr;
    }

    static const std::map<Key, Creator> &entries()
    {
        return mutable_entries();
    }

private:
    static std::map<Key, Creator> &mutable_entries()
    {
        static std::map<Key, Creator> instance;
        return instance;
    }
};

#endif // REGISTRY_HPP