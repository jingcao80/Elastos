
#ifndef __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecomm.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Telephony_CTelephonyManager.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IIPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Telecomm::Internal::IITelecomService;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CTelephonyManager)
    , public Object
    , public ITelephonyManager
{
public:
    CTelephonyManager();

    virtual ~CTelephonyManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Returns the multi SIM variant
     * Returns DSDS for Dual SIM Dual Standby
     * Returns DSDA for Dual SIM Dual Active
     * Returns TSTS for Triple SIM Triple Standby
     * Returns UNKNOWN for others
     */
    /** {@hide} */
    CARAPI GetMultiSimConfiguration(
        /* [out] */ MultiSimVariants* result);

    /**
     * Returns the number of phones available.
     * Returns 1 for Single standby mode (Single SIM functionality)
     * Returns 2 for Dual standby mode.(Dual SIM functionality)
     */
    /** {@hide} */
    CARAPI GetPhoneCount(
        /* [out] */ Int32* result);

    /** {@hide} */
    CARAPI IsMultiSimEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetDeviceSoftwareVersion(
        /* [out] */ String* res);

    CARAPI GetDeviceId(
        /* [out] */ String* res);

    /**
     * Returns the unique device ID of a subscription, for example, the IMEI for
     * GSM and the MEID for CDMA phones. Return null if device ID is not available.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     *
     * @param slotId of which deviceID is returned
     */
    /** {@hide} */
    CARAPI GetDeviceId(
        /* [in] */ Int32 slotId,
        /* [out] */ String* res);

    /**
     * Returns the IMEI. Return null if IMEI is not available.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     */
    /** {@hide} */
    CARAPI GetImei(
        /* [out] */ String* res);

    /**
     * Returns the IMEI. Return null if IMEI is not available.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     *
     * @param slotId of which deviceID is returned
     */
    /** {@hide} */
    CARAPI GetImei(
        /* [in] */ Int32 slotId,
        /* [out] */ String* res);

    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** res);

    /**
     * Enables location update notifications.  {@link PhoneStateListener#onCellLocationChanged
     * PhoneStateListener.onCellLocationChanged} will be called on location updates.
     *
     * <p>Requires Permission: {@link android.Manifest.permission#CONTROL_LOCATION_UPDATES
     * CONTROL_LOCATION_UPDATES}
     *
     * @hide
     */
    CARAPI EnableLocationUpdates();

    /**
     * Enables location update notifications for a subscription.
     * {@link PhoneStateListener#onCellLocationChanged
     * PhoneStateListener.onCellLocationChanged} will be called on location updates.
     *
     * <p>Requires Permission: {@link android.Manifest.permission#CONTROL_LOCATION_UPDATES
     * CONTROL_LOCATION_UPDATES}
     *
     * @param subId for which the location updates are enabled
     */
    /** @hide */
    CARAPI EnableLocationUpdates(
        /* [in] */ Int64 subId);

    /**
     * Disables location update notifications.  {@link PhoneStateListener#onCellLocationChanged
     * PhoneStateListener.onCellLocationChanged} will be called on location updates.
     *
     * <p>Requires Permission: {@link android.Manifest.permission#CONTROL_LOCATION_UPDATES
     * CONTROL_LOCATION_UPDATES}
     *
     * @hide
     */
    CARAPI DisableLocationUpdates();

    /** @hide */
    CARAPI DisableLocationUpdates(
        /* [in] */ Int64 subId);

    CARAPI GetNeighboringCellInfo(
        /* [out] */ IList** res);

    /**
     * Returns the current phone type.
     * TODO: This is a last minute change and hence hidden.
     *
     * @see #PHONE_TYPE_NONE
     * @see #PHONE_TYPE_GSM
     * @see #PHONE_TYPE_CDMA
     * @see #PHONE_TYPE_SIP
     *
     * {@hide}
     */
    //@SystemApi
    CARAPI GetCurrentPhoneType(
        /* [out] */ Int32* val);

    /**
     * Returns a constant indicating the device phone type for a subscription.
     *
     * @see #PHONE_TYPE_NONE
     * @see #PHONE_TYPE_GSM
     * @see #PHONE_TYPE_CDMA
     *
     * @param subId for which phone type is returned
     */
    /** {@hide} */
    //@SystemApi
    CARAPI GetCurrentPhoneType(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    /**
     * Returns a constant indicating the device phone type.  This
     * indicates the type of radio used to transmit voice calls.
     *
     * @see #PHONE_TYPE_NONE
     * @see #PHONE_TYPE_GSM
     * @see #PHONE_TYPE_CDMA
     * @see #PHONE_TYPE_SIP
     */
    CARAPI GetPhoneType(
        /* [out] */ Int32* val);

    CARAPI GetNetworkOperatorName(
        /* [out] */ String* res);

    /**
     * Returns the alphabetic name of current registered operator
     * for a particular subscription.
     * <p>
     * Availability: Only when user is registered to a network. Result may be
     * unreliable on CDMA networks (use {@link #getPhoneType()} to determine if
     * on a CDMA network).
     * @param subId
     */
    /** {@hide} */
    CARAPI GetNetworkOperatorName(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetNetworkOperator(
        /* [out] */ String* res);

    /**
     * Returns the numeric name (MCC+MNC) of current registered operator
     * for a particular subscription.
     * <p>
     * Availability: Only when user is registered to a network. Result may be
     * unreliable on CDMA networks (use {@link #getPhoneType()} to determine if
     * on a CDMA network).
     *
     * @param subId
     */
    /** {@hide} */
    CARAPI GetNetworkOperator(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI IsNetworkRoaming(
        /* [out] */ Boolean* res);

    /**
     * Returns true if the device is considered roaming on the current
     * network for a subscription.
     * <p>
     * Availability: Only when user registered to a network.
     *
     * @param subId
     */
    /** {@hide} */
    CARAPI IsNetworkRoaming(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* res);

    CARAPI GetNetworkCountryIso(
        /* [out] */ String* res);

    /**
     * Returns the ISO country code equivalent of the current registered
     * operator's MCC (Mobile Country Code) of a subscription.
     * <p>
     * Availability: Only when user is registered to a network. Result may be
     * unreliable on CDMA networks (use {@link #getPhoneType()} to determine if
     * on a CDMA network).
     *
     * @param subId for which Network CountryIso is returned
     */
    /** {@hide} */
    CARAPI GetNetworkCountryIso(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetNetworkType(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetNetworkType(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetDataNetworkType(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetDataNetworkType(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetVoiceNetworkType(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetVoiceNetworkType(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    CARAPI GetNetworkTypeName(
        /* [out] */ String* res);

    CARAPI HasIccCard(
        /* [out] */ Boolean* res);

    /** {@hide} */
    CARAPI HasIccCard(
        /* [in] */ Int64 slotId,
        /* [out] */ Boolean* res);

    CARAPI GetSimState(
        /* [out] */ Int32* val);

    CARAPI GetSimState(
        /* [in] */ Int32 slotId,
        /* [out] */ Int32* val);

    CARAPI GetSimOperator(
        /* [out] */ String* res);

    CARAPI GetSimOperator(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetSimOperatorName(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetSimOperatorName(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetSimCountryIso(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetSimCountryIso(
        /* [in] */ Int64 slotId,
        /* [out] */ String* res);

    CARAPI GetSimSerialNumber(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetSimSerialNumber(
        /* [in] */ Int64 slotId,
        /* [out] */ String* res);

    CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetLteOnCdmaMode(
        /* [in] */ Int64 slotId,
        /* [out] */ Int32* val);

    CARAPI GetSubscriberId(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetSubscriberId(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetGroupIdLevel1(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetGroupIdLevel1(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetLine1Number(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetLine1NumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    /**
     * Set the line 1 phone number string and its alphatag for the current ICCID
     * for display purpose only, for example, displayed in Phone Status. It won't
     * change the actual MSISDN/MDN. To unset alphatag or number, pass in a null
     * value.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param alphaTag alpha-tagging of the dailing nubmer
     * @param number The dialing number
     * @hide
     */
    CARAPI SetLine1NumberForDisplay(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number);

    /**
     * Set the line 1 phone number string and its alphatag for the current ICCID
     * for display purpose only, for example, displayed in Phone Status. It won't
     * change the actual MSISDN/MDN. To unset alphatag or number, pass in a null
     * value.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param subId the subscriber that the alphatag and dialing number belongs to.
     * @param alphaTag alpha-tagging of the dailing nubmer
     * @param number The dialing number
     * @hide
     */
    CARAPI SetLine1NumberForDisplayForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number);

    /** {@hide} */
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetLine1AlphaTagForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetMsisdn(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetMsisdn(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI GetVoiceMailNumber(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetVoiceMailNumber(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetCompleteVoiceMailNumber(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetCompleteVoiceMailNumber(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetVoiceMessageCount(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* res);

    /** {@hide} */
    CARAPI GetVoiceMailAlphaTag(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    /**
     * Returns the IMS private user identity (IMPI) that was loaded from the ISIM.
     * @return the IMPI, or null if not present or not loaded
     * @hide
     */
    CARAPI GetIsimImpi(
        /* [out] */ String* res);

    /**
     * Returns the IMS home network domain name that was loaded from the ISIM.
     * @return the IMS domain name, or null if not present or not loaded
     * @hide
     */
    CARAPI GetIsimDomain(
        /* [out] */ String* res);

    /**
     * Returns the IMS public user identities (IMPU) that were loaded from the ISIM.
     * @return an array of IMPU strings, with one IMPU per string, or null if
     *      not present or not loaded
     * @hide
     */
    CARAPI GetIsimImpu(
        /* [out, callee] */ ArrayOf<String>** res);

    CARAPI GetCallState(
        /* [out] */ Int32* val);

    /** {@hide} */
    CARAPI GetCallState(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    CARAPI GetDataActivity(
        /* [out] */ Int32* val);

    CARAPI GetDataState(
        /* [out] */ Int32* val);

    CARAPI Listen(
        /* [in] */ IPhoneStateListener* funclistener,
        /* [in] */ Int32 Events);

    /**
     * Returns the CDMA ERI icon index to display
     *
     * @hide
     */
    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* val);

    /**
     * Returns the CDMA ERI icon index to display for a subscription
     */
    /** {@hide} */
    CARAPI GetCdmaEriIconIndex(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     *
     * @hide
     */
    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* val);

    /**
     * Returns the CDMA ERI icon mode for a subscription.
     * 0 - ON
     * 1 - FLASHING
     */
    /** {@hide} */
    CARAPI GetCdmaEriIconMode(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* val);

    /**
     * Returns the CDMA ERI text,
     *
     * @hide
     */
    CARAPI GetCdmaEriText(
        /* [out] */ String* res);

    /**
     * Returns the CDMA ERI text, of a subscription
     *
     */
    /** {@hide} */
    CARAPI GetCdmaEriText(
        /* [in] */ Int64 subId,
        /* [out] */ String* res);

    CARAPI IsVoiceCapable(
        /* [out] */ Boolean* res);

    CARAPI IsSmsCapable(
        /* [out] */ Boolean* res);

    CARAPI GetAllCellInfo(
        /* [out] */ IList** res);

    /**
     * Sets the minimum time in milli-seconds between {@link PhoneStateListener#onCellInfoChanged
     * PhoneStateListener.onCellInfoChanged} will be invoked.
     *<p>
     * The default, 0, means invoke onCellInfoChanged when any of the reported
     * information changes. Setting the value to INT_MAX(0x7fffffff) means never issue
     * A onCellInfoChanged.
     *<p>
     * @param rateInMillis the rate
     *
     * @hide
     */
    CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis);

    /**
     * Returns the MMS user agent.
     */
    CARAPI GetMmsUserAgent(
        /* [out] */ String* res);

    /**
     * Returns the MMS user agent profile URL.
     */
    CARAPI GetMmsUAProfUrl(
        /* [out] */ String* res);

    /**
     * Opens a logical channel to the ICC card.
     *
     * Input parameters equivalent to TS 27.007 AT+CCHO command.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param AID Application id. See ETSI 102.221 and 101.220.
     * @return an IccOpenLogicalChannelResponse object.
     */
    CARAPI IccOpenLogicalChannel(
        /* [in] */ const String& AID,
        /* [out] */ IIccOpenLogicalChannelResponse** response);

    /**
     * Closes a previously opened logical channel to the ICC card.
     *
     * Input parameters equivalent to TS 27.007 AT+CCHC command.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param channel is the channel id to be closed as retruned by a successful
     *            iccOpenLogicalChannel.
     * @return true if the channel was closed successfully.
     */
    CARAPI IccCloseLogicalChannel(
        /* [in] */ Int32 channel,
        /* [out] */ Boolean* result);

    /**
     * Transmit an APDU to the ICC card over a logical channel.
     *
     * Input parameters equivalent to TS 27.007 AT+CGLA command.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param channel is the channel id to be closed as returned by a successful
     *            iccOpenLogicalChannel.
     * @param cla Class of the APDU command.
     * @param instruction Instruction of the APDU command.
     * @param p1 P1 value of the APDU command.
     * @param p2 P2 value of the APDU command.
     * @param p3 P3 value of the APDU command. If p3 is negative a 4 byte APDU
     *            is sent to the SIM.
     * @param data Data to be sent with the APDU.
     * @return The APDU response from the ICC card with the status appended at
     *            the end.
     */
    CARAPI IccTransmitApduLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
     * Transmit an APDU to the ICC card over the basic channel.
     *
     * Input parameters equivalent to TS 27.007 AT+CSIM command.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param cla Class of the APDU command.
     * @param instruction Instruction of the APDU command.
     * @param p1 P1 value of the APDU command.
     * @param p2 P2 value of the APDU command.
     * @param p3 P3 value of the APDU command. If p3 is negative a 4 byte APDU
     *            is sent to the SIM.
     * @param data Data to be sent with the APDU.
     * @return The APDU response from the ICC card with the status appended at
     *            the end.
     */
    CARAPI IccTransmitApduBasicChannel(
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
     * Returns the response APDU for a command APDU sent through SIM_IO.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param fileID
     * @param command
     * @param p1 P1 value of the APDU command.
     * @param p2 P2 value of the APDU command.
     * @param p3 P3 value of the APDU command.
     * @param filePath
     * @return The APDU response.
     */
    CARAPI IccExchangeSimIO(
        /* [in] */ Int32 fileID,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& filePath,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Send ENVELOPE to the SIM and return the response.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *
     * @param content String containing SAT/USAT response in hexadecimal
     *                format starting with command tag. See TS 102 223 for
     *                details.
     * @return The APDU response from the ICC card in hexadecimal format
     *         with the last 4 bytes being the status word. If the command fails,
     *         returns an empty string.
     */
    CARAPI SendEnvelopeWithStatus(
        /* [in] */ const String& content,
        /* [out] */ String* result);

    /**
     * Read one of the NV items defined in com.android.internal.telephony.RadioNVItems.
     * Used for device configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param itemID the ID of the item to read.
     * @return the NV item as a String, or null on any failure.
     *
     * @hide
     */
    CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [out] */ String* result);

    /**
     * Write one of the NV items defined in com.android.internal.telephony.RadioNVItems.
     * Used for device configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param itemID the ID of the item to read.
     * @param itemValue the value to write, as a String.
     * @return true on success; false on any failure.
     *
     * @hide
     */
    CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ const String& itemValue,
        /* [out] */ Boolean* result);

    /**
     * Update the CDMA Preferred Roaming List (PRL) in the radio NV storage.
     * Used for device configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param preferredRoamingList byte array containing the new PRL.
     * @return true on success; false on any failure.
     *
     * @hide
     */
    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [out] */ Boolean* result);

    /**
     * Perform the specified type of NV config reset. The radio will be taken offline
     * and the device must be rebooted after the operation. Used for device
     * configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param resetType reset type: 1: reload NV reset, 2: erase NV reset, 3: factory NV reset
     * @return true on success; false on any failure.
     *
     * @hide
     */
    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [out] */ Boolean* result);

    /** {@hide} */
    CARAPI GetDefaultSim(
        /* [out] */ Int32* result);

    /** {@hide} */
    CARAPI GetSimCount(
        /* [out] */ Int32* result);

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or null if not present or not loaded
     * @hide
     */
    CARAPI GetIsimIst(
        /* [out] */ String* result);

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of PCSCF strings with one PCSCF per string, or null if
     *         not present or not loaded
     * @hide
     */
    CARAPI GetIsimPcscf(
        /* [out, callee] */ ArrayOf<String>** result);


    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns null if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or null if not available
     * @hide
     * @deprecated
     * @see getIccSimChallengeResponse with appType=PhoneConstants.APPTYPE_ISIM
     */
    CARAPI GetIsimChallengeResponse(
        /* [in] */ const String& nonce,
        /* [out] */ String* result);

    /**
     * Returns the response of SIM Authentication through RIL.
     * Returns null if the Authentication hasn't been successful
     * @param subId subscription ID to be queried
     * @param appType ICC application type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return the response of SIM Authentication, or null if not available
     * @hide
     */
    CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 appType,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
     * Returns the response of SIM Authentication through RIL for the default subscription.
     * Returns null if the Authentication hasn't been successful
     * @param appType ICC application type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return the response of SIM Authentication, or null if not available
     * @hide
     */
    CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int32 appType,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
     * Get P-CSCF address from PCO after data connection is established or modified.
     * @param apnType the apnType, "ims" for IMS APN, "emergency" for EMERGENCY APN
     * @return array of P-CSCF address
     * @hide
     */
    CARAPI GetPcscfAddress(
        /* [in] */ const String& appType,
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Set IMS registration state
     *
     * @param Registration state
     * @hide
     */
    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    /**
     * Get the preferred network type.
     * Used for device configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @return the preferred network type, defined in RILConstants.java.
     * @hide
     */
    CARAPI GetPreferredNetworkType(
        /* [out] */ Int32* result);

    /**
     * Set the preferred network type.
     * Used for device configuration by some CDMA operators.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param networkType the preferred network type, defined in RILConstants.java.
     * @return true on success; false on any failure.
     * @hide
     */
    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Set the preferred network type to global mode which includes LTE, CDMA, EvDo and GSM/WCDMA.
     *
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @return true on success; false on any failure.
     * @hide
     */
    CARAPI SetGlobalPreferredNetworkType(
        /* [out] */ Boolean* result);

    /**
     * Has the calling application been granted carrier privileges by the carrier.
     *
     * If any of the packages in the calling UID has carrier privileges, the
     * call will return true. This access is granted by the owner of the UICC
     * card and does not depend on the registered carrier.
     *
     * TODO: Add a link to documentation.
     *
     * @return CARRIER_PRIVILEGE_STATUS_HAS_ACCESS if the app has carrier privileges.
     *         CARRIER_PRIVILEGE_STATUS_NO_ACCESS if the app does not have carrier privileges.
     *         CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED if the carrier rules are not loaded.
     *         CARRIER_PRIVILEGE_STATUS_ERROR_LOADING_RULES if there was an error loading carrier
     *             rules (or if there are no rules).
     * @hide
     */
    CARAPI HasCarrierPrivileges(
        /* [out] */ Int32* result);

    /**
     * Override the branding for the current ICCID.
     *
     * Once set, whenever the SIM is present in the device, the service
     * provider name (SPN) and the operator name will both be replaced by the
     * brand value input. To unset the value, the same function should be
     * called with a null brand value.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     *  or has to be carrier app - see #hasCarrierPrivileges.
     *
     * @param brand The brand name to display/set.
     * @return true if the operation was executed correctly.
     * @hide
     */
    CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    /**
     * Expose the rest of ITelephony to @SystemApi
     */

    /** @hide */
    CARAPI GetCdmaMdn(
        /* [out] */ String* str);

    /** @hide */
    CARAPI GetCdmaMdn(
        /* [in] */ Int64 subId,
        /* [out] */ String* str);

    /** @hide */
    CARAPI GetCdmaMin(
        /* [out] */ String* str);

    /** @hide */
    CARAPI GetCdmaMin(
        /* [in] */ Int64 subId,
        /* [out] */ String* str);

    /** @hide */
    CARAPI CheckCarrierPrivilegesForPackage(
        /* [in] */ const String& pkgname,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetCarrierPackageNamesForIntent(
        /* [in] */ IIntent* intent,
        /* [out] */ IList** result);

    /** @hide */
    CARAPI Dial(
        /* [in] */ const String& number);

    /** @hide */
    CARAPI Call(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& number);

    /** @hide */
    CARAPI EndCall(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI AnswerRingingCall();

    /** @hide */
    CARAPI SilenceRinger();

    /** @hide */
    CARAPI IsOffhook(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsIdle(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsRadioOn(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsSimPinEnabled(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SupplyPin(
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SupplyPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SupplyPinReportResult(
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /** @hide */
    CARAPI SupplyPukReportResult(
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /** @hide */
    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI ToggleRadioOnOff();

    /** @hide */
    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SetRadioPower(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI UpdateServiceLocation();

    /** @hide */
    CARAPI EnableDataConnectivity(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI DisableDataConnectivity(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsDataConnectivityPossible(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SetDataEnabled(
        /* [in] */ Boolean value);

    /** @hide */
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    /**
     * Set whether Android should display a simplified Mobile Network Settings UI
     * for the current ICCID.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param enable true means enabling the simplified UI.
     * @hide
     */
    CARAPI EnableSimplifiedNetworkSettings(
        /* [in] */ Boolean enable);

    /**
     * Set whether Android should display a simplified Mobile Network Settings UI
     * for the current ICCID.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_PHONE_STATE MODIFY_PHONE_STATE}
     * Or the calling app has carrier privileges. @see #hasCarrierPrivileges
     *
     * @param subId for which the simplified UI should be enabled or disabled.
     * @param enable true means enabling the simplified UI.
     * @hide
     */
    CARAPI EnableSimplifiedNetworkSettingsForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Boolean enable);

    /**
     * Get whether a simplified Mobile Network Settings UI is enabled for the
     * current ICCID.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     *
     * @return true if the simplified UI is enabled.
     * @hide
     */
    CARAPI GetSimplifiedNetworkSettingsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Get whether a simplified Mobile Network Settings UI is enabled for the
     * current ICCID.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     *
     * @param subId for which the simplified UI should be enabled or disabled.
     * @return true if the simplified UI is enabled.
     * @hide
     */
    CARAPI GetSimplifiedNetworkSettingsEnabledForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * Returns the result and response from RIL for oem request
     *
     * @param oemReq the data is sent to ril.
     * @param oemResp the respose data from RIL.
     * @return negative value request was not handled or get error
     *         0 request was handled succesfully, but no response data
     *         positive value success, data length of response
     * @hide
     */
    CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* oemReq,
        /* [out, callee] */ ArrayOf<Byte>** oemResp,
        /* [out] */ Int32* result);

    static CARAPI GetDefault(
        /* [out] */ ITelephonyManager** res);

    static CARAPI From(
        /* [in] */ IContext* ctx,
        /* [out] */ ITelephonyManager** res);

    /**
     * This function returns the type of the phone, depending
     * on the network mode.
     *
     * @param networkMode
     * @return Phone Type
     *
     * @hide
     */
    static CARAPI GetPhoneType(
        /* [in] */ Int32 networkMode,
        /* [out] */ Int32* val);

    static CARAPI GetLteOnCdmaModeStatic(
        /* [out] */ Int32* val);

    static CARAPI GetNetworkClass(
        /* [in] */ Int32 networkType,
        /* [out] */ Int32* val);

    static CARAPI GetNetworkTypeName(
        /* [in] */ Int32 type,
        /* [out] */ String* res);

    /**
     * Sets the telephony property with the value specified.
     *
     * @hide
     */
    static CARAPI SetTelephonyProperty(
        /* [in] */ const String& property,
        /* [in] */ Int64 subId,
        /* [in] */ const String& value);

    /**
     * Convenience function for retrieving a value from the secure settings
     * value list as an integer.  Note that internally setting values are
     * always stored as strings; this function converts the string to an
     * integer for you.
     * <p>
     * This version does not take a default value.  If the setting has not
     * been set, or the string value is not a number,
     * it throws {@link SettingNotFoundException}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param index The index of the list
     *
     * @throws SettingNotFoundException Thrown if a setting by the given
     * name can't be found or the setting value is not an integer.
     *
     * @return The value at the given index of settings.
     * @hide
     */
    static CARAPI GetInt32AtIndex(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ Int32 index,
        /* [out] */ Int32* val);

    /**
     * Convenience function for updating settings value as coma separated
     * integer values. This will either create a new entry in the table if the
     * given name does not exist, or modify the value of the existing row
     * with that name.  Note that internally setting values are always
     * stored as strings, so this function converts the given value to a
     * string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param index The index of the list
     * @param value The new value for the setting to be added to the list.
     * @return true if the value was set, false on database errors
     * @hide
     */
    static CARAPI PutInt32AtIndex(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ Int32 index,
        /* [in] */ Int32 val,
        /* [out] */ Boolean* result);

    /**
     * Gets the telephony property.
     *
     * @hide
     */
    static CARAPI GetTelephonyProperty(
        /* [in] */ const String& property,
        /* [in] */ Int64 subId,
        /* [in] */ const String& defaultVal,
        /* [out] */ String* result);

private:
    CARAPI_(Int32) GetPhoneTypeFromProperty();

    CARAPI_(Int32) GetPhoneTypeFromProperty(
        /* [in] */ Int64 subId);

    CARAPI_(Int32) GetPhoneTypeFromNetworkType();

    CARAPI_(Int32) GetPhoneTypeFromNetworkType(
        /* [in] */ Int64 subId);

    static CARAPI_(String) GetProcCmdLine();

    CARAPI_(AutoPtr<IIPhoneSubInfo>) GetSubscriberInfo();

    CARAPI_(AutoPtr<IITelephony>) GetITelephony();

    CARAPI_(AutoPtr<IITelecomService>) GetTelecomService();

    static CARAPI_(Int64) GetDefaultSubscription();

    static CARAPI_(Boolean) InitStatic();

private:
    static const String TAG;

    static AutoPtr<IITelephonyRegistry> sRegistry;

    AutoPtr<IContext> mContext;

    static String sMultiSimConfig;

    static AutoPtr<ITelephonyManager> sInstance;

    /** Kernel command line */
    static String sKernelCmdLine;

    /** Pattern for selecting the product type from the kernel command line */
    static AutoPtr<IPattern> sProductTypePattern;

    /** The ProductType used for LTE on CDMA devices */
    static String sLteOnCdmaProductType;

    static Boolean Init;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__
