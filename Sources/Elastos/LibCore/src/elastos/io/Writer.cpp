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

#include "Writer.h"
#include "Character.h"
#include "AutoLock.h"

using Elastos::Core::Character;
using Elastos::Core::EIID_IAppendable;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_4(Writer, Object, IWriter, IAppendable, ICloseable, IFlushable)

Writer::Writer()
    : mLock(NULL)
    , mIsStrongLock(FALSE)
{
}

Writer::Writer(
    /* [in] */ ISynchronize* lock)
    : mIsStrongLock(TRUE)
{
    assert(lock != NULL);
    mLock = lock;
    REFCOUNT_ADD(mLock);
}

Writer::~Writer()
{
    if (mIsStrongLock) {
        REFCOUNT_RELEASE(mLock)
    }
}

ECode Writer::constructor()
{
    mLock = this;
    mIsStrongLock = FALSE;
    return NOERROR;
}

ECode Writer::constructor(
    /* [in] */ ISynchronize* lock)
{
    assert(lock != NULL);
    VALIDATE_NOT_NULL(lock)

    mLock = lock;
    if (mLock && mLock != this) {
        REFCOUNT_ADD(mLock)
        mIsStrongLock = TRUE;
    }
    return NOERROR;
}

ECode Writer::Write(
    /* [in] */ Int32 oneChar32)
{
    AutoLock lock(mLock);

    AutoPtr<ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(1);
    (*buf)[0] = oneChar32;
    return Write(buf);
}

ECode Writer::Write(
    /* [in] */ ArrayOf<Char32>* buffer)
{
    VALIDATE_NOT_NULL(buffer)
    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note

    return Write(buffer, 0, buffer->GetLength());
}

ECode Writer::Write(
    /* [in] */ const String& str)
{
    return Write(str, 0, str.GetLength());
}

ECode Writer::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if ((offset | count) < 0 || offset > str.GetLength() - count) {
//      throw new StringIndexOutOfBoundsException();
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoLock lock(mLock);

    AutoPtr<ArrayOf<Char32> > buf = str.GetChars(offset, offset + count);
    return Write(buf, 0, buf->GetLength());
}

ECode Writer::AppendChar(
    /* [in] */ Char32 c)
{
    return Write(c);
}

ECode Writer::Append(
    /* [in] */ ICharSequence* csq)
{
    String tmp("NULL");
    if (NULL != csq) {
        csq->ToString(&tmp);
    }

    return Write(tmp);
}

ECode Writer::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String tmp("NULL");
    if (NULL != csq) {
        csq->ToString(&tmp);
    }

    return Write(tmp.Substring(start, end));
}

ECode Writer::CheckError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError)

    *hasError = FALSE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
