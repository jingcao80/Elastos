
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNPROFILEOMH_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNPROFILEOMH_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnSetting.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class ApnProfileOmh
    : public ApnSetting
    , public IApnProfileOmh
{
public:
    /**
     *  OMH spec 3GPP2 C.S0023-D defines the application types in terms of a
     *  32-bit mask where each bit represents one application
     *
     *  Application bit and the correspondign app type is listed below:
     *  1 Unspecified (all applications use the same profile)
     *  2 MMS
     *  3 Browser
     *  4 BREW
     *  5 Java
     *  6 LBS
     *  7 Terminal (tethered mode for terminal access)
     *  8-32 Reserved for future use
     *
     *  From this list all the implemented app types are listed in the enum
     */
    class ApnProfileTypeModem
        : public Object
        , public IApnProfileTypeModem
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Getid(
            /* [out] */ Int32* result);

        CARAPI GetDataServiceType(
            /* [out] */ String* result);

        static CARAPI_(AutoPtr<IApnProfileTypeModem>) GetApnProfileTypeModem(
            /* [in] */ const String& serviceType);

    private:
        ApnProfileTypeModem(
            /* [in] */ Int32 i,
            /* [in] */ const String& serviceType);

    public:
        /* Static mapping of OMH profiles to Android Service Types */
        static AutoPtr<IApnProfileTypeModem> PROFILE_TYPE_UNSPECIFIED;
        static AutoPtr<IApnProfileTypeModem> PROFILE_TYPE_MMS;
        static AutoPtr<IApnProfileTypeModem> PROFILE_TYPE_LBS;
        static AutoPtr<IApnProfileTypeModem> PROFILE_TYPE_TETHERED;

    private:
        Int32 mId;
        String mServiceType;
    };

public:
    CAR_INTERFACE_DECL()

    ApnProfileOmh();

    CARAPI constructor(
        /* [in] */ Int32 profileId,
        /* [in] */ Int32 priority);

    // @Override
    CARAPI CanHandleType(
        /* [in] */ const String& serviceType,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetApnProfileType(
        /* [out] */ ApnProfileType* result);

    // @Override
    CARAPI ToShortString(
        /* [out] */ String* result);

    // @Override
    CARAPI ToHash(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetApnProfileTypeModem(
        /* [in] */ IApnProfileTypeModem* modemProfile);

    CARAPI GetApnProfileTypeModem(
        /* [out] */ IApnProfileTypeModem** result);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    /* priority defined from 0..255; 0 is highest */
    CARAPI IsPriorityHigher(
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

    /* priority defined from 0..255; 0 is highest */
    CARAPI IsPriorityLower(
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

    CARAPI IsValidPriority(
        /* [out] */ Boolean* result);

    CARAPI GetProfileId(
        /* [out] */ Int32* result);

    CARAPI GetPriority(
        /* [out] */ Int32* result);

    CARAPI AddServiceType(
        /* [in] */ IApnProfileTypeModem* modemProfile);

private:
    /* NOTE: priority values are reverse, lower number = higher priority */
    CARAPI IsValidPriority(
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

private:
    static const Int32 DATA_PROFILE_OMH_PRIORITY_LOWEST;

    static const Int32 DATA_PROFILE_OMH_PRIORITY_HIGHEST;

    AutoPtr<IApnProfileTypeModem> mApnProfileModem;

    Int32 mServiceTypeMasks;

    /* Priority of this profile in the modem */
    Int32 mPriority;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNPROFILEOMH_H__
