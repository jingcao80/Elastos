#ifndef __CAR_BRIDGE_STRING_H
# define __CAR_BRIDGE_STRING_H

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class String: public _ELASTOS String {
public:
    String(void): _ELASTOS String()
    {}

    String(char const *s): _ELASTOS String(s)
    {}

    String(String const &string): _ELASTOS String(string)
    {}

    operator char const * () const noexcept
    {
        return static_cast<char const *>(*static_cast<_ELASTOS String const *>(this));
    }

    void Append(String const &string)
    {
    }

    void Append(char const *format, ...)
    {
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
