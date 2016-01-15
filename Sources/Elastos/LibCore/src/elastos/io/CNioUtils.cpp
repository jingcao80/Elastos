
#include "CNioUtils.h"
#include "NioUtils.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CNioUtils, Singleton, INioUtils)

CAR_SINGLETON_IMPL(CNioUtils)

ECode CNioUtils::FreeDirectBuffer(
    /* [in] */ IByteBuffer* buffer)
{
    return NioUtils::FreeDirectBuffer(buffer);
}

ECode CNioUtils::GetFD(
    /* [in] */ IFileChannel* fc,
    /* [out] */ IFileDescriptor** outfd)
{
    VALIDATE_NOT_NULL(outfd)
    AutoPtr<IFileDescriptor> fd = NioUtils::GetFD(fc);
    *outfd = fd;
    REFCOUNT_ADD(*outfd)
    return NOERROR;
}

ECode CNioUtils::NewFileChannel(
    /* [in] */ ICloseable* ioObject,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode,
    /* [out] */ IFileChannel** outchannel)
{
    VALIDATE_NOT_NULL(outchannel)

    AutoPtr<IFileChannel> res = NioUtils::NewFileChannel(ioObject, fd, mode);
    *outchannel = res;
    REFCOUNT_ADD(*outchannel)
    return NOERROR;
}

ECode CNioUtils::GetUnsafeArray(
    /* [in] */ IByteBuffer* b,
    /* [out, callee] */ ArrayOf<Byte>** outbyte)
{
    VALIDATE_NOT_NULL(outbyte)

    AutoPtr< ArrayOf<Byte> > res = NioUtils::GetUnsafeArray(b);
    *outbyte = res;
    REFCOUNT_ADD(*outbyte)
    return NOERROR;
}

ECode CNioUtils::GetUnsafeArrayOffset(
    /* [in] */ IByteBuffer* b,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = NioUtils::GetUnsafeArrayOffset(b);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
