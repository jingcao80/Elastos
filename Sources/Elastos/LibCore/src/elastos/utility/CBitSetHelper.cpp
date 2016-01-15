
#include "CBitSetHelper.h"
#include "CBitSet.h"

namespace Elastos {
namespace Utility {

CAR_SINGLETON_IMPL(CBitSetHelper)
CAR_INTERFACE_IMPL(CBitSetHelper, Singleton, IBitSetHelper)


ECode CBitSetHelper::ValueOf(
    /* [in] */ ArrayOf<Int64>* int64Arr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(int64Arr, bs);
}

/**
 * Returns a {@code BitSet} corresponding to {@code longBuffer}, interpreted as a little-endian
 * sequence of bits. This method does not alter the {@code LongBuffer}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ IInt64Buffer* int64Arr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(int64Arr, bs);
}

/**
 * Equivalent to {@code BitSet.valueOf(ByteBuffer.wrap(bytes))}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ ArrayOf<Byte>* byteArr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(byteArr, bs);
}

/**
 * Returns a {@code BitSet} corresponding to {@code byteBuffer}, interpreted as a little-endian
 * sequence of bits. This method does not alter the {@code ByteBuffer}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ IByteBuffer* byteArr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(byteArr, bs);
}

} // namespace Utility
} // namespace Elastos
