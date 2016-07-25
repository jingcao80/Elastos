
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/CallManager.h"
#include "elastos/droid/internal/telephony/CallTracker.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/telecomm/telecom/VideoProfile.h"
#include "elastos/droid/telephony/CCellIdentityCdma.h"
#include "elastos/droid/telephony/CCellInfoCdma.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/CVoLteServiceState.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.Wifi.h"
#include <elastos/droid/R.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telecomm::Telecom::IVideoProfile;
using Elastos::Droid::Telecomm::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telecomm::Telecom::VideoProfile;
using Elastos::Droid::Telephony::CCellIdentityCdma;
using Elastos::Droid::Telephony::CCellInfoCdma;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::CVoLteServiceState;
using Elastos::Droid::Telephony::ICellIdentityCdma;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoCdma;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::TextUtils;
// using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::R;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhone;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhoneConnection;
// using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_UNKNOWN;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFlushable;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 PhoneBase::EVENT_RADIO_AVAILABLE;
const Int32 PhoneBase::EVENT_SSN;
const Int32 PhoneBase::EVENT_SIM_RECORDS_LOADED;
const Int32 PhoneBase::EVENT_MMI_DONE;
const Int32 PhoneBase::EVENT_RADIO_ON;
const Int32 PhoneBase::EVENT_GET_BASEBAND_VERSION_DONE;
const Int32 PhoneBase::EVENT_USSD;
const Int32 PhoneBase::EVENT_RADIO_OFF_OR_NOT_AVAILABLE;
const Int32 PhoneBase::EVENT_GET_IMEI_DONE;
const Int32 PhoneBase::EVENT_GET_IMEISV_DONE;
const Int32 PhoneBase::EVENT_GET_SIM_STATUS_DONE;
const Int32 PhoneBase::EVENT_SET_CALL_FORWARD_DONE;
const Int32 PhoneBase::EVENT_GET_CALL_FORWARD_DONE;
const Int32 PhoneBase::EVENT_CALL_RING;
const Int32 PhoneBase::EVENT_CALL_RING_CONTINUE;
const Int32 PhoneBase::EVENT_SET_NETWORK_MANUAL_COMPLETE;
const Int32 PhoneBase::EVENT_SET_NETWORK_AUTOMATIC_COMPLETE;
const Int32 PhoneBase::EVENT_SET_CLIR_COMPLETE;
const Int32 PhoneBase::EVENT_REGISTERED_TO_NETWORK;
const Int32 PhoneBase::EVENT_SET_VM_NUMBER_DONE;
const Int32 PhoneBase::EVENT_GET_DEVICE_IDENTITY_DONE;
const Int32 PhoneBase::EVENT_RUIM_RECORDS_LOADED;
const Int32 PhoneBase::EVENT_NV_READY;
const Int32 PhoneBase::EVENT_SET_ENHANCED_VP;
const Int32 PhoneBase::EVENT_EMERGENCY_CALLBACK_MODE_ENTER;
const Int32 PhoneBase::EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE;
const Int32 PhoneBase::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
const Int32 PhoneBase::EVENT_SET_NETWORK_AUTOMATIC;
const Int32 PhoneBase::EVENT_ICC_RECORD_EVENTS;
const Int32 PhoneBase::EVENT_ICC_CHANGED;
const Int32 PhoneBase::EVENT_SRVCC_STATE_CHANGED;
const Int32 PhoneBase::EVENT_INITIATE_SILENT_REDIAL;
const Int32 PhoneBase::EVENT_UNSOL_OEM_HOOK_RAW;
const Int32 PhoneBase::EVENT_SS;
const Int32 PhoneBase::EVENT_SET_CALL_FORWARD_TIMER_DONE;
const Int32 PhoneBase::EVENT_GET_CALL_FORWARD_TIMER_DONE;
const Int32 PhoneBase::EVENT_LAST;

CAR_INTERFACE_IMPL(PhoneBase::NetworkSelectMessage, Object, IPhoneBaseNetworkSelectMessage)

PhoneBase::ImsIntentReceiver::ImsIntentReceiver(
    /* [in] */ PhoneBase* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

ECode PhoneBase::ImsIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // TODO: MSIM potentially replace SUBID with information available to external
    // service, e.g. slot ID
    assert(0 && "TODO Need IImsManager");
    // Boolean has = FALSE;
    // if (intent->HasExtra(IImsManager::EXTRA_SUBID, &has), has) {
    //     Int64 v1 = 0, v2 = 0;
    //     intent->GetInt64Extra(IImsManager::EXTRA_SUBID, -1, &v1);
    //     mHost->GetSubId(&v2);
    //     if (v1 != v2) {
    //         return NOERROR;
    //     }
    // }

    // // For MSIM, check phone ID
    // if (intent->HasExtra(IImsManager::EXTRA_PHONEID, &has), has) {
    //     Int32 v1 = 0, v2 = 0;
    //     intent->GetIntExtra(IImsManager::EXTRA_PHONEID, -1, &v1);
    //     mHost->GetPhoneId(&v2);
    //     if (v1 != v2) {
    //         return NOERROR;
    //     }
    // }

    // String s;
    // intent->GetAction(&s);
    // if (s.Equals(IImsManager::ACTION_IMS_SERVICE_UP)) {
    //     mImsServiceReady = TRUE;
    //     mHost->UpdateImsPhone();
    // }
    // else if (s.Equals(IImsManager::ACTION_IMS_SERVICE_DOWN)) {
    //     mImsServiceReady = FALSE;
    //     mHost->UpdateImsPhone();
    // }
    return NOERROR;
}

const String PhoneBase::TAG("PhoneBase");
CAR_INTERFACE_IMPL_2(PhoneBase, Handler, IPhone, IPhoneBase)
PhoneBase::PhoneBase()
    : mIsTheCurrentActivePhone(TRUE)
    , mDnsCheckDisabled(FALSE)
    , mDoesRilSendMultipleCallRing(FALSE)
    , mCallRingContinueToken(0)
    , mCallRingDelay(0)
    , mIsVoiceCapable(TRUE)
    , mPhoneId(0)
    , mUnitTestMode(FALSE)
    , mVmCount(0)
    , mIsVideoCapable(FALSE)
    , mImsServiceReady(FALSE)
{
    Handler::constructor();
    CAtomicReference::New((IAtomicReference**)&mIccRecords);
    CAtomicReference::New((IAtomicReference**)&mUiccApplication);
    SystemProperties::GetBoolean(PROPERTY_OOS_IS_DISCONNECT, TRUE, &mOosIsDisconnect);

    mPreciseCallStateRegistrants = new RegistrantList();
    mHandoverRegistrants = new RegistrantList();
    mNewRingingConnectionRegistrants = new RegistrantList();
    mIncomingRingRegistrants = new RegistrantList();
    mDisconnectRegistrants = new RegistrantList();
    mServiceStateRegistrants = new RegistrantList();
    mMmiCompleteRegistrants = new RegistrantList();
    mMmiRegistrants = new RegistrantList();
    mUnknownConnectionRegistrants = new RegistrantList();
    mSuppServiceFailedRegistrants = new RegistrantList();
    mSimRecordsLoadedRegistrants = new RegistrantList();
    mVideoCapabilityChangedRegistrants = new RegistrantList();

    mImsIntentReceiver = new ImsIntentReceiver(this);
}

PhoneBase::~PhoneBase()
{}

// @Override
ECode PhoneBase::GetPhoneName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mName;
    return NOERROR;
}

/**
 * Return the ActionDetached string. When this action is received by components
 * they are to simulate detaching from the network.
 *
 * @return com.android.internal.telephony.{mName}.action_detached
 *          {mName} is GSM, CDMA ...
 */
ECode PhoneBase::GetActionDetached(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActionDetached;
    return NOERROR;
}

/**
 * Return the ActionAttached string. When this action is received by components
 * they are to simulate attaching to the network.
 *
 * @return com.android.internal.telephony.{mName}.action_detached
 *          {mName} is GSM, CDMA ...
 */
ECode PhoneBase::GetActionAttached(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActionAttached;
    return NOERROR;
}

/**
 * Set a system property, unless we're in unit test mode
 */
// CAF_MSIM TODO this need to be replated with TelephonyManager API ?
ECode PhoneBase::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    Boolean tmp = FALSE;
    if(GetUnitTestMode(&tmp), tmp) {
        return NOERROR;
    }
    SystemProperties::Set(property, value);
    return NOERROR;
}

/**
 * Set a system property, unless we're in unit test mode
 */
// CAF_MSIM TODO this need to be replated with TelephonyManager API ?
ECode PhoneBase::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if(GetUnitTestMode(&tmp), tmp) {
        *result = String(NULL);
        return NOERROR;
    }
    return SystemProperties::Get(property, defValue, result);
}

/**
 * Constructs a PhoneBase in normal (non-unit test) mode.
 *
 * @param notifier An instance of DefaultPhoneNotifier,
 * @param context IContext object from hosting application
 * unless unit testing.
 * @param ci the ICommandsInterface
 */
ECode PhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci)
{
    return constructor(name, notifier, context, ci, FALSE);
}

/**
 * Constructs a PhoneBase in normal (non-unit test) mode.
 *
 * @param notifier An instance of DefaultPhoneNotifier,
 * @param context IContext object from hosting application
 * unless unit testing.
 * @param ci is ICommandsInterface
 * @param unitTestMode when TRUE, prevents notifications
 * of state change events
 */
ECode PhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ Boolean unitTestMode)
{
    return constructor(name, notifier, context, ci, unitTestMode, ISubscriptionManager::DEFAULT_PHONE_ID);
}

/**
 * Constructs a PhoneBase in normal (non-unit test) mode.
 *
 * @param notifier An instance of DefaultPhoneNotifier,
 * @param context IContext object from hosting application
 * unless unit testing.
 * @param ci is ICommandsInterface
 * @param unitTestMode when TRUE, prevents notifications
 * of state change events
 * @param subscription is current phone subscription
 */
ECode PhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ Boolean unitTestMode,
    /* [in] */ Int32 phoneId)
{
    mPhoneId = phoneId;
    mName = name;
    mNotifier = notifier;
    mContext = context;
    mLooper = Looper::GetMyLooper();
    mCi = ci;
    Logger::D(TAG, "[TODO] PhoneBase::constructor : right??");
    String sv;
    ToString(&sv);
    mActionDetached = /*this.getClass().getPackage().getName()*/sv + ".action_detached";
    mActionAttached = /*this.getClass().getPackage().getName()*/sv + ".action_attached";

    if (Build::IS_DEBUGGABLE) {
        //TODO assert(0 && "TODO");
        // mTelephonyTester = new TelephonyTester(this);
    }

    SetUnitTestMode(unitTestMode);

    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(context, (ISharedPreferences**)&sp);
    sp->GetBoolean(DNS_SERVER_CHECK_DISABLED_KEY, FALSE, &mDnsCheckDisabled);
    mCi->SetOnCallRing(this, EVENT_CALL_RING, NULL);

    /* "Voice capable" means that this device supports circuit-switched
    * (i.e. voice) phone calls over the telephony network, and is allowed
    * to display the in-call UI while a cellular voice call is active.
    * This will be FALSE on "data only" devices which can't make voice
    * calls and don't support any in-call UI.
    */
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &mIsVoiceCapable);

    /**
     *  Some RIL's don't always send RIL_UNSOL_CALL_RING so it needs
     *  to be generated locally. Ideally all ring tones should be loops
     * and this wouldn't be necessary. But to minimize changes to upper
     * layers it is requested that it be generated by lower layers.
     *
     * By default old phones won't have the property set but do generate
     * the RIL_UNSOL_CALL_RING so the default if there is no property is
     * TRUE.
     */
    SystemProperties::GetBoolean(ITelephonyProperties::PROPERTY_RIL_SENDS_MULTIPLE_CALL_RING
            , TRUE, &mDoesRilSendMultipleCallRing);
    Logger::D(TAG, "mDoesRilSendMultipleCallRing=%d", mDoesRilSendMultipleCallRing);

    SystemProperties::GetInt32(
            ITelephonyProperties::PROPERTY_CALL_RING_DELAY, 3000, &mCallRingDelay);
    Logger::D(TAG, "mCallRingDelay=" + mCallRingDelay);

    Int32 iv = 0;
    GetPhoneType(&iv);
    if (iv == IPhoneConstants::PHONE_TYPE_IMS) return NOERROR;

    SetPropertiesByCarrier();

    Logger::E(TAG, "TODO SmsStorageMonitor & SmsUsageMonitor is not ready");
    // Initialize device storage and outgoing SMS usage monitors for SMSDispatchers.
    //TODO mSmsStorageMonitor = new SmsStorageMonitor();
    //TODO mSmsStorageMonitor->constructor(this);
    //TODO mSmsUsageMonitor = new SmsUsageMonitor();
    //TODO mSmsUsageMonitor->constructor(context);

    mUiccController = UiccController::GetInstance();
    mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);

    // Monitor IMS service
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(String("com.android.ims.IMS_SERVICE_UP"));//TODO IImsManager::ACTION_IMS_SERVICE_UP);
    filter->AddAction(String("com.android.ims.IMS_SERVICE_DOWN"));//TODO IImsManager::ACTION_IMS_SERVICE_DOWN);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mImsIntentReceiver, filter, (IIntent**)&i);

    mCi->RegisterForSrvccStateChanged(this, EVENT_SRVCC_STATE_CHANGED, NULL);
    mCi->SetOnUnsolOemHookRaw(this, EVENT_UNSOL_OEM_HOOK_RAW, NULL);
    Logger::D(TAG, "mOosIsDisconnect=%d", mOosIsDisconnect);
    return NOERROR;
}

// @Override
ECode PhoneBase::Dispose()
{
    assert(0 && "TODO");
    // AutoLock lock(PhoneProxy::sLockForRadioTechnologyChange);
    mContext->UnregisterReceiver(mImsIntentReceiver);
    mCi->UnSetOnCallRing(this);
    // Must cleanup all connectionS and needs to use sendMessage!
    mDcTracker->CleanUpAllConnections(String(NULL));
    mIsTheCurrentActivePhone = FALSE;
    // Dispose the SMS usage and storage monitors
    if (mSmsStorageMonitor)
        mSmsStorageMonitor->Dispose();
    if (mSmsUsageMonitor)
        mSmsUsageMonitor->Dispose();
    if (mUiccController)
        mUiccController->UnregisterForIccChanged(this);
    mCi->UnregisterForSrvccStateChanged(this);
    mCi->UnSetOnUnsolOemHookRaw(this);

    if (mTelephonyTester != NULL) {
        assert(0 && "TODO");
        // mTelephonyTester->Dispose();
    }

    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        imsPhone->UnregisterForSilentRedial(this);
        IPhone::Probe(imsPhone)->Dispose();
    }
    return NOERROR;
}

// @Override
ECode PhoneBase::RemoveReferences()
{
    mSmsStorageMonitor = NULL;
    mSmsUsageMonitor = NULL;
    mIccRecords->Set(NULL);
    mUiccApplication->Set(NULL);
    mDcTracker = NULL;
    mUiccController = NULL;

    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    if (imsPhone != NULL) {
        IPhone::Probe(imsPhone)->RemoveReferences();
        mImsPhone = NULL;
    }
    return NOERROR;
}

/**
 * When overridden the derived class needs to call
 * super.handleMessage(msg) so this method has a
 * a chance to process the message.
 *
 * @param msg
 */
// @Override
ECode PhoneBase::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;

    Int32 what = 0;
    msg->GetWhat(&what);
    if (!mIsTheCurrentActivePhone) {
        Logger::E(TAG, "Received message %s[%d] while being destroyed. Ignoring.", TO_CSTR(msg), what);
        return NOERROR;
    }
    switch(what) {
        case EVENT_CALL_RING: {
            PhoneConstantsState cs;
            GetState(&cs);
            Logger::D(TAG, "Event EVENT_CALL_RING Received state=%d", cs);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IObject::Probe(obj);
            if (ar->mException == NULL) {
                PhoneConstantsState state;
                GetState(&state);
                if ((!mDoesRilSendMultipleCallRing)
                        && ((state == PhoneConstantsState_RINGING) ||
                                (state == PhoneConstantsState_IDLE))) {
                    mCallRingContinueToken += 1;
                    SendIncomingCallRingNotification(mCallRingContinueToken);
                }
                else {
                    NotifyIncomingRing();
                }
            }
            break;
        }

        case EVENT_CALL_RING_CONTINUE: {
            PhoneConstantsState state;
            GetState(&state);
            Logger::D(TAG, "Event EVENT_CALL_RING_CONTINUE Received stat=%d", state);
            if (state == PhoneConstantsState_RINGING) {
                Int32 arg1 = 0;
                msg->GetArg1(&arg1);
                SendIncomingCallRingNotification(arg1);
            }
            break;
        }

        case EVENT_ICC_CHANGED: {
            OnUpdateIccAvailability();
            break;
        }

        // handle the select network completion callbacks.
        case EVENT_SET_NETWORK_MANUAL_COMPLETE:
        case EVENT_SET_NETWORK_AUTOMATIC_COMPLETE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            HandleSetSelectNetwork((AsyncResult*) IObject::Probe(obj));
            break;
        }

        case EVENT_INITIATE_SILENT_REDIAL: {
            Logger::D(TAG, "Event EVENT_INITIATE_SILENT_REDIAL Received");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*) IObject::Probe(obj);
            if ((ar->mException == NULL) && (ar->mResult != NULL)) {
                String dialString;
                ICharSequence::Probe(ar->mResult)->ToString(&dialString);
                if (TextUtils::IsEmpty(dialString)) return NOERROR;
                AutoPtr<IConnection> c;
                if (FAILED(DialInternal(dialString, NULL, IVideoProfileVideoState::AUDIO_ONLY, (IConnection**)&c))) {
                    Logger::E(TAG, "silent redial failed: ");
                }
            }
            break;
        }

        case EVENT_SRVCC_STATE_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IObject::Probe(obj);
            if (ar->mException == NULL) {
                assert(0 && "TODO");
                // HandleSrvccStateChanged((Int32[]) ar.result);
            }
            else {
                Logger::E(TAG, "Srvcc exception: %s", TO_CSTR(ar->mException));
            }
            break;
        }

        case EVENT_UNSOL_OEM_HOOK_RAW: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IObject::Probe(obj);
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> byteArray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                byteArray->GetLength(&size);
                AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
                for(Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> ele;
                    byteArray->Get(i, (IInterface**)&ele);
                    Byte b;
                    IByte::Probe(ele)->GetValue(&b);
                    data->Set(i, b);
                }

                Logger::D(TAG, "EVENT_UNSOL_OEM_HOOK_RAW data=%s", IccUtils::BytesToHexString(data).string());
                Int64 iv = 0;
                GetSubId(&iv);
                mNotifier->NotifyOemHookRawEventForSubscriber(iv, data);
            }
            else {
                Logger::E(TAG, "OEM hook raw exception: %s", TO_CSTR(ar->mException));
            }
            break;
        }

        default: {
            // throw new RuntimeException("unexpected event not handled");
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

void PhoneBase::HandleSrvccStateChanged(
    /* [in] */ ArrayOf<Int32>* ret)
{
    Logger::D(TAG, "HandleSrvccStateChanged");

    AutoPtr<IArrayList> conn;/*<Connection*/
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    ICallSrvccState srvccState = ICallSrvccState_NONE;
    if (ret != NULL && ret->GetLength() != 0) {
        Int32 state = (*ret)[0];
        switch(state) {
            case IVoLteServiceState::HANDOVER_STARTED:
                srvccState = ICallSrvccState_STARTED;
                if (imsPhone != NULL) {
                    imsPhone->GetHandoverConnection((IArrayList**)&conn);
                    MigrateFrom(IPhoneBase::Probe(imsPhone));
                }
                else {
                    Logger::D(TAG, "HANDOVER_STARTED: mImsPhone NULL");
                }
                break;
            case IVoLteServiceState::HANDOVER_COMPLETED:
                srvccState = ICallSrvccState_COMPLETED;
                if (imsPhone != NULL) {
                    imsPhone->NotifySrvccState(srvccState);
                }
                else {
                    Logger::D(TAG, "HANDOVER_COMPLETED: mImsPhone NULL");
                }
                break;
            case IVoLteServiceState::HANDOVER_FAILED:
            case IVoLteServiceState::HANDOVER_CANCELED:
                srvccState = ICallSrvccState_FAILED;
                break;

            default:
                //ignore invalid state
                return;
        }

        AutoPtr<ICallTracker> ct;
        GetCallTracker((ICallTracker**)&ct);
        ((CallTracker*)ct.Get())->NotifySrvccState(srvccState, conn);

        AutoPtr<IVoLteServiceState> lteState;
        CVoLteServiceState::New(state, (IVoLteServiceState**)&lteState);
        NotifyVoLteServiceStateChanged(lteState);
    }
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = mContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

// Set the Card into the Phone Book.
void PhoneBase::SetCardInPhoneBook()
{
}

/**
 * Disables the DNS check (i.e., allows "0.0.0.0").
 * Useful for lab testing environment.
 * @param b TRUE disables the check, FALSE enables.
 */
// @Override
ECode PhoneBase::DisableDnsCheck(
    /* [in] */ Boolean b)
{
    mDnsCheckDisabled = b;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(DNS_SERVER_CHECK_DISABLED_KEY, b);
    editor->Apply();
    return NOERROR;
}

/**
 * Returns TRUE if the DNS check is currently disabled.
 */
// @Override
ECode PhoneBase::IsDnsCheckDisabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDnsCheckDisabled;
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mPreciseCallStateRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h)
{
    mPreciseCallStateRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Subclasses of Phone probably want to replace this with a
 * version scoped to their packages
 */
void PhoneBase::NotifyPreciseCallStateChangedP()
{
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, this->Probe(EIID_IInterface), NULL);
    mPreciseCallStateRegistrants->NotifyRegistrants(ar);

    mNotifier->NotifyPreciseCallState(this);
}

// @Override
ECode PhoneBase::RegisterForHandoverStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));
    mHandoverRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForHandoverStateChanged(
    /* [in] */ IHandler* h)
{
    mHandoverRegistrants->Remove(h);
    return NOERROR;
}

/**
 * Subclasses of Phone probably want to replace this with a
 * version scoped to their packages
 */
ECode PhoneBase::NotifyHandoverStateChanged(
    /* [in] */ IConnection* cn)
{
   AutoPtr<AsyncResult> ar = new AsyncResult(NULL, cn, NULL);
   mHandoverRegistrants->NotifyRegistrants(ar);
   return NOERROR;
}

ECode PhoneBase::MigrateFrom(
    /* [in] */ IPhoneBase* _from)
{
    PhoneBase* from = (PhoneBase*)_from;
    Migrate(mHandoverRegistrants, from->mHandoverRegistrants);
    Migrate(mPreciseCallStateRegistrants, from->mPreciseCallStateRegistrants);
    Migrate(mNewRingingConnectionRegistrants, from->mNewRingingConnectionRegistrants);
    Migrate(mIncomingRingRegistrants, from->mIncomingRingRegistrants);
    Migrate(mDisconnectRegistrants, from->mDisconnectRegistrants);
    Migrate(mServiceStateRegistrants, from->mServiceStateRegistrants);
    Migrate(mMmiCompleteRegistrants, from->mMmiCompleteRegistrants);
    Migrate(mMmiRegistrants, from->mMmiRegistrants);
    Migrate(mUnknownConnectionRegistrants, from->mUnknownConnectionRegistrants);
    Migrate(mSuppServiceFailedRegistrants, from->mSuppServiceFailedRegistrants);
    return NOERROR;
}

ECode PhoneBase::Migrate(
    /* [in] */ IRegistrantList* _to,
    /* [in] */ IRegistrantList* _from)
{
    RegistrantList* to = (RegistrantList*)_to;
    RegistrantList* from = (RegistrantList*)_from;
    from->RemoveCleared();
    for (Int32 i = 0, n = from->GetSize(); i < n; i++) {
        to->Add((Registrant*) IObject::Probe(from->Get(i)));
    }
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForUnknownConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mUnknownConnectionRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForUnknownConnection(
    /* [in] */ IHandler* h)
{
    mUnknownConnectionRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForNewRingingConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mNewRingingConnectionRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForNewRingingConnection(
    /* [in] */ IHandler* h)
{
    mNewRingingConnectionRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForVideoCapabilityChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mVideoCapabilityChangedRegistrants->AddUnique(h, what, obj);

    // Notify any registrants of the cached video capability as soon as they register.
    NotifyForVideoCapabilityChanged(mIsVideoCapable);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForVideoCapabilityChanged(
    /* [in] */ IHandler* h)
{
    mVideoCapabilityChangedRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForInCallVoicePrivacyOn(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForInCallVoicePrivacyOn(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForInCallVoicePrivacyOff(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForInCallVoicePrivacyOff(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForIncomingRing(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mIncomingRingRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForIncomingRing(
    /* [in] */ IHandler* h)
{
    mIncomingRingRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForDisconnect(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mDisconnectRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForDisconnect(
    /* [in] */ IHandler* h)
{
    mDisconnectRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForSuppServiceFailed(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mSuppServiceFailedRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForSuppServiceFailed(
    /* [in] */ IHandler* h)
{
    mSuppServiceFailedRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForMmiInitiate(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mMmiRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForMmiInitiate(
    /* [in] */ IHandler* h)
{
    mMmiRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForMmiComplete(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mMmiCompleteRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForMmiComplete(
    /* [in] */ IHandler* h)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mMmiCompleteRegistrants->Remove(h);
    return NOERROR;
}

ECode PhoneBase::RegisterForSimRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    LogUnexpectedCdmaMethodCall(String("registerForSimRecordsLoaded"));
    return NOERROR;
}

ECode PhoneBase::UnregisterForSimRecordsLoaded(
    /* [in] */ IHandler* h)
{
    LogUnexpectedCdmaMethodCall(String("unregisterForSimRecordsLoaded"));
    return NOERROR;
}

// @Override
ECode PhoneBase::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    // wrap the response message in our own message along with
    // an empty string (to indicate automatic selection) for the
    // operator's id.
    AutoPtr<NetworkSelectMessage> nsm = new NetworkSelectMessage();
    nsm->mMessage = response;
    nsm->mOperatorNumeric = String("");
    nsm->mOperatorAlphaLong = String("");

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_NETWORK_AUTOMATIC_COMPLETE, nsm->Probe(EIID_IInterface), (IMessage**)&msg);
    mCi->SetNetworkSelectionModeAutomatic(msg);
    return NOERROR;
}

// @Override
ECode PhoneBase::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    // wrap the response message in our own message along with
    // the operator's id.
    AutoPtr<NetworkSelectMessage> nsm = new NetworkSelectMessage();
    nsm->mMessage = response;
    network->GetOperatorNumeric(&nsm->mOperatorNumeric);
    network->GetOperatorAlphaLong(&nsm->mOperatorAlphaLong);

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_NETWORK_MANUAL_COMPLETE, nsm->Probe(EIID_IInterface), (IMessage**)&msg);
    String s;
    network->GetRadioTech(&s);
    if (s.Equals("")) {
        String v;
        network->GetOperatorNumeric(&v);
        mCi->SetNetworkSelectionModeManual(v, msg);
    }
    else {
        String v;
        network->GetOperatorNumeric(&v);
        mCi->SetNetworkSelectionModeManual(v + "+" + s, msg);
    }
    return NOERROR;
}

/**
 * Used to track the settings upon completion of the network change.
 */
void PhoneBase::HandleSetSelectNetwork(
    /* [in] */ AsyncResult* ar)
{
    // look for our wrapper within the asyncresult, skip the rest if it
    // is NULL.
    if (IPhoneBaseNetworkSelectMessage::Probe(ar->mUserObj) == NULL) {
        Logger::E(TAG, "unexpected result from user object.");
        return;
    }

    NetworkSelectMessage* nsm = (NetworkSelectMessage*)IPhoneBaseNetworkSelectMessage::Probe(ar->mUserObj);

    // found the object, now we send off the message we had originally
    // attached to the request.
    if (nsm->mMessage != NULL) {
        AsyncResult::ForMessage(nsm->mMessage, ar->mResult, ar->mException);
        nsm->mMessage->SendToTarget();
    }

    // open the shared preferences editor, and write the value.
    // nsm.mOperatorNumeric is "" if we're in automatic.selection.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(NETWORK_SELECTION_KEY, nsm->mOperatorNumeric);
    editor->PutString(NETWORK_SELECTION_NAME_KEY, nsm->mOperatorAlphaLong);

    // commit and log the result.
    Boolean tmp = FALSE;
    editor->Commit(&tmp);
    if (!tmp) {
        Logger::E(TAG, "failed to commit network selection preference");
    }
}

/**
 * Method to retrieve the saved operator id from the Shared Preferences
 */
String PhoneBase::GetSavedNetworkSelection()
{
    // open the shared preferences and search with our key.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    String v;
    sp->GetString(NETWORK_SELECTION_KEY, String(""), &v);
    return v;
}

/**
 * Method to restore the previously saved operator id, or reset to
 * automatic selection, all depending upon the value in the shared
 * preferences.
 */
ECode PhoneBase::RestoreSavedNetworkSelection(
    /* [in] */ IMessage* response)
{
    // retrieve the operator id
    String networkSelection = GetSavedNetworkSelection();

    // set to auto if the id is empty, otherwise select the network.
    if (TextUtils::IsEmpty(networkSelection)) {
        mCi->SetNetworkSelectionModeAutomatic(response);
    }
    else {
        mCi->SetNetworkSelectionModeManual(networkSelection, response);
    }
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::SetUnitTestMode(
    /* [in] */ Boolean f)
{
    mUnitTestMode = f;
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::GetUnitTestMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUnitTestMode;
    return NOERROR;
}

/**
 * To be invoked when a voice call Connection disconnects.
 *
 * Subclasses of Phone probably want to replace this with a
 * version scoped to their packages
 */
void PhoneBase::NotifyDisconnectP(
    /* [in] */ IConnection* cn)
{
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, cn, NULL);
    mDisconnectRegistrants->NotifyRegistrants(ar);
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForServiceStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckCorrectThread(h));

    mServiceStateRegistrants->Add(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForServiceStateChanged(
    /* [in] */ IHandler* h)
{
    mServiceStateRegistrants->Remove(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForRingbackTone(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForRingbackTone(h);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForOnHoldTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForOnHoldTone(
    /* [in] */ IHandler* h)
{
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::RegisterForResendIncallMute(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForResendIncallMute(h, what, obj);
    return NOERROR;
}

// Inherited documentation suffices.
// @Override
ECode PhoneBase::UnregisterForResendIncallMute(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForResendIncallMute(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::SetEchoSuppressionEnabled()
{
    // no need for regular phone
    return NOERROR;
}

/**
 * Subclasses of Phone probably want to replace this with a
 * version scoped to their packages
 */
void PhoneBase::NotifyServiceStateChangedP(
    /* [in] */ IServiceState* ss)
{
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, ss, NULL);
    mServiceStateRegistrants->NotifyRegistrants(ar);

    mNotifier->NotifyServiceState(this);
}

// Inherited documentation suffices.
// @Override
// ECode PhoneBase::GetSimulatedRadioControl(
//     /* [out] */ ISimulatedRadioControl** result)
// {
//     VALIDATE_NOT_NULL(result);
//     *result = mSimulatedRadioControl;
//     REFCOUNT_ADD(*result);
//     return NOERROR;
// }

/**
 * Verifies the current thread is the same as the thread originally
 * used in the initialization of this instance. Throws RuntimeException
 * if not.
 *
 * @exception RuntimeException if the current thread is not
 * the thread that originally obtained this PhoneBase instance.
 */
ECode PhoneBase::CheckCorrectThread(
    /* [in] */ IHandler* h)
{
    AutoPtr<ILooper> looper;
    h->GetLooper((ILooper**)&looper);
    if (looper.Get() != mLooper) {
        // throw new RuntimeException(
        //         "com.android.internal.telephony.Phone must be used from within one thread");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

/**
 * Set the properties by matching the carrier string in
 * a string-array resource
 */
void PhoneBase::SetPropertiesByCarrier()
{
    String carrier;
    SystemProperties::Get(String("ro.carrier"), &carrier);

    if (NULL == carrier || 0 == carrier.GetLength() || carrier.Equals("unknown")) {
        return;
    }

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<ICharSequence*> > carrierLocales;
    res->GetTextArray(R::array::carrier_properties, (ArrayOf<ICharSequence*>**)&carrierLocales);

    for (Int32 i = 0; i < carrierLocales->GetLength(); i+=3) {
        String c;
        (*carrierLocales)[i]->ToString(&c);
        if (carrier.Equals(c)) {
            AutoPtr<ILocale> l;
            String sv;
            (*carrierLocales)[i + 1]->ToString(&sv);
            AutoPtr<ILocaleHelper> helper;
            AutoPtr<ILocale> locale;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
            helper->ForLanguageTag(sv.Replace('_', '-'), (ILocale**)&l);
            String country;
            l->GetCountry(&country);
            assert(0 && "TODO");
            // MccTable::SetSystemLocale(mContext, l.getLanguage(), country);

            if (!country.IsEmpty()) {
                AutoPtr<IContentResolver> cr;
                mContext->GetContentResolver((IContentResolver**)&cr);
                Int32 t = 0;
                if (FAILED(Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_COUNTRY_CODE, &t))) {
                    // note this is not persisting
                    AutoPtr<IInterface> w;
                    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&w);
                    AutoPtr<IWifiManager> wM = IWifiManager::Probe(w);
                    wM->SetCountryCode(country, FALSE);
                }
            }
            return;
        }
    }
}

/**
 * Retrieves the IccFileHandler of the Phone instance
 */
ECode PhoneBase::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> uiccApplication = IUiccCardApplication::Probe(obj);
    AutoPtr<IIccFileHandler> fh;

    if (uiccApplication == NULL) {
        Logger::D(TAG, "getIccFileHandler: uiccApplication == NULL, return NULL");
        fh = NULL;
    }
    else {
        uiccApplication->GetIccFileHandler((IIccFileHandler**)&fh);
    }

    Logger::D(TAG, "getIccFileHandler: fh=%s", TO_CSTR(fh));
    *result = fh;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Retrieves the IccRecords of the Phone instance
 */
ECode PhoneBase::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> uiccApplication = IUiccCardApplication::Probe(obj);
    if (uiccApplication == NULL) {
        *result = NULL;
        return NOERROR;
    }
    return uiccApplication->GetIccRecords(result);
}

/*
 * Retrieves the Handler of the Phone instance
 */
ECode PhoneBase::GetHandler(
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result);
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode PhoneBase::UpdatePhoneObject(
    /* [in] */ Int32 voiceRadioTech)
{
    Logger::D(TAG, "updatePhoneObject: phoneid = %d rat = %d", mPhoneId, voiceRadioTech);
    // Only the PhoneProxy can update the phone object.
    AutoPtr<IPhone> phoneProxy;
    AutoPtr<IPhoneFactory> pf;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
    pf->GetPhone(mPhoneId, (IPhone**)&phoneProxy);
    if (phoneProxy != NULL) {
        phoneProxy->UpdatePhoneObject(voiceRadioTech);
    }
    return NOERROR;
}

/**
* Retrieves the ServiceStateTracker of the phone instance.
*/
ECode PhoneBase::GetServiceStateTracker(
    /* [out] */ IServiceStateTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

/**
* Get call tracker
*/
ECode PhoneBase::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode PhoneBase::GetCurrentUiccAppType(
    /* [out] */ AppType* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> currentApp = IUiccCardApplication::Probe(obj);
    if (currentApp != NULL) {
        return currentApp->GetType(result);
    }
    *result = /*AppType.*/APPTYPE_UNKNOWN;
    return NOERROR;
}

// @Override
ECode PhoneBase::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    //throw new Exception("getIccCard Shouldn't be called from PhoneBase");
    return NOERROR;
}

// @Override
ECode PhoneBase::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != NULL) {
        return r->GetIccId(result);
    }
    *result = String(NULL);
    return NOERROR;
}

// @Override
ECode PhoneBase::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != NULL) {
        return r->GetRecordsLoaded(result);
    }
    *result = FALSE;
    return NOERROR;
}

/**
 * @return all available cell information or NULL if none.
 */
// @Override
ECode PhoneBase::GetAllCellInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceStateTracker> sst;
    GetServiceStateTracker((IServiceStateTracker**)&sst);
    AutoPtr<IList> cellInfoList;/*<CellInfo*/
    sst->GetAllCellInfo((IList**)&cellInfoList);
    AutoPtr<IList> l = PrivatizeCellInfoList(cellInfoList);
    *result = l;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Clear CDMA base station lat/Int64 values if location setting is disabled.
 * @param cellInfoList the original cell info list from the RIL
 * @return the original list with CDMA lat/Int64 cleared if necessary
 */
AutoPtr<IList/*<CellInfo*/> PhoneBase::PrivatizeCellInfoList(
    /* [in] */ IList/*<CellInfo>*/* _cellInfoList)
 {
    AutoPtr<IList> cellInfoList = _cellInfoList;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Int32 mode = 0;
    Settings::Secure::GetInt32(cr,
            ISettingsSecure::LOCATION_MODE, ISettingsSecure::LOCATION_MODE_OFF, &mode);
    if (mode == ISettingsSecure::LOCATION_MODE_OFF) {
        AutoPtr<IArrayList> privateCellInfoList;/*<CellInfo*/
        Int32 size = 0;
        cellInfoList->GetSize(&size);
        CArrayList::New(size, (IArrayList**)&privateCellInfoList);
        // clear lat/lon values for location privacy
        AutoPtr<IIterator> ator;
        cellInfoList->GetIterator((IIterator**)&ator);
        Boolean has = FALSE;
        while (ator->HasNext(&has), has) {
            AutoPtr<IInterface> c;
            ator->GetNext((IInterface**)&c);

            if (ICellInfoCdma::Probe(c)) {
                AutoPtr<ICellInfoCdma> cellInfoCdma = ICellInfoCdma::Probe(c);
                AutoPtr<ICellIdentityCdma> cellIdentity;
                cellInfoCdma->GetCellIdentity((ICellIdentityCdma**)&cellIdentity);
                AutoPtr<ICellIdentityCdma> maskedCellIdentity;
                Int32 id = 0, sId = 0, bId = 0;
                cellIdentity->GetNetworkId(&id);
                cellIdentity->GetSystemId(&sId);
                cellIdentity->GetBasestationId(&bId);
                CCellIdentityCdma::New(id, sId, bId,
                        Elastos::Core::Math::INT32_MAX_VALUE, Elastos::Core::Math::INT32_MAX_VALUE
                        , (ICellIdentityCdma**)&maskedCellIdentity);
                AutoPtr<ICellInfoCdma> privateCellInfoCdma;
                CCellInfoCdma::New(cellInfoCdma, (ICellInfoCdma**)&privateCellInfoCdma);
                privateCellInfoCdma->SetCellIdentity(maskedCellIdentity);
                privateCellInfoList->Add(privateCellInfoCdma);
            }
            else {
                privateCellInfoList->Add(c);
            }
        }
        cellInfoList = IList::Probe(privateCellInfoList);
    }
    return cellInfoList;
}

/**
 * {@inheritDoc}
 */
// @Override
ECode PhoneBase::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis)
{
    mCi->SetCellInfoListRate(rateInMillis, NULL);
    return NOERROR;
}

// @Override
/** @return TRUE if there are messages waiting, FALSE otherwise. */
ECode PhoneBase::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVmCount != 0;
    return NOERROR;
}

// @Override
ECode PhoneBase::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != NULL) {
        return r->GetVoiceCallForwardingFlag(result);
    }
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(TAG, "setCallForwardingUncondTimerOption error ");
    return NOERROR;
}

// @Override
ECode PhoneBase::GetCallForwardingUncondTimerOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(TAG, "getCallForwardingUncondTimerOption error ");
    return NOERROR;
}

/**
 *  Query the status of the CDMA roaming preference
 */
// @Override
ECode PhoneBase::QueryCdmaRoamingPreference(
    /* [in] */ IMessage* response)
{
    mCi->QueryCdmaRoamingPreference(response);
    return NOERROR;
}

/**
 * Get the signal strength
 */
// @Override
ECode PhoneBase::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceStateTracker> sst;
    GetServiceStateTracker((IServiceStateTracker**)&sst);
    if (sst == NULL) {
        return CSignalStrength::New(result);
    }

    return sst->GetSignalStrength(result);
}

/**
 *  Set the status of the CDMA roaming preference
 */
// @Override
ECode PhoneBase::SetCdmaRoamingPreference(
    /* [in] */ Int32 cdmaRoamingType,
    /* [in] */ IMessage* response)
{
    mCi->SetCdmaRoamingPreference(cdmaRoamingType, response);
    return NOERROR;
}

/**
 *  Set the status of the CDMA subscription mode
 */
// @Override
ECode PhoneBase::SetCdmaSubscription(
    /* [in] */ Int32 cdmaSubscriptionType,
    /* [in] */ IMessage* response)
{
    mCi->SetCdmaSubscriptionSource(cdmaSubscriptionType, response);
    return NOERROR;
}

/**
 *  Set the preferred Network Type: Global, CDMA only or GSM/UMTS only
 */
// @Override
ECode PhoneBase::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessage* response)
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 value = 0;
    tm->GetPhoneCount(&value);
    if (value > 1) {
        assert(0 && "TODO");
        GetPhoneId(&value);
        // ModemBindingPolicyHandler::GetInstance()->SetPreferredNetworkType(networkType,
        //         value,response);
    }
    else {
        mCi->SetPreferredNetworkType(networkType, response);
    }
    return NOERROR;
}

// @Override
ECode PhoneBase::GetPreferredNetworkType(
    /* [in] */ IMessage* response)
{
    mCi->GetPreferredNetworkType(response);
    return NOERROR;
}

// @Override
ECode PhoneBase::GetSmscAddress(
    /* [in] */ IMessage* result)
{
    mCi->GetSmscAddress(result);
    return NOERROR;
}

// @Override
ECode PhoneBase::SetSmscAddress(
    /* [in] */ const String& address,
    /* [in] */ IMessage* result)
{
    mCi->SetSmscAddress(address, result);
    return NOERROR;
}

// @Override
ECode PhoneBase::SetTTYMode(
    /* [in] */ Int32 ttyMode,
    /* [in] */ IMessage* onComplete)
{
    mCi->SetTTYMode(ttyMode, onComplete);
    return NOERROR;
}

// @Override
ECode PhoneBase::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    Logger::D(TAG, "unexpected setUiTTYMode method call");
    return NOERROR;
}

// @Override
ECode PhoneBase::QueryTTYMode(
    /* [in] */ IMessage* onComplete)
{
    mCi->QueryTTYMode(onComplete);
    return NOERROR;
}

// @Override
ECode PhoneBase::EnableEnhancedVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("enableEnhancedVoicePrivacy"));
    return NOERROR;
}

// @Override
ECode PhoneBase::GetEnhancedVoicePrivacy(
    /* [in] */ IMessage* onComplete)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("getEnhancedVoicePrivacy"));
    return NOERROR;
}

// @Override
ECode PhoneBase::SetBandMode(
    /* [in] */ Int32 bandMode,
    /* [in] */ IMessage* response)
{
    mCi->SetBandMode(bandMode, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::QueryAvailableBandMode(
    /* [in] */ IMessage* response)
{
    mCi->QueryAvailableBandMode(response);
    return NOERROR;
}

// @Override
ECode PhoneBase::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* response)
{
    mCi->InvokeOemRilRequestRaw(data, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::InvokeOemRilRequestStrings(
    /* [in] */ ArrayOf<String>* strings,
    /* [in] */ IMessage* response)
{
    mCi->InvokeOemRilRequestStrings(strings, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [in] */ IMessage* response)
{
    mCi->NvReadItem(itemID, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [in] */ IMessage* response)
{
    mCi->NvWriteItem(itemID, itemValue, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [in] */ IMessage* response)
{
    mCi->NvWriteCdmaPrl(preferredRoamingList, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [in] */ IMessage* response)
{
    mCi->NvResetConfig(resetType, response);
    return NOERROR;
}

// @Override
ECode PhoneBase::NotifyDataActivity()
{
    mNotifier->NotifyDataActivity(this);
    return NOERROR;
}

ECode PhoneBase::NotifyMessageWaitingIndicator()
{
    // Do not notify voice mail waiting if device doesn't support voice
    if (!mIsVoiceCapable) {
        return NOERROR;
    }

    // This function is added to send the notification to DefaultPhoneNotifier.
    mNotifier->NotifyMessageWaitingChanged(this);
    return NOERROR;
}

ECode PhoneBase::NotifyDataConnection(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ PhoneConstantsDataState state)
{
    mNotifier->NotifyDataConnection(this, reason, apnType, state);
    return NOERROR;
}

ECode PhoneBase::NotifyDataConnection(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType)
{
    PhoneConstantsDataState ds;
    GetDataConnectionState(apnType, &ds);
    mNotifier->NotifyDataConnection(this, reason, apnType, ds);
    return NOERROR;
}

ECode PhoneBase::NotifyDataConnection(
    /* [in] */ const String& reason)
{
    AutoPtr<ArrayOf<String> > types;
    GetActiveApnTypes((ArrayOf<String>**)&types);
    for (Int32 i = 0; i < types->GetLength(); i++) {
        String apnType = (*types)[i];
        PhoneConstantsDataState ds;
        GetDataConnectionState(apnType, &ds);
        mNotifier->NotifyDataConnection(this, reason, apnType, ds);
    }
    return NOERROR;
}

ECode PhoneBase::NotifyOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    mNotifier->NotifyOtaspChanged(this, otaspMode);
    return NOERROR;
}

ECode PhoneBase::NotifySignalStrength()
{
    mNotifier->NotifySignalStrength(this);
    return NOERROR;
}

ECode PhoneBase::NotifyCellInfo(
    /* [in] */ IList/*<CellInfo>*/* cellInfo)
{
    mNotifier->NotifyCellInfo(this, PrivatizeCellInfoList(cellInfo));
    return NOERROR;
}

ECode PhoneBase::NotifyDataConnectionRealTimeInfo(
    /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo)
{
    mNotifier->NotifyDataConnectionRealTimeInfo(this, dcRtInfo);
    return NOERROR;
}

ECode PhoneBase::NotifyVoLteServiceStateChanged(
    /* [in] */ IVoLteServiceState* lteState)
{
    mNotifier->NotifyVoLteServiceStateChanged(this, lteState);
    return NOERROR;
}

/**
 * @return TRUE if a mobile originating emergency call is active
 */
ECode PhoneBase::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * @return TRUE if we are in the emergency call back mode. This is a period where
 * the phone should be using as little power as possible and be ready to receive an
 * incoming call from the emergency operator.
 */
ECode PhoneBase::IsInEcm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

Int32 PhoneBase::GetVideoState(
    /* [in] */ ICall* call)
{
    Int32 videoState = IVideoProfileVideoState::AUDIO_ONLY;
    // ImsPhoneConnection conn = (ImsPhoneConnection) call->GetEarliestConnection();
    AutoPtr<IConnection> conn;
    call->GetEarliestConnection((IConnection**)&conn);
    if (conn != NULL) {
        conn->GetVideoState(&videoState);
    }
    return videoState;
}

Boolean PhoneBase::IsImsVideoCall(
    /* [in] */ ICall* call)
{
    Int32 videoState = GetVideoState(call);
    Boolean tmp = FALSE;
    VideoProfile::VideoState::IsVideo(videoState, &tmp);
    return tmp;
}

ECode PhoneBase::IsImsVtCallPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isVideoCallActive = FALSE;
    if (mImsPhone != NULL) {
        AutoPtr<ICall> c1;
        AutoPtr<ICall> c2;
        AutoPtr<ICall> c3;
        isVideoCallActive = IsImsVideoCall((IPhone::Probe(mImsPhone)->GetForegroundCall((ICall**)&c1), c1)) ||
                IsImsVideoCall((IPhone::Probe(mImsPhone)->GetBackgroundCall((ICall**)&c2), c2)) ||
                IsImsVideoCall((IPhone::Probe(mImsPhone)->GetRingingCall((ICall**)&c3), c3));
    }
    Logger::D(TAG, "isVideoCallActive: %d", isVideoCallActive);
    *result = isVideoCallActive;
    return NOERROR;
}

/** @hide */
/** @return number of voicemails */
// @Override
ECode PhoneBase::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVmCount;
    return NOERROR;
}

/** sets the voice mail count of the phone and notifies listeners. */
ECode PhoneBase::SetVoiceMessageCount(
    /* [in] */ Int32 countWaiting)
{
    mVmCount = countWaiting;
    // notify listeners of voice mail
    NotifyMessageWaitingIndicator();
    return NOERROR;
}

/**
 * Returns the CDMA ERI icon index to display
 */
// @Override
ECode PhoneBase::GetCdmaEriIconIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    LogUnexpectedCdmaMethodCall(String("getCdmaEriIconIndex"));
    *result = -1;
    return NOERROR;
}

/**
 * Returns the CDMA ERI icon mode,
 * 0 - ON
 * 1 - FLASHING
 */
// @Override
ECode PhoneBase::GetCdmaEriIconMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    LogUnexpectedCdmaMethodCall(String("getCdmaEriIconMode"));
    *result = -1;
    return NOERROR;
}

/**
 * Returns the CDMA ERI text,
 */
// @Override
ECode PhoneBase::GetCdmaEriText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    LogUnexpectedCdmaMethodCall(String("getCdmaEriText"));
    *result = String("GSM nw, no ERI");
    return NOERROR;
}

// @Override
ECode PhoneBase::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("getCdmaMin"));
    *result = String(NULL);
    return NOERROR;
}

// @Override
ECode PhoneBase::IsMinInfoReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("isMinInfoReady"));
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::GetCdmaPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    //  This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("getCdmaPrlVersion"));
    *result = String(NULL);
    return NOERROR;
}

// @Override
ECode PhoneBase::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* onComplete)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("sendBurstDtmf"));
    return NOERROR;
}

// @Override
ECode PhoneBase::ExitEmergencyCallbackMode()
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("exitEmergencyCallbackMode"));
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("registerForCdmaOtaStatusChange"));
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("unregisterForCdmaOtaStatusChange"));
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("registerForSubscriptionInfoReady"));
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("unregisterForSubscriptionInfoReady"));
    return NOERROR;
}

/**
 * Returns TRUE if OTA Service Provisioning needs to be performed.
 * If not overridden return FALSE.
 */
// @Override
ECode PhoneBase::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Return TRUE if number is an OTASP number.
 * If not overridden return FALSE.
 */
// @Override
ECode PhoneBase::IsOtaSpNumber(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("registerForCallWaiting"));
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("unregisterForCallWaiting"));
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("registerForEcmTimerReset"));
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("unregisterForEcmTimerReset"));
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForSignalInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForSignalInfo(h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForSignalInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForSignalInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForDisplayInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForDisplayInfo(h, what, obj);
    return NOERROR;
}

 // @Override
ECode PhoneBase::UnregisterForDisplayInfo(
    /* [in] */ IHandler* h)
{
     mCi->UnregisterForDisplayInfo(h);
     return NOERROR;
 }

// @Override
ECode PhoneBase::RegisterForNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForNumberInfo(h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForNumberInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForNumberInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForRedirectedNumberInfo(h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForRedirectedNumberInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForLineControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForLineControlInfo( h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForLineControlInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForLineControlInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterFoT53ClirlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterFoT53ClirlInfo(h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForT53ClirInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForT53ClirInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::RegisterForT53AudioControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mCi->RegisterForT53AudioControlInfo( h, what, obj);
    return NOERROR;
}

// @Override
ECode PhoneBase::UnregisterForT53AudioControlInfo(
    /* [in] */ IHandler* h)
{
    mCi->UnregisterForT53AudioControlInfo(h);
    return NOERROR;
}

// @Override
ECode PhoneBase::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("setOnEcbModeExitResponse"));
    return NOERROR;
}

// @Override
ECode PhoneBase::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
    LogUnexpectedCdmaMethodCall(String("unsetOnEcbModeExitResponse"));
    return NOERROR;
}

// @Override
ECode PhoneBase::GetActiveApnTypes(
    /* [out] */ ArrayOf<String>** types)
{
    VALIDATE_NOT_NULL(types);
    return mDcTracker->GetActiveApnTypes(types);
}

// @Override
ECode PhoneBase::GetActiveApnHost(
    /* [in] */ const String& apnType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetActiveApnString(apnType, result);
}

// @Override
ECode PhoneBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetLinkProperties(apnType, result);
}

// @Override
ECode PhoneBase::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetNetworkCapabilities(apnType, result);
}

// @Override
ECode PhoneBase::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IsDataConnectivityPossible(IPhoneConstants::APN_TYPE_DEFAULT, result);
}

// @Override
ECode PhoneBase::IsOnDemandDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = ((mDcTracker != NULL) &&
            (mDcTracker->IsOnDemandDataPossible(apnType, &tmp), tmp));
    return NOERROR;
}

// @Override
ECode PhoneBase::IsDataConnectivityPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = ((mDcTracker != NULL) &&
            (mDcTracker->IsDataPossible(apnType, &tmp), tmp));
    return NOERROR;
}

/**
 * Notify registrants of a new ringing Connection.
 * Subclasses of Phone probably want to replace this with a
 * version scoped to their packages
 */
ECode PhoneBase::NotifyNewRingingConnectionP(
    /* [in] */ IConnection* cn)
{
    if (!mIsVoiceCapable) {
        return NOERROR;
    }
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, cn, NULL);
    mNewRingingConnectionRegistrants->NotifyRegistrants(ar);
    return NOERROR;
}


/**
 * Notify registrants if phone is video capable.
 */
ECode PhoneBase::NotifyForVideoCapabilityChanged(
    /* [in] */ Boolean isVideoCallCapable)
{
    // Cache the current video capability so that we don't lose the information.
    mIsVideoCapable = isVideoCallCapable;

    AutoPtr<IBoolean> v;
    CBoolean::New(isVideoCallCapable, (IBoolean**)&v);
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, v, NULL);
    mVideoCapabilityChangedRegistrants->NotifyRegistrants(ar);
    return NOERROR;
}

/**
 * Notify registrants of a RING event.
 */
void PhoneBase::NotifyIncomingRing()
{
    if (!mIsVoiceCapable) {
        return;
    }
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, this->Probe(EIID_IInterface), NULL);
    mIncomingRingRegistrants->NotifyRegistrants(ar);
}

/**
 * Send the incoming call Ring notification if conditions are right.
 */
void PhoneBase::SendIncomingCallRingNotification(
    /* [in] */ Int32 token)
{
    if (mIsVoiceCapable && !mDoesRilSendMultipleCallRing &&
            (token == mCallRingContinueToken)) {
        Logger::D(TAG, "Sending NotifyIncomingRing");
        NotifyIncomingRing();
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_CALL_RING_CONTINUE, token, 0, (IMessage**)&msg);
        Boolean tmp = FALSE;
        SendMessageDelayed(msg, mCallRingDelay, &tmp);
    }
    else {
        Logger::D(TAG,
            "Ignoring ring notification request, mDoesRilSendMultipleCallRing=%d token=%d mCallRingContinueToken=%d mIsVoiceCapable=%d"
            , mDoesRilSendMultipleCallRing,  token , mCallRingContinueToken, mIsVoiceCapable);
    }
}

ECode PhoneBase::IsManualNetSelAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // This function should be overridden in GsmPhone.
    // Not implemented by default.
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // This function should be overridden by the class GSMPhone.
    // Not implemented in CDMAPhone.
    LogUnexpectedGsmMethodCall(String("isCspPlmnEnabled"));
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(TAG, "getIsimRecords() is only supported on LTE devices");
    *result = NULL;
    return NOERROR;
}

// @Override
ECode PhoneBase::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    LogUnexpectedGsmMethodCall(String("getMsisdn"));
    *result = String(NULL);
    return NOERROR;
}

/**
 * Common error logger method for unexpected calls to CDMA-only methods.
 */
void PhoneBase::LogUnexpectedCdmaMethodCall(
    /* [in] */ const String& name)
{
    Logger::E(TAG, "Error! %s() in PhoneBase should not be called, CDMAPhone inactive.", name.string());
}

// @Override
ECode PhoneBase::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDataConnectionState(IPhoneConstants::APN_TYPE_DEFAULT, result);
}

/**
 * Common error logger method for unexpected calls to GSM/WCDMA-only methods.
 */
void PhoneBase::LogUnexpectedGsmMethodCall(
    /* [in] */ const String& name)
{
    Logger::E(TAG, "Error! %s() in PhoneBase should not be called, GSMPhone inactive.", name.string());
}

// Called by SimRecords which is constructed with a PhoneBase instead of a GSMPhone.
ECode PhoneBase::NotifyCallForwardingIndicator()
{
    // This function should be overridden by the class GSMPhone. Not implemented in CDMAPhone.
    Logger::E(TAG, "Error! This function should never be executed, inactive CDMAPhone.");
    return NOERROR;
}

ECode PhoneBase::NotifyDataConnectionFailed(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType)
{
    mNotifier->NotifyDataConnectionFailed(this, reason, apnType);
    return NOERROR;
}

ECode PhoneBase::NotifyPreciseDataConnectionFailed(
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ const String& apn,
    /* [in] */ const String& failCause)
{
    mNotifier->NotifyPreciseDataConnectionFailed(this, reason, apnType, apn, failCause);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
// @Override
ECode PhoneBase::GetLteOnCdmaMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mCi->GetLteOnCdmaMode(result);
}

/**
 * {@hide}
 */
// @Override
ECode PhoneBase::GetLteOnGsmMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mCi->GetLteOnGsmMode(result);
}

/**
 * Sets the SIM voice message waiting indicator records.
 * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
 * @param countWaiting The number of messages waiting, if known. Use
 *                     -1 to indicate that an unknown number of
 *                      messages are waiting
 */
// @Override
ECode PhoneBase::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // This function should be overridden by class GSMPhone and CDMAPhone.
    Logger::E(TAG, "Error! This function should never be executed, inactive Phone.");
    return NOERROR;
}

/**
 * Gets the USIM service table from the UICC, if present and available.
 * @return an interface to the UsimServiceTable record, or NULL if not available
 */
// @Override
ECode PhoneBase::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != NULL) {
        return r->GetUsimServiceTable(result);
    }
    *result = NULL;
    return NOERROR;
}

/**
 * Gets the Uicc card corresponding to this phone.
 * @return the UiccCard object corresponding to the phone ID.
 */
// @Override
ECode PhoneBase::GetUiccCard(
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result);
    return mUiccController->GetUiccCard(mPhoneId, result);
}

/**
 * Get P-CSCF address from PCO after data connection is established or modified.
 * @param apnType the apnType, "ims" for IMS APN, "emergency" for EMERGENCY APN
 */
// @Override
ECode PhoneBase::GetPcscfAddress(
    /* [in] */ const String& apnType,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    return mDcTracker->GetPcscfAddress(apnType, result);
}

/**
 * Set IMS registration state
 */
// @Override
ECode PhoneBase::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    mDcTracker->SetImsRegistrationState(registered);
    return NOERROR;
}

/**
 * Return an instance of a IMS phone
 */
// @Override
ECode PhoneBase::GetImsPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(mImsPhone);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode PhoneBase::RelinquishOwnershipOfImsPhone(
    /* [out] */ IImsPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mImsLock);
    if (mImsPhone == NULL) {
        *result =  NULL;
        return NOERROR;
    }

    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    mImsPhone = NULL;

    CallManager::GetInstance()->UnregisterPhone(IPhone::Probe(imsPhone));
    imsPhone->UnregisterForSilentRedial(this);

    *result = imsPhone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode PhoneBase::AcquireOwnershipOfImsPhone(
    /* [in] */ IImsPhone* imsPhone)
{
    {
        AutoLock lock(mImsLock);
        if (imsPhone == NULL) {
            return NOERROR;
        }

        if (mImsPhone != NULL) {
            Logger::E(TAG, "acquireOwnershipOfImsPhone: non-NULL mImsPhone. Shouldn't happen - but disposing");
            IPhone::Probe(mImsPhone)->Dispose();
            // Potential GC issue if someone keeps a reference to ImsPhone.
            // However: this change will make sure that such a reference does
            // not access functions through NULL pointer.
            //mImsPhone.removeReferences();
        }

        mImsPhone = imsPhone;

        mImsServiceReady = TRUE;
        mImsPhone->UpdateParentPhone(this);
        Boolean tmp = FALSE;
        CallManager::GetInstance()->RegisterPhone(IPhone::Probe(mImsPhone), &tmp);
        mImsPhone->RegisterForSilentRedial(
                this, EVENT_INITIATE_SILENT_REDIAL, NULL);
    }
    return NOERROR;
}

void PhoneBase::UpdateImsPhone()
{
    {
        AutoLock lock(mImsLock);
        Logger::D(TAG, "UpdateImsPhone mImsServiceReady=%d", mImsServiceReady);

        if (mImsServiceReady && (mImsPhone == NULL)) {
            assert(0 && "TODO");
            // mImsPhone = PhoneFactory::MakeImsPhone(mNotifier, this);
            Boolean tmp = FALSE;
            CallManager::GetInstance()->RegisterPhone(IPhone::Probe(mImsPhone), &tmp);
            mImsPhone->RegisterForSilentRedial(
                    this, EVENT_INITIATE_SILENT_REDIAL, NULL);
        }
        else if (!mImsServiceReady && (mImsPhone != NULL)) {
            CallManager::GetInstance()->UnregisterPhone(IPhone::Probe(mImsPhone));
            mImsPhone->UnregisterForSilentRedial(this);

            IPhone::Probe(mImsPhone)->Dispose();
            // Potential GC issue if someone keeps a reference to ImsPhone.
            // However: this change will make sure that such a reference does
            // not access functions through NULL pointer.
            //mImsPhone.removeReferences();
            mImsPhone = NULL;
        }
    }
}

/**
 * Dials a number.
 *
 * @param dialString The number to dial.
 * @param uusInfo The UUSInfo.
 * @param videoState The video state for the call.
 * @return The Connection.
 * @throws CallStateException
 */
ECode PhoneBase::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result) /*throws CallStateException*/
{
    VALIDATE_NOT_NULL(result);
    // DialInternal shall be overriden by GSMPhone and CDMAPhone
    *result = NULL;
    return NOERROR;
}

ECode PhoneBase::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("PhoneBase:"));
    pw->Println(String(" mCi=") + TO_CSTR(mCi));
    pw->Println(String(" mDnsCheckDisabled=") + StringUtils::ToString(mDnsCheckDisabled));
    pw->Println(String(" mDcTracker=") + TO_CSTR(mDcTracker));
    pw->Println(String(" mDoesRilSendMultipleCallRing=") + StringUtils::ToString(mDoesRilSendMultipleCallRing));
    pw->Println(String(" mCallRingContinueToken=") + StringUtils::ToString(mCallRingContinueToken));
    pw->Println(String(" mCallRingDelay=") + StringUtils::ToString(mCallRingDelay));
    pw->Println(String(" mIsTheCurrentActivePhone=") + StringUtils::ToString(mIsTheCurrentActivePhone));
    pw->Println(String(" mIsVoiceCapable=") + StringUtils::ToString(mIsVoiceCapable));

    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    pw->Println(String(" mIccRecords=") + TO_CSTR(obj));

    obj = NULL;
    mUiccApplication->Get((IInterface**)&obj);
    pw->Println(String(" mUiccApplication=") + TO_CSTR(obj));
    pw->Println(String(" mSmsStorageMonitor=") + TO_CSTR(mSmsStorageMonitor));
    pw->Println(String(" mSmsUsageMonitor=") + TO_CSTR(mSmsUsageMonitor));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mLooper=") + TO_CSTR(mLooper));
    pw->Println(String(" mContext=") + TO_CSTR(mContext));
    pw->Println(String(" mNotifier=") + TO_CSTR(mNotifier));
    // pw->Println(String(" mSimulatedRadioControl=") + TO_CSTR(mSimulatedRadioControl));
    pw->Println(String(" mUnitTestMode=") + StringUtils::ToString(mUnitTestMode));

    Boolean bv = 0;
    IsDnsCheckDisabled(&bv);
    pw->Println(String(" isDnsCheckDisabled()=") + StringUtils::ToString(bv));
    GetUnitTestMode(&bv);
    pw->Println(String(" GetUnitTestMode()=") + StringUtils::ToString(bv));

    PhoneConstantsState cs;
    GetState(&cs);
    pw->Println(String(" getState()=") + StringUtils::ToString(cs));

    String sv;
    GetIccSerialNumber(&sv);
    pw->Println(String(" getIccSerialNumber()=") + sv);

    GetIccRecordsLoaded(&bv);
    pw->Println(String(" getIccRecordsLoaded()=") + StringUtils::ToString(bv));
    GetMessageWaitingIndicator(&bv);
    pw->Println(String(" getMessageWaitingIndicator()=") + StringUtils::ToString(bv));
    GetCallForwardingIndicator(&bv);
    pw->Println(String(" getCallForwardingIndicator()=") + StringUtils::ToString(bv));

    IsInEmergencyCall(&bv);
    pw->Println(String(" isInEmergencyCall()=") + StringUtils::ToString(bv));
    IFlushable::Probe(pw)->Flush();
    IsInEcm(&bv);
    pw->Println(String(" isInEcm()=") + StringUtils::ToString(bv));

    GetPhoneName(&sv);
    pw->Println(String(" getPhoneName()=") + sv);

    Int32 iv = 0;
    GetPhoneType(&iv);
    pw->Println(String(" GetPhoneType()=") + StringUtils::ToString(iv));

    GetVoiceMessageCount(&iv);
    pw->Println(String(" getVoiceMessageCount()=") + StringUtils::ToString(iv));

    AutoPtr<ArrayOf<String> > vs;
    GetActiveApnTypes((ArrayOf<String>**)&vs);
    String ss;
    for (Int32 i = 0; i < vs->GetLength(); i++) {
        ss += (*vs)[i];
    }
    pw->Println(String(" GetActiveApnTypes()=") + ss);
    IsDataConnectivityPossible(&bv);
    pw->Println(String(" isDataConnectivityPossible()=") + StringUtils::ToString(bv));
    NeedsOtaServiceProvisioning(&bv);
    pw->Println(String(" needsOtaServiceProvisioning=") + StringUtils::ToString(bv));
    return NOERROR;
}

/**
 * Returns the subscription id.
 */
ECode PhoneBase::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISubscriptionControllerHelper> helper;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&helper);
    AutoPtr<ISubscriptionController> ssc;
    helper->GetInstance((ISubscriptionController**)&ssc);
    ssc->GetSubIdUsingPhoneId(mPhoneId, result);
    return NOERROR;
}

/**
 * Returns the phone id.
 */
ECode PhoneBase::GetPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPhoneId;
    return NOERROR;
}

//Gets Subscription information in the Phone Object
ECode PhoneBase::GetSubscriptionInfo(
    /* [out] */ ISubscription** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubscriptionData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/**
 * Return the service state of mImsPhone if it is STATE_IN_SERVICE
 * otherwise return the current voice service state
 */
// @Override
ECode PhoneBase::GetVoicePhoneServiceState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IImsPhone> imsPhone = mImsPhone;
    AutoPtr<IServiceState> ss;
    if (imsPhone != NULL) {
        IPhone::Probe(imsPhone)->GetServiceState((IServiceState**)&ss);
        Int32 s = 0;
        if ((ss->GetState(&s), s) == IServiceState::STATE_IN_SERVICE) {
            *result = IServiceState::STATE_IN_SERVICE;
            return NOERROR;
        }
    }

    ss = NULL;
    GetServiceState((IServiceState**)&ss);
    return ss->GetState(result);
}

// @Override
ECode PhoneBase::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

// @Override
ECode PhoneBase::IsRadioAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICommandsInterfaceRadioState rs;
    mCi->GetRadioState(&rs);
    *result = rs != RADIO_UNAVAILABLE;
    return NOERROR;
}

// @Override
ECode PhoneBase::ShutdownRadio()
{
    AutoPtr<IServiceStateTracker> sst;
    GetServiceStateTracker((IServiceStateTracker**)&sst);
    sst->RequestShutdown();
    return NOERROR;
}

ECode PhoneBase::GetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    LogUnexpectedCdmaMethodCall(String("getCallBarringOption"));
    return NOERROR;
}

ECode PhoneBase::SetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    LogUnexpectedCdmaMethodCall(String("setCallBarringOption"));
    return NOERROR;
}

ECode PhoneBase::RequestChangeCbPsw(
    /* [in] */ const String& facility,
    /* [in] */ const String& oldPwd,
    /* [in] */ const String& newPwd,
    /* [in] */ IMessage* result)
{
    LogUnexpectedCdmaMethodCall(String("requestChangeCbPsw"));
    return NOERROR;
}

// @Override
ECode PhoneBase::SetLocalCallHold(
    /* [in] */ Int32 lchStatus)
{
    mCi->SetLocalCallHold(lchStatus);
    return NOERROR;
}

// @Override
ECode PhoneBase::DeflectCall(
    /* [in] */ const String& number) /*throws CallStateException*/
{
    // throw new CallStateException("Unexpected deflectCall method call");
    assert(0 && "TODO: CallStateException");
    return NOERROR;
}

// @Override
ECode PhoneBase::AddParticipant(
    /* [in] */ const String& dialString) /*throws CallStateException*/
{
    // throw new CallStateException("addParticipant is not supported in this phone " + this);
    assert(0 && "TODO: CallStateException");
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
