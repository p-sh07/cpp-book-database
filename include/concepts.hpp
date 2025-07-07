#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

//Book must have field 'author' which is a string or string_view
//Can add other required fields if needed
template <typename T>
concept IsBook = requires(T const& b) {
    requires std::same_as<std::remove_cvref_t<decltype(b.author)>, std::string_view>
        || std::same_as<std::remove_cvref_t<decltype(b.author)>, std::string>;
};

template <typename T>
concept BookIterator = requires(T const &t) {
    // is iterator
    typename std::iterator_traits<T>::iterator_category;

    // has books
    IsBook<typename std::iterator_traits<T>::value_type>;
};

template <typename T>
concept BookContainerLike = requires(T const &c) {
    // Must have iterators pointing to a BookType and support begin/end
    requires BookIterator<decltype(c.begin())>;
    requires BookIterator<decltype(c.end())>;

    // can get size
    { c.size() } noexcept -> std::same_as<std::size_t>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I> && BookIterator<I>;

//TODO: replace "Book" with a BookType concept somehow?
template <typename P /*,typename BookType*/> //-> adding this makes BookPredicate require to specify explicit template parameters in code
concept BookPredicate = /*IsBook<BookType> && */requires(P pred, const Book& book_type) {
    //Accepts a book type
    IsBook<decltype(book_type)>;

    //takes a book object and returns bool
    { pred(book_type) } -> std::convertible_to<bool>;
};

template <typename C>
concept BookComparator = requires(C comp, const Book& lhs, const Book& rhs) {
    IsBook<decltype(lhs)>;
    IsBook<decltype(rhs)>;
    { comp(lhs, rhs) } -> std::convertible_to<bool>;
};

}  // namespace bookdb