#include "elastos/droid/server/CTelephonyRegistry.h"
#include "elastos/droid/server/am/BatteryStatsService.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
// using Elastos::Droid::Telephony::CSignalStrength;
// using Elastos::Droid::Telephony::CServiceState;
// using Elastos::Droid::Telephony::CVoLteServiceState;
// using Elastos::Droid::Telephony::CDisconnectCause;
// using Elastos::Droid::Telephony::CPreciseCallState;
// using Elastos::Droid::Telephony::CPreciseDataConnectionState;
using Elastos::Droid::Telephony::ICellLocationHelper;
// using Elastos::Droid::Telephony::CCellLocationHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
// using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
// using Elastos::Droid::Telephony::CSubscriptionManagerHelper;
using Elastos::Droid::Internal::Telephony::EIID_IITelephonyRegistry;
// using Elastos::Droid::Opt::Internal::Telephony::IDefaultPhoneNotifierHelper;
// using Elastos::Droid::Opt::Internal::Telephony::CDefaultPhoneNotifierHelper;

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CTelephonyRegistry::TAG("TelephonyRegistry");
const Boolean CTelephonyRegistry::DBG = FALSE; // STOPSHIP if TRUE
const Boolean CTelephonyRegistry::DBG_LOC = FALSE; // STOPSHIP if TRUE
const Boolean CTelephonyRegistry::VDBG = FALSE; // STOPSHIP if TRUE

const Int32 CTelephonyRegistry::PHONE_STATE_PERMISSION_MASK =
            IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR |
            IPhoneStateListener::LISTEN_CALL_STATE |
            IPhoneStateListener::LISTEN_DATA_ACTIVITY |
            IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE |
            IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR |
            IPhoneStateListener::LISTEN_VOLTE_STATE;

const Int32 CTelephonyRegistry::PRECISE_PHONE_STATE_PERMISSION_MASK =
            IPhoneStateListener::LISTEN_PRECISE_CALL_STATE |
            IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE;

const Int32 CTelephonyRegistry::MSG_USER_SWITCHED = 1;
const Int32 CTelephonyRegistry::MSG_UPDATE_DEFAULT_SUB = 2;

//===================================================================================
// LogSSC
//===================================================================================
LogSSC::LogSSC()
    : mSubId(0)
    , mPhoneId(0)
{}

ECode LogSSC::Set(
    /* [in] */ ITime* t,
    /* [in] */ const String& s,
    /* [in] */ Int64 subId,
    /* [in] */ Int32 phoneId,
    /* [in] */ Elastos::Droid::Telephony::IServiceState* state)
{
    mTime = t; mS = s; mSubId = subId; mPhoneId = phoneId; mState = state;
    return NOERROR;
}

ECode LogSSC::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb += mS;
    sb += " ";
    sb += TO_STR(mTime);
    sb += " ";
    sb += mSubId;
    sb += " ";
    sb += mPhoneId;
    sb += " ";
    sb += mState;
    *str = sb.ToString();
    return NOERROR;
}

//===================================================================================
// CTelephonyRegistry::Record
//===================================================================================

CTelephonyRegistry::Record::Record()
    : mCallerUid(0)
    , mEvents(0)
    , mSubId(0)
    , mPhoneId(0)
{
}

ECode CTelephonyRegistry::Record::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("{pkgForDebug=");
    sb += mPkgForDebug;
    sb += " callerUid=";
    sb += mCallerUid;
    sb += " subId=";
    sb += mSubId;
    sb += " phoneId=";
    sb += mPhoneId;
    sb += " events=";
    sb += StringUtils::ToHexString(mEvents);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//===================================================================================
// CTelephonyRegistry::MyHandler
//===================================================================================

CTelephonyRegistry::MyHandler::MyHandler(
    /* [in] */ CTelephonyRegistry* host)
    : mHost(host)
{}

ECode CTelephonyRegistry::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CTelephonyRegistry::MSG_USER_SWITCHED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (CTelephonyRegistry::VDBG)
                Slogger::D("CTelephonyRegistry", "MSG_USER_SWITCHED userId=%d", arg1);
            AutoPtr<ITelephonyManagerHelper> helper;
            assert(0 && "TODO");
            // CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
            AutoPtr<ITelephonyManager> tm;
            helper->GetDefault((ITelephonyManager**)&tm);
            Int32 numPhones;
            tm->GetPhoneCount(&numPhones);
            for (Int32 sub = 0; sub < numPhones; sub++) {
                mHost->NotifyCellLocationForSubscriber(sub, (*mHost->mCellLocation)[sub]);
            }
            break;
        }
        case CTelephonyRegistry::MSG_UPDATE_DEFAULT_SUB: {// do nothing
            if (CTelephonyRegistry::VDBG)
                Slogger::D("CTelephonyRegistry", "MSG_UPDATE_DEFAULT_SUB");
            break;
        }
    }
    return NOERROR;
}

//===================================================================================
// CTelephonyRegistry::MyBroadcastReceiver
//===================================================================================
CTelephonyRegistry::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CTelephonyRegistry* host)
{}

ECode CTelephonyRegistry::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (CTelephonyRegistry::VDBG)
        Slogger::D("CTelephonyRegistry", "mBroadcastReceiver: action=%s", action.string());
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 userHandle;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &userHandle);
        if (CTelephonyRegistry::DBG)
            Slogger::D("CTelephonyRegistry", "OnReceive: userHandle=%d", userHandle);
        AutoPtr<IMessage> msg;
        Boolean bval;
        mHost->mHandler->ObtainMessage(CTelephonyRegistry::MSG_USER_SWITCHED,
            userHandle, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (action.Equals(ITelephonyIntents::ACTION_DEFAULT_SUBSCRIPTION_CHANGED)) {
        if (DBG) {
            Slogger::D("CTelephonyRegistry", "OnReceive: ACTION_DEFAULT_SUBSCRIPTION_CHANGED");
        }
        AutoPtr<IMessage> msg;
        Boolean bval;
        mHost->mHandler->ObtainMessage(CTelephonyRegistry::MSG_UPDATE_DEFAULT_SUB,
            0, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

//===================================================================================
// CTelephonyRegistry
//===================================================================================

CAR_INTERFACE_IMPL_2(CTelephonyRegistry, Object, IITelephonyRegistry, IBinder)

CAR_OBJECT_IMPL(CTelephonyRegistry)

CTelephonyRegistry::CTelephonyRegistry()
    : mNumPhones(0)
    , mOtaspMode(1/*IServiceStateTracker::OTASP_UNKNOWN*/)
    , mRingingCallState(IPreciseCallState::PRECISE_CALL_STATE_IDLE)
    , mForegroundCallState(IPreciseCallState::PRECISE_CALL_STATE_IDLE)
    , mBackgroundCallState(IPreciseCallState::PRECISE_CALL_STATE_IDLE)
    , mNext(0)
{}

CTelephonyRegistry::~CTelephonyRegistry()
{}

ECode CTelephonyRegistry::constructor(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO");
    // CVoLteServiceState::New((IVoLteServiceState**)&mVoLteServiceState);
    // CDataConnectionRealTimeInfo::New((IDataConnectionRealTimeInfo**)&mDcRtInfo);
    // CPreciseCallState::New((IPreciseCallState**)&mPreciseCallState);
    // CPreciseDataConnectionState::New((IPreciseDataConnectionState**)&mPreciseDataConnectionState);nState();

    mHandler = new MyHandler(this);
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    mLogSSC = ArrayOf<LogSSC*>::Alloc(10);

    mContext = context;
    assert(0 && "TODO");
    // mBatteryStats = BatteryStatsService::GetService();

    AutoPtr<ITelephonyManagerHelper> tmHelper;
    // CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmHelper);
    AutoPtr<ITelephonyManager> tm;
    tmHelper->GetDefault((ITelephonyManager**)&tm);
    Int32 numPhones;
    tm->GetPhoneCount(&numPhones);
    if (DBG) Slogger::D(TAG, "TelephonyRegistor: ctor numPhones=%d", numPhones);
    mNumPhones = numPhones;
    mCallState = ArrayOf<Int32>::Alloc(numPhones);
    mDataActivity = ArrayOf<Int32>::Alloc(numPhones);
    mConnectedApns = ArrayOf<IArrayList*>::Alloc(numPhones);
    mDataConnectionState = ArrayOf<Int32>::Alloc(numPhones);
    mDataConnectionNetworkType = ArrayOf<Int32>::Alloc(numPhones);
    mCallIncomingNumber = ArrayOf<String>::Alloc(numPhones);
    mServiceState = ArrayOf<Elastos::Droid::Telephony::IServiceState*>::Alloc(numPhones);
    mSignalStrength = ArrayOf<ISignalStrength*>::Alloc(numPhones);
    mMessageWaiting = ArrayOf<Boolean>::Alloc(numPhones);
    mDataConnectionPossible = ArrayOf<Boolean>::Alloc(numPhones);
    mDataConnectionReason = ArrayOf<String>::Alloc(numPhones);
    mDataConnectionApn = ArrayOf<String>::Alloc(numPhones);
    mCallForwarding = ArrayOf<Boolean>::Alloc(numPhones);
    mCellLocation = ArrayOf<IBundle*>::Alloc(numPhones);
    mDataConnectionLinkProperties = ArrayOf<ILinkProperties*>::Alloc(numPhones);
    mDataConnectionNetworkCapabilities =  ArrayOf<INetworkCapabilities*>::Alloc(numPhones);

    String emptStr("");
    for (Int32 i = 0; i < numPhones; i++) {
        (*mCallState)[i] =  ITelephonyManager::CALL_STATE_IDLE;
        (*mDataActivity)[i] = ITelephonyManager::DATA_ACTIVITY_NONE;
        CArrayList::New(&((*mConnectedApns)[i]));
        (*mDataConnectionState)[i] = ITelephonyManager::DATA_UNKNOWN;
        (*mCallIncomingNumber)[i] = "";
        // CServiceState::New((Elastos::Droid::Telephony::IServiceState**)&(*mServiceState)[i]);
        // CSignalStrength::New((ISignalStrength**)&(*mSignalStrength)[i]);
        (*mDataConnectionReason)[i] =  emptStr;
        (*mDataConnectionApn)[i] =  emptStr;
        CBundle::New((IBundle**)&(*mCellLocation)[i]);
        mCellInfo.PushBack(NULL);
    }

    // Note that location can be NULL for non-phone builds like
    // like the generic one.
    AutoPtr<ICellLocationHelper> clHelper;
    assert(0 && "TODO");
    // CCellLocationHelper::AcquireSingleton((ICellLocationHelper**)&clHelper);
    AutoPtr<ICellLocation> location;
    clHelper->GetEmpty((ICellLocation**)&location);
    if (location != NULL) {
        for (Int32 i = 0; i < numPhones; i++) {
            location->FillInNotifierBundle((*mCellLocation)[i]);
        }
    }
    return NOERROR;
}

ECode CTelephonyRegistry::SystemRunning()
{
    // Watch for interesting updates
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    filter->AddAction(IIntent::ACTION_USER_REMOVED);
    filter->AddAction(ITelephonyIntents::ACTION_DEFAULT_SUBSCRIPTION_CHANGED);
    Slogger::D(TAG, "SystemRunning register for intents");
    AutoPtr<IIntent> intent;
    return mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);
}

ECode CTelephonyRegistry::Listen(
    /* [in] */ const String& pkgForDebug,
    /* [in] */ IIPhoneStateListener* callback,
    /* [in] */ Int32 events,
    /* [in] */ Boolean notifyNow)
{
    return ListenForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, pkgForDebug, callback, events,
        notifyNow);
}

ECode CTelephonyRegistry::ListenForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& pkgForDebug,
    /* [in] */ IIPhoneStateListener* callback,
    /* [in] */ Int32 events,
    /* [in] */ Boolean notifyNow)
{
    return Listen(pkgForDebug, callback, events, notifyNow, subId);
}

ECode CTelephonyRegistry::Listen(
    /* [in] */ const String& pkgForDebug,
    /* [in] */ IIPhoneStateListener* callback,
    /* [in] */ Int32 events,
    /* [in] */ Boolean notifyNow,
    /* [in] */ Int64 subId)
{
    Int32 callerUid = UserHandle::GetCallingUserId();
    Int32 myUid = UserHandle::GetMyUserId();
    if (VDBG) {
        Slogger::D(TAG, "listen: E pkg=%s events=0x%08x notifyNow=%d subId=%lld myUid=%d callerUid=%d",
         pkgForDebug.string(), events, notifyNow, subId, myUid, callerUid);
    }
    if (events != 0) {
        /* Checks permission and throws Security exception */
        FAIL_RETURN(CheckListenerPermission(events))

        synchronized(mRecordsLock) {
            // register
            AutoPtr<Record> r;
            AutoPtr<ISubscriptionManager> smHelper;
            //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
            Boolean found = FALSE, bval;
            AutoPtr<IBinder> b = IBinder::Probe(callback);
            List<AutoPtr<Record> >::Iterator it;
            for (it = mRecords.Begin(); it != mRecords.End(); ++it) {
                r = *it;
                if (b == r->mBinder) {
                    found = TRUE;
                    break;
                }
            }
            if (!found) {
                r = new Record();
                r->mBinder = b;
                r->mCallback = callback;
                r->mPkgForDebug = pkgForDebug;
                r->mCallerUid = callerUid;
                // Legacy applications pass ISubscriptionManager::DEFAULT_SUB_ID,
                // force all illegal subId to ISubscriptionManager::DEFAULT_SUB_ID
                smHelper->IsValidSubId(subId, &bval);
                if (!bval) {
                    r->mSubId = ISubscriptionManager::DEFAULT_SUB_ID;
                }
                else {//APP specify subID
                    r->mSubId = subId;
                }
                smHelper->GetPhoneId(r->mSubId, &r->mPhoneId);

                mRecords.PushBack(r);
                if (DBG) Slogger::D(TAG, "listen: add new record");
            }

            Int32 phoneId = r->mPhoneId;
            r->mEvents = events;
            if (DBG) {
                Slogger::D(TAG, "listen: r=%s r->mSubId=%d phoneId=%d",
                    TO_CSTR(r), r->mSubId, phoneId);
            }
            if (VDBG) ToStringLogSSC(String("listen"));

            ECode ec = NOERROR;
            if (notifyNow && ValidatePhoneId(phoneId)) {
                if ((events & IPhoneStateListener::LISTEN_SERVICE_STATE) != 0) {
                    if (VDBG) Slogger::D(TAG, "listen: call onSSC state=%s",
                        TO_CSTR((*mServiceState)[phoneId]));
                    AutoPtr<Elastos::Droid::Telephony::IServiceState> ss;
                    // CServiceState::New((*mServiceState)[phoneId]), (Elastos::Droid::Telephony::IServiceState**)&ss)
                    ec = r->mCallback->OnServiceStateChanged(ss);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_SIGNAL_STRENGTH) != 0) {
                    Int32 gsmSignalStrength;
                    (*mSignalStrength)[phoneId]->GetGsmSignalStrength(&gsmSignalStrength);
                    ec = r->mCallback->OnSignalStrengthChanged((gsmSignalStrength == 99 ? -1 : gsmSignalStrength));
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR) != 0) {
                    ec = r->mCallback->OnMessageWaitingIndicatorChanged((*mMessageWaiting)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR) != 0) {
                    ec = r->mCallback->OnCallForwardingIndicatorChanged((*mCallForwarding)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if (ValidateEventsAndUserLocked(r, IPhoneStateListener::LISTEN_CELL_LOCATION)) {
                    if (DBG_LOC) Slogger::D(TAG, "listen: mCellLocation = %s",
                        Object::ToString((*mCellLocation)[phoneId]).string());
                    AutoPtr<IBundle> bundle;
                    CBundle::New((*mCellLocation)[phoneId], (IBundle**)&bundle);
                    ec = r->mCallback->OnCellLocationChanged(bundle);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_CALL_STATE) != 0) {
                    ec = r->mCallback->OnCallStateChanged((*mCallState)[phoneId],
                        (*mCallIncomingNumber)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE) != 0) {
                    ec = r->mCallback->OnDataConnectionStateChanged((*mDataConnectionState)[phoneId],
                        (*mDataConnectionNetworkType)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_DATA_ACTIVITY) != 0) {
                    ec = r->mCallback->OnDataActivity((*mDataActivity)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS) != 0) {
                    ec = r->mCallback->OnSignalStrengthsChanged((*mSignalStrength)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_OTASP_CHANGED) != 0) {
                    ec = r->mCallback->OnOtaspChanged(mOtaspMode);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if (ValidateEventsAndUserLocked(r, IPhoneStateListener::LISTEN_CELL_INFO)) {
                    if (DBG_LOC) Slogger::D(TAG, "listen: mCellInfo[%d] = %s",
                        phoneId, TO_CSTR(mCellInfo[phoneId]));
                    ec = r->mCallback->OnCellInfoChanged(mCellInfo[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_DATA_CONNECTION_REAL_TIME_INFO) != 0) {
                    ec = r->mCallback->OnDataConnectionRealTimeInfoChanged(mDcRtInfo);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_PRECISE_CALL_STATE) != 0) {
                    ec = r->mCallback->OnPreciseCallStateChanged(mPreciseCallState);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
                if ((events & IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE) != 0) {
                    ec = r->mCallback->OnPreciseDataConnectionStateChanged(mPreciseDataConnectionState);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Remove(r->mBinder);
                    }
                }
            }
        }
    }
    else {
        Remove(IBinder::Probe(callback));
    }
    return NOERROR;
}

ECode CTelephonyRegistry::Remove(
    /* [in] */ IBinder* binder)
{
    synchronized(mRecordsLock) {
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            if ((*it)->mBinder.Get() == binder) {
                mRecords.Erase(it);
                break;
            }
        }
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyCallState(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (!CheckNotifyPermission(String("notifyCallState()"))) {
        return NOERROR;
    }

    if (VDBG) {
        Slogger::D(TAG, "notifyCallState: state=%d incomingNumber=%s",
            state, incomingNumber.string());
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if (((r->mEvents & IPhoneStateListener::LISTEN_CALL_STATE) != 0)
                && (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID)) {
                ec = r->mCallback->OnCallStateChanged(state, incomingNumber);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return BroadcastCallStateChanged(state, incomingNumber, ISubscriptionManager::DEFAULT_SUB_ID);
}

ECode CTelephonyRegistry::NotifyCallStateForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (!CheckNotifyPermission(String("notifyCallState()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyCallStateForSubscriber: subId=%lld state=%d, incomingNumber=%s",
            subId, state, incomingNumber.string());
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        if (ValidatePhoneId(phoneId)) {
            (*mCallState)[phoneId] = state;
            (*mCallIncomingNumber)[phoneId] = incomingNumber;
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (((r->mEvents & IPhoneStateListener::LISTEN_CALL_STATE) != 0) &&
                        (r->mSubId == subId) &&
                        (r->mSubId != ISubscriptionManager::DEFAULT_SUB_ID)) {
                    ec = r->mCallback->OnCallStateChanged(state, incomingNumber);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        HandleRemoveListLocked();
    }
    return BroadcastCallStateChanged(state, incomingNumber, subId);
}

ECode CTelephonyRegistry::NotifyServiceStateForPhoneId(
    /* [in] */ Int32 phoneId,
    /* [in] */ Int64 subId,
    /* [in] */ Elastos::Droid::Telephony::IServiceState* state)
{
    if (!CheckNotifyPermission(String("notifyServiceState()"))) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        if (VDBG) {
            Slogger::D(TAG, "notifyServiceStateForSubscriber: subId=%lld phoneId=%d, state=%s",
                subId, phoneId, TO_CSTR(state));
        }
        if (ValidatePhoneId(phoneId)) {
            mServiceState->Set(phoneId, state);
            LogServiceStateChanged(String("notifyServiceStateForSubscriber"), subId, phoneId, state);
            if (VDBG) ToStringLogSSC(String("notifyServiceStateForSubscriber"));

            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (VDBG) {
                    Slogger::D(TAG, "notifyServiceStateForSubscriber: r=%s subId=%lld phoneId=%d, state=%s",
                        TO_CSTR(r), subId, phoneId, TO_CSTR(state));
                }
                if (((r->mEvents & IPhoneStateListener::LISTEN_SERVICE_STATE) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {

                    if (DBG) {
                        Slogger::D(TAG, "notifyServiceStateForSubscriber: callback.onSSC r=%s subId=%lld phoneId=%d, state=%s",
                            TO_CSTR(r), subId, phoneId, TO_CSTR(state));
                    }
                    AutoPtr<Elastos::Droid::Telephony::IServiceState> ss;
                    //CServiceState::New(state, (Elastos::Droid::Telephony::IServiceState**)&ss);
                    ec = r->mCallback->OnServiceStateChanged(ss);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        else {
            Slogger::D(TAG, "notifyServiceStateForSubscriber: INVALID phoneId=%d", phoneId);
        }
        HandleRemoveListLocked();
    }
    return BroadcastServiceStateChanged(state, subId);
}

ECode CTelephonyRegistry::NotifySignalStrength(
    /* [in] */ ISignalStrength* signalStrength)
{
    return NotifySignalStrengthForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, signalStrength);
}

ECode CTelephonyRegistry::NotifySignalStrengthForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ ISignalStrength* signalStrength)
{
    if (!CheckNotifyPermission(String("notifySignalStrength()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifySignalStrengthForSubscriber: subId=%lld signalStrength=%s",
            subId, TO_CSTR(signalStrength));
        ToStringLogSSC(String("notifySignalStrengthForSubscriber"));
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        if (ValidatePhoneId(phoneId)) {
            if (VDBG) Slogger::D(TAG, "notifySignalStrengthForSubscriber: valid phoneId=%d", phoneId);
            mSignalStrength->Set(phoneId, signalStrength);
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (VDBG) {
                    Slogger::D(TAG, "notifySignalStrengthForSubscriber: r=%s subId=%lld, phoneId=%d, ss=%s",
                        TO_CSTR(r), subId, phoneId, TO_CSTR(signalStrength));
                }
                if (((r->mEvents & IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                    if (DBG) {
                        Slogger::D(TAG, "notifySignalStrengthForSubscriber: callback.onSsS r=%s subId=%lld, phoneId=%d, ss=%s",
                            TO_CSTR(r), subId, phoneId, TO_CSTR(signalStrength));
                    }

                    AutoPtr<ISignalStrength> ss;
                    //CSignalStrength::New(signalStrength, (ISignalStrength**)&ss);
                    ec = r->mCallback->OnSignalStrengthsChanged(ss);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
                if (((r->mEvents & IPhoneStateListener::LISTEN_SIGNAL_STRENGTH) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                    Int32 gsmSignalStrength;
                    signalStrength->GetGsmSignalStrength(&gsmSignalStrength);
                    Int32 ss = (gsmSignalStrength == 99 ? -1 : gsmSignalStrength);
                    if (DBG) {
                        Slogger::D(TAG, "notifySignalStrengthForSubscriber: callback.onSsS r=%s subId=%lld, phoneId=%d, gsmSS=%d, ss=%d",
                            TO_CSTR(r), subId, phoneId, gsmSignalStrength, ss);
                    }
                    ec = r->mCallback->OnSignalStrengthChanged(ss);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        else {
            Slogger::D(TAG, "notifySignalStrengthForSubscriber: invalid phoneId=%d", phoneId);
        }
        HandleRemoveListLocked();
    }
    return BroadcastSignalStrengthChanged(signalStrength, subId);
}

ECode CTelephonyRegistry::NotifyCellInfo(
    /* [in] */ IList* cellInfo) //<CellInfo>
{
     return NotifyCellInfoForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, cellInfo);
}

ECode CTelephonyRegistry::NotifyCellInfoForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ IList* cellInfo)
{
    if (!CheckNotifyPermission(String("notifyCellInfo()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyCellInfoForSubscriber: subId=%lld cellInfo=%s",
            subId, TO_CSTR(cellInfo));
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        if (ValidatePhoneId(phoneId)) {
            mCellInfo[phoneId] = cellInfo;
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (ValidateEventsAndUserLocked(r, IPhoneStateListener::LISTEN_CELL_INFO) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {

                    if (DBG_LOC) {
                        Slogger::D(TAG, "notifyCellInfo: mCellInfo=%s r=%s",
                            TO_CSTR(cellInfo), TO_CSTR(r));
                    }
                    ec = r->mCallback->OnCellInfoChanged(cellInfo);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyDataConnectionRealTimeInfo(
    /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo)
{
    if (!CheckNotifyPermission(String("notifyDataConnectionRealTimeInfo()"))) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mDcRtInfo = dcRtInfo;
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if (ValidateEventsAndUserLocked(r,
                    IPhoneStateListener::LISTEN_DATA_CONNECTION_REAL_TIME_INFO)) {

                if (DBG_LOC) {
                    Slogger::D(TAG, "notifyDataConnectionRealTimeInfo: mDcRtInfo=%s, r=%s",
                        TO_CSTR(mDcRtInfo), TO_CSTR(mDcRtInfo));
                }
                ec = r->mCallback->OnDataConnectionRealTimeInfoChanged(mDcRtInfo);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyMessageWaitingChangedForPhoneId(
    /* [in] */ Int32 phoneId,
    /* [in] */ Int64 subId,
    /* [in] */ Boolean mwi)
{
    if (!CheckNotifyPermission(String("notifyMessageWaitingChanged()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyMessageWaitingChangedForSubscriberPhoneID: phoneId=%d, subId=%lld mwi=%d",
            phoneId, subId, mwi);
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        if (ValidatePhoneId(phoneId)) {
            (*mMessageWaiting)[phoneId] = mwi;
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (((r->mEvents & IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                    ec = r->mCallback->OnMessageWaitingIndicatorChanged(mwi);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyCallForwardingChanged(
    /* [in] */ Boolean cfi)
{
    return NotifyCallForwardingChangedForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, cfi);
}

ECode CTelephonyRegistry::NotifyCallForwardingChangedForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Boolean cfi)
{
    if (!CheckNotifyPermission(String("notifyCallForwardingChanged()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyCallForwardingChangedForSubscriber: subId=%lld cfi=%d",
            subId, cfi);
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        if (ValidatePhoneId(phoneId)) {
            (*mCallForwarding)[phoneId] = cfi;
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (((r->mEvents & IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                    ec = r->mCallback->OnCallForwardingIndicatorChanged(cfi);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyDataActivity(
    /* [in] */ Int32 state)
{
    return NotifyDataActivityForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, state);
}

ECode CTelephonyRegistry::NotifyDataActivityForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 state)
{
    if (!CheckNotifyPermission(String("notifyDataActivity()" ))) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        (*mDataActivity)[phoneId] = state;
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_DATA_ACTIVITY) != 0) {
                ec = r->mCallback->OnDataActivity(state);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyDataConnection(
    /* [in] */ Int32 state,
    /* [in] */ Boolean isDataConnectivityPossible,
    /* [in] */ const String& reason,
    /* [in] */ const String& apn,
    /* [in] */ const String& apnType,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean roaming)
{
    return NotifyDataConnectionForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, state,
        isDataConnectivityPossible,reason, apn, apnType, linkProperties,
        networkCapabilities, networkType, roaming);
}

ECode CTelephonyRegistry::NotifyDataConnectionForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 state,
    /* [in] */ Boolean isDataConnectivityPossible,
    /* [in] */ const String& reason,
    /* [in] */ const String& apn,
    /* [in] */ const String& apnType,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean roaming)
{
    if (!CheckNotifyPermission(String("notifyDataConnection()" ))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyDataConnectionForSubscriber: subId=%lld state=%d isDataConnectivityPossible=%d"
            " reason=%s apn=%s apnType=%s networkType=%d mRecords size=%d",
            subId, state, isDataConnectivityPossible, reason.string(), apn.string(), apnType.string(),
            networkType, mRecords.GetSize());
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        Boolean modified = FALSE;
        if (state == ITelephonyManager::DATA_CONNECTED) {
            AutoPtr<IArrayList> p = (*mConnectedApns)[phoneId];
            Boolean b = FALSE;
            p->Contains(CoreUtils::Convert(apnType), &b);
            if (!b) {
                p->Add(CoreUtils::Convert(apnType));
                if ((*mDataConnectionState)[phoneId] != state) {
                    (*mDataConnectionState)[phoneId] = state;
                    modified = TRUE;
                }
            }
        }
        else {
            Boolean bRm = FALSE, bEmp = FALSE;
            (*mConnectedApns)[phoneId]->Remove(CoreUtils::Convert(apnType), &bRm);
            if (bRm) {
                (*mConnectedApns)[phoneId]->IsEmpty(&bEmp);
                if (bEmp) {
                    (*mDataConnectionState)[phoneId] = state;
                    modified = TRUE;
                }
                else {
                    // leave mDataConnectionState as is and
                    // send out the new status for the APN in question.
                }
            }
        }

        (*mDataConnectionPossible)[phoneId] = isDataConnectivityPossible;
        (*mDataConnectionReason)[phoneId] = reason;
        mDataConnectionLinkProperties->Set(phoneId, linkProperties);
        mDataConnectionNetworkCapabilities->Set(phoneId, networkCapabilities);

        if ((*mDataConnectionNetworkType)[phoneId] != networkType) {
            (*mDataConnectionNetworkType)[phoneId] = networkType;
            // need to tell registered Listen(ers about the new network type
            modified = TRUE;
        }
        if (modified) {
            if (DBG) {
                Slogger::D(TAG, "onDataConnectionStateChanged(%d, %d)",
                    (*mDataConnectionState)[phoneId], (*mDataConnectionNetworkType)[phoneId]);
            }
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (((r->mEvents & IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE) != 0) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                    Slogger::D(TAG, "Notify data connection state changed on sub: %lld", subId);
                    ec = r->mCallback->OnDataConnectionStateChanged(
                        (*mDataConnectionState)[phoneId], (*mDataConnectionNetworkType)[phoneId]);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
            HandleRemoveListLocked();
        }
        mPreciseDataConnectionState = NULL;
        assert(0 && "TODO");
        // CPreciseDataConnectionState::New(state, networkType, apnType, apn,
        //     reason, linkProperties, String(""), (IPreciseDataConnectionState**)&mPreciseDataConnectionState);
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE) != 0) {
                ec = r->mCallback->OnPreciseDataConnectionStateChanged(mPreciseDataConnectionState);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    BroadcastDataConnectionStateChanged(state, isDataConnectivityPossible, reason, apn,
            apnType, linkProperties, networkCapabilities, roaming, subId);
    return BroadcastPreciseDataConnectionStateChanged(state, networkType, apnType, apn, reason,
            linkProperties, String(""));
}

ECode CTelephonyRegistry::NotifyDataConnectionFailed(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType)
{
     return NotifyDataConnectionFailedForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID,
             reason, apnType);
}

ECode CTelephonyRegistry::NotifyDataConnectionFailedForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType)
{
    if (!CheckNotifyPermission(String("notifyDataConnectionFailed()"))) {
        return NOERROR;
    }
    if (VDBG) {
        Slogger::D(TAG, "notifyDataConnectionFailedForSubscriber: subId=%lld reason=%s apnType=%s",
            subId, reason.string(), apnType.string());
    }
    ECode ec = NOERROR;
    String emptyStr("");
    synchronized(mRecordsLock) {
        mPreciseDataConnectionState = NULL;
        // CPreciseDataConnectionState::New(
        //     ITelephonyManager::DATA_UNKNOWN,ITelephonyManager::NETWORK_TYPE_UNKNOWN,
        //     apnType, emptyStr, reason, NULL, emptyStr, (IPreciseDataConnectionState**)&mPreciseDataConnectionState);
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE) != 0) {
                ec = r->mCallback->OnPreciseDataConnectionStateChanged(mPreciseDataConnectionState);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    BroadcastDataConnectionFailed(reason, apnType, subId);
    return BroadcastPreciseDataConnectionStateChanged(ITelephonyManager::DATA_UNKNOWN,
            ITelephonyManager::NETWORK_TYPE_UNKNOWN, apnType, emptyStr, reason, NULL, emptyStr);
}

ECode CTelephonyRegistry::NotifyCellLocation(
    /* [in] */ IBundle* cellLocation)
{
     return NotifyCellLocationForSubscriber(ISubscriptionManager::DEFAULT_SUB_ID, cellLocation);
}

ECode CTelephonyRegistry::NotifyCellLocationForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ IBundle* cellLocation)
{
    if (VDBG) {
        Slogger::D(TAG, "notifyCellLocationForSubscriber: subId=%lld cellLocation=%s",
            subId, TO_CSTR(cellLocation));
    }
    if (!CheckNotifyPermission(String("notifyCellLocation()"))) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        AutoPtr<ISubscriptionManager> smHelper;
        //CSubscriptionManagerHelper::AcquireSingleton((ISubscriptionManagerHelper**)&smHelper);
        Int32 phoneId;
        smHelper->GetPhoneId(subId, &phoneId);
        if (ValidatePhoneId(phoneId)) {
            mCellLocation->Set(phoneId, cellLocation);
            List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
            for (; it != mRecords.End(); ++it) {
                Record* r = *it;
                if (ValidateEventsAndUserLocked(r, IPhoneStateListener::LISTEN_CELL_LOCATION) &&
                        ((r->mSubId == subId) ||
                        (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {

                    if (DBG_LOC) {
                        Slogger::D(TAG, "notifyCellLocation: cellLocation=%s r=%s",
                            TO_CSTR(cellLocation), TO_CSTR(r));
                    }
                    AutoPtr<IBundle> bundle;
                    CBundle::New(cellLocation, (IBundle**)&bundle);
                    ec = r->mCallback->OnCellLocationChanged(bundle);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        mRemoveList.PushBack(r->mBinder);
                    }
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    if (!CheckNotifyPermission(String("notifyOtaspChanged()" ))) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mOtaspMode = otaspMode;
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_OTASP_CHANGED) != 0) {
                ec = r->mCallback->OnOtaspChanged(otaspMode);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyPreciseCallState(
    /* [in] */ Int32 ringingCallState,
    /* [in] */ Int32 foregroundCallState,
    /* [in] */ Int32 backgroundCallState)
{
    if (!CheckNotifyPermission(String("notifyPreciseCallState()"))) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mRingingCallState = ringingCallState;
        mForegroundCallState = foregroundCallState;
        mBackgroundCallState = backgroundCallState;
        mPreciseCallState = NULL;
        // CPreciseCallState::New(ringingCallState, foregroundCallState,
        //     backgroundCallState, IDisconnectCause::NOT_VALID,
        //     IPreciseDisconnectCause::NOT_VALID,
        //     (IPreciseCallState**)&mPreciseCallState);
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_PRECISE_CALL_STATE) != 0) {
                ec = r->mCallback->OnPreciseCallStateChanged(mPreciseCallState);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return BroadcastPreciseCallStateChanged(ringingCallState,
        foregroundCallState, backgroundCallState,
        IDisconnectCause::NOT_VALID, IPreciseDisconnectCause::NOT_VALID);
}

ECode CTelephonyRegistry::NotifyDisconnectCause(
    /* [in] */ Int32 disconnectCause,
    /* [in] */ Int32 preciseDisconnectCause)
{
    if (!CheckNotifyPermission(String("notifyDisconnectCause()"))) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mPreciseCallState = NULL;
        // CPreciseCallState::New(mRingingCallState, mForegroundCallState,
        //     mBackgroundCallState, disconnectCause, preciseDisconnectCause,
        //     (IPreciseCallState**)&mPreciseCallState);
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_PRECISE_CALL_STATE) != 0) {
                 ec = r->mCallback->OnPreciseCallStateChanged(mPreciseCallState);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return BroadcastPreciseCallStateChanged(mRingingCallState, mForegroundCallState,
        mBackgroundCallState, disconnectCause, preciseDisconnectCause);
}

ECode CTelephonyRegistry::NotifyPreciseDataConnectionFailed(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ const String& apn,
    /* [in] */ const String& failCause)
{
    if (!CheckNotifyPermission(String("NotifyPreciseDataConnectionFailed()"))) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mPreciseDataConnectionState = NULL;
        // CPreciseDataConnectionState::New(
        //     ITelephonyManager::DATA_UNKNOWN, ITelephonyManager::NETWORK_TYPE_UNKNOWN,
        //     apnType, apn, reason, NULL, failCause,
        //     (IPreciseDataConnectionState**)&mPreciseDataConnectionState);
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE) != 0) {
                ec = r->mCallback->OnPreciseDataConnectionStateChanged(mPreciseDataConnectionState);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return BroadcastPreciseDataConnectionStateChanged(ITelephonyManager::DATA_UNKNOWN,
            ITelephonyManager::NETWORK_TYPE_UNKNOWN, apnType, apn, reason, NULL, failCause);
}

ECode CTelephonyRegistry::NotifyVoLteServiceStateChanged(
    /* [in] */ IVoLteServiceState* lteState)
{
    if (!CheckNotifyPermission(String("NotifyVoLteServiceStateChanged()"))) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        mVoLteServiceState = lteState;
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if ((r->mEvents & IPhoneStateListener::LISTEN_VOLTE_STATE) != 0) {
                AutoPtr<IVoLteServiceState> vss;
                // CVoLteServiceState::New(mVoLteServiceState, (IVoLteServiceState**)&vss);
                ec = r->mCallback->OnVoLteServiceStateChanged(vss);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::NotifyOemHookRawEventForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ ArrayOf<Byte>* rawData)
{
    if (!CheckNotifyPermission(String("NotifyOemHookRawEventForSubscriber"))) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    synchronized(mRecordsLock) {
        List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
        for (; it != mRecords.End(); ++it) {
            Record* r = *it;
            if (VDBG) {
                Slogger::D(TAG, "NotifyOemHookRawEventForSubscriber:  r=%s subId=",
                    TO_CSTR(r), subId);
            }
            if (((r->mEvents & IPhoneStateListener::LISTEN_OEM_HOOK_RAW_EVENT) != 0) &&
                    ((r->mSubId == subId) ||
                    (r->mSubId == ISubscriptionManager::DEFAULT_SUB_ID))) {
                ec = r->mCallback->OnOemHookRawEvent(rawData);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    mRemoveList.PushBack(r->mBinder);
                }
            }
        }
        HandleRemoveListLocked();
    }
    return NOERROR;
}

ECode CTelephonyRegistry::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // if (mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
    //         != IPackageManager::PERMISSION_GRANTED) {
    //     pw->Println("Permission Denial: can't dump telephony.registry from from pid="
    //             + Binder::GetCallingPid() + ", uid=" + Binder::GetCallingUid());
    //     return NOERROR;
    // }
    // synchronized(mRecordsLock) {
    //     Int32 recordCount = mRecords.size();
    //     pw->Println("last known state:");
    //     for (Int32 i = 0; i < ITelephonyManager::getDefault().getPhoneCount(); i++) {
    //         pw->Println("  Phone Id=" + i);
    //         pw->Println("  mCallState=" + mCallState[i]);
    //         pw->Println("  mCallIncomingNumber=" + mCallIncomingNumber[i]);
    //         pw->Println("  mServiceState=" + mServiceState[i]);
    //         pw->Println("  mSignalStrength=" + mSignalStrength[i]);
    //         pw->Println("  mMessageWaiting=" + mMessageWaiting[i]);
    //         pw->Println("  mCallForwarding=" + mCallForwarding[i]);
    //         pw->Println("  mDataActivity=" + mDataActivity[i]);
    //         pw->Println("  mDataConnectionState=" + mDataConnectionState[i]);
    //         pw->Println("  mDataConnectionPossible=" + mDataConnectionPossible[i]);
    //         pw->Println("  mDataConnectionReason=" + mDataConnectionReason[i]);
    //         pw->Println("  mDataConnectionApn=" + mDataConnectionApn[i]);
    //         pw->Println("  mDataConnectionLinkProperties=" + mDataConnectionLinkProperties[i]);
    //         pw->Println("  mDataConnectionNetworkCapabilities=" +
    //                 mDataConnectionNetworkCapabilities[i]);
    //         pw->Println("  mCellLocation=" + (*mCellLocation)[i]);
    //         pw->Println("  mCellInfo=" + mCellInfo.get(i));
    //     }
    //     pw->Println("  mDcRtInfo=" + mDcRtInfo);
    //     pw->Println("registrations: count=" + recordCount);
    //     List<AutoPtr<Record> >::Iterator it = mRecords.Begin();
    //     for (; it != mRecords.End(); ++it) {
    //         Record* r = *it;
    //         pw->Print("  ");
    //         pw->Println(TO_STR(r));
    //     }
    // }
    return NOERROR;
}

ECode CTelephonyRegistry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CTelephonyRegistry";
    return NOERROR;
}

//
// the legacy intent Broadcasting
//

ECode CTelephonyRegistry::BroadcastServiceStateChanged(
    /* [in] */ Elastos::Droid::Telephony::IServiceState* state,
    /* [in] */ Int64 subId)
{
    Int32 ival;
    Int64 ident = Binder::ClearCallingIdentity();
    state->GetState(&ival);
    mBatteryStats->NotePhoneState(ival);
    Binder::RestoreCallingIdentity(ident);

    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED, (IIntent**)&intent);
    AutoPtr<IBundle> data;
    CBundle::New((IBundle**)&data);
    state->FillInNotifierBundle(data);
    intent->PutExtras(data);
    // Pass the subscription along with the intent->
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    return mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CTelephonyRegistry::BroadcastSignalStrengthChanged(
    /* [in] */ ISignalStrength* signalStrength,
    /* [in] */ Int64 subId)
{
    Int64 ident = Binder::ClearCallingIdentity();
    mBatteryStats->NotePhoneSignalStrength(signalStrength);
    Binder::RestoreCallingIdentity(ident);

    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_SIGNAL_STRENGTH_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    AutoPtr<IBundle> data;
    CBundle::New((IBundle**)&data);
    signalStrength->FillInNotifierBundle(data);
    intent->PutExtras(data);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    return mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CTelephonyRegistry::BroadcastCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber,
    /* [in] */ Int64 subId)
{
    Int64 ident = Binder::ClearCallingIdentity();
    if (state == ITelephonyManager::CALL_STATE_IDLE) {
        mBatteryStats->NotePhoneOff();
    }
    else {
        mBatteryStats->NotePhoneOn();
    }

    Binder::RestoreCallingIdentity(ident);

    assert(0 && "TODO");
    Int32 ival;
    // AutoPtr<IDefaultPhoneNotifierHelper> dpnHelper;
    // CDefaultPhoneNotifierHelper::AcquireSingleton((IDefaultPhoneNotifierHelper**)&dpnHelper);
    // dpnHelper->ConvertCallState(state, &ival);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyManager::ACTION_PHONE_STATE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::STATE_KEY, StringUtils::ToString(ival));
    if (!TextUtils::IsEmpty(incomingNumber)) {
        intent->PutExtra(ITelephonyManager::EXTRA_INCOMING_NUMBER, incomingNumber);
    }
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    return mContext->SendBroadcastAsUser(intent, UserHandle::ALL,
        Manifest::permission::READ_PHONE_STATE);
}

ECode CTelephonyRegistry::BroadcastDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Boolean isDataConnectivityPossible,
    /* [in] */ const String& reason,
    /* [in] */ const String& apn,
    /* [in] */ const String& apnType,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ Boolean roaming,
    /* [in] */ Int64 subId)
{
    // Note: not reporting to the battery stats service here, because the
    // status bar takes care of that after taking into account all of the
    // required info.
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_ANY_DATA_CONNECTION_STATE_CHANGED, (IIntent**)&intent);

    assert(0 && "TODO");
    Int32 ival;
    // AutoPtr<IDefaultPhoneNotifierHelper> dpnHelper;
    // CDefaultPhoneNotifierHelper::AcquireSingleton((IDefaultPhoneNotifierHelper**)&dpnHelper);
    // dpnHelper->ConvertCallState(state, &ival);
    intent->PutExtra(IPhoneConstants::STATE_KEY, StringUtils::ToString(ival));
    if (!isDataConnectivityPossible) {
        intent->PutExtra(IPhoneConstants::NETWORK_UNAVAILABLE_KEY, TRUE);
    }
    if (reason != NULL) {
        intent->PutExtra(IPhoneConstants::STATE_CHANGE_REASON_KEY, reason);
    }
    if (linkProperties != NULL) {
        intent->PutExtra(IPhoneConstants::DATA_LINK_PROPERTIES_KEY, IParcelable::Probe(linkProperties));
        String iface;
        linkProperties->GetInterfaceName(&iface);
        if (iface != NULL) {
            intent->PutExtra(IPhoneConstants::DATA_IFACE_NAME_KEY, iface);
        }
    }
    if (networkCapabilities != NULL) {
        intent->PutExtra(IPhoneConstants::DATA_NETWORK_CAPABILITIES_KEY, IParcelable::Probe(networkCapabilities));
    }
    if (roaming) intent->PutExtra(IPhoneConstants::DATA_NETWORK_ROAMING_KEY, TRUE);

    intent->PutExtra(IPhoneConstants::DATA_APN_KEY, apn);
    intent->PutExtra(IPhoneConstants::DATA_APN_TYPE_KEY, apnType);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    return mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CTelephonyRegistry::BroadcastDataConnectionFailed(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ Int64 subId)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_DATA_CONNECTION_FAILED, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::FAILURE_REASON_KEY, reason);
    intent->PutExtra(IPhoneConstants::DATA_APN_TYPE_KEY, apnType);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    return mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CTelephonyRegistry::BroadcastPreciseCallStateChanged(
    /* [in] */ Int32 ringingCallState,
    /* [in] */ Int32 foregroundCallState,
    /* [in] */ Int32 backgroundCallState,
    /* [in] */ Int32 disconnectCause,
    /* [in] */ Int32 preciseDisconnectCause)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyManager::ACTION_PRECISE_CALL_STATE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(ITelephonyManager::EXTRA_RINGING_CALL_STATE, ringingCallState);
    intent->PutExtra(ITelephonyManager::EXTRA_FOREGROUND_CALL_STATE, foregroundCallState);
    intent->PutExtra(ITelephonyManager::EXTRA_BACKGROUND_CALL_STATE, backgroundCallState);
    intent->PutExtra(ITelephonyManager::EXTRA_DISCONNECT_CAUSE, disconnectCause);
    intent->PutExtra(ITelephonyManager::EXTRA_PRECISE_DISCONNECT_CAUSE, preciseDisconnectCause);
    return mContext->SendBroadcastAsUser(intent, UserHandle::ALL,
            Manifest::permission::READ_PRECISE_PHONE_STATE);
}

ECode CTelephonyRegistry::BroadcastPreciseDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& apnType,
    /* [in] */ const String& apn,
    /* [in] */ const String& reason,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ const String& failCause)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyManager::ACTION_PRECISE_DATA_CONNECTION_STATE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::STATE_KEY, state);
    intent->PutExtra(IPhoneConstants::DATA_NETWORK_TYPE_KEY, networkType);
    if (reason != NULL) intent->PutExtra(IPhoneConstants::STATE_CHANGE_REASON_KEY, reason);
    if (apnType != NULL) intent->PutExtra(IPhoneConstants::DATA_APN_TYPE_KEY, apnType);
    if (apn != NULL) intent->PutExtra(IPhoneConstants::DATA_APN_KEY, apn);
    if (linkProperties != NULL) intent->PutExtra(IPhoneConstants::DATA_LINK_PROPERTIES_KEY, IParcelable::Probe(linkProperties));
    if (failCause != NULL) intent->PutExtra(IPhoneConstants::DATA_FAILURE_CAUSE_KEY, failCause);

    return mContext->SendBroadcastAsUser(intent, UserHandle::ALL,
            Manifest::permission::READ_PRECISE_PHONE_STATE);
}

Boolean CTelephonyRegistry::CheckNotifyPermission(
    /* [in] */ const String& method)
{
    Int32 perm;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::MODIFY_PHONE_STATE, &perm);
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }

    if (DBG) Slogger::D(TAG, "Modify Phone State Permission Denial: %s from pid=%d, uid=%d",
        method.string(), Binder::GetCallingPid(), Binder::GetCallingUid());
    return FALSE;
}

ECode CTelephonyRegistry::CheckListenerPermission(
    /* [in] */ Int32 events)
{
    String nullStr;
    if ((events & IPhoneStateListener::LISTEN_CELL_LOCATION) != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::ACCESS_COARSE_LOCATION, nullStr))

    }

    if ((events & IPhoneStateListener::LISTEN_CELL_INFO) != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::ACCESS_COARSE_LOCATION, nullStr))

    }

    if ((events & PHONE_STATE_PERMISSION_MASK) != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::READ_PHONE_STATE, nullStr))
    }

    if ((events & PRECISE_PHONE_STATE_PERMISSION_MASK) != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::READ_PRECISE_PHONE_STATE, nullStr))
    }

    if ((events & IPhoneStateListener::LISTEN_OEM_HOOK_RAW_EVENT) != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::READ_PRIVILEGED_PHONE_STATE, nullStr))
    }
    return NOERROR;
}

ECode CTelephonyRegistry::HandleRemoveListLocked()
{
    if (!mRemoveList.IsEmpty()) {
        List<AutoPtr<IBinder> >::Iterator it;
        for (it = mRemoveList.Begin(); it != mRemoveList.End(); ++it) {
            Remove(*it);
        }
        mRemoveList.Clear();
    }
    return NOERROR;
}

Boolean CTelephonyRegistry::ValidateEventsAndUserLocked(
    /* [in] */ Record* r,
    /* [in] */ Int32 events)
{
    Int32 foregroundUser;
    Int64 callingIdentity = Binder::ClearCallingIdentity();
    Boolean valid = FALSE;

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    helper->GetCurrentUser(&foregroundUser);
    valid = r->mCallerUid ==  foregroundUser && (r->mEvents & events) != 0;
    if (DBG | DBG_LOC) {
        Slogger::D(TAG, "ValidateEventsAndUserLocked: valid=%d r->mCallerUid=%d,"
            " foregroundUser=%d r->mEvents=%d events=%d",
            valid, r->mCallerUid, foregroundUser, r->mEvents, events);
    }

    Binder::RestoreCallingIdentity(callingIdentity);

    return valid;
}

Boolean CTelephonyRegistry::ValidatePhoneId(
    /* [in] */ Int32 phoneId)
{
    Boolean valid = (phoneId >= 0) && (phoneId < mNumPhones);
    if (VDBG) Slogger::D(TAG, "ValidatePhoneId: %d", valid);
    return valid;
}

void CTelephonyRegistry::Log(
    /* [in] */ const String& s)
{
    Slogger::D(TAG, s.string());
}

ECode CTelephonyRegistry::LogServiceStateChanged(
    /* [in] */ const String& s,
    /* [in] */ Int64 subId,
    /* [in] */ Int32 phoneId,
    /* [in] */ Elastos::Droid::Telephony::IServiceState* state)
{
    if (mLogSSC == NULL || mLogSSC->GetLength() == 0) {
        return NOERROR;
    }
    if ((*mLogSSC)[mNext] == NULL) {
        AutoPtr<LogSSC> ssc = new LogSSC();
        mLogSSC->Set(mNext, ssc);
    }
    AutoPtr<ITime> t;
    CTime::New((ITime**)&t);
    t->SetToNow();
    (*mLogSSC)[mNext]->Set(t, s, subId, phoneId, state);
    if (++mNext >= mLogSSC->GetLength()) {
        mNext = 0;
    }
    return NOERROR;
}

ECode CTelephonyRegistry::ToStringLogSSC(
    /* [in] */ const String& prompt)
{
    if (mLogSSC == NULL || mLogSSC->GetLength() == 0 || (mNext == 0 && (*mLogSSC)[mNext] == NULL)) {
        Slogger::D(TAG, "%s: mLogSSC is empty", prompt.string());
    }
    else {
        // There is at least one element
        Slogger::D(TAG, "%s: mLogSSC.length=%d next=%d",
            prompt.string(), mLogSSC->GetLength(), mNext);
        Int32 i =  mNext;
        if ((*mLogSSC)[i] == NULL) {
            // mLogSSC is not full so back to the beginning
            i = 0;
        }
        do {
            Slogger::D(TAG, TO_CSTR((*mLogSSC)[i]));
            if (++i >= mLogSSC->GetLength()) {
                i = 0;
            }
        } while (i != mNext);
        Slogger::D(TAG, "%s: ----------------", prompt.string());
    }
    return NOERROR;
}


}// Server
}// Droid
}// Elastos
