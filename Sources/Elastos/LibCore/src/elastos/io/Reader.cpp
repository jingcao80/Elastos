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

#include "Reader.h"
#include "Character.h"
#include "Math.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_3(Reader, Object, IReadable, IReader, ICloseable)

Reader::Reader()
    : mLock(NULL)
    , mIsStrongLock(FALSE)
{
}

Reader::~Reader()
{
    if (mIsStrongLock) {
        REFCOUNT_RELEASE(mLock)
    }
}

ECode Reader::constructor()
{
    mLock = this;
    mIsStrongLock = FALSE;
    return NOERROR;
}

ECode Reader::constructor(
    /* [in] */ ISynchronize* lock)
{
    VALIDATE_NOT_NULL(lock)

    mLock = lock;
    if (mLock && mLock != this) {
        REFCOUNT_ADD(mLock)
        mIsStrongLock = TRUE;
    }
    return NOERROR;
}

ECode Reader::Mark(
    /* [in] */ Int32 readLimit)
{
//    throw new IOException();
    return E_IO_EXCEPTION;
}

ECode Reader::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = FALSE;
    return NOERROR;
}

ECode Reader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoLock lock(mLock);

    AutoPtr< ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(1);
    Int32 number;
    FAIL_RETURN(Read(buf, 0, 1, &number));
    if (number != -1) {
        *value = (*buf)[0];
    }
    else
        *value = -1;

    return NOERROR;
}

ECode Reader::Read(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode Reader::IsReady(
    /* [out] */ Boolean* ready)
{
    VALIDATE_NOT_NULL(ready)
    *ready = FALSE;
    return NOERROR;
}

ECode Reader::Reset()
{
    return E_IO_EXCEPTION;
}

ECode Reader::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (count < 0) {
//      throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mLock);

    Int64 skipped = 0;
    Int32 toRead = count < 512 ? (Int32)count : 512;
    AutoPtr<ArrayOf<Char32> > charsSkipped = ArrayOf<Char32>::Alloc(toRead);
    Int32 read;
    while (skipped < count) {
        FAIL_RETURN(Read(charsSkipped, 0, toRead, &read));
        if (read == -1) {
            *number = skipped;
            return NOERROR;
        }

        skipped += read;
        if (read < toRead) {
            *number = skipped;
            return NOERROR;
        }
        if (count - skipped < toRead) {
            toRead = (Int32)(count - skipped);
        }
    }
    *number = skipped;
    return NOERROR;
}

ECode Reader::Read(
    /* [in] */ ICharBuffer* target,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    Int32 length;
    target->GetLength(&length);
    AutoPtr<ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(length);
    FAIL_RETURN(Read(buf, number));
    length = Elastos::Core::Math::Min(length, *number);
    if (length > 0) {
        target->Put(buf, 0, length);
    }
    *number = length;

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
