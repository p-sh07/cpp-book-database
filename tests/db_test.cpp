#include "book_database.hpp"
#include "filters.hpp"
#include "statsistics.hpp"
#include <gtest/gtest.h>
#include <unordered_set>

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

//====== Database construction =======
TEST(TestConstructors, DbInitializerList) {
    Book book1 ("1984"s, "George Orwell"sv, 1949, Genre::SciFi, 4., 190);
    Book book2 ("Animal Farm"s, "George Orwell"sv, 1945, Genre::Fiction, 4.4, 143);
    Book book3 ("The Great Gatsby"s, "F. Scott Fitzgerald"sv, 1925, Genre::Fiction, 4.5, 120);

    BookDatabase db{book1, book2, book3};

    EXPECT_EQ(db.size(), 3u);
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

    //currently, author is still added, but undefined behaviour with string_view
    //theoretically author prints "123!@#321!#@1", but can differ depending on system
    EXPECT_EQ(db.GetAuthors().size(), 1u);
}

TEST(TestAuthorName, AuthorStringStoredCorrect) {
    BookDatabase db;
    {
        std::string author_temp_str = "George Orwell";
        Book book1 ("1984"s, author_temp_str, 1949, Genre::SciFi, 4., 190);
        db.PushBack(book1);
    }

    EXPECT_EQ(db.begin()->author, "George Orwell"sv);
}

TEST(TestAuthorName, AuthorNameStoredOnce) {

    BookDatabase db;
    db.EmplaceBack("Bright Future Poetry", "Victor Pushkin", 2038, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("Another Life", "Victor Pushkin", 2032, Genre::Fiction, 4.2, 141);
    db.EmplaceBack("Captains' Son", "Victor Pushkin", 2039, Genre::Fiction, 4.9, 11);
    db.EmplaceBack("Frost and Sun", "Victor Pushkin", 2045, Genre::Fiction, 3.8, 2123);

    EXPECT_EQ(db.GetAuthors().size(), 1u);
}


//====== Book construction and insertion =======
//Testing book construction, push back and emplace for move/copy
//TODO: add EXPECT_EQ expected counts for move/copy counts
struct TestBook {
    TestBook(std::string author) : author(author) {
        std::println(std::cerr, " *Book ctr");
        ++ctr;
    }

    //copy constructor
    TestBook(TestBook& other) : author(other.author) {
        std::println(std::cerr, " *Book COPY ctr");
        ++copy_ctr;
    }

    TestBook(const TestBook& other) : author(other.author) {
        std::println(std::cerr, " *Book C-COPY ctr");
        ++copy_ctr;
    }

    //move constructor
    TestBook(TestBook&& other) noexcept : author(std::move(other.author)) {
        std::println(std::cerr, " =Book MOVE ctr");
        ++move_ctr;
    }

    TestBook& operator=(TestBook& other) {
        author = other.author;
        std::println(std::cerr, " **Book COPY ASSIGN");
        ++copy_asgn;
        return *this;
    }

    TestBook& operator=(const TestBook& other) {
        author = other.author;
        std::println(std::cerr, " **Book C-COPY ASSIGN");
        ++copy_asgn;
        return *this;
    }

    TestBook& operator=(TestBook&& other) noexcept {
        author = std::move(other.author);
        std::println(std::cerr, " ==Book MOVE ASSIGN");
        ++move_asgn;
        return *this;
    }

    std::string author;
    unsigned short ctr = 0u;
    unsigned short copy_ctr = 0u;
    unsigned short move_ctr = 0u;
    unsigned short copy_asgn = 0u;
    unsigned short move_asgn = 0u;
};

//TODO: test suite TEST_P?
// class TestConstruction : public testing::Test {
// protected:

TEST(TestBookCopyMove, InitializerListConstruct) {
    std::println(std::cerr, "->Constructing books");
    TestBook tb1("first");
    TestBook tb2("second");

    std::println(std::cerr, "->Constructing DB");

    //TODO: should initializer_list copy/move? or just be a const val ref?
    BookDatabase<std::vector<TestBook>> db{tb1, tb2};
}

TEST(TestBookCopyMove, PushBack) {
    std::println(std::cerr, "->Constructing book for push back");
    TestBook bk("book");

    BookDatabase<std::vector<TestBook>> db;
    db.PushBack(bk);
}

TEST(TestBookCopyMove, Emplace) {
    std::println(std::cerr, "->Constructing book for emplace");

    BookDatabase<std::vector<TestBook>> db;
    db.EmplaceBack("book");
}

//====== Edge Cases =======
TEST(TestDb, StatisticsForEmptyDB) {
    BookDatabase db;
    std::flat_map<std::string_view, size_t, bookdb::TransparentStringLess> empty_histogram{};
    EXPECT_EQ(bookdb::buildAuthorHistogramFlat(db), empty_histogram);

    EXPECT_EQ(bookdb::calculateAverageRating(db), 0.0);

    EXPECT_EQ(bookdb::sampleRandomBooks(db).size(), 0u);
    EXPECT_EQ(bookdb::getTopNBy(db).size(), 0u);
}

TEST(TestDb, FiltersForEmptyDB) {
    BookDatabase db;
    EXPECT_EQ(filterBooks(db.begin(), db.end(), bookdb::YearBetween(1900, 2000)).size(), 0u);
    EXPECT_EQ(filterBooks(db.begin(), db.end(), bookdb::RatingAbove(4.0)).size(), 0u);
}

//====== Correct statistics =======
TEST(TestDb, StatisticsCorrectRatings) {
    BookDatabase db{
        {"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.6, 120},
        {"Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.4, 98},
        {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178},
        {"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 190},
        {"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.2, 143},
    };

    //should sort authors alphabetically
    auto author_histogram = buildAuthorHistogramFlat(db);
    ASSERT_EQ(author_histogram.size(), 4u);
    EXPECT_EQ(author_histogram.begin()->first, "Aldous Huxley"sv);
    EXPECT_EQ(author_histogram.begin()->second, 1u);

    EXPECT_EQ(author_histogram.at("George Orwell"sv), 2u);

    auto genre_ratings = calculateGenreRatings(db.begin(), db.end());
    EXPECT_DOUBLE_EQ(genre_ratings.at(Genre::SciFi), 4.450);
    EXPECT_DOUBLE_EQ(genre_ratings.at(Genre::Fiction), 4.50);

    auto avg_rating = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(avg_rating, 4.480);
}

TEST(TestDb, StatisticsCorrectTopNby) {
    BookDatabase db{
            {"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.6, 120},
            {"Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.4, 98},
            {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178},
            {"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 190},
            {"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.2, 143},
        };

    //Greater rating on top by default
    auto top_2_rating = getTopNBy(db, 2);
    ASSERT_EQ(top_2_rating.size(), 2u);

    EXPECT_EQ(top_2_rating[0].get().title, "Pride and Prejudice"sv);
    EXPECT_EQ(top_2_rating[1].get().title, "The Great Gatsby"sv);

    bookdb::comp::LessByRating rating_less{};
    auto bottom_3_rating = getTopNBy(db, 3, rating_less);
    ASSERT_EQ(bottom_3_rating.size(), 3u);

    EXPECT_EQ(bottom_3_rating[0].get().title, "Animal Farm"sv);
    EXPECT_EQ(bottom_3_rating[1].get().title, "Brave New World"sv);
    EXPECT_EQ(bottom_3_rating[2].get().title, "1984"sv);
}

TEST(TestDb, SampleBooks) {
    BookDatabase db{
        {"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.6, 120},
        {"Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98},
        {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178},
        {"1984", "George Orwell", 1949, Genre::SciFi, 4., 190},
        {"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143},
        {"To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156},
        {"The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112},
        {"Jane Eyre", "Charlotte Brontë", 1847, Genre::Fiction, 4.6, 110},
        {"The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203},
        {"Lord of the Flies", "William Golding", 1954, Genre::Fiction, 4.2, 89},
    };

    //Check nbooks > db.size()
    auto book_sample = sampleRandomBooks(db, 15);
    ASSERT_EQ(book_sample.size(), 10u);

    book_sample = sampleRandomBooks(db, 1);
    ASSERT_EQ(book_sample.size(), 1u);

    //Chack that sample doesn't contain repeating books
    book_sample = sampleRandomBooks(db, 8);

    std::unordered_set<std::string_view> book_names;
    for (const auto book : book_sample) {
        //will be false if book title already exists in set
        EXPECT_TRUE(book_names.insert(book.get().title).second);
    }
}

//====== Correct filtering =======
TEST(TestDb, FiltersCorrect) {
    BookDatabase db{
        {"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.6, 120},
        {"Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98},
        {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178}
    };


    auto filtered_by_year_and_rating = filterBooks(db.begin(), db.end(), bookdb::YearBetween(1900, 2000), bookdb::RatingAbove(4.5));
    ASSERT_EQ(filtered_by_year_and_rating.size(), 1u);
    EXPECT_EQ(filtered_by_year_and_rating[0].get().title, "The Great Gatsby"sv);

    auto filtered_by_genre = filterBooks(db.begin(), db.end(), bookdb::GenreIs(Genre::SciFi));
    ASSERT_EQ(filtered_by_genre.size(), 1u);
    EXPECT_EQ(filtered_by_genre[0].get().title, "Brave New World"sv);

    auto filtered_any_of = filterBooks(db.begin(), db.end(), bookdb::any_of(bookdb::GenreIs(Genre::SciFi), bookdb::YearBetween(1800, 1900)));
    ASSERT_EQ(filtered_any_of.size(), 2u);
    EXPECT_EQ(filtered_any_of[0].get().title, "Brave New World"sv);
}

//====== Test with a std::deque =======