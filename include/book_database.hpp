#pragma once

#include <algorithm>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {
namespace rg = std::ranges;

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases
    using AuthorContainer = std::unordered_set<std::string>;

    using Iterator = typename BookContainer::iterator;
    using ConstIterator = typename BookContainer::const_iterator;
    using Size = typename BookContainer::size_type;

    using Books = std::span<const Book>;
    using Authors = const AuthorContainer &;

    // use this or force bookdb::Book?
    using BookType = typename BookContainer::value_type;
    // using BookType = Book;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<BookType> books) {
        // std::println("1.starting construct");
        // book stores Author as string_view, assume that it has access to
        // sv data at the moment of DB construction
        rg::for_each(books, [&](auto &book) {
            // std::println(" 2.iterating for each");
            PushBack(book);
        });
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    //=========== Standard container interface methods ==========
    //=== Iterators =====
    Iterator begin() { return books_.begin(); }
    Iterator end() { return books_.end(); }

    ConstIterator begin() const { return books_.cbegin(); }
    ConstIterator end() const { return books_.end(); }

    //==== Op's =====
    BookType &operator[](size_t index) noexcept { return books_[index]; }
    const BookType &operator[](size_t index) const noexcept { return books_[index]; }

    // TODO: any others? Check in concept that bookcontainer supports operator[]? not required in task, but useful

    //==== Get =====
    // TODO: "Добавьте методы .. для _безопасного_ просмотра внутреннего состояния контейнера." - const only?

    // BookContainer &GetBooks() { return books_; }
    // AuthorContainer &GetAuthors() { return authors_; }

    Books GetBooks() const { return books_; }
    Authors GetAuthors() const { return authors_; }

    //==== Standard Functions ====
    Size size() const { return books_.size(); }

    void PushBack(BookType book) {
        // std::println(" 3.in pb");
        books_.push_back(std::move(book));

        // std::println(" 4.going to athor name");
        StoreAuthorNameString(books_.back());
    }

    void PushBack(std::span<BookType> books) {
        rg::for_each(books, [&](const BookType &book) { PushBack(book); });
    }

    // Works, but doesn't allow to store author name string in authors_ easily
    template <typename... BookArgs>
    BookType &EmplaceBack(BookArgs &&...book_args) {
        auto &book_ref = books_.emplace_back(std::forward<BookArgs>(book_args)...);
        StoreAuthorNameString(book_ref);
        return book_ref;
    }

private:
    BookContainer books_;
    AuthorContainer authors_;

    void StoreAuthorNameString(BookType &book_ref) {
        auto [author_it, success] = authors_.emplace(book_ref.author);

        // replace s_view in book to new one, pointing to string in authors_
        book_ref.author = *author_it;
    }
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "[BookDatabase (size = {})]\n", db.size());

        size_t n = 1;
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), " {}. {}\n", n++, book);
        }

        // No need to always print all authors?
        //  format_to(fc.out(), "Authors:\n");
        //  n = 1;
        //  for (const auto &author : db.GetAuthors()) {
        //      format_to(fc.out(), " {}. {}\n", n++, author);
        //  }
        // Get output like: "Books sorted by ...(sorted); Authors ...(random order);" - looks messy

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
