#pragma once

#include <algorithm>
#include <ranges>
#include <span>
#include <string>
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

    using value_type = Book;
    // using BookType = Book;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<value_type> books) {
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
    value_type &operator[](size_t index) noexcept { return books_[index]; }
    const value_type &operator[](size_t index) const noexcept { return books_[index]; }

    // TODO: any others? Check in concept that bookcontainer supports operator[]? not required in task, but useful

    //==== Get =====
    Books GetBooks() const { return books_; }
    Authors GetAuthors() const { return authors_; }

    //==== Standard Functions ====
    Size size() const { return books_.size(); }

    void PushBack(const value_type& book) {
        books_.push_back(std::move(book));
        StoreAuthorNameString(books_.back());
    }

    void PushBack(value_type&& book) {
        books_.push_back(std::move(book));
        StoreAuthorNameString(books_.back());
    }

    void PushBack(std::span<value_type> books) {
        rg::for_each(books, [&](const value_type &book) { PushBack(book); });
    }

    template <typename... BookArgs>
    value_type &EmplaceBack(BookArgs &&...book_args) {
        auto &book_ref = books_.emplace_back(std::forward<BookArgs>(book_args)...);
        StoreAuthorNameString(book_ref);
        return book_ref;
    }

private:
    BookContainer books_;
    AuthorContainer authors_;

    void StoreAuthorNameString(value_type &book_ref) {
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

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
