#ifndef __LIBCORE_IO_TOSTRINGARRAY_H__
#define __LIBCORE_IO_TOSTRINGARRAY_H__

#include <coredef.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::AutoPtr;
using Elastos::String;
using Elastos::ArrayOf;
using Elastos::Utility::Etl::Vector;

namespace Libcore {
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
#endif //__LIBCORE_IO_TOSTRINGARRAY_H__
