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

#ifndef __ELASTOS_IO_CDOUBLEBUFFERHELPER_H__
#define __ELASTOS_IO_CDOUBLEBUFFERHELPER_H__

#include "_Elastos_IO_CDoubleBufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CDoubleBufferHelper)
    , public Singleton
    , public IDoubleBufferHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a double buffer based on a newly allocated double array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created double buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IDoubleBuffer** doubleBuf);

    /**
     * Creates a new double buffer by wrapping the given double array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the double array which the new buffer will be based on.
     * @return the created double buffer.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Double>* array,
        /* [out] */ IDoubleBuffer** doubleBuf);

    /**
     * Creates a new double buffer by wrapping the given double array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + doubleCount}, capacity will be the length of the array.
     *
     * @param array
     *            the double array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param doubleCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created double buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code doubleCount} is invalid.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Double>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 doubleCount,
        /* [out] */ IDoubleBuffer** doubleBuf);

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CCHARBUFFERHELPER_H__
