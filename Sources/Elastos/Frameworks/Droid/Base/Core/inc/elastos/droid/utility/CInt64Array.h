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

#ifndef __ELASTOS_DROID_UTILITY_CINT64ARRAY_H__
#define __ELASTOS_DROID_UTILITY_CINT64ARRAY_H__

#include "_Elastos_Droid_Utility_CInt64Array.h"
#include <elastos/core/Object.h>

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CInt64Array)
    , public Object
    , public IInt64Array
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInt64Array();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    /**
     * Appends the specified value to the end of this array.
     */
    CARAPI Add(
        /* [in] */ Int64 value);

    /**
     * Inserts a value at the specified position in this array.
     *
     * @throws IndexOutOfBoundsException when index &lt; 0 || index &gt; size()
     */
    CARAPI Add(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    /**
     * Adds the values in the specified array to this array.
     */
    CARAPI AddAll(
        /* [in] */ IInt64Array* values);

    /**
     * Removes all values from this array.
     */
    CARAPI Clear();

    CARAPI Clone(
        /* [out] */ IInterface** clone);

    /**
     * Returns the value at the specified position in this array.
     */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    /**
     * Returns the index of the first occurrence of the specified value in this
     * array, or -1 if this array does not contain the value.
     */
    CARAPI IndexOf(
        /* [in] */ Int64 value,
        /* [out] */ Int32* index);

    /**
     * Removes the value at the specified index from this array.
     */
    CARAPI Remove(
        /* [in] */ Int32 index);

    /**
     * Returns the number of values in this array.
     */
    CARAPI GetSize(
        /* [out] */ Int32* value);

private:
    /**
     * Ensures capacity to append at least <code>count</code> values.
     */
    CARAPI_(void) EnsureCapacity(
        /* [in] */ Int32 count);

private:
    static const Int32 MIN_CAPACITY_INCREMENT;

    AutoPtr< ArrayOf<Int64> > mValues;
    Int32 mSize;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CINT64ARRAY_H__
