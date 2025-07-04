#include <gtest/gtest.h>

#include "book_database.hpp"

using std::literals::operator""s;
using std::literals::operator ""sv;

using bookdb::Book;
using bookdb::Genre;

TEST(TestConstructors, BookGenreOrStrview) {
    Book book1 ("1984"s, "George Orwell"s, 1949, Genre::SciFi, 4., 190);
    Book book2 ("Animal Farm"s, "George Orwell"s, 1945, "Fiction"sv, 4.4, 143);

    ASSERT_EQ(book1.genre, Genre::SciFi);
    ASSERT_EQ(book2.genre, Genre::Fiction);
}