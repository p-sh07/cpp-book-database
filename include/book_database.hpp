#pragma once

#include <algorithm>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases
    using AuthorContainer = std::unordered_set<std::string>;

    using Iterator = typename BookContainer::iterator;
    using ConstIterator = typename BookContainer::const_iterator;
    using Size = typename BookContainer::size_type;

    using Books = std::span<Book>;
    using Authors = const AuthorContainer&;

    //use this or force bookdb::Book?
    //using BookType = typename BookContainer::value_type;
    using BookType = Book;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<Book> books) {
        //book stores Author as string_view, assume that it has access to
        //sv data at the moment of DB construction
        std::for_each(books, [](auto& book) {
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

    // TODO?

    //==== Get =====
    // TODO: "Добавьте методы .. для _безопасного_ просмотра внутреннего состояния контейнера." - const only?

    // BookContainer &GetBooks() { return books_; }
    // AuthorContainer &GetAuthors() { return authors_; }

    Books GetBooks() const { return books_; }
    Authors GetAuthors() const { return authors_; }

    //==== Standard Functions ====
    Size size() const { return books_.size(); }

    void PushBack(BookType book) {
        books_.push_back(std::move(book));
        StoreAuthorNameString(books_.back());
    }

    void PushBack(std::span<Book> books) {
        std::for_each(books, [&](const BookType& book) {
            PushBack(book);
        });
    }

    //Works, but doesn't allow to store author name string in authors_ easily
    template <typename... BookArgs>
    BookType& EmplaceBack(BookArgs &&...book_args) {
        auto& book_ref = books_.emplace_back(std::forward<BookArgs>(book_args)...);
        StoreAuthorNameString(book_ref);
        return book_ref;
    }

private:
    BookContainer books_;
    AuthorContainer authors_;

    [[maybe_unused]] bool StoreAuthorNameString(BookType& book_ref) {
        try {
            auto [author_it, success] = authors_.emplace(book_ref.author);

            //replace s_view in book to new one, pointing to string in authors_
            book_ref.author = *author_it;
            return true;
        } catch (std::exception& ex) {
            return false;
        }
    }
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {

        // Раскомментируйте, когда bookdb::BookDatabase поддержит интерфейсы, доступные стандартным контейнерам
        //(size/begin/...)

        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
