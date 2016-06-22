#ifndef __ELASTOS_UTILITY_STRINGPOOL_H__
#define __ELASTOS_UTILITY_STRINGPOOL_H__

#include "Elastos.CoreLibrary.h"
#include "Object.h"

namespace Elastos {
namespace Utility {

/**
 * A pool of string instances. Unlike the {@link String#intern() VM's
 * interned strings}, this pool provides no guarantee of reference equality.
 * It is intended only to save allocations. This class is not thread safe.
 */
class ECO_PUBLIC StringPool
    : public Object
{
public:
    StringPool();

    /**
     * Returns a string equal to {@code new String(array, start, length)}.
     */
    String Get(
        /* [in] */ ArrayOf<Char32>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

private:
    static Boolean ContentEquals(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

private:
    AutoPtr<ArrayOf<String> > mPool;// new String[512];
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_STRINGPOOL_H__