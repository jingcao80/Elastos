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

#include "HeapBufferIterator.h"
#include "io/Memory.h"
#include "io/CByteOrderHelper.h"

// using Elastos::IO::Memory;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteOrderHelper;

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(HeapBufferIterator, Object, IBufferIterator)

/**
 * Seeks to the absolute position {offset}, measured in bytes from the start.
 */
ECode HeapBufferIterator::Seek(
    /* [in] */ Int32 offset)
{
    mPosition = offset;
    return NOERROR;
}

/**
 * Skips forwards or backwards {byteCount} bytes from the current position.
 */
ECode HeapBufferIterator::Skip(
    /* [in] */ Int32 byteCount)
{
    mPosition += byteCount;
    return NOERROR;
}

/**
 * Copies {byteCount} bytes from the current position into {dst}, starting at
 * {dstOffset}, and advances the current position {byteCount} bytes.
 */
ECode HeapBufferIterator::ReadByteArray(
    /* [in] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount)
{
    dst->Copy(mOffset + mPosition, mBuffer, dstOffset, byteCount);
    return NOERROR;
}

/**
 * Returns the byte at the current position, and advances the current position one byte.
 */
ECode HeapBufferIterator::ReadByte(
    /* [out] */ Byte* result)
{
    *result = (*mBuffer)[mOffset + mPosition];
    ++mPosition;
    return NOERROR;
}

/**
 * Returns the 32-bit int at the current position, and advances the current position four bytes.
 */
ECode HeapBufferIterator::ReadInt32(
    /* [out] */ Int32* result)
{
    *result = Memory::PeekInt32(mBuffer, mOffset + mPosition, mOrder);
    mPosition += sizeof(Int32);
    return NOERROR;
}

/**
 * Copies {intCount} 32-bit ints from the current position into {dst}, starting at
 * {dstOffset}, and advances the current position {4 * intCount} bytes.
 */
ECode HeapBufferIterator::ReadInt32Array(
    /* [in] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 intCount)
{
    Int32 byteCount = intCount * sizeof(Int32);
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    Boolean needsSwap;
    helper->IsNeedsSwap(mOrder, &needsSwap);
    Memory::UnsafeBulkGet((Byte*)(dst->GetPayload()), dstOffset, byteCount, mBuffer, mOffset + mPosition, sizeof(Int32), needsSwap);
    mPosition += byteCount;
    return NOERROR;
}

/**
 * Returns the 16-bit short at the current position, and advances the current position two bytes.
 */
ECode HeapBufferIterator::ReadInt16(
    /* [out] */ Int16* result)
{
    *result = Memory::PeekInt16(mBuffer, mOffset + mPosition, mOrder);
    mPosition += sizeof(Int16);
    return NOERROR;
}

AutoPtr<IBufferIterator> HeapBufferIterator::Iterator(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ ByteOrder order)
{
    return new HeapBufferIterator(buffer, offset, byteCount, order);
}

HeapBufferIterator::HeapBufferIterator(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ ByteOrder order)
    : mBuffer(buffer)
    , mOffset(offset)
    , mByteCount(byteCount)
    , mOrder(order)
    , mPosition(0)
{

}

} // namespace IO
} // namespace Libcore
