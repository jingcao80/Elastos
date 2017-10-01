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

#ifndef __ELASTOS_IO_MEMORYBLOCK_H__
#define __ELASTOS_IO_MEMORYBLOCK_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::Channels::FileChannelMapMode;

namespace Elastos {
namespace IO {

class MemoryBlock : public Object
{
public:
    virtual ~MemoryBlock() {}

    static CARAPI Mmap(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 size,
        /* [in] */ FileChannelMapMode mapMode,
        /* [out] */ MemoryBlock** mb);

    static CARAPI_(AutoPtr<MemoryBlock>) Allocate(
        /* [in] */ Int32 byteCount);

    static CARAPI_(AutoPtr<MemoryBlock>) WrapFromNative(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount);

    // Used to support array/arrayOffset/hasArray for direct buffers
    virtual CARAPI_(AutoPtr< ArrayOf<Byte> >) GetArray() { return NULL; }

    virtual CARAPI Free() { return NOERROR; }

    CARAPI_(void) PokeByte(
        /* [in] */ Int32 offset,
        /* [in] */ Byte value);

    CARAPI_(void) PokeByteArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 byteCount);

    CARAPI_(void) PokeCharArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeDoubleArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Double>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeFloatArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 floatCount,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeInt32Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int32Count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeInt64Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int64>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int64Count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeInt16Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int16>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int16Count,
        /* [in] */ Boolean swap);

    CARAPI_(Byte) PeekByte(
        /* [in] */ Int32 offset);

    CARAPI_(void) PeekByteArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count);

    CARAPI_(void) PeekCharArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PeekDoubleArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PeekFloatArray(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PeekInt32Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PeekInt64Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PeekInt16Array(
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean swap);

    CARAPI_(void) PokeInt16(
        /* [in] */ Int32 offset,
        /* [in] */ Int16 value,
        /* [in] */ ByteOrder order);

    CARAPI_(Int16) PeekInt16(
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    CARAPI_(void) PokeInt32(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 value,
        /* [in] */ ByteOrder order);

    CARAPI_(Int32) PeekInt32(
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    CARAPI_(void) PokeInt64(
        /* [in] */ Int32 offset,
        /* [in] */ Int64 value,
        /* [in] */ ByteOrder order);

    CARAPI_(Int64) PeekInt64(
        /* [in] */ Int32 offset,
        /* [in] */ ByteOrder order);

    CARAPI_(Int64) ToInt64();

    using Object::ToString;

    CARAPI_(String) ToString();

    CARAPI_(Int64) GetSize();

    CARAPI_(Boolean) IsFreed();

    CARAPI_(Boolean) IsAccessible();

    CARAPI_(void) SetAccessible(
        /* [in] */ Boolean accessible);

    CARAPI_(Byte *) GetRawAddress()
    {
        return reinterpret_cast<Byte *>(mAddress);
    }

protected:
    MemoryBlock(
        /* [in] */ Int64 address,
        /* [in] */ Int64 size)
        : mAddress(address)
        , mSize(size)
        , mAccessible(TRUE)
        , mFreed(FALSE)
    {}

protected:
    Int64 mAddress;
    Int64 mSize;

private:
    Boolean mAccessible;
    Boolean mFreed;
};

/**
 * Handle calling munmap(2) on a memory-mapped region.
 */
class MemoryMappedBlock : public MemoryBlock
{
    friend class MemoryBlock;

public:
    CARAPI Free();

protected:
    ~MemoryMappedBlock()
    {
        Free();
    }

private:
    MemoryMappedBlock(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount)
        : MemoryBlock(address, byteCount)
    {}
};

class NonMovableHeapBlock : public MemoryBlock
{
    friend class MemoryBlock;

public:
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetArray();

    CARAPI Free();

private:
    NonMovableHeapBlock(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount)
        : MemoryBlock(address, byteCount)
        , mArray(array)
    {}

private:
    AutoPtr< ArrayOf<Byte> > mArray;
};

/**
 * Represents a block of memory we don't own. (We don't take ownership of memory corresponding
 * to direct buffers created by the JNI NewDirectByteBuffer function.)
 */
class UnmanagedBlock : public MemoryBlock
{
    friend class MemoryBlock;

private:
    UnmanagedBlock(
        /* [in] */ Int64 address,
        /* [in] */ Int64 byteCount)
        : MemoryBlock(address, byteCount)
    {}
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_MEMORYBLOCK_H__
