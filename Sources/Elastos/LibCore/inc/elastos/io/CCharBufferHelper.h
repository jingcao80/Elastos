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

#ifndef __ELASTOS_IO_CCHARBUFFERHELPER_H__
#define __ELASTOS_IO_CCHARBUFFERHELPER_H__

#include "_Elastos_IO_CCharBufferHelper.h"
#include "Singleton.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

CarClass(CCharBufferHelper)
    , public Singleton
    , public ICharBufferHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /*
     * Creates a char buffer based on a newly allocated char array.
     *
     * @param capacity
     *      the capacity of the new buffer.
     * @param charBuf
     *      the char buffer interface.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ ICharBuffer** charBuf);

    /*
     * Creates a char buffer by wrapping the given char array.
     * <p>
     * Calling this method has the same effect as
     * Wrap();
     *
     * @param array
     *      the char array which the new buffer will be based on.
     * @param charBuf
     *      the char buffer interface.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Char32>* array,
        /* [out] */ ICharBuffer** charBuf);

    /*
     * Creates a char buffer by wrapping the given char array.
     * <p>
     * The new buffer's position will be start, limit will be
     * charCount, capacity will be the length of the array.
     *
     * @param array
     *      the char array which the new buffer will be based on.
     * @param start
     *      the start index, must not be negative and not greater
     *      than the array.length.
     * @param charCount
     *      the length, must not be negative and not greater than
     *      length - start.
     * @param charBuf
     *      the char buffer interface.
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<Char32>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 charCount,
        /* [out] */ ICharBuffer** charBuf);

    /*
     * Creates a char buffer by wrapping the given char sequence.
     * <p>
     * Calling this method has the same effect as
     * Wrap();
     *
     * @param chseq
     *      the char sequence which the new buffer will be based on.
     * @param charBuf
     *      the char buffer interface.
     */
    CARAPI Wrap(
        /* [in] */ ICharSequence* charSequence,
        /* [out] */ ICharBuffer**  charBuf);

    /*
     * Creates a char buffer by wrapping the given char sequence.
     * <p>
     * The new buffer's position will be start, limit will be
     * end, capacity will be the length of the char sequence.
     * The new buffer is read-only.
     *
     * @param cs
     *      the char sequence which the new buffer will be based on.
     * @param start
     *      then start index, must not be negative and not greater
     *      than cs.length().
     * @param end
     *      the end index, must be no less than start, and no greater
     *      than cs.length().
     * @param charBuf
     *      the char buffer interface.
     */
    CARAPI Wrap(
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharBuffer** charBuf);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CCHARBUFFERHELPER_H__
