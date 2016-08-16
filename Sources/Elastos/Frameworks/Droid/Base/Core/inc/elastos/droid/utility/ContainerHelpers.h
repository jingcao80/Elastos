
#ifndef __ELASTOS_DROID_UTILITY_CONTAINERHELPERS_H__
#define __ELASTOS_DROID_UTILITY_CONTAINERHELPERS_H__

#include <elastos.h>

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace Utility {

class ContainerHelpers
{
public:
    // This is Arrays.binarySearch(), but doesn't do any argument validation.
    template<typename T>
    static Int32 BinarySearch(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ Int32 size,
        /* [in] */ T value)
    {
        Int32 lo = 0;
        Int32 hi = size - 1;

        while (lo <= hi) {
            Int32 mid = ((UInt32)(lo + hi)) >> 1;
            T midVal = (*array)[mid];

            if (midVal < value) {
                lo = mid + 1;
            } else if (midVal > value) {
                hi = mid - 1;
            } else {
                return mid;  // value found
            }
        }
        return ~lo;  // value not present
    }

    template<typename T>
    static Int32 BinarySearch(
        /* [in] */ const AutoPtr<ArrayOf<T> >& array,
        /* [in] */ Int32 size,
        /* [in] */ T value)
    {
        return BinarySearch(array.Get(), size, value);
    }

private:
    ContainerHelpers();

};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CONTAINERHELPERS_H__
