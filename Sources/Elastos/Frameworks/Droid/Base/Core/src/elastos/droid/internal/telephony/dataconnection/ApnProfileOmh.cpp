
#include "elastos/droid/internal/telephony/dataconnection/ApnProfileOmh.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// ApnProfileOmh::ApnProfileTypeModem
//=============================================================================
CAR_INTERFACE_IMPL(ApnProfileOmh::ApnProfileTypeModem, Object, IApnProfileTypeModem)

AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::PROFILE_TYPE_UNSPECIFIED = GetApnProfileTypeModem(IPhoneConstants::APN_TYPE_DEFAULT);
AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::PROFILE_TYPE_MMS = GetApnProfileTypeModem(IPhoneConstants::APN_TYPE_MMS);
AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::PROFILE_TYPE_LBS = GetApnProfileTypeModem(IPhoneConstants::APN_TYPE_SUPL);
AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::PROFILE_TYPE_TETHERED = GetApnProfileTypeModem(IPhoneConstants::APN_TYPE_DUN);

ApnProfileOmh::ApnProfileTypeModem::ApnProfileTypeModem(
    /* [in] */ Int32 i,
    /* [in] */ const String& serviceType)
    : mId(i)
    , mServiceType(serviceType)
{}

ECode ApnProfileOmh::ApnProfileTypeModem::Getid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode ApnProfileOmh::ApnProfileTypeModem::GetDataServiceType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mServiceType;
    return NOERROR;
}

AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(
    /* [in] */ const String& serviceType)
{
    AutoPtr<IApnProfileTypeModem> rev;
    if (TextUtils::Equals(serviceType, IPhoneConstants::APN_TYPE_DEFAULT)) {
        rev = new ApnProfileTypeModem(0x00000001, IPhoneConstants::APN_TYPE_DEFAULT);
    } else if (TextUtils::Equals(serviceType, IPhoneConstants::APN_TYPE_MMS)) {
        rev = new ApnProfileTypeModem(0x00000002, IPhoneConstants::APN_TYPE_MMS);
    } else if (TextUtils::Equals(serviceType, IPhoneConstants::APN_TYPE_SUPL)) {
        rev = new ApnProfileTypeModem(0x00000020, IPhoneConstants::APN_TYPE_SUPL);
    } else if (TextUtils::Equals(serviceType, IPhoneConstants::APN_TYPE_DUN)) {
        rev = new ApnProfileTypeModem(0x00000040, IPhoneConstants::APN_TYPE_DUN);
    } else {
        /* For all other service types, return unspecified */
        rev = new ApnProfileTypeModem(0x00000001, IPhoneConstants::APN_TYPE_DEFAULT);
    }
    return rev;
}

AutoPtr<ArrayOf<IApnProfileTypeModem*> > ApnProfileOmh::ApnProfileTypeModem::GetValues()
{
    AutoPtr<ArrayOf<IApnProfileTypeModem*> > rev = ArrayOf<IApnProfileTypeModem*>::Alloc(4);
    (*rev)[0] = PROFILE_TYPE_UNSPECIFIED;
    (*rev)[1] = PROFILE_TYPE_MMS;
    (*rev)[2] = PROFILE_TYPE_LBS;
    (*rev)[3] = PROFILE_TYPE_TETHERED;
    return rev;
}

//=============================================================================
// ApnProfileOmh
//=============================================================================
CAR_INTERFACE_IMPL(ApnProfileOmh, ApnSetting, IApnProfileOmh)

const Int32 ApnProfileOmh::DATA_PROFILE_OMH_PRIORITY_LOWEST = 255;
const Int32 ApnProfileOmh::DATA_PROFILE_OMH_PRIORITY_HIGHEST = 0;

ApnProfileOmh::ApnProfileOmh()
    : mServiceTypeMasks(0)
    , mPriority(0)
{}

ECode ApnProfileOmh::constructor(
    /* [in] */ Int32 profileId,
    /* [in] */ Int32 priority)
{
    /**
     * Default values if the profile is being used for only selective
     * fields e.g: just profileId and Priority. use case is when rest of the
     * fields can be read and processed only by the modem
     */
    ApnSetting::constructor(0, String(""), String(NULL), String(""),
            String(NULL), String(NULL), String(NULL), String(NULL),
            String(NULL), String(NULL), String(NULL),
            IRILConstants::SETUP_DATA_AUTH_PAP_CHAP, ArrayOf<String>::Alloc(0),
            String("IP"), String("IP"), TRUE, 0, profileId, FALSE, 0, 0, 0,
            0, String(""), String(""));
    mPriority = priority;
    return NOERROR;
}

ECode ApnProfileOmh::CanHandleType(
    /* [in] */ const String& serviceType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(serviceType)->Getid(&id);
    *result = ( 0 != (mServiceTypeMasks & id));
    return NOERROR;
}

ECode ApnProfileOmh::GetApnProfileType(
    /* [out] */ ApnProfileType* result)
{
    VALIDATE_NOT_NULL(result)
    *result = PROFILE_TYPE_OMH;
    return NOERROR;
}

ECode ApnProfileOmh::ToShortString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("ApnProfile OMH");
    return NOERROR;
}

ECode ApnProfileOmh::ToHash(
    /* [out] */ String* result)
{
    return ToString(result);
}

ECode ApnProfileOmh::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    String super;
    ApnSetting::ToString(&super);
    sb.Append(super);
    Int32 profileId;
    GetProfileId(&profileId);
    sb.Append(profileId);
    sb.Append(", ");
    sb.Append(mPriority);
    sb.Append("]");
    return sb.ToString(result);
}

ECode ApnProfileOmh::SetApnProfileTypeModem(
    /* [in] */ IApnProfileTypeModem* modemProfile)
{
    mApnProfileModem = modemProfile;
    return NOERROR;
}

ECode ApnProfileOmh::GetApnProfileTypeModem(
    /* [out] */ IApnProfileTypeModem** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mApnProfileModem;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnProfileOmh::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}

ECode ApnProfileOmh::IsPriorityHigher(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isValidPriority;
    IsValidPriority(priority, &isValidPriority);
    *result = isValidPriority && (mPriority < priority);
    return NOERROR;
}

ECode ApnProfileOmh::IsPriorityLower(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isValidPriority;
    IsValidPriority(priority, &isValidPriority);
    *result = isValidPriority && mPriority > priority;
    return NOERROR;
}

ECode ApnProfileOmh::IsValidPriority(
    /* [out] */ Boolean* result)
{
    return IsValidPriority(mPriority, result);
}

ECode ApnProfileOmh::IsValidPriority(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = priority >= DATA_PROFILE_OMH_PRIORITY_HIGHEST &&
                priority <= DATA_PROFILE_OMH_PRIORITY_LOWEST;
    return NOERROR;
}

ECode ApnProfileOmh::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPriority;
    return NOERROR;
}

ECode ApnProfileOmh::AddServiceType(
    /* [in] */ IApnProfileTypeModem* modemProfile)
{
    Int32 id;
    modemProfile->Getid(&id);
    mServiceTypeMasks |= id;
    // Update the types
    AutoPtr<IArrayList> serviceTypes;
    CArrayList::New((IArrayList**)&serviceTypes);
    AutoPtr<ArrayOf<IApnProfileTypeModem*> > values = ApnProfileTypeModem::GetValues();
    for (Int32 i = 0; i < values->GetLength(); ++i) {
        AutoPtr<IApnProfileTypeModem> apt = (*values)[i];
        Int32 id;
        apt->Getid(&id);
        if (0 != (mServiceTypeMasks & id)) {
            String type;
            apt->GetDataServiceType(&type);
            serviceTypes->Add(StringUtils::ParseCharSequence(type));
        }
    }
    AutoPtr<ArrayOf<IInterface*> > types;
    serviceTypes->ToArray(ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&types);
    Int32 size = types->GetLength();
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        (*array)[i] = TO_STR((*types)[i]);
    }
    SetTypes(array);
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
