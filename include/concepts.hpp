#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T const& t) {
    //Must have iterator?
    typename T::iterator;

    //stores books?
    // std::is_same_v<Book, typename T::value_type>();
};


template <typename T>
concept BookIterator = requires(T const& t) {
    //is iterator
    typename std::iterator_traits<T>::iterator_category;

    //has books
    std::is_same_v<Book, typename std::iterator_traits<T>::value_type>();
};

//TODO:
template <typename S, typename I>
concept BookSentinel = true;

template <typename P>
concept BookPredicate = true;

template <typename C>
concept BookComparator = true;

}  // namespace bookdb