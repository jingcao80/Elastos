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

#ifndef __ELASTOS_IO_CINT16BUFFERHELPER_H__
#define __ELASTOS_IO_CINT16BUFFERHELPER_H__

#include "_Elastos_IO_CInt16BufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CInt16BufferHelper)
    , public Singleton
    , public IInt16BufferHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a short buffer based on a newly allocated short array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created short buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt16Buffer** int16Buffer);

    /**
     * Creates a new short buffer by wrapping the given short array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the short array which the new buffer will be based on.
     * @return the created short buffer.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int16>* array,
        /* [out] */ IInt16Buffer** int16Buffer);

    /**
     * Creates a new short buffer by wrapping the given short array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + shortCount}, capacity will be the length of the array.
     *
     * @param array
     *            the short array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param shortCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created short buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code shortCount} is invalid.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int16>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int16Count,
        /* [out] */ IInt16Buffer** int16Buffer);

private:
    // TODO: Add your private member variables here.
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CINT16BUFFERHELPER_H__
