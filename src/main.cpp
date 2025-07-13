#include <algorithm>
#include <iostream>
#include <print>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

int main() {
    Book book("Dostoevsky", "Hello", 2029, "Fiction", 4.2, 1234);
    //
    // Ниже приведён пример работы `BookDatabase`.
    //
    //     - Обратите внимание, что в этой функции реализованы основные возможности, охватывающие как обязательные, так
    //     и опциональные требования,
    //       которые не обязательны к реализации для сдачи работы.
    //     - Не забудьте перед созданием коммита вызвать 'run_clang_format.sh' для форматирования кода
    //

    // Create a book database
    BookDatabase<std::vector<Book>> db;

    // Код закомментирован, чтобы не приводить к ошибке компиляции

    // Add some books
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4., 190);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
    db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156);
    db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178);
    db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);
    db.EmplaceBack("Jane Eyre", "Charlotte Brontë", 1847, Genre::Biography, 4.6, 110);
    db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("Lord of the Rings: Fellowship of the ring", "J.R.R. Tolkien", 1931, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("Lord of the Rings: The Two Towers", "J.R.R. Tolkien", 1933, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("Lord of the Rings: The Return of the king", "J.R.R. Tolkien", 1935, Genre::Fiction, 4.9, 203);
    std::print("Books: {}\n\n", db);

    // Sorts
    std::sort(db.begin(), db.end(), comp::LessByAuthor{});
    std::print("Books sorted by author: {}\n\n==================\n", db);

    std::sort(db.begin(), db.end(), comp::LessByPopularity{});
    std::print("Books sorted by popularity: {}\n\n==================\n", db);

    // Author histogram
    auto histogram = buildAuthorHistogramFlat(db);
    //ERR: error: static assertion failed: std::formatter must be specialized for each type being formatted
    //std::print("Author histogram: {}", histogram);

    // Ratings
    auto genreRatings = calculateGenreRatings(db.begin(), db.end());
    //ERR: same as above, no formatter for flat_map in g++ 15 ?s
    //std::print("\n\nAverage ratings by genres: {}\n", genreRatings);

    auto avrRating = calculateAverageRating(db);
    std::print("Average books rating in library: {:.2f}\n", avrRating);

    auto sample = sampleRandomBooks(db, 3);
    std::print("Random sample of 3 books:\n{}\n", sample);

    // Filters
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    std::print("\n\nBooks from the 20th century with rating ≥ 4.5:\n{}\n", filtered);

    auto filtered2 = filterBooks(db.begin(), db.end(), any_of(GenreIs(Genre::SciFi), GenreIs(Genre::Biography)));
    std::print("\n\nBooks Genre is SciFi or biography:\n{}\n", filtered2);

    auto filtered3 = filterBooks(db.begin(), db.end(), GenreIs(Genre::Biography));
    std::print("\n\nBooks Genre is biography:\n{}\n", filtered3);

    // Top 3 books
    auto topBooks = getTopNBy(db, 3);
    std::print("\n\nTop 3 books by rating:\n{}\n", topBooks);

    auto orwellBookIt = std::ranges::find_if(db, [](const auto &v) { return v.author == "George Orwell"; });
    if (orwellBookIt != db.end()) {
        std::print("\n\nTransparent lookup by authors. Found Orwell's book: {}\n", *orwellBookIt);
    }

    return 0;
}