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
