
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
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    mClassId = classId;
    mInterfaceId = interfaceId;

    String clsid;
    clsid.AppendFormat("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }",
        mClassId.mClsid.mData1, mClassId.mClsid.mData2, mClassId.mClsid.mData3,
        mClassId.mClsid.mData4[0], mClassId.mClsid.mData4[1],
        mClassId.mClsid.mData4[2], mClassId.mClsid.mData4[3],
        mClassId.mClsid.mData4[4], mClassId.mClsid.mData4[5],
        mClassId.mClsid.mData4[6], mClassId.mClsid.mData4[7]);
    mHash = clsid.GetHashCode();
    return NOERROR;
}

ECode TypeReference::constructor(
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ ITypeReference* component)
{
    constructor(classId, interfaceId);
    mComponent = component;
    mHash = mHash ^ Object::GetHashCode(mComponent);
    return NOERROR;
}

ECode TypeReference::GetClassType(
    /* [out] */ ClassID* classId)
{
    VALIDATE_NOT_NULL(classId)
    *classId = mClassId;
    return NOERROR;
}

ECode TypeReference::GetInterfaceType(
    /* [out] */ InterfaceID* interfaceId)
{
    VALIDATE_NOT_NULL(interfaceId)
    *interfaceId = mInterfaceId;
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

    *result = other->mClassId == mClassId && other->mInterfaceId == mInterfaceId;
    if (*result) {
        AutoPtr<ITypeReference> otherComp;
        other->GetComponent((ITypeReference**)&otherComp);
        *result = Object::Equals(mComponent, otherComp);
    }
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
        mClassId.mClsid.mData1, mClassId.mClsid.mData2, mClassId.mClsid.mData3,
        mClassId.mClsid.mData4[0], mClassId.mClsid.mData4[1],
        mClassId.mClsid.mData4[2], mClassId.mClsid.mData4[3],
        mClassId.mClsid.mData4[4], mClassId.mClsid.mData4[5],
        mClassId.mClsid.mData4[6], mClassId.mClsid.mData4[7]);
    sb += clsid;
    sb += ", module:";
    sb += mClassId.mUunm;
    if (mComponent != NULL) {
        sb += ", component:";
        sb += TO_CSTR(mComponent);
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

AutoPtr<ITypeReference> TypeReference::CreateSpecializedTypeReference(
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    AutoPtr<TypeReference> ref = new TypeReference();
    ref->constructor(classId, interfaceId);
    return (ITypeReference*)ref.Get();
}

AutoPtr<ITypeReference> TypeReference::CreateSpecializedTypeReference(
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ const ClassID& componentClassId,
    /* [in] */ const InterfaceID& componentInterfaceId)
{
    AutoPtr<TypeReference> component = new TypeReference();
    component->constructor(componentClassId, componentInterfaceId);
    AutoPtr<TypeReference> ref = new TypeReference();
    ref->constructor(classId, interfaceId, component);
    return (ITypeReference*)ref.Get();
}

AutoPtr<ITypeReference> TypeReference::CreateSpecializedTypeReference(
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ ITypeReference* component)
{
    AutoPtr<TypeReference> ref = new TypeReference();
    ref->constructor(classId, interfaceId, component);
    return (ITypeReference*)ref.Get();
}

ECode TypeReference::GetComponent(
    /* [out] */ ITypeReference** component)
{
    VALIDATE_NOT_NULL(component)
    *component = mComponent;
    REFCOUNT_ADD(*component);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
