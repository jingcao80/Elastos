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

#ifndef __ELASTOS_IO_IOVEC_H__
#define __ELASTOS_IO_IOVEC_H__

#include <elastos/core/Object.h>
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {

/**
 * Used to implement java.nio read(ByteBuffer[])/write(ByteBuffer[]) operations as POSIX readv(2)
 * and writev(2) calls.
 */
class IoVec
    : public Object
{
public:
    enum Direction { Direction_READV, Direction_WRITEV };

public:
    IoVec(
        /* [in] */ ArrayOf<IByteBuffer*>* byteBuffers,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 bufferCount,
        /* [in] */ Direction direction);

    CARAPI Init(
        /* [out] */ Int32* totalRemaining);

    CARAPI DoTransfer(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

    CARAPI DidTransfer(
        /* [in] */ Int32 byteCount);

private:
    AutoPtr< ArrayOf<IByteBuffer*> > mByteBuffers;
    Int32 mOffset;
    Int32 mBufferCount;

    AutoPtr< ArrayOf<IInterface*> > mIoBuffers;
    AutoPtr< ArrayOf<Int32> > mOffsets;
    AutoPtr< ArrayOf<Int32> > mByteCounts;

    Direction mDirection;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_IOVEC_H__
