#include "CByteBufferHelper.h"
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CByteBufferHelper, Singleton, IByteBufferHelper)

CAR_SINGLETON_IMPL(CByteBufferHelper)

ECode CByteBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Allocate(capacity, byteBuf);
}

ECode CByteBufferHelper::AllocateDirect(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::AllocateDirect(capacity, byteBuf);
}

ECode CByteBufferHelper::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Wrap(array, byteBuf);
}

ECode CByteBufferHelper::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 byteCount,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Wrap(array, start, byteCount, byteBuf);
}

} // namespace IO
} // namespace Elastos
