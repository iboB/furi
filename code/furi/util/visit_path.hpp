#pragma once
#include "strutil.hpp"

namespace furi::util
{

// Visit path items
// Call f for each one
// f must be invokable like `void f(std::string_view elem)`
// The last item for which f is called, will either be a filename or an empty string
// in case the path is a directory (ends with '/')
template <typename F>
void visit_path(std::string_view path, F f)
{
    if (path.empty()) return;
    if (path[0] == '/') path.remove_prefix(1); // remove first separator if any

    for (auto p = path.begin(); p != path.end(); )
    {
        if (*p == '/')
        {
            f(strutil::make_string_view(path.begin(), p));
            path = strutil::make_string_view(p + 1, path.end()); // slice off
            p = path.begin(); // redundant but we appease MSSTL here (no mixing of iterators)
        }
        else
        {
            ++p;
        }
    }

    // last item
    f(path);
}

}
