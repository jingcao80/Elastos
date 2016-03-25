
#include "elastos/droid/widget/CVolumePanel.h"
#include "elastos/droid/widget/CVolumePanelDialog.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/media/CAudioSystemHelper.h"
#include "elastos/droid/media/CToneGenerator.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::CAudioSystemHelper;
using Elastos::Droid::Media::IAudioSystemHelper;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IGravity;
using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const String CVolumePanel::TAG("VolumePanel");
const Boolean CVolumePanel::LOGD = FALSE;

const Int32 CVolumePanel::VIBRATE_DURATION = 300;
const Int32 CVolumePanel::BEEP_DURATION = 150;
const Int32 CVolumePanel::MAX_VOLUME = 100;
const Int32 CVolumePanel::FREE_DELAY = 10000;
const Int32 CVolumePanel::TIMEOUT_DELAY = 3000;

const Int32 CVolumePanel::MSG_VOLUME_CHANGED = 0;
const Int32 CVolumePanel::MSG_FREE_RESOURCES = 1;
const Int32 CVolumePanel::MSG_PLAY_SOUND = 2;
const Int32 CVolumePanel::MSG_STOP_SOUNDS = 3;
const Int32 CVolumePanel::MSG_VIBRATE = 4;
const Int32 CVolumePanel::MSG_TIMEOUT = 5;
const Int32 CVolumePanel::MSG_RINGER_MODE_CHANGED = 6;
const Int32 CVolumePanel::MSG_MUTE_CHANGED = 7;
const Int32 CVolumePanel::MSG_REMOTE_VOLUME_CHANGED = 8;
const Int32 CVolumePanel::MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN = 9;
const Int32 CVolumePanel::MSG_SLIDER_VISIBILITY_CHANGED = 10;
const Int32 CVolumePanel::MSG_DISPLAY_SAFE_VOLUME_WARNING = 11;

const Int32 CVolumePanel::STREAM_MASTER = -100;

CVolumePanel::StreamResources CVolumePanel::BluetoothSCOStream(
    IAudioManager::STREAM_BLUETOOTH_SCO,
    R::string::volume_icon_description_bluetooth,
    R::drawable::ic_audio_bt,
    R::drawable::ic_audio_bt,
    FALSE);
CVolumePanel::StreamResources CVolumePanel::RingerStream(
    IAudioManager::STREAM_RING,
    R::string::volume_icon_description_ringer,
    R::drawable::ic_audio_ring_notif,
    R::drawable::ic_audio_ring_notif_mute,
    FALSE);
CVolumePanel::StreamResources CVolumePanel::VoiceStream(
    IAudioManager::STREAM_VOICE_CALL,
    R::string::volume_icon_description_incall,
    R::drawable::ic_audio_phone,
    R::drawable::ic_audio_phone,
    FALSE);
CVolumePanel::StreamResources CVolumePanel::AlarmStream(
    IAudioManager::STREAM_ALARM,
    R::string::volume_alarm,
    R::drawable::ic_audio_alarm,
    R::drawable::ic_audio_alarm_mute,
    FALSE);
CVolumePanel::StreamResources CVolumePanel::MediaStream(
    IAudioManager::STREAM_MUSIC,
    R::string::volume_icon_description_media,
    R::drawable::ic_audio_vol,
    R::drawable::ic_audio_vol_mute,
    TRUE);
CVolumePanel::StreamResources CVolumePanel::NotificationStream(
    IAudioManager::STREAM_NOTIFICATION,
    R::string::volume_icon_description_notification,
    R::drawable::ic_audio_notification,
    R::drawable::ic_audio_notification_mute,
    TRUE);
    // for now, use media resources for master volume
CVolumePanel::StreamResources CVolumePanel::MasterStream(
    CVolumePanel::STREAM_MASTER,
    R::string::volume_icon_description_media, //FIXME should have its own description
    R::drawable::ic_audio_vol,
    R::drawable::ic_audio_vol_mute,
    FALSE);
CVolumePanel::StreamResources CVolumePanel::RemoteStream(
    IAudioService::STREAM_REMOTE_MUSIC,
    R::string::volume_icon_description_media, //FIXME should have its own description
    R::drawable::ic_media_route_on_holo_dark,
    R::drawable::ic_media_route_disabled_holo_dark,
    FALSE);

// List of stream types and their order
const Int32 CVolumePanel::STREAMS_LENGTH;
CVolumePanel::StreamResources* CVolumePanel::STREAMS[8] = {
    &CVolumePanel::BluetoothSCOStream, &CVolumePanel::RingerStream,
    &CVolumePanel::VoiceStream, &CVolumePanel::AlarmStream,
    &CVolumePanel::MediaStream, &CVolumePanel::NotificationStream,
    &CVolumePanel::MasterStream, &CVolumePanel::RemoteStream,
};

AutoPtr<IAlertDialog> CVolumePanel::sConfirmSafeVolumeDialog;
Mutex CVolumePanel::sConfirmSafeVolumeLock;// = new Object();


//=================================================================
//              CVolumePanel::StreamControl
//=================================================================
CAR_INTERFACE_IMPL(CVolumePanel::StreamControl, IInterface)

//=================================================================
//              CVolumePanel::WarningDialogReceiver
//=================================================================

UInt32 CVolumePanel::WarningDialogReceiver::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CVolumePanel::WarningDialogReceiver::Release()
{
    return ElRefBase::Release();
}

PInterface CVolumePanel::WarningDialogReceiver::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    else if (riid == EIID_IObject) {
        return (IInterface*)this;
    }
    else if (riid == EIID_IBroadcastReceiver) {
        return (IInterface*)this;
    }
    else if (riid == EIID_IDialogInterfaceOnDismissListener) {
        return (IInterface*)this;
    }
    return NULL;
}

ECode CVolumePanel::WarningDialogReceiver::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IObject;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_IBroadcastReceiver;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_IDialogInterfaceOnDismissListener;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

CVolumePanel::WarningDialogReceiver::WarningDialogReceiver(
    /* [in] */ IContext* context,
    /* [in] */ IDialog* dialog)
    : mContext(context)
    , mDialog(dialog)
{
    assert(context != NULL);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
}

ECode CVolumePanel::WarningDialogReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mDialog->Cancel();

    AutoLock lock(CVolumePanel::sConfirmSafeVolumeLock);
    CVolumePanel::sConfirmSafeVolumeDialog = NULL;
    return NOERROR;
}

ECode CVolumePanel::WarningDialogReceiver::OnDismiss(
    /* [Int32] */ IDialogInterface* unused)
{
    mContext->UnregisterReceiver(this);
    AutoLock lock(CVolumePanel::sConfirmSafeVolumeLock);
    CVolumePanel::sConfirmSafeVolumeDialog = NULL;
    return NOERROR;
}

//=================================================================
//                  CVolumePanel::ViewOnTouchListener
//=================================================================
CAR_INTERFACE_IMPL(CVolumePanel::ViewOnTouchListener, IViewOnTouchListener)

CVolumePanel::ViewOnTouchListener::ViewOnTouchListener(
    /* [in] */ CVolumePanel* host)
    : mHost(host)
{
}

ECode CVolumePanel::ViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->ResetTimeout();
    *result = FALSE;
    return NOERROR;
}

//=================================================================
//                  CVolumePanel::DialogOnDismissListener
//=================================================================
CAR_INTERFACE_IMPL(CVolumePanel::DialogOnDismissListener, IDialogInterfaceOnDismissListener)

CVolumePanel::DialogOnDismissListener::DialogOnDismissListener(
    /* [in] */ CVolumePanel* host)
    : mHost(host)
{
}

ECode CVolumePanel::DialogOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->mActiveStreamType = -1;
    mHost->mAudioManager->ForceVolumeControlStream(mHost->mActiveStreamType);
    return NOERROR;
}

//=================================================================
//                  CVolumePanel::PositiveButtonOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(CVolumePanel::PositiveButtonOnClickListener, IDialogInterfaceOnClickListener)

CVolumePanel::PositiveButtonOnClickListener::PositiveButtonOnClickListener(
    /* [in] */ CVolumePanel* host)
    : mHost(host)
{}

ECode CVolumePanel::PositiveButtonOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->mAudioService->DisableSafeMediaVolume();
}

//=================================================================
//                  CVolumePanel::RingerModeChangedActionReceiver
//=================================================================
CVolumePanel::RingerModeChangedActionReceiver::RingerModeChangedActionReceiver(
    /* [in] */ IHandler* host)
    : mHost(host)
{
}

ECode CVolumePanel::RingerModeChangedActionReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IAudioManager::RINGER_MODE_CHANGED_ACTION.Equals(action)) {
        mHost->RemoveMessages(CVolumePanel::MSG_RINGER_MODE_CHANGED);

        AutoPtr<IMessage> message;
        mHost->ObtainMessage(CVolumePanel::MSG_RINGER_MODE_CHANGED, (IMessage**)&message);
        Boolean bval;
        mHost->SendMessage(message, &bval);
    }
    return NOERROR;
}

//=================================================================
//                  CVolumePanel
//=================================================================
IHANDLER_METHODS_IMPL(CVolumePanel, Handler)

PInterface CVolumePanel::Probe(
    /* [in]  */ REIID riid)
{
    return _CVolumePanel::Probe(riid);
}

CVolumePanel::CVolumePanel()
    : mRingIsSilent(FALSE)
    , mShowCombinedVolumes(FALSE)
    , mVoiceCapable(FALSE)
    , mPlayMasterStreamTones(FALSE)
    , mActiveStreamType(-1)
{
    Handler::Init();
}

ECode CVolumePanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAudioService* volumeService)
{
    mContext = context;
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&tmpObj);
    mAudioManager = IAudioManager::Probe(tmpObj.Get());
    if (mAudioManager == NULL) {
        return E_RUNTIME_EXCEPTION;
    }

    mAudioService = volumeService;

    // For now, only show master volume if master volume is supported
    Boolean useMasterVolume;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_useMasterVolume, &useMasterVolume);
    if (useMasterVolume) {
        for (Int32 i = 0; i < STREAMS_LENGTH; i++) {
            STREAMS[i]->mShow = (STREAMS[i]->mStreamType == STREAM_MASTER);
        }
    }

    tmpObj = NULL;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(tmpObj.Get());

    mView = NULL;
    ECode ec = inflater->Inflate(R::layout::volume_adjust, NULL, (IView**)&mView);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to inflate volume_adjust.xml.");
        return ec;
    }
    assert(mView != NULL);

    AutoPtr<IViewOnTouchListener> listener = new ViewOnTouchListener(this);
    mView->SetOnTouchListener(listener);

    AutoPtr<IView> tmpView;
    mView->FindViewById(R::id::visible_panel, (IView**)&tmpView);
    mPanel = IViewGroup::Probe(tmpView.Get());

    tmpView = NULL;
    mView->FindViewById(R::id::slider_group, (IView**)&tmpView);
    mSliderGroup = IViewGroup::Probe(tmpView.Get());
    {
        // TODO luo.zhaohui delete
        AutoPtr<IViewGroupLayoutParams> lp;
        mSliderGroup->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        lp->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    }

    tmpView = NULL;
    mView->FindViewById(R::id::expand_button, (IView**)&tmpView);
    mMoreButton = IImageView::Probe(tmpView.Get());

    tmpView = NULL;
    mView->FindViewById(R::id::expand_button_divider, (IView**)&tmpView);
    mDivider = IImageView::Probe(tmpView.Get());

    // Setup Dialog
    //
    FAIL_RETURN(CVolumePanelDialog::New(context, R::style::Theme_Panel_Volume, this,
        (IDialog**)&mDialog));
    assert(mDialog != NULL);

    AutoPtr<ICharSequence> titleSeq;
    CStringWrapper::New(String("Volume control"), (ICharSequence**)&titleSeq);
    AutoPtr<IDialogInterfaceOnDismissListener> dl = new DialogOnDismissListener(this);

    mDialog->SetTitle(titleSeq); // No need to localize
    mDialog->SetContentView(mView);
    mDialog->SetOnDismissListener(dl);

    // Change some window properties
    AutoPtr<IWindow> window;
    mDialog->GetWindow((IWindow**)&window);
    window->SetGravity(IGravity::TOP);
    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);
    lp->SetToken(NULL);

    // Offset from the top
    Int32 tmpInt;
    res->GetDimensionPixelOffset(R::dimen::volume_panel_top, &tmpInt);
    lp->SetY(tmpInt);
    lp->SetType(IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY);
    lp->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    lp->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
    lp->GetPrivateFlags(&tmpInt);
    lp->SetPrivateFlags(tmpInt | IWindowManagerLayoutParams::PRIVATE_FLAG_FORCE_SHOW_NAV_BAR);
    window->SetAttributes(lp);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
        | IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH);

    AutoPtr<IAudioSystemHelper> asHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&asHelper);
    Int32 numStreamTypes;
    asHelper->GetNumStreamTypes(&numStreamTypes);
    mToneGenerators = ArrayOf<IToneGenerator*>::Alloc(numStreamTypes);

    tmpObj = NULL;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&tmpObj);
    mVibrator = IVibrator::Probe(tmpObj.Get());

    res->GetBoolean(R::bool_::config_voice_capable, &mVoiceCapable);
    mShowCombinedVolumes = !mVoiceCapable && !useMasterVolume;
    // If we don't want to show multiple volumes, hide the settings button and divider
    if (!mShowCombinedVolumes) {
        mMoreButton->SetVisibility(IView::GONE);
        mDivider->SetVisibility(IView::GONE);
    }
    else {
        mMoreButton->SetOnClickListener(this);
    }

    Boolean masterVolumeOnly, masterVolumeKeySounds;
    res->GetBoolean(R::bool_::config_useMasterVolume, &masterVolumeOnly);
    res->GetBoolean(R::bool_::config_useVolumeKeySounds, &masterVolumeKeySounds);

    mPlayMasterStreamTones = masterVolumeOnly && masterVolumeKeySounds;

    ListenToRingerMode();
    return NOERROR;
}

void CVolumePanel::ListenToRingerMode()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IAudioManager::RINGER_MODE_CHANGED_ACTION);

    AutoPtr<IBroadcastReceiver> receive = new RingerModeChangedActionReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(receive, filter, (IIntent**)&intent);
}

Boolean CVolumePanel::IsMuted(
    /* [in] */ Int32 streamType)
{
    if (mAudioService == NULL) return FALSE;

    Boolean bval;
    if (streamType == STREAM_MASTER) {
        mAudioManager->IsMasterMute(&bval);
    }
    else if (streamType == IAudioService::STREAM_REMOTE_MUSIC) {
        Int32 ival;
        mAudioService->GetRemoteStreamVolume(&ival);
        bval = (ival <= 0);
    }
    else {
         mAudioManager->IsStreamMute(streamType, &bval);
    }
    return bval;
}

Int32 CVolumePanel::GetStreamMaxVolume(
    /* [in] */ Int32 streamType)
{
    Int32 ival;
    if (streamType == STREAM_MASTER) {
        mAudioManager->GetMasterMaxVolume(&ival);
    }
    else if (streamType == IAudioService::STREAM_REMOTE_MUSIC) {
        mAudioService->GetRemoteStreamMaxVolume(&ival);
    }
    else {
        mAudioManager->GetStreamMaxVolume(streamType, &ival);
    }
    return ival;
}

Int32 CVolumePanel::GetStreamVolume(
    /* [in] */ Int32 streamType)
{
    Int32 ival;
    if (streamType == STREAM_MASTER) {
        mAudioManager->GetMasterVolume(&ival);
    }
    else if (streamType == IAudioService::STREAM_REMOTE_MUSIC) {
        mAudioService->GetRemoteStreamVolume(&ival);
    }
    else {
        mAudioManager->GetStreamVolume(streamType, &ival);
    }
    return ival;
}

void CVolumePanel::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    if (streamType == STREAM_MASTER) {
        mAudioManager->SetMasterVolume(index, flags);
    }
    else if (streamType == IAudioService::STREAM_REMOTE_MUSIC) {
        mAudioService->SetRemoteStreamVolume(index);
    }
    else {
        mAudioManager->SetStreamVolume(streamType, index, flags);
    }
}

void CVolumePanel::CreateSliders()
{
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(tmpObj.Get());
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    Int32 streamType, plusOne;
    String descRes;
    StreamResources* streamRes;
    for (Int32 i = 0; i < STREAMS_LENGTH; i++) {
        streamRes = STREAMS[i];
        streamType = streamRes->mStreamType;
        if (mVoiceCapable && streamRes == &NotificationStream) {
            streamRes = &RingerStream;
        }
        AutoPtr<StreamControl> sc = new StreamControl();
        sc->mStreamType = streamType;
        AutoPtr<IView> tmpView;
        inflater->Inflate(R::layout::volume_adjust_item, NULL, (IView**)&tmpView);
        sc->mGroup = IViewGroup::Probe(tmpView.Get());
        sc->mGroup->SetTag(sc);

        tmpView = NULL;
        sc->mGroup->FindViewById(R::id::stream_icon, (IView**)&tmpView);
        sc->mIcon = IImageView::Probe(tmpView.Get());
        sc->mIcon->SetTag(sc);
        res->GetString(streamRes->mDescRes, &descRes);
        AutoPtr<ICharSequence> seq;
        CStringWrapper::New(descRes, (ICharSequence**)&seq);
        sc->mIcon->SetContentDescription(seq);
        sc->mIconRes = streamRes->mIconRes;
        sc->mIconMuteRes = streamRes->mIconMuteRes;
        sc->mIcon->SetImageResource(sc->mIconRes);

        tmpView = NULL;
        sc->mGroup->FindViewById(R::id::seekbar, (IView**)&tmpView);
        sc->mSeekbarView = ISeekBar::Probe(tmpView.Get());
        plusOne = (streamType == IAudioSystem::STREAM_BLUETOOTH_SCO
            || streamType == IAudioSystem::STREAM_VOICE_CALL) ? 1 : 0;
        sc->mSeekbarView->SetMax(GetStreamMaxVolume(streamType) + plusOne);
        sc->mSeekbarView->SetOnSeekBarChangeListener(this);
        sc->mSeekbarView->SetTag(sc);
        mStreamControls[streamType] = sc;
    }
}

void CVolumePanel::ReorderSliders(
    /* [in] */ Int32 activeStreamType)
{
    mSliderGroup->RemoveAllViews();

    HashMap<Int32, AutoPtr<StreamControl> >::Iterator it;
    it = mStreamControls.Find(activeStreamType);
    if (it == mStreamControls.End()) {
        Logger::E(TAG, "Missing stream type! - %d", activeStreamType);
        mActiveStreamType = -1;
    }
    else {
        AutoPtr<StreamControl> active = it->mSecond;
        mSliderGroup->AddView(active->mGroup);
        mActiveStreamType = activeStreamType;
        active->mGroup->SetVisibility(IView::VISIBLE);
        UpdateSlider(active);
    }

    AddOtherVolumes();
}

void CVolumePanel::AddOtherVolumes()
{
    if (!mShowCombinedVolumes) return;

    Int32 streamType;
    for (Int32 i = 0; i < STREAMS_LENGTH; i++) {
        // Skip the phone specific ones and the active one
        streamType = STREAMS[i]->mStreamType;
        if (!STREAMS[i]->mShow || streamType == mActiveStreamType) {
            continue;
        }

        AutoPtr<StreamControl> sc = mStreamControls[streamType];
        mSliderGroup->AddView(sc->mGroup);
        UpdateSlider(sc);
    }
}

/** Update the mute and progress state of a slider */
void CVolumePanel::UpdateSlider(
    /* [in] */ StreamControl* sc)
{
    Int32 volume = GetStreamVolume(sc->mStreamType);
    sc->mSeekbarView->SetProgress(volume);
    Boolean muted = IsMuted(sc->mStreamType);
    sc->mIcon->SetImageResource(muted ? sc->mIconMuteRes : sc->mIconRes);
    Int32 ringerMode;
    mAudioManager->GetRingerMode(&ringerMode);
    if (sc->mStreamType == IAudioManager::STREAM_RING &&
        ringerMode == IAudioManager::RINGER_MODE_VIBRATE) {
        sc->mIcon->SetImageResource(R::drawable::ic_audio_ring_notif_vibrate);
    }

    Int32 streamType;
    if (sc->mStreamType == IAudioService::STREAM_REMOTE_MUSIC) {
        // never disable touch interactions for remote playback, the muting is not tied to
        // the state of the phone.
        sc->mSeekbarView->SetEnabled(TRUE);
    }
    else if (muted && sc->mStreamType != (mAudioManager->GetMasterStreamType(&streamType), streamType)) {
        sc->mSeekbarView->SetEnabled(FALSE);
    }
    else {
        sc->mSeekbarView->SetEnabled(TRUE);
    }

    if (LOGD) Logger::D("CVolumePanel::UpdateSlider", "Update seekbar progress %d, muted %d", volume, muted);
}

Boolean CVolumePanel::IsExpanded()
{
    Int32 visible;
    mMoreButton->GetVisibility(&visible);
    return visible != IView::VISIBLE;
}

void CVolumePanel::Expand()
{
    Int32 count;
    mSliderGroup->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mSliderGroup->GetChildAt(i, (IView**)&child);
        child->SetVisibility(IView::VISIBLE);
    }
    mMoreButton->SetVisibility(IView::INVISIBLE);
    mDivider->SetVisibility(IView::INVISIBLE);
}

void CVolumePanel::Collapse()
{
    mMoreButton->SetVisibility(IView::VISIBLE);
    mDivider->SetVisibility(IView::VISIBLE);
    Int32 count;
    mSliderGroup->GetChildCount(&count);
    for (Int32 i = 1; i < count; i++) {
        AutoPtr<IView> child;
        mSliderGroup->GetChildAt(i, (IView**)&child);
        child->SetVisibility(IView::GONE);
    }
}

void CVolumePanel::UpdateStates()
{
    Int32 count;
    mSliderGroup->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mSliderGroup->GetChildAt(i, (IView**)&child);

        AutoPtr<IInterface> tmpObj;
        child->GetTag((IInterface**)&tmpObj);
        StreamControl* sc = (StreamControl*)tmpObj.Get();
        UpdateSlider(sc);
    }
}

ECode CVolumePanel::PostVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if (LOGD) Logger::D(TAG, "PostVolumeChanged streamType=%d flags=%08x", streamType, flags);
    Boolean hasMessage;
    HasMessages(MSG_VOLUME_CHANGED, &hasMessage);
    if (hasMessage) return NOERROR;

    {
        AutoLock lock(this);

        if (mStreamControls.IsEmpty()) {
            CreateSliders();
        }
    }

    RemoveMessages(MSG_FREE_RESOURCES);

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_VOLUME_CHANGED, streamType, flags, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

ECode CVolumePanel::PostRemoteVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean hasMessage;
    HasMessages(MSG_REMOTE_VOLUME_CHANGED, &hasMessage);
    if (hasMessage) return NOERROR;

    {
        AutoLock lock(this);

        if (mStreamControls.IsEmpty()) {
            CreateSliders();
        }
    }

    RemoveMessages(MSG_FREE_RESOURCES);

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_REMOTE_VOLUME_CHANGED, streamType, flags, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

ECode CVolumePanel::PostRemoteSliderVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IMessage> message;
    ObtainMessage(MSG_SLIDER_VISIBILITY_CHANGED, IAudioService::STREAM_REMOTE_MUSIC,
        visible ? 1 : 0, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

ECode CVolumePanel::PostHasNewRemotePlaybackInfo()
{
    Boolean hasMessage;
    HasMessages(MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN, &hasMessage);
    if (hasMessage) return NOERROR;

    // don't create or prevent resources to be freed, if they disappear, this update came too
    //   late and shouldn't warrant the panel to be displayed longer
    AutoPtr<IMessage> message;
    ObtainMessage(MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

ECode CVolumePanel::PostMasterVolumeChanged(
    /* [in] */ Int32 flags)
{
    return PostVolumeChanged(STREAM_MASTER, flags);
}

ECode CVolumePanel::PostMuteChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean hasMessage;
    HasMessages(MSG_VOLUME_CHANGED, &hasMessage);
    if (hasMessage) return NOERROR;

    {
        AutoLock lock(this);

        if (mStreamControls.IsEmpty()) {
            CreateSliders();
        }
    }

    RemoveMessages(MSG_FREE_RESOURCES);

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_MUTE_CHANGED, streamType, flags, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

ECode CVolumePanel::PostMasterMuteChanged(
    /* [in] */ Int32 flags)
{
    return PostMuteChanged(STREAM_MASTER, flags);;
}

ECode CVolumePanel::PostDisplaySafeVolumeWarning()
{
    Boolean hasMessage;
    HasMessages(MSG_DISPLAY_SAFE_VOLUME_WARNING, &hasMessage);
    if (hasMessage) return NOERROR;

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_DISPLAY_SAFE_VOLUME_WARNING, 0, 0, (IMessage**)&message);
    Boolean bval;
    return SendMessage(message, &bval);
}

void CVolumePanel::OnVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if ((flags & IAudioManager::FLAG_SHOW_UI) != 0) {
        {
            AutoLock lock(this);
            if (mActiveStreamType != streamType) {
                ReorderSliders(streamType);
            }
            OnShowVolumeChanged(streamType, flags);
        }
    }

    Boolean bval;
    if ((flags & IAudioManager::FLAG_PLAY_SOUND) != 0 && ! mRingIsSilent) {
        RemoveMessages(MSG_PLAY_SOUND);

        AutoPtr<IMessage> message;
        ObtainMessage(MSG_PLAY_SOUND, streamType, flags, (IMessage**)&message);
        SendMessageDelayed(message, PLAY_SOUND_DELAY, &bval);
    }

    if ((flags & IAudioManager::FLAG_REMOVE_SOUND_AND_VIBRATE) != 0) {
        RemoveMessages(MSG_PLAY_SOUND);
        RemoveMessages(MSG_VIBRATE);
        OnStopSounds();
    }

    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> message;
    ObtainMessage(MSG_FREE_RESOURCES, (IMessage**)&message);
    SendMessageDelayed(message, FREE_DELAY, &bval);

    ResetTimeout();
}

void CVolumePanel::OnMuteChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if (LOGD) Logger::D(TAG, "onMuteChanged(streamType: %d, flags: %d)", streamType, flags);

    HashMap<Int32, AutoPtr<StreamControl> >::Iterator it = mStreamControls.Find(streamType);
    if (it != mStreamControls.End()) {
        AutoPtr<StreamControl> sc = it->mSecond;
        if (sc != NULL) {
            sc->mIcon->SetImageResource(
                IsMuted(sc->mStreamType) ? sc->mIconMuteRes : sc->mIconRes);
        }
    }

    OnVolumeChanged(streamType, flags);
}

void CVolumePanel::OnShowVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Int32 index = GetStreamVolume(streamType);

    mRingIsSilent = FALSE;

    if (LOGD) {
        static HashMap<Int32, String> infoMap;
        if (infoMap.IsEmpty()) {
            infoMap[IAudioManager::STREAM_VOICE_CALL] = String("STREAM_VOICE_CALL");
            infoMap[IAudioManager::STREAM_SYSTEM] = String("STREAM_SYSTEM");
            infoMap[IAudioManager::STREAM_RING] = String("STREAM_RING");
            infoMap[IAudioManager::STREAM_MUSIC] = String("STREAM_MUSIC");
            infoMap[IAudioManager::STREAM_ALARM] = String("STREAM_ALARM");
            infoMap[IAudioManager::STREAM_NOTIFICATION] = String("STREAM_NOTIFICATION");
            infoMap[IAudioManager::STREAM_BLUETOOTH_SCO] = String("STREAM_BLUETOOTH_SCO");
            infoMap[IAudioManager::STREAM_SYSTEM_ENFORCED] = String("STREAM_SYSTEM_ENFORCED");
            infoMap[IAudioManager::STREAM_DTMF] = String("STREAM_DTMF");
            infoMap[IAudioManager::STREAM_TTS] = String("STREAM_TTS");
        }

        Logger::D(TAG, "OnShowVolumeChanged(streamType: %s, flags: %d), index: %d",
            infoMap[streamType].string(), flags, index);
    }

    // get max volume for progress bar

    Int32 max = GetStreamMaxVolume(streamType);

    switch (streamType) {
        case IAudioManager::STREAM_RING: {
//                setRingerIcon();
            AutoPtr<IRingtoneManagerHelper> helper;
            CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
            AutoPtr<IUri> ringuri;
            helper->GetActualDefaultRingtoneUri(mContext,
                IRingtoneManager::TYPE_RINGTONE, (IUri**)&ringuri);
            if (ringuri == NULL) {
                mRingIsSilent = TRUE;
            }
            break;
        }

        case IAudioManager::STREAM_MUSIC: {
            // Special case for when Bluetooth is active for music
            Int32 devices;
            mAudioManager->GetDevicesForStream(IAudioManager::STREAM_MUSIC, &devices);
            if ((devices &
                    (IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP |
                    IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
                    IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)) != 0) {
                SetMusicIcon(R::drawable::ic_audio_bt, R::drawable::ic_audio_bt_mute);
            }
            else {
                SetMusicIcon(R::drawable::ic_audio_vol, R::drawable::ic_audio_vol_mute);
            }
            break;
        }

        case IAudioManager::STREAM_VOICE_CALL: {
            /*
             * For in-call voice call volume, there is no inaudible volume.
             * Rescale the UI control so the progress bar doesn't go all
             * the way to zero and don't show the mute icon.
             */
            index++;
            max++;
            break;
        }

        case IAudioManager::STREAM_ALARM: {
            break;
        }

        case IAudioManager::STREAM_NOTIFICATION: {
            AutoPtr<IRingtoneManagerHelper> helper;
            CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
            AutoPtr<IUri> ringuri;
            helper->GetActualDefaultRingtoneUri(mContext,
                IRingtoneManager::TYPE_NOTIFICATION, (IUri**)&ringuri);

            if (ringuri == NULL) {
                mRingIsSilent = TRUE;
            }
            break;
        }

        case IAudioManager::STREAM_BLUETOOTH_SCO: {
            /*
             * For in-call voice call volume, there is no inaudible volume.
             * Rescale the UI control so the progress bar doesn't go all
             * the way to zero and don't show the mute icon.
             */
            index++;
            max++;
            break;
        }

        case IAudioService::STREAM_REMOTE_MUSIC: {
            if (LOGD) Logger::D(TAG, "showing remote volume %d over %d", index, max);
            break;
        }
    }

    HashMap<Int32, AutoPtr<StreamControl> >::Iterator it = mStreamControls.Find(streamType);
    if (it != mStreamControls.End()) {
        AutoPtr<StreamControl> sc = it->mSecond;
        if (sc != NULL) {
            Int32 svMax;
            sc->mSeekbarView->GetMax(&svMax);
            if (svMax != max) {
                sc->mSeekbarView->SetMax(max);
            }
            if (LOGD) Logger::D(TAG, "showing volume streamType %d, %d/%d", streamType, index, max);

            Int32 masterStreamType;
            mAudioManager->GetMasterStreamType(&masterStreamType);
            sc->mSeekbarView->SetProgress(index);
            if (((flags & IAudioManager::FLAG_FIXED_VOLUME) != 0)
                || (streamType != masterStreamType
                    && streamType != IAudioService::STREAM_REMOTE_MUSIC
                    && IsMuted(streamType))) {
                sc->mSeekbarView->SetEnabled(FALSE);
            }
            else {
                sc->mSeekbarView->SetEnabled(TRUE);
            }
        }
    }

    Boolean bval;
    mDialog->IsShowing(&bval);
    if (!bval) {
        Int32 stream = (streamType == IAudioService::STREAM_REMOTE_MUSIC) ? -1 : streamType;
        // when the stream is for remote playback, use -1 to reset the stream type evaluation
        mAudioManager->ForceVolumeControlStream(stream);
        mDialog->SetContentView(mView);
        // Showing dialog - use collapsed state
        if (mShowCombinedVolumes) {
            Collapse();
        }
        mDialog->Show();
    }

    // Do a little vibrate if applicable (only when going into vibrate mode)
    Int32 ringerMode = 0;
    mAudioService->IsStreamAffectedByRingerMode(streamType, &bval);
    if (bval) {
        mAudioManager->GetRingerMode(&ringerMode);
    }
    if ((streamType != IAudioService::STREAM_REMOTE_MUSIC)
        && ((flags & IAudioManager::FLAG_VIBRATE) != 0)
        && bval
        && ringerMode == IAudioManager::RINGER_MODE_VIBRATE)
    {
        AutoPtr<IMessage> message;
        ObtainMessage(MSG_VIBRATE, (IMessage**)&message);
        SendMessageDelayed(message, VIBRATE_DELAY, &bval);
    }
}

void CVolumePanel::OnPlaySound(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean bval;
    HasMessages(MSG_STOP_SOUNDS, &bval);
    if (bval) {
        RemoveMessages(MSG_STOP_SOUNDS);
        // Force stop right now
        OnStopSounds();
    }

    {
        AutoLock lock(this);
        AutoPtr<IToneGenerator> toneGen = GetOrCreateToneGenerator(streamType);
        if (toneGen != NULL) {
            toneGen->StartTone(IToneGenerator::TONE_PROP_BEEP, &bval);
            AutoPtr<IMessage> message;
            ObtainMessage(MSG_STOP_SOUNDS, (IMessage**)&message);
            SendMessageDelayed(message, BEEP_DURATION, &bval);
        }
    }
}

void CVolumePanel::OnStopSounds()
{
    {
        AutoLock lock(this);
        AutoPtr<IAudioSystemHelper> asHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&asHelper);
        Int32 numStreamTypes;
        asHelper->GetNumStreamTypes(&numStreamTypes);
        for (Int32 i = numStreamTypes - 1; i >= 0; i--) {
            AutoPtr<IToneGenerator> toneGen = (*mToneGenerators)[i];
            if (toneGen.Get() != NULL) {
                toneGen->StopTone();
            }
        }
    }
}

void CVolumePanel::OnVibrate()
{
    // Make sure we ended up in vibrate ringer mode
    Int32 mode;
    mAudioManager->GetRingerMode(&mode);
    if (mode != IAudioManager::RINGER_MODE_VIBRATE) {
        return;
    }

    if (mVibrator != NULL) {
        mVibrator->Vibrate(VIBRATE_DURATION);
    }
}

void CVolumePanel::OnRemoteVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    // streamType is the real stream type being affected, but for the UI sliders, we
    // refer to IAudioService::STREAM_REMOTE_MUSIC. We still play the beeps on the real
    // stream type.
    if (LOGD) Logger::D(TAG, "onRemoteVolumeChanged(stream: %d, flags: %d)",
        streamType, flags);

    Boolean bval;
    if (((flags & IAudioManager::FLAG_SHOW_UI) != 0)
        || (mDialog->IsShowing(&bval), bval)) {
        AutoLock lock(this);
        if (mActiveStreamType != IAudioService::STREAM_REMOTE_MUSIC) {
            ReorderSliders(IAudioService::STREAM_REMOTE_MUSIC);
        }
        OnShowVolumeChanged(IAudioService::STREAM_REMOTE_MUSIC, flags);
    }
    else {
        if (LOGD) Logger::D(TAG, "not calling OnShowVolumeChanged(), no FLAG_SHOW_UI or no UI");
    }

    if ((flags & IAudioManager::FLAG_PLAY_SOUND) != 0 && ! mRingIsSilent) {
        RemoveMessages(MSG_PLAY_SOUND);
        AutoPtr<IMessage> message;
        ObtainMessage(MSG_PLAY_SOUND, streamType, flags, (IMessage**)&message);
        SendMessageDelayed(message, PLAY_SOUND_DELAY, &bval);
    }

    if ((flags & IAudioManager::FLAG_REMOVE_SOUND_AND_VIBRATE) != 0) {
        RemoveMessages(MSG_PLAY_SOUND);
        RemoveMessages(MSG_VIBRATE);
        OnStopSounds();
    }

    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> message;
    ObtainMessage(MSG_FREE_RESOURCES, (IMessage**)&message);
    SendMessageDelayed(message, FREE_DELAY, &bval);

    ResetTimeout();
}

void CVolumePanel::OnRemoteVolumeUpdateIfShown()
{
    if (LOGD) Logger::D(TAG, "onRemoteVolumeUpdateIfShown()");
    Boolean isShowing;
    mDialog->IsShowing(&isShowing);
    if (isShowing
        && (mActiveStreamType == IAudioService::STREAM_REMOTE_MUSIC)
        && !mStreamControls.IsEmpty()) {
        OnShowVolumeChanged(IAudioService::STREAM_REMOTE_MUSIC, 0);
    }
}

void CVolumePanel::OnSliderVisibilityChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 visible)
{
    if (LOGD) Logger::D(TAG, "onSliderVisibilityChanged(stream=%d, visi: %d)", streamType, visible);

    Boolean isVisible = (visible == 1);
    StreamResources* streamRes ;
    for (Int32 i = STREAMS_LENGTH - 1 ; i >= 0 ; i--) {
        streamRes = STREAMS[i];
        if (streamRes->mStreamType == streamType) {
            streamRes->mShow = isVisible;
            if (!isVisible && (mActiveStreamType == streamType)) {
                mActiveStreamType = -1;
            }
            break;
        }
    }
}

void CVolumePanel::OnDisplaySafeVolumeWarning()
{
    {
        AutoLock lock(CVolumePanel::sConfirmSafeVolumeLock);
        if (sConfirmSafeVolumeDialog != NULL) {
            return;
        }

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(R::string::safe_media_volume_warning);
        AutoPtr<IDialogInterfaceOnClickListener> listener = new PositiveButtonOnClickListener(this);
        builder->SetPositiveButton(R::string::yes, listener);
        builder->SetPositiveButton(R::string::no, NULL);
        builder->SetIconAttribute(R::attr::alertDialogIcon);

        builder->Create((IAlertDialog**)&sConfirmSafeVolumeDialog);

        AutoPtr<WarningDialogReceiver> warning = new WarningDialogReceiver(mContext,
            IDialog::Probe(sConfirmSafeVolumeDialog.Get()));

        sConfirmSafeVolumeDialog->SetOnDismissListener(
            (IDialogInterfaceOnDismissListener*)(warning->Probe(EIID_IDialogInterfaceOnDismissListener)));
        AutoPtr<IWindow> window;
        sConfirmSafeVolumeDialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
        sConfirmSafeVolumeDialog->Show();
    }
}

AutoPtr<IToneGenerator> CVolumePanel::GetOrCreateToneGenerator(
    /* [in] */ Int32 streamType)
{
    if (streamType == STREAM_MASTER) {
        // For devices that use the master volume setting only but still want to
        // play a volume-changed tone, direct the master volume pseudostream to
        // the system stream's tone generator.
        if (mPlayMasterStreamTones) {
            streamType = IAudioManager::STREAM_SYSTEM;
        }
        else {
            return NULL;
        }
    }

    {
        AutoLock lock(this);

        if ((*mToneGenerators)[streamType] == NULL) {
            // try {
                AutoPtr<IToneGenerator> tg;
                ECode ec = CToneGenerator::New(streamType, MAX_VOLUME, (IToneGenerator**)&tg);;
                mToneGenerators->Set(streamType, tg);
            // } catch (RuntimeException e) {
                if (FAILED(ec) && LOGD) {
                    Logger::D(TAG, "ToneGenerator constructor failed with RuntimeException: %d", ec);
                }
            // }
        }
        return (*mToneGenerators)[streamType];
    }
    return NULL;
}

void CVolumePanel::SetMusicIcon(
    /* [in] */ Int32 resId,
    /* [in] */ Int32 resMuteId)
{
    HashMap<Int32, AutoPtr<StreamControl> >::Iterator it;
    it = mStreamControls.Find(IAudioManager::STREAM_MUSIC);
    if (it != mStreamControls.End()) {
        AutoPtr<StreamControl> sc = it->mSecond;
        if (sc != NULL) {
            sc->mIconRes = resId;
            sc->mIconMuteRes = resMuteId;
            sc->mIcon->SetImageResource(
                IsMuted(sc->mStreamType) ? sc->mIconMuteRes : sc->mIconRes);
        }
    }
}

void CVolumePanel::OnFreeResources()
{
    {
        AutoLock lock(this);
        for (Int32 i = mToneGenerators->GetLength() - 1; i >= 0; i--) {
            if ((*mToneGenerators)[i] != NULL) {
                (*mToneGenerators)[i]->ReleaseResources();
            }
            mToneGenerators->Set(i, NULL);
        }
    }
}

ECode CVolumePanel::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    if (LOGD) {
        static HashMap<Int32, String> infoMap;
        if (infoMap.IsEmpty()) {
            infoMap[MSG_VOLUME_CHANGED] = String("MSG_VOLUME_CHANGED");
            infoMap[MSG_FREE_RESOURCES] = String("MSG_FREE_RESOURCES");
            infoMap[MSG_PLAY_SOUND] = String("MSG_PLAY_SOUND");
            infoMap[MSG_STOP_SOUNDS] = String("MSG_STOP_SOUNDS");
            infoMap[MSG_VIBRATE] = String("MSG_VIBRATE");
            infoMap[MSG_TIMEOUT] = String("MSG_TIMEOUT");
            infoMap[MSG_RINGER_MODE_CHANGED] = String("MSG_RINGER_MODE_CHANGED");
            infoMap[MSG_MUTE_CHANGED] = String("MSG_MUTE_CHANGED");
            infoMap[MSG_REMOTE_VOLUME_CHANGED] = String("MSG_REMOTE_VOLUME_CHANGED");
            infoMap[MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN] = String("MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN");
            infoMap[MSG_SLIDER_VISIBILITY_CHANGED] = String("MSG_SLIDER_VISIBILITY_CHANGED");
            infoMap[MSG_DISPLAY_SAFE_VOLUME_WARNING] = String("MSG_DISPLAY_SAFE_VOLUME_WARNING");
        }

        Logger::D(TAG, "HandleMessage: what:%s, arg1: %d, arg2: %d",
            infoMap[what].string(), arg1, arg2);
    }

    switch (what) {
        case MSG_VOLUME_CHANGED: {
            OnVolumeChanged(arg1, arg2);
            break;
        }

        case MSG_MUTE_CHANGED: {
            OnMuteChanged(arg1, arg2);
            break;
        }

        case MSG_FREE_RESOURCES: {
            OnFreeResources();
            break;
        }

        case MSG_STOP_SOUNDS: {
            OnStopSounds();
            break;
        }

        case MSG_PLAY_SOUND: {
            OnPlaySound(arg1, arg2);
            break;
        }

        case MSG_VIBRATE: {
            OnVibrate();
            break;
        }

        case MSG_TIMEOUT: {
            Boolean isShowing;
            mDialog->IsShowing(&isShowing);
            if (isShowing) {
                mDialog->Dismiss();
                mActiveStreamType = -1;
            }
            break;
        }
        case MSG_RINGER_MODE_CHANGED: {
            Boolean isShowing;
            mDialog->IsShowing(&isShowing);
            if (isShowing) {
                UpdateStates();
            }
            break;
        }

        case MSG_REMOTE_VOLUME_CHANGED: {
            OnRemoteVolumeChanged(arg1, arg2);
            break;
        }

        case MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN:
            OnRemoteVolumeUpdateIfShown();
            break;

        case MSG_SLIDER_VISIBILITY_CHANGED:
            OnSliderVisibilityChanged(arg1, arg2);
            break;

        case MSG_DISPLAY_SAFE_VOLUME_WARNING:
            OnDisplaySafeVolumeWarning();
            break;
    }
    return NOERROR;
}

void CVolumePanel::ResetTimeout()
{
    RemoveMessages(MSG_TIMEOUT);

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_TIMEOUT, (IMessage**)&message);
    Boolean bval;
    SendMessageDelayed(message, TIMEOUT_DELAY, &bval);
}

void CVolumePanel::ForceTimeout()
{
    RemoveMessages(MSG_TIMEOUT);

    AutoPtr<IMessage> message;
    ObtainMessage(MSG_TIMEOUT, (IMessage**)&message);
    Boolean bval;
    SendMessage(message, &bval);
}

ECode CVolumePanel::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (LOGD) Logger::D("CVolumePanel", "OnProgressChanged progress %d, fromUser %d", progress, fromUser);
    AutoPtr<IInterface> tag;
    seekBar->GetTag((IInterface**)&tag);
    AutoPtr<StreamControl> sc = (StreamControl*)tag.Get();
    if (fromUser && sc != NULL) {
        Int32 volume = GetStreamVolume(sc->mStreamType);
        if (volume != progress) {
            SetStreamVolume(sc->mStreamType, progress, 0);
        }
    }

    ResetTimeout();
    return NOERROR;
}

ECode CVolumePanel::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode CVolumePanel::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    AutoPtr<IInterface> tag;
    seekBar->GetTag((IInterface**)&tag);
    AutoPtr<StreamControl> sc = (StreamControl*)tag.Get();
    if (sc != NULL) {
        // because remote volume updates are asynchronous, AudioService might have received
        // a new remote volume value since the finger adjusted the slider. So when the
        // progress of the slider isn't being tracked anymore, adjust the slider to the last
        // "published" remote volume value, so the UI reflects the actual volume.
        if (sc->mStreamType == IAudioService::STREAM_REMOTE_MUSIC) {
            seekBar->SetProgress(GetStreamVolume(IAudioService::STREAM_REMOTE_MUSIC));
        }
    }
    return NOERROR;
}

ECode CVolumePanel::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    if (id == R::id::expand_button) { // mMoreButton
        Expand();
    }
    ResetTimeout();
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
