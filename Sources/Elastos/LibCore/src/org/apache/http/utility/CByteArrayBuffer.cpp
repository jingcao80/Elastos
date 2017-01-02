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

#include "org/apache/http/utility/CByteArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

CByteArrayBuffer::CByteArrayBuffer()
    : mLen(0)
{}

CAR_OBJECT_IMPL(CByteArrayBuffer)

CAR_INTERFACE_IMPL(CByteArrayBuffer, Object, IByteArrayBuffer)

void CByteArrayBuffer::Expand(
    /* [in] */ Int32 newlen)
{
    using Elastos::Core::Math;

    AutoPtr< ArrayOf<Byte> > newbuffer = ArrayOf<Byte>::Alloc(Math::Max(mBuffer->GetLength() << 1, newlen));
    newbuffer->Copy(0, mBuffer, 0, mLen);
    mBuffer = newbuffer;
}

ECode CByteArrayBuffer::Append(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    if ((off < 0) || (off > b->GetLength()) || (len < 0) ||
            ((off + len) < 0) || ((off + len) > b->GetLength())) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (len == 0) {
        return NOERROR;
    }
    Int32 newlen = mLen + len;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    mBuffer->Copy(mLen, b, off, len);
    mLen = newlen;
    return NOERROR;
}

ECode CByteArrayBuffer::Append(
    /* [in] */ Int32 b)
{
    Int32 newlen = mLen + 1;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    (*mBuffer)[mLen] = (Byte)b;
    mLen = newlen;
    return NOERROR;
}

ECode CByteArrayBuffer::Append(
    /* [in] */ ArrayOf<Char32>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    if ((off < 0) || (off > b->GetLength()) || (len < 0) ||
            ((off + len) < 0) || ((off + len) > b->GetLength())) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (len == 0) {
        return NOERROR;
    }
    Int32 oldlen = mLen;
    Int32 newlen = oldlen + len;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    for (Int32 i1 = off, i2 = oldlen; i2 < newlen; i1++, i2++) {
        (*mBuffer)[i2] = (Byte)(*b)[i1];
    }
    mLen = newlen;
    return NOERROR;
}

ECode CByteArrayBuffer::Append(
    /* [in] */ ICharArrayBuffer* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<Char32> > buf;
    b->GetBuffer((ArrayOf<Char32>**)&buf);
    return Append(buf, off, len);
}

ECode CByteArrayBuffer::Clear()
{
    mLen = 0;
    return NOERROR;
}

ECode CByteArrayBuffer::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** byteArray)
{
    VALIDATE_NOT_NULL(byteArray)
    AutoPtr< ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(mLen);
    if (mLen > 0) {
        b->Copy(0, mBuffer, 0, mLen);
    }
    *byteArray = b;
    REFCOUNT_ADD(*byteArray)
    return NOERROR;
}

ECode CByteArrayBuffer::ByteAt(
    /* [in] */ Int32 i,
    /* [out] */ Int32* byteAt)
{
    VALIDATE_NOT_NULL(byteAt)
    *byteAt = (*mBuffer)[i];
    return NOERROR;
}

ECode CByteArrayBuffer::GetCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity)
    *capacity = mBuffer->GetLength();
    return NOERROR;
}

ECode CByteArrayBuffer::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLen;
    return NOERROR;
}

ECode CByteArrayBuffer::GetBuffer(
    /* [out, callee] */ ArrayOf<Byte>** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = mBuffer;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CByteArrayBuffer::SetLength(
    /* [in] */ Int32 len)
{
    if (len < 0 || len > mBuffer->GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mLen = len;
    return NOERROR;
}

ECode CByteArrayBuffer::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty)
    *isEmpty = mLen == 0;
    return NOERROR;
}

ECode CByteArrayBuffer::IsFull(
    /* [out] */ Boolean* isFull)
{
    VALIDATE_NOT_NULL(isFull)
    *isFull = mBuffer->GetLength() == mLen;
    return NOERROR;
}

ECode CByteArrayBuffer::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity < 0) {
        Logger::E("CByteArrayBuffer", "Buffer capacity may not be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuffer = ArrayOf<Byte>::Alloc(capacity);
    return NOERROR;
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org
