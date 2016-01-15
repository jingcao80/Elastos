
#include "CInt32BufferHelper.h"
#include "Int32Buffer.h"

namespace Elastos {
namespace IO {

ECode CInt32BufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Allocate(capacity, int32Buffer);
}

ECode CInt32BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Wrap(array, int32Buffer);
}

ECode CInt32BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int32Count,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Wrap(array, start, int32Count, int32Buffer);
}

} // namespace IO
} // namespace Elastos

