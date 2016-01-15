#ifndef __ELASTOS_IO_NIOUTILS_H__
#define __ELASTOS_IO_NIOUTILS_H__

#include "Buffer.h"
#include "ByteBuffer.h"

using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {
/*
 * @ hilde internal use only
 */
class NioUtils
{
public:
    static CARAPI FreeDirectBuffer(
        /* [in] */ IByteBuffer* buffer);

    static AutoPtr<IFileDescriptor> GetFD(
        /* [in] */ IFileChannel* fc);

    static AutoPtr<IFileChannel> NewFileChannel(
        /* [in] */ ICloseable *stream,
        /* [in] */ IFileDescriptor *fd,
        /* [in] */ Int32 mode);

    static AutoPtr<ArrayOf<Byte> > GetUnsafeArray(
        /* [in] */ IByteBuffer* b);

    static Int32 GetUnsafeArrayOffset(
        /* [in] */ IByteBuffer* b);
private:
    NioUtils() {}
};

} // namespace IO
} // namespace Elastos

#endif
