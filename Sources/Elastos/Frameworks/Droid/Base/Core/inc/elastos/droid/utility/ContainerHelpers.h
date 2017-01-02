//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
