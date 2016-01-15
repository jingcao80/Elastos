
#include "elastos/droid/wifi/CScanResultInformationElement.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CScanResultInformationElement, Object, IScanResultInformationElement)

CAR_OBJECT_IMPL(CScanResultInformationElement)

ECode CScanResultInformationElement::constructor()
{
    mId = 0;
    return NOERROR;
}

ECode CScanResultInformationElement::constructor(
    /* [in] */ IScanResultInformationElement* rhs)
{
    rhs->GetId(&mId);
    AutoPtr< ArrayOf<Byte> > bytes;
    rhs->GetBytes((ArrayOf<Byte>**)&bytes);
    mBytes = bytes->Clone();
    return NOERROR;
}

ECode CScanResultInformationElement::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mId;
    return NOERROR;
}

ECode CScanResultInformationElement::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CScanResultInformationElement::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CScanResultInformationElement::SetBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos