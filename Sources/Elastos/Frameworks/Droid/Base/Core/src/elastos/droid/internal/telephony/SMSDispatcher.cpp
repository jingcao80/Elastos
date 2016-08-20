#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/database/sqlite/SQLiteWrapper.h"
#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/internal/telephony/CSmsResponse.h"
#include "elastos/droid/internal/telephony/CGsmAlphabetTextEncodingDetails.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/SMSDispatcher.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/CTelephonySms.h"
#include "elastos/droid/provider/CTelephonySmsOutbox.h"
#include "elastos/droid/provider/CTelephonySmsSent.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/Html.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/HashMap.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.CoreLibrary.Utility.Concurrent.h"

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::Sqlite::SQLiteWrapper;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::CTelephonySms;
using Elastos::Droid::Provider::CTelephonySmsOutbox;
using Elastos::Droid::Provider::CTelephonySmsSent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::ITelephonySms;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Provider::ITelephonySmsOutbox;
using Elastos::Droid::Provider::ITelephonySmsSent;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::Html;
using Elastos::Droid::Text::IHtml;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::CRandom;
using Elastos::Utility::ICollections;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================================
//          SettingsObserver
//==============================================================================
/**
 * Observe the secure setting for updated premium sms determination rules
 */
SMSDispatcher::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ IAtomicInteger32* premiumSmsRule,
    /* [in] */ IContext* context)
    : mPremiumSmsRule(premiumSmsRule)
    , mContext(context)
{
    ContentObserver::constructor(handler);
    OnChange(FALSE); // load initial value;
}

//@Override
ECode SMSDispatcher::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::SMS_SHORT_CODE_RULE, PREMIUM_RULE_USE_SIM, &val);
    mPremiumSmsRule->Set(val);
    return NOERROR;
}

/**
 * Handler for a {@link GsmSMSDispatcher} or {@link CdmaSMSDispatcher} broadcast.
 * if SMS sending is successfuly, sends EVENT_SEND_SMS_COMPLETE message. Otherwise,
 * send the message via the GSM/CDMA network.
 */
SMSDispatcher::SMSDispatcherReceiver::SMSDispatcherReceiver(
    /* [in] */ SmsTracker* tracker,
    /* [in] */ SMSDispatcher* host)
    : mTracker(tracker)
    , mHost(host)
{
}

//@Override
ECode SMSDispatcher::SMSDispatcherReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(ITelephonySmsIntents::SMS_SEND_ACTION)) {
        Int32 rc;
        GetResultCode(&rc);
        if (rc == IActivity::RESULT_OK) {
            Logger::D(TAG, "Sending SMS by IP pending.");
            AutoPtr<IBundle> resultExtras;
            GetResultExtras(FALSE, (IBundle**)&resultExtras);
            Boolean b;
            if (resultExtras != NULL && (resultExtras->ContainsKey(String("messageref"), &b), b)) {
                resultExtras->GetInt32(String("messageref"), &mTracker->mMessageRef);
                Logger::D(TAG, "messageref = %d", mTracker->mMessageRef);
            }
            else {
                Logger::E(TAG, "Can't find messageref in result extras.");
            }
            mHost->mSendPendingList->Add(mTracker->Probe(EIID_IInterface));
        }
        else {
            Logger::D(TAG, "Sending SMS by IP failed.");
            mHost->SendSmsByPstn(mTracker);
        }
    }
    else {
        Logger::E(TAG, "unexpected BroadcastReceiver action: %s", action.string());
    }

    return NOERROR;
}

//==============================================================================
//          SmsTracker
//==============================================================================
/**
 * Keeps track of an SMS that has been sent to the RIL, until it has
 * successfully been sent, or we're done trying.
 */
SMSDispatcher::SmsTracker::SmsTracker()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTimestamp);
}

SMSDispatcher::SmsTracker::SmsTracker(
    /* [in] */ IHashMap* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ IPackageInfo* appInfo,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& format,
    /* [in] */ IAtomicInteger32* unsentPartCount,
    /* [in] */ IAtomicBoolean* anyPartFailed,
    /* [in] */ IUri* messageUri,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
    : mData(data)
    , mRetryCount(0)
    , mImsRetry(0)
    , mMessageRef(0)
    , mExpectMore(isExpectMore)
    , mValidityPeriod(validityPeriod)
    , mFormat(format)
    , mSentIntent(sentIntent)
    , mDeliveryIntent(deliveryIntent)
    , mAppInfo(appInfo)
    , mDestAddress(destAddr)
    , mSmsHeader(smsHeader)
    , mMessageUri(messageUri)
    , mUnsentPartCount(unsentPartCount)
    , mAnyPartFailed(anyPartFailed)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTimestamp);
}

/**
 * Returns whether this tracker holds a multi-part SMS.
 * @return TRUE if the tracker holds a multi-part SMS; FALSE otherwise
 */
Boolean SMSDispatcher::SmsTracker::IsMultipart()
{
    Boolean contains;
    AutoPtr<ICharSequence> key = CoreUtils::Convert("parts");
    mData->ContainsKey(key, &contains);
    return contains;
}

/**
 * Persist this as a sent message
 */
void SMSDispatcher::SmsTracker::WriteSentMessage(IContext* context)
{
    AutoPtr<IInterface> obj;
    mData->Get(CoreUtils::Convert(String("text")), (IInterface**)&obj);
    String text;
    ICharSequence::Probe(obj)->ToString(&text);
    if (!text.IsNull()) {
        Boolean deliveryReport = (mDeliveryIntent != NULL);
        // Using invalid threadId 0 here. When the message is inserted into the db, the
        // provider looks up the threadId based on the Recipient(s).
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ITelephonySmsSent> smsSent;
        CTelephonySmsSent::AcquireSingleton((ITelephonySmsSent**)&smsSent);
        AutoPtr<IUri> uri;
        smsSent->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<ITelephonySms> sms;
        CTelephonySms::AcquireSingleton((ITelephonySms**)&sms);
        sms->AddMessageToUri(cr,
                uri,
                mDestAddress,
                text /*body*/,
                String(NULL) /*subject*/,
                mTimestamp /*date*/,
                TRUE /*read*/,
                deliveryReport /*deliveryReport*/,
                0 /*threadId*/,
                (IUri**)&mMessageUri);
    }
}

/**
 * Update the status of this message if we persisted it
 */
CARAPI SMSDispatcher::SmsTracker::UpdateSentMessageStatus(IContext* context, Int32 status)
{
    if (mMessageUri != NULL) {
        // if we wrote this message in writeSentMessage, update it now
        AutoPtr<IContentValues> values;
        CContentValues::New(1, (IContentValues**)&values);
        values->Put(ITelephonyTextBasedSmsColumns::STATUS, status);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        Int32 val;
        SQLiteWrapper::Update(context, cr,
                mMessageUri, values, String(NULL), NULL, &val);
    }
    return NOERROR;
}

/**
 * Update the error_code column of a message
 *
 * @param context The Context
 * @param errorCode The error code
 */
void SMSDispatcher::SmsTracker::UpdateMessageErrorCode(IContext* context, Int32 errorCode)
{
    if (mMessageUri == NULL) {
        return;
    }
    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ITelephonyTextBasedSmsColumns::ERROR_CODE, errorCode);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    SQLiteWrapper::Update(context, cr, mMessageUri, values,
                String(NULL)/*where*/, NULL/*selectionArgs*/, &val);
    if (val != 1) {
        Logger::E(TAG, "Failed to update message error code");
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
}

/**
 * Set the final state of a message: FAILED or SENT
 *
 * @param context The Context
 * @param messageType The final message type
 */
void SMSDispatcher::SmsTracker::SetMessageFinalState(IContext* context, Int32 messageType)
{
    if (mMessageUri == NULL) {
        return;
    }
    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ITelephonyTextBasedSmsColumns::TYPE, messageType);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    SQLiteWrapper::Update(context, cr, mMessageUri, values,
                String(NULL)/*where*/, NULL/*selectionArgs*/, &val);
    if (val != 1) {
        Logger::E(TAG, "Failed to move message to %d", messageType);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
}

/**
 * Handle a failure of a single part message or a part of a multipart message
 *
 * @param context The Context
 * @param error The error to send back with
 * @param errorCode
 */
CARAPI SMSDispatcher::SmsTracker::OnFailed(IContext* context, Int32 error, Int32 errorCode)
{
    if (mAnyPartFailed != NULL) {
        mAnyPartFailed->Set(TRUE);
    }
    // is single part or last part of multipart message
    Boolean isSinglePartOrLastPart = TRUE;
    if (mUnsentPartCount != NULL) {
        Int32 val;
        mUnsentPartCount->DecrementAndGet(&val);
        isSinglePartOrLastPart = val == 0;
    }
    if (errorCode != 0) {
        UpdateMessageErrorCode(context, errorCode);
    }
    if (isSinglePartOrLastPart) {
        SetMessageFinalState(context, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED);
    }
    if (mSentIntent != NULL) {
        // try {
        // Extra information to send with the sent intent
        AutoPtr<IIntent> fillIn;
        CIntent::New((IIntent**)&fillIn);
        if (mMessageUri != NULL) {
            // Pass this to SMS apps so that they know where it is stored
            String str;
            ((Uri*)mMessageUri.Get())->ToString(&str);
            fillIn->PutExtra(String("uri"), str);
        }
        if (errorCode != 0) {
            fillIn->PutExtra(String("errorCode"), errorCode);
        }
        if (mUnsentPartCount != NULL && isSinglePartOrLastPart) {
            // Is multipart and last part
            fillIn->PutExtra(SEND_NEXT_MSG_EXTRA, TRUE);
        }
        mSentIntent->Send(context, error, fillIn);
        // } Catch (CanceledException ex) {
        //     Logger::E(TAG, "Failed to send result");
        // }
    }
    return NOERROR;
}

/**
 * Handle the sent of a single part message or a part of a multipart message
 *
 * @param context The Context
 */
CARAPI SMSDispatcher::SmsTracker::OnSent(IContext* context)
{
    // is single part or last part of multipart message
    Boolean isSinglePartOrLastPart = TRUE;
    if (mUnsentPartCount != NULL) {
        Int32 val;
        mUnsentPartCount->DecrementAndGet(&val);
        isSinglePartOrLastPart = val == 0;
    }
    if (isSinglePartOrLastPart) {
        Boolean success = TRUE;
        Boolean b = FALSE;
        if (mAnyPartFailed != NULL && (mAnyPartFailed->Get(&b), b)) {
            success = FALSE;
        }
        if (success) {
            SetMessageFinalState(context, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT);
        }
        else {
            SetMessageFinalState(context, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED);
        }
    }
    if (mSentIntent != NULL) {
        // try {
        // Extra information to send with the sent intent
        AutoPtr<IIntent> fillIn;
        CIntent::New((IIntent**)&fillIn);
        if (mMessageUri != NULL) {
            // Pass this to SMS apps so that they know where it is stored
            String str;
            ((Uri*)mMessageUri.Get())->ToString(&str);
            fillIn->PutExtra(String("uri"), str);
        }
        if (mUnsentPartCount != NULL && isSinglePartOrLastPart) {
            // Is multipart and last part
            fillIn->PutExtra(SEND_NEXT_MSG_EXTRA, TRUE);
        }
        mSentIntent->Send(context, IActivity::RESULT_OK, fillIn);
        // } Catch (CanceledException ex) {
        //     Logger::E(TAG, "Failed to send result");
        // }
    }
    return NOERROR;
}

//==============================================================================
//          ConfirmDialogListener
//==============================================================================
/**
 * Dialog listener for SMS confirmation dialog.
 */

CAR_INTERFACE_IMPL_3(SMSDispatcher::ConfirmDialogListener, Object,
        IDialogInterfaceOnClickListener, IDialogInterfaceOnCancelListener,
        ICompoundButtonOnCheckedChangeListener)

SMSDispatcher::ConfirmDialogListener::ConfirmDialogListener(
    /* [in] */ SmsTracker* tracker,
    /* [in] */ ITextView* textView,
    /* [in] */ SMSDispatcher* host)
    : mTracker(tracker)
    , mRememberUndoInstruction(textView)
    , mHost(host)
{
}

void SMSDispatcher::ConfirmDialogListener::SetPositiveButton(IButton* button)
{
    mPositiveButton = button;
}

void SMSDispatcher::ConfirmDialogListener::SetNegativeButton(IButton* button)
{
    mNegativeButton = button;
}

//@Override
CARAPI SMSDispatcher::ConfirmDialogListener::OnClick(IDialogInterface* dialog, Int32 which)
{
    // Always set the SMS permission so that Settings will show a permission setting
    // for the App (it won't be shown until after the app tries to send to a short code).
   Int32 newSmsPermission = ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_ASK_USER;

    if (which == IDialogInterface::BUTTON_POSITIVE) {
        Logger::D(TAG, "CONFIRM sending SMS");
        // XXX this is lossy- apps can have more than one signature
        // EventLog->WriteEvent(EventLogTags.EXP_DET_SMS_SENT_BY_USER,
        //                     mTracker->mAppInfo.applicationInfo == NULL ?
        //                     -1 : mTracker->mAppInfo.applicationInfo.uid);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(EVENT_SEND_CONFIRMED_SMS,
                mTracker->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean b;
        mHost->SendMessage(msg, &b);
        if (mRememberChoice) {
            newSmsPermission = ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW;
        }
    }
    else if (which == IDialogInterface::BUTTON_NEGATIVE) {
        Logger::D(TAG, "DENY sending SMS");
        // XXX this is lossy- apps can have more than one signature
        // EventLog->WriteEvent(EventLogTags.EXP_DET_SMS_DENIED_BY_USER,
        //                     mTracker->mAppInfo.applicationInfo == NULL ?
        //                     -1 :  mTracker->mAppInfo.applicationInfo.uid);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(EVENT_STOP_SENDING,
                mTracker->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean b;
        mHost->SendMessage(msg, &b);
        if (mRememberChoice) {
            newSmsPermission = ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_NEVER_ALLOW;
        }
    }
    String name;
    mTracker->mAppInfo->GetPackageName(&name);
    mHost->SetPremiumSmsPermission(name, newSmsPermission);
    return NOERROR;
}

//@Override
CARAPI SMSDispatcher::ConfirmDialogListener::OnCancel(IDialogInterface* dialog)
{
   Logger::D(TAG, "dialog dismissed: don't send SMS");
   AutoPtr<IMessage> msg;
   mHost->ObtainMessage(EVENT_STOP_SENDING,
        mTracker->Probe(EIID_IInterface), (IMessage**)&msg);
   Boolean b;
   mHost->SendMessage(msg, &b);
   return NOERROR;
}

//@Override
CARAPI SMSDispatcher::ConfirmDialogListener::OnCheckedChanged(ICompoundButton* buttonView, Boolean isChecked)
{
    Logger::D(TAG, "remember this choice: %d", isChecked);
    mRememberChoice = isChecked;
    if (isChecked) {
        ITextView::Probe(mPositiveButton)->SetText(
                R::string::sms_short_code_confirm_always_allow);
        ITextView::Probe(mNegativeButton)->SetText(
                R::string::sms_short_code_confirm_never_allow);
        if (mRememberUndoInstruction != NULL) {
            ITextView::Probe(mRememberUndoInstruction)->SetText(
                    R::string::sms_short_code_remember_undo_instruction);
            IView::Probe(mRememberUndoInstruction)->SetPadding(0,0,0,32);
        }
    }
    else {
        ITextView::Probe(mPositiveButton)->SetText(
                R::string::sms_short_code_confirm_allow);
        ITextView::Probe(mNegativeButton)->SetText(
                R::string::sms_short_code_confirm_deny);
        if (mRememberUndoInstruction != NULL) {
            mRememberUndoInstruction->SetText(CoreUtils::Convert(String("")));
            IView::Probe(mRememberUndoInstruction)->SetPadding(0,0,0,0);
        }
    }
    return NOERROR;
}


const String SMSDispatcher::TAG("SMSDispatcher");    // accessed from inner class
const Boolean SMSDispatcher::DBG = FALSE;
const String SMSDispatcher::SEND_NEXT_MSG_EXTRA("SendNextMsg");

/** Permission required to send SMS to short codes without user confirmation. */
const String SMSDispatcher::SEND_SMS_NO_CONFIRMATION_PERMISSION("android.permission.SEND_SMS_NO_CONFIRMATION");

const Int32 SMSDispatcher::PREMIUM_RULE_USE_SIM = 1;
const Int32 SMSDispatcher::PREMIUM_RULE_USE_NETWORK = 2;
const Int32 SMSDispatcher::PREMIUM_RULE_USE_BOTH = 3;

/** SMS send complete. */
const Int32 SMSDispatcher::EVENT_SEND_SMS_COMPLETE;

/** Retry sending a previously failed SMS message */
const Int32 SMSDispatcher::EVENT_SEND_RETRY;


/** Confirmation required for sending a large number of messages. */
const Int32 SMSDispatcher::EVENT_SEND_LIMIT_REACHED_CONFIRMATION;

/** Send the user confirmed SMS */
const Int32 SMSDispatcher::EVENT_SEND_CONFIRMED_SMS;  // accessed from inner class

/** Don't send SMS (user did not confirm). */
const Int32 SMSDispatcher::EVENT_STOP_SENDING;        // accessed from inner class

/** Confirmation required for third-party apps sending to an SMS short code. */
const Int32 SMSDispatcher::EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE;

/** Confirmation required for third-party apps sending to an SMS short code. */
const Int32 SMSDispatcher::EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE;

/** Handle status report from {@code CdmaInboundSmsHandler}. */
const Int32 SMSDispatcher::EVENT_HANDLE_STATUS_REPORT;

/** Radio is ON */
const Int32 SMSDispatcher::EVENT_RADIO_ON;

/** IMS registration/SMS format changed */
const Int32 SMSDispatcher::EVENT_IMS_STATE_CHANGED;

/** Callback from RIL_REQUEST_IMS_REGISTRATION_STATE */
const Int32 SMSDispatcher::EVENT_IMS_STATE_DONE;

// other
const Int32 SMSDispatcher::EVENT_NEW_ICC_SMS; // = 14;
const Int32 SMSDispatcher::EVENT_ICC_CHANGED; // = 15;

/** Class2 SMS  */
const Int32 SMSDispatcher::EVENT_SMS_ON_ICC; // = 16;

/** Maximum number of times to retry sending a failed SMS. */
const Int32 SMSDispatcher::MAX_SEND_RETRIES = 3;
/** Delay before next send attempt on a failed SMS, in milliseconds. */
const Int32 SMSDispatcher::SEND_RETRY_DELAY = 2000;
/** single part SMS */
const Int32 SMSDispatcher::SINGLE_PART_SMS = 1;
/** Message sending queue limit */
const Int32 SMSDispatcher::MO_MSG_QUEUE_LIMIT = 5;

/**
 * Message reference for a CONCATENATED_8_BIT_REFERENCE or
 * CONCATENATED_16_BIT_REFERENCE message set.  Should be
 * incremented for each set of concatenated messages.
 * Static field shared by all dispatcher objects.
 */
// TODO:
Int32 SMSDispatcher::sConcatenatedRef = InitConcatenatedRef();

CAR_INTERFACE_IMPL(SMSDispatcher, Handler, ISMSDispatcher)

Int32 SMSDispatcher::InitConcatenatedRef()
{
    AutoPtr<IRandom> rand;
    CRandom::New((IRandom**)&rand);
    Int32 ConcatenatedRef;
    rand->NextInt32(256, &ConcatenatedRef);
    return ConcatenatedRef;
}

Int32 SMSDispatcher::GetNextConcatenatedRef()
{
    sConcatenatedRef += 1;
    return sConcatenatedRef;
}

SMSDispatcher::SMSDispatcher()
    : mSmsCapable(TRUE)
    , mSmsSendDisabled(FALSE)
    , mPendingTrackerCount(0)
    , mSmsPseudoMultipart(FALSE)
{
    Handler::constructor();
    CAtomicInteger32::New(PREMIUM_RULE_USE_SIM, (IAtomicInteger32**)&mPremiumSmsRule);

    CArrayList::New((IArrayList**)&mDeliveryPendingList);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IArrayList> arrayList;
    CArrayList::New((IArrayList**)&arrayList);
    helper->SynchronizedList(IList::Probe(arrayList), (IList**)&mSendPendingList);
}

/**
 * Create a new SMS dispatcher.
 * @param phone the Phone to use
 * @param usageMonitor the SmsUsageMonitor to use
 */
ECode SMSDispatcher::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ISmsUsageMonitor* usageMonitor,
    /* [in] */ IImsSMSDispatcher* imsSMSDispatcher)
{
    mPhone = phone;
    mImsSMSDispatcher = imsSMSDispatcher;
    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    mContext->GetContentResolver((IContentResolver**)&mResolver);

    mCi = ((PhoneBase*)phone)->mCi;
    mUsageMonitor = usageMonitor;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    mTelephonyManager = ITelephonyManager::Probe(obj) ;
    mSettingsObserver = new SettingsObserver(this, mPremiumSmsRule, mContext);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(
            ISettingsGlobal::SMS_SHORT_CODE_RULE, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, mSettingsObserver);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(
            R::bool_::config_sms_capable, &mSmsCapable);
    SystemProperties::GetBoolean(
            ITelephonyProperties::PROPERTY_SMS_SEND, mSmsCapable, &mSmsSendDisabled);
    mSmsSendDisabled = !mSmsSendDisabled;
    SystemProperties::GetBoolean(String("telephony.sms.pseudo_multipart"), FALSE,
            &mSmsPseudoMultipart);
    String str;
    GetFormat(&str);
    Logger::D(TAG, "SMSDispatcher: ctor mSmsCapable=%d format=%s mSmsSendDisabled=%d",
            mSmsCapable, str.string(), mSmsSendDisabled);

    return NOERROR;
}


void SMSDispatcher::UpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    mPhone = phone;
    mUsageMonitor = ((PhoneBase*)phone)->mSmsUsageMonitor;
    String str;
    IPhone::Probe(mPhone)->GetPhoneName(&str);
    Logger::D(TAG, "Active phone changed to %s", str.string() );
}

/** Unregister for incoming SMS events. */
ECode SMSDispatcher::Dispose()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(mSettingsObserver);
    return NOERROR;
}

/**
 * Pass the Message object to subclass to handle. Currently used to pass CDMA status reports
 * from {@link com.android.internal.telephony.cdma.CdmaInboundSmsHandler}.
 * @param o the SmsMessage containing the status report
 */
ECode SMSDispatcher::HandleStatusReport(
    /* [in] */ IInterface* o)
{
    Logger::D(TAG, "HandleStatusReport() called with no subclass.");
    return NOERROR;
}

/**
 * Handles events coming from the phone stack. Overridden from handler.
 *
 * @param msg the message to handle
 */
//@Override
ECode SMSDispatcher::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
    case EVENT_SEND_SMS_COMPLETE:
        // An outbound SMS has been successfully transferred, or failed.
        HandleSendComplete((AsyncResult*)IAsyncResult::Probe(obj));
        break;

    case EVENT_SEND_RETRY:
        Logger::D(TAG, "SMS retry..");
        SendRetrySms((SmsTracker*)IObject::Probe(obj));
        break;

    case EVENT_SEND_LIMIT_REACHED_CONFIRMATION:
        HandleReachSentLimit((SmsTracker*)IObject::Probe(obj));
        break;

    case EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE:
        HandleConfirmShortCode(FALSE, (SmsTracker*)IObject::Probe(obj));
        break;

    case EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE:
        HandleConfirmShortCode(TRUE, (SmsTracker*)IObject::Probe(obj));
        break;

    case EVENT_SEND_CONFIRMED_SMS:
    {
        AutoPtr<SmsTracker> tracker = (SmsTracker*)IObject::Probe(obj);
        if (tracker->IsMultipart()) {
            SendMultipartSms(tracker);
        }
        else {
            if (mPendingTrackerCount > 1) {
                tracker->mExpectMore = TRUE;
            }
            else {
                tracker->mExpectMore = FALSE;
            }
            SendSms(tracker);
        }
        mPendingTrackerCount--;
        break;
    }

    case EVENT_STOP_SENDING:
    {
        AutoPtr<SmsTracker> tracker = (SmsTracker*)IObject::Probe(obj);
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_LIMIT_EXCEEDED, 0/*errorCode*/);
        mPendingTrackerCount--;
        break;
    }

    case EVENT_HANDLE_STATUS_REPORT:
        HandleStatusReport(obj);
        break;

    default:
        Logger::E(TAG, "HandleMessage() ignoring message of unexpected type %d", what);
    }
    return NOERROR;
}

Boolean SMSDispatcher::IsSystemUid(
    /* [in] */ IContext* context,
    /* [in] */ const String& pkgName)
{
   AutoPtr<IPackageManager> packageManager;
   context->GetPackageManager((IPackageManager**)&packageManager);
   // try {
   AutoPtr<IPackageInfo> pi;
   packageManager->GetPackageInfo(pkgName, 0, (IPackageInfo**)&pi);
   AutoPtr<IApplicationInfo> ai;
   pi->GetApplicationInfo((IApplicationInfo**)&ai);
   Int32 uid;
   ai->GetUid(&uid);
   return uid == IProcess::SYSTEM_UID;
   // } Catch (PackageManager.NameNotFoundException e) {
   //     return FALSE;
   // }
}

/**
 * Called when SMS send completes. Broadcasts a sentIntent on success.
 * On failure, either sets up retries or broadcasts a sentIntent with
 * the failure in the result code.
 *
 * @param ar AsyncResult passed into the message handler.  ar->mResult should
 *           an SmsResponse instance if send was successful.  ar.userObj
 *           should be an SmsTracker instance.
 */
void SMSDispatcher::HandleSendComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<SmsTracker> tracker = (SmsTracker*)IObject::Probe(ar->mUserObj);
    AutoPtr<IPendingIntent> sentIntent = tracker->mSentIntent;

    if (ar->mResult != NULL) {
        tracker->mMessageRef = ((CSmsResponse*)ISmsResponse::Probe(ar->mResult))->mMessageRef;
    }
    else {
        Logger::D(TAG, "SmsResponse was NULL");
    }

    if (ar->mException == NULL) {
        // if (DBG) Logger::D(TAG, "SMS send complete. Broadcasting intent: " + sentIntent);

        if (tracker->mDeliveryIntent != NULL) {
            // Expecting a status report.  Add it to the list.
            mDeliveryPendingList->Add(tracker->Probe(EIID_IInterface));
        }
        tracker->OnSent(mContext);
    }
    else {
        if (DBG) Logger::D(TAG, "SMS send failed");

        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Int32 ss;
        serviceState->GetState(&ss);

        if ( tracker->mImsRetry > 0 && ss != IServiceState::STATE_IN_SERVICE) {
            // This is retry after failure over IMS but voice is not available.
            // Set retry to max allowed, so no retry is sent and
            //   cause RESULT_ERROR_GENERIC_FAILURE to be returned to app.
            tracker->mRetryCount = MAX_SEND_RETRIES;

            // Logger::D(TAG, "handleSendComplete: Skipping retry: "
            // +" IsIms()="+IsIms()
            // +" mRetryCount="+tracker->mRetryCount
            // +" mImsRetry="+tracker->mImsRetry
            // +" mMessageRef="+tracker->mMessageRef
            // +" SS= "+mPhone->GetServiceState()->GetState());
        }

        // if sms over IMS is not supported on data and voice is not available...
        Boolean b;
        if ((IsIms(&b), !b) && ss != IServiceState::STATE_IN_SERVICE) {
            Int32 err = GetNotInServiceError(ss);
            tracker->OnFailed(mContext, err, 0/*errorCode*/);
        }
// TODO: Need CommandException.Error
        // else if ((((CommandException)(ar->mException)).GetCommandError()
        //         == CommandException.Error.SMS_FAIL_RETRY) &&
        //        tracker->mRetryCount < MAX_SEND_RETRIES) {
        //     // Retry after a delay if needed.
        //     // TODO: According to TS 23.040, 9.2.3.6, we should resend
        //     //       with the same TP-MR as the failed message, and
        //     //       TP-RD set to 1.  However, we don't have a means of
        //     //       knowing the MR for the failed Message (EF_SMSstatus
        //     //       may or may not have the MR corresponding to this
        //     //       message, depending on the failure).  Also, in some
        //     //       implementations this retry is handled by the baseband.
        //     tracker->mRetryCount++;
        //     AutoPtr<IMessage> retryMsg;
        //     ObtainMessage(EVENT_SEND_RETRY, tracker, (IMessage**)&retryMsg);
        //     SendMessageDelayed(retryMsg, SEND_RETRY_DELAY);
        // }
        else {
            Int32 errorCode = 0;
            if (ar->mResult != NULL) {
                errorCode = ((CSmsResponse*)ISmsResponse::Probe(ar->mResult))->mErrorCode;
            }
            Int32 error = ISmsManager::RESULT_ERROR_GENERIC_FAILURE;
// TODO: Need CommandException.Error
            // if (((CommandException)(ar->mException)).GetCommandError()
            //         == CommandException.Error.FDN_CHECK_FAILURE) {
            //     error = RESULT_ERROR_FDN_CHECK_FAILURE;
            // }
            tracker->OnFailed(mContext, error, errorCode);
        }
    }
}

/**
 * Handles outbound message when the phone is not in service.
 *
 * @param ss     Current service state.  Valid values are:
 *                  OUT_OF_SERVICE
 *                  EMERGENCY_ONLY
 *                  POWER_OFF
 * @param sentIntent the IPendingIntent* to send the error to
 */
void SMSDispatcher::HandleNotInService(
    /* [in] */ Int32 ss,
    /* [in] */ IPendingIntent* sentIntent)
{
    if (sentIntent != NULL) {
        // try {
        if (ss == IServiceState::STATE_POWER_OFF) {
            sentIntent->Send(ISmsManager::RESULT_ERROR_RADIO_OFF);
        }
        else {
            sentIntent->Send(ISmsManager::RESULT_ERROR_NO_SERVICE);
        }
        // } Catch (CanceledException ex) {}
    }
}

/**
 * @param ss service state
 * @return The result error based on input service state for not in service error
 */
Int32 SMSDispatcher::GetNotInServiceError(
    /* [in] */ Int32 ss)
{
    if (ss == IServiceState::STATE_POWER_OFF) {
        return ISmsManager::RESULT_ERROR_RADIO_OFF;
    }
    return ISmsManager::RESULT_ERROR_NO_SERVICE;
}



/**
 * Send a multi-part text based SMS.
 *  @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *   the current default SMSC
 * @param parts an <code>ArrayList</code> of strings that, in order,
 *   comprise the original message
 * @param sentIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>S (one for each message part) that is
 *   broadcast when the corresponding message part has been sent.
 *   The result code will be <code>Activity.RESULT_OK<code> for success,
 *   or one of these errors:
 *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
 *   <code>RESULT_ERROR_RADIO_OFF</code>
 *   <code>RESULT_ERROR_NULL_PDU</code>
 *   <code>RESULT_ERROR_NO_SERVICE</code>.
 *  The per-application based SMS control checks sentIntent. if sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>S (one for each message part) that is
 *   broadcast when the corresponding message part has been delivered
 *   to the recipient.  The raw pdu of the status report is in the
 * @param messageUri optional IUri* of the message if it is already stored in the system
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
void SMSDispatcher::SendMultipartText(const String& destAddr, const String& scAddr,
        IArrayList* parts, IArrayList* sentIntents,
        IArrayList* deliveryIntents, IUri* messageUri, const String& callingPkg,
        Int32 priority, Boolean isExpectMore, Int32 validityPeriod)
{
    if (mSmsPseudoMultipart) {
        // Send as individual messages as the combination of device and
        // carrier behavior may not process concatenated messages correctly.
        SendPseudoMultipartText(destAddr, scAddr, parts, sentIntents, deliveryIntents,
                messageUri, callingPkg, priority, isExpectMore, validityPeriod);
        return;
    }

    Int64 subId = GetSubId();
    if (messageUri == NULL) {
        AutoPtr<ISmsApplication> application;
        CSmsApplication::AcquireSingleton((ISmsApplication**)&application);
        Boolean b;
        if (application->ShouldWriteMessageForPackage(callingPkg, mContext, &b), b) {
            Int32 size;
            messageUri = WriteOutboxMessage(
                    subId,
                    destAddr,
                    GetMultipartMessageText(parts),
                    deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > 0,
                    callingPkg);
        }
    }
    else {
        MoveToOutbox(subId, messageUri, callingPkg);
    }
    Int32 refNumber = GetNextConcatenatedRef() & 0x00FF;
    Int32 msgCount;
    parts->GetSize(&msgCount);
    Int32 encoding = ISmsConstants::ENCODING_UNKNOWN;

    AutoPtr<ArrayOf<IGsmAlphabetTextEncodingDetails*> > encodingForParts =
            ArrayOf<IGsmAlphabetTextEncodingDetails*>::Alloc(msgCount);

    for (Int32 i = 0; i < msgCount; i++) {
        AutoPtr<IInterface> obj;
        parts->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmAlphabetTextEncodingDetails> details;
        CalculateLength(ICharSequence::Probe(obj), FALSE,
                (IGsmAlphabetTextEncodingDetails**)&details);
        Int32 codeUnitSize;
        details->GetCodeUnitSize(&codeUnitSize);
        if (encoding != codeUnitSize
                && (encoding == ISmsConstants::ENCODING_UNKNOWN
                        || encoding == ISmsConstants::ENCODING_7BIT)) {
            encoding = codeUnitSize;
        }
        (*encodingForParts)[i] = details;
    }

    // States to track at the message Level (for all parts)
    AutoPtr<IAtomicInteger32> unsentPartCount;
    CAtomicInteger32::New(msgCount, (IAtomicInteger32**)&unsentPartCount);
    AutoPtr<IAtomicBoolean> anyPartFailed;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&anyPartFailed);

    for (Int32 i = 0; i < msgCount; i++) {
        AutoPtr<SmsHeader> smsHeader;
        AutoPtr<SmsHeader::ConcatRef> concatRef = new SmsHeader::ConcatRef();
        concatRef->refNumber = refNumber;
        concatRef->seqNumber = i + 1;  // 1-based sequence
        concatRef->msgCount = msgCount;
        // TODO: We currently set this to TRUE since our messaging app will never
        // send more than 255 Parts (it converts the message to MMS well before that).
        // However, we should support 3rd party messaging apps that might need 16-bit
        // references
        // Note:  It's not sufficient to just flip this bit to TRUE; it will have
        // ripple Effects (several calculations assume 8-bit ref).
        concatRef->isEightBits = TRUE;
        smsHeader = new SmsHeader();
        smsHeader->mConcatRef = concatRef;

        // Set the national language tables for 3GPP 7-bit encoding, if enabled.
        if (encoding == ISmsConstants::ENCODING_7BIT) {
            smsHeader->mLanguageTable = ((CGsmAlphabetTextEncodingDetails*)(*encodingForParts)[i])->mLanguageTable;
            smsHeader->mLanguageShiftTable = ((CGsmAlphabetTextEncodingDetails*)(*encodingForParts)[i])->mLanguageShiftTable;
        }

        Int32 size;
        AutoPtr<IInterface> obj;
        AutoPtr<IPendingIntent> sentIntent;
        if (sentIntents != NULL && (sentIntents->GetSize(&size), size) > i) {
            sentIntents->Get(i, (IInterface**)&obj);
            sentIntent = IPendingIntent::Probe(obj);
        }

        AutoPtr<IPendingIntent> deliveryIntent;
        if (deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > i) {
            obj = NULL;
            deliveryIntents->Get(i, (IInterface**)&obj);
            deliveryIntent = IPendingIntent::Probe(obj);
        }

        obj = NULL;
        parts->Get(i, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        SendNewSubmitPdu(destAddr, scAddr, str, smsHeader, encoding,
                sentIntent, deliveryIntent, (i == (msgCount - 1)), priority, isExpectMore,
                validityPeriod, unsentPartCount, anyPartFailed, messageUri);
    }
}

/**
 * Send a multi-part text based SMS as individual messages
 * (i.e., without User Data Headers).
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 * the current default SMSC
 * @param parts an <code>ArrayList</code> of strings that, in order,
 * comprise the original message
 * @param sentIntents if not NULL, an <code>ArrayList</code> of
 * <code>PendingIntent</code>S (one for each message part) that is
 * broadcast when the corresponding message part has been sent.
 * The result code will be <code>Activity.RESULT_OK<code> for success,
 * or one of these errors:
 * <code>RESULT_ERROR_GENERIC_FAILURE</code>
 * <code>RESULT_ERROR_RADIO_OFF</code>
 * <code>RESULT_ERROR_NULL_PDU</code>
 * <code>RESULT_ERROR_NO_SERVICE</code>.
 * The per-application based SMS control checks sentIntent. if sentIntent
 * is NULL the caller will be checked against all unknown applications,
 * which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
 * <code>PendingIntent</code>S (one for each message part) that is
 * broadcast when the corresponding message part has been delivered
 * to the recipient. The raw pdu of the status report is in the
 * extended Data ("pdu").
 * @param messageUri optional IUri* of the message if it is already stored in the system
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
void SMSDispatcher::SendPseudoMultipartText(const String& destAddr, const String& scAddr,
        IArrayList* parts, IArrayList* sentIntents,
        IArrayList* deliveryIntents,
        IUri* messageUri, const String& callingPkg,
        Int32 priority, Boolean isExpectMore, Int32 validityPeriod)
{
    Int32 msgCount;
    parts->GetSize(&msgCount);

    for (Int32 i = 0; i < msgCount; i++) {
        Int32 size;
        AutoPtr<IInterface> obj;
        AutoPtr<IPendingIntent> sentIntent;
        if (sentIntents != NULL && (sentIntents->GetSize(&size), size) > i) {
            sentIntents->Get(i, (IInterface**)&obj);
            sentIntent = IPendingIntent::Probe(obj);
        }

        AutoPtr<IPendingIntent> deliveryIntent;
        if (deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > i) {
            obj = NULL;
            deliveryIntents->Get(i, (IInterface**)&obj);
            deliveryIntent = IPendingIntent::Probe(obj);
        }

        obj = NULL;
        parts->Get(i, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        SendText(destAddr, scAddr, str, sentIntent, deliveryIntent,
                 messageUri, callingPkg, priority, isExpectMore, validityPeriod);
    }
}

/**
 * Send a SMS
 * @param tracker will contain:
 * -smsc the SMSC to send the message through, or NULL for the
 *  default SMSC
 * -pdu the raw PDU to send
 * -sentIntent if not NULL this <code>Intent</code> is
 *  broadcast when the message is successfully sent, or failed.
 *  The result code will be <code>Activity.RESULT_OK<code> for success,
 *  or one of these errors:
 *  <code>RESULT_ERROR_GENERIC_FAILURE</code>
 *  <code>RESULT_ERROR_RADIO_OFF</code>
 *  <code>RESULT_ERROR_NULL_PDU</code>
 *  <code>RESULT_ERROR_NO_SERVICE</code>.
 *  The per-application based SMS control checks sentIntent. if sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * -deliveryIntent if not NULL this <code>Intent</code> is
 *  broadcast when the message is delivered to the recipient.  The
 *  raw pdu of the status report is in the extended Data ("pdu").
 * -param destAddr the destination phone Number (for short code confirmation)
 */
void SMSDispatcher::SendRawPdu(SmsTracker* tracker)
{
    AutoPtr<IHashMap> map = tracker->mData;
    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(String("pdu")), (IInterface**)&obj);
    AutoPtr<IArrayOf> pdu = IArrayOf::Probe(obj);
    // AutoPtr<ArrayOf<Byte> > pdu = (Byte[]) map->Get("pdu");

    if (mSmsSendDisabled) {
        Logger::E(TAG, "Device does not support sending sms.");
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_NO_SERVICE, 0/*errorCode*/);
        return;
    }

    if (pdu == NULL) {
        Logger::E(TAG, "Empty PDU");
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_NULL_PDU, 0/*errorCode*/);
        return;
    }

    AutoPtr<IPendingIntent> sentIntent = tracker->mSentIntent;
    // Get calling app package name via UID from Binder call
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 callingUid = Binder::GetCallingUid();
    // Special case: We're being proxied by the telephony stack itself,
    // so use the intent generator's UID if one exists
    AutoPtr<ArrayOf<String> > packageNames;

    String creatorPkg;
    if (callingUid == IProcess::PHONE_UID && sentIntent != NULL &&
            (sentIntent->GetCreatorPackage(&creatorPkg), creatorPkg) != NULL) {
        packageNames = ArrayOf<String>::Alloc(1);
        (*packageNames)[0] = creatorPkg;
    }
    else {
        pm->GetPackagesForUid(callingUid, (ArrayOf<String>**)&packageNames);
    }

    if (packageNames == NULL || packageNames->GetLength() == 0) {
        // Refuse to send SMS if we can't get the calling package name.
        Logger::E(TAG, "Can't get calling app package name: refusing to send SMS");
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
        return;
    }

    // Get package info via packagemanager
    AutoPtr<IPackageInfo> appInfo;
    // try {
    // XXX this is lossy- apps can share a UID
    pm->GetPackageInfo((*packageNames)[0], IPackageManager::GET_SIGNATURES, (IPackageInfo**)&appInfo);
    // } Catch (PackageManager.NameNotFoundException e) {
    //     Logger::E(TAG, "Can't get calling app package info: refusing to send SMS");
    //     tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
    //     return;
    // }

    // CheckDestination() returns TRUE if the destination is not a premium short code or the
    // sending app is approved to send to short codes. Otherwise, a message is sent to our
    // handler with the SmsTracker to request user confirmation before sending.
    if (CheckDestination(tracker)) {
        // check for excessive outgoing SMS usage by this app
        String pkgName;
        appInfo->GetPackageName(&pkgName);
        Boolean b = FALSE;
        if (mUsageMonitor != NULL) {
            mUsageMonitor->Check(pkgName, SINGLE_PART_SMS, &b);
        }
        if (!b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SEND_LIMIT_REACHED_CONFIRMATION,
                    tracker->Probe(EIID_IInterface), (IMessage**)&msg);
            SendMessage(msg, &b);
            return;
        }

        SendSms(tracker);
    }
}

/**
 * Check if destination is a potential premium short code and sender is not pre-approved to
 * send to short codes.
 *
 * @param tracker the tracker for the SMS to send
 * @return TRUE if the destination is approved; FALSE if user confirmation event was sent
 */
Boolean SMSDispatcher::CheckDestination(SmsTracker* tracker)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > array;
    res->GetStringArray(R::array::config_ignored_sms_packages, (ArrayOf<String>**)&array);
    AutoPtr<IList> ignorePackages;
    Arrays::AsList(array, (IList**)&ignorePackages);

    String packageName = ResolvePackageName(tracker);

    Boolean b;
    if (ignorePackages->Contains(CoreUtils::Convert(packageName), &b), b) {
        return TRUE;
    }

    Int32 val;
    mContext->CheckCallingOrSelfPermission(SEND_SMS_NO_CONFIRMATION_PERMISSION, &val);
    if (val == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;            // app is pre-approved to send to short codes
    }
    else {
        Int32 rule;
        mPremiumSmsRule->Get(&rule);
        Int32 smsCategory = SmsUsageMonitor::CATEGORY_NOT_SHORT_CODE;
        if (rule == PREMIUM_RULE_USE_SIM || rule == PREMIUM_RULE_USE_BOTH) {
            String simCountryIso;
            mTelephonyManager->GetSimCountryIso(&simCountryIso);
            if (simCountryIso.IsNull() || simCountryIso.GetLength() != 2) {
                Logger::E(TAG, "Can't get SIM country Iso: trying network country Iso");
                mTelephonyManager->GetNetworkCountryIso(&simCountryIso);
            }

            if (mUsageMonitor != NULL) {
                mUsageMonitor->CheckDestination(tracker->mDestAddress, simCountryIso, &smsCategory);
            }
        }
        if (rule == PREMIUM_RULE_USE_NETWORK || rule == PREMIUM_RULE_USE_BOTH) {
            String networkCountryIso;
            mTelephonyManager->GetNetworkCountryIso(&networkCountryIso);
            if (networkCountryIso.IsNull() || networkCountryIso.GetLength() != 2) {
                Logger::E(TAG, "Can't get Network country Iso: trying SIM country Iso");
                mTelephonyManager->GetSimCountryIso(&networkCountryIso);
            }

            Int32 val = 0;
            if (mUsageMonitor != NULL) {
                mUsageMonitor->CheckDestination(tracker->mDestAddress, networkCountryIso, &val);
            }
            SmsUsageMonitor::MergeShortCodeCategories(smsCategory, val, &smsCategory);
        }

        if (smsCategory == SmsUsageMonitor::CATEGORY_NOT_SHORT_CODE
                || smsCategory == SmsUsageMonitor::CATEGORY_FREE_SHORT_CODE
                || smsCategory == SmsUsageMonitor::CATEGORY_STANDARD_SHORT_CODE) {
            return TRUE;    // not a premium short code
        }

        // Wait for user confirmation unless the user has set permission to always allow/deny
        String pkgName;
        tracker->mAppInfo->GetPackageName(&pkgName);
        Int32 premiumSmsPermission = 0;
        if (mUsageMonitor != NULL) {
            mUsageMonitor->GetPremiumSmsPermission(pkgName, &premiumSmsPermission);
        }
        if (premiumSmsPermission == ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_UNKNOWN) {
            // First time trying to send to premium SMS.
            premiumSmsPermission = ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_ASK_USER;
        }

        switch (premiumSmsPermission) {
            case ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW:
                Logger::D(TAG, "User approved this app to send to premium SMS");
                return TRUE;

            case ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_NEVER_ALLOW: {
                Logger::W(TAG, "User denied this app from sending to premium SMS");
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_STOP_SENDING, tracker->Probe(EIID_IInterface), (IMessage**)&msg);
                SendMessage(msg, &b);
                return FALSE;   // reject this message
            }
            case ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_ASK_USER:
            default: {
                Int32 event;
                if (smsCategory == SmsUsageMonitor::CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE) {
                    event = EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE;
                } else {
                    event = EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE;
                }
                AutoPtr<IMessage> msg;
                ObtainMessage(event, tracker->Probe(EIID_IInterface), (IMessage**)&msg);
                SendMessage(msg, &b);
                return FALSE;   // wait for user confirmation
            }
        }
    }
}

/**
 * Deny sending an SMS if the outgoing queue limit is reached. Used when the message
 * must be confirmed by the user due to excessive usage or potential premium SMS detected.
 * @param tracker the SmsTracker for the message to send
 * @return TRUE if the message was denied; FALSE to continue with send confirmation
 */
 Boolean SMSDispatcher::DenyIfQueueLimitReached(SmsTracker* tracker)
{
    if (mPendingTrackerCount >= MO_MSG_QUEUE_LIMIT) {
        // Deny sending message when the queue limit is reached.
        Logger::E(TAG, "Denied because queue limit reached");
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_LIMIT_EXCEEDED, 0/*errorCode*/);
        return TRUE;
    }

    mPendingTrackerCount++;
    return FALSE;
}

/**
 * Returns the label for the specified app package name.
 * @param appPackage the package name of the app requesting to send an SMS
 * @return the label for the specified app, or the package name if GetApplicationInfo() fails
 */
AutoPtr<ICharSequence> SMSDispatcher::GetAppLabel(const String& appPackage)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IApplicationInfo> appInfo;
    pm->GetApplicationInfo(appPackage, 0, (IApplicationInfo**)&appInfo);
    AutoPtr<ICharSequence> cs;
    IPackageItemInfo::Probe(appInfo)->LoadLabel(pm, (ICharSequence**)&cs);
    return cs;
    // } Catch (PackageManager.NameNotFoundException e) {
    //     Logger::E(TAG, "PackageManager Name Not Found for package " + appPackage);
    //     return appPackage;  // fall back to package name if we can't get app label
    // }
}

/**
 * Returns the package name from the original creator of the sms, even
 * if the package is mapped with others in a specific UID (like System UID)
 *
 * @param tracker
 * @return the package name that created the original sms
 */
String SMSDispatcher::ResolvePackageName(SmsTracker* tracker)
{
    AutoPtr<IPendingIntent> sentIntent = tracker->mSentIntent;
    String packageName;
    AutoPtr<IApplicationInfo> appInfo;
    tracker->mAppInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
    // System UID maps to multiple packages. Try to narrow it
    // down to an actual sender if possible
    String str;
    if (IsSystemUid(mContext, packageName) && sentIntent != NULL &&
            !(sentIntent->GetCreatorPackage(&str), str).IsNull()) {
        sentIntent->GetCreatorPackage(&packageName);
    }
    return packageName;
}

/**
 * Post an alert when SMS needs confirmation due to excessive usage.
 * @param tracker an SmsTracker for the current message.
 */
void SMSDispatcher::HandleReachSentLimit(SmsTracker* tracker)
{
    if (DenyIfQueueLimitReached(tracker)) {
        return;     // queue limit reached; error was returned to caller
    }

    String pkgName;
    tracker->mAppInfo->GetPackageName(&pkgName);
    AutoPtr<ICharSequence> appLabel = GetAppLabel(pkgName);
    AutoPtr<IResources> r = CResources::GetSystem();
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, appLabel);
    String str;
    r->GetString(R::string::sms_control_message, array, &str);
    AutoPtr<ISpanned> messageText = Html::FromHtml(str);

    AutoPtr<ConfirmDialogListener> listener = new ConfirmDialogListener(tracker, NULL, this);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::sms_control_title);
    builder->SetIcon(R::drawable::stat_sys_warning);
    builder->SetMessage(ICharSequence::Probe(messageText));
    builder->SetPositiveButton(CoreUtils::Convert((r->GetString(
            R::string::sms_control_yes, &str), str)), listener);
    builder->SetNegativeButton(CoreUtils::Convert((r->GetString(
            R::string::sms_control_no, &str), str)), listener);
    builder->SetOnCancelListener(listener);
    AutoPtr<IAlertDialog> d;
    builder->Create((IAlertDialog**)&d);
    AutoPtr<IWindow> win;
    IDialog::Probe(d)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    IDialog::Probe(d)->Show();
}

/**
 * Post an alert for user confirmation when sending to a potential short code.
 * @param isPremium TRUE if the destination is known to be a premium short code
 * @param tracker the SmsTracker for the current message.
 */
void SMSDispatcher::HandleConfirmShortCode(Boolean isPremium, SmsTracker* tracker)
{
    if (DenyIfQueueLimitReached(tracker)) {
        return;     // queue limit reached; error was returned to caller
    }

    Int32 detailsId;
    if (isPremium) {
        detailsId = R::string::sms_premium_short_code_details;
    }
    else {
        detailsId = R::string::sms_short_code_details;
    }

    String pkgName;
    tracker->mAppInfo->GetPackageName(&pkgName);
    AutoPtr<ICharSequence> appLabel = GetAppLabel(pkgName);
    AutoPtr<IResources> r = CResources::GetSystem();

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
    array->Set(0, appLabel);
    array->Set(1, CoreUtils::Convert(tracker->mDestAddress));

    String str;
    r->GetString(R::string::sms_short_code_confirm_message, array, &str);
    AutoPtr<ISpanned> messageText = Html::FromHtml(str);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::sms_short_code_confirmation_dialog, NULL, (IView**)&layout);

    AutoPtr<IView> view;
    layout->FindViewById(R::id::sms_short_code_remember_undo_instruction, (IView**)&view);
    AutoPtr<ConfirmDialogListener> listener = new ConfirmDialogListener(tracker,
            ITextView::Probe(obj), this);

    view = NULL;
    layout->FindViewById(R::id::sms_short_code_confirm_message, (IView**)&view);
    AutoPtr<ITextView> messageView = ITextView::Probe(view);
    messageView->SetText(ICharSequence::Probe(messageText));

    view = NULL;
    layout->FindViewById(R::id::sms_short_code_detail_layout, (IView**)&view);
    AutoPtr<IViewGroup> detailsLayout = IViewGroup::Probe(view);

    view = NULL;
    IView::Probe(detailsLayout)->FindViewById(R::id::sms_short_code_detail_message, (IView**)&view);
    AutoPtr<ITextView> detailsView = ITextView::Probe(view);
    detailsView->SetText(detailsId);

    view = NULL;
    layout->FindViewById(R::id::sms_short_code_remember_choice_checkbox, (IView**)&view);
    AutoPtr<ICheckBox> rememberChoice = ICheckBox::Probe(view);
    ICompoundButton::Probe(rememberChoice)->SetOnCheckedChangeListener(listener);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
    builder->SetView(layout);
    builder->SetPositiveButton(CoreUtils::Convert((r->GetString(
            R::string::sms_short_code_confirm_allow, &str), str)), listener);
    builder->SetNegativeButton(CoreUtils::Convert((r->GetString(
            R::string::sms_short_code_confirm_deny, &str), str)), listener);
    builder->SetOnCancelListener(listener);
    AutoPtr<IAlertDialog> d;
    builder->Create((IAlertDialog**)&d);
    builder->Create((IAlertDialog**)&d);
    AutoPtr<IWindow> win;
    IDialog::Probe(d)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    IDialog::Probe(d)->Show();

    AutoPtr<IButton> button;
    d->GetButton(IDialogInterface::BUTTON_POSITIVE, (IButton**)&button);
    listener->SetPositiveButton(button);

    button = NULL;
    d->GetButton(IDialogInterface::BUTTON_NEGATIVE, (IButton**)&button);
    listener->SetNegativeButton(button);
}

/**
 * Returns the premium SMS permission for the specified package. if the package has never
 * been seen before, the default {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER}
 * will be returned.
 * @param packageName the name of the package to query permission
 * @return one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_UNKNOWN},
 *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
 *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
 *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
 */
ECode SMSDispatcher::GetPremiumSmsPermission(const String& packageName, Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mUsageMonitor != NULL) {
        return mUsageMonitor->GetPremiumSmsPermission(packageName, result);
    }
    return NOERROR;
}

/**
 * Sets the premium SMS permission for the specified package and save the value asynchronously
 * to persistent storage.
 * @param packageName the name of the package to set permission
 * @param permission one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
 *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
 *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
 */
ECode SMSDispatcher::SetPremiumSmsPermission(const String& packageName, Int32 permission)
{
    if (mUsageMonitor != NULL) {
        return mUsageMonitor->SetPremiumSmsPermission(packageName, permission);
    }
    return NOERROR;
}

/**
 * Retry the message along to the radio.
 *
 * @param tracker holds the SMS message to send
 */
ECode SMSDispatcher::SendRetrySms(SmsTracker* tracker)
{
    // re-routing to ImsSMSDispatcher
    if (mImsSMSDispatcher != NULL) {
        ((SMSDispatcher*)mImsSMSDispatcher.Get())->SendRetrySms(tracker);
    }
    else {
        Logger::E(TAG, "mImsSMSDispatcher is NULL. Retry failed");
    }
    return NOERROR;
}

/**
 * Send the multi-part SMS based on multipart Sms tracker
 *
 * @param tracker holds the multipart Sms tracker ready to be sent
 */
void SMSDispatcher::SendMultipartSms(SmsTracker* tracker)
{
    AutoPtr<IArrayList> parts;
    AutoPtr<IArrayList> sentIntents;
    AutoPtr<IArrayList> deliveryIntents;

    AutoPtr<IHashMap> map = tracker->mData;


    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(String("destination")), (IInterface**)&obj);
    String destinationAddress;
    ICharSequence::Probe(obj)->ToString(&destinationAddress);

    obj = NULL;
    map->Get(CoreUtils::Convert(String("scaddress")), (IInterface**)&obj);
    String scAddress;
    ICharSequence::Probe(obj)->ToString(&scAddress);

    obj = NULL;
    map->Get(CoreUtils::Convert(String("parts")), (IInterface**)&obj);
    parts = IArrayList::Probe(obj);

    obj = NULL;
    map->Get(CoreUtils::Convert(String("sentIntents")), (IInterface**)&obj);
    sentIntents = IArrayList::Probe(obj);

    obj = NULL;
    map->Get(CoreUtils::Convert(String("deliveryIntents")), (IInterface**)&obj);
    deliveryIntents = IArrayList::Probe(obj);

    // check if in service
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 ss;
    serviceState->GetState(&ss);
    // if sms over IMS is not supported on data and voice is not available...
    Boolean b;
    if ((IsIms(&b), !b) && ss != IServiceState::STATE_IN_SERVICE) {
        Int32 size;
        parts->GetSize(&size);
        for (Int32 i = 0, count = size; i < count; i++) {
            Int32 len;
            sentIntents->GetSize(&len);
            AutoPtr<IPendingIntent> sentIntent;
            if (sentIntents != NULL && len > i) {
                obj = NULL;
                sentIntents->Get(i, (IInterface**)&obj);
                sentIntent = IPendingIntent::Probe(obj);
            }
            HandleNotInService(ss, sentIntent);
        }
        return;
    }

    SendMultipartText(destinationAddress, scAddress, parts, sentIntents, deliveryIntents,
            NULL/*messageUri*/, String(NULL)/*callingPkg*/, -1, tracker->mExpectMore,
            tracker->mValidityPeriod);
}

AutoPtr<SMSDispatcher::SmsTracker> SMSDispatcher::GetSmsTracker(
    /* [in] */ IHashMap* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ const String& format,
    /* [in] */ IAtomicInteger32* unsentPartCount,
    /* [in] */ IAtomicBoolean* anyPartFailed,
    /* [in] */ IUri* messageUri,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    // Get calling app package name via UID from Binder call
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 callingUid = Binder::GetCallingUid();
    // Special case: We're being proxied by the telephony stack itself,
    // so use the intent generator's UID if one exists
    AutoPtr<ArrayOf<String> > packageNames;

    String pkg;
    if (callingUid == IProcess::PHONE_UID && sentIntent != NULL &&
            !(sentIntent->GetCreatorPackage(&pkg), pkg).IsNull()) {
        packageNames = ArrayOf<String>::Alloc(1);
        (*packageNames)[0] = pkg;
    }
    else {
        pm->GetPackagesForUid(callingUid, (ArrayOf<String>**)&packageNames);
    }

    // Get package info via packagemanager
    AutoPtr<IPackageInfo> appInfo;
    if (packageNames != NULL && packageNames->GetLength() > 0) {
        // try {
        // XXX this is lossy- apps can share a UID
        pm->GetPackageInfo((*packageNames)[0], IPackageManager::GET_SIGNATURES, (IPackageInfo**)&appInfo);
        // } Catch (PackageManager.NameNotFoundException e) {
        //     // error will be logged in sendRawPdu
        // }
    }
    // Strip non-digits from destination phone number before checking for short codes
    // and before displaying the number to the user if confirmation is required.
    AutoPtr<IInterface> obj;
    data->Get(CoreUtils::Convert(String("destAddr")), (IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    String destAddr;
    PhoneNumberUtils::ExtractNetworkPortion(str, &destAddr);
    return new SmsTracker(data, sentIntent, deliveryIntent, appInfo, destAddr, format,
            unsentPartCount, anyPartFailed, messageUri, smsHeader, isExpectMore, validityPeriod);
}

AutoPtr<SMSDispatcher::SmsTracker> SMSDispatcher::GetSmsTracker(
    /* [in] */ IHashMap* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ const String& format,
    /* [in] */ IUri* messageUri,
    /* [in] */ Boolean isExpectMore)
{
    return GetSmsTracker(data, sentIntent, deliveryIntent, format, NULL/*unsentPartCount*/,
            NULL/*anyPartFailed*/, messageUri, NULL/*smsHeader*/, isExpectMore, -1);
}

AutoPtr<SMSDispatcher::SmsTracker> SMSDispatcher::GetSmsTracker(
    /* [in] */ IHashMap* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ const String& format,
    /* [in] */ IUri* messageUri,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    return GetSmsTracker(data, sentIntent, deliveryIntent, format, NULL/*unsentPartCount*/,
            NULL/*anyPartFailed*/, messageUri, NULL/*smsHeader*/, isExpectMore, validityPeriod);
}

AutoPtr<IHashMap> SMSDispatcher::GetSmsTrackerMap(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ SmsMessageBase::SubmitPduBase* pdu)
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    map->Put(CoreUtils::Convert(String("destAddr")), CoreUtils::Convert(destAddr));
    map->Put(CoreUtils::Convert(String("scAddr")), CoreUtils::Convert(scAddr));
    map->Put(CoreUtils::Convert(String("text")), CoreUtils::Convert(text));
    map->Put(CoreUtils::Convert(String("smsc")), CoreUtils::ConvertByteArray(pdu->mEncodedScAddress));
    map->Put(CoreUtils::Convert(String("pdu")), CoreUtils::ConvertByteArray(pdu->mEncodedMessage));
    return map;
}

AutoPtr<IHashMap> SMSDispatcher::GetSmsTrackerMap(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ SmsMessageBase::SubmitPduBase* pdu)
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    map->Put(CoreUtils::Convert(String("destAddr")), CoreUtils::Convert(destAddr));
    map->Put(CoreUtils::Convert(String("scAddr")), CoreUtils::Convert(scAddr));
    map->Put(CoreUtils::Convert(String("destPort")), CoreUtils::Convert(destPort));
    map->Put(CoreUtils::Convert(String("origPort")), CoreUtils::Convert(origPort));
    map->Put(CoreUtils::Convert(String("data")), CoreUtils::ConvertByteArray(data));
    map->Put(CoreUtils::Convert(String("smsc")), CoreUtils::ConvertByteArray(pdu->mEncodedScAddress));
    map->Put(CoreUtils::Convert(String("pdu")), CoreUtils::ConvertByteArray(pdu->mEncodedMessage));
    return map;
}

ECode SMSDispatcher::IsIms(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mImsSMSDispatcher != NULL) {
        return ISMSDispatcher::Probe(mImsSMSDispatcher)->IsIms(result);
    }
    else {
        Logger::E(TAG, "mImsSMSDispatcher is NULL");
        *result = FALSE;
    }
    return NOERROR;
}

ECode SMSDispatcher::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mImsSMSDispatcher != NULL) {
        return ISMSDispatcher::Probe(mImsSMSDispatcher)->GetImsSmsFormat(result);
    }
    else {
        Logger::E(TAG, "mImsSMSDispatcher is NULL");
        *result = String(NULL);
    }
    return NOERROR;
}

AutoPtr<IUri> SMSDispatcher::WriteOutboxMessage(Int64 subId, const String& address, const String& text,
        Boolean requireDeliveryReport, const String& creator)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(8, (IContentValues**)&values);
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    values->Put(ITelephonyTextBasedSmsColumns::PHONE_ID, phoneId);
    values->Put(ITelephonyTextBasedSmsColumns::ADDRESS, address);
    values->Put(ITelephonyTextBasedSmsColumns::BODY, text);
    Int64 currentTimeMillis;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTimeMillis);
    values->Put(ITelephonyTextBasedSmsColumns::DATE, currentTimeMillis); // milliseconds
    values->Put(ITelephonyTextBasedSmsColumns::SEEN, 1);
    values->Put(ITelephonyTextBasedSmsColumns::READ, 1);

    if (!TextUtils::IsEmpty(creator)) {
        values->Put(ITelephonyTextBasedSmsColumns::CREATOR, creator);
    }
    if (requireDeliveryReport) {
        values->Put(ITelephonyTextBasedSmsColumns::STATUS, ITelephonyTextBasedSmsColumns::STATUS_PENDING);
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ITelephonySmsOutbox> box;
    CTelephonySmsOutbox::AcquireSingleton((ITelephonySmsOutbox**)&box);
    AutoPtr<IUri> contentUri;
    box->GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<IUri> uri;
    cr->Insert(contentUri, values, (IUri**)&uri);
    // } Catch (Exception e) {
    //     Logger::E(TAG, "writeOutboxMessage: Failed to persist outbox message", e);
    //     return NULL;
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return uri;
}

void SMSDispatcher::MoveToOutbox(Int64 subId, IUri* messageUri, const String& creator)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(4, (IContentValues**)&values);
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    values->Put(ITelephonyTextBasedSmsColumns::PHONE_ID, phoneId);
    if (!TextUtils::IsEmpty(creator)) {
        // Reset creator/sender
        values->Put(ITelephonyTextBasedSmsColumns::CREATOR, creator);
    }
    // Reset the timestamp
    Int64 currentTimeMillis;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTimeMillis);
    values->Put(ITelephonyTextBasedSmsColumns::DATE, currentTimeMillis); // milliseconds
    values->Put(ITelephonyTextBasedSmsColumns::TYPE, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    cr->Update(messageUri, values,
                String(NULL)/*where*/, NULL/*selectionArgs*/, &val);
    if (val != 1) {
        // Logger::E(TAG, "moveToOutbox: failed to update message ", messageUri);
    }
    // } Catch (Exception e) {
    //     Logger::E(TAG, "moveToOutbox: Failed to update message", e);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
}

String SMSDispatcher::GetMultipartMessageText(IArrayList* parts)
{
    StringBuilder sb;
    Int32 size;
    parts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        parts->Get(i, (IInterface**)&obj);
        String part;
        ICharSequence::Probe(obj)->ToString(&part);
        if (!part.IsNull()) {
            sb.Append(part);
        }
    }

    String str;
    sb.ToString(&str);
    return str;
}

String SMSDispatcher::GetCarrierAppPackageName(IIntent* intent)
{
    AutoPtr<IUiccController> uc = UiccController::GetInstance();
    AutoPtr<IUiccCard> card;
    uc->GetUiccCard((IUiccCard**)&card);
    if (card == NULL) {
        return String(NULL);
    }

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> carrierPackages;
    card->GetCarrierPackageNamesForIntent(pm, intent, (IList**)&carrierPackages);

    Int32 size;
    if (carrierPackages != NULL && (carrierPackages->GetSize(&size), size) == 1) {
        AutoPtr<IInterface> obj;
        carrierPackages->Get(0, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        return str;
    }

    return String(NULL);
}

Int64 SMSDispatcher::GetSubId()
{
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
    Int64 val;
    sc->GetSubIdUsingPhoneId(((PhoneBase*)mPhone.Get())->mPhoneId, &val);
    return val;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

