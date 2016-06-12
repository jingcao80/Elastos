
#include "elastos/droid/internal/telephony/dataconnection/ApnProfileOmh.h"

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            return id;

#endif
}

ECode ApnProfileOmh::ApnProfileTypeModem::GetDataServiceType(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            return serviceType;

#endif
}

AutoPtr<IApnProfileTypeModem> ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(
    /* [in] */ const String& serviceType)
{
    AutoPtr<IApnProfileTypeModem> rev;
    return rev;
#if 0 // TODO: Translate codes below
            if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_DEFAULT)) {
                return ApnProfileTypeModem(0x00000001, IPhoneConstants::APN_TYPE_DEFAULT);
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_MMS)) {
                return ApnProfileTypeModem(0x00000002, IPhoneConstants::APN_TYPE_MMS);
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_SUPL)) {
                return ApnProfileTypeModem(0x00000020, IPhoneConstants::APN_TYPE_SUPL);
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_DUN)) {
                return ApnProfileTypeModem(0x00000040, IPhoneConstants::APN_TYPE_DUN);
            } else {
                /* For all other service types, return unspecified */
                return ApnProfileTypeModem(0x00000001, IPhoneConstants::APN_TYPE_DEFAULT);
            }

#endif
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        /**
         * Default values if the profile is being used for only selective
         * fields e.g: just profileId and Priority. use case is when rest of the
         * fields can be read and processed only by the modem
         */
        super(0, "", null, "", null, null, null, null, null,
                null, null, RILConstants.SETUP_DATA_AUTH_PAP_CHAP,
                new String[0], "IP", "IP", true, 0, profileId, false, 0,
                0, 0, 0, "", "");
        this.mPriority = priority;

#endif
}

ECode ApnProfileOmh::CanHandleType(
    /* [in] */ const String& serviceType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ( 0 != (mServiceTypeMasks & ApnProfileTypeModem.
                getApnProfileTypeModem(serviceType).getid()));

#endif
}

ECode ApnProfileOmh::GetApnProfileType(
    /* [out] */ ApnProfileType* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ApnProfileType.PROFILE_TYPE_OMH;

#endif
}

ECode ApnProfileOmh::ToShortString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return "ApnProfile OMH";

#endif
}

ECode ApnProfileOmh::ToHash(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return this.toString();

#endif
}

ECode ApnProfileOmh::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        StringBuilder sb = new StringBuilder();
        sb.append(super.toString())
                .append(profileId)
                .append(", ").append(mPriority);
        sb.append("]");
        return sb.toString();

#endif
}

ECode ApnProfileOmh::SetApnProfileTypeModem(
    /* [in] */ IApnProfileTypeModem* modemProfile)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mApnProfileModem = modemProfile;

#endif
}

ECode ApnProfileOmh::GetApnProfileTypeModem(
    /* [out] */ IApnProfileTypeModem** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mApnProfileModem;

#endif
}

ECode ApnProfileOmh::SetPriority(
    /* [in] */ Int32 priority)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPriority = priority;

#endif
}

ECode ApnProfileOmh::IsPriorityHigher(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return isValidPriority(priority) && (mPriority < priority);

#endif
}

ECode ApnProfileOmh::IsPriorityLower(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return isValidPriority(priority) && mPriority > priority;

#endif
}

ECode ApnProfileOmh::IsValidPriority(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return isValidPriority(mPriority);

#endif
}

ECode ApnProfileOmh::IsValidPriority(
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return priority >= DATA_PROFILE_OMH_PRIORITY_HIGHEST &&
                priority <= DATA_PROFILE_OMH_PRIORITY_LOWEST;

#endif
}

ECode ApnProfileOmh::GetProfileId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return profileId;

#endif
}

ECode ApnProfileOmh::GetPriority(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mPriority;

#endif
}

ECode ApnProfileOmh::AddServiceType(
    /* [in] */ IApnProfileTypeModem* modemProfile)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mServiceTypeMasks |= modemProfile.getid();
        // Update the types
        ArrayList<String> serviceTypes = new ArrayList<String>();
        for (ApnProfileTypeModem apt : ApnProfileTypeModem.values()) {
            if (0 != (mServiceTypeMasks & apt.getid())) {
                serviceTypes.add(apt.getDataServiceType());
            }
        }
        types = serviceTypes.toArray(new String[0]);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
