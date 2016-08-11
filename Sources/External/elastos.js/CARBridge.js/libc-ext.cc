
#include <cstddef>
#include <cstring>

#include <new>

#include "macros.h"



using namespace std;

CAR_BRIDGE_NAMESPACE_BEGIN

char *strdup(char const *s) noexcept
{
    char *_s;

    _s = new(nothrow) char[strlen(s) + 1];
    if (_s == nullptr)
        return nullptr;

    strcpy(_s, s);

    return _s;
}

char *strndup(char const *s, size_t n) noexcept
{
    size_t len;

    char *_s;

    len = strlen(s);

    n = n > len ? len : n;

    _s = new(nothrow) char[n + 1];
    if (_s == nullptr)
        return nullptr;

    strncpy(_s, s, n);

    return _s;
}

CAR_BRIDGE_NAMESPACE_END

