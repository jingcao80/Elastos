#include "elastos/droid/systemui/volume/VolumeUI.h"
#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerImpl.h"
// #include "elastos/droid/systemui/volume/CVolumePanel.h"
#include "elastos/droid/systemui/volume/ZenModePanel.h"
#include "Elastos.Droid.Provider.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::EIID_IIRemoteVolumeController;
using Elastos::Droid::Media::EIID_IIVolumeController;
using Elastos::Droid::Media::Session::CMediaController;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardViewMediator;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardViewMediator;
using Elastos::Droid::SystemUI::StatusBar::Phone::EIID_IPhoneStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Policy::ZenModeControllerImpl;
// using Elastos::Droid::SystemUI::Volume::CVolumePanel;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//-----------------------------------------------------------------------------------------
// VolumeUI::MyObserver
//-----------------------------------------------------------------------------------------

VolumeUI::MyObserver::MyObserver(
    /* [in] */ VolumeUI* host)
    : mHost(host)
{
    ContentObserver::constructor(mHost->mHandler);
}

ECode VolumeUI::MyObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Boolean b;
    IObject::Probe(SETTING_URI)->Equals(uri, &b);
    if (b) {
        mHost->UpdateController();
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------------
// VolumeUI::MyRunnable
//-----------------------------------------------------------------------------------------

VolumeUI::MyRunnable::MyRunnable(
    /* [in] */ VolumeUI* host)
    : mHost(host)
{
}

ECode VolumeUI::MyRunnable::Run()
{
    AutoPtr<IInterface> obj = mHost->GetComponent(EIID_IPhoneStatusBar);
    AutoPtr<IPhoneStatusBar> component = IPhoneStatusBar::Probe(obj);
    component->StartActivityDismissingKeyguard(ZenModePanel::ZEN_SETTINGS,
        TRUE /* onlyProvisioned */, TRUE /* dismissShade */);

    mHost->mPanel->PostDismiss((Int64)(mHost->mDismissDelay));
    return NOERROR;
}

//-----------------------------------------------------------------------------------------
// VolumeUI::VolumeController
//-----------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_3(VolumeUI::VolumeController, Object, IIVolumeController, IBinder, IVolumeComponent)

VolumeUI::VolumeController::VolumeController()
{}

ECode VolumeUI::VolumeController::constructor(
    /* [in] */ Handle32 host)
{
    mHost = (VolumeUI*)(&host);
    return NOERROR;
}

VolumeUI::VolumeController::VolumeController(
    /* [in] */ VolumeUI* host)
    : mHost(host)
{}

ECode VolumeUI::VolumeController::DisplaySafeVolumeWarning(
    /* [in] */ Int32 flags)
{
    ECode ec = mHost->mPanel->PostDisplaySafeVolumeWarning(flags);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::VolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    ECode ec = mHost->mPanel->PostVolumeChanged(streamType, flags);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::MasterVolumeChanged(
    /* [in] */ Int32 flags)
{
    ECode ec = mHost->mPanel->PostMasterVolumeChanged(flags);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::MasterMuteChanged(
    /* [in] */ Int32 flags)
{
    ECode ec = mHost->mPanel->PostMasterMuteChanged(flags);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    ECode ec = mHost->mPanel->PostLayoutDirection(layoutDirection);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::Dismiss()
{
    ECode ec = mHost->mPanel->PostDismiss(0);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::VolumeController::GetZenController(
    /* [out] */ IZenModeController** zmc)
{
    return mHost->mPanel->GetZenController(zmc);
}

ECode VolumeUI::VolumeController::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

//-----------------------------------------------------------------------------------------
// VolumeUI::RemoteVolumeController
//-----------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(VolumeUI::RemoteVolumeController, Object, IIRemoteVolumeController, IBinder)

VolumeUI::RemoteVolumeController::RemoteVolumeController()
{}

ECode VolumeUI::RemoteVolumeController::constructor(
    /* [in] */ Handle32 host)
{
    mHost = (VolumeUI*)(&host);
    return NOERROR;
}

VolumeUI::RemoteVolumeController::RemoteVolumeController(
    /* [in] */ VolumeUI* host)
    : mHost(host)
{}

ECode VolumeUI::RemoteVolumeController::RemoteVolumeChanged(
    /* [in] */ IISessionController* binder,
    /* [in] */ Int32 flags)
{
    AutoPtr<IMediaController> controller;
    CMediaController::New(mHost->mContext, binder, (IMediaController**)&controller);
    ECode ec = mHost->mPanel->PostRemoteVolumeChanged(controller, flags);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::RemoteVolumeController::UpdateRemoteController(
    /* [in] */ IISessionController* session)
{
    ECode ec = mHost->mPanel->PostRemoteSliderVisibility(session != NULL);
    // TODO stash default session in case the slider can be opened other
    // than by remoteVolumeChanged.
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VolumeUI::RemoteVolumeController::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

//-----------------------------------------------------------------------------------------
// VolumeUI::MyVolumePanelCallback
//-----------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumeUI::MyVolumePanelCallback, Object, IVolumePanelCallback)

VolumeUI::MyVolumePanelCallback::MyVolumePanelCallback(
    /* [in] */ VolumeUI* host)
    : mHost(host)
{}

ECode VolumeUI::MyVolumePanelCallback::OnZenSettings()
{
    mHost->mHandler->RemoveCallbacks(mHost->mStartZenSettings);
    Boolean b;
    mHost->mHandler->Post(mHost->mStartZenSettings, &b);
    return NOERROR;
}

ECode VolumeUI::MyVolumePanelCallback::OnInteraction()
{
    AutoPtr<IInterface> obj = mHost->GetComponent(EIID_IKeyguardViewMediator);
    AutoPtr<IKeyguardViewMediator> kvm = IKeyguardViewMediator::Probe(obj);
    if (kvm != NULL) {
        kvm->UserActivity();
    }
    return NOERROR;
}

ECode VolumeUI::MyVolumePanelCallback::OnVisible(
    /* [in] */ Boolean visible)
{
    if (mHost->mAudioManager != NULL && mHost->mVolumeController != NULL) {
        mHost->mAudioManager->NotifyVolumeControllerVisible(mHost->mVolumeController, visible);
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------------
// VolumeUI
//-----------------------------------------------------------------------------------------

const String VolumeUI::TAG("VolumeUI");
const String VolumeUI::SETTING("systemui_volume_controller");

static AutoPtr<IUri> init()
{
    AutoPtr<ISettingsGlobal> ss;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&ss);
    AutoPtr<IUri> uri;
    ss->GetUriFor(String("systemui_volume_controller"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> VolumeUI::SETTING_URI = init();
const Int32 VolumeUI::DEFAULT;

VolumeUI::VolumeUI()
    : mDismissDelay(0)
{
    CHandler::New((IHandler**)&mHandler);
    mObserver = new MyObserver(this);
    mStartZenSettings = new MyRunnable(this);
}

ECode VolumeUI::Start()
{
    AutoPtr<IInterface> amObj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&amObj);
    mAudioManager = IAudioManager::Probe(amObj);

    AutoPtr<IInterface> msmObj;
    mContext->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&msmObj);
    mMediaSessionManager = IMediaSessionManager::Probe(msmObj);

    InitPanel();
    mVolumeController = new VolumeController(this);
    mRemoteVolumeController = new RemoteVolumeController(this);
    PutComponent(EIID_IVolumeComponent, (IIVolumeController*)mVolumeController);
    UpdateController();
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(SETTING_URI, FALSE, mObserver);
    return NOERROR;
}

ECode VolumeUI::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    SystemUI::OnConfigurationChanged(newConfig);
    if (mPanel != NULL) {
        mPanel->OnConfigurationChanged(newConfig);
    }
    return NOERROR;
}

void VolumeUI::UpdateController()
{
    AutoPtr<ISettingsGlobal> ss;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&ss);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 i;
    ss->GetInt32(resolver, SETTING, DEFAULT, &i);
    if (i != 0) {
        Logger::D(TAG, "Registering volume controller");
        mAudioManager->SetVolumeController(mVolumeController);
        mMediaSessionManager->SetRemoteVolumeController(mRemoteVolumeController);
    }
    else {
        Logger::D(TAG, "Unregistering volume controller");
        mAudioManager->SetVolumeController(NULL);
        mMediaSessionManager->SetRemoteVolumeController(NULL);
    }
}

void VolumeUI::InitPanel()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::volume_panel_dismiss_delay, &mDismissDelay);

    AutoPtr<ZenModeControllerImpl> zmc = new ZenModeControllerImpl(mContext, mHandler);
    //TODO
    // CVolumePanel::New(mContext, zmc, (IVolumePanel**)&mPanel);

    // AutoPtr<MyVolumePanelCallback> vpc = new MyVolumePanelCallback(this);
    // mPanel->SetCallback(vpc);
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos