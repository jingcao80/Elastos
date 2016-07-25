
#include "elastos/droid/server/telecom/TtyManager.h"
#include "R.h"
#include "elastos/droid/server/telecom/CTtyManagerTtyBroadcastReceiver.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telecom.h>
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telecom::ITelecomManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// TtyManager::TtyBroadcastReceiver
//=============================================================================
CAR_INTERFACE_IMPL(TtyManager::TtyBroadcastReceiver, BroadcastReceiver, ITtyManagerTtyBroadcastReceiver)

ECode TtyManager::TtyBroadcastReceiver::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (TtyManager*) IObject::Probe(host);
    return NOERROR;
}

ECode TtyManager::TtyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Log::V("TtyManager", "onReceive, action: %s", action.string());
    if (action.Equals(ITelecomManager::ACTION_TTY_PREFERRED_MODE_CHANGED)) {
        Int32 newPreferredTtyMode;
        intent->GetInt32Extra(
                ITelecomManager::EXTRA_TTY_PREFERRED_MODE, ITelecomManager::TTY_MODE_OFF, &newPreferredTtyMode);
        if (mHost->mPreferredTtyMode != newPreferredTtyMode) {
            mHost->mPreferredTtyMode = newPreferredTtyMode;
            mHost->UpdateCurrentTtyMode();
        }
    }
    return NOERROR;
}

//=============================================================================
// TtyManager
//=============================================================================
CAR_INTERFACE_IMPL(TtyManager, Object, IWiredHeadsetManagerListener)

TtyManager::TtyManager()
    : mPreferredTtyMode(ITelecomManager::TTY_MODE_OFF)
    , mCurrentTtyMode(ITelecomManager::TTY_MODE_OFF)
{
    CTtyManagerTtyBroadcastReceiver::New(TO_IINTERFACE(this), (ITtyManagerTtyBroadcastReceiver**)&mReceiver);
}

ECode TtyManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ WiredHeadsetManager* wiredHeadsetManager)
{
    mContext = context;
    mWiredHeadsetManager = wiredHeadsetManager;
    mWiredHeadsetManager->AddListener(this);
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Settings::Secure::GetInt32(
            contentResolver,
            ISettingsSecure::PREFERRED_TTY_MODE,
            ITelecomManager::TTY_MODE_OFF, &mPreferredTtyMode);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(
            ITelecomManager::ACTION_TTY_PREFERRED_MODE_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(IBroadcastReceiver::Probe(mReceiver), intentFilter, (IIntent**)&intent);
    UpdateCurrentTtyMode();
    return NOERROR;
}

ECode TtyManager::IsTtySupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isEnabled;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::tty_enabled, &isEnabled);
    Log::V("TtyManager", "isTtySupported: %d", isEnabled);
    *result = isEnabled;
    return NOERROR;
}

ECode TtyManager::GetCurrentTtyMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCurrentTtyMode;
    return NOERROR;
}

ECode TtyManager::OnWiredHeadsetPluggedInChanged(
    /* [in] */ Boolean oldIsPluggedIn,
    /* [in] */ Boolean newIsPluggedIn)
{
    Log::V("TtyManager", "onWiredHeadsetPluggedInChanged");
    UpdateCurrentTtyMode();
    return NOERROR;
}

ECode TtyManager::UpdateCurrentTtyMode()
{
    Int32 newTtyMode = ITelecomManager::TTY_MODE_OFF;
    Boolean isPluggedIn;
    mWiredHeadsetManager->IsPluggedIn(&isPluggedIn);
    Boolean isTtySupported;
    IsTtySupported(&isTtySupported);
    if (isTtySupported && isPluggedIn) {
        newTtyMode = mPreferredTtyMode;
    }
    Log::V("TtyManager", "updateCurrentTtyMode, %d -> %d", mCurrentTtyMode, newTtyMode);
    if (mCurrentTtyMode != newTtyMode) {
        mCurrentTtyMode = newTtyMode;
        AutoPtr<IIntent> ttyModeChanged;
        CIntent::New(ITelecomManager::ACTION_CURRENT_TTY_MODE_CHANGED, (IIntent**)&ttyModeChanged);
        ttyModeChanged->PutExtra(ITelecomManager::EXTRA_CURRENT_TTY_MODE, mCurrentTtyMode);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> all;
        helper->GetALL((IUserHandle**)&all);
        mContext->SendBroadcastAsUser(ttyModeChanged, all);
        UpdateAudioTtyMode();
    }
    return NOERROR;
}

ECode TtyManager::UpdateAudioTtyMode()
{
    String audioTtyMode;
    switch (mCurrentTtyMode) {
        case ITelecomManager::TTY_MODE_FULL:
            audioTtyMode = "tty_full";
            break;
        case ITelecomManager::TTY_MODE_VCO:
            audioTtyMode = "tty_vco";
            break;
        case ITelecomManager::TTY_MODE_HCO:
            audioTtyMode = "tty_hco";
            break;
        case ITelecomManager::TTY_MODE_OFF:
        default:
            audioTtyMode = "tty_off";
            break;
    }
    Log::V("TtyManager", "updateAudioTtyMode, %s", audioTtyMode.string());
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    audioManager->SetParameters(String("tty_mode=") + audioTtyMode);
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
