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

#ifndef __ELASTOS_IO_MAPPEDBYTEBUFFER_H__
#define __ELASTOS_IO_MAPPEDBYTEBUFFER_H__

#include "ByteBuffer.h"
#include "MemoryBlock.h"

using Elastos::IO::Channels::FileChannelMapMode;

namespace Elastos {
namespace IO {

/**
 * {@code MappedByteBuffer} is a special kind of direct byte buffer which maps a
 * region of file to memory.
 * <p>
 * {@code MappedByteBuffer} can be created by calling
 * {@link java.nio.channels.FileChannel#map(java.nio.channels.FileChannel.MapMode, long, long) FileChannel.map}.
 * Once created, the mapping between the byte buffer and the file region remains
 * valid until the byte buffer is garbage collected.
 * <p>
 * All or part of a {@code MappedByteBuffer}'s content may change or become
 * inaccessible at any time, since the mapped file region can be modified by
 * another thread or process at any time. If this happens, the behavior of the
 * {@code MappedByteBuffer} is undefined.
 */
class MappedByteBuffer
    : public ByteBuffer
    , public IMappedByteBuffer
{
public:
    MappedByteBuffer();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ MemoryBlock* block,
        /* [in] */ Int32 capacity,
        /* [in] */ FileChannelMapMode mapMode,
        /* [in] */ Int64 effectiveDirectAddress);

    /**
     * Returns true if there is a high probability that every page of this buffer is currently
     * loaded in RAM, meaning that accesses will not cause a page fault. It is impossible to give
     * a strong guarantee since this is only a snapshot of a dynamic situation.
     */
    CARAPI IsLoaded(
        /* [out] */ Boolean* isLoaded);

    /**
     * Attempts to load every page of this buffer into RAM. See {@link #isLoaded}.
     * @return this buffer.
     */
    CARAPI Load();

    /**
     * Writes all changes of the buffer to the mapped file. If the mapped file
     * is stored on a local device, it is guaranteed that the changes are
     * written to the file. No such guarantee is given if the file is located on
     * a remote device.
     *
     * @return this buffer.
     */
    CARAPI Force();

private:
    // DirectByteBuffer is a subclass of MappedByteBuffer, but not all DirectByteBuffers
    // actually correspond to an mmap(2)ed region.
    CARAPI CheckIsMapped();

public:
    AutoPtr<MemoryBlock> mBlock;

    FileChannelMapMode mMapMode;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_MAPPEDBYTEBUFFER_H__
