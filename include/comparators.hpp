#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.author < rhs.author;
    }
};

struct LessByTitle {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.title < rhs.title;
    }
};

struct LessByYear {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.year < rhs.year;
    }
};

struct LessByRating {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.rating < rhs.rating;
    }
};

struct GreaterByRating {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.rating > rhs.rating;
    }
};

//Least read first
struct LessByPopularity {
    bool operator()(const Book& lhs, const Book& rhs) const {
        //NB: Less popular == less read_count
        return lhs.read_count < rhs.read_count;
    }
};

//Most read first
struct GreaterByPopularity {
    bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.read_count > rhs.read_count;
    }
};

}  // namespace bookdb::comp