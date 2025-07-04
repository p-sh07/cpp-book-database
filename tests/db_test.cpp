#include <gtest/gtest.h>
#include "book_database.hpp"

using std::literals::operator""s;
using std::literals::operator ""sv;

using bookdb::Genre;
using bookdb::Book;
using bookdb::BookDatabase;

/*
*db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4., 190);
db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156);
db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178);
db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112);
db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);
db.EmplaceBack("Jane Eyre", "Charlotte Brontë", 1847, Genre::Fiction, 4.6, 110);
db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);
db.EmplaceBack("Lord of the Flies", "William Golding", 1954, Genre::Fiction, 4.2, 89);
*/

TEST(TestConstructors, DbInitializerList) {
    Book book1 ("1984"s, "George Orwell"sv, 1949, Genre::SciFi, 4., 190);
    Book book2 ("Animal Farm"s, "George Orwell"sv, 1945, Genre::Fiction, 4.4, 143);
    Book book3 ("The Great Gatsby"s, "F. Scott Fitzgerald"sv, 1925, Genre::Fiction, 4.5, 120);

    BookDatabase db{book1, book2, book3};

    ASSERT_EQ(db.size(), 3u);
}

TEST(TestAuthorName, AuthorStringStoredInaccessible) {
    std::string_view bad_author_string_view;
    std::string garbage = "123!@#321!#@127317$&#$"s;
    {
        std::string author_temp_str = "George Orwell";
        bad_author_string_view = author_temp_str;
        std::swap(author_temp_str, garbage);
    }
    BookDatabase db;
    Book book1("1984"s, bad_author_string_view, 1949, Genre::SciFi, 4., 190);
    db.PushBack(book1);

    std::println("author: {}", db.begin()->author);
    EXPECT_EQ(db.GetAuthors().size(), 0u);
    // try {
    //     //Should be segfault
    //      std::println("author: {}", *db.GetAuthors().begin());
    //     FAIL() << "Access to invalid author name did not throw exception!\n";
    // } catch (std::exception& ex){
    //     //worked as expected
    // }
}

TEST(TestAuthorName, AuthorStringStoredCorrect) {
    BookDatabase db;
    {
        std::string author_temp_str = "George Orwell";
        Book book1 ("1984"s, author_temp_str, 1949, Genre::SciFi, 4., 190);
        db.PushBack(book1);
    }

    ASSERT_EQ(db.begin()->author, "George Orwell"sv);
}

TEST(TestAuthorName, AuthorNameStoredOnce) {

    BookDatabase db;
    db.EmplaceBack("Bright Future Poetry", "Victor Pushkin", 2038, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("Another Life", "Victor Pushkin", 2032, Genre::Fiction, 4.2, 141);
    db.EmplaceBack("Captains' Son", "Victor Pushkin", 2039, Genre::Fiction, 4.9, 11);
    db.EmplaceBack("Frost and Sun", "Victor Pushkin", 2045, Genre::Fiction, 3.8, 2123);

    ASSERT_EQ(db.GetAuthors().size(), 1u);
}
