#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/internal/telephony/CRestrictedState.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCallTracker.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/internal/telephony/gsm/CGsmServiceStateTracker.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/ProxyController.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CCellInfoGsm.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/gsm/CGsmCellLocation.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::ProxyController;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CCellInfoGsm;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::Gsm::CGsmCellLocation;
using Elastos::Droid::Telephony::ICellIdentityGsm;
using Elastos::Droid::Telephony::ICellIdentityLte;
using Elastos::Droid::Telephony::ICellIdentityWcdma;
using Elastos::Droid::Telephony::ICellInfoGsm;
using Elastos::Droid::Telephony::ICellInfoLte;
using Elastos::Droid::Telephony::ICellInfoWcdma;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CGsmServiceStateTracker::TAG("GsmSST");
const Boolean CGsmServiceStateTracker::VDBG = TRUE;
const Int32 CGsmServiceStateTracker::PS_ENABLED;            // Access Control blocks data service
const Int32 CGsmServiceStateTracker::PS_DISABLED;           // Access Control enables data service
const Int32 CGsmServiceStateTracker::CS_ENABLED;            // Access Control blocks all voice/sms service
const Int32 CGsmServiceStateTracker::CS_DISABLED;           // Access Control enables all voice/sms service
const Int32 CGsmServiceStateTracker::CS_NORMAL_ENABLED;     // Access Control blocks normal voice/sms service
const Int32 CGsmServiceStateTracker::CS_EMERGENCY_ENABLED;  // Access Control blocks emergency call service
const Int32 CGsmServiceStateTracker::PS_NOTIFICATION;  // Id to update and cancel PS restricted
const Int32 CGsmServiceStateTracker::CS_NOTIFICATION;  // Id to update and cancel CS restricted
const Int32 CGsmServiceStateTracker::EVENT_ALL_DATA_DISCONNECTED;
const String CGsmServiceStateTracker::WAKELOCK_TAG("ServiceStateTracker");

CAR_INTERFACE_IMPL(CGsmServiceStateTracker, ServiceStateTracker, IGsmServiceStateTracker)

CAR_OBJECT_IMPL(CGsmServiceStateTracker)

ECode CGsmServiceStateTracker::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (!((CGSMPhone*)mHost->mPhone.Get())->mIsTheCurrentActivePhone) {
        // Logger::E(TAG, "Received Intent " + intent +
        //         " while being destroyed. Ignoring.");
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_LOCALE_CHANGED)) {
        // update emergency string whenever locale changed
        mHost->UpdateSpnDisplay();
    }
    else if (action.Equals(ACTION_RADIO_OFF)) {
        mHost->mAlarmSwitch = FALSE;
        AutoPtr<IDcTrackerBase> dcTracker = ((CGSMPhone*)mHost->mPhone.Get())->mDcTracker;
        mHost->PowerOffRadioSafely(dcTracker);
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::AutoTimeContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Logger::I("GsmServiceStateTracker", "Auto time state changed");
    mHost->RevertToNitzTime();
    return NOERROR;
}

ECode CGsmServiceStateTracker::AutoTimeZoneContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Logger::I("GsmServiceStateTracker", "Auto time zone state changed");
    mHost->RevertToNitzTimeZone();
    return NOERROR;
}

CGsmServiceStateTracker::CGsmServiceStateTracker()
    : mPreferredNetworkType(0)
    , mSavedTime(0)
    , mSavedAtTime(0)
    , mMaxDataCalls(1)
    , mNewMaxDataCalls(1)
    , mReasonDataDenied(-1)
    , mNewReasonDataDenied(-1)
    , mGsmRoaming(FALSE)
    , mDataRoaming(FALSE)
    , mEmergencyOnly(FALSE)
    , mNeedFixZoneAfterNitz(FALSE)
    , mZoneOffset(0)
    , mZoneDst(FALSE)
    , mZoneTime(0)
    , mGotCountryCode(FALSE)
    , mNitzUpdatedTime(FALSE)
    , mStartedGprsRegCheck(FALSE)
    , mReportedGprsNoReg(FALSE)
    , mCurShowPlmn(FALSE)
    , mCurShowSpn(FALSE)
{
    mIntentReceiver = new MyBroadcastReceiver(this);
    AutoPtr<IHandler> handler1;
    CHandler::New((IHandler**)&handler1);
    mAutoTimeObserver = new AutoTimeContentObserver(handler1, this);
    AutoPtr<IHandler> handler2;
    CHandler::New((IHandler**)&handler2);
    mAutoTimeZoneObserver = new AutoTimeZoneContentObserver(handler2, this);
}

CGsmServiceStateTracker::~CGsmServiceStateTracker()
{
    if(DBG) Log(String("finalize"));
}

ECode CGsmServiceStateTracker::constructor()
{
    return NOERROR;
}

ECode CGsmServiceStateTracker::constructor(
    /* [in] */ IGSMPhone* phone)
{
    AutoPtr<ICellInfoGsm> cig;
    CCellInfoGsm::New((ICellInfoGsm**)&cig);
    ServiceStateTracker::constructor(IPhoneBase::Probe(phone),
            ((CGSMPhone*)phone)->mCi, ICellInfo::Probe(cig));

    mPhone = phone;
    CGsmCellLocation::New((IGsmCellLocation**)&mCellLoc);
    CGsmCellLocation::New((IGsmCellLocation**)&mNewCellLoc);

    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_TAG, (IPowerManagerWakeLock**)&mWakeLock);

    mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForRadioStateChanged(this, EVENT_RADIO_STATE_CHANGED, NULL);

    mCi->RegisterForVoiceNetworkStateChanged(this, EVENT_NETWORK_STATE_CHANGED, NULL);
    mCi->SetOnNITZTime(this, EVENT_NITZ_TIME, NULL);
    mCi->SetOnRestrictedStateChanged(this, EVENT_RESTRICTED_STATE_CHANGED, NULL);

    // system setting property AIRPLANE_MODE_ON is set in Settings.
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Int32 airplaneMode;
    sg->GetInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &airplaneMode);
    mDesiredPowerState = ! (airplaneMode > 0);

    mCr = cr;
    AutoPtr<IUri> uri;
    sg->GetUriFor(ISettingsGlobal::AUTO_TIME, (IUri**)&uri);
    mCr->RegisterContentObserver(uri, TRUE, mAutoTimeObserver);

    uri = NULL;
    sg->GetUriFor(ISettingsGlobal::AUTO_TIME_ZONE, (IUri**)&uri);
    mCr->RegisterContentObserver(uri, TRUE, mAutoTimeZoneObserver);

    SetSignalStrengthDefaultValues();

    // Monitor locale change
    AutoPtr<IIntentFilter >filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    AutoPtr<IIntent> intent;
    ctx->RegisterReceiver(mIntentReceiver, filter, (IIntent**)&intent);

    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ACTION_RADIO_OFF);
    intent = NULL;
    ctx->RegisterReceiver(mIntentReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode CGsmServiceStateTracker::Dispose()
{
    CheckCorrectThread();
    Log(String("ServiceStateTracker dispose"));

    // Unregister for all events.
    mCi->UnregisterForAvailable(this);
    mCi->UnregisterForRadioStateChanged(this);
    mCi->UnregisterForVoiceNetworkStateChanged(this);
    if (mUiccApplcation != NULL) {
        mUiccApplcation->UnregisterForReady(this);
    }
    if (mIccRecords != NULL) {
        mIccRecords->UnregisterForRecordsLoaded(this);
    }
    mCi->UnSetOnRestrictedStateChanged(this);
    mCi->UnSetOnNITZTime(this);
    mCr->UnregisterContentObserver(mAutoTimeObserver);
    mCr->UnregisterContentObserver(mAutoTimeZoneObserver);

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->UnregisterReceiver(mIntentReceiver);
    ServiceStateTracker::Dispose();
    return NOERROR;
}

ECode CGsmServiceStateTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<ArrayOf<Int32> > ints;
    AutoPtr<ArrayOf<String> > strings;
    AutoPtr<IMessage> message;

    if (!((CGSMPhone*)mPhone.Get())->mIsTheCurrentActivePhone) {
        // Logger::E(TAG, "Received message " + msg +
        //         "[" + msg.what + "] while being destroyed. Ignoring.");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    if (VDBG) {
        Logger::E("CGsmServiceStateTracker", "HandleMessage, what:%d", what);
    }

    switch (what) {
        case EVENT_RADIO_AVAILABLE:
            //this is unnecessary
            //setPowerStateToDesired();
            break;

        case EVENT_SIM_READY: {
            // Set the network type, in case the radio does not restore it.
            Int32 pid;
            IPhone::Probe(mPhone)->GetPhoneId(&pid);
            AutoPtr<IContext> ctx;
            IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);

            AutoPtr<IPhoneFactory> pf;
            CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
            Int32 networkType;
            pf->CalculatePreferredNetworkType(ctx, pid, &networkType);

            mCi->SetPreferredNetworkType(networkType, NULL);

            AutoPtr<IResources> res;
            ctx->GetResources((IResources**)&res);
            Boolean skipRestoringSelection;
            res->GetBoolean(Elastos::Droid::R::bool_::skip_restoring_network_selection, &skipRestoringSelection);

            if (!skipRestoringSelection) {
                // restore the previous network selection.
                IPhoneBase::Probe(mPhone)->RestoreSavedNetworkSelection(NULL);
            }
            PollState();
            // Signal strength polling stops when radio is off
            QueueNextSignalStrengthPoll();
            break;
        }
        case EVENT_RADIO_STATE_CHANGED:
            // This will do nothing in the radio not
            // available case
            SetPowerStateToDesired();
            PollState();
            break;

        case EVENT_NETWORK_STATE_CHANGED:
            PollState();
            break;

        case EVENT_GET_SIGNAL_STRENGTH: {
            // This callback is called when signal strength is polled
            // all by itself

            ICommandsInterfaceRadioState state;
            mCi->GetRadioState(&state);
            if (state != RADIO_ON) {
                // Polling will continue when radio turns back on
                return NOERROR;
            }
            ar = (AsyncResult*)(IObject::Probe(obj));
            OnSignalStrengthResult(ar, TRUE);
            QueueNextSignalStrengthPoll();

            break;
        }
        case EVENT_GET_LOC_DONE:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iarray->GetLength(&size);
                AutoPtr<ArrayOf<String> > states = ArrayOf<String>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&((*states)[i]));
                }

                Int32 lac = -1;
                Int32 cid = -1;
                if (states->GetLength() >= 3) {
                    // try {
                    if ((*states)[1] != NULL && (*states)[1].GetLength() > 0) {
                        lac = StringUtils::ParseInt32((*states)[1], 16);
                    }
                    if ((*states)[2] != NULL && (*states)[2].GetLength() > 0) {
                        cid = StringUtils::ParseInt32((*states)[2], 16);
                    }
                    // } catch (NumberFormatException ex) {
                    //     Logger::W(TAG, "error parsing location: " + ex);
                    // }
                }
                mCellLoc->SetLacAndCid(lac, cid);
                ((CGSMPhone*)mPhone.Get())->NotifyLocationChanged();
            }

            // Release any temporary cell lock, which could have been
            // acquired to allow a single-shot location update.
            DisableSingleLocationUpdate();
            break;

        case EVENT_POLL_STATE_REGISTRATION:
        case EVENT_POLL_STATE_GPRS:
        case EVENT_POLL_STATE_OPERATOR:
        case EVENT_POLL_STATE_NETWORK_SELECTION_MODE:
            ar = (AsyncResult*)(IObject::Probe(obj));

            HandlePollStateResult(what, ar);
            break;

        case EVENT_POLL_SIGNAL_STRENGTH: {
            // Just poll signal strength...not part of pollState()
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_SIGNAL_STRENGTH, (IMessage**)&msg);
            mCi->GetSignalStrength(msg);
            break;
        }
        case EVENT_NITZ_TIME: {
            ar = (AsyncResult*)(IObject::Probe(obj));

            AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
            AutoPtr<IInterface> o;
            iarray->Get(0, (IInterface**)&o);
            String nitzString;
            ICharSequence::Probe(o)->ToString(&nitzString);
            o = NULL;
            iarray->Get(1, (IInterface**)&o);
            Int64 nitzReceiveTime;
            IInteger64::Probe(o)->GetValue(&nitzReceiveTime);

            SetTimeFromNITZString(nitzString, nitzReceiveTime);
            break;
        }
        case EVENT_SIGNAL_STRENGTH_UPDATE:
            // This is a notification from
            // CommandsInterface.setOnSignalStrengthUpdate

            ar = (AsyncResult*)(IObject::Probe(obj));

            // The radio is telling us about signal strength changes
            // we don't have to ask it
            mDontPollSignalStrength = TRUE;

            OnSignalStrengthResult(ar, TRUE);
            break;

        case EVENT_SIM_RECORDS_LOADED:
            Log(String("EVENT_SIM_RECORDS_LOADED: what=") + StringUtils::ToString(what));
            // Gsm doesn't support OTASP so its not needed
            IPhoneBase::Probe(mPhone)->NotifyOtaspChanged(OTASP_NOT_NEEDED);

            UpdatePhoneObject();
            UpdateSpnDisplay();
            break;

        case EVENT_LOCATION_UPDATES_ENABLED:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException == NULL) {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_LOC_DONE, NULL, (IMessage**)&msg);
                mCi->GetVoiceRegistrationState(msg);
            }
            break;

        case EVENT_SET_PREFERRED_NETWORK_TYPE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            // Don't care the result, only use for dereg network (COPS=2)
            ObtainMessage(EVENT_RESET_PREFERRED_NETWORK_TYPE, ar->mUserObj, (IMessage**)&message);
            mCi->SetPreferredNetworkType(mPreferredNetworkType, message);
            break;

        case EVENT_RESET_PREFERRED_NETWORK_TYPE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            if (ar->mUserObj != NULL) {
                AutoPtr<IMessage> msg = IMessage::Probe(ar->mUserObj);
                AsyncResult::ForMessage(msg)->mException = ar->mException;
                msg->SendToTarget();
            }
            break;

        case EVENT_GET_PREFERRED_NETWORK_TYPE: {
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                AutoPtr<IInterface> obj;
                iarray->Get(0, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&mPreferredNetworkType);
            }
            else {
                mPreferredNetworkType = IRILConstants::NETWORK_MODE_GLOBAL;
            }

            ObtainMessage(EVENT_SET_PREFERRED_NETWORK_TYPE, ar->mUserObj, (IMessage**)&message);
            Int32 toggledNetworkType = IRILConstants::NETWORK_MODE_GLOBAL;

            mCi->SetPreferredNetworkType(toggledNetworkType, message);
            break;
        }
        case EVENT_CHECK_REPORT_GPRS: {
            Int32 val1, val2;
            mSS->GetDataRegState(&val1);
            mSS->GetVoiceRegState(&val2);
            if (mSS != NULL && !IsGprsConsistent(val1, val2)) {

                // Can't register data service while voice service is ok
                // i.e. CREG is ok while CGREG is not
                // possible a network or baseband side error
                AutoPtr<ICellLocation> cl;
                IPhone::Probe(mPhone)->GetCellLocation((ICellLocation**)&cl);
                AutoPtr<IGsmCellLocation> loc = IGsmCellLocation::Probe(cl);
                // EventLog::WriteEvent(IEventLogTags::DATA_NETWORK_REGISTRATION_FAIL,
                //         mSS->GetOperatorNumeric(), loc != NULL ? loc->GetCid() : -1);
                mReportedGprsNoReg = TRUE;
            }
            mStartedGprsRegCheck = FALSE;
            break;
        }
        case EVENT_RESTRICTED_STATE_CHANGED:
            // This is a notification from
            // CommandsInterface.setOnRestrictedStateChanged

            if (DBG) Log(String("EVENT_RESTRICTED_STATE_CHANGED"));

            ar = (AsyncResult*)(IObject::Probe(obj));

            OnRestrictedStateChanged(ar);
            break;

        case EVENT_ALL_DATA_DISCONNECTED: {
            Int64 dds;
            SubscriptionManager::GetDefaultDataSubId(&dds);
            AutoPtr<IProxyController> pc;
            ProxyController::GetInstance((IProxyController**)&pc);
            pc->UnregisterForAllDataDisconnected(dds, this);
            {
                AutoLock lock(this);
                if (mPendingRadioPowerOffAfterDataOff) {
                    if (DBG) Log(String("EVENT_ALL_DATA_DISCONNECTED, turn radio off now."));
                    HangupAndPowerOff();
                    mPendingRadioPowerOffAfterDataOff = FALSE;
                }
                else {
                    Log(String("EVENT_ALL_DATA_DISCONNECTED is stale"));
                }
            }
            break;
        }
        case EVENT_CHANGE_IMS_STATE:
            if (DBG) Log(String("EVENT_CHANGE_IMS_STATE:"));

            SetPowerStateToDesired();
            break;

        default:
            ServiceStateTracker::HandleMessage(msg);
        break;
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::PollState()
{
    mPollingContext = ArrayOf<Int32>::Alloc(1);
    (*mPollingContext)[0] = 0;

    ICommandsInterfaceRadioState state;
    mCi->GetRadioState(&state);
    switch (state) {
        case RADIO_UNAVAILABLE:
            mNewSS->SetStateOutOfService();
            mNewCellLoc->SetStateInvalid();
            SetSignalStrengthDefaultValues();
            mGotCountryCode = FALSE;
            mNitzUpdatedTime = FALSE;
            PollStateDone();
        break;

        case RADIO_OFF:
            mNewSS->SetStateOff();
            mNewCellLoc->SetStateInvalid();
            SetSignalStrengthDefaultValues();
            mGotCountryCode = FALSE;
            mNitzUpdatedTime = FALSE;
            PollStateDone();

            /**
             * If iwlan feature is enabled then we do get
             * voice_network_change indication from RIL. At this moment we
             * dont know the current RAT since we are in Airplane mode.
             * We have to request for current registration state and hence
             * fallthrough to default case only if iwlan feature is
             * applicable.
             */
            if (!IsIwlanFeatureAvailable()) {
                /* fall-through */
                break;
            }

        default:
            // Issue all poll-related commands at once
            // then count down the responses, which
            // are allowed to arrive out-of-order

            AutoPtr<IArrayOf> iArray;
            CArrayOf::New(EIID_IInteger32, mPollingContext->GetLength(), (IArrayOf**)&iArray);
            for (Int32 i = 0; i < mPollingContext->GetLength(); i++) {
                iArray->Set(i, CoreUtils::Convert((*mPollingContext)[i]));
            }

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_POLL_STATE_OPERATOR, iArray, (IMessage**)&msg);
            mCi->GetOperator(msg);

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            ObtainMessage(EVENT_POLL_STATE_GPRS, iArray, (IMessage**)&msg);
            mCi->GetDataRegistrationState(msg);

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            ObtainMessage(EVENT_POLL_STATE_REGISTRATION, iArray, (IMessage**)&msg);
            mCi->GetVoiceRegistrationState(msg);

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            ObtainMessage(EVENT_POLL_STATE_NETWORK_SELECTION_MODE, iArray, (IMessage**)&msg);
            mCi->GetNetworkSelectionMode(msg);
        break;
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::GetCurrentDataConnectionState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSS->GetDataRegState(result);
}

ECode CGsmServiceStateTracker::IsConcurrentVoiceAndDataAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 rt;
    mSS->GetRilDataRadioTechnology(&rt);
    if (rt >= IServiceState::RIL_RADIO_TECHNOLOGY_UMTS) {
        *result = TRUE;
    }
    else {
        Int32 val;
        mSS->GetCssIndicator(&val);
        *result = val == 1;
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 lac, cid;
    mCellLoc->GetLac(&lac);
    mCellLoc->GetCid(&cid);
    if ((lac >= 0) && (cid >= 0)) {
        // if (DBG) Log(String("getCellLocation(): X good mCellLoc=") + mCellLoc);
        *result = ICellLocation::Probe(mCellLoc);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IList> list;
        GetAllCellInfo((IList**)&list);
        if (list != NULL) {
            // A hack to allow tunneling of LTE information via GsmCellLocation
            // so that older Network Location Providers can return some information
            // on LTE only networks, see bug 9228974.
            //
            // We'll search the return CellInfo array preferring GSM/WCDMA
            // data, but if there is none we'll tunnel the first LTE information
            // in the list.
            //
            // The tunnel'd LTE information is returned as follows:
            //   LAC = TAC field
            //   CID = CI field
            //   PSC = 0.
            AutoPtr<IGsmCellLocation> cellLocOther;
            CGsmCellLocation::New((IGsmCellLocation**)&cellLocOther);
            Int32 size;
            list->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                list->Get(i, (IInterface**)&obj);
                AutoPtr<ICellInfo> ci = ICellInfo::Probe(obj);

                AutoPtr<ICellInfoGsm> cellInfoGsm = ICellInfoGsm::Probe(ci);
                AutoPtr<ICellInfoWcdma> cellInfoWcdma = ICellInfoWcdma::Probe(ci);
                AutoPtr<ICellInfoLte> cellInfoLte = ICellInfoLte::Probe(ci);
                if (cellInfoGsm != NULL) {
                    AutoPtr<ICellIdentityGsm> cellIdentityGsm;
                    cellInfoGsm->GetCellIdentity((ICellIdentityGsm**)&cellIdentityGsm);
                    cellIdentityGsm->GetLac(&lac);
                    cellIdentityGsm->GetCid(&cid);
                    cellLocOther->SetLacAndCid(lac, cid);
                    Int32 psc;
                    cellIdentityGsm->GetPsc(&psc);
                    cellLocOther->SetPsc(psc);
                    // if (DBG) Log("getCellLocation(): X ret GSM info=" + cellLocOther);
                    *result = ICellLocation::Probe(cellLocOther);
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
                if (cellInfoWcdma != NULL) {
                    AutoPtr<ICellIdentityWcdma> cellIdentityWcdma;
                    cellInfoWcdma->GetCellIdentity((ICellIdentityWcdma**)&cellIdentityWcdma);
                    cellIdentityWcdma->GetLac(&lac);
                    cellIdentityWcdma->GetCid(&cid);
                    cellLocOther->SetLacAndCid(lac, cid);
                    Int32 psc;
                    cellIdentityWcdma->GetPsc(&psc);
                    cellLocOther->SetPsc(psc);
                    // if (DBG) Log("getCellLocation(): X ret WCDMA info=" + cellLocOther);
                    *result = ICellLocation::Probe(cellLocOther);
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
                else if (cellInfoLte != NULL &&
                        (((cellLocOther->GetLac(&lac), lac) < 0) || ((cellLocOther->GetCid(&cid), cid) < 0))) {
                    // We'll return the first good LTE info we get if there is no better answer
                    AutoPtr<ICellIdentityLte> cellIdentityLte;
                    cellInfoLte->GetCellIdentity((ICellIdentityLte**)&cellIdentityLte);

                    Int32 tac, ci;
                    cellIdentityLte->GetTac(&tac);
                    cellIdentityLte->GetCi(&ci);
                    if ((tac != Elastos::Core::Math::INT32_MAX_VALUE)
                            && (ci != Elastos::Core::Math::INT32_MAX_VALUE)) {
                        cellLocOther->SetLacAndCid(tac, ci);
                        cellLocOther->SetPsc(0);
                        if (DBG) {
                            // Log("getCellLocation(): possible LTE cellLocOther=" + cellLocOther);
                        }
                    }
                }
            }
            if (DBG) {
                // Log("getCellLocation(): X ret best answer cellLocOther=" + cellLocOther);
            }
            *result = ICellLocation::Probe(cellLocOther);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            if (DBG) {
                // Log("getCellLocation(): X empty mCellLoc and CellInfo mCellLoc=" + mCellLoc);
            }
            *result = ICellLocation::Probe(mCellLoc);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("GsmServiceStateTracker extends:"));
    ServiceStateTracker::Dump(fd, pw, args);
    // pw->Println(" mPhone=" + mPhone);
    // pw->Println(" mSS=" + mSS);
    // pw->Println(" mNewSS=" + mNewSS);
    // pw->Println(" mCellLoc=" + mCellLoc);
    // pw->Println(" mNewCellLoc=" + mNewCellLoc);
    // pw->Println(" mPreferredNetworkType=" + mPreferredNetworkType);
    // pw->Println(" mMaxDataCalls=" + mMaxDataCalls);
    // pw->Println(" mNewMaxDataCalls=" + mNewMaxDataCalls);
    // pw->Println(" mReasonDataDenied=" + mReasonDataDenied);
    // pw->Println(" mNewReasonDataDenied=" + mNewReasonDataDenied);
    // pw->Println(" mGsmRoaming=" + mGsmRoaming);
    // pw->Println(" mDataRoaming=" + mDataRoaming);
    // pw->Println(" mEmergencyOnly=" + mEmergencyOnly);
    // pw->Println(" mNeedFixZoneAfterNitz=" + mNeedFixZoneAfterNitz);
    // pw->Println(" mZoneOffset=" + mZoneOffset);
    // pw->Println(" mZoneDst=" + mZoneDst);
    // pw->Println(" mZoneTime=" + mZoneTime);
    // pw->Println(" mGotCountryCode=" + mGotCountryCode);
    // pw->Println(" mNitzUpdatedTime=" + mNitzUpdatedTime);
    // pw->Println(" mSavedTimeZone=" + mSavedTimeZone);
    // pw->Println(" mSavedTime=" + mSavedTime);
    // pw->Println(" mSavedAtTime=" + mSavedAtTime);
    // pw->Println(" mStartedGprsRegCheck=" + mStartedGprsRegCheck);
    // pw->Println(" mReportedGprsNoReg=" + mReportedGprsNoReg);
    // pw->Println(" mNotification=" + mNotification);
    // pw->Println(" mWakeLock=" + mWakeLock);
    // pw->Println(" mCurSpn=" + mCurSpn);
    // pw->Println(" mCurShowSpn=" + mCurShowSpn);
    // pw->Println(" mCurPlmn=" + mCurPlmn);
    // pw->Println(" mCurShowPlmn=" + mCurShowPlmn);
    return NOERROR;
}

ECode CGsmServiceStateTracker::PowerOffRadioSafely(
    /* [in] */ IDcTrackerBase* dcTracker)
{
    AutoLock lock(this);
    if (!mPendingRadioPowerOffAfterDataOff) {
        Int64 dds;
        SubscriptionManager::GetDefaultDataSubId(&dds);
        // To minimize race conditions we call cleanUpAllConnections on
        // both if else paths instead of before this isDisconnected test.
        Int64 sid;
        IPhone::Probe(mPhone)->GetSubId(&sid);
        AutoPtr<IProxyController> pc;
        ProxyController::GetInstance((IProxyController**)&pc);
        Boolean b, b1;
        if ((dcTracker->IsDisconnected(&b), b)
                && (dds == sid
                    || (dds != sid
                        && (pc->IsDataDisconnected(dds, &b1), b1)))) {
            // To minimize race conditions we do this after isDisconnected
            dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);
            if (DBG) Log(String("Data disconnected, turn off radio right away."));
            HangupAndPowerOff();
        }
        else {
            dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);
            if (dds != sid
                    && !(pc->IsDataDisconnected(dds, &b1), b1)) {
                if (DBG) Log(String("Data is active on DDS.  Wait for all data disconnect"));
                // Data is not disconnected on DDS. Wait for the data disconnect complete
                // before sending the RADIO_POWER off.
                pc->RegisterForAllDataDisconnected(dds, this,
                        EVENT_ALL_DATA_DISCONNECTED, NULL);
                mPendingRadioPowerOffAfterDataOff = TRUE;
            }
            AutoPtr<IMessage> msg = CMessage::Obtain(IHandler::Probe(this));
            msg->SetWhat(EVENT_SET_RADIO_POWER_OFF);
            msg->SetArg1(++mPendingRadioPowerOffAfterDataOffTag);
            Boolean b;
            if (SendMessageDelayed(msg, 30000, &b), b) {
                if (DBG) Log((String("Wait upto 30s for data to disconnect, then turn off radio.")));
                mPendingRadioPowerOffAfterDataOff = TRUE;
            }
            else {
                Log(String("Cannot send delayed Msg, turn off radio right away."));
                HangupAndPowerOff();
                mPendingRadioPowerOffAfterDataOff = FALSE;
            }
        }
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    if (mImsRegistrationOnOff && !registered) {
        if (mAlarmSwitch) {
            mImsRegistrationOnOff = registered;

            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
            AutoPtr<IAlarmManager> am = IAlarmManager::Probe(obj);
            am->Cancel(mRadioOffIntent);
            mAlarmSwitch = FALSE;

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_CHANGE_IMS_STATE, (IMessage**)&msg);
            Boolean b;
            SendMessage(msg, &b);
            return NOERROR;;
        }
    }
    mImsRegistrationOnOff = registered;
    return NOERROR;
}

ECode CGsmServiceStateTracker::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPhone::Probe(mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmServiceStateTracker::SetPowerStateToDesired()
{
    if (DBG) {
        ICommandsInterfaceRadioState state;
        mCi->GetRadioState(&state);
        Log(String("mDeviceShuttingDown = ") + StringUtils::BooleanToString(mDeviceShuttingDown));
        Log(String("mDesiredPowerState = ") + StringUtils::BooleanToString(mDesiredPowerState));
        Log(String("getRadioState = ") + StringUtils::ToString(state));
        Log(String("mPowerOffDelayNeed = ") + StringUtils::BooleanToString(mPowerOffDelayNeed));
        Log(String("mAlarmSwitch = ") + StringUtils::BooleanToString(mAlarmSwitch));
    }

    if (mAlarmSwitch) {
        if(DBG) Log(String("mAlarmSwitch == TRUE"));
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
        AutoPtr<IAlarmManager> am = IAlarmManager::Probe(obj);
        am->Cancel(mRadioOffIntent);
        mAlarmSwitch = FALSE;
    }

    // If we want it on and it's off, turn it on
    ICommandsInterfaceRadioState state;
    mCi->GetRadioState(&state);
    if (mDesiredPowerState && state == Elastos::Droid::Internal::Telephony::RADIO_OFF) {
        mCi->SetRadioPower(TRUE, NULL);
    }
// TODO: Need ICommandsInterfaceRadioState::IsOn
    // else if (!mDesiredPowerState && state->IsOn()) {
    //     // If it's on and available and we want it off gracefully
    //     if (mPowerOffDelayNeed) {
    //         if (mImsRegistrationOnOff && !mAlarmSwitch) {
    //             if(DBG) Log(String("mImsRegistrationOnOff == TRUE"));
    //             AutoPtr<IContext> context;
    //             IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    //             AutoPtr<IInterface> obj;
    //             context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    //             AutoPtr<IAlarmManager> am = IAlarmManager::Probe(obj);

    //             AutoPtr<IIntent> intent;
    //             CIntent::New(ACTION_RADIO_OFF, (IIntent**)&intent);
    //             CPendingIntent::GetBroadcast(context, 0, intent, 0, (IIntent**)&mRadioOffIntent);

    //             mAlarmSwitch = TRUE;
    //             if (DBG) Log(String("Alarm setting"));
    //             am->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
    //                     SystemClock::GetElapsedRealtime() + 3000, mRadioOffIntent);
    //         }
    //         else {
    //             AutoPtr<IDcTrackerBase> dcTracker = ((CGSMPhone*)mPhone.Get())->mDcTracker;
    //             PowerOffRadioSafely(dcTracker);
    //         }
    //     }
    //     else {
    //         AutoPtr<IDcTrackerBase> dcTracker = ((CGSMPhone*)mPhone.Get())->mDcTracker;
    //         PowerOffRadioSafely(dcTracker);
    //     }
    // }
// TODO: Need ICommandsInterfaceRadioState::IsAvailable
    // else if (mDeviceShuttingDown && state->IsAvailable()) {
    //     mCi->RequestShutdown(NULL);
    // }
    return NOERROR;
}

ECode CGsmServiceStateTracker::HangupAndPowerOff()
{
    // hang up all active voice calls
    Boolean b;
    if (((CGSMPhone*)mPhone.Get())->IsInCall(&b), b) {
        ICall::Probe(((CGsmCallTracker*)((CGSMPhone*)mPhone.Get())->mCT.Get())->mRingingCall)->HangupIfAlive();
        ICall::Probe(((CGsmCallTracker*)((CGSMPhone*)mPhone.Get())->mCT.Get())->mBackgroundCall)->HangupIfAlive();
        ICall::Probe(((CGsmCallTracker*)((CGSMPhone*)mPhone.Get())->mCT.Get())->mForegroundCall)->HangupIfAlive();
    }

    mCi->SetRadioPower(FALSE, NULL);
    return NOERROR;
}

ECode CGsmServiceStateTracker::UpdateSpnDisplay()
{
    // The values of plmn/showPlmn change in different scenarios.
    // 1) No service but emergency call allowed -> expected
    //    to show "Emergency call only"
    //    EXTRA_SHOW_PLMN = TRUE
    //    EXTRA_PLMN = "Emergency call only"

    // 2) No service at all --> expected to show "No service"
    //    EXTRA_SHOW_PLMN = TRUE
    //    EXTRA_PLMN = "No service"

    // 3) Normal operation in either home or roaming service
    //    EXTRA_SHOW_PLMN = depending on IccRecords rule
    //    EXTRA_PLMN = plmn

    // 4) No service due to power off, aka airplane mode
    //    EXTRA_SHOW_PLMN = FALSE
    //    EXTRA_PLMN = NULL

    AutoPtr<IIccRecords> iccRecords = mIccRecords;
    String plmn;
    Boolean showPlmn = FALSE;
    Int32 rule;
    if (iccRecords != NULL) {
        String str;
        mSS->GetOperatorNumeric(&str);
        iccRecords->GetDisplayRule(str, &rule);
    }
    else {
        rule = 0;
    }

    Int32 combinedRegState = GetCombinedRegState();
    if (combinedRegState == IServiceState::STATE_OUT_OF_SERVICE
            || combinedRegState == IServiceState::STATE_EMERGENCY_ONLY) {
        showPlmn = TRUE;
        AutoPtr<ICharSequence> cs;
        AutoPtr<IResources> res = CResources::GetSystem();
        if (mEmergencyOnly) {
            // No service but emergency call allowed
            res->GetText(Elastos::Droid::R::string::emergency_calls_only, (ICharSequence**)&cs);
            cs->ToString(&plmn);
        }
        else {
            // No service at all
            res->GetText(Elastos::Droid::R::string::lockscreen_carrier_default, (ICharSequence**)&cs);
            cs->ToString(&plmn);
        }
        if (DBG) Log(String("updateSpnDisplay: radio is on but out ") +
                "of service, set plmn='" + plmn + "'");
    }
    else if (combinedRegState == IServiceState::STATE_IN_SERVICE) {
        // In either home or roaming service
        mSS->GetOperatorAlphaLong(&plmn);
        showPlmn = !TextUtils::IsEmpty(plmn) &&
                ((rule & IIccRecords::SPN_RULE_SHOW_PLMN)
                        == IIccRecords::SPN_RULE_SHOW_PLMN);
    }
    else {
        // Power off state, such as airplane mode
        if (DBG)  {
            Logger::I("CGsmServiceStateTracker", "updateSpnDisplay: radio is off w/ showPlmn=%d plmn=%s",
                showPlmn, plmn.string());
        }
    }

    // The value of spn/showSpn are same in different scenarios.
    //    EXTRA_SHOW_SPN = depending on IccRecords rule
    //    EXTRA_SPN = spn
    String spn;
    if (iccRecords != NULL) {
        iccRecords->GetServiceProviderName(&spn);
    }
    else {
        spn = String("");
    }
    Boolean showSpn = !TextUtils::IsEmpty(spn)
            && ((rule & IIccRecords::SPN_RULE_SHOW_SPN)
                    == IIccRecords::SPN_RULE_SHOW_SPN);

    // Update SPN_STRINGS_UPDATED_ACTION IFF any value changes
    if (showPlmn != mCurShowPlmn
            || showSpn != mCurShowSpn
            || !spn.Equals(mCurSpn)
            || !plmn.Equals(mCurPlmn)) {
        if (DBG) {
            // Log(String.format("updateSpnDisplay: changed" +
            //         " sending intent rule=" + rule +
            //         " showPlmn='%b' plmn='%s' showSpn='%b' spn='%s'",
            //         showPlmn, plmn, showSpn, spn));
        }
        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutBooleanExtra(ITelephonyIntents::EXTRA_SHOW_SPN, showSpn);
        intent->PutExtra(ITelephonyIntents::EXTRA_SPN, spn);
        intent->PutBooleanExtra(ITelephonyIntents::EXTRA_SHOW_PLMN, showPlmn);
        intent->PutExtra(ITelephonyIntents::EXTRA_PLMN, plmn);
        Int32 id;
        IPhone::Probe(mPhone)->GetPhoneId(&id);
        SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, id);
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        context->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    }

    mCurShowSpn = showSpn;
    mCurShowPlmn = showPlmn;
    mCurSpn = spn;
    mCurPlmn = plmn;

    return NOERROR;
}

ECode CGsmServiceStateTracker::HandlePollStateResult(
    /* [in] */ Int32 what,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ArrayOf<Int32> > ints;
    AutoPtr<ArrayOf<String> > states;

    // Ignore stale requests from last poll
// TODO:
    // if (!Object::Equals(ar->mUserObj, mPollingContext)) return NOERROR;

    if (ar->mException != NULL) {
// TODO: Need ICommandException::Error
        // AutoPtr<ICommandExceptionError> err;

        // if (ICommandException::Probe(ar->mException) != NULL) {
        //     ICommandException::Probe(ar->mException)->GetCommandError((ICommandExceptionError**)&err);
        // }

        // if (err == ICommandExceptionError::RADIO_NOT_AVAILABLE) {
        //     // Radio has crashed or turned off
        //     CancelPollState();
        //     return NOERROR;
        // }

        // if (err != ICommandExceptionError::OP_NOT_ALLOWED_BEFORE_REG_NW) {
        //     Loge(String("RIL implementation has returned an error where it must succeed"));
        //              // + ar->mException);
        // }
    }
    else {
        // try {
        switch (what) {
            case EVENT_POLL_STATE_REGISTRATION: {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iarray->GetLength(&size);
                states = ArrayOf<String>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&((*states)[i]));
                }

                Int32 lac = -1;
                Int32 cid = -1;
                Int32 type = IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN;
                Int32 regState = IServiceState::RIL_REG_STATE_UNKNOWN;
                Int32 reasonRegStateDenied = -1;
                Int32 psc = -1;
                Int32 cssIndicator = 0;
                if (states->GetLength() > 0) {
                    // try {
                    regState = StringUtils::ParseInt32((*states)[0]);
                    if (states->GetLength() >= 3) {
                        if ((*states)[1] != NULL && (*states)[1].GetLength() > 0) {
                            lac = StringUtils::ParseInt32((*states)[1], 16);
                        }
                        if ((*states)[2] != NULL && (*states)[2].GetLength() > 0) {
                            cid = StringUtils::ParseInt32((*states)[2], 16);
                        }

                        // (*states)[3] (if present) is the current radio technology
                        if (states->GetLength() >= 4 && (*states)[3] != NULL) {
                            type = StringUtils::ParseInt32((*states)[3]);
                        }
                    }
                    if (states->GetLength() >= 7 && ((*states)[7] != NULL)) {
                        cssIndicator = StringUtils::ParseInt32((*states)[7]);
                    }
                    if (states->GetLength() > 14) {
                        if ((*states)[14] != NULL && (*states)[14].GetLength() > 0) {
                            psc = StringUtils::ParseInt32((*states)[14], 16);
                        }
                    }
                    // } catch (NumberFormatException ex) {
                    //     loge("error parsing RegistrationState: " + ex);
                    // }
                }

                mGsmRoaming = RegCodeIsRoaming(regState);
                mNewSS->SetState(RegCodeToServiceState(regState));
                mNewSS->SetRilVoiceRadioTechnology(type);
                mNewSS->SetCssIndicator(cssIndicator);

                if ((regState == IServiceState::RIL_REG_STATE_DENIED
                        || regState == IServiceState::RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED)
                        && (states->GetLength() >= 14)) {
                    // try {
                    Int32 rejCode = StringUtils::ParseInt32((*states)[13]);
                    // Check if rejCode is "Persistent location update reject",
                    if (rejCode == 10) {
                        Int64 sid;
                        IPhone::Probe(mPhone)->GetSubId(&sid);
                        Log(String(" Posting Managed roaming intent sub = ") + StringUtils::ToString(sid));
                        AutoPtr<IIntent> intent;
                        CIntent::New(ITelephonyIntents::ACTION_MANAGED_ROAMING_IND, (IIntent**)&intent);
                        intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, sid);
                        AutoPtr<IContext> context;
                        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
                        context->SendBroadcast(intent);
                    }
                    // } catch (NumberFormatException ex) {
                    //     loge("error parsing regCode: " + ex);
                    // }
                }

                AutoPtr<IContext> ctx;
                IPhone::Probe(mPhoneBase)->GetContext((IContext**)&ctx);
                AutoPtr<IResources> res;
                ctx->GetResources((IResources**)&res);
                Boolean isVoiceCapable;
                res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &isVoiceCapable);

                if ((regState == IServiceState::RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED
                     || regState == IServiceState::RIL_REG_STATE_NOT_REG_EMERGENCY_CALL_ENABLED
                     || regState == IServiceState::RIL_REG_STATE_SEARCHING_EMERGENCY_CALL_ENABLED
                     || regState == IServiceState::RIL_REG_STATE_UNKNOWN_EMERGENCY_CALL_ENABLED)
                     && isVoiceCapable) {
                    mEmergencyOnly = TRUE;
                }
                else {
                    mEmergencyOnly = FALSE;
                }

                // LAC and CID are -1 if not avail
                mNewCellLoc->SetLacAndCid(lac, cid);
                mNewCellLoc->SetPsc(psc);
                break;
            }

            case EVENT_POLL_STATE_GPRS: {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iarray->GetLength(&size);
                states = ArrayOf<String>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&((*states)[i]));
                }

                Int32 type = 0;
                Int32 regState = IServiceState::RIL_REG_STATE_UNKNOWN;
                mNewReasonDataDenied = -1;
                mNewMaxDataCalls = 1;
                if (states->GetLength() > 0) {
                    // try {
                    regState = StringUtils::ParseInt32((*states)[0]);

                    // (*states)[3] (if present) is the current radio technology
                    if (states->GetLength() >= 4 && (*states)[3] != NULL) {
                        type = StringUtils::ParseInt32((*states)[3]);
                    }
                    if ((states->GetLength() >= 5 ) &&
                            (regState == IServiceState::RIL_REG_STATE_DENIED)) {
                        mNewReasonDataDenied = StringUtils::ParseInt32((*states)[4]);
                    }
                    if (states->GetLength() >= 6) {
                        mNewMaxDataCalls = StringUtils::ParseInt32((*states)[5]);
                    }
                    // } catch (NumberFormatException ex) {
                    //     loge("error parsing GprsRegistrationState: " + ex);
                    // }
                }
                Int32 dataRegState = RegCodeToServiceState(regState);
                mNewSS->SetDataRegState(dataRegState);
                mDataRoaming = RegCodeIsRoaming(regState);
                mNewSS->SetRilDataRadioTechnology(type);
                if (DBG) {
                    Log(String("handlPollStateResultMessage: GsmSST setDataRegState=") + StringUtils::ToString(dataRegState)
                            + " regState=" + StringUtils::ToString(regState)
                            + " dataRadioTechnology=" + StringUtils::ToString(type));
                }
                break;
            }

            case EVENT_POLL_STATE_OPERATOR: {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iarray->GetLength(&size);
                AutoPtr<ArrayOf<String> > opNames = ArrayOf<String>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&((*opNames)[i]));
                }

                if (opNames != NULL && opNames->GetLength() >= 3) {
                    AutoPtr<IUiccCard> uc;
                    if (mUiccController != NULL)
                        mUiccController->GetUiccCard((IUiccCard**)&uc);
                    String brandOverride;
                    if (uc != NULL) {
                        uc->GetOperatorBrandOverride(&brandOverride);
                    }
                    else {
                        brandOverride = String("");
                    }
                    if (brandOverride != NULL) {
                        mNewSS->SetOperatorName(brandOverride, brandOverride, (*opNames)[2]);
                    }
                    else {
                        mNewSS->SetOperatorName((*opNames)[0], (*opNames)[1], (*opNames)[2]);
                    }
                }
                break;
            }

            case EVENT_POLL_STATE_NETWORK_SELECTION_MODE: {
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iarray->GetLength(&size);
                AutoPtr<ArrayOf<Int32> > ints = ArrayOf<Int32>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iarray->Get(i, (IInterface**)&obj);
                    IInteger32::Probe(obj)->GetValue(&((*ints)[i]));
                }

                mNewSS->SetIsManualSelection((*ints)[0] == 1);
                Boolean b;
                if (((*ints)[0] == 1) && ((mPhone->IsManualNetSelAllowed(&b), !b))) {
                    /*
                     * modem is currently in manual selection but manual
                     * selection is not allowed in the current mode so
                     * switch to automatic registration
                     */
                    IPhone::Probe(mPhone)->SetNetworkSelectionModeAutomatic(NULL);
                    Log(String(" Forcing Automatic Network Selection, ") +
                            "manual selection is not allowed");
                }
                break;
            }
        }
        // } catch (RuntimeException ex) {
        //     loge("Exception while polling service state. Probably malformed RIL response." + ex);
        // }
    }

    (*mPollingContext)[0]--;

    if ((*mPollingContext)[0] == 0) {
        /**
         * Since the roaming state of gsm service (from +CREG) and
         * data service (from +CGREG) could be different, the new SS
         * is set to roaming when either is TRUE.
         *
         * There are exceptions for the above rule.
         * The new SS is not set as roaming while gsm service reports
         * roaming but indeed it is same operator.
         * And the operator is considered non roaming.
         *
         * The test for the operators is to handle special roaming
         * agreements and MVNO's.
         */
        Boolean roaming = (mGsmRoaming || mDataRoaming);
        if (mGsmRoaming && !IsOperatorConsideredRoaming(mNewSS) &&
            (IsSameNamedOperators(mNewSS) || IsOperatorConsideredNonRoaming(mNewSS))) {
            roaming = FALSE;
        }
        mNewSS->SetRoaming(roaming);
        mNewSS->SetEmergencyOnly(mEmergencyOnly);
        PollStateDone();
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::OnUpdateIccAvailability()
{
    if (mUiccController == NULL ) {
        return NOERROR;
    }

    AutoPtr<IUiccCardApplication> newUiccApplication = GetUiccCardApplication();

    if (mUiccApplcation != newUiccApplication) {
        if (mUiccApplcation != NULL) {
            Log(String("Removing stale icc objects."));
            mUiccApplcation->UnregisterForReady(this);
            if (mIccRecords != NULL) {
                mIccRecords->UnregisterForRecordsLoaded(this);
            }
            mIccRecords = NULL;
            mUiccApplcation = NULL;
        }
        if (newUiccApplication != NULL) {
            Log(String("New card found"));
            mUiccApplcation = newUiccApplication;
            mUiccApplcation->GetIccRecords((IIccRecords**)&mIccRecords);
            mUiccApplcation->RegisterForReady(this, EVENT_SIM_READY, NULL);
            if (mIccRecords != NULL) {
                mIccRecords->RegisterForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
            }
        }
    }
    return NOERROR;
}

ECode CGsmServiceStateTracker::Log(
    /* [in] */ const String& s)
{
    return Logger::D(TAG, String("[GsmSST] ") + s);
}

ECode CGsmServiceStateTracker::Loge(
    /* [in] */ const String& s)
{
    return Logger::E(TAG, String("[GsmSST] ") + s);
}

Int32 CGsmServiceStateTracker::GetCombinedRegState()
{
    Int32 regState;
    mSS->GetVoiceRegState(&regState);
    Int32 dataRegState;
    mSS->GetDataRegState(&dataRegState);

    if ((regState == IServiceState::STATE_OUT_OF_SERVICE)
            && (dataRegState == IServiceState::STATE_IN_SERVICE)) {
        Log(String("getCombinedRegState: return STATE_IN_SERVICE as Data is in service"));
        regState = dataRegState;
    }

    return regState;
}

void CGsmServiceStateTracker::SetSignalStrengthDefaultValues()
{
    mSignalStrength = NULL;
    CSignalStrength::New(TRUE, (ISignalStrength**)&mSignalStrength);
}

void CGsmServiceStateTracker::PollStateDone()
{
    if (DBG) {
        // Log("Poll ServiceState done: " +
        //     " oldSS=[" + mSS + "] newSS=[" + mNewSS + "]" +
        //     " oldMaxDataCalls=" + mMaxDataCalls +
        //     " mNewMaxDataCalls=" + mNewMaxDataCalls +
        //     " oldReasonDataDenied=" + mReasonDataDenied +
        //     " mNewReasonDataDenied=" + mNewReasonDataDenied);
    }

    Boolean b;
    if (Build::IS_DEBUGGABLE && (SystemProperties::GetBoolean(PROP_FORCE_ROAMING, FALSE, &b), b)) {
        mNewSS->SetRoaming(TRUE);
    }

    UseDataRegStateForDataOnlyDevices();

    Int32 voiceRegState, newVoiceRegState;
    mSS->GetVoiceRegState(&voiceRegState);
    mNewSS->GetVoiceRegState(&newVoiceRegState);

    Int32 dataRegState, newDataRegState;
    mSS->GetDataRegState(&dataRegState);
    mNewSS->GetDataRegState(&newDataRegState);

    Int32 rilVoiceRadioTechnology, newRilVoiceRadioTechnology;
    mSS->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
    mNewSS->GetRilVoiceRadioTechnology(&newRilVoiceRadioTechnology);

    Int32 rilDataRadioTechnology, newRilDataRadioTechnology;
    mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    mNewSS->GetRilDataRadioTechnology(&newRilDataRadioTechnology);

    Boolean hasRegistered =
        voiceRegState != IServiceState::STATE_IN_SERVICE
        && newVoiceRegState == IServiceState::STATE_IN_SERVICE;

    Boolean hasDeregistered =
        voiceRegState == IServiceState::STATE_IN_SERVICE
        && newVoiceRegState != IServiceState::STATE_IN_SERVICE;

    Boolean hasGprsAttached =
            dataRegState != IServiceState::STATE_IN_SERVICE
            && newDataRegState == IServiceState::STATE_IN_SERVICE;

    Boolean hasGprsDetached =
            dataRegState == IServiceState::STATE_IN_SERVICE
            && newDataRegState != IServiceState::STATE_IN_SERVICE;

    Boolean hasDataRegStateChanged =
            dataRegState != newDataRegState;

    Boolean hasVoiceRegStateChanged =
            voiceRegState != newVoiceRegState;

    Boolean hasRilVoiceRadioTechnologyChanged =
            rilVoiceRadioTechnology != newRilVoiceRadioTechnology;

    Boolean hasRilDataRadioTechnologyChanged =
            rilDataRadioTechnology != newRilDataRadioTechnology;

    Boolean hasChanged = (IObject::Probe(mNewSS)->Equals(mSS, &b), !b);

    Boolean b1, b2;
    Boolean hasRoamingOn = (mSS->GetRoaming(&b1), !b1) && (mNewSS->GetRoaming(&b2), b2);

    Boolean hasRoamingOff = (mSS->GetRoaming(&b1), b1) && (mNewSS->GetRoaming(&b2), !b2);

    Boolean hasLocationChanged = (IObject::Probe(mNewCellLoc)->Equals(mCellLoc, &b), !b);

    Int32 val1, val2;
    Boolean needNotifyData = ((mSS->GetCssIndicator(&val1), val1) != (mNewSS->GetCssIndicator(&val2), val2));

    ICommandsInterfaceRadioState rs;
    mCi->GetRadioState(&rs);
    if (rs == Elastos::Droid::Internal::Telephony::RADIO_OFF) {
        Boolean resetIwlanRatVal = FALSE;
        Log(String("set service state as POWER_OFF"));
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == newRilDataRadioTechnology)) {
            // Log("pollStateDone: mNewSS = " + mNewSS);
            Log(String("pollStateDone: reset iwlan RAT value"));
            resetIwlanRatVal = TRUE;
        }
        mNewSS->SetStateOff();
        if (resetIwlanRatVal) {
            mNewSS->SetRilDataRadioTechnology(IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN);
            // Log("pollStateDone: mNewSS = " + mNewSS);
            resetIwlanRatVal = FALSE;
        }
    }

    // Add an event log when connection state changes
    if (hasVoiceRegStateChanged || hasDataRegStateChanged) {
        // EventLog.writeEvent(EventLogTags.GSM_SERVICE_STATE_CHANGE,
        //     voiceRegState, dataRegState,
        //     newVoiceRegState, newDataRegState);
    }

    // Add an event log when network type switched
    // TODO: we may add filtering to reduce the event logged,
    // i.e. check preferred network setting, only switch to 2G, etc
    if (hasRilVoiceRadioTechnologyChanged) {
        Int32 cid = -1;
        AutoPtr<IGsmCellLocation> loc = mNewCellLoc;
        if (loc != NULL) loc->GetCid(&cid);
        // NOTE: this code was previously located after mSS and mNewSS are swapped, so
        // existing logs were incorrectly using the new state for "network_from"
        // and STATE_OUT_OF_SERVICE for "network_to". To avoid confusion, use a new log tag
        // to record the correct states.
        // EventLog.writeEvent(EventLogTags.GSM_RAT_SWITCHED_NEW, cid,
        //         rilVoiceRadioTechnology,
        //         newRilVoiceRadioTechnology);
        if (DBG) {
            // Log("RAT switched "
            //         + ServiceState.rilRadioTechnologyToString(mSS.getRilVoiceRadioTechnology())
            //         + " -> "
            //         + ServiceState.rilRadioTechnologyToString(
            //                 mNewSS.getRilVoiceRadioTechnology()) + " at cell " + cid);
        }
    }

    // swap mSS and mNewSS to put new state in mSS
    AutoPtr<IServiceState> tss = mSS;
    mSS = mNewSS;
    mNewSS = tss;
    // clean slate for next time
    mNewSS->SetStateOutOfService();

    // swap mCellLoc and mNewCellLoc to put new state in mCellLoc
    AutoPtr<IGsmCellLocation> tcl = mCellLoc;
    mCellLoc = mNewCellLoc;
    mNewCellLoc = tcl;

    mReasonDataDenied = mNewReasonDataDenied;
    mMaxDataCalls = mNewMaxDataCalls;

    if (hasRilVoiceRadioTechnologyChanged) {
        UpdatePhoneObject();
    }

    if (hasRilDataRadioTechnologyChanged) {
        mSS->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
        String str;
        CServiceState::RilRadioTechnologyToString(rilVoiceRadioTechnology, &str);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE, str);

        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            Log(String("pollStateDone: IWLAN enabled"));
        }

    }

    if (hasRegistered) {
        mNetworkAttachedRegistrants->NotifyRegistrants();

        if (DBG) {
            // Log("pollStateDone: registering current mNitzUpdatedTime=" +
            //         mNitzUpdatedTime + " changing to FALSE");
        }
        mNitzUpdatedTime = FALSE;
    }

    if (hasChanged) {
        String operatorNumeric;

        UpdateSpnDisplay();

        String str;
        mSS->GetOperatorAlphaLong(&str);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ALPHA, str);

        String prevOperatorNumeric;
        SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, String(""), &prevOperatorNumeric);
        mSS->GetOperatorNumeric(&operatorNumeric);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        UpdateCarrierMccMncConfiguration(operatorNumeric,
                prevOperatorNumeric, context);
        if (operatorNumeric == NULL) {
            if (DBG) Log(String("operatorNumeric is NULL"));
            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""));
            mGotCountryCode = FALSE;
            mNitzUpdatedTime = FALSE;
        }
        else {
            String iso = String("");
            String mcc = String("");
            // try{
            mcc = operatorNumeric.Substring(0, 3);
            iso = MccTable::CountryCodeForMcc(StringUtils::ParseInt32(mcc));
            // } catch ( NumberFormatException ex){
            //     loge("pollStateDone: countryCodeForMcc error" + ex);
            // } catch ( StringIndexOutOfBoundsException ex) {
            //     loge("pollStateDone: countryCodeForMcc error" + ex);
            // }

            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, iso);
            mGotCountryCode = TRUE;

            AutoPtr<ITimeZone> zone;

            if (!mNitzUpdatedTime && !mcc.Equals("000") && !TextUtils::IsEmpty(iso) &&
                    GetAutoTimeZone()) {

                // Test both paths if ignore nitz is TRUE
                Boolean b;
                SystemProperties::GetBoolean(
                        ITelephonyProperties::PROPERTY_IGNORE_NITZ, FALSE, &b);
                Boolean testOneUniqueOffsetPath = b &&
                        ((SystemClock::GetUptimeMillis() & 1) == 0);

                Logger::E("CGsmServiceStateTracker", "TODO GetTimeZonesWithUniqueOffsets is not ready!");
                AutoPtr<ArrayOf<ITimeZone *> > uniqueZones;//TODO  = TimeUtils::GetTimeZonesWithUniqueOffsets(iso);
                Int32 size = 0;
                //TODO size = uniqueZones->GetLength();
                if ((size == 1) || testOneUniqueOffsetPath) {
                    zone = (*uniqueZones)[0];
                    if (DBG) {
                       // Log("pollStateDone: no nitz but one TZ for iso-cc=" + iso +
                       //         " with zone.getID=" + zone.getID() +
                       //         " testOneUniqueOffsetPath=" + testOneUniqueOffsetPath);
                    }
                    String str;
                    zone->GetID(&str);
                    SetAndBroadcastNetworkSetTimeZone(str);
                }
                else {
                    if (DBG) {
                        // Log("pollStateDone: there are " + uniqueZones.size() +
                        //     " unique offsets for iso-cc='" + iso +
                        //     " testOneUniqueOffsetPath=" + testOneUniqueOffsetPath +
                        //     "', do nothing");
                    }
                }
            }

            Logger::E("CGsmServiceStateTracker", "TODO ShouldFixTimeZoneNow is not ready!");
            if (FALSE/*ShouldFixTimeZoneNow(IPhoneBase::Probe(mPhone), operatorNumeric, prevOperatorNumeric,
                    mNeedFixZoneAfterNitz)*/) {
                // If the offset is (0, FALSE) and the timezone property
                // is set, use the timezone property rather than
                // GMT.
                String zoneName;
                SystemProperties::Get(TIMEZONE_PROPERTY, &zoneName);
                if (DBG) {
                    // Log("pollStateDone: fix time zone zoneName='" + zoneName +
                    //     "' mZoneOffset=" + mZoneOffset + " mZoneDst=" + mZoneDst +
                    //     " iso-cc='" + iso +
                    //     "' iso-cc-idx=" + Arrays.binarySearch(GMT_COUNTRY_CODES, iso));
                }

                if (iso.Equals("") && mNeedFixZoneAfterNitz) {
                    // Country code not found.  This is likely a test network.
                    // Get a TimeZone based only on the NITZ parameters (best guess).
                    zone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
                    if (DBG) Log(String("pollStateDone: using NITZ TimeZone"));
                }
                else {
                    // "(mZoneOffset == 0) && (mZoneDst == FALSE) &&
                    //  (Arrays.binarySearch(GMT_COUNTRY_CODES, iso) < 0)"
                    // means that we received a NITZ string telling
                    // it is in GMT+0 w/ DST time zone
                    // BUT iso tells is NOT, e.g, a wrong NITZ reporting
                    // local time w/ 0 offset.
                    Int32 index;
                    if ((mZoneOffset == 0) && (mZoneDst == FALSE) &&
                        (zoneName != NULL) && (zoneName.GetLength() > 0) &&
                        ((Arrays::BinarySearch(GMT_COUNTRY_CODES, iso, &index), index) < 0)) {
                        AutoPtr<ITimeZoneHelper> helper;
                        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
                        helper->GetDefault((ITimeZone**)&zone);
                        if (mNeedFixZoneAfterNitz) {
                            // For wrong NITZ reporting local time w/ 0 offset,
                            // need adjust time to reflect default timezone setting
                            AutoPtr<ISystem> system;
                            CSystem::AcquireSingleton((ISystem**)&system);
                            Int64 ctm;
                            system->GetCurrentTimeMillis(&ctm);
                            Int32 tzOffset;
                            zone->GetOffset(ctm, &tzOffset);
                            if (DBG) {
                                // Log("pollStateDone: tzOffset=" + tzOffset + " ltod=" +
                                //         TimeUtils.logTimeOfDay(ctm));
                            }
                            if (GetAutoTime()) {
                                Int64 adj = ctm - tzOffset;
                                // if (DBG) Log("pollStateDone: adj ltod=" +
                                //         TimeUtils.logTimeOfDay(adj));
                                SetAndBroadcastNetworkSetTime(adj);
                            }
                            else {
                                // Adjust the saved NITZ time to account for tzOffset.
                                mSavedTime = mSavedTime - tzOffset;
                            }
                        }
                        if (DBG) Log(String("pollStateDone: using default TimeZone"));
                    }
                    else {
                        zone = TimeUtils::GetTimeZone(mZoneOffset, mZoneDst, mZoneTime, iso);
                        if (DBG) Log(String("pollStateDone: using getTimeZone(off, dst, time, iso)"));
                    }
                }

                mNeedFixZoneAfterNitz = FALSE;

                if (zone != NULL) {
                    String str;
                    zone->GetID(&str);
                    Log(String("pollStateDone: zone != NULL zone.getID=") + str);
                    if (GetAutoTimeZone()) {
                        SetAndBroadcastNetworkSetTimeZone(str);
                    }
                    SaveNitzTimeZone(str);
                }
                else {
                    Log(String("pollStateDone: zone == NULL"));
                }
            }
        }

        Boolean b;
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISROAMING,
            (mSS->GetRoaming(&b), b) ? String("TRUE") : String("FALSE"));

        ((CGSMPhone*)mPhone.Get())->NotifyServiceStateChanged(mSS);
    }

    // First notify detached, then rat changed, then attached - that's the way it
    // happens in the modem.
    // Behavior of recipients (DcTracker, for instance) depends on this sequence
    // since DcTracker reloads profiles on "rat_changed" notification and sets up
    // data call on "attached" notification.
    if (hasGprsDetached) {
        mDetachedRegistrants->NotifyRegistrants();
    }

    if (hasDataRegStateChanged || hasRilDataRadioTechnologyChanged) {
        NotifyDataRegStateRilRadioTechnologyChanged();

        mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            IPhoneBase::Probe(mPhone)->NotifyDataConnection(IPhone::REASON_IWLAN_AVAILABLE);
            needNotifyData =  FALSE;
            mIwlanRatAvailable = TRUE;
        }
        else {
            ProcessIwlanToWwanTransition(mSS);
            needNotifyData = TRUE;
            mIwlanRatAvailable = FALSE;
        }
    }

    if (needNotifyData) {
        IPhoneBase::Probe(mPhone)->NotifyDataConnection(String(NULL));
    }

    if (hasGprsAttached) {
        mAttachedRegistrants->NotifyRegistrants();
    }

    if (hasRoamingOn) {
        mRoamingOnRegistrants->NotifyRegistrants();
    }

    if (hasRoamingOff) {
        mRoamingOffRegistrants->NotifyRegistrants();
    }

    if (hasLocationChanged) {
        ((CGSMPhone*)mPhone.Get())->NotifyLocationChanged();
    }

    mSS->GetDataRegState(&dataRegState);
    mSS->GetVoiceRegState(&voiceRegState);
    if (! IsGprsConsistent(dataRegState, voiceRegState)) {
        if (!mStartedGprsRegCheck && !mReportedGprsNoReg) {
            mStartedGprsRegCheck = TRUE;

            AutoPtr<IContext> ctx;
            IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
            AutoPtr<IContentResolver> cr;
            ctx->GetContentResolver((IContentResolver**)&cr);
            Int32 check_period;
            Settings::Global::GetInt32(
                    cr,
                    ISettingsGlobal::GPRS_REGISTER_CHECK_PERIOD_MS,
                    DEFAULT_GPRS_CHECK_PERIOD_MILLIS, &check_period);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_CHECK_REPORT_GPRS, (IMessage**)&msg);
            Boolean b;
            SendMessageDelayed(msg, check_period, &b);
        }
    }
    else {
        mReportedGprsNoReg = FALSE;
    }
    // TODO: Add GsmCellIdenity updating, see CdmaLteServiceStateTracker.
}

Boolean CGsmServiceStateTracker::IsGprsConsistent(
    /* [in] */ Int32 dataRegState,
    /* [in] */ Int32 voiceRegState)
{
    return !((voiceRegState == IServiceState::STATE_IN_SERVICE) &&
            (dataRegState != IServiceState::STATE_IN_SERVICE));
}

AutoPtr<ITimeZone> CGsmServiceStateTracker::GetNitzTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean dst,
    /* [in] */ Int64 when)
{
    AutoPtr<ITimeZone> guess = FindTimeZone(offset, dst, when);
    if (guess == NULL) {
        // Couldn't find a proper timezone.  Perhaps the DST data is wrong.
        guess = FindTimeZone(offset, !dst, when);
    }
    String str;
    if (DBG) Log(String("getNitzTimeZone returning ") +
            (guess == NULL ? String(NULL) : (guess->GetID(&str), str)));
    return guess;
}

AutoPtr<ITimeZone> CGsmServiceStateTracker::FindTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean dst,
    /* [in] */ Int64 when)
{
    Int32 rawOffset = offset;
    if (dst) {
        rawOffset -= 3600000;
    }
    AutoPtr<ArrayOf<String> > zones;
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    helper->GetAvailableIDs(rawOffset, (ArrayOf<String>**)&zones);
    AutoPtr<ITimeZone> guess;
    AutoPtr<IDate> d;
    CDate::New(when, (IDate**)&d);

    for (Int32 i = 0; i < zones->GetLength(); i++) {
        String zone = (*zones)[i];
        AutoPtr<ITimeZone> tz;
        helper->GetTimeZone(zone, (ITimeZone**)&tz);
        Int32 val;
        Boolean b;
        if ((tz->GetOffset(when, &val), val) == offset &&
            (tz->InDaylightTime(d, &b), b) == dst) {
            guess = tz;
            break;
        }
    }

    return guess;
}

void CGsmServiceStateTracker::QueueNextSignalStrengthPoll()
{
    if (mDontPollSignalStrength) {
        // The radio is telling us about signal strength changes
        // we don't have to ask it
        return;
    }

    AutoPtr<IMessage> msg;

    ObtainMessage((IMessage**)&msg);
    msg->SetWhat(EVENT_POLL_SIGNAL_STRENGTH);

    Int64 nextTime;

    // TODO Don't poll signal strength if screen is off
    Boolean b;
    SendMessageDelayed(msg, POLL_PERIOD_MILLIS, &b);
}

void CGsmServiceStateTracker::OnRestrictedStateChanged(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<IRestrictedState> newRs;
    CRestrictedState::New((IRestrictedState**)&newRs);

    // if (DBG) Log(String("onRestrictedStateChanged: E rs ")+ mRestrictedState);

    if (ar->mException == NULL) {
        AutoPtr<IArrayOf> iarray = IArrayOf::Probe(ar->mResult);
        AutoPtr<IInterface> obj;
        iarray->Get(0, (IInterface**)&obj);
        Int32 state;
        IInteger32::Probe(obj)->GetValue(&state);

        newRs->SetCsEmergencyRestricted(
                ((state & IRILConstants::RIL_RESTRICTED_STATE_CS_EMERGENCY) != 0) ||
                ((state & IRILConstants::RIL_RESTRICTED_STATE_CS_ALL) != 0) );
        //ignore the normal call and data restricted state before SIM READY
        AppState appState;
        if (mUiccApplcation != NULL && (mUiccApplcation->GetState(&appState), appState) == Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_READY) {
            newRs->SetCsNormalRestricted(
                    ((state & IRILConstants::RIL_RESTRICTED_STATE_CS_NORMAL) != 0) ||
                    ((state & IRILConstants::RIL_RESTRICTED_STATE_CS_ALL) != 0) );
            newRs->SetPsRestricted(
                    (state & IRILConstants::RIL_RESTRICTED_STATE_PS_ALL)!= 0);
        }

        // if (DBG) Log(String("onRestrictedStateChanged: new rs ")+ newRs);

        Boolean b, b1, b2;
        if ((mRestrictedState->IsPsRestricted(&b1), !b1) && (newRs->IsPsRestricted(&b2), b2)) {
            mPsRestrictEnabledRegistrants->NotifyRegistrants();
            SetNotification(PS_ENABLED);
        }
        else if ((mRestrictedState->IsPsRestricted(&b1), b1) && (newRs->IsPsRestricted(&b2), !b2)) {
            mPsRestrictDisabledRegistrants->NotifyRegistrants();
            SetNotification(PS_DISABLED);
        }

        /**
         * There are two kind of cs restriction, normal and emergency. So
         * there are 4 x 4 combinations in current and new restricted states
         * and we only need to notify when state is changed.
         */
        if (mRestrictedState->IsCsRestricted(&b), b) {
            if (newRs->IsCsRestricted(&b), !b) {
                // remove all restriction
                SetNotification(CS_DISABLED);
            }
            else if (newRs->IsCsNormalRestricted(&b), !b) {
                // remove normal restriction
                SetNotification(CS_EMERGENCY_ENABLED);
            }
            else if (newRs->IsCsEmergencyRestricted(&b), !b) {
                // remove emergency restriction
                SetNotification(CS_NORMAL_ENABLED);
            }
        }
        else if ((mRestrictedState->IsCsEmergencyRestricted(&b1), b1) &&
                (mRestrictedState->IsCsNormalRestricted(&b2), !b2)) {
            if (newRs->IsCsRestricted(&b), !b) {
                // remove all restriction
                SetNotification(CS_DISABLED);
            }
            else if (newRs->IsCsRestricted(&b), b) {
                // enable all restriction
                SetNotification(CS_ENABLED);
            }
            else if (newRs->IsCsNormalRestricted(&b), b) {
                // remove emergency restriction and enable normal restriction
                SetNotification(CS_NORMAL_ENABLED);
            }
        }
        else if ((mRestrictedState->IsCsEmergencyRestricted(&b1), b1) &&
                (mRestrictedState->IsCsNormalRestricted(&b2), b2)) {
            if (newRs->IsCsRestricted(&b), !b) {
                // remove all restriction
                SetNotification(CS_DISABLED);
            }
            else if (newRs->IsCsRestricted(&b), b) {
                // enable all restriction
                SetNotification(CS_ENABLED);
            }
            else if (newRs->IsCsEmergencyRestricted(&b), b) {
                // remove normal restriction and enable emergency restriction
                SetNotification(CS_EMERGENCY_ENABLED);
            }
        }
        else {
            if (newRs->IsCsRestricted(&b), b) {
                // enable all restriction
                SetNotification(CS_ENABLED);
            }
            else if (newRs->IsCsEmergencyRestricted(&b), b) {
                // enable emergency restriction
                SetNotification(CS_EMERGENCY_ENABLED);
            }
            else if (newRs->IsCsNormalRestricted(&b), b) {
                // enable normal restriction
                SetNotification(CS_NORMAL_ENABLED);
            }
        }

        mRestrictedState = newRs;
    }
    // Log(String("onRestrictedStateChanged: X rs ")+ mRestrictedState);
}

Int32 CGsmServiceStateTracker::RegCodeToServiceState(
    /* [in] */ Int32 code)
{
    switch (code) {
        case 0:
        case 2: // 2 is "searching"
        case 3: // 3 is "registration denied"
        case 4: // 4 is "unknown" no vaild in current baseband
        case 10:// same as 0, but indicates that emergency call is possible.
        case 12:// same as 2, but indicates that emergency call is possible.
        case 13:// same as 3, but indicates that emergency call is possible.
        case 14:// same as 4, but indicates that emergency call is possible.
            return IServiceState::STATE_OUT_OF_SERVICE;

        case 1:
            return IServiceState::STATE_IN_SERVICE;

        case 5:
            // in service, roam
            return IServiceState::STATE_IN_SERVICE;

        default:
            Loge(String("regCodeToServiceState: unexpected service state ") + StringUtils::ToString(code));
            return IServiceState::STATE_OUT_OF_SERVICE;
    }
}

Boolean CGsmServiceStateTracker::RegCodeIsRoaming(
    /* [in] */ Int32 code)
{
    return IServiceState::RIL_REG_STATE_ROAMING == code;
}

Boolean CGsmServiceStateTracker::IsSameNamedOperators(
    /* [in] */ IServiceState* s)
{
    String spn;
    GetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, String("empty"), &spn);

    String onsl;
    s->GetOperatorAlphaLong(&onsl);
    String onss;
    s->GetOperatorAlphaShort(&onss);

    Boolean equalsOnsl = onsl != NULL && spn.Equals(onsl);
    Boolean equalsOnss = onss != NULL && spn.Equals(onss);

    return CurrentMccEqualsSimMcc(s) && (equalsOnsl || equalsOnss);
}

Boolean CGsmServiceStateTracker::CurrentMccEqualsSimMcc(
    /* [in] */ IServiceState* s)
{
    String simNumeric;
    GetSystemProperty(
            ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, String(""), &simNumeric);
    String operatorNumeric;
    s->GetOperatorNumeric(&operatorNumeric);
    Boolean equalsMcc = TRUE;

    // try {
    equalsMcc = simNumeric.Substring(0, 3).
            Equals(operatorNumeric.Substring(0, 3));
    // } catch (Exception e){
    // }
    return equalsMcc;
}

Boolean CGsmServiceStateTracker::IsOperatorConsideredNonRoaming(
    /* [in] */ IServiceState* s)
{
    String operatorNumeric;
    s->GetOperatorNumeric(&operatorNumeric);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > numericArray;
    res->GetStringArray(
            Elastos::Droid::R::array::config_operatorConsideredNonRoaming, (ArrayOf<String>**)&numericArray);

    if (numericArray->GetLength() == 0 || operatorNumeric == NULL) {
        return FALSE;
    }

    for (Int32 i = 0; i < numericArray->GetLength(); i++) {
        String numeric = (*numericArray)[i];
        if (operatorNumeric.StartWith(numeric)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CGsmServiceStateTracker::IsOperatorConsideredRoaming(
    /* [in] */ IServiceState* s)
{
    String operatorNumeric;
    s->GetOperatorNumeric(&operatorNumeric);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > numericArray;
    res->GetStringArray(
            Elastos::Droid::R::array::config_sameNamedOperatorConsideredRoaming, (ArrayOf<String>**)&numericArray);

    if (numericArray->GetLength() == 0 || operatorNumeric == NULL) {
        return FALSE;
    }

    for (Int32 i = 0; i < numericArray->GetLength(); i++) {
        String numeric = (*numericArray)[i];
        if (operatorNumeric.StartWith(numeric)) {
            return TRUE;
        }
    }
    return FALSE;
}

void CGsmServiceStateTracker::SetTimeFromNITZString(
    /* [in] */ const String& nitz,
    /* [in] */ Int64 nitzReceiveTime)
{
    // "yy/mm/dd,hh:mm:ss(+/-)tz"
    // tz is in number of quarter-hours

    Int64 start = SystemClock::GetElapsedRealtime();
    if (DBG) {
        Logger::I("CGsmServiceStateTracker", "NITZ: %s, %lld, start=%lld, delay=%lld",
            nitz.string(), nitzReceiveTime, start,  (start - nitzReceiveTime));
    }

    // try {
        /* NITZ time (hour:min:sec) will be in UTC but it supplies the timezone
         * offset as well (which we won't worry about until later) */
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("GMT"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance(tz, (ICalendar**)&c);

    c->Clear();
    c->Set(ICalendar::DST_OFFSET, 0);

    AutoPtr<ArrayOf<String> > nitzSubs;
    StringUtils::Split(nitz, String("[/:,+-]"), (ArrayOf<String>**)&nitzSubs);

    Int32 year = 2000 + StringUtils::ParseInt32((*nitzSubs)[0]);
    c->Set(ICalendar::YEAR, year);

    // month is 0 based!
    Int32 month = StringUtils::ParseInt32((*nitzSubs)[1]) - 1;
    c->Set(ICalendar::MONTH, month);

    Int32 date = StringUtils::ParseInt32((*nitzSubs)[2]);
    c->Set(ICalendar::DATE, date);

    Int32 hour = StringUtils::ParseInt32((*nitzSubs)[3]);
    c->Set(ICalendar::HOUR, hour);

    Int32 minute = StringUtils::ParseInt32((*nitzSubs)[4]);
    c->Set(ICalendar::MINUTE, minute);

    Int32 second = StringUtils::ParseInt32((*nitzSubs)[5]);
    c->Set(ICalendar::SECOND, second);

    Boolean sign = (nitz.IndexOf('-') == -1);

    Int32 tzOffset = StringUtils::ParseInt32((*nitzSubs)[6]);

    Int32 dst = (nitzSubs->GetLength() >= 8 ) ? StringUtils::ParseInt32((*nitzSubs)[7])
                                      : 0;

    // The zone offset received from NITZ is for current local time,
    // so DST correction is already applied.  Don't add it again.
    //
    // tzOffset += dst * 4;
    //
    // We could unapply it if we wanted the raw offset.

    tzOffset = (sign ? 1 : -1) * tzOffset * 15 * 60 * 1000;

    AutoPtr<ITimeZone> zone;

    // As a special extension, the Android emulator appends the name of
    // the host computer's timezone to the nitz string. this is zoneinfo
    // timezone name of the form Area!Location or Area!Location!SubLocation
    // so we need to convert the ! into /
    if (nitzSubs->GetLength() >= 9) {
        String tzname = (*nitzSubs)[8].Replace('!','/');
        tzHelper->GetTimeZone( tzname, (ITimeZone**)&zone);
        // From luni's getTimeZone() "We never return NULL; on failure we return the
        // equivalent of "GMT"." This is bad, since it'll force all invalid strings
        // to "GMT"... and all the NULL-zone checks below will fail, making tzOffset
        // irrelevant and GMT the active TZ. So tzOffset will take precedence if this
        // results in "GMT"
        Boolean b;
        if ((IObject::Probe(tz)->Equals(zone, &b), b) && tzOffset != 0) {
            zone = NULL;
        }
    }

    String iso;
    GetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""), &iso);

    if (zone == NULL) {

        Int64 ms;
        c->GetTimeInMillis(&ms);
        if (mGotCountryCode) {
            if (iso != NULL && iso.GetLength() > 0) {
                zone = TimeUtils::GetTimeZone(tzOffset, dst != 0,
                        ms, iso);
            }
            else {
                // We don't have a valid iso country code.  This is
                // most likely because we're on a test network that's
                // using a bogus MCC (eg, "001"), so get a TimeZone
                // based only on the NITZ parameters.
                zone = GetNitzTimeZone(tzOffset, (dst != 0), ms);
            }
        }
    }

    if ((zone == NULL) || (mZoneOffset != tzOffset) || (mZoneDst != (dst != 0))){
        // We got the time before the country or the zone has changed
        // so we don't know how to identify the DST rules yet.  Save
        // the information and hope to fix it up later.

        mNeedFixZoneAfterNitz = TRUE;
        mZoneOffset  = tzOffset;
        mZoneDst     = dst != 0;
        c->GetTimeInMillis(&mZoneTime);
    }

    if (zone != NULL) {
        String id;
        zone->GetID(&id);
        if (GetAutoTimeZone()) {
            SetAndBroadcastNetworkSetTimeZone(id);
        }
        SaveNitzTimeZone(id);
    }

    String ignore;
    SystemProperties::Get(String("gsm.ignore-nitz"), &ignore);
    if (!ignore.IsNull() && ignore.Equals("yes")) {
        Log(String("NITZ: Not setting clock because gsm.ignore-nitz is set"));
        return;
    }

    // try {
        mWakeLock->AcquireLock();

        if (GetAutoTime()) {
            Int64 millisSinceNitzReceived
                    = SystemClock::GetElapsedRealtime() - nitzReceiveTime;

            if (millisSinceNitzReceived < 0) {
                // Sanity check: something is wrong
                if (DBG) {
                    Log(String("NITZ: not setting time, clock has rolled ")
                                    + "backwards since NITZ time was received, "
                                    + nitz);
                }
                return;
            }

            if (millisSinceNitzReceived > Elastos::Core::Math::INT32_MAX_VALUE) {
                // If the time is this far off, something is wrong > 24 days!
                if (DBG) {
                    Logger::I("CGsmServiceStateTracker", "NITZ: not setting time, processing has taken %d days.",
                        (millisSinceNitzReceived / (1000 * 60 * 60 * 24)));
                }
                return;
            }

            // Note: with range checks above, cast to int is safe
            c->Add(ICalendar::MILLISECOND, (Int32)millisSinceNitzReceived);

            if (DBG) {
                // Log(String("NITZ: Setting time of day to ") + c.getTime()
                //     + " NITZ receive delay(ms): " + millisSinceNitzReceived
                //     + " gained(ms): "
                //     + (c.getTimeInMillis() - System.currentTimeMillis())
                //     + " from " + nitz);
            }

            Int64 ms;
            c->GetTimeInMillis(&ms);
            SetAndBroadcastNetworkSetTime(ms);
            Logger::I(TAG, "NITZ: after Setting time of day");
        }
        Int64 ms;
        c->GetTimeInMillis(&ms);
        SystemProperties::Set(String("gsm.nitz.time"), StringUtils::ToString(ms));
        SaveNitzTime(ms);
        if (VDBG) {
            Int64 end = SystemClock::GetElapsedRealtime();
            Log(String("NITZ: end=") + StringUtils::ToString(end) + " dur=" + StringUtils::ToString((end - start)));
        }
        mNitzUpdatedTime = TRUE;
    // } finally {
    mWakeLock->ReleaseLock();
    // }
    // } catch (RuntimeException ex) {
    //     loge("NITZ: Parsing NITZ time " + nitz + " ex=" + ex);
    // }
}

Boolean CGsmServiceStateTracker::GetAutoTime()
{
    // try {
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::AUTO_TIME, &val);
    return val > 0;
    // } catch (SettingNotFoundException snfe) {
    //     return TRUE;
    // }
}

Boolean CGsmServiceStateTracker::GetAutoTimeZone()
{
    // try {
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::AUTO_TIME_ZONE, &val);
    return val > 0;
    // } catch (SettingNotFoundException snfe) {
    //     return TRUE;
    // }
}

void CGsmServiceStateTracker::SaveNitzTimeZone(
    /* [in] */ const String& zoneId)
{
    mSavedTimeZone = zoneId;
}

void CGsmServiceStateTracker::SaveNitzTime(
    /* [in] */ Int64 time)
{
    mSavedTime = time;
    mSavedAtTime = SystemClock::GetElapsedRealtime();
}

void CGsmServiceStateTracker::SetAndBroadcastNetworkSetTimeZone(
    /* [in] */ const String& zoneId)
{
    if (DBG) Log(String("setAndBroadcastNetworkSetTimeZone: setTimeZone=") + zoneId);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarm = IAlarmManager::Probe(obj);
    alarm->SetTimeZone(zoneId);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_NETWORK_SET_TIMEZONE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(String("time-zone"), zoneId);
    context->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    if (DBG) {
        Log(String("setAndBroadcastNetworkSetTimeZone: call alarm.setTimeZone and broadcast zoneId=") + zoneId);
    }
}

void CGsmServiceStateTracker::SetAndBroadcastNetworkSetTime(
    /* [in] */ Int64 time)
{
    if (DBG) Log(String("setAndBroadcastNetworkSetTime: time=") + StringUtils::ToString(time) + "ms");
    SystemClock::SetCurrentTimeMillis(time);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_NETWORK_SET_TIME, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(String("time"), time);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    context->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void CGsmServiceStateTracker::RevertToNitzTime()
{
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::AUTO_TIME, 0, &val);
    if (val == 0) {
        return;
    }
    if (DBG) {
        Logger::I("CGsmServiceStateTracker", "Reverting to NITZ Time: mSavedTime=%lld mSavedAtTime=%lld",
            mSavedTime, mSavedAtTime);
    }
    if (mSavedTime != 0 && mSavedAtTime != 0) {
        SetAndBroadcastNetworkSetTime(mSavedTime
                + (SystemClock::GetElapsedRealtime() - mSavedAtTime));
    }
}

void CGsmServiceStateTracker::RevertToNitzTimeZone()
{
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::AUTO_TIME_ZONE, 0, &val);
    if (val == 0) {
        return;
    }
    if (DBG) Log(String("Reverting to NITZ TimeZone: tz='") + mSavedTimeZone);
    if (mSavedTimeZone != NULL) {
        SetAndBroadcastNetworkSetTimeZone(mSavedTimeZone);
    }
}

void CGsmServiceStateTracker::SetNotification(
    /* [in] */ Int32 notifyType)
{
    if (DBG) Log(String("setNotification: create notification ") + StringUtils::ToString(notifyType));
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);

    CNotification::New((INotification**)&mNotification);
    AutoPtr<CNotification> notification = (CNotification*)mNotification.Get();
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&notification->mWhen);
    notification->mFlags = INotification::FLAG_AUTO_CANCEL;
    notification->mIcon = Elastos::Droid::R::drawable::stat_sys_warning;
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    CPendingIntent::GetActivity(context, 0, intent, IPendingIntent::FLAG_CANCEL_CURRENT,
        (IPendingIntent**)&notification->mContentIntent);

    AutoPtr<ICharSequence> details = CoreUtils::Convert(String(""));
    AutoPtr<ICharSequence> title;
    context->GetText(Elastos::Droid::R::string::RestrictedChangedTitle, (ICharSequence**)&title);
    Int32 notificationId = CS_NOTIFICATION;

    switch (notifyType) {
    case PS_ENABLED:
        notificationId = PS_NOTIFICATION;
        context->GetText(Elastos::Droid::R::string::RestrictedOnData, (ICharSequence**)&details);
        break;
    case PS_DISABLED:
        notificationId = PS_NOTIFICATION;
        break;
    case CS_ENABLED:
        context->GetText(Elastos::Droid::R::string::RestrictedOnAllVoice, (ICharSequence**)&details);
        break;
    case CS_NORMAL_ENABLED:
        context->GetText(Elastos::Droid::R::string::RestrictedOnNormal, (ICharSequence**)&details);
        break;
    case CS_EMERGENCY_ENABLED:
        context->GetText(Elastos::Droid::R::string::RestrictedOnEmergency, (ICharSequence**)&details);
        break;
    case CS_DISABLED:
        // do nothing and cancel the notification later
        break;
    }

    if (DBG) Log(String("setNotification: put notification ") + CoreUtils::Unbox(title)
            + " / " + CoreUtils::Unbox(details));
    notification->mTickerText = title;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &notification->mColor);
    notification->SetLatestEventInfo(context, title, details,
            notification->mContentIntent);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(obj);

    if (notifyType == PS_DISABLED || notifyType == CS_DISABLED) {
        // cancel previous post notification
        notificationManager->Cancel(notificationId);
    }
    else {
        // update restricted state notification
        notificationManager->Notify(notificationId, mNotification);
    }
}

AutoPtr<IUiccCardApplication> CGsmServiceStateTracker::GetUiccCardApplication()
{
    Int32 pid;
    IPhone::Probe(mPhone)->GetPhoneId(&pid);
    AutoPtr<IUiccCardApplication> app;
    if (mUiccController)
        mUiccController->GetUiccCardApplication(pid, IUiccController::APP_FAM_3GPP,
            (IUiccCardApplication**)&app);
    return app;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
