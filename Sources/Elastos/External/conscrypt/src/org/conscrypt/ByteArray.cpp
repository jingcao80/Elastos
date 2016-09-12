#include "org/conscrypt/ByteArray.h"
#include <elastos/utility/Arrays.h>

using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(ByteArray, Object, IByteArray)

ByteArray::ByteArray(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
    mHashCode = Arrays::GetHashCode(bytes);
}

ECode ByteArray::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHashCode;
    return NOERROR;
}

ECode ByteArray::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IByteArray::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IByteArray> lhs = IByteArray::Probe(obj);
    *result = Arrays::Equals(mBytes, ((ByteArray*)lhs.Get())->mBytes);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
