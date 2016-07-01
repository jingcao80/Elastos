
#include "elastos/droid/internal/telephony/UUSInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  UUSInfo::
//==============================================================
CAR_INTERFACE_IMPL(UUSInfo, Object, IUUSInfo)

UUSInfo::UUSInfo()
{
    mUusType = UUS_TYPE1_IMPLICIT;
    mUusDcs = UUS_DCS_IA5c;
    mUusData = NULL;
}

ECode UUSInfo::constructor(
    /* [in] */ Int32 uusType,
    /* [in] */ Int32 uusDcs,
    /* [in] */ ArrayOf<Byte>* uusData)
{
    mUusType = uusType;
    mUusDcs = uusDcs;
    mUusData = uusData;
    return NOERROR;
}

ECode UUSInfo::GetDcs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusDcs;
    return NOERROR;
}

ECode UUSInfo::SetDcs(
    /* [in] */ Int32 uusDcs)
{
    mUusDcs = uusDcs;
    return NOERROR;
}

ECode UUSInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusType;
    return NOERROR;
}

ECode UUSInfo::SetType(
    /* [in] */ Int32 uusType)
{
    mUusType = uusType;
    return NOERROR;
}

ECode UUSInfo::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusData;
    return NOERROR;
}

ECode UUSInfo::SetUserData(
    /* [in] */ ArrayOf<Byte>* uusData)
{
    mUusData = uusData;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
