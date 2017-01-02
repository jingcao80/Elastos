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

#include "ByteArrayOutputStream.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(ByteArrayOutputStream, OutputStream, IByteArrayOutputStream)

ByteArrayOutputStream::ByteArrayOutputStream()
    : mCount(0)
{
}

ByteArrayOutputStream::~ByteArrayOutputStream()
{
}

ECode ByteArrayOutputStream::constructor()
{
    mBuf = ArrayOf<Byte>::Alloc(32);
    return NOERROR;
}

ECode ByteArrayOutputStream::constructor(
    /* [in] */ Int32 size)
{
    if (size < 0) {
//      throw new IllegalArgumentException("size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuf = ArrayOf<Byte>::Alloc(size);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    return NOERROR;
}

ECode ByteArrayOutputStream::Close()
{
    /**
     * Although the spec claims "A closed stream cannot perform output
     * operations and cannot be reopened.", this implementation must do
     * nothing.
     */
    return OutputStream::Close();
}

ECode ByteArrayOutputStream::Expand(
    /* [in] */ Int32 i)
{
    /* Can the buffer handle @i more bytes, if not expand it */
    if (mCount + i <= mBuf->GetLength()){
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > newbuf = ArrayOf<Byte>::Alloc((mCount + i) * 2);
    if (newbuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    memcpy(newbuf->GetPayload(), mBuf->GetPayload(), mCount);
    mBuf = newbuf;

    return NOERROR;
}

ECode ByteArrayOutputStream::Reset()
{
    AutoLock lock(this);

    mCount = 0;
    return NOERROR;
}

ECode ByteArrayOutputStream::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mCount;
    return NOERROR;
}

ECode ByteArrayOutputStream::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** byteArray)
{
    VALIDATE_NOT_NULL(byteArray)

    AutoLock lock(this);

    AutoPtr< ArrayOf<Byte> > newArray = ArrayOf<Byte>::Alloc(mCount);
    newArray->Copy(mBuf, mCount);
    *byteArray = newArray;
    REFCOUNT_ADD(*byteArray);
    return NOERROR;
}

ECode ByteArrayOutputStream::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String((const char*)mBuf->GetPayload(), mCount);
    return NOERROR;
}

ECode ByteArrayOutputStream::ToString(
    /* [in] */ Int32 hibyte,
    /* [out] */ String* rev)
{
    VALIDATE_NOT_NULL(rev)

    Int32 size;
    GetSize(&size);
    AutoPtr<ArrayOf<Char32> > newBuf = ArrayOf<Char32>::Alloc(size);
    for (Int32 i = 0; i < newBuf->GetLength(); ++i) {
        (*newBuf)[i] = (Char32) (((hibyte & 0xff) << 8) | ((*mBuf)[i] & 0xff));
    }
    *rev = String(*newBuf);
    return NOERROR;
}

ECode ByteArrayOutputStream::ToString(
    /* [in] */ const String& enc,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String((const char*)mBuf->GetPayload(), mCount) + enc;
    return NOERROR;
}

ECode ByteArrayOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoLock lock(this);

    if (mCount == mBuf->GetLength()){
        Expand(1);
    }
    (*mBuf)[mCount++] = (Byte)oneByte;

    return NOERROR;
}

ECode ByteArrayOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(buffer)
    AutoLock lock(this);

    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // removed redundant check, made implicit null check explicit,
    // used (offset | len) < 0 instead of (offset < 0) || (len < 0)
    // to safe one operation
    if ((offset | count) < 0 || count > buffer->GetLength() - offset) {
//      throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    if (count == 0) {
        return NOERROR;
    }
    /* Expand if necessary */
    Expand(count);
    memcpy(mBuf->GetPayload() + mCount, buffer->GetPayload() + offset, count);
    mCount += count;

    return NOERROR;
}

ECode ByteArrayOutputStream::WriteTo(
    /* [in] */ IOutputStream* out)
{
    VALIDATE_NOT_NULL(out);

    AutoLock lock(this);

    return out->Write(mBuf, 0, mCount);
}

} // namespace IO
} // namespace Elastos
