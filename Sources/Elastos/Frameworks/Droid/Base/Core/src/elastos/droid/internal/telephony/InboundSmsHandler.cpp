
#include "elastos/droid/internal/telephony/InboundSmsHandler.h"
#include "elastos/droid/internal/telephony/InboundSmsTracker.h"
#include "elastos/droid/internal/telephony/CWapPushOverSms.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/internal/telephony/utility/BlacklistUtils.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/provider/CTelephonySmsIntents.h"
#include "elastos/droid/internal/utility/HexDump.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CUserHandle.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SmsManager.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/Manifest.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Internal::Telephony::Utility::BlacklistUtils;
using Elastos::Droid::Internal::Telephony::Utility::IBlacklistUtils;
using Elastos::Droid::Internal::Utility::HexDump;
using Elastos::Droid::Internal::Utility::IStateMachineLogRec;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Provider::CTelephonySmsIntents;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SmsManager;
using Elastos::Droid::Telephony::SmsMessage;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Manifest;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Boolean InboundSmsHandler::DBG = TRUE;
const Int32 InboundSmsHandler::PDU_COLUMN = 0;
const Int32 InboundSmsHandler::SEQUENCE_COLUMN = 1;
const Int32 InboundSmsHandler::DESTINATION_PORT_COLUMN = 2;
const Int32 InboundSmsHandler::DATE_COLUMN = 3;
const Int32 InboundSmsHandler::REFERENCE_NUMBER_COLUMN = 4;
const Int32 InboundSmsHandler::COUNT_COLUMN = 5;
const Int32 InboundSmsHandler::ADDRESS_COLUMN = 6;
const Int32 InboundSmsHandler::ID_COLUMN = 7;
const String InboundSmsHandler::SELECT_BY_ID("_id=?");
const String InboundSmsHandler::SELECT_BY_REFERENCE("address=? AND reference_number=? AND count=?");
/** Message type containing a {@link InboundSmsTracker} ready to broadcast to listeners. */
const Int32 InboundSmsHandler::EVENT_BROADCAST_SMS = 2;
/** Message from resultReceiver notifying {@link WaitingState} of a completed broadcast. */
const Int32 InboundSmsHandler::EVENT_BROADCAST_COMPLETE = 3;
/** Sent on exit from {@link WaitingState} to return to idle after sending all broadcasts. */
const Int32 InboundSmsHandler::EVENT_RETURN_TO_IDLE = 4;
/** Release wakelock after a short timeout when returning to idle state. */
const Int32 InboundSmsHandler::EVENT_RELEASE_WAKELOCK = 5;
/** Sent by {@link SmsBroadcastUndelivered} after cleaning the raw table. */
const Int32 InboundSmsHandler::EVENT_START_ACCEPTING_SMS = 6;
/** Update phone object */
const Int32 InboundSmsHandler::EVENT_UPDATE_PHONE_OBJECT = 7;
const Boolean InboundSmsHandler::VDBG = TRUE;
AutoPtr<ArrayOf<String> > InboundSmsHandler::PDU_PROJECTION = InitStatic(String("PDU_PROJECTION"));
AutoPtr<ArrayOf<String> > InboundSmsHandler::PDU_SEQUENCE_PORT_PROJECTION = InitStatic(String("PDU_SEQUENCE_PORT_PROJECTION"));
const Int32 InboundSmsHandler::WAKELOCK_TIMEOUT = 3000;
AutoPtr<IUri> InboundSmsHandler::sRawUri = InitRawUri();

InboundSmsHandler::DefaultState::DefaultState(
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{}

ECode InboundSmsHandler::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_UPDATE_PHONE_OBJECT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->OnUpdatePhoneObject(IPhoneBase::Probe(obj));
            break;
        }
        default: {
            String n;
            mHost->GetCurrentState()->GetName(&n);
            String errorText = String("processMessage: unhandled message type ")
                + StringUtils::ToString(what) + " currState=" + n;
            if (Build::IS_DEBUGGABLE) {
                mHost->Loge(String("---- Dumping InboundSmsHandler ----"));
                Int32 v = 0;
                mHost->GetLogRecCount(&v);
                mHost->Loge(String("Total records=") + StringUtils::ToString(v));
                mHost->GetLogRecSize(&v);
                Int32 i = Elastos::Core::Math::Max(v - 20, 0);
                for (; i < v; i++) {
                    String s;
                    AutoPtr<IStateMachineLogRec> sm;
                    mHost->GetLogRec(i, (IStateMachineLogRec**)&sm);
                    s.AppendFormat("Rec[%d]: %s\n", i, TO_CSTR(sm));
                    mHost->Loge(s);
                }
                mHost->Loge(String("---- Dumped InboundSmsHandler ----"));

                // throw new RuntimeException(errorText);
                assert(0 && errorText.string());
            }
            else {
                mHost->Loge(errorText);
            }
            break;
        }
    }
    *result = IState::HANDLED;
    return NOERROR;
}

InboundSmsHandler::StartupState::StartupState(
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{}

ECode InboundSmsHandler::StartupState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    mHost->Log(String("StartupState.processMessage:") + StringUtils::ToString(what));
    switch (what) {
        case EVENT_NEW_SMS:
        case EVENT_INJECT_SMS:
        case EVENT_BROADCAST_SMS: {
            mHost->DeferMessage(msg);
            *result = IState::HANDLED;
            return NOERROR;
        }

        case EVENT_START_ACCEPTING_SMS: {
            mHost->TransitionTo(mHost->mIdleState);
            *result = IState::HANDLED;
            return NOERROR;
        }

        case EVENT_BROADCAST_COMPLETE:
        case EVENT_RETURN_TO_IDLE:
        case EVENT_RELEASE_WAKELOCK:
        default: {
            // let DefaultState handle these unexpected message types
            *result = IState::NOT_HANDLED;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

InboundSmsHandler::IdleState::IdleState(
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{}

ECode InboundSmsHandler::IdleState::Enter()
{
    if (DBG) mHost->Log(String("entering Idle state"));
    mHost->SendMessageDelayed(EVENT_RELEASE_WAKELOCK, WAKELOCK_TIMEOUT);
    return NOERROR;
}

ECode InboundSmsHandler::IdleState::Exit()
{
    mHost->mWakeLock->AcquireLock();
    if (DBG) mHost->Log(String("acquired wakelock, leaving Idle state"));
    return NOERROR;
}

ECode InboundSmsHandler::IdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 what = 0;
    msg->GetWhat(&what);

    mHost->Log(String("IdleState.processMessage:") + StringUtils::ToString(what));
    if (DBG) {
        mHost->Log(String("Idle state processing message type ") + StringUtils::ToString(what));
    }

    switch (what) {
        case EVENT_NEW_SMS:
        case EVENT_INJECT_SMS:
        case EVENT_BROADCAST_SMS: {
            mHost->DeferMessage(msg);
            mHost->TransitionTo(mHost->mDeliveringState);
            *result = IState::HANDLED;
            return NOERROR;
        }
        case EVENT_RELEASE_WAKELOCK:
            mHost->mWakeLock->ReleaseLock();
            if (DBG) {
                Boolean tmp = FALSE;
                if (mHost->mWakeLock->IsHeld(&tmp), tmp) {
                    // this is okay as Int64 as we call release() for every acquire()
                    mHost->Log(String("mWakeLock is still held after release"));
                }
                else {
                    mHost->Log(String("mWakeLock released"));
                }
            }
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_RETURN_TO_IDLE:
            // already in idle state; ignore
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_BROADCAST_COMPLETE:
        case EVENT_START_ACCEPTING_SMS:
        default:
            // let DefaultState handle these unexpected message types
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

InboundSmsHandler::DeliveringState::DeliveringState(
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{}

ECode InboundSmsHandler::DeliveringState::Enter()
{
    if (DBG) mHost->Log(String("entering Delivering state"));
    return NOERROR;
}

ECode InboundSmsHandler::DeliveringState::Exit()
{
    if (DBG) mHost->Log(String("leaving Delivering state"));
    return NOERROR;
}

ECode InboundSmsHandler::DeliveringState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);

    mHost->Log(String("DeliveringState.processMessage, what:") + StringUtils::ToString(what));
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_NEW_SMS:
            // handle new SMS from RIL
            mHost->HandleNewSms((AsyncResult*)IAsyncResult::Probe(obj));
            mHost->SendMessage(EVENT_RETURN_TO_IDLE);
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_INJECT_SMS:
            // handle new injected SMS
            mHost->HandleInjectSms((AsyncResult*)IAsyncResult::Probe(obj));
            mHost->SendMessage(EVENT_RETURN_TO_IDLE);
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_BROADCAST_SMS:
            // if any broadcasts were sent, transition to waiting state
            if (mHost->ProcessMessagePart((InboundSmsTracker*)IObject::Probe(obj))) {
                mHost->TransitionTo(mHost->mWaitingState);
            }
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_RETURN_TO_IDLE:
            // return to idle after processing all other messages
            mHost->TransitionTo(mHost->mIdleState);
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_RELEASE_WAKELOCK: {
            mHost->mWakeLock->ReleaseLock();    // decrement wakelock from previous entry to Idle
            Boolean tmp = FALSE;
            if (mHost->mWakeLock->IsHeld(&tmp), !tmp) {
                // wakelock should still be held until 3 seconds after we enter Idle
                mHost->Loge(String("mWakeLock released while delivering/broadcasting!"));
            }
            *result = IState::HANDLED;
            return NOERROR;
        }

        // we shouldn't get this message type in this state, Log error and halt.
        case EVENT_BROADCAST_COMPLETE:
        case EVENT_START_ACCEPTING_SMS:
        default:
            // let DefaultState handle these unexpected message types
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }
}

InboundSmsHandler::WaitingState::WaitingState(
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{}

ECode InboundSmsHandler::WaitingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 what = 0;
    msg->GetWhat(&what);

    mHost->Log(String("WaitingState.processMessage:") + StringUtils::ToString(what));
    switch (what) {
        case EVENT_BROADCAST_SMS:
            // defer until the current broadcast completes
            mHost->DeferMessage(msg);
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_BROADCAST_COMPLETE:
            // return to idle after handling all deferred messages
            mHost->SendMessage(EVENT_RETURN_TO_IDLE);
            mHost->TransitionTo(mHost->mDeliveringState);
            *result = IState::HANDLED;
            return NOERROR;

        case EVENT_RETURN_TO_IDLE:
            // not ready to return to idle; ignore
            *result = IState::HANDLED;
            return NOERROR;

        default:
            // parent state handles the other message types
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }
}

InboundSmsHandler::SmsBroadcastReceiver::SmsBroadcastReceiver(
    /* [in] */ InboundSmsTracker* tracker,
    /* [in] */ InboundSmsHandler* host)
    : mHost(host)
{
    mDeleteWhere = tracker->GetDeleteWhere();
    mDeleteWhereArgs = tracker->GetDeleteWhereArgs();

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetNanoTime(&mBroadcastTimeNano);
    BroadcastReceiver::constructor();
}

ECode InboundSmsHandler::SmsBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DBG) {
        mHost->Log(String("SmsBroadcastReceiver::OnReceive action:") + action);
    }
    if (action.Equals(ITelephonySmsIntents::SMS_FILTER_ACTION)) {
        Int32 rc = 0;
        GetResultCode(&rc);
        Boolean tmp = FALSE;
        if (rc == IActivity::RESULT_OK) {
            // Overwrite pdus data if the SMS filter has set it.
            AutoPtr<IBundle> resultExtras;
            GetResultExtras(FALSE, (IBundle**)&resultExtras);
            if (resultExtras != NULL && (resultExtras->ContainsKey(String("pdus"), &tmp), tmp)) {
                AutoPtr<ArrayOf<Byte> > arrayBytes;
                resultExtras->GetByteArray(String("pdus"), (ArrayOf<Byte>**)&arrayBytes);
                intent->PutByteArrayExtra(String("pdus"), arrayBytes);
            }
            if (intent->HasExtra(String("destport"), &tmp), tmp) {
                Int32 destPort = 0;
                intent->GetInt32Extra(String("destport"), -1, &destPort);
                intent->RemoveExtra(String("destport"));
                mHost->SetAndDirectIntent(intent, destPort);
                if (SmsManager::GetDefault()->GetAutoPersisting(&tmp), tmp) {
                    AutoPtr<IUri> uri = mHost->WriteInboxMessage(intent);
                    if (uri != NULL) {
                        // Pass this to SMS apps so that they know where it is stored
                        intent->PutExtra(String("uri"), IParcelable::Probe(uri));
                    }
                }
                mHost->DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                               IAppOpsManager::OP_RECEIVE_SMS, this, UserHandle::OWNER);
            }
            else {
                mHost->Loge(String("destport doesn't exist in the extras for SMS filter action."));
            }
        }
        else {
            // Drop this SMS.
            mHost->Log(String("SMS filtered by result code ") + StringUtils::ToString(rc));
            mHost->DeleteFromRawTable(mDeleteWhere, mDeleteWhereArgs);
            mHost->SendMessage(EVENT_BROADCAST_COMPLETE);
        }
    }
    else if (action.Equals(ITelephonySmsIntents::SMS_DELIVER_ACTION)) {
        // Now dispatch the notification only intent
        intent->SetAction(ITelephonySmsIntents::SMS_RECEIVED_ACTION);
        intent->SetComponent(NULL);
        // All running users will be notified of the received sms.
        mHost->DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                IAppOpsManager::OP_RECEIVE_SMS, this, UserHandle::ALL);
    }
    else if (action.Equals(ITelephonySmsIntents::WAP_PUSH_DELIVER_ACTION)) {
        // Now dispatch the notification only intent
        intent->SetAction(ITelephonySmsIntents::WAP_PUSH_RECEIVED_ACTION);
        intent->SetComponent(NULL);
        // Only the primary user will receive notification of incoming mms.
        // That app will do the actual downloading of the mms.
        mHost->DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                IAppOpsManager::OP_RECEIVE_SMS, this, UserHandle::OWNER);
    }
    else {
        // Now that the intents have been deleted we can clean up the PDU data.
        if (!ITelephonySmsIntents::DATA_SMS_RECEIVED_ACTION.Equals(action)
                && !ITelephonySmsIntents::SMS_RECEIVED_ACTION.Equals(action)
                && !ITelephonySmsIntents::DATA_SMS_RECEIVED_ACTION.Equals(action)
                && !ITelephonySmsIntents::WAP_PUSH_RECEIVED_ACTION.Equals(action)) {
            mHost->Loge(String("unexpected BroadcastReceiver action: ") + action);
        }

        Int32 rc = 0;
        GetResultCode(&rc);
        if ((rc != IActivity::RESULT_OK) && (rc != ITelephonySmsIntents::RESULT_SMS_HANDLED)) {
            mHost->Loge(String("a broadcast receiver set the result code to ")
                    + StringUtils::ToString(rc)
                    + ", deleting from raw table anyway!");
        }
        else if (DBG) {
            mHost->Log(String("successful broadcast, deleting from raw table."));
        }

        mHost->DeleteFromRawTable(mDeleteWhere, mDeleteWhereArgs);
        mHost->SendMessage(EVENT_BROADCAST_COMPLETE);

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nn = 0;
        system->GetNanoTime(&nn);
        Int32 durationMillis = (Int32) ((nn - mBroadcastTimeNano) / 1000000);
        if (durationMillis >= 5000) {
            mHost->Loge(String("Slow ordered broadcast completion time: ")
                    + StringUtils::ToString(durationMillis) + " ms");
        }
        else if (DBG) {
            mHost->Log(String("ordered broadcast completed in: ")
                    + StringUtils::ToString(durationMillis) + " ms");
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(InboundSmsHandler, StateMachine, IInboundSmsHandler)
InboundSmsHandler::InboundSmsHandler(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ CellBroadcastHandler* cellBroadcastHandler)
{
    StateMachine::constructor(name);
    mDefaultState = new DefaultState(this);
    mStartupState = new StartupState(this);
    mIdleState = new IdleState(this);
    mDeliveringState = new DeliveringState(this);
    mWaitingState = new WaitingState(this);

    mContext = context;
    mStorageMonitor = storageMonitor;
    mPhone = phone;
    mCellBroadcastHandler = cellBroadcastHandler;
    context->GetContentResolver((IContentResolver**)&mResolver);
    CWapPushOverSms::New(context, (IWapPushOverSms**)&mWapPush);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean smsCapable = FALSE;
    res->GetBoolean(Elastos::Droid::R::bool_::config_sms_capable, &smsCapable);
    SystemProperties::GetBoolean(ITelephonyProperties::PROPERTY_SMS_RECEIVE
            , smsCapable, &mSmsReceiveDisabled);
    mSmsReceiveDisabled = !mSmsReceiveDisabled;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, name, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->AcquireLock();    // wake lock released after we enter idle state

    obj = NULL;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    AddState(mDefaultState);
    AddState(mStartupState, mDefaultState);
    AddState(mIdleState, mDefaultState);
    AddState(mDeliveringState, mDefaultState);
    AddState(mWaitingState, mDeliveringState);

    SetInitialState(mStartupState);
    if (DBG) Log(String("created InboundSmsHandler"));
}

/**
 * Tell the state machine to quit after processing all messages.
 */
ECode InboundSmsHandler::Dispose()
{
    Quit();
    return NOERROR;
}

/**
 * Update the phone object when it changes.
 */
ECode InboundSmsHandler::UpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    SendMessage(EVENT_UPDATE_PHONE_OBJECT, phone);
    return NOERROR;
}

/**
 * Dispose of the WAP push object and release the wakelock.
 */
// @Override
void InboundSmsHandler::OnQuitting()
{
    ((CWapPushOverSms*)mWapPush.Get())->Dispose();

    Boolean tmp = FALSE;
    while (mWakeLock->IsHeld(&tmp), tmp) {
        mWakeLock->ReleaseLock();
    }
}

// CAF_MSIM Is this used anywhere ? if not remove it
ECode InboundSmsHandler::GetPhone(
    /* [out] */ IPhoneBase** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPhone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void InboundSmsHandler::HandleNewSms(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mException != NULL) {
        Loge(String("Exception processing incoming SMS: ") + TO_CSTR(ar->mException));
        return;
    }

    Int32 result = 0, blacklistMatchType = -1;
    AutoPtr<ISmsMessage> sms;

    // try {
    sms = ISmsMessage::Probe(ar->mResult);
    if (FAILED(DispatchMessage(((SmsMessage*)sms.Get())->mWrappedSmsMessage, &result))) {
        Loge(String("Exception dispatching message"));
        result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    }
    // } catch (RuntimeException ex) {
    //     Loge("Exception dispatching message", ex);
    //     result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    // }

    // Translate (internal) blacklist check results to
    // RESULT_SMS_HANDLED + match type
    switch (result) {
        case ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_UNKNOWN:
            blacklistMatchType = IBlacklistUtils::MATCH_UNKNOWN;
            result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
            break;
        case ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_LIST:
            blacklistMatchType = IBlacklistUtils::MATCH_LIST;
            result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
            break;
        case ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_REGEX:
            blacklistMatchType = IBlacklistUtils::MATCH_REGEX;
            result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
            break;
    }


    // RESULT_OK means that the SMS will be acknowledged by special handling,
    // e.g. for SMS-PP data download. Any other result, we should ack here.
    if (result != IActivity::RESULT_OK) {
        Boolean handled = (result == ITelephonySmsIntents::RESULT_SMS_HANDLED); // 1
        NotifyAndAcknowledgeLastIncomingSms(handled, result, blacklistMatchType, sms, NULL);
    }
}

/**
 * This method is called when a new SMS PDU is injected into application framework.
 * @param ar is the AsyncResult that has the SMS PDU to be injected.
 */
void InboundSmsHandler::HandleInjectSms(
    /* [in] */ AsyncResult* ar)
{
    Int32 result;
    AutoPtr<IPendingIntent> receivedIntent;
    // try {
    receivedIntent = IPendingIntent::Probe(ar->mUserObj);
    AutoPtr<ISmsMessage> sms = ISmsMessage::Probe(ar->mResult);
    if (sms == NULL) {
        result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    }
    else {
        if (FAILED(DispatchMessage(((SmsMessage*)sms.Get())->mWrappedSmsMessage, &result))) {
            Loge(String("Exception dispatching message"));
            result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
        }
    }
    // } catch (RuntimeException ex) {
    //     Loge("Exception dispatching message");
    //     result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    // }

    if (receivedIntent != NULL) {
        // try {
        receivedIntent->Send(result);
        // } catch (CanceledException e) { }
    }
}

/**
 * Process an SMS message from the RIL, calling subclass methods to handle 3GPP and
 * 3GPP2-specific message types.
 *
 * @param smsb the SmsMessageBase object from the RIL
 * @return a result code from {@link android.provider.Telephony.Sms.Intents},
 *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
 */
ECode InboundSmsHandler::DispatchMessage(
    /* [in] */ ISmsMessageBase* smsb,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // If sms is NULL, there was a parsing error.
    if (smsb == NULL) {
        Loge(String("dispatchSmsMessage: message is NULL"));
        *result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
        return NOERROR;
    }

    if (mSmsReceiveDisabled) {
        // Device doesn't support receiving SMS,
        Log(String("Received short message on device which doesn't support receiving SMS. Ignored."));
        *result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
        return NOERROR;
    }

    *result = DispatchMessageRadioSpecific(smsb);
    return NOERROR;
}

/**
 * Called when the phone changes the default method updates mPhone
 * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
 * Override if different or other behavior is desired.
 *
 * @param phone
 */
void InboundSmsHandler::OnUpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    mPhone = phone;
    mStorageMonitor = ((PhoneBase*)mPhone.Get())->mSmsStorageMonitor;
    Logger::D("InboundSmsHandler", "TODO InboundSmsHandler::OnUpdatePhoneObject");
    //TODO  Log(String("onUpdatePhoneObject: phone=") + mPhone.getClass().getSimpleName());
}

/**
 * Notify interested apps if the framework has rejected an incoming SMS,
 * and send an acknowledge message to the network.
 * @param success indicates that last message was successfully received.
 * @param result result code indicating any error
 * @param blacklistMatchType blacklist type if the message was blacklisted,
 *                           -1 if it wasn't blacklisted
 * @param sms incoming SMS
 * @param response callback message sent when operation completes.
 */
void InboundSmsHandler::NotifyAndAcknowledgeLastIncomingSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 result,
    /* [in] */ Int32 blacklistMatchType,
    /* [in] */ ISmsMessage* sms,
    /* [in] */ IMessage* response)
{
    if (!success || blacklistMatchType >= 0) {
        // broadcast SMS_REJECTED_ACTION intent
        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonySmsIntents::SMS_REJECTED_ACTION, (IIntent**)&intent);
        intent->PutExtra(String("result"), result);
        intent->PutExtra(String("blacklisted"), blacklistMatchType >= 0);
        if (blacklistMatchType >= 0) {
            intent->PutExtra(String("blacklistMatchType"), blacklistMatchType);
        }
        if (sms != NULL) {
            String s;
            sms->GetOriginatingAddress(&s);
            intent->PutExtra(String("sender"), s);
            Int64 v = 0;
            sms->GetTimestampMillis(&v);
            intent->PutExtra(String("timestamp"), v);
        }
        if (DBG) Log(String("NotifyAndAcknowledgeLastIncomingSms(): reject intent= ") + TO_CSTR(intent));
        mContext->SendBroadcast(intent, Manifest::permission::RECEIVE_SMS);
    }
    AcknowledgeLastIncomingSms(success, result, response);
}

/**
 * Dispatch a normal incoming SMS. This is called from {@link #dispatchMessageRadioSpecific}
 * if no format-specific handling was required. Saves the PDU to the SMS provider raw table,
 * creates an {@link InboundSmsTracker}, then sends it to the state machine as an
 * {@link #EVENT_BROADCAST_SMS}. Returns {@link Intents#RESULT_SMS_HANDLED} or an error value.
 *
 * @param sms the message to dispatch
 * @return {@link Intents#RESULT_SMS_HANDLED} if the message was accepted, or an error status
 */
Int32 InboundSmsHandler::DispatchNormalMessage(
    /* [in] */ ISmsMessageBase* sms)
{
    Int32 blacklistResult = CheckIfBlacklisted(sms);
    if (blacklistResult != ITelephonySmsIntents::RESULT_SMS_HANDLED) {
        return blacklistResult;
    }

    AutoPtr<ISmsHeader> ismsHeader;
    sms->GetUserDataHeader((ISmsHeader**)&ismsHeader);
    SmsHeader* smsHeader = (SmsHeader*)(IObject::Probe(ismsHeader));
    AutoPtr<InboundSmsTracker> tracker;

    if ((smsHeader == NULL) || (smsHeader->mConcatRef == NULL)) {
        // Message is not concatenated.
        Int32 destPort = -1;
        if (smsHeader != NULL && smsHeader->mPortAddrs != NULL) {
            // The message was sent to a port.
            destPort = smsHeader->mPortAddrs->destPort;
            if (DBG) Log(String("destination port: ") + StringUtils::ToString(destPort));
        }

        AutoPtr<ArrayOf<Byte> > pdu;
        sms->GetPdu((ArrayOf<Byte>**)&pdu);
        Int64 tsm;
        sms->GetTimestampMillis(&tsm);
        tracker = new InboundSmsTracker(pdu, tsm, destPort, Is3gpp2(), FALSE);
    }
    else {
        // Create a tracker for this message segment.
        AutoPtr<SmsHeader::ConcatRef> concatRef = smsHeader->mConcatRef;
        AutoPtr<SmsHeader::PortAddrs> portAddrs = smsHeader->mPortAddrs;
        Int32 destPort = (portAddrs != NULL ? portAddrs->destPort : -1);

        AutoPtr<ArrayOf<Byte> > pdu;
        sms->GetPdu((ArrayOf<Byte>**)&pdu);
        Int64 tsm;
        sms->GetTimestampMillis(&tsm);
        String oriAddress;
        sms->GetOriginatingAddress(&oriAddress);
        tracker = new InboundSmsTracker(pdu, tsm, destPort,
                Is3gpp2(), oriAddress, concatRef->refNumber,
                concatRef->seqNumber, concatRef->msgCount, FALSE);
    }

    if (VDBG) Log(String("created tracker: ") + TO_CSTR(tracker));
    return AddTrackerToRawTableAndSendMessage(tracker);
}

Int32 InboundSmsHandler::CheckIfBlacklisted(
    /* [in] */ ISmsMessageBase* sms)
{
    String s;
    sms->GetOriginatingAddress(&s);
    Int32 result = BlacklistUtils::IsListed(mContext,
            s, IBlacklistUtils::BLOCK_MESSAGES);

    switch (result) {
        case IBlacklistUtils::MATCH_UNKNOWN:
            return ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_UNKNOWN;
        case IBlacklistUtils::MATCH_LIST:
            return ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_LIST;
        case IBlacklistUtils::MATCH_REGEX:
            return ITelephonySmsIntents::RESULT_SMS_BLACKLISTED_REGEX;
    }

    return ITelephonySmsIntents::RESULT_SMS_HANDLED;
}

/**
 * Helper to add the tracker to the raw table and then send a message to broadcast it, if
 * successful. Returns the SMS intent status to return to the SMSC.
 * @param tracker the tracker to save to the raw table and then deliver
 * @return {@link Intents#RESULT_SMS_HANDLED} or {@link Intents#RESULT_SMS_GENERIC_ERROR}
 * or {@link Intents#RESULT_SMS_DUPLICATED}
 */
Int32 InboundSmsHandler::AddTrackerToRawTableAndSendMessage(
    /* [in] */ InboundSmsTracker* tracker)
{
    switch(AddTrackerToRawTable(tracker)) {
    case ITelephonySmsIntents::RESULT_SMS_HANDLED:
        SendMessage(EVENT_BROADCAST_SMS, tracker->Probe(EIID_IInterface));
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;

    case ITelephonySmsIntents::RESULT_SMS_DUPLICATED:
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;

    case ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR:
    default:
        return ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    }
}

/**
 * Process the inbound SMS segment. If the message is complete, send it as an ordered
 * broadcast to interested receivers and return TRUE. If the message is a segment of an
 * incomplete multi-part SMS, return FALSE.
 * @param tracker the tracker containing the message segment to process
 * @return TRUE if an ordered broadcast was sent; FALSE if waiting for more message segments
 */
Boolean InboundSmsHandler::ProcessMessagePart(
    /* [in] */ InboundSmsTracker* tracker)
{
    Int32 messageCount = tracker->GetMessageCount();
    AutoPtr<ArrayOf<ArrayOf<Byte>* > > pdus;
    Int32 destPort = tracker->GetDestPort();
    String address("");

    if (messageCount == 1) {
        // single-part message
        AutoPtr<ArrayOf<Byte> > pdu = tracker->GetPdu();
        pdus = ArrayOf<ArrayOf<Byte>* >::Alloc(1);
        pdus->Set(0, pdu);
        // pdus = new Byte[][]{tracker->GetPdu()};
    }
    else {
        // multi-part message
        AutoPtr<ICursor> cursor;
        // try {
        // used by several query selection arguments
        address = tracker->GetAddress();
        String refNumber = StringUtils::ToString(tracker->GetReferenceNumber());
        String count = StringUtils::ToString(tracker->GetMessageCount());

        // query for all segments and broadcast message if we have all the parts
        AutoPtr<ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(3);
        (*whereArgs)[0] = address;
        (*whereArgs)[1] = refNumber;
        (*whereArgs)[2] = count;
        mResolver->Query(sRawUri, PDU_SEQUENCE_PORT_PROJECTION,
                SELECT_BY_REFERENCE, whereArgs, String(NULL), (ICursor**)&cursor);

        Int32 cursorCount = 0;
        cursor->GetCount(&cursorCount);
        if (cursorCount < messageCount) {
            // Wait for the other message parts to arrive. It's also possible for the last
            // segment to arrive before processing the EVENT_BROADCAST_SMS for one of the
            // earlier segments. In that case, the broadcast will be sent as soon as all
            // segments are in the table, and any later EVENT_BROADCAST_SMS messages will
            // get a row count of 0 and return.
            return FALSE;
        }

        // All the parts are in place, deal with them
        pdus = ArrayOf<ArrayOf<Byte>* >::Alloc(messageCount);
        // pdus = new Byte[messageCount][];
        Boolean next = FALSE, tmp = FALSE;
        Int32 v = 0;
        String ss;
        while (cursor->MoveToNext(&next), next) {
            // subtract offset to convert sequence to 0-based array index
            ASSERT_SUCCEEDED(cursor->GetInt32(SEQUENCE_COLUMN, &v));
            Int32 index = v - tracker->GetIndexOffset();

            ASSERT_SUCCEEDED(cursor->GetString(PDU_COLUMN, &ss));
            pdus->Set(index, HexDump::HexStringToByteArray(ss));

            // Read the destination port from the first segment (needed for CDMA WAP PDU).
            // It's not a bad idea to prefer the port from the first segment in other cases.
            ASSERT_SUCCEEDED(cursor->IsNull(DESTINATION_PORT_COLUMN, &tmp));
            if (index == 0 && !tmp) {
                Int32 port = 0;
                ASSERT_SUCCEEDED(cursor->GetInt32(DESTINATION_PORT_COLUMN, &port));
                // strip format flags and convert to real port number, or -1
                port = InboundSmsTracker::GetRealDestPort(port);
                if (port != -1) {
                    destPort = port;
                }
            }
        }
        // } catch (SQLException e) {
        //     if (cursor != NULL) {
        //         cursor->Close();
        //     }
        //     Loge(String("Can't access multipart SMS database"));
        //     return FALSE;
        // } finally {
        //     if (cursor != NULL) {
        //         cursor->Close();
        //     }
        // }

        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
    }

    AutoPtr<IBroadcastReceiver> resultReceiver = new SmsBroadcastReceiver(tracker, this);

    if (destPort == ISmsHeader::PORT_WAP_PUSH) {
        // Build up the data stream
        AutoPtr<IByteArrayOutputStream> output;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&output);
        for (Int32 i = 0; i < pdus->GetLength(); i++) {
            AutoPtr<ArrayOf<Byte> > pdu = (*pdus)[0];
            // 3GPP needs to extract the User Data from the PDU; 3GPP2 has already done this
            if (!tracker->Is3gpp2()) {
                AutoPtr<ISmsMessage> msg = SmsMessage::CreateFromPdu(pdu, ISmsConstants::FORMAT_3GPP);
                msg->GetUserData((ArrayOf<Byte>**)&pdu);
                if (address == "") {
                   msg->GetOriginatingAddress(&address);
                }
                else if(address == ""){
                   address = tracker->GetAddress();
                }
            }
            IOutputStream::Probe(output)->Write(pdu, 0, pdu->GetLength());
        }

        AutoPtr<ArrayOf<Byte> > bs;
        output->ToByteArray((ArrayOf<Byte>**)&bs);
        Int32 result = 0;
        if (mWapPush != NULL) {
            mWapPush->DispatchWapPdu(bs, resultReceiver, this, address, &result);
        }
        if (DBG) Log(String("dispatchWapPdu() returned ") + StringUtils::ToString(result));
        // result is IActivity::RESULT_OK if an ordered broadcast was sent
        return (result == IActivity::RESULT_OK);
    }

    AutoPtr<IIntent> intent;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IList> regAddresses;    /*List<String*/
    Settings::Secure::GetDelimitedStringAsList(cr, ISettingsSecure::PROTECTED_SMS_ADDRESSES
            , String("|"), (IList**)&regAddresses);

    AutoPtr<IList> allAddresses;    /*List<String*/
    Elastos::Droid::Provider::Telephony::Sms::Intents::GetNormalizedAddressesFromPdus(pdus, tracker->GetFormat(), (IList**)&allAddresses);

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Boolean tmp = FALSE;
    if (collections->Disjoint(ICollection::Probe(regAddresses), ICollection::Probe(allAddresses), &tmp), !tmp) {
        CIntent::New(ITelephonySmsIntents::PROTECTED_SMS_RECEIVED_ACTION, (IIntent**)&intent);
        //NOTE: here we will convert the 2-D arrayof to 1-D arrayof
        //      every SMS will not more than 140 bytes,
        //      so Byte type will be ok for the length of the SMS
        //      below show how the convert work
        //      a 2-D arrayof have 3 element:
        //         XXXXXXXX|YYYYYYYYYYYY|ZZZZZZ
        //      convert to 1-D arrayof
        //         3 8 XXXXXXXX 12 YYYYYYYYYYYY 6 ZZZZZZ
        //      3 parts: the 1st part have 8 bytes, 2nd have 12 bytes, 3rd have 6 bytes
        Int32 arrayLen = pdus->GetLength();
        Int32 allByteLen = 0;
        for (Int32 i = 0; i < arrayLen; i++) {
            AutoPtr<ArrayOf<Byte> > ab = (*pdus)[i];
            Int32 len = ab->GetLength();
            allByteLen += len;
            //TODO debug log should remove
            Logger::E("leliang:InboundSmsHandler", "index: %d, len:%d, line:%d", i, len, __LINE__);
            for (Int32 j = 0; j < len; ++j) {
                Logger::E("leliang:InboundSmsHandler", "    %d value: 0x%x", j, (*ab)[j]);
            }
        }
        AutoPtr<ArrayOf<Byte> > arrayBytes = ArrayOf<Byte>::Alloc(1 + arrayLen + allByteLen);
        Int32 pos = 0;
        arrayBytes->Set(pos++, (Byte)arrayLen);
        for (Int32 i = 0; i < arrayLen; i++) {
            AutoPtr<ArrayOf<Byte> > aob = (*pdus)[i];
            Int32 len = aob->GetLength();
            if (len > 164) {
                // in fact the chinese version is 164 bytes
                Logger::E("InboundSmsHandler", "maybe wrong length of the SMS, should never be over 164, len:%d", len);
                //assert(0);
            }
            arrayBytes->Set(pos++, (Byte)len);
            memcpy(arrayBytes->GetPayload() + pos, aob->GetPayload(), len);
            pos += len;
        }
        intent->PutByteArrayExtra(String("pdus"), arrayBytes);// GetByteArrayExtra

        intent->PutExtra(String("format"), tracker->GetFormat());
        DispatchIntent(intent, Manifest::permission::RECEIVE_PROTECTED_SMS,
                IAppOpsManager::OP_RECEIVE_SMS, resultReceiver, UserHandle::OWNER);
        return TRUE;
    }

    CIntent::New(ITelephonySmsIntents::SMS_FILTER_ACTION, (IIntent**)&intent);
    AutoPtr<IList> carrierPackages; // List<<String>
    AutoPtr<IUiccCard> card;
    UiccController::GetInstance()->GetUiccCard((IUiccCard**)&card);
    if (card != NULL) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        card->GetCarrierPackageNamesForIntent(
                pm, intent, (IList**)&carrierPackages);
    }
    Int32 v = 0;
    if (carrierPackages != NULL && (carrierPackages->GetSize(&v), v) == 1) {
        AutoPtr<IInterface> obj;
        carrierPackages->Get(0, (IInterface**)&obj);
        String s;
        ICharSequence::Probe(obj)->ToString(&s);
        intent->SetPackage(s);
        intent->PutExtra(String("destport"), destPort);
    }
    else {
        SetAndDirectIntent(intent, destPort);
    }

    //NOTE: here we will convert the 2-D arrayof to 1-D arrayof
    //      every SMS will not more than 140 bytes,
    //      so Byte type will be ok for the length of the SMS
    //      below show how the convert work
    //      a 2-D arrayof have 3 element:
    //         XXXXXXXX|YYYYYYYYYYYY|ZZZZZZ
    //      convert to 1-D arrayof
    //         3 8 XXXXXXXX 12 YYYYYYYYYYYY 6 ZZZZZZ
    //      3 part: the 1st part have 8 bytes, 2nd have 12 bytes, 3rd have 6 bytes
    Int32 arrayLen = pdus->GetLength();
    Int32 allByteLen = 0;
    for (Int32 i = 0; i < arrayLen; i++) {
        AutoPtr<ArrayOf<Byte> > ab = (*pdus)[i];
        Int32 len = ab->GetLength();
        allByteLen += len;
        //TODO debug log should remove
        //Logger::E("leliang:InboundSmsHandler", "index: %d, len:%d, line:%d", i, len, __LINE__);
        //for (Int32 j = 0; j < len; ++j) {
        //    Logger::E("leliang:InboundSmsHandler", "    %d value: 0x%x", j, (*ab)[j]);
        //}
    }
    AutoPtr<ArrayOf<Byte> > arrayBytes = ArrayOf<Byte>::Alloc(1 + arrayLen + allByteLen);
    Int32 pos = 0;
    arrayBytes->Set(pos++, (Byte)arrayLen);
    for (Int32 i = 0; i < arrayLen; i++) {
        AutoPtr<ArrayOf<Byte> > aob = (*pdus)[i];
        Int32 len = aob->GetLength();
        if (len > 164) {
            // in fact the chinese version is 164 bytes
            Logger::E("InboundSmsHandler", "maybe wrong length of the SMS, should never be over 164, len:%d", len);
            //assert(0);
        }
        arrayBytes->Set(pos++, (Byte)len);
        memcpy(arrayBytes->GetPayload() + pos, aob->GetPayload(), len);
        pos += len;
    }
    intent->PutByteArrayExtra(String("pdus"), arrayBytes);// GetByteArrayExtra

    intent->PutExtra(String("format"), tracker->GetFormat());
    DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
            IAppOpsManager::OP_RECEIVE_SMS, resultReceiver, UserHandle::OWNER);
    return TRUE;
}

/**
 * Dispatch the intent with the specified permission, appOp, and result receiver, using
 * this state machine's handler thread to run the result receiver.
 *
 * @param intent the intent to broadcast
 * @param permission receivers are required to have this permission
 * @param appOp app op that is being performed when dispatching to a receiver
 * @param user user to deliver the intent to
 */
void InboundSmsHandler::DispatchIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& permission,
    /* [in] */ Int32 appOp,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IUserHandle* user)
{
    intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
    Int32 id = 0;
    IPhone::Probe(mPhone)->GetPhoneId(&id);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, id);
    Boolean tmp = FALSE;
    if (user->Equals(UserHandle::ALL, &tmp), tmp) {
        // Get a list of currently started users.
        AutoPtr<ArrayOf<Int32> > users;
        // try {
        ActivityManagerNative::GetDefault()->GetRunningUserIds((ArrayOf<Int32>**)&users);
        // } catch (RemoteException re) {
        // }
        if (users == NULL) {
            user->GetIdentifier(&id);
            users = ArrayOf<Int32>::Alloc(1);
            (*users)[0] = id;
        }
        // Deliver the broadcast only to those running users that are permitted
        // by user policy.
        for (Int32 i = users->GetLength() - 1; i >= 0; i--) {
            AutoPtr<IUserHandle> targetUser;
            CUserHandle::New((*users)[i], (IUserHandle**)&targetUser);
            if ((*users)[i] != UserHandle::USER_OWNER) {
                // Is the user not allowed to use SMS?
                if (mUserManager->HasUserRestriction(IUserManager::DISALLOW_SMS, targetUser, &tmp), tmp) {
                    continue;
                }
                // Skip unknown users and managed profiles as well
                AutoPtr<IUserInfo> info;
                mUserManager->GetUserInfo((*users)[i], (IUserInfo**)&info);
                if (info == NULL || (info->IsManagedProfile(&tmp), tmp)) {
                    continue;
                }
            }
            // Only pass in the resultReceiver when the USER_OWNER is processed.
            AutoPtr<IHandler> handler;
            GetHandler((IHandler**)&handler);
            mContext->SendOrderedBroadcastAsUser(intent, targetUser, permission, appOp,
                    (*users)[i] == IUserHandle::USER_OWNER ? resultReceiver : NULL,
                    handler, IActivity::RESULT_OK, String(NULL), NULL);
        }
    }
    else {
        AutoPtr<IHandler> handler;
        GetHandler((IHandler**)&handler);
        mContext->SendOrderedBroadcastAsUser(intent, user, permission, appOp,
                resultReceiver, handler, IActivity::RESULT_OK, String(NULL), NULL);
    }
}

/**
 * Helper for {@link SmsBroadcastUndelivered} to delete an old message in the raw table.
 */
void InboundSmsHandler::DeleteFromRawTable(
    /* [in] */ const String& deleteWhere,
    /* [in] */ ArrayOf<String>* deleteWhereArgs)
{
    Int32 rows = 0;
    mResolver->Delete(sRawUri, deleteWhere, deleteWhereArgs, &rows);
    if (rows == 0) {
        Loge(String("No rows were deleted from raw table!"));
    }
    else if (DBG) {
        Log(String("Deleted ") + StringUtils::ToString(rows) + " rows from raw table.");
    }
}

/**
 * Set the appropriate intent action and direct the intent to the default SMS app or the
 * appropriate port.
 *
 * @param intent the intent to set and direct
 * @param destPort the destination port
 */
void InboundSmsHandler::SetAndDirectIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 destPort)
{
    if (destPort == -1) {
        intent->SetAction(ITelephonySmsIntents::SMS_DELIVER_ACTION);

        // Direct the intent to only the default SMS app. If we can't find a default SMS app
        // then sent it to all broadcast receivers.
        // We are deliberately delivering to the primary user's default SMS App.
        AutoPtr<IComponentName> componentName;
        AutoPtr<ISmsApplication> smsApp;
        CSmsApplication::AcquireSingleton((ISmsApplication**)&smsApp);
        smsApp->GetDefaultSmsApplication(mContext, TRUE, (IComponentName**)&componentName);
        if (componentName != NULL) {
            // Deliver SMS message only to this receiver.
            intent->SetComponent(componentName);
            String pn, cn;
            componentName->GetPackageName(&pn);
            componentName->GetClassName(&cn);
            Log(String("Delivering SMS to: ") + pn + " " + cn);
        }
        else {
            intent->SetComponent(NULL);
        }
    }
    else {
        intent->SetAction(ITelephonySmsIntents::DATA_SMS_RECEIVED_ACTION);
        AutoPtr<IUri> uri;
        Uri::Parse(String("sms://localhost:") + StringUtils::ToString(destPort), (IUri**)&uri);
        intent->SetData(uri);
        intent->SetComponent(NULL);
    }
}

/**
 * Insert a message PDU into the raw table so we can acknowledge it immediately.
 * If the device crashes before the broadcast to listeners completes, it will be delivered
 * from the raw table on the next device boot. For single-part messages, the deleteWhere
 * and deleteWhereArgs fields of the tracker will be set to delete the correct row after
 * the ordered broadcast completes.
 *
 * @param tracker the tracker to add to the raw table
 * @return TRUE on success; FALSE on failure to write to database
 */
Int32 InboundSmsHandler::AddTrackerToRawTable(
    /* [in] */ InboundSmsTracker* tracker)
{
    if (tracker->GetMessageCount() != 1) {
        // check for duplicate message segments
        AutoPtr<ICursor> cursor;
        ECode ec = NOERROR;
        do {
            // sequence numbers are 1-based except for CDMA WAP, which is 0-based
            Int32 sequence = tracker->GetSequenceNumber();

            // convert to strings for query
            String address = tracker->GetAddress();
            String refNumber = StringUtils::ToString(tracker->GetReferenceNumber());
            String count = StringUtils::ToString(tracker->GetMessageCount());

            String seqNumber = StringUtils::ToString(sequence);

            // set the delete selection args for multi-part message
            AutoPtr<ArrayOf<String> > deleteWhereArgs = ArrayOf<String>::Alloc(3);
            (*deleteWhereArgs)[0] = address;
            (*deleteWhereArgs)[1] = refNumber;
            (*deleteWhereArgs)[2] = count;
            tracker->SetDeleteWhere(SELECT_BY_REFERENCE, deleteWhereArgs);

            // Check for duplicate message segments
            AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(4);
            (*vs)[0] = address;
            (*vs)[1] = refNumber;
            (*vs)[2] = count;
            (*vs)[3] = seqNumber;
            ec = mResolver->Query(sRawUri, PDU_PROJECTION,
                    String("address=? AND reference_number=? AND count=? AND sequence=?"),
                    vs, String(NULL), (ICursor**)&cursor);
            if (FAILED(ec)) {
                break;
            }

            // moveToNext() returns FALSE if no duplicates were found
            Boolean tmp = FALSE;
            ec = cursor->MoveToNext(&tmp);
            if (FAILED(ec)) {
                break;
            }

            if (tmp) {
                Loge(String("Discarding duplicate message segment, refNumber=") + refNumber
                        + " seqNumber=" + seqNumber);
                String oldPduString;
                ec = cursor->GetString(PDU_COLUMN, &oldPduString);
                if (FAILED(ec)) {
                    break;
                }
                AutoPtr<ArrayOf<Byte> > pdu = tracker->GetPdu();
                AutoPtr<ArrayOf<Byte> > oldPdu = HexDump::HexStringToByteArray(oldPduString);
                if (!Arrays::Equals(oldPdu, tracker->GetPdu())) {
                    Loge(String("Warning: dup message segment PDU of length ")
                        + StringUtils::ToString(pdu->GetLength())
                        + " is different from existing PDU of length "
                        + StringUtils::ToString(oldPdu->GetLength()));
                }

                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
                return ITelephonySmsIntents::RESULT_SMS_DUPLICATED;   // reject message
            }

            Logger::D("InboundSmsHandler", "Close twice?????");
            ICloseable::Probe(cursor)->Close();
        } while (0);

        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }

        if (FAILED(ec)) {
            Loge(String("Can't access multipart SMS database"));
            return ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;    // reject message
        }
    }

    AutoPtr<IContentValues> values = tracker->GetContentValues();

    if (VDBG) Log(String("adding content values to raw table: ") + TO_CSTR(values));
    AutoPtr<IUri> newUri;
    mResolver->Insert(sRawUri, values, (IUri**)&newUri);
    if (DBG) Log(String("URI of new row -> "));
    if (DBG) Logger::D("InboundSmsHandler", "newUri:%s", TO_CSTR(newUri));

    // try {
    AutoPtr<IContentUris> cUris;
    CContentUris::AcquireSingleton((IContentUris**)&cUris);
    Int64 rowId = 0;
    if (FAILED(cUris->ParseId(newUri, &rowId))) {
        Loge(String("error parsing URI for new row: "));
        Logger::D("InboundSmsHandler", " newUri :%s", TO_CSTR(newUri));
        return ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    }
    if (tracker->GetMessageCount() == 1) {
        // set the delete selection args for single-part message
        AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(1);
        (*vs)[0] = StringUtils::ToString(rowId);
        tracker->SetDeleteWhere(SELECT_BY_ID, vs);
    }
    return ITelephonySmsIntents::RESULT_SMS_HANDLED;
    // } catch (Exception e) {
    //     Loge(String("error parsing URI for new row: ") + TO_CSTR(newUri));
    //     return ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
    // }
}

/**
 * Returns whether the default message format for the current radio technology is 3GPP2.
 * @return TRUE if the radio technology uses 3GPP2 format by default, FALSE for 3GPP format
 */
Boolean InboundSmsHandler::IsCurrentFormat3gpp2()
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone = 0;
    tm->GetCurrentPhoneType(&activePhone);
    return (ITelephonyManager::PHONE_TYPE_CDMA == activePhone);
}

void InboundSmsHandler::StoreVoiceMailCount()
{
    // Store the voice mail count in persistent memory.
    String imsi;
    IPhone::Probe(mPhone)->GetSubscriberId(&imsi);
    Int32 mwi = 0;
    IPhone::Probe(mPhone)->GetVoiceMessageCount(&mwi);

    Int64 id = 0;
    IPhone::Probe(mPhone)->GetSubId(&id);
    Log(String("Storing Voice Mail Count = ") + StringUtils::ToString(mwi)
                + " for mVmCountKey = " + IPhoneBase::VM_COUNT
                + " vmId = " + IPhoneBase::VM_ID
                + " subId = "+ StringUtils::ToString(id)
                + " in preferences.");

    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt32(IPhoneBase::VM_COUNT + StringUtils::ToString(id), mwi);
    editor->PutString(IPhoneBase::VM_ID + StringUtils::ToString(id), imsi);
    Boolean tmp = FALSE;
    editor->Commit(&tmp);
}

/**
 * Log with debug level.
 * @param s the string to Log
 */
// @Override
void InboundSmsHandler::Log(
    /* [in] */ const String& s)
{
    Logger::D(GetName(), s);
}

/**
 * Log with error level.
 * @param s the string to Log
 */
// @Override
void InboundSmsHandler::Loge(
    /* [in] */ const String& s)
{
    Logger::E(GetName(), s);
}

/**
 * Log with error level.
 * @param s the string to Log
 * @param e is a Throwable which logs additional information.
 */
// @Override
void InboundSmsHandler::Loge(
    /* [in] */ const String& s,
    /* [in] */ IThrowable* e)
{
    Logger::E(GetName(), "%s %s", s.string(), TO_CSTR(e));
}

String InboundSmsHandler::GetName()
{
    String result;
    StateMachine::GetName(&result);
    return result;
}
/**
 * Store a received SMS into Telephony provider
 *
 * @param intent The intent containing the received SMS
 * @return The URI of written message
 */
AutoPtr<IUri> InboundSmsHandler::WriteInboxMessage(
    /* [in] */ IIntent* intent)
{
    AutoPtr<ArrayOf<ISmsMessage*> > messages;
    AutoPtr<ITelephonySmsIntents> tsi;
    CTelephonySmsIntents::AcquireSingleton((ITelephonySmsIntents**)&tsi);
    tsi->GetMessagesFromIntent(intent, (ArrayOf<ISmsMessage*>**)&messages);
    if (messages == NULL || messages->GetLength() < 1) {
        Loge(String("Failed to parse SMS pdu"));
        return NULL;
    }
    // Sometimes, SmsMessage.mWrappedSmsMessage is NULL causing NPE when we access
    // the methods on it although the SmsMessage itself is not NULL. So do this check
    // before we do anything on the parsed SmsMessages.
    String v;
    for (Int32 i = 0; i < messages->GetLength(); i++) {
        AutoPtr<ISmsMessage> sms = (*messages)[i];
        if (FAILED(sms->GetDisplayMessageBody(&v))) {
            Loge(String("NPE inside SmsMessage"));
            return NULL;
        }
    }
    AutoPtr<IContentValues> values = ParseSmsMessage(messages);
    const Int64 identity = Binder::ClearCallingIdentity();
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    if (FAILED(cr->Insert(Elastos::Droid::Provider::Telephony::Sms::Inbox::CONTENT_URI, values, (IUri**)&uri))) {
        Loge(String("Failed to persist inbox message"));
        Binder::RestoreCallingIdentity(identity);
        return NULL;
    }
    Binder::RestoreCallingIdentity(identity);
    return uri;
}

/**
 * Convert SmsMessage[] into SMS database schema columns
 *
 * @param msgs The SmsMessage array of the received SMS
 * @return ContentValues representing the columns of parsed SMS
 */
AutoPtr<IContentValues> InboundSmsHandler::ParseSmsMessage(
    /* [in] */ ArrayOf<ISmsMessage*>* msgs)
{
    AutoPtr<ISmsMessage> sms = (*msgs)[0];
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    String s;
    sms->GetDisplayOriginatingAddress(&s);
    values->Put(ITelephonyTextBasedSmsColumns::ADDRESS, s);
    values->Put(ITelephonyTextBasedSmsColumns::BODY, BuildMessageBodyFromPdus(msgs));
    Int64 v = 0;
    sms->GetTimestampMillis(&v);
    values->Put(ITelephonyTextBasedSmsColumns::DATE_SENT, v);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&v);
    values->Put(ITelephonyTextBasedSmsColumns::DATE, v);
    Int32 iv = 0;
    sms->GetProtocolIdentifier(&iv);
    values->Put(ITelephonyTextBasedSmsColumns::PROTOCOL, iv);
    values->Put(ITelephonyTextBasedSmsColumns::SEEN, 0);
    values->Put(ITelephonyTextBasedSmsColumns::READ, 0);
    String subject;
    sms->GetPseudoSubject(&subject);
    if (!TextUtils::IsEmpty(subject)) {
        values->Put(ITelephonyTextBasedSmsColumns::SUBJECT, subject);
    }
    Boolean tmp = FALSE;
    sms->IsReplyPathPresent(&tmp);
    values->Put(ITelephonyTextBasedSmsColumns::REPLY_PATH_PRESENT, tmp ? 1 : 0);
    sms->GetServiceCenterAddress(&s);
    values->Put(ITelephonyTextBasedSmsColumns::SERVICE_CENTER, s);
    return values;
}

/**
 * Build up the SMS message body from the SmsMessage array of received SMS
 *
 * @param msgs The SmsMessage array of the received SMS
 * @return The text message body
 */
String InboundSmsHandler::BuildMessageBodyFromPdus(
    /* [in] */ ArrayOf<ISmsMessage*>* msgs)
{
    if (msgs->GetLength() == 1) {
        // There is only one part, so grab the body directly.
        String s;
        (*msgs)[0]->GetDisplayMessageBody(&s);
        return ReplaceFormFeeds(s);
    }
    else {
        // Build up the body from the parts.
        StringBuilder body;
        for (Int32 i = 0; i < msgs->GetLength(); i++) {
            AutoPtr<ISmsMessage> msg = (*msgs)[i];
            // getDisplayMessageBody() can NPE if mWrappedMessage inside is NULL.
            String s;
            msg->GetDisplayMessageBody(&s);
            body.Append(s);
        }
        return ReplaceFormFeeds(body.ToString());
    }
}

// Some providers send formfeeds in their messages. Convert those formfeeds to newlines.
String InboundSmsHandler::ReplaceFormFeeds(
    /* [in] */ const String& s)
{
    return s == NULL ? String("") : s.Replace('\f', '\n');
}

AutoPtr<ArrayOf<String> > InboundSmsHandler::InitStatic(
    /* [in] */ const String& flag)
{
    AutoPtr<ArrayOf<String> > values;
    if (flag.Equals("PDU_PROJECTION")) {
        values = ArrayOf<String>::Alloc(1);
        (*values)[0] = String("pdu");
    }
    else if (flag.Equals("PDU_SEQUENCE_PORT_PROJECTION")) {
        values = ArrayOf<String>::Alloc(3);
        (*values)[0] = String("pdu");
        (*values)[1] = String("sequence");
        (*values)[2] = String("destination_port");
    }
    else {
        assert(0 && "TODO");
    }
    return values;
}

AutoPtr<IUri> InboundSmsHandler::InitRawUri()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(Elastos::Droid::Provider::Telephony::Sms::CONTENT_URI, String("raw"), (IUri**)&uri);
    return uri;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
