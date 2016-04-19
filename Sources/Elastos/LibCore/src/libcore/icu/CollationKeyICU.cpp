
#include "CollationKeyICU.h"
#include <Math.h>

using Elastos::Text::EIID_ICollationKey;

namespace Libcore {
namespace ICU {

CAR_INTERFACE_IMPL(CollationKeyICU, CollationKey, ICollationKeyICU)

ECode CollationKeyICU::constructor(
    /* [in] */ const String& source,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    CollationKey::constructor(source);
    mBytes = bytes->Clone();
    mHashCode = 0;
    return NOERROR;
}

ECode CollationKeyICU::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(_other);
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICollationKeyICU> other = ICollationKeyICU::Probe(_other);
    if (other == NULL) return E_CLASS_CAST_EXCEPTION;

    // Get the bytes from the other collation key.
    CollationKeyICU* c = (CollationKeyICU*)(other.Get());
    AutoPtr<ArrayOf<Byte> > rhsBytes = c->mBytes;

    if (mBytes == NULL || mBytes->GetLength() == 0) {
        if (rhsBytes == NULL || rhsBytes->GetLength() == 0) {
            *result = 0;
            return NOERROR;
        }
        *result = -1;
        return NOERROR;
    }
    else {
        if (rhsBytes == NULL || rhsBytes->GetLength() == 0) {
            *result = 1;
            return NOERROR;
        }
    }

    Int32 count = Elastos::Core::Math::Min(mBytes->GetLength(), rhsBytes->GetLength());
    for (Int32 i = 0; i < count; ++i) {
        Int32 s = (*mBytes)[i] & 0xFF;
        Int32 t = (*rhsBytes)[i] & 0xFF;
        if (s < t) {
            *result = -1;
            return NOERROR;
        }
        if (s > t) {
            *result = 1;
            return NOERROR;
        }
    }
    if (mBytes->GetLength() < rhsBytes->GetLength()) {
        *result = -1;
        return NOERROR;
    }
    if (mBytes->GetLength() > rhsBytes->GetLength()) {
        *result = 1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CollationKeyICU::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * value)
{
    if (object == TO_IINTERFACE(this)) {
        *value = TRUE;
        return NOERROR;
    }

    Int32 out;
    CompareTo(object, &out);
    *value = out == 0;
    return NOERROR;
}

ECode CollationKeyICU::GetHashCode(
    /* [out] */ Int32 * value)
{
    if (mHashCode == 0) {
        if (mBytes != NULL && mBytes->GetLength() != 0) {
            Int32 len = mBytes->GetLength();
            Int32 inc = ((len - 32) / 32) + 1;
            for (Int32 i = 0; i < len;) {
                mHashCode = (mHashCode * 37) + (*mBytes)[i];
                i += inc;
            }
        }
        if (mHashCode == 0) {
            mHashCode = 1;
        }
    }
    *value = mHashCode;
    return NOERROR;
}

ECode CollationKeyICU::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    if (mBytes != NULL && mBytes->GetLength() > 0) {
        *array = mBytes->Clone();
        REFCOUNT_ADD(*array);
    }
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
