#ifndef __ELASTOS_DROID_TELEPHONY_SMSCBETWSINFO_H__
#define __ELASTOS_DROID_TELEPHONY_SMSCBETWSINFO_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * Contains information elements for a GSM or UMTS ETWS warning notification.
  * Supported values for each element are defined in 3GPP TS 23.041.
  *
  * {@hide}
  */
class SmsCbEtwsInfo
    : public Object
    , public ISmsCbEtwsInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    SmsCbEtwsInfo();

    /** Create a new SmsCbEtwsInfo object with the specified values. */
    CARAPI constructor(
        /* [in] */ Int32 warningType,
        /* [in] */ Boolean emergencyUserAlert,
        /* [in] */ Boolean activatePopup,
        /* [in] */ ArrayOf<Byte>* warningSecurityInformation);

    CARAPI constructor();
    /**
      * Flatten this object into a Parcel.
      *
      * @param dest  The Parcel in which the object should be written.
      * @param flags Additional flags about how the object should be written (ignored).
      */
    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Returns the ETWS warning type.
      * @return a warning type such as {@link #ETWS_WARNING_TYPE_EARTHQUAKE}
      */
    virtual CARAPI GetWarningType(
        /* [out] */ Int32* result);

    /**
      * Returns the ETWS emergency user alert flag.
      * @return true to notify terminal to activate emergency user alert; false otherwise
      */
    virtual CARAPI IsEmergencyUserAlert(
        /* [out] */ Boolean* result);

    /**
      * Returns the ETWS activate popup flag.
      * @return true to notify terminal to activate display popup; false otherwise
      */
    virtual CARAPI IsPopupAlert(
        /* [out] */ Boolean* result);

    /**
      * Returns the Warning-Security-Information timestamp (GSM primary notifications only).
      * As of Release 10, 3GPP TS 23.041 states that the UE shall ignore this value if received.
      * @return a UTC timestamp in System.currentTimeMillis() format, or 0 if not present
      */
    virtual CARAPI GetPrimaryNotificationTimestamp(
        /* [out] */ Int64* result);

    /**
      * Returns the digital signature (GSM primary notifications only). As of Release 10,
      * 3GPP TS 23.041 states that the UE shall ignore this value if received.
      * @return a byte array containing a copy of the primary notification digital signature
      */
    virtual CARAPI GetPrimaryNotificationSignature(
        /* [out] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /** One of the ETWS warning type constants defined in this class. */
    /*const*/ Int32 mWarningType;
    /** Whether or not to activate the emergency user alert tone and vibration. */
    /*const*/ Boolean mEmergencyUserAlert;
    /** Whether or not to activate a popup alert. */
    /*const*/ Boolean mActivatePopup;
    /**
      * 50-byte security information (ETWS primary notification for GSM only). As of Release 10,
      * 3GPP TS 23.041 states that the UE shall ignore the ETWS primary notification timestamp
      * and digital signature if received. Therefore it is treated as a raw byte array and
      * parceled with the broadcast intent if present, but the timestamp is only computed if an
      * application asks for the individual components.
      */
    /*const*/ AutoPtr< ArrayOf<Byte> > mWarningSecurityInformation;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SMSCBETWSINFO_H__

