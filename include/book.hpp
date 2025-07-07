#pragma once

#include <array>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

namespace bookdb {
using std::literals::operator""sv;

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

static constexpr std::array<std::string_view, 6> genre_strings{"Fiction",   "NonFiction", "SciFi",
                                                               "Biography", "Mystery",    "Unknown"};

constexpr Genre GenreFromString(std::string_view str) {
    for (size_t i = 0; i < genre_strings.size(); ++i) {
        if (genre_strings[i] == str) {
            return static_cast<Genre>(i);
        }
    }
    return Genre::Unknown;
}

constexpr std::string_view StringFromGenre(Genre genre) {
    if (auto genre_idx = static_cast<size_t>(genre); genre_idx < genre_strings.size()) {
        return genre_strings[genre_idx];
    }
    // To prevent errors in case list is expanded or string values changed
    return genre_strings[static_cast<size_t>(Genre::Unknown)];
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
