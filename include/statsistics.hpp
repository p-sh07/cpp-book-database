#pragma once

#include <algorithm>
#include <flat_map>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"
#include "comparators.hpp"

#include <print>

namespace bookdb {
namespace rg = std::ranges;
namespace vw = std::views;

using BookRefVec = std::vector<std::reference_wrapper<const Book>>;

// Number of books by each author
template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &db, Comparator comp = {}) {
    std::flat_map<std::string_view, size_t, Comparator> author_counts;
    rg::for_each(db.GetBooks(), [&](const auto &book) { ++author_counts[book.author]; });
    return author_counts;
}

// TODO: не до конца понятно, в чем смысл использования в этих функциях flat_ контейнеров, более эффективный поиск по
// результатам? Average ratings by genres
template <BookIterator It>
auto calculateGenreRatings(It begin, It end) {
    std::flat_map<Genre, double> genre_ratings;
    std::flat_map<Genre, size_t> genre_book_count;

    std::for_each(begin, end, [&](const auto &book) {
        genre_ratings[book.genre] += book.rating;
        ++genre_book_count[book.genre];
    });

    for (const auto &[genre, count] : genre_book_count) {
        genre_ratings.at(genre) /= count;
    }

    return genre_ratings;
}

// Average books rating in library
template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &db) {
    if (db.size() == 0) {
        return 0.0;
    }

    auto get_book_rating = [](const auto &book) { return book.rating; };

    // more eficient?
    // https://stackoverflow.com/questions/76796910/why-do-we-have-stdtransform-reduce-in-c-standard-library
    double sum = std::transform_reduce(db.begin(), db.end(), 0.0, std::plus<>(), get_book_rating);

    // could also use:
    // double sum = rg::fold_left( db | vw::transform(get_book_rating), 0.0, std::plus<>());

    return db.size() == 0u ? 0.0 : sum / (1.0 * db.size());
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &db, size_t nbooks = 0) {
    // random generator
    static std::mt19937 gen{std::random_device()()};

    BookRefVec result;
    rg::sample(db.GetBooks(), std::back_inserter(result), nbooks, gen);

    return result;
}  // TODO: possible sentinel use here

template <BookContainerLike T, BookComparator Comparator = comp::GreaterByRating>
auto getTopNBy(BookDatabase<T> &db, size_t nbooks = 0, Comparator comp = {}) {
    // prevent getting more than max
    if (nbooks > db.size()) {
        nbooks = db.size();
    }
    // sort books underlying container to get top n books
    auto sorted_end = db.begin() + nbooks;
    rg::partial_sort(db, sorted_end, comp);

    return BookRefVec{db.begin(), sorted_end};
}  // TODO: possible sentinel use here

}  // namespace bookdb

namespace std {
// Formatter для BookRefVec
template <>
struct std::formatter<bookdb::BookRefVec, char> {
    template <typename FormatContext>
    auto format(const bookdb::BookRefVec &book_vec, FormatContext &fc) const {
        size_t n = 1;
        for (const auto &book : book_vec) {
            format_to(fc.out(), " {}. {}\n", n++, book.get());
        }

        return fc.out();
    }

    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
};
}  // namespace std
