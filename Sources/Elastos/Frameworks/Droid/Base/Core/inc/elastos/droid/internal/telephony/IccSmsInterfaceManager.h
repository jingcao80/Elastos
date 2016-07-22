
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCSMSINTERFACEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCSMSINTERFACEMANAGER_H__

#include "elastos/droid/internal/telephony/IntRangeManager.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * IccSmsInterfaceManager to provide an inter-process communication to
 * access Sms in Icc.
 */
class IccSmsInterfaceManager
    : public Object
    , public IIccSmsInterfaceManager
{
protected:
    class InnerHandler: public Handler
    {
    public:
        TO_STRING_IMPL("IccSmsInterfaceManager::InnerHandler")

        InnerHandler(
            /* [in] */ IccSmsInterfaceManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        IccSmsInterfaceManager* mHost;
    };

private:
    class CellBroadcastRangeManager: public IntRangeManager
    {
    public:
        CellBroadcastRangeManager(
            /* [in] */ IccSmsInterfaceManager* host);

    protected:
        /**
         * Called when the list of enabled ranges has changed. This will be
         * followed by zero or more calls to {@link #addRange} followed by
         * a call to {@link #finishUpdate}.
         */
        CARAPI_(void) StartUpdate();

        /**
         * Called after {@link #startUpdate} to indicate a range of enabled
         * values.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         */
        CARAPI_(void) AddRange(
            /* [in] */ Int32 startId,
            /* [in] */ Int32 endId,
            /* [in] */ Boolean selected);

        /**
         * Called to indicate the end of a range update started by the
         * previous call to {@link #startUpdate}.
         * @return TRUE if successful, FALSE otherwise
         */
        CARAPI_(Boolean) FinishUpdate();

    private:
        AutoPtr<IArrayList> mConfigList; //ArrayList<SmsBroadcastConfigInfo>
        IccSmsInterfaceManager* mHost;
    };

    class CdmaBroadcastRangeManager: public IntRangeManager
    {
    public:
        CdmaBroadcastRangeManager(
            /* [in] */ IccSmsInterfaceManager* host);

    protected:
        /**
         * Called when the list of enabled ranges has changed. This will be
         * followed by zero or more calls to {@link #addRange} followed by a
         * call to {@link #finishUpdate}.
         */
        CARAPI_(void) StartUpdate();

        /**
         * Called after {@link #startUpdate} to indicate a range of enabled
         * values.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         */
        CARAPI_(void) AddRange(
            /* [in] */ Int32 startId,
            /* [in] */ Int32 endId,
            /* [in] */ Boolean selected);

        /**
         * Called to indicate the end of a range update started by the previous
         * call to {@link #startUpdate}.
         * @return TRUE if successful, FALSE otherwise
         */
        CARAPI_(Boolean) FinishUpdate();

    private:
        AutoPtr<IArrayList> mConfigList; //ArrayList<CdmaSmsBroadcastConfigInfo>
        IccSmsInterfaceManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Update the specified message on the Icc.
     *
     * @param index record index of message to update
     * @param status new message status (STATUS_ON_ICC_READ,
     *                  STATUS_ON_ICC_UNREAD, STATUS_ON_ICC_SENT,
     *                  STATUS_ON_ICC_UNSENT, STATUS_ON_ICC_FREE)
     * @param pdu the raw PDU to store
     * @return success or not
     *
     */
    CARAPI UpdateMessageOnIccEf(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 index,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    /**
     * Copy a raw SMS PDU to the Icc.
     *
     * @param pdu the raw PDU to store
     * @param status message status (STATUS_ON_ICC_READ, STATUS_ON_ICC_UNREAD,
     *               STATUS_ON_ICC_SENT, STATUS_ON_ICC_UNSENT)
     * @return success or not
     *
     */
    CARAPI CopyMessageToIccEf(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ ArrayOf<Byte>* smsc,
        /* [out] */ Boolean* result);

    CARAPI SynthesizeMessages(
        /* [in] */ const String& originatingAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ IList/*<String>*/* messages,
        /* [in] */ Int64 timestampMillis) /*throws RemoteException*/;

    /**
     * Retrieves all messages currently stored on Icc.
     *
     * @return list of SmsRawData of all sms on Icc
     */
    CARAPI GetAllMessagesFromIccEf(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** result);

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     */
    CARAPI SendData(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param origPort the originator port set by sender
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     */
    CARAPI SendDataWithOrigPort(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Send a text based SMS.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     */
    CARAPI SendText(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Send a text based SMS with Messaging Options.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     * @param priority Priority level of the message
     *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
     *  ---------------------------------
     *  PRIORITY      | Level of Priority
     *  ---------------------------------
     *      '00'      |     Normal
     *      '01'      |     Interactive
     *      '10'      |     Urgent
     *      '11'      |     Emergency
     *  ----------------------------------
     *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     */
    CARAPI SendTextWithOptions(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    /**
     * Inject an SMS PDU into the android application framework.
     *
     * @param pdu is the Byte array of pdu to be injected into android application framework
     * @param format is the format of SMS pdu (3gpp or 3gpp2)
     * @param receivedIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully received by the
     *  android application framework. This intent is broadcasted at
     *  the same time an SMS received from radio is acknowledged back.
     */
    CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    /**
     * Update the status of a pending (send-by-IP) SMS message and resend by PSTN if necessary.
     * This outbound message was handled by the carrier app. If the carrier app fails to send
     * this message, it would be resent by PSTN.
     *
     * @param messageRef the reference number of the SMS message.
     * @param success True if and only if the message was sent successfully. If its value is
     *  FALSE, this message should be resent via PSTN.
     * {@hide}
     */
    CARAPI UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    /**
     * Send a multi-part text based SMS.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended data ("pdu").
     */
    CARAPI SendMultipartText(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IList/*<String>*/* parts,
        /* [in] */ IList/*<PendingIntent>*/* sentIntents,
        /* [in] */ IList/*<PendingIntent>*/* deliveryIntents);

    /**
     * Send a multi-part text based SMS with Messaging Options.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended data ("pdu").
     * @param priority Priority level of the message
     *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
     *  ---------------------------------
     *  PRIORITY      | Level of Priority
     *  ---------------------------------
     *      '00'      |     Normal
     *      '01'      |     Interactive
     *      '10'      |     Urgent
     *      '11'      |     Emergency
     *  ----------------------------------
     *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     */
    CARAPI SendMultipartTextWithOptions(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IList/*<String>*/* parts,
        /* [in] */ IList/*<PendingIntent>*/* sentIntents,
        /* [in] */ IList/*<PendingIntent>*/* deliveryIntents,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    CARAPI GetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    CARAPI SetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 permission);

    CARAPI EnableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI EnableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI EnableGsmBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI DisableGsmBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI EnableCdmaBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI DisableCdmaBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI IsImsSmsSupported(
        /* [out] */ Boolean* result);

    CARAPI GetImsSmsFormat(
        /* [out] */ String* result);

    CARAPI SendStoredText(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendStoredMultipartText(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IList/*<PendingIntent>*/* sentIntents,
        /* [in] */ IList/*<PendingIntent>*/* deliveryIntents);

    CARAPI GetSmsCapacityOnIcc(
        /* [out] */ Int32* result);

    /** @hide **/
    CARAPI IsShortSMSCode(
        /* [in] */ const String& destAddr,
        /* [out] */ Boolean* result);

protected:
    IccSmsInterfaceManager(
        /* [in] */ IPhoneBase* phone);

    virtual CARAPI_(void) MarkMessagesAsRead(
        /* [in] */ IArrayList* messages);

    virtual CARAPI_(void) UpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    virtual CARAPI_(void) EnforceReceiveAndSend(
        /* [in] */ const String& message);

    /**
     * create SmsRawData lists from all sms record Byte[]
     * Use NULL to indicate "free" record
     *
     * @param messages List of message records from EF_SMS.
     * @return SmsRawData list of all in-used records
     */
    virtual CARAPI_(AutoPtr<IArrayList>) BuildValidRawData(
        /* [in] */ IArrayList* messages);

    /**
     * Generates an EF_SMS record from status and raw PDU.
     *
     * @param status Message status.  See TS 51.011 10.5.3.
     * @param pdu Raw message PDU.
     * @return Byte array for the record.
     */
    virtual CARAPI_(AutoPtr<ArrayOf<Byte> >) MakeSmsRecordData(
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    CARAPI_(Boolean) SetCellBroadcastConfig(
        /* [in] */ ArrayOf</*SmsBroadcastConfigInfo*/IInterface*>* configs);

    CARAPI_(Boolean) SetCellBroadcastActivation(
        /* [in] */ Boolean activate);

    CARAPI_(Boolean) SetCdmaBroadcastConfig(
        /* [in] */ ArrayOf<IInterface*>/*CdmaSmsBroadcastConfigInfo[]*/* configs);

    CARAPI_(Boolean) SetCdmaBroadcastActivation(
        /* [in] */ Boolean activate);

    CARAPI_(Boolean) IsFailedOrDraft(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* messageUri);

    // Return an array including both the SMS text (0) and address (1)
    CARAPI_(AutoPtr<ArrayOf<String> >) LoadTextAndAddress(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* messageUri);

    CARAPI_(void) ReturnUnspecifiedFailure(
        /* [in] */ IPendingIntent* pi);

    CARAPI_(void) ReturnUnspecifiedFailure(
        /* [in] */ IList/*<PendingIntent>*/* pis);

    CARAPI EnforceCarrierPrivilege();

    CARAPI_(String) FilterDestAddress(
        /* [in] */ const String& destAddr);

protected:
    static const String TAG;
    static const Boolean DBG;

    Object mLock;
    Boolean mSuccess;
    static const Int32 EVENT_SET_BROADCAST_ACTIVATION_DONE;
    static const Int32 EVENT_SET_BROADCAST_CONFIG_DONE;
    AutoPtr<IPhoneBase> mPhone;
    AutoPtr<IContext> mContext;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<ISMSDispatcher> mDispatcher;
    AutoPtr<IHandler> mHandler;

private:
    AutoPtr<IList> mSms; //List<SmsRawData>

    AutoPtr<CellBroadcastRangeManager> mCellBroadcastRangeManager;
    AutoPtr<CdmaBroadcastRangeManager> mCdmaBroadcastRangeManager;

    static const Int32 EVENT_LOAD_DONE;
    static const Int32 EVENT_UPDATE_DONE;
    static const Int32 SMS_CB_CODE_SCHEME_MIN;
    static const Int32 SMS_CB_CODE_SCHEME_MAX;

    AutoPtr<IUserManager> mUserManager;
    Object mSelfLock;
    friend class PhoneProxy;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCSMSINTERFACEMANAGER_H__
