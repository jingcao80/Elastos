#ifndef __ELASTOS_DROID_TELEPHONY_SMSCBMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_SMSCBMESSAGE_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * Parcelable object containing a received cell broadcast message. There are four different types
  * of Cell Broadcast messages:
  *
  * <ul>
  * <li>opt-in informational broadcasts, e.g. news, weather, stock quotes, sports scores</li>
  * <li>cell information messages, broadcast on channel 50, indicating the current cell name for
  *  roaming purposes (required to display on the idle screen in Brazil)</li>
  * <li>emergency broadcasts for the Japanese Earthquake and Tsunami Warning System (ETWS)</li>
  * <li>emergency broadcasts for the American Commercial Mobile Alert Service (CMAS)</li>
  * </ul>
  *
  * <p>There are also four different CB message formats: GSM, ETWS Primary Notification (GSM only),
  * UMTS, and CDMA. Some fields are only applicable for some message formats. Other fields were
  * unified under a common name, avoiding some names, such as "Message Identifier", that refer to
  * two completely different concepts in 3GPP and CDMA.
  *
  * <p>The GSM/UMTS Message Identifier field is available via {@link #getServiceCategory}, the name
  * of the equivalent field in CDMA. In both cases the service category is a 16-bit value, but 3GPP
  * and 3GPP2 have completely different meanings for the respective values. For ETWS and CMAS, the
  * application should
  *
  * <p>The CDMA Message Identifier field is available via {@link #getSerialNumber}, which is used
  * to detect the receipt of a duplicate message to be discarded. In CDMA, the message ID is
  * unique to the current PLMN. In GSM/UMTS, there is a 16-bit serial number containing a 2-bit
  * Geographical Scope field which indicates whether the 10-bit message code and 4-bit update number
  * are considered unique to the PLMN, to the current cell, or to the current Location Area (or
  * Service Area in UMTS). The relevant values are concatenated into a single String which will be
  * unique if the messages are not duplicates.
  *
  * <p>The SMS dispatcher does not detect duplicate messages. However, it does concatenate the
  * pages of a GSM multi-page cell broadcast into a single SmsCbMessage object.
  *
  * <p>Interested applications with {@code RECEIVE_SMS_PERMISSION} can register to receive
  * {@code SMS_CB_RECEIVED_ACTION} broadcast intents for incoming non-emergency broadcasts.
  * Only system applications such as the CellBroadcastReceiver may receive notifications for
  * emergency broadcasts (ETWS and CMAS). This is intended to prevent any potential for delays or
  * interference with the immediate display of the alert message and playing of the alert sound and
  * vibration pattern, which could be caused by poorly written or malicious non-system code.
  *
  * @hide
  */
class SmsCbMessage
    : public Object
    , public ISmsCbMessage
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    SmsCbMessage();

    /**
      * Create a new SmsCbMessage with the specified data.
      */
    CARAPI constructor(
        /* [in] */ Int32 messageFormat,
        /* [in] */ Int32 geographicalScope,
        /* [in] */ Int32 serialNumber,
        /* [in] */ ISmsCbLocation* location,
        /* [in] */ Int32 serviceCategory,
        /* [in] */ const String& language,
        /* [in] */ const String& body,
        /* [in] */ Int32 priority,
        /* [in] */ ISmsCbEtwsInfo* etwsWarningInfo,
        /* [in] */ ISmsCbCmasInfo* cmasWarningInfo);

    /** Create a new SmsCbMessage object from a Parcel. */
    CARAPI constructor(
        /* [in] */ IParcel* in);

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Return the geographical scope of this message (GSM/UMTS only).
      *
      * @return Geographical scope
      */
    virtual CARAPI GetGeographicalScope(
        /* [out] */ Int32* result);

    /**
      * Return the broadcast serial number of broadcast (message identifier for CDMA, or
      * geographical scope + message code + update number for GSM/UMTS). The serial number plus
      * the location code uniquely identify a cell broadcast for duplicate detection.
      *
      * @return the 16-bit CDMA message identifier or GSM/UMTS serial number
      */
    virtual CARAPI GetSerialNumber(
        /* [out] */ Int32* result);

    /**
      * Return the location identifier for this message, consisting of the MCC/MNC as a
      * 5 or 6-digit decimal string. In addition, for GSM/UMTS, if the Geographical Scope of the
      * message is not binary 01, the Location Area is included. If the GS is 00 or 11, the
      * cell ID is also included. The {@link SmsCbLocation} object includes a method to test
      * if the location is included within another location area or within a PLMN and CellLocation.
      *
      * @return the geographical location code for duplicate message detection
      */
    virtual CARAPI GetLocation(
        /* [out] */ ISmsCbLocation** result);

    /**
      * Return the 16-bit CDMA service category or GSM/UMTS message identifier. The interpretation
      * of the category is radio technology specific. For ETWS and CMAS warnings, the information
      * provided by the category is available via {@link #getEtwsWarningInfo()} or
      * {@link #getCmasWarningInfo()} in a radio technology independent format.
      *
      * @return the radio technology specific service category
      */
    virtual CARAPI GetServiceCategory(
        /* [out] */ Int32* result);

    /**
      * Get the ISO-639-1 language code for this message, or null if unspecified
      *
      * @return Language code
      */
    virtual CARAPI GetLanguageCode(
        /* [out] */ String* result);

    /**
      * Get the body of this message, or null if no body available
      *
      * @return Body, or null
      */
    virtual CARAPI GetMessageBody(
        /* [out] */ String* result);

    /**
      * Get the message format ({@link #MESSAGE_FORMAT_3GPP} or {@link #MESSAGE_FORMAT_3GPP2}).
      * @return an integer representing 3GPP or 3GPP2 message format
      */
    virtual CARAPI GetMessageFormat(
        /* [out] */ Int32* result);

    /**
      * Get the message priority. Normal broadcasts return {@link #MESSAGE_PRIORITY_NORMAL}
      * and emergency broadcasts return {@link #MESSAGE_PRIORITY_EMERGENCY}. CDMA also may return
      * {@link #MESSAGE_PRIORITY_INTERACTIVE} or {@link #MESSAGE_PRIORITY_URGENT}.
      * @return an integer representing the message priority
      */
    virtual CARAPI GetMessagePriority(
        /* [out] */ Int32* result);

    /**
      * If this is an ETWS warning notification then this method will return an object containing
      * the ETWS warning type, the emergency user alert flag, and the popup flag. If this is an
      * ETWS primary notification (GSM only), there will also be a 7-byte timestamp and 43-byte
      * digital signature. As of Release 10, 3GPP TS 23.041 states that the UE shall ignore the
      * ETWS primary notification timestamp and digital signature if received.
      *
      * @return an SmsCbEtwsInfo object, or null if this is not an ETWS warning notification
      */
    virtual CARAPI GetEtwsWarningInfo(
        /* [out] */ ISmsCbEtwsInfo** result);

    /**
      * If this is a CMAS warning notification then this method will return an object containing
      * the CMAS message class, category, response type, severity, urgency and certainty.
      * The message class is always present. Severity, urgency and certainty are present for CDMA
      * warning notifications containing a type 1 elements record and for GSM and UMTS warnings
      * except for the Presidential-level alert category. Category and response type are only
      * available for CDMA notifications containing a type 1 elements record.
      *
      * @return an SmsCbCmasInfo object, or null if this is not a CMAS warning notification
      */
    virtual CARAPI GetCmasWarningInfo(
        /* [out] */ ISmsCbCmasInfo** result);

    /**
      * Return whether this message is an emergency (PWS) message type.
      * @return true if the message is a public warning notification; false otherwise
      */
    virtual CARAPI IsEmergencyMessage(
        /* [out] */ Boolean* result);

    /**
      * Return whether this message is an ETWS warning alert.
      * @return true if the message is an ETWS warning notification; false otherwise
      */
    virtual CARAPI IsEtwsMessage(
        /* [out] */ Boolean* result);

    /**
      * Return whether this message is a CMAS warning alert.
      * @return true if the message is a CMAS warning notification; false otherwise
      */
    virtual CARAPI IsCmasMessage(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

protected:
    static const String LOGTAG;

private:
    /** Format of this message (for interpretation of service category values). */
    /*const*/ Int32 mMessageFormat;
    /** Geographical scope of broadcast. */
    /*const*/ Int32 mGeographicalScope;
    /**
      * Serial number of broadcast (message identifier for CDMA, geographical scope + message code +
      * update number for GSM/UMTS). The serial number plus the location code uniquely identify
      * a cell broadcast for duplicate detection.
      */
    /*const*/ Int32 mSerialNumber;
    /**
      * Location identifier for this message. It consists of the current operator MCC/MNC as a
      * 5 or 6-digit decimal string. In addition, for GSM/UMTS, if the Geographical Scope of the
      * message is not binary 01, the Location Area is included for comparison. If the GS is
      * 00 or 11, the Cell ID is also included. LAC and Cell ID are -1 if not specified.
      */
    /*const*/ AutoPtr<ISmsCbLocation> mLocation;
    /**
      * 16-bit CDMA service category or GSM/UMTS message identifier. For ETWS and CMAS warnings,
      * the information provided by the category is also available via {@link #getEtwsWarningInfo()}
      * or {@link #getCmasWarningInfo()}.
      */
    /*const*/ Int32 mServiceCategory;
    /** Message language, as a two-character string, e.g. "en". */
    /*const*/ String mLanguage;
    /** Message body, as a String. */
    /*const*/ String mBody;
    /** Message priority (including emergency priority). */
    /*const*/ Int32 mPriority;
    /** ETWS warning notification information (ETWS warnings only). */
    /*const*/ AutoPtr<ISmsCbEtwsInfo> mEtwsWarningInfo;
    /** CMAS warning notification information (CMAS warnings only). */
    /*const*/ AutoPtr<ISmsCbCmasInfo> mCmasWarningInfo;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SMSCBMESSAGE_H__

