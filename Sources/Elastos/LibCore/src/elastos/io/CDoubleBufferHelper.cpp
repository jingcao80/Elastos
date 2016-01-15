
#include "CDoubleBufferHelper.h"
#include "DoubleBuffer.h"

namespace Elastos {
namespace IO {

ECode CDoubleBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Allocate(capacity, doubleBuf);
}

ECode CDoubleBufferHelper::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Wrap(array, doubleBuf);
}

ECode CDoubleBufferHelper::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 doubleCount,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Wrap(array, start, doubleCount, doubleBuf);
}

} // namespace IO
} // namespace Elastos
