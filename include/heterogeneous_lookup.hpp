#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    bool operator()(const std::string &a, const std::string &b) const { return a < b; }

    bool operator()(const std::string_view a, const std::string_view b) const { return a < b; }
};

struct TransparentStringEqual {
    using is_transparent = void;

    bool operator()(const std::string &a, const std::string &b) const { return a == b; }

    bool operator()(const std::string_view a, const std::string_view b) const { return a == b; }

    // book here?
};

struct TransparentStringHash {
    using is_transparent = void;

    std::size_t operator()(const std::string &str) const { return std::hash<std::string>{}(str); }

    std::size_t operator()(std::string_view str) const { return std::hash<std::string_view>{}(str); }
};

struct BookHash {
    using is_transparent = void;
    // TODO: if needed
    //  std::size_t operator()(const std::string& str) const {
    //      return std::hash<std::string>{}(str);
    //  }
    //
    //  std::size_t operator()(std::string_view str) const {
    //      return std::hash<std::string_view>{}(str);
    //  }
};

}  // namespace bookdb

// e.g.:
/*
namespace std {
template <>
struct hash<MyId> {
    using is_transparent = void;  // поддержка гетерогенного поиска

    size_t operator()(const MyId &key) const { return std::hash<int>{}(key.id); }

    // гетерогенное получение хеша
    std::size_t operator()(int id) const { return std::hash<int>{}(id); }
};

template <>
struct equal_to<MyId> {
    using is_transparent = void;  // поддержка гетерогенного поиска

    // операция сравнения для двух элементов класса MyId
    bool operator()(const MyId &lhs, const MyId &rhs) const { return lhs.id == rhs.id; }

    // гетерогенные операции сравнения
    bool operator()(const MyId &lhs, int rhs) const { return lhs.id == rhs; }
    bool operator()(int lhs, const MyId &rhs) const { return operator()(rhs, lhs); }
};
}  // namespace std
*/