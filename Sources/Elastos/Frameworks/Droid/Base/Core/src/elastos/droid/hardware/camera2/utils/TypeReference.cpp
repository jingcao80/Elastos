
#include "elastos/droid/hardware/camera2/utils/TypeReference.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(TypeReference, Object, ITypeReference)

TypeReference::TypeReference()
    : mHash(0)
{
}

ECode TypeReference::constructor(
        /* [in] */ const ClassID& clsID)
{
    mType = clsID;

    String clsid;
    clsid.AppendFormat("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }",
        mType.mClsid.mData1, mType.mClsid.mData2, mType.mClsid.mData3,
        mType.mClsid.mData4[0], mType.mClsid.mData4[1],
        mType.mClsid.mData4[2], mType.mClsid.mData4[3],
        mType.mClsid.mData4[4], mType.mClsid.mData4[5],
        mType.mClsid.mData4[6], mType.mClsid.mData4[7]);
    mHash = clsid.GetHashCode();
    return NOERROR;
}

ECode TypeReference::GetType(
    /* [out] */ ClassID* clsID)
{
    VALIDATE_NOT_NULL(clsID)
    *clsID = mType;
    return NOERROR;
}

ECode TypeReference::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ITypeReference* o = ITypeReference::Probe(obj);
    if (o == NULL) return NOERROR;

    TypeReference* other = (TypeReference*)o;
    if (other == this) {
        *result = TRUE;
        return NOERROR;
    }

    *result = other->mType == mType;
    return NOERROR;
}

ECode TypeReference::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mHash;
    return NOERROR;
}

ECode TypeReference::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("TypeReference:");
    sb += StringUtils::ToHexString((Int32)this);
    sb += "{clsid:";
    String clsid;
    clsid.AppendFormat("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
        mType.mClsid.mData1, mType.mClsid.mData2, mType.mClsid.mData3,
        mType.mClsid.mData4[0], mType.mClsid.mData4[1],
        mType.mClsid.mData4[2], mType.mClsid.mData4[3],
        mType.mClsid.mData4[4], mType.mClsid.mData4[5],
        mType.mClsid.mData4[6], mType.mClsid.mData4[7]);
    sb += clsid;
    sb += ", module:";
    sb += mType.mUunm;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

AutoPtr<ITypeReference> TypeReference::CreateSpecializedTypeReference(
    /* [in] */ const ClassID& clsID)
{
    AutoPtr<TypeReference> ref = new TypeReference();
    ref->constructor(clsID);
    return (ITypeReference*)ref.Get();
}

ECode TypeReference::GetRawType(
    /* [out] */ IClassInfo** classInfo)
{
    assert(0 && "E_NOT_IMPLEMENTED");
    return E_NOT_IMPLEMENTED;
}

ECode TypeReference::GetComponentType(
    /* [out] */ ITypeReference** reference)
{
    assert(0 && "E_NOT_IMPLEMENTED");
    return E_NOT_IMPLEMENTED;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
