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

#ifndef __ELASTOS_IO_CINT32BUFFERHELPER_H__
#define __ELASTOS_IO_CINT32BUFFERHELPER_H__

#include "_Elastos_IO_CInt32BufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CInt32BufferHelper)
    , public Singleton
    , public IInt32BufferHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates an int buffer based on a newly allocated int array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created int buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt32Buffer** int32Buffer);

    /**
     * Creates a new int buffer by wrapping the given int array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the int array which the new buffer will be based on.
     * @return the created int buffer.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int32>* array,
        /* [out] */ IInt32Buffer** int32Buffer);

    /**
     * Creates a new int buffer by wrapping the given int array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + intCount}, capacity will be the length of the array.
     *
     * @param array
     *            the int array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}
     * @param intCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created int buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code intCount} is invalid.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int32>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int32Count,
        /* [out] */ IInt32Buffer** int32Buffer);

private:
    // TODO: Add your private member variables here.
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CINT32BUFFERHELPER_H__
