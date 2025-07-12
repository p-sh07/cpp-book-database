#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {
template <typename It>
concept BookIterator = std::input_iterator<It>
    && std::same_as<std::iter_value_t<It>, Book>;

template <typename T>
concept BookContainerLike = requires(T const &c)
{
    // Must have iterators pointing to a BookType and support begin/end
    { c.begin() } -> BookIterator;
    { c.end() } -> BookIterator;

    // can get size
    { c.size() } noexcept -> std::same_as<std::size_t>;

    // has ::value_type, which is book
    typename T::value_type;
    std::same_as<typename T::value_type, Book>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I> && BookIterator<I>;

// TODO: replace "Book" with a BookType concept somehow?
template <typename P>
concept BookPredicate = std::predicate<P, const Book&>;


template <typename C>
concept BookComparator = requires(C comp, const Book &lhs, const Book &rhs) {
    { comp(lhs, rhs) } -> std::convertible_to<bool>;
};

}  // namespace bookdb