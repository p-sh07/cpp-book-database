#pragma once

#include <array>
#include <format>
#include <string>
#include <string_view>

namespace bookdb {
namespace rg = std::ranges;
using std::literals::operator""sv;

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

using GenreStringPair = std::pair<Genre, std::string_view>;
static constexpr std::array<GenreStringPair, 6> genre_strings{
    {// <- need extra braces: https://stackoverflow.com/questions/27669200/how-should-i-brace-initialize-an-stdarray-of-stdpairs
        {Genre::Fiction, "Fiction"sv},
        {Genre::NonFiction, "NonFiction"sv},
        {Genre::SciFi, "SciFi"sv},
        {Genre::Biography, "Biography"sv},
        {Genre::Mystery, "Mystery"sv},
        {Genre::Unknown, "Unknown"sv},
    }
};

constexpr Genre GenreFromString(std::string_view str) {
    const auto it = rg::find(genre_strings, str, &GenreStringPair::second);
    if (it == genre_strings.end()) {
        return Genre::Unknown;
    }
    return it->first;
}

constexpr std::string_view StringFromGenre(Genre genre) {
    const auto it = rg::find(genre_strings, genre, &GenreStringPair::first);
    if (it == genre_strings.end()) {
        return StringFromGenre(Genre::Unknown);
    }
    return it->second;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;
    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string title, std::string_view author, int year, Genre genre, double rating, int read_count)
        : author(author), title(std::move(title)), year(year), genre(genre), rating(rating), read_count(read_count) {}

    constexpr Book(std::string title, std::string_view author, int year, std::string_view genre_str, double rating,
                   int read_count)
        : author(author), title(std::move(title)), year(year), genre(GenreFromString(genre_str)), rating(rating),
          read_count(read_count) {}
};

}  // namespace bookdb

namespace std {

// Formatter для Genre
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        return format_to(fc.out(), "{}", bookdb::StringFromGenre(g));
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

// Formatter для Book
template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &book, FormatContext &fc) const {
        return format_to(fc.out(), "\"{}\" by {} ({}), {}, Rating: {:.2f} [read {} times]", book.title, book.author,
                         book.year, book.genre, book.rating, book.read_count);
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

}  // namespace std
