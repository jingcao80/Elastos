#ifndef __ELASTOS_IO_TOSTRINGARRAY_S_H__
#define __ELASTOS_IO_TOSTRINGARRAY_S_H__

#include <elquintet.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace IO {

template <typename Counter, typename Getter>
AutoPtr< ArrayOf<String> > ToStringArray(Counter* counter, Getter* getter)
{
    size_t count = (*counter)();
    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(count);
    if (result == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        String s((*getter)(i));
        (*result)[i] = s;
    }
    return result;
}

AutoPtr< ArrayOf<String> > ToStringArray(const Vector<String>& strings);

AutoPtr< ArrayOf<String> > ToStringArray(const char* const* strings);
}
}
#endif //__TOSTRINGARRAY_H__
