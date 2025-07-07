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