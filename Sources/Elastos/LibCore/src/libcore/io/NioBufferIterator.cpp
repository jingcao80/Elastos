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

#include "NioBufferIterator.h"
#include "io/Memory.h"

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(NioBufferIterator, Object, IBufferIterator)

/**
 * Seeks to the absolute position {offset}, measured in bytes from the start.
 */
ECode NioBufferIterator::Seek(
    /* [in] */ Int32 offset)
{
    mPosition = offset;
    return NOERROR;
}

/**
 * Skips forwards or backwards {byteCount} bytes from the current position.
 */
ECode NioBufferIterator::Skip(
    /* [in] */ Int32 byteCount)
{
    mPosition += byteCount;
    return NOERROR;
}

/**
 * Copies {byteCount} bytes from the current position into {dst}, starting at
 * {dstOffset}, and advances the current position {byteCount} bytes.
 */
ECode NioBufferIterator::ReadByteArray(
    /* [in] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount)
{
    Memory::PeekByteArray(mAddress + mPosition, dst, dstOffset, byteCount);
    mPosition += byteCount;
    return NOERROR;
}

/**
 * Returns the byte at the current position, and advances the current position one byte.
 */
ECode NioBufferIterator::ReadByte(
    /* [out] */ Byte* result)
{
    *result = Memory::PeekByte(mAddress + mPosition);
    ++mPosition;
    return NOERROR;
}

/**
 * Returns the 32-bit int at the current position, and advances the current position four bytes.
 */
ECode NioBufferIterator::ReadInt32(
    /* [out] */ Int32* result)
{
    *result = Memory::PeekInt32(mAddress + mPosition, mSwap);
    mPosition += sizeof(Int32);
    return NOERROR;
}

/**
 * Copies {intCount} 32-bit ints from the current position into {dst}, starting at
 * {dstOffset}, and advances the current position {4 * intCount} bytes.
 */
ECode NioBufferIterator::ReadInt32Array(
    /* [in] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 intCount)
{
    Memory::PeekInt32Array(mAddress + mPosition, dst, dstOffset, intCount, mSwap);
    mPosition += sizeof(Int32) * intCount;
    return NOERROR;
}

/**
 * Returns the 16-bit short at the current position, and advances the current position two bytes.
 */
ECode NioBufferIterator::ReadInt16(
    /* [out] */ Int16* result)
{
    *result = Memory::PeekInt16(mAddress + mPosition, mSwap);
    mPosition += sizeof(Int16);
    return NOERROR;
}

NioBufferIterator::NioBufferIterator(
    /* [in] */ Int64 address,
    /* [in] */ Int32 size,
    /* [in] */ Boolean swap)
    : mAddress(address)
    , mSize(size)
    , mSwap(swap)
    , mPosition(0)
{}

} // namespace IO
} // namespace Libcore