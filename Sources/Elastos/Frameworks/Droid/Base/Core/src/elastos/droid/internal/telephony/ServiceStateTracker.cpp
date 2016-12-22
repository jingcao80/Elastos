
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/CRestrictedState.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/telephony/CServiceStateHelper.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/utility/CPair.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/core/IntegralToString.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_UNKNOWN;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
// using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::IServiceStateHelper;
using Elastos::Droid::Telephony::CServiceStateHelper;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;

using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(ServiceStateTracker, Handler, IServiceStateTracker)

const Boolean ServiceStateTracker::DBG = TRUE;
const Boolean ServiceStateTracker::VDBG = FALSE;
const String ServiceStateTracker::PROP_FORCE_ROAMING("telephony.test.forceRoaming");

/** Signal strength poll rate. */
const Int32 ServiceStateTracker::POLL_PERIOD_MILLIS = 20 * 1000;

/** GSM events */
const Int32 ServiceStateTracker::EVENT_RADIO_STATE_CHANGED;
const Int32 ServiceStateTracker::EVENT_NETWORK_STATE_CHANGED;
const Int32 ServiceStateTracker::EVENT_GET_SIGNAL_STRENGTH;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_REGISTRATION;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_GPRS;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_OPERATOR;
const Int32 ServiceStateTracker::EVENT_POLL_SIGNAL_STRENGTH;
const Int32 ServiceStateTracker::EVENT_NITZ_TIME;
const Int32 ServiceStateTracker::EVENT_SIGNAL_STRENGTH_UPDATE;
const Int32 ServiceStateTracker::EVENT_RADIO_AVAILABLE;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_NETWORK_SELECTION_MODE;
const Int32 ServiceStateTracker::EVENT_GET_LOC_DONE;
const Int32 ServiceStateTracker::EVENT_SIM_RECORDS_LOADED;
const Int32 ServiceStateTracker::EVENT_SIM_READY;
const Int32 ServiceStateTracker::EVENT_LOCATION_UPDATES_ENABLED;
const Int32 ServiceStateTracker::EVENT_GET_PREFERRED_NETWORK_TYPE;
const Int32 ServiceStateTracker::EVENT_SET_PREFERRED_NETWORK_TYPE;
const Int32 ServiceStateTracker::EVENT_RESET_PREFERRED_NETWORK_TYPE;
const Int32 ServiceStateTracker::EVENT_CHECK_REPORT_GPRS;
const Int32 ServiceStateTracker::EVENT_RESTRICTED_STATE_CHANGED;

/** CDMA events */
const Int32 ServiceStateTracker::EVENT_POLL_STATE_REGISTRATION_CDMA;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_OPERATOR_CDMA;
const Int32 ServiceStateTracker::EVENT_RUIM_READY;
const Int32 ServiceStateTracker::EVENT_RUIM_RECORDS_LOADED;
const Int32 ServiceStateTracker::EVENT_POLL_SIGNAL_STRENGTH_CDMA;
const Int32 ServiceStateTracker::EVENT_GET_SIGNAL_STRENGTH_CDMA;
const Int32 ServiceStateTracker::EVENT_NETWORK_STATE_CHANGED_CDMA;
const Int32 ServiceStateTracker::EVENT_GET_LOC_DONE_CDMA;
//const Int32 ServiceStateTracker::EVENT_UNUSED;
const Int32 ServiceStateTracker::EVENT_NV_LOADED;
const Int32 ServiceStateTracker::EVENT_POLL_STATE_CDMA_SUBSCRIPTION;
const Int32 ServiceStateTracker::EVENT_NV_READY;
const Int32 ServiceStateTracker::EVENT_ERI_FILE_LOADED;
const Int32 ServiceStateTracker::EVENT_OTA_PROVISION_STATUS_CHANGE;
const Int32 ServiceStateTracker::EVENT_SET_RADIO_POWER_OFF;
const Int32 ServiceStateTracker::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
const Int32 ServiceStateTracker::EVENT_CDMA_PRL_VERSION_CHANGED;
const Int32 ServiceStateTracker::EVENT_RADIO_ON;
const Int32 ServiceStateTracker::EVENT_ICC_CHANGED;
const Int32 ServiceStateTracker::EVENT_GET_CELL_INFO_LIST;
const Int32 ServiceStateTracker::EVENT_UNSOL_CELL_INFO_LIST;
const Int32 ServiceStateTracker::EVENT_CHANGE_IMS_STATE;

const AutoPtr<ArrayOf<String> > ServiceStateTracker::GMT_COUNTRY_CODES = InitCOUNTCODE();

const String ServiceStateTracker::TIMEZONE_PROPERTY("persist.sys.timezone");

/** Reason for registration denial. */
const String ServiceStateTracker::REGISTRATION_DENIED_GEN("General");
const String ServiceStateTracker::REGISTRATION_DENIED_AUTH("Authentication Failure");

const String ServiceStateTracker::ACTION_RADIO_OFF("android.intent.action.ACTION_RADIO_OFF");

const Int64 ServiceStateTracker::LAST_CELL_INFO_LIST_MAX_AGE_MS = 2000;

ServiceStateTracker::ServiceStateTracker()
    : mUiccController(NULL)
    , mUiccApplcation(NULL)
    , mIccRecords(NULL)
    , mLastCellInfoList(NULL)
    , mIntentFilter(NULL)
    , mRadioOffIntent(NULL)
    , mDontPollSignalStrength(FALSE)
    , mPendingRadioPowerOffAfterDataOff(FALSE)
    , mPendingRadioPowerOffAfterDataOffTag(0)
    , mIwlanRatAvailable(FALSE)
    , mImsRegistrationOnOff(FALSE)
    , mAlarmSwitch(FALSE)
    , mPowerOffDelayNeed(TRUE)
    , mDeviceShuttingDown(FALSE)
    , mLastSignalStrength(NULL)
{
    Handler::constructor();
    CServiceState::New((IServiceState**)&mSS);
    CServiceState::New((IServiceState**)&mNewSS);

    CSignalStrength::New((ISignalStrength**)&mSignalStrength);
    CRestrictedState::New((IRestrictedState**)&mRestrictedState);

    AutoPtr<IRegistrantList> list;
    CRegistrantList::New((IRegistrantList**)&list);
    mRoamingOnRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mRoamingOffRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mAttachedRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mDetachedRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mDataRegStateOrRatChangedRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mNetworkAttachedRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mPsRestrictEnabledRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
    CRegistrantList::New((IRegistrantList**)&list);
    mPsRestrictDisabledRegistrants = (RegistrantList*)(list.Get());
    list = NULL;
}

AutoPtr<ArrayOf<String> > ServiceStateTracker::InitCOUNTCODE()
{
    AutoPtr<ArrayOf<String> > sArray = ArrayOf<String>::Alloc(20);

    (*sArray)[0] = String("bf"); // Burkina Faso
    (*sArray)[1] = String("ci"); // Cote d'Ivoire
    (*sArray)[2] = String("eh"); // Western Sahara
    (*sArray)[3] = String("fo"); // Faroe Islands, Denmark
    (*sArray)[4] = String("gb"); // United Kingdom of Great Britain and Northern Ireland
    (*sArray)[5] = String("gh"); // Ghana
    (*sArray)[6] = String("gm"); // Gambia
    (*sArray)[7] = String("gn"); // Guinea
    (*sArray)[8] = String("gw"); // Guinea Bissau
    (*sArray)[9] = String("ie"); // Ireland
    (*sArray)[10] = String("lr"); // Liberia
    (*sArray)[11] = String("is"); // Iceland
    (*sArray)[12] = String("ma"); // Morocco
    (*sArray)[13] = String("ml"); // Mali
    (*sArray)[14] = String("mr"); // Mauritania
    (*sArray)[15] = String("pt"); // Portugal
    (*sArray)[16] = String("sl"); // Sierra Leone
    (*sArray)[17] = String("sn"); // Senegal
    (*sArray)[18] = String("st"); // Sao Tome and Principe
    (*sArray)[19] = String("tg"); // Togo
    return sArray;
}

ECode ServiceStateTracker::constructor(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ ICellInfo* cellInfo)
{
    mPhoneBase = phoneBase;
    mCellInfo = cellInfo;
    mCi = ci;

    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneBase)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_voice_capable, &mVoiceCapable);

    mUiccController = UiccController::GetInstance();
    mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
    mCi->SetOnSignalStrengthUpdate(this, EVENT_SIGNAL_STRENGTH_UPDATE, NULL);
    mCi->RegisterForCellInfoList(this, EVENT_UNSOL_CELL_INFO_LIST, NULL);

    String str;
    AutoPtr<IServiceStateHelper> ssHelper;
    CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&ssHelper);
    ssHelper->RilRadioTechnologyToString(IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN, &str);
    return mPhoneBase->SetSystemProperty(ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE, str);
}

ECode ServiceStateTracker::RequestShutdown()
{
    if (mDeviceShuttingDown == TRUE) return NOERROR;
    mDeviceShuttingDown = TRUE;
    mDesiredPowerState = FALSE;
    SetPowerStateToDesired();
    return NOERROR;
}

ECode ServiceStateTracker::Dispose()
{
    assert(0);
    //mCi->UnSetOnSignalStrengthUpdate(this);
    //mUiccController->UnregisterForIccChanged(this);
    //mCi->UnregisterForCellInfoList(this);
    return NOERROR;
}

ECode ServiceStateTracker::GetDesiredPowerState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDesiredPowerState;
    return NOERROR;
}

Boolean ServiceStateTracker::NotifySignalStrength()
{
    {
        AutoLock syncLock(mCellInfo);

        Boolean bEqual;
        if (IObject::Probe(mSignalStrength)->Equals(mLastSignalStrength, &bEqual), !bEqual) {
            FAIL_RETURN(mPhoneBase->NotifySignalStrength());
            return TRUE;
        }
    }
    return FALSE;
}

void ServiceStateTracker::NotifyDataRegStateRilRadioTechnologyChanged()
{
    Int32 rat, drs;
    mSS->GetRilDataRadioTechnology(&rat);
    mSS->GetDataRegState(&drs);

    if (DBG) {
        Log(String("notifyDataRegStateRilRadioTechnologyChanged: drs=") + \
            IntegralToString::ToString(drs) + String(" rat= ") + IntegralToString::ToString(rat));
    }

    AutoPtr<IServiceStateHelper> ssHelper;
    String str;
    CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&ssHelper);
    ssHelper->RilRadioTechnologyToString(rat, &str);
    mPhoneBase->SetSystemProperty(ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE, str);

    AutoPtr<IPair> pair;
    CPair::New(CoreUtils::Convert(drs), CoreUtils::Convert(rat), (IPair**)&pair);
    mDataRegStateOrRatChangedRegistrants->NotifyResult(pair);
}

void ServiceStateTracker::UseDataRegStateForDataOnlyDevices()
{
    if (mVoiceCapable == FALSE) {
        if (DBG) {
            assert(0 && "TODO");
            // Log("useDataRegStateForDataOnlyDevice: VoiceRegState=" + mNewSS->GetVoiceRegState()
            //     + " DataRegState=" + mNewSS->GetDataRegState());
        }
        // TODO: Consider not lying and instead have callers know the difference.

        Int32 state = 0;
        mNewSS->GetDataRegState(&state);
        mNewSS->SetVoiceRegState(state);
    }
}

void ServiceStateTracker::UpdatePhoneObject()
{
    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneBase)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean bResult = FALSE;
    res->GetBoolean(R::bool_::config_switch_phone_on_voice_reg_state_change, &bResult);
    if (bResult) {
        Int32 rt = 0;
        mSS->GetRilVoiceRadioTechnology(&rt);
        IPhone::Probe(mPhoneBase)->UpdatePhoneObject(rt);
    }
}

ECode ServiceStateTracker::RegisterForRoamingOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRoamingOnRegistrants->Add(r);

    Boolean bRoaming;
    mSS->GetRoaming(&bRoaming);
    if (bRoaming) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForRoamingOn(
    /* [in] */ IHandler* h)
{
    mRoamingOnRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForRoamingOff(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRoamingOffRegistrants->Add(r);

    Boolean bRoaming = FALSE;
    mSS->GetRoaming(&bRoaming);
    if (!bRoaming) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForRoamingOff(
    /* [in] */ IHandler* h)
{
    mRoamingOffRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::ReRegisterNetwork(
    /* [in] */ IMessage* onComplete)
{
    assert(0);
    //AutoPtr<IMessage> response;
    //ObtainMessage(EVENT_GET_PREFERRED_NETWORK_TYPE, onComplete, (IMessage**)&response);
    //return mCi->GetPreferredNetworkType(response);
    return NOERROR;
}

ECode ServiceStateTracker::SetRadioPower(
    /* [in] */ Boolean power)
{
    mDesiredPowerState = power;

    SetPowerStateToDesired();
    return NOERROR;
}

ECode ServiceStateTracker::EnableSingleLocationUpdate()
{
    assert(0);
    //if (mWantSingleLocationUpdate || mWantContinuousLocationUpdates) return NOERROR;

    //mWantSingleLocationUpdate = TRUE;
    //AutoPtr<IMessage> response;
    //ObtainMessage(EVENT_LOCATION_UPDATES_ENABLED, (IMessage**)&response);
    //return mCi->SetLocationUpdates(TRUE, response);
    return NOERROR;
}

ECode ServiceStateTracker::EnableLocationUpdates()
{
    assert(0);
    //if (mWantSingleLocationUpdate || mWantContinuousLocationUpdates) return NOERROR;

    //mWantContinuousLocationUpdates = TRUE;
    //AutoPtr<IMessage> response;
    //ObtainMessage(EVENT_LOCATION_UPDATES_ENABLED, (IMessage**)&response);
    //return mCi->SetLocationUpdates(TRUE, response);
    return NOERROR;
}

void ServiceStateTracker::DisableSingleLocationUpdate()
{
    assert(0);
    //mWantSingleLocationUpdate = FALSE;
    //if (!mWantSingleLocationUpdate && !mWantContinuousLocationUpdates) {
    //    mCi->SetLocationUpdates(FALSE, NULL);
    //}
}

ECode ServiceStateTracker::DisableLocationUpdates()
{
    assert(0);
    //mWantContinuousLocationUpdates = FALSE;
    //if (!mWantSingleLocationUpdate && !mWantContinuousLocationUpdates) {
    //    mCi->SetLocationUpdates(FALSE, NULL);
    //}
    return NOERROR;
}

ECode ServiceStateTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0, arg1 = 0, arg2 = 0;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case EVENT_SET_RADIO_POWER_OFF:
            {
                AutoLock syncLock(this);
                if (mPendingRadioPowerOffAfterDataOff &&
                        (arg1 == mPendingRadioPowerOffAfterDataOffTag)) {
                    if (DBG) {
                        Log(String("EVENT_SET_RADIO_OFF, turn radio off now."));
                    }
                    HangupAndPowerOff();
                    mPendingRadioPowerOffAfterDataOffTag += 1;
                    mPendingRadioPowerOffAfterDataOff = FALSE;
                }
                else {
                    Log(String("EVENT_SET_RADIO_OFF is stale arg1=") + StringUtils::ToString(arg1) +
                            String("!= tag=") + StringUtils::ToString(mPendingRadioPowerOffAfterDataOffTag));
                }
            }
            break;

        case EVENT_ICC_CHANGED:
            OnUpdateIccAvailability();
            break;

        case EVENT_GET_CELL_INFO_LIST: {
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<CellInfoResult> result = (CellInfoResult*)(IObject*)(ar->mUserObj).Get();
            {
                AutoLock syncLock(result->mLockObj);
                if (ar->mException != NULL) {
//                    Log("EVENT_GET_CELL_INFO_LIST: error ret NULL, e=" + ar.exception);
                    result->mList = NULL;
                }
                else {
                    result->mList = IList::Probe(ar->mResult);

//                    if (VDBG) {
//                        Log("EVENT_GET_CELL_INFO_LIST: size=" + result.list->Size()
//                                + " list=" + result.list);
//                    }
                }
                mLastCellInfoListTime = SystemClock::GetElapsedRealtime();
                mLastCellInfoList = result->mList;
                result->mLockObj.Notify();
            }
            break;
        }

        case EVENT_UNSOL_CELL_INFO_LIST: {
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                assert(0 && "TODO");
                // Log(String("EVENT_UNSOL_CELL_INFO_LIST: error ignoring, e=") + StringUtils::ToString(ar->mException));
            }
            else {
                AutoPtr<IList> list = IList::Probe(ar->mResult);
                if (DBG) {
                    Int32 size = 0;
                    list->GetSize(&size);
                    assert(0 && "TODO");
                    // Log("EVENT_UNSOL_CELL_INFO_LIST: size=" + size
                    //         + " list=" + list);
                }
                mLastCellInfoListTime = SystemClock::GetElapsedRealtime();
                mLastCellInfoList = list;
                mPhoneBase->NotifyCellInfo(list);
            }
            break;
        }

        default:
            Log(String("Unhandled message with number: ") + StringUtils::ToString(what));
            break;
    }
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForDataConnectionAttached(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mAttachedRegistrants->Add(r);

    Int32 state = 0;
    GetCurrentDataConnectionState(&state);
    if (state == IServiceState::STATE_IN_SERVICE) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForDataConnectionAttached(
    /* [in] */ IHandler* h)
{
    mAttachedRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForDataConnectionDetached(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mDetachedRegistrants->Add(r);

    Int32 state = 0;
    GetCurrentDataConnectionState(&state);
    if (state != IServiceState::STATE_IN_SERVICE) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForDataConnectionDetached(
    /* [in] */ IHandler* h)
{
    mDetachedRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForDataRegStateOrRatChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mDataRegStateOrRatChangedRegistrants->Add(r);
    NotifyDataRegStateRilRadioTechnologyChanged();
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForDataRegStateOrRatChanged(
    /* [in] */ IHandler* h)
{
    mDataRegStateOrRatChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForNetworkAttached(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mNetworkAttachedRegistrants->Add(r);

    Int32 state;
    mSS->GetVoiceRegState(&state);
    if (state == IServiceState::STATE_IN_SERVICE) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForNetworkAttached(
    /* [in] */ IHandler* h)
{
    mNetworkAttachedRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForPsRestrictedEnabled(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mPsRestrictEnabledRegistrants->Add(r);

    Boolean bRestricted = FALSE;
    mRestrictedState->IsPsRestricted(&bRestricted);
    if (bRestricted) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForPsRestrictedEnabled(
    /* [in] */ IHandler* h)
{
    mPsRestrictEnabledRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::RegisterForPsRestrictedDisabled(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mPsRestrictDisabledRegistrants->Add(r);

    Boolean result = FALSE;
    mRestrictedState->IsPsRestricted(&result);
    if (result) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode ServiceStateTracker::UnregisterForPsRestrictedDisabled(
    /* [in] */ IHandler* h)
{
    mPsRestrictDisabledRegistrants->Remove(h);
    return NOERROR;
}

ECode ServiceStateTracker::PowerOffRadioSafely(
    /* [in] */ IDcTrackerBase* dcTracker)
{
    {
        AutoLock syncLock(this);
        if (!mPendingRadioPowerOffAfterDataOff) {
            // In some network, deactivate PDP connection cause releasing of RRC connection,
            // which MM/IMSI detaching request needs. Without this detaching, network can
            // not release the network resources previously attached.
            // So we are avoiding data detaching on these networks.
            AutoPtr<IContext> context;
            IPhone::Probe(mPhoneBase)->GetContext((IContext**)&context);
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            AutoPtr< ArrayOf<String> > networkNotClearData;
            res->GetStringArray(R::array::networks_not_clear_data, (ArrayOf<String>**)&networkNotClearData);

            String currentNetwork;
            mSS->GetOperatorNumeric(&currentNetwork);

            if ((networkNotClearData != NULL) && (currentNetwork != NULL)) {
                Int32 arrayLen = networkNotClearData->GetLength();
                for (Int32 i = 0; i < arrayLen; i++) {
                    if (currentNetwork.Equals((*networkNotClearData)[i])) {
                        // Don't clear data connection for this carrier
                        if (DBG) {
                            Log(String("Not disconnecting data for ") + currentNetwork);
                        }
                        HangupAndPowerOff();
                        return NOERROR;
                    }
                }
            }
            // To minimize race conditions we call cleanUpAllConnections on
            // both if else paths instead of before this isDisconnected test.
            Boolean result;
            if (dcTracker->IsDisconnected(&result), result) {
                // To minimize race conditions we do this after isDisconnected
                dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);
                if (DBG) Log(String("Data disconnected, turn off radio right away."));
                HangupAndPowerOff();
            }
            else {
                dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);

                AutoPtr<IMessageHelper> mHelper;
                AutoPtr<IMessage> msg;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&mHelper);
                mHelper->Obtain(this, (IMessage**)&msg);
                msg->SetWhat(EVENT_SET_RADIO_POWER_OFF);
                msg->SetArg1(++mPendingRadioPowerOffAfterDataOffTag);

                if (SendMessageDelayed(msg, 30000, &result), result) {
                    if (DBG) Log(String("Wait upto 30s for data to disconnect, then turn off radio."));
                    mPendingRadioPowerOffAfterDataOff = TRUE;
                }
                else {
                    Log(String("Cannot send delayed Msg, turn off radio right away."));
                    HangupAndPowerOff();
                }
            }
        }
    }
    return NOERROR;
}

ECode ServiceStateTracker::ProcessPendingRadioPowerOffAfterDataOff(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(this);
        if (mPendingRadioPowerOffAfterDataOff) {
            if (DBG) Log(String("Process pending request to turn radio off."));
            mPendingRadioPowerOffAfterDataOffTag += 1;
            HangupAndPowerOff();
            mPendingRadioPowerOffAfterDataOff = FALSE;
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
}

Boolean ServiceStateTracker::OnSignalStrengthResult(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Boolean isGsm)
{
    AutoPtr<ISignalStrength> oldSignalStrength = mSignalStrength;

    // This signal is used for both voice and data radio signal so parse
    // all fields

    if ((ar->mException == NULL) && (ar->mResult != NULL)) {
        mSignalStrength = ISignalStrength::Probe(ar->mResult);
        mSignalStrength->ValidateInput();
        mSignalStrength->SetGsm(isGsm);
    }
    else {
        String str("OnSignalStrengthResult() Exception from RIL : ");
        assert(0 && "TODO");
        // str += ar->mException;
        Log(str);
        // CSignalStrength::New(isGsm, (ISignalStrength**)&mSignalStrength);
    }

    return NotifySignalStrength();
}

void ServiceStateTracker::CancelPollState()
{
    assert(0);
    // This will effectively cancel the rest of the poll requests.
    //mPollingContext = ArrayOf<Int32>::Alloc(1);
}

Boolean ServiceStateTracker::ShouldFixTimeZoneNow(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ const String& operatorNumeric,
    /* [in] */ const String& prevOperatorNumeric,
    /* [in] */ Boolean needToFixTimeZone)
{
    // Return FALSE if the mcc isn't valid as we don't know where we are.
    // Return TRUE if we have an IccCard and the mcc changed or we
    // need to fix it because when the NITZ time came in we didn't
    // know the country code.

    // if mcc is invalid then we'll return FALSE
    String subStr = operatorNumeric.Substring(0, 3);
    Int32 mcc = StringUtils::ParseInt32(subStr);

    // if prevMcc is invalid will make it different from mcc
    // so we'll return TRUE if the card exists.
    subStr = prevOperatorNumeric.Substring(0, 3);
    Int32 prevMcc = StringUtils::ParseInt32(subStr);

    // Determine if the Icc card exists
    Boolean iccCardExist = FALSE;
    if (mUiccApplcation != NULL) {
        AppState aState;
        mUiccApplcation->GetState(&aState);
        iccCardExist = aState != APPSTATE_UNKNOWN;
    }

    // Determine retVal
    Boolean retVal = ((iccCardExist && (mcc != prevMcc)) || needToFixTimeZone);
    if (DBG) {
        AutoPtr<ISystem> sys;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 ctm = 0;
        sys->GetCurrentTimeMillis(&ctm);
        // Log("shouldFixTimeZoneNow: retVal=" + retVal +
        //         " iccCardExist=" + iccCardExist +
        //         " operatorNumeric=" + operatorNumeric + " mcc=" + mcc +
        //         " prevOperatorNumeric=" + prevOperatorNumeric + " prevMcc=" + prevMcc +
        //         " needToFixTimeZone=" + needToFixTimeZone +
        //         " ltod=" + TimeUtils->LogTimeOfDay(ctm));
    }
    return retVal;
}

ECode ServiceStateTracker::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    Int64 subId = 0;
    mPhoneBase->GetSubId(&subId);
    return hlp->GetTelephonyProperty(property, subId, defValue, result);
}

ECode ServiceStateTracker::GetAllCellInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CellInfoResult> res = new CellInfoResult();
    if (VDBG) Log(String("SST->GetAllCellInfo(): E"));

    Int32 ver = 0;
    mCi->GetRilVersion(&ver);
    if (ver >= 8) {
        if (IsCallerOnDifferentThread()) {
            if ((SystemClock::GetElapsedRealtime() - mLastCellInfoListTime)
                    > LAST_CELL_INFO_LIST_MAX_AGE_MS) {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_CELL_INFO_LIST, (IObject*)res.Get(), (IMessage**)&msg);
                {
                    AutoLock syncLock(res->mLockObj);
                    res->mList = NULL;
                    mCi->GetCellInfoList(msg);
//                    try {
                        res->mLockObj.Wait(5000);
//                    } Catch (InterruptedException e) {
//                        e->PrintStackTrace();
//                    }
                }
            }
            else {
                if (DBG) Log(String("SST->GetAllCellInfo(): return last, back to back calls"));
                res->mList = mLastCellInfoList;
            }
        }
        else {
            if (DBG) Log(String("SST->GetAllCellInfo(): return last, same thread can't block"));
            res->mList = mLastCellInfoList;
        }
    }
    else {
        if (DBG) Log(String("SST->GetAllCellInfo(): not implemented"));
        res->mList = NULL;
    }

    {
        AutoLock syncLock(res->mLockObj);
        if (res->mList != NULL) {
            if (DBG) {
                Int32 size = 0;
                res->mList->GetSize(&size);
                assert(0 && "TODO");
                // Log("SST->GetAllCellInfo(): X size=" + size
                //     + " list=" + res->mList);
            }
            *result = res->mList;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            if (DBG) Log(String("SST->GetAllCellInfo(): X size=0 list=NULL"));

            *result = NULL;
            return NOERROR;
        }
    }
}

ECode ServiceStateTracker::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(mCellInfo);
        *result = mSignalStrength;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode ServiceStateTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("ServiceStateTracker:"));
    pw->Println(String(" mSS=") + Object::ToString(mSS));
    pw->Println(String(" mNewSS=") + Object::ToString(mNewSS));
    pw->Println(String(" mCellInfo=") + Object::ToString(mCellInfo));
    pw->Println(String(" mRestrictedState=") + Object::ToString(mRestrictedState));
    assert(0 && "TODO");
    // pw->Println(String(" mPollingContext=") + Object::ToString(mPollingContext));
    pw->Println(String(" mDesiredPowerState=") + StringUtils::BooleanToString(mDesiredPowerState));
    pw->Println(String(" mDontPollSignalStrength=") + StringUtils::BooleanToString(mDontPollSignalStrength));
    pw->Println(String(" mPendingRadioPowerOffAfterDataOff=") + StringUtils::BooleanToString(mPendingRadioPowerOffAfterDataOff));
    pw->Println(String(" mPendingRadioPowerOffAfterDataOffTag=") + IntegralToString::ToString(mPendingRadioPowerOffAfterDataOffTag));
    return NOERROR;
}

ECode ServiceStateTracker::CheckCorrectThread()
{
    AutoPtr<IThread> looperThread;
    AutoPtr<ILooper> handlerLooper;
    GetLooper((ILooper**)&handlerLooper);
    handlerLooper->GetThread((IThread**)&looperThread);
    if (!Object::Equals(Thread::GetCurrentThread(), looperThread.Get())) {
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

Boolean ServiceStateTracker::IsCallerOnDifferentThread()
{
    AutoPtr<IThread> looperThread;
    AutoPtr<ILooper> handlerLooper;
    GetLooper((ILooper**)&handlerLooper);
    handlerLooper->GetThread((IThread**)&looperThread);
    Boolean value = !Object::Equals(Thread::GetCurrentThread(), looperThread.Get());
    if (VDBG) Log(String("isCallerOnDifferentThread: ") + StringUtils::BooleanToString(value));
    return value;
}

void ServiceStateTracker::UpdateCarrierMccMncConfiguration(
    /* [in] */ const String& newOp,
    /* [in] */ const String& oldOp,
    /* [in] */ IContext* context)
{
    // if we have a change in operator, notify Wifi (even to/from none)
    if (((newOp.IsNull()) && (TextUtils::IsEmpty(oldOp) == FALSE)) ||
            ((!newOp.IsNull()) && (newOp.Equals(oldOp) == FALSE))) {
        Log(String("update mccmnc=") + newOp + String(" fromServiceState=TRUE"));
        MccTable::UpdateMccMncConfiguration(context, newOp, TRUE);
    }
}

Boolean ServiceStateTracker::IsIwlanFeatureAvailable()
{
    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneBase)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean iwlanAvailable;
    res->GetBoolean(R::bool_::config_feature_iwlan_enabled, &iwlanAvailable);

    Log(String("Iwlan feature available = ") + StringUtils::BooleanToString(iwlanAvailable));
    return iwlanAvailable;
}

void ServiceStateTracker::ProcessIwlanToWwanTransition(
    /* [in] */ IServiceState* ss)
{
    // Wifi Connected(iwlan feature on) AND a Valid(non-wlan) RAT present
    // AND attached AND previous RAT was iwlan.
    //
    // Notify that we are attached so that we can setup connectable
    // APNs.
    Int32 rat = 0, state = 0;
    ss->GetRilDataRadioTechnology(&rat);
    ss->GetDataRegState(&state);

    if (IsIwlanFeatureAvailable() &&
            (rat != IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN) &&
            (rat != IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN) &&
            (state == IServiceState::STATE_IN_SERVICE) &&
            (mIwlanRatAvailable == TRUE)) {

        Log(String("pollStateDone: Wifi connected and moved out of iwlan and wwan is attached."));
        mAttachedRegistrants->NotifyRegistrants();
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
