
#include "elastos/droid/phone/COtaStartupReceiver.h"

namespace Elastos {
namespace Droid {
namespace Phone {

ECode COtaStartupReceiver::MyPhoneStateListener::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    if (mOtaspMode == otaspMode) {
        return NOERROR;
    }
    mOtaspMode = otaspMode;
    StringBuilder sb;
    sb += "onOtaspChanged: mOtaspMode=";
    sb += mOtaspMode;
    Logger::V(TAG, sb.ToString());

    if (otaspMode == IServiceStateTracker::OTASP_NEEDED) {
        Logger::I(TAG, "OTASP is needed - performing CDMA provisioning");
        AutoPtr<IIntent> intent;
        CIntent::New(OtaUtils::ACTION_PERFORM_CDMA_PROVISIONING, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mContext->StartActivity(intent);
    }
    return NOERROR;
}

COtaStartupReceiver::MyHandler::MyHandler(
    /* [in] */ COtaStartupReceiver* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode COtaStartupReceiver::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MIN_READY:
        {
            StringBuilder sb;
            sb += "Attempting OtaActivation from handler, mOtaspMode=";
            sb += mOtaspMode;
            Logger::V(TAG, sb.ToString());
            OtaUtils::MaybeDoOtaCall(mContext, mHandler, MIN_READY);
            break;
        }
        case SERVICE_STATE_CHANGED:
        {
            ServiceState state = (ServiceState) ((AsyncResult) msg.obj).result;
            StringBuilder sb;
            sb += "onServiceStateChanged()...  new state = ";
            sb += state;
            if (DBG) Logger::D(TAG, sb.ToString());

            // Possible service states:
            // - STATE_IN_SERVICE        // Normal operation
            // - STATE_OUT_OF_SERVICE    // Still searching for an operator to register to,
            //                           // or no radio signal
            // - STATE_EMERGENCY_ONLY    // Phone is locked; only emergency numbers are allowed
            // - STATE_POWER_OFF         // Radio is explicitly powered off (airplane mode)

            // Once we reach STATE_IN_SERVICE
            // it's finally OK to start OTA provisioning
            Int32 state;
            state->GetState(&state);
            if (state == IServiceState::STATE_IN_SERVICE) {
                if (DBG) Logger::D(TAG, "call OtaUtils.maybeDoOtaCall after network is available");
                AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
                phone->UnregisterForServiceStateChanged(this);
                OtaUtils::MaybeDoOtaCall(mContext, mHandler, MIN_READY);
            }
            break;
        }
    }
    return NOERROR;
}

const String COtaStartupReceiver::TAG("OtaStartupReceiver");
const Boolean COtaStartupReceiver::DBG = FALSE;
const Int32 COtaStartupReceiver::MIN_READY = 10;
const Int32 COtaStartupReceiver::SERVICE_STATE_CHANGED = 11;

CAR_INTERFACE_IMPL(COtaStartupReceiver, BroadcastReceiver, IOtaStartupReceiver)

CAR_OBJECT_IMPL(COtaStartupReceiver)

COtaStartupReceiver::COtaStartupReceiver()
    : mOtaspMode(-1)
    , mPhoneStateListenerRegistered(FALSE)
{
    mPhoneStateListener = new MyPhoneStateListener(this);

    mHandler = new MyHandler(this);
}

ECode COtaStartupReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode COtaStartupReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mContext = context;
    if (DBG) {
        String action;
        intent->GetAction(&action);
        StringBuilder sb;
        sb += "onReceive: intent action=";
        sb += action;
        sb += "  mOtaspMode=";
        sb += mOtaspMode;
        Logger::V(TAG, sb.ToString());
    }

    AutoPtr<IPhoneGlobals> globals = PhoneGlobals::GetInstanceIfPrimary();
    if (globals == NULL) {
        if (DBG) Logger::D(TAG, "Not primary user, nothing to do.");
        return NOERROR;
    }

    if (mPhoneStateListenerRegistered == FALSE) {
        if (DBG) Logger::D(TAG, "Register our PhoneStateListener");
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
        AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
        telephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_OTASP_CHANGED);
        mPhoneStateListenerRegistered = TRUE;
    }
    else {
        if (DBG) Logger::D(TAG, "PhoneStateListener already registered");
    }

    AutoPtr<IPhone> phone;
    PhoneGlobals->GetPhone((IPhone**)&phone);
    if (!TelephonyCapabilities::SupportsOtasp(phone)) {
        if (DBG) Logger::D(TAG, "OTASP not supported, nothing to do.");
        return NOERROR;
    }

    if (ShouldPostpone(context)) {
        if (DBG) Logger::D(TAG, "Postponing OTASP until wizard runs");
        return NOERROR;
    }

    // Delay OTA provisioning if network is not available yet
    AutoPtr<IPhoneGlobals> app = PhoneGlobals::GetInstance();
    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    if (app.mCM.getServiceState() != IServiceState::STATE_IN_SERVICE) {
        if (DBG) Logger::W(TAG, "Network is not ready. Registering to receive notification.");
        phone->RegisterForServiceStateChanged(mHandler, SERVICE_STATE_CHANGED, NULL);
        return NOERROR;
    }

    // The following depends on the phone process being persistent. Normally we can't
    // expect a BroadcastReceiver to persist after returning from this function but it does
    // because the phone activity is persistent.
    if (DBG) Logger::D(TAG, "call OtaUtils.maybeDoOtaCall");
    return OtaUtils::MaybeDoOtaCall(mContext, mHandler, MIN_READY);
}

Boolean COtaStartupReceiver::ShouldPostpone(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.DEVICE_INITIALIZATION_WIZARD"), (IIntent**)&intent);

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IResolveInfo> resolveInfo;
    packageManager->ResolveActivity(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&resolveInfo);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &tmp);
    Boolean provisioned = tmp != 0;

    String mode;
    SystemProperties::Get(Stirng("ro.setupwizard.mode"), String("REQUIRED"), &mode);
    Boolean runningSetupWizard = String("REQUIRED").Equals(mode) || String("OPTIONAL").Equals(mode);
    if (DBG) {
        StringBuilder sb;
        sb += "resolvInfo = ";
        sb += TO_CSTR(resolveInfo);
        sb += ", provisioned = ";
        sb += provisioned;
        sb += ", runningSetupWizard = ";
        sb += runningSetupWizard;
        Logger::V(TAG, sb.ToString());
    }
    return resolveInfo != NULL && !provisioned && runningSetupWizard;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos