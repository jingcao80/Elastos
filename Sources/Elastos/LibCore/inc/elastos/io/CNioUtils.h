#ifndef __ELASTOS_IO_CNIOUTILSHELPER_H__
#define __ELASTOS_IO_CNIOUTILSHELPER_H__

#include "_Elastos_IO_CNioUtils.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {

CarClass(CNioUtils)
    , public Singleton
    , public INioUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FreeDirectBuffer(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Returns the int file descriptor from within the given FileChannel 'fc'.
     */
    CARAPI GetFD(
        /* [in] */ IFileChannel* fc,
        /* [out] */ IFileDescriptor** outfd);

    /**
     * Helps bridge between io and nio.
     */
    CARAPI NewFileChannel(
        /* [in] */ ICloseable* ioObject,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 mode,
        /* [out] */ IFileChannel** outchannel);

    /**
     * Exposes the array backing a non-direct ByteBuffer, even if the ByteBuffer is read-only.
     * Normally, attempting to access the array backing a read-only buffer throws.
     */
    CARAPI GetUnsafeArray(
        /* [in] */ IByteBuffer* b,
        /* [out, callee] */ ArrayOf<Byte>** outbyte);

    /**
     * Exposes the array offset for the array backing a non-direct ByteBuffer,
     * even if the ByteBuffer is read-only.
     */
    CARAPI GetUnsafeArrayOffset(
        /* [in] */ IByteBuffer* b,
        /* [out] */ Int32* value);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CNIOUTILSHELPER_H__
