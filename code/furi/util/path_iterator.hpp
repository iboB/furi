#pragma once
#include "strutil.hpp"
#include <cassert>

namespace furi::util
{

// Iterate a path
// Usage: for (path_iterator i(split.path); !i.done(); i.next()) f(i.get());
class path_iterator
{
    const char* begin;
    const char* p;
    const char* range_end;

    path_iterator() noexcept = default;
public:
    static path_iterator make_begin(std::string_view path) noexcept
    {
        path_iterator r;
        if (path.empty()) return make_end(path);
        r.begin = path.data();
        r.p = path.data();
        r.range_end = path.data() + path.size();
        if (path[0] != '/') --r.begin; // hacky redirect for paths which don't begin with /
        r.next();
        return r;
    }

    static path_iterator make_end(std::string_view path) noexcept
    {
        path_iterator r;
        r.begin = r.p = r.range_end = path.data() + path.size();
        return r;
    }

    void next() noexcept
    {
        begin = p;
        ++p; // overflow to skip last separator

        for (; p < range_end; ++p)
        {
            if (*p == '/') break;
        }
    }

    void operator++() noexcept { next(); }

    explicit operator bool() const noexcept { return begin != range_end; }

    std::string_view operator*() const noexcept
    {
        assert(p <= range_end); // out-of bounds check

        // at this point begin points to one before the actual item
        return strutil::make_string_view(begin + 1, p);
    }

    bool operator==(const path_iterator& other) const noexcept { return begin == other.begin; }
    bool operator!=(const path_iterator& other) const noexcept { return begin != other.begin; }
};

}
