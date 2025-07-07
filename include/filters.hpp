#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"
#include "statsistics.hpp"

namespace bookdb {

template<BookPredicate... Predicates>
static std::function<bool(const Book&)> all_of(Predicates... pred) {
    return [&](const Book& book) {
        return (pred(book) && ...);
    };
}

template<BookPredicate... Predicates>
static std::function<bool(const Book&)> any_of(Predicates... pred) {
    return [&](const Book& book) {
        return (pred(book) || ...);
    };
}

template<BookIterator It, BookPredicate... Preds>
BookRefVec filterBooks(It begin, It end, Preds&&... preds) {
    BookRefVec result;
    std::for_each(begin, end, [&](const auto& book) {
        //Applies all of the predicates to each book
        if ((std::forward<Preds>(preds)(book) && ...)) {
            result.emplace_back(book);
        }
    });
    return result;
}

//Factories
static std::function<bool(const Book&)> YearBetween(int start, int end) {
    return [start, end](const Book& book) {
        return start < book.year && book.year < end;
    };
}

static std::function<bool(const Book&)> RatingAbove(double rating) {
    return [rating](const Book& book) {
        return rating < book.rating;
    };
}

static std::function<bool(const Book&)> GenreIs(Genre genre) {
    return [genre](const Book& book) {
        return book.genre == genre;
    };
}

}  // namespace bookdb