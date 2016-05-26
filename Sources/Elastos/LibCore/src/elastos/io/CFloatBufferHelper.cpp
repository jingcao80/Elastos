
#include "CFloatBufferHelper.h"
#include "FloatBuffer.h"

namespace Elastos {
namespace IO {
CAR_SINGLETON_IMPL(CFloatBufferHelper)

CAR_INTERFACE_IMPL(CFloatBufferHelper, Singleton, IFloatBufferHelper)

ECode CFloatBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Allocate(capacity, floatBuffer);
}

ECode CFloatBufferHelper::Wrap(
    /* [in] */ ArrayOf<Float>* array,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Wrap(array, floatBuffer);
}

ECode CFloatBufferHelper::Wrap(
    /* [in] */ ArrayOf<Float>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 floatCount,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Wrap(array, start, floatCount, floatBuffer);
}

} // namespace IO
} // namespace Elastos

