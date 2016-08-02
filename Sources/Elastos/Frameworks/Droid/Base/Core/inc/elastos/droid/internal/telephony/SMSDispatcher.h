
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSDISPATCHER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSDISPATCHER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.App.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/utility/CAtomicFile.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Core::Object;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Database::ContentObserver;
//using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SMSDispatcher
    : public Handler
    , public ISMSDispatcher
{
protected:
    class SmsTracker : public Object
    {
    public:
        SmsTracker();

        CARAPI_(Boolean) IsMultipart();

        CARAPI_(void) WriteSentMessage(IContext* context);

        CARAPI UpdateSentMessageStatus(IContext* context, Int32 status);

        CARAPI OnFailed(IContext* context, Int32 error, Int32 errorCode);

    private:
        CARAPI_(void) UpdateMessageErrorCode(IContext* context, Int32 errorCode);

        CARAPI_(void) SetMessageFinalState(IContext* context, Int32 messageType);

        CARAPI OnSent(IContext* context);

    private:
        CARAPI constructor(IHashMap* data, IPendingIntent* sentIntent,
            IPendingIntent* deliveryIntent, IPackageInfo* appInfo, String destAddr, String format,
            IAtomicInteger32* unsentPartCount, IAtomicBoolean* anyPartFailed, IUri* messageUri,
            ISmsHeader* smsHeader, Boolean isExpectMore, Int32 validityPeriod);

    public:
        // fields need to be public for derived SmsDispatchers
        AutoPtr<IHashMap> mData;
        Int32 mRetryCount;
        Int32 mImsRetry; // nonzero indicates initial message was sent over Ims
        Int32 mMessageRef;
        Boolean mExpectMore;
        Int32 mvalidityPeriod;

        String mFormat;

        AutoPtr<IPendingIntent> mSentIntent;
        AutoPtr<IPendingIntent> mDeliveryIntent;

        AutoPtr<IPackageInfo> mAppInfo;
        String mDestAddress;

        AutoPtr<ISmsHeader> mSmsHeader;

        AutoPtr<IUri> mMessageUri; // IUri* of persisted message if we wrote one
    private:
        Int64 mTimestamp;
        // Reference to states of a multipart message that this part belongs to
        AutoPtr<IAtomicInteger32> mUnsentPartCount;
        AutoPtr<IAtomicBoolean> mAnyPartFailed;
    };

    class ECO_LOCAL SMSDispatcherReceiver : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("SMSDispatcher::SMSDispatcherReceiver")

        CARAPI constructor(
            /* [in] */ SmsTracker* tracker);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AutoPtr<SmsTracker> mTracker;
    };

private:
    class ECO_LOCAL SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ IAtomicInteger32* premiumSmsRule,
            /* [in] */ IContext* context);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        const AutoPtr<IAtomicInteger32> mPremiumSmsRule;
        const AutoPtr<IContext> mContext;
    };

    class ConfirmDialogListener
        : public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnCancelListener
        , public ICompoundButtonOnCheckedChangeListener
    {
    public :
        CARAPI constructor(SmsTracker* tracker, ITextView* textView);

        CARAPI_(void) SetPositiveButton(IButton* button);

        CARAPI_(void) SetNegativeButton(IButton* button);

        CARAPI OnClick(IDialogInterface* dialog, Int32 which);

        CARAPI OnCancel(IDialogInterface* dialog);

        CARAPI OnCheckedChanged(ICompoundButton* buttonView, Boolean isChecked);

    private:
        AutoPtr<SmsTracker> mTracker;
        AutoPtr<IButton> mPositiveButton;
        AutoPtr<IButton> mNegativeButton;
        Boolean mRememberChoice;    // default is unchecked
        AutoPtr<ITextView> mRememberUndoInstruction;
    };

public:
    CAR_INTERFACE_DECL()

    SMSDispatcher();

    CARAPI Dispose();

    /**
     * Returns the premium SMS permission for the specified package. If the package has never
     * been seen before, the default {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER}
     * will be returned.
     * @param packageName the name of the package to query permission
     * @return one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_UNKNOWN},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
     */
    CARAPI GetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    /**
     * Sets the premium SMS permission for the specified package and save the value asynchronously
     * to persistent storage.
     * @param packageName the name of the package to set permission
     * @param permission one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
     */
    CARAPI SetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 permission);

    /**
     * Retry the message along to the radio.
     *
     * @param tracker holds the SMS message to send
     */
     CARAPI SendRetrySms(
         /* [in] */ SmsTracker* tracker);

    CARAPI IsIms(
        /* [out] */ Boolean* result);

    CARAPI GetImsSmsFormat(
        /* [out] */ String* result);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

protected:
    static Int32 GetNextConcatenatedRef();

    constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ISmsUsageMonitor* usageMonitor,
        /* [in] */ IImsSMSDispatcher* imsSMSDispatcher);

    void UpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    /**
     * The format of the message PDU in the associated broadcast intent.
     * This will be either "3gpp" for GSM/UMTS/LTE messages in 3GPP format
     * or "3gpp2" for CDMA/LTE messages in 3GPP2 format.
     *
     * Note: All applications which handle incoming SMS messages by processing the
     * SMS_RECEIVED_ACTION broadcast intent MUST pass the "format" extra from the intent
     * into the new methods in {@link android.telephony.SmsMessage} which take an
     * extra format parameter. This is required in order to correctly decode the PDU on
     * devices which require support for both 3GPP and 3GPP2 formats at the same time,
     * such as CDMA/LTE devices and GSM/CDMA world phones.
     *
     * @return the format of the message PDU
     */
    virtual ECode GetFormat(
        /* [out] */ String* result) = 0;

    void HandleStatusReport(
        /* [in] */ IInterface* o);

    void HandleSendComplete(
        /* [in] */ AsyncResult ar);

    static void HandleNotInService(
        /* [in] */ Int32 ss,
        /* [in] */ IPendingIntent* sentIntent);

    static Int32 GetNotInServiceError(
        /* [in] */ Int32 ss);

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param origPort the port set by the sender
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  <code>RESULT_ERROR_NO_SERVICE</code><br>.
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. if sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     */
    virtual CARAPI SendData(const String& destAddr, const String& scAddr, Int32 destPort, Int32 origPort,
            ArrayOf<Byte>* data, IPendingIntent* sentIntent, IPendingIntent* deliveryIntent) = 0;

    /**
     * Send a text based SMS.
     *  @param destAddr the address to send the message to
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
     *  <code>RESULT_ERROR_NO_SERVICE</code><br>.
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. if sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     * @param messageUri optional URI of the message if it is already stored in the system
     * @param callingPkg the calling package name
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
     *  Validity Period(Maximum) -> 635040 Mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     */
    virtual CARAPI SendText(const String& destAddr, const String& scAddr, const String& text,
            IPendingIntent* sentIntent, IPendingIntent* deliveryIntent, IUri* messageUri,
            const String& callingPkg, Int32 priority, Boolean isExpectMore, Int32 validityPeriod) = 0;

    /**
     * Inject an SMS PDU into the android platform.
     *
     * @param pdu is the Byte array of pdu to be injected into android telephony layer
     * @param format is the format of SMS Pdu (3gpp or 3gpp2)
     * @param receivedIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully received by the
     *  android telephony layer. This intent is broadcasted at
     *  the same time an SMS received from radio is responded back.
     */
    virtual CARAPI InjectSmsPdu(ArrayOf<Byte>* pdu, const String& format, IPendingIntent* receivedIntent) = 0;

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly Ignore (but still count) illegal characters if TRUE
     * @return TextEncodingDetails
     */
     //TODO
//    virtual AutoPtr<ITextEncodingDetails> CalculateLength(ICharSequence* messageBody,
//            Boolean use7bitOnly) = 0;

    /**
     * Update the status of a Pending (send-by-IP) SMS message and resend by PSTN if necessary.
     * This outbound message was handled by the carrier app. if the carrier app fails to send
     * this message, it would be resent by PSTN.
     *
     * @param messageRef the reference number of the SMS message.
     * @param success True if and only if the message was sent successfully. if its value is
     *  FALSE, this message should be resent via PSTN.
     */
    virtual void UpdateSmsSendStatus(Int32 messageRef, Boolean success) = 0;

    void SendMultipartText(const String& destAddr, const String& scAddr,
        IArrayList* parts, IArrayList* sentIntents,
        IArrayList* deliveryIntents, IUri* messageUri, const String& callingPkg,
        Int32 priority, Boolean isExpectMore, Int32 validityPeriod);

    void SendPseudoMultipartText(const String& destAddr, const String& scAddr,
        IArrayList* parts, IArrayList* sentIntents,
        IArrayList* deliveryIntents,
        IUri* messageUri, const String& callingPkg,
        Int32 priority, Boolean isExpectMore, Int32 validityPeriod);

    /**
     * Create a new SubmitPdu and send it.
     */
    virtual CARAPI SendNewSubmitPdu(const String& destinationAddress, const String& scAddress,
            const String& message, ISmsHeader* smsHeader, Int32 encoding,
            IPendingIntent* sentIntent, IPendingIntent* deliveryIntent, Boolean lastPart, Int32 priority,
            Boolean isExpectMore, Int32 validityPeriod, IAtomicInteger32* unsentPartCount,
            IAtomicBoolean* anyPartFailed, IUri* messageUri) = 0;

    void SendRawPdu(SmsTracker* tracker);

    void HandleReachSentLimit(SmsTracker* tracker);

    void HandleConfirmShortCode(Boolean isPremium, SmsTracker* tracker);

    /**
     * Send the message along to the radio.
     *
     * @param tracker holds the SMS message to send
     */
    virtual void SendSms(SmsTracker* tracker) = 0;

    /**
     * Send the SMS via the PSTN network.
     *
     * @param tracker holds the Sms tracker ready to be sent
     */
    virtual void SendSmsByPstn(SmsTracker* tracker) = 0;

//    AutoPtr<SmsTracker> GetSmsTracker(IHashMap* data, IPendingIntent* sentIntent,
//        IPendingIntent* deliveryIntent, String format, IAtomicInteger32* unsentPartCount,
//        IAtomicBoolean* anyPartFailed, IUri* messageUri, ISmsHeader* smsHeader,
//        Boolean isExpectMore, Int32 validityPeriod);
//
//    AutoPtr<SmsTracker> GetSmsTracker(IHashMap* data, IPendingIntent* sentIntent,
//        IPendingIntent* deliveryIntent, String format, IUri* messageUri, Boolean isExpectMore);
//
//    AutoPtr<SmsTracker> GetSmsTracker(IHashMap* data, IPendingIntent* sentIntent,
//        IPendingIntent* deliveryIntent, String format, IUri* messageUri, Boolean isExpectMore,
//        Int32 validityPeriod);

//    AutoPtr<IHashMap> GetSmsTrackerMap(String destAddr, String scAddr,
//        String text, SmsMessageBase::SubmitPduBase* pdu)

//    AutoPtr<IHashMap> GetSmsTrackerMap(String destAddr, String scAddr,
//        Int32 destPort, Int32 origPort, Byte[] data, SmsMessageBase.SubmitPduBase pdu);

    AutoPtr<IUri> WriteOutboxMessage(Int64 subId, const String& address, const String& text,
        Boolean requireDeliveryReport, const String& creator);

    void MoveToOutbox(Int64 subId, IUri* messageUri, const String& creator);

    String GetCarrierAppPackageName(IIntent* intent);

    Int64 GetSubId();

private:
    static CARAPI_(Int32) InitConcatenatedRef();

    static Boolean IsSystemUid(
        /* [in] */ IContext* context,
        /* [in] */ const String& pkgName);

    Boolean CheckDestination(SmsTracker* tracker);

    Boolean DenyIfQueueLimitReached(SmsTracker* tracker);

    AutoPtr<ICharSequence> GetAppLabel(const String& appPackage);

    String ResolvePackageName(SmsTracker* tracker);

    /**
     * Send the multi-part SMS based on multipart Sms tracker
     *
     * @param tracker holds the multipart Sms tracker ready to be sent
     */
    void SendMultipartSms(SmsTracker* tracker);

    String GetMultipartMessageText(IArrayList* parts);

protected:
    /** SMS send complete. */
    static const Int32 EVENT_SEND_SMS_COMPLETE;

    /** Handle status report from {@code CdmaInboundSmsHandler}. */
    static const Int32 EVENT_HANDLE_STATUS_REPORT;

    /** Radio is ON */
    static const Int32 EVENT_RADIO_ON;

    /** IMS registration/SMS format changed */
    static const Int32 EVENT_IMS_STATE_CHANGED;

    /** Callback from RIL_REQUEST_IMS_REGISTRATION_STATE */
    static const Int32 EVENT_IMS_STATE_DONE;

    // other
    static const Int32 EVENT_NEW_ICC_SMS = 14;
    static const Int32 EVENT_ICC_CHANGED = 15;

    /** Class2 SMS  */
    static const Int32 EVENT_SMS_ON_ICC = 16;

    AutoPtr<IPhoneBase> mPhone;
    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mResolver;
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<ITelephonyManager> mTelephonyManager;

    AutoPtr<IImsSMSDispatcher> mImsSMSDispatcher;

    /* Flags indicating whether the current device allows sms service */
    Boolean mSmsCapable;
    Boolean mSmsSendDisabled;

    /* TODO: Need to figure out how to keep track of status report routing in a
     *       persistent manner. If the phone process Restarts (reboot or crash),
     *       we will lose this list and any status reports that come in after
     *       will be dropped.
     */
    /** Sent messages awaiting a delivery status report. */
    AutoPtr<IArrayList> deliveryPendingList;

    /** Outgoing messages being handled by the carrier app. */
    AutoPtr<IList> sendPendingList;

private:
    static const String TAG;    // accessed from inner class
    static const Boolean DBG;
    static const String SEND_NEXT_MSG_EXTRA;

    /** Permission required to send SMS to short codes without user confirmation. */
    static const String SEND_SMS_NO_CONFIRMATION_PERMISSION;

    static const Int32 PREMIUM_RULE_USE_SIM;
    static const Int32 PREMIUM_RULE_USE_NETWORK;
    static const Int32 PREMIUM_RULE_USE_BOTH;
    AutoPtr<IAtomicInteger32> mPremiumSmsRule;
    AutoPtr<SettingsObserver> mSettingsObserver;

    /** Retry sending a previously failed SMS message */
    static const Int32 EVENT_SEND_RETRY;

    /** Confirmation required for sending a large number of messages. */
    static const Int32 EVENT_SEND_LIMIT_REACHED_CONFIRMATION;

    /** Send the user confirmed SMS */
    static const Int32 EVENT_SEND_CONFIRMED_SMS;  // accessed from inner class

    /** Don't send SMS (user did not confirm). */
    static const Int32 EVENT_STOP_SENDING;        // accessed from inner class

    /** Confirmation required for third-party apps sending to an SMS short code. */
    static const Int32 EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE;

    /** Confirmation required for third-party apps sending to an SMS short code. */
    static const Int32 EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE;

    /** Maximum number of times to retry sending a failed SMS. */
    static const Int32 MAX_SEND_RETRIES;
    /** Delay before next send attempt on a failed SMS, in milliseconds. */
    static const Int32 SEND_RETRY_DELAY;
    /** single part SMS */
    static const Int32 SINGLE_PART_SMS;
    /** Message sending queue limit */
    static const Int32 MO_MSG_QUEUE_LIMIT;

    /**
     * Message reference for a CONCATENATED_8_BIT_REFERENCE or
     * CONCATENATED_16_BIT_REFERENCE message set.  Should be
     * incremented for each set of concatenated messages.
     * Static field shared by all dispatcher objects.
     */
    static Int32 sConcatenatedRef;

    /** Outgoing message counter. Shared by all dispatchers. */
    AutoPtr<ISmsUsageMonitor> mUsageMonitor;

    /** Number of outgoing SmsTrackers waiting for user confirmation. */
    Int32 mPendingTrackerCount;

    Boolean mSmsPseudoMultipart;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSDISPATCHER_H__
