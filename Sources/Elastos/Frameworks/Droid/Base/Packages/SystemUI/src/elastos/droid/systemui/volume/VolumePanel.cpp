#include "elastos/droid/systemui/volume/VolumePanel.h"
#include "elastos/droid/systemui/volume/CVolumePanelBroadcastReceiver1.h"
#include "elastos/droid/systemui/volume/CVolumePanelBroadcastReceiver2.h"
#include "elastos/droid/systemui/volume/Interaction.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/app/AlertDialog.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioSystem;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::IVolumeProvider;
using Elastos::Droid::Media::Session::EIID_IMediaControllerCallback;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IZenModeControllerCallback;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//------------------------------------------------------------------------------------
// VolumePanel::StreamResources
//------------------------------------------------------------------------------------

VolumePanel::StreamResources::StreamResources(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 descRes,
    /* [in] */ Int32 iconRes,
    /* [in] */ Int32 iconMuteRes,
    /* [in] */ Boolean show)
    : mStreamType(streamType)
    , mDescRes(descRes)
    , mIconRes(iconRes)
    , mIconMuteRes(iconMuteRes)
    , mShow(show)
{}

//------------------------------------------------------------------------------------
// VolumePanel::ENUM_StreamResources
//------------------------------------------------------------------------------------

static AutoPtr<VolumePanel::StreamResources> initBluetoothSCOStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_BLUETOOTH_SCO,
        Elastos::Droid::R::string::volume_icon_description_bluetooth,
        Elastos::Droid::R::drawable::ic_audio_bt,
        Elastos::Droid::R::drawable::ic_audio_bt,
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initRingerStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_RING,
        Elastos::Droid::R::string::volume_icon_description_ringer,
        R::drawable::ic_ringer_audible,
        R::drawable::ic_ringer_audible,
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initVoiceStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_VOICE_CALL,
        Elastos::Droid::R::string::volume_icon_description_incall,
        Elastos::Droid::R::drawable::ic_audio_phone,
        Elastos::Droid::R::drawable::ic_audio_phone,
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initAlarmStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_ALARM,
        Elastos::Droid::R::string::volume_alarm,
        R::drawable::ic_audio_alarm,
        R::drawable::ic_audio_alarm_mute,
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initMediaStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_MUSIC,
        Elastos::Droid::R::string::volume_icon_description_media,
        R::drawable::ic_audio_vol,/*IC_AUDIO_VOL*/
        R::drawable::ic_audio_vol_mute,/*IC_AUDIO_VOL_MUTE*/
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initNotificationStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        IAudioManager::STREAM_NOTIFICATION,
        Elastos::Droid::R::string::volume_icon_description_notification,
        R::drawable::ic_ringer_audible,
        R::drawable::ic_ringer_vibrate,
        TRUE);
    return sr;
}

// for now, use media resources for master volume
static AutoPtr<VolumePanel::StreamResources> initMasterStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        /*STREAM_MASTER*/-100,
        Elastos::Droid::R::string::volume_icon_description_media, //FIXME should have its own description
        R::drawable::ic_audio_vol,/*IC_AUDIO_VOL*/
        R::drawable::ic_audio_vol_mute,/*IC_AUDIO_VOL_MUTE*/
        FALSE);
    return sr;
}

static AutoPtr<VolumePanel::StreamResources> initRemoteStream()
{
    AutoPtr<VolumePanel::StreamResources> sr = new VolumePanel::StreamResources(
        /*STREAM_REMOTE_MUSIC*/-200,
        Elastos::Droid::R::string::volume_icon_description_media,//FIXME should have its own description
        Elastos::Droid::R::drawable::ic_media_route_on_holo_dark,
        Elastos::Droid::R::drawable::ic_media_route_disabled_holo_dark,
        FALSE);// will be dynamically updated
    return sr;
}

const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::BluetoothSCOStream = initBluetoothSCOStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::RingerStream = initRingerStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::VoiceStream = initVoiceStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::AlarmStream = initAlarmStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::MediaStream = initMediaStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::NotificationStream = initNotificationStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::MasterStream = initMasterStream();
const AutoPtr<VolumePanel::StreamResources> VolumePanel::ENUM_StreamResources::RemoteStream = initRemoteStream();

//------------------------------------------------------------------------------------
// VolumePanel::StreamControl
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::StreamControl, Object, IVolumePanelStreamControl)

//------------------------------------------------------------------------------------
// CVolumePanelBroadcastReceiver2
//------------------------------------------------------------------------------------
CAR_OBJECT_IMPL(CVolumePanelBroadcastReceiver2)
ECode CVolumePanelBroadcastReceiver2::constructor(
    /* [in] */ ISystemUIDialog* host)
{
    mHost = (VolumePanel::SafetyWarning*)host;
    return BroadcastReceiver::constructor();
}

ECode CVolumePanelBroadcastReceiver2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)) {
        if (VolumePanel::LOGD) {
            Logger::D(VolumePanel::TAG, "Received ACTION_CLOSE_SYSTEM_DIALOGS");
        }
        mHost->Cancel();
        mHost->CleanUp();
    }
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::SafetyWarning
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(VolumePanel::SafetyWarning, SystemUIDialog, IDialogInterfaceOnDismissListener, IDialogInterfaceOnClickListener)

VolumePanel::SafetyWarning::SafetyWarning(
    /* [in] */ IContext* context,
    /* [in] */ VolumePanel* volumePanel,
    /* [in] */ IAudioManager* audioManager)
    : mContext(context)
    , mVolumePanel(volumePanel)
    , mAudioManager(audioManager)
{
    SystemUIDialog::constructor(context);

    String s1;
    mContext->GetString(Elastos::Droid::R::string::safe_media_volume_warning, &s1);
    SystemUIDialog::SetMessage(StringUtils::ParseInt32(s1));

    String s2;
    mContext->GetString(Elastos::Droid::R::string::yes, &s2);
    AutoPtr<ICharSequence> cs;
    CString::New(s2, (ICharSequence**)&cs);
    AlertDialog::SetButton(IDialogInterface::BUTTON_POSITIVE, cs, this);
    String s3;
    mContext->GetString(Elastos::Droid::R::string::no, &s3);
    AutoPtr<ICharSequence> cs2;
    CString::New(s3, (ICharSequence**)&cs2);
    AlertDialog::SetButton(IDialogInterface::BUTTON_NEGATIVE, cs2, (IMessage*)NULL);
    Dialog::SetOnDismissListener(this);

    CVolumePanelBroadcastReceiver2::New(this, (IBroadcastReceiver**)&mReceiver);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
}

ECode VolumePanel::SafetyWarning::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 c;
    event->GetRepeatCount(&c);
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_UP && c == 0) {
        mNewVolumeUp = TRUE;
    }
    return Dialog::OnKeyDown(keyCode, event, result);
}

ECode VolumePanel::SafetyWarning::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_UP && mNewVolumeUp) {
        if (LOGD) Logger::D(VolumePanel::TAG, "Confirmed warning via VOLUME_UP");
        mAudioManager->DisableSafeMediaVolume();
        Dismiss();
    }
    return Dialog::OnKeyUp(keyCode, event, result);
}

ECode VolumePanel::SafetyWarning::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mAudioManager->DisableSafeMediaVolume();
}

ECode VolumePanel::SafetyWarning::OnDismiss(
    /* [in] */ IDialogInterface* unused)
{
    mContext->UnregisterReceiver(mReceiver);
    CleanUp();
    return NOERROR;
}

ECode VolumePanel::SafetyWarning::CleanUp()
{
    synchronized(VolumePanel::sSafetyWarningLock) {
        sSafetyWarning = NULL;
    }
    mVolumePanel->ForceTimeout(0);
    mVolumePanel->UpdateStates();
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MySeekListener
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::MySeekListener, Object, ISeekBarOnSeekBarChangeListener)

VolumePanel::MySeekListener::MySeekListener(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::MySeekListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    AutoPtr<IInterface> tag;
    IView::Probe(seekBar)->GetTag((IInterface**)&tag);

    if (fromUser && tag->Probe(EIID_IVolumePanelStreamControl)) {
        AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(tag));
        mHost->SetStreamVolume(sc, progress,
            IAudioManager::FLAG_SHOW_UI | IAudioManager::FLAG_VIBRATE);
    }
    mHost->ResetTimeout();
    return NOERROR;
}

ECode VolumePanel::MySeekListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode VolumePanel::MySeekListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MyZenCallback
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::MyZenCallback, Object, IZenModeControllerCallback)

VolumePanel::MyZenCallback::MyZenCallback(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::MyZenCallback::OnZenAvailableChanged(
    /* [in] */ Boolean available)
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        VolumePanel::MSG_ZEN_MODE_AVAILABLE_CHANGED,
        available ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::MyZenCallback::OnEffectsSupressorChanged()
{
    AutoPtr<IComponentName> name;
    mHost->mZenController->GetEffectsSuppressor((IComponentName**)&name);
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        VolumePanel::MSG_NOTIFICATION_EFFECTS_SUPPRESSOR_CHANGED,
        name,
        (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::MyZenCallback::OnZenChanged(
   /* [in] */ Int32 zen)
{
    return NOERROR;
}

ECode VolumePanel::MyZenCallback::OnExitConditionChanged(
   /* [in] */ ICondition* exitCondition)
{
    return NOERROR;
}

ECode VolumePanel::MyZenCallback::OnConditionsChanged(
   /* [in] */ ArrayOf<ICondition*>* conditions)
{
    return NOERROR;
}

ECode VolumePanel::MyZenCallback::OnNextAlarmChanged()
{
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MyMediaControllerCb
//------------------------------------------------------------------------------------
VolumePanel::MyMediaControllerCb::MyMediaControllerCb(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::MyMediaControllerCb::OnAudioInfoChanged(
    /* [in] */ IMediaControllerPlaybackInfo* info)
{
    return mHost->OnRemoteVolumeUpdateIfShown();
}

//------------------------------------------------------------------------------------
// VolumePanel::MyDialog
//------------------------------------------------------------------------------------

VolumePanel::MyDialog::MyDialog(
    /* [in] */ IContext* ctx,
    /* [in] */ VolumePanel* host)
    : mCtx(ctx)
    , mHost(host)
{
    Dialog::constructor(mCtx);
}

ECode VolumePanel::MyDialog::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action;
    event->GetAction(&action);
    if (mHost->IsShowing() &&
        action == IMotionEvent::ACTION_OUTSIDE && sSafetyWarning == NULL) {
        mHost->ForceTimeout(0);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MyDismissListener
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::MyDismissListener, Object, IDialogInterfaceOnDismissListener)

VolumePanel::MyDismissListener::MyDismissListener(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::MyDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->mActiveStreamType = -1;
    mHost->mAudioManager->ForceVolumeControlStream(mHost->mActiveStreamType);
    mHost->SetZenPanelVisible(FALSE);
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::InteractionCallback
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::InteractionCallback, Object, IInteractionCallback)

VolumePanel::InteractionCallback::InteractionCallback(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::InteractionCallback::OnInteraction()
{
    mHost->ResetTimeout();
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::ZMPCallback
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::ZMPCallback, Object, IZenModePanelCallback)

VolumePanel::ZMPCallback::ZMPCallback(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::ZMPCallback::OnMoreSettings()
{
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnZenSettings();
    }
    return NOERROR;
}

ECode VolumePanel::ZMPCallback::OnInteraction()
{
    mHost->ResetTimeout();
    return NOERROR;
}

ECode VolumePanel::ZMPCallback::OnExpanded(
    /* [in] */ Boolean expanded)
{
    if (mHost->mZenPanelExpanded == expanded) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mHost->mZenPanelExpanded = expanded;
    mHost->UpdateTimeoutDelay();
    mHost->ResetTimeout();
    return NOERROR;
}

//------------------------------------------------------------------------------------
// CVolumePanelBroadcastReceiver1
//------------------------------------------------------------------------------------
CAR_OBJECT_IMPL(CVolumePanelBroadcastReceiver1)
ECode CVolumePanelBroadcastReceiver1::constructor(
    /* [in] */ IVolumePanel* host)
{
    mHost = (VolumePanel*)host;
    return BroadcastReceiver::constructor();
}

ECode CVolumePanelBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IAudioManager::RINGER_MODE_CHANGED_ACTION.Equals(action)) {
        mHost->RemoveMessages(VolumePanel::MSG_RINGER_MODE_CHANGED);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(VolumePanel::MSG_RINGER_MODE_CHANGED, (IMessage**)&msg);
        Boolean b;
        mHost->SendMessage(msg, &b);
    }

    if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->PostDismiss(0);
    }
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MyOnClickListener
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::MyOnClickListener, Object, IViewOnClickListener)

VolumePanel::MyOnClickListener::MyOnClickListener(
    /* [in] */ StreamControl* sc,
    /* [in] */ VolumePanel* host)
    : mSc(sc)
    , mHost(host)
{}

ECode VolumePanel::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->ResetTimeout();
    mHost->Toggle(mSc);
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel::MyOnTouchListener
//------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(VolumePanel::MyOnTouchListener, Object, IViewOnTouchListener)

VolumePanel::MyOnTouchListener::MyOnTouchListener(
    /* [in] */ VolumePanel* host)
    : mHost(host)
{}

ECode VolumePanel::MyOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->ResetTimeout();
    mHost->ShowSilentHint();
    *result = FALSE;
    return NOERROR;
}

//------------------------------------------------------------------------------------
// VolumePanel
//------------------------------------------------------------------------------------

static AutoPtr<IAudioAttributes> initVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> aab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aab);
    aab->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    aab->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    AutoPtr<IAudioAttributes> aa;
    aab->Build((IAudioAttributes**)&aa);
    return aa;
}

static AutoPtr<ArrayOf<VolumePanel::StreamResources*> > initSTREAMS()
{
    AutoPtr<ArrayOf<VolumePanel::StreamResources*> > streams = ArrayOf<VolumePanel::StreamResources*>::Alloc(8);
    streams->Set(0, VolumePanel::ENUM_StreamResources::BluetoothSCOStream);
    streams->Set(1, VolumePanel::ENUM_StreamResources::RingerStream);
    streams->Set(2, VolumePanel::ENUM_StreamResources::VoiceStream);
    streams->Set(3, VolumePanel::ENUM_StreamResources::MediaStream);
    streams->Set(4, VolumePanel::ENUM_StreamResources::NotificationStream);
    streams->Set(5, VolumePanel::ENUM_StreamResources::AlarmStream);
    streams->Set(6, VolumePanel::ENUM_StreamResources::MasterStream);
    streams->Set(7, VolumePanel::ENUM_StreamResources::RemoteStream);
    return streams;
}

const String VolumePanel::TAG("VolumePanel");
Boolean VolumePanel::LOGD = Logger::IsLoggable(TAG.string(), Logger::___DEBUG);
const Int32 VolumePanel::PLAY_SOUND_DELAY;
const Int32 VolumePanel::VIBRATE_DELAY;
const Int32 VolumePanel::VIBRATE_DURATION;
const Int32 VolumePanel::BEEP_DURATION;
const Int32 VolumePanel::MAX_VOLUME;
const Int32 VolumePanel::FREE_DELAY;
const Int32 VolumePanel::TIMEOUT_DELAY;
const Int32 VolumePanel::TIMEOUT_DELAY_SHORT;
const Int32 VolumePanel::TIMEOUT_DELAY_COLLAPSED;
const Int32 VolumePanel::TIMEOUT_DELAY_SAFETY_WARNING;
const Int32 VolumePanel::TIMEOUT_DELAY_EXPANDED;
const Int32 VolumePanel::MSG_VOLUME_CHANGED;
const Int32 VolumePanel::MSG_FREE_RESOURCES;
const Int32 VolumePanel::MSG_PLAY_SOUND;
const Int32 VolumePanel::MSG_STOP_SOUNDS;
const Int32 VolumePanel::MSG_VIBRATE;
const Int32 VolumePanel::MSG_TIMEOUT;
const Int32 VolumePanel::MSG_RINGER_MODE_CHANGED;
const Int32 VolumePanel::MSG_MUTE_CHANGED;
const Int32 VolumePanel::MSG_REMOTE_VOLUME_CHANGED;
const Int32 VolumePanel::MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN;
const Int32 VolumePanel::MSG_SLIDER_VISIBILITY_CHANGED;
const Int32 VolumePanel::MSG_DISPLAY_SAFE_VOLUME_WARNING;
const Int32 VolumePanel::MSG_LAYOUT_DIRECTION;
const Int32 VolumePanel::MSG_ZEN_MODE_AVAILABLE_CHANGED;
const Int32 VolumePanel::MSG_USER_ACTIVITY;
const Int32 VolumePanel::MSG_NOTIFICATION_EFFECTS_SUPPRESSOR_CHANGED;
const Int32 VolumePanel::STREAM_MASTER;
const Int32 VolumePanel::STREAM_REMOTE_MUSIC;
const AutoPtr<IAudioAttributes> VolumePanel::VIBRATION_ATTRIBUTES = initVIBRATION_ATTRIBUTES();
const Int32 VolumePanel::IC_AUDIO_VOL;
const Int32 VolumePanel::IC_AUDIO_VOL_MUTE;
const AutoPtr<ArrayOf<VolumePanel::StreamResources*> > VolumePanel::STREAMS = initSTREAMS();
AutoPtr<IAlertDialog> VolumePanel::sSafetyWarning;
Object VolumePanel::sSafetyWarningLock;

CAR_INTERFACE_IMPL(VolumePanel, Handler, IVolumePanel)
VolumePanel::VolumePanel()
    : mRingIsSilent(FALSE)
    , mVoiceCapable(FALSE)
    , mZenModeAvailable(FALSE)
    , mZenPanelExpanded(FALSE)
    , mTimeoutDelay(TIMEOUT_DELAY)
    , mDisabledAlpha(0.0f)
    , mLastRingerMode(IAudioManager::RINGER_MODE_NORMAL)
    , mLastRingerProgress(0)
    , mPlayMasterStreamTones(FALSE)
    , mActiveStreamType(-1)
{}

ECode VolumePanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IZenModeController* zenController)
{
    mSeekListener = new MySeekListener(this);
    mZenCallback = new MyZenCallback(this);
    mMediaControllerCb = new MyMediaControllerCb(this);

    mContext = context;
    mZenController = zenController;
    Int32 hashCode;
    GetHashCode(&hashCode);
    mTag.AppendFormat("%s.%08x", TAG.string(),hashCode);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
    AutoPtr<IInterface> obj2;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj2);
    mAccessibilityManager = IAccessibilityManager::Probe(obj2);

    // For now, only show master volume if master volume is supported
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean useMasterVolume;
    res->GetBoolean(Elastos::Droid::R::bool_::config_useMasterVolume, &useMasterVolume);
    if (useMasterVolume) {
        for (Int32 i = 0; i < STREAMS->GetLength(); i++) {
            AutoPtr<StreamResources> streamRes = (*STREAMS)[i];
            streamRes->mShow = (streamRes->mStreamType == STREAM_MASTER);
        }
    }
    if (LOGD) Logger::D(mTag, "new VolumePanel");

    mDisabledAlpha = 0.5f;
    AutoPtr<IResourcesTheme> theme;
    mContext->GetTheme((IResourcesTheme**)&theme);
    if (theme != NULL) {
        AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(1);
        (*attrs)[0] = Elastos::Droid::R::attr::disabledAlpha;
        AutoPtr<ITypedArray> attr;
        theme->ObtainStyledAttributes(attrs, (ITypedArray**)&attr);
        attr->GetFloat(0, mDisabledAlpha, &mDisabledAlpha);
        attr->Recycle();
    }

    mDialog = new MyDialog(context, this);

    AutoPtr<IWindow> window;
    mDialog->GetWindow((IWindow**)&window);
    Boolean b1;
    window->RequestFeature(IWindow::FEATURE_NO_TITLE, &b1);
    mDialog->SetCanceledOnTouchOutside(TRUE);
    mDialog->SetContentView(R::layout::volume_dialog);

    AutoPtr<MyDismissListener> dl = new MyDismissListener(this);

    mDialog->SetOnDismissListener(dl);

    mDialog->Create();

    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);
    lp->SetToken(NULL);
    Int32 offset;
    res->GetDimensionPixelOffset(R::dimen::volume_panel_top, &offset);
    lp->SetY(offset);
    lp->SetType(IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL);
    lp->SetFormat(IPixelFormat::TRANSLUCENT);
    lp->SetWindowAnimations(R::style::VolumePanelAnimation);
    AutoPtr<ICharSequence> title;
    CString::New(TAG, (ICharSequence**)&title);
    lp->SetTitle(title);
    window->SetAttributes(lp);

    UpdateWidth();

    AutoPtr<IDrawable> cd;
    CColorDrawable::New(0x00000000, (IDrawable**)&cd);
    window->SetBackgroundDrawable(cd);
    window->ClearFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
        | IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH
        | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    window->FindViewById(Elastos::Droid::R::id::content, (IView**)&mView);
    AutoPtr<InteractionCallback> icb = new InteractionCallback(this);
    Interaction::Register(mView, icb);

    AutoPtr<IView> v1;
    mView->FindViewById(R::id::visible_panel, (IView**)&v1);
    mPanel = IViewGroup::Probe(v1);
    AutoPtr<IView> v2;
    mView->FindViewById(R::id::slider_panel, (IView**)&v2);
    mSliderPanel = IViewGroup::Probe(v2);
    AutoPtr<IView> v3;
    mView->FindViewById(R::id::zen_mode_panel, (IView**)&v3);
    mZenPanel = (ZenModePanel*)v3.Get();
    InitZenModePanel();

    AutoPtr<IAudioSystem> as;
    CAudioSystem::AcquireSingleton((IAudioSystem**)&as);
    Int32 nst;
    as->GetNumStreamTypes(&nst);
    mToneGenerators = ArrayOf<IToneGenerator*>::Alloc(nst);
    AutoPtr<IInterface> obj3;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj3);
    mVibrator = IVibrator::Probe(obj3);
    res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &mVoiceCapable);

    if (mZenController != NULL && !useMasterVolume) {
        mZenController->IsZenAvailable(&mZenModeAvailable);
        mZenController->AddCallback(mZenCallback);
    }

    Boolean masterVolumeOnly;
    res->GetBoolean(Elastos::Droid::R::bool_::config_useMasterVolume, &masterVolumeOnly);
    Boolean masterVolumeKeySounds;
    res->GetBoolean(Elastos::Droid::R::bool_::config_useVolumeKeySounds, &masterVolumeKeySounds);
    mPlayMasterStreamTones = masterVolumeOnly && masterVolumeKeySounds;

    RegisterReceiver();

    return Handler::constructor((IHandlerCallback*)NULL, FALSE);
}

ECode VolumePanel::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    UpdateWidth();
    if (mZenPanel != NULL) {
        mZenPanel->UpdateLocale();
    }
    return NOERROR;
}

void VolumePanel::UpdateWidth()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IWindow> window;
    mDialog->GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);
    Int32 size;
    res->GetDimensionPixelSize(R::dimen::notification_panel_width, &size);
    IViewGroupLayoutParams::Probe(lp)->SetWidth(size);
    Int32 i;
    res->GetInteger(R::integer::notification_panel_layout_gravity, &i);
    lp->SetGravity(i);
    window->SetAttributes(lp);
}

void VolumePanel::InitZenModePanel()
{
    mZenPanel->Init(mZenController);
    AutoPtr<ZMPCallback> cb = new ZMPCallback(this);
    mZenPanel->SetCallback(cb);
}

void VolumePanel::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    IView::Probe(mPanel)->SetLayoutDirection(layoutDirection);
    UpdateStates();
}

void VolumePanel::RegisterReceiver()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IAudioManager::RINGER_MODE_CHANGED_ACTION);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IBroadcastReceiver> br;
    CVolumePanelBroadcastReceiver1::New(this, (IBroadcastReceiver**)&br);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(br, filter, (IIntent**)&intent);
}

ECode VolumePanel::IsMuted(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* isMuted)
{
    VALIDATE_NOT_NULL(isMuted)
    if (streamType == STREAM_MASTER) {
        return mAudioManager->IsMasterMute(isMuted);
    }
    else if (streamType == STREAM_REMOTE_MUSIC) {
        // TODO do we need to support a distinct mute property for remote?
        *isMuted = FALSE;
        return NOERROR;
    }
    else {
        return mAudioManager->IsStreamMute(streamType, isMuted);
    }
}

ECode VolumePanel::GetStreamMaxVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* smv)
{
    VALIDATE_NOT_NULL(smv)
    if (streamType == STREAM_MASTER) {
        return mAudioManager->GetMasterMaxVolume(smv);
    }
    else if (streamType == STREAM_REMOTE_MUSIC) {
        if (mStreamControls != NULL) {
            AutoPtr<IInterface> vpsc;
            mStreamControls->Get(streamType, (IInterface**)&vpsc);
            AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));
            if (sc != NULL && sc->mController != NULL) {
                AutoPtr<IMediaControllerPlaybackInfo> ai;
                sc->mController->GetPlaybackInfo((IMediaControllerPlaybackInfo**)&ai);
                return ai->GetMaxVolume(smv);
            }
        }
        *smv = -1;
    }
    else {
        return mAudioManager->GetStreamMaxVolume(streamType, smv);
    }
    return NOERROR;
}

ECode VolumePanel::GetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* sv)
{
    if (streamType == STREAM_MASTER) {
        return mAudioManager->GetMasterVolume(sv);
    }
    else if (streamType == STREAM_REMOTE_MUSIC) {
        if (mStreamControls != NULL) {
            AutoPtr<IInterface> vpsc;
            mStreamControls->Get(streamType, (IInterface**)&vpsc);
            AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));
            if (sc != NULL && sc->mController != NULL) {
                AutoPtr<IMediaControllerPlaybackInfo> ai;
                sc->mController->GetPlaybackInfo((IMediaControllerPlaybackInfo**)&ai);
                return ai->GetCurrentVolume(sv);
            }
        }
        *sv = -1;
        return NOERROR;
    }
    else {
        return mAudioManager->GetStreamVolume(streamType, sv);
    }
}

void VolumePanel::SetStreamVolume(
    /* [in] */ StreamControl* sc,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    Int32 sv;
    GetStreamVolume(sc->mStreamType, &sv);
    if (sc->mStreamType == STREAM_REMOTE_MUSIC) {
        if (sc->mController != NULL) {
            sc->mController->SetVolumeTo(index, flags);
        }
        else {
            Logger::W(mTag, "Adjusting remote volume without a controller!");
        }
    }
    else if (sv != index) {
        if (sc->mStreamType == STREAM_MASTER) {
            mAudioManager->SetMasterVolume(index, flags);
        }
        else {
            mAudioManager->SetStreamVolume(sc->mStreamType, index, flags);
        }
    }
}

void VolumePanel::CreateSliders()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    CSparseArray::New(STREAMS->GetLength(), (ISparseArray**)&mStreamControls);

    for (Int32 i = 0; i < STREAMS->GetLength(); i++) {
        AutoPtr<StreamResources> streamRes = (*STREAMS)[i];

        const Int32 streamType = streamRes->mStreamType;

        AutoPtr<StreamControl> sc = new StreamControl();
        sc->mStreamType = streamType;
        AutoPtr<IView> v1;
        inflater->Inflate(R::layout::volume_panel_item, NULL, (IView**)&v1);
        sc->mGroup = IViewGroup::Probe(v1);
        IView::Probe(sc->mGroup)->SetTag((IVolumePanelStreamControl*)sc);

        AutoPtr<IView> v2;
        IView::Probe(sc->mGroup)->FindViewById(R::id::stream_icon, (IView**)&v2);
        sc->mIcon = IImageView::Probe(v2);
        IView::Probe(sc->mIcon)->SetTag((IVolumePanelStreamControl*)sc);

        String str;
        res->GetString(streamRes->mDescRes, &str);
        AutoPtr<ICharSequence> cdes;
        CString::New(str, (ICharSequence**)&cdes);
        IView::Probe(sc->mIcon)->SetContentDescription(cdes);
        sc->mIconRes = streamRes->mIconRes;
        sc->mIconMuteRes = streamRes->mIconMuteRes;
        sc->mIcon->SetImageResource(sc->mIconRes);
        IView::Probe(sc->mIcon)->SetClickable(IsNotificationOrRing(streamType));
        Boolean isClickable;
        IView::Probe(sc->mIcon)->IsClickable(&isClickable);
        if (isClickable) {
            IView::Probe(sc->mIcon)->SetSoundEffectsEnabled(FALSE);
            AutoPtr<MyOnClickListener> ocl = new MyOnClickListener(sc, this);
            IView::Probe(sc->mIcon)->SetOnClickListener(ocl);
            sc->mIconSuppressedRes = R::drawable::ic_ringer_mute;
        }
        AutoPtr<IView> v3;
        IView::Probe(sc->mGroup)->FindViewById(R::id::seekbar, (IView**)&v3);
        sc->mSeekbarView = ISeekBar::Probe(v3);

        AutoPtr<IView> v4;
        IView::Probe(sc->mGroup)->FindViewById(R::id::suppressor, (IView**)&v4);
        sc->mSuppressorView = ITextView::Probe(v4);

        IView::Probe(sc->mSuppressorView)->SetVisibility(IView::GONE);
        const Int32 plusOne = (streamType == IAudioSystem::STREAM_BLUETOOTH_SCO ||
            streamType == IAudioSystem::STREAM_VOICE_CALL) ? 1 : 0;

        Int32 smvolume;
        GetStreamMaxVolume(streamType, &smvolume);

        IProgressBar::Probe(sc->mSeekbarView)->SetMax(smvolume + plusOne);
        sc->mSeekbarView->SetOnSeekBarChangeListener(mSeekListener);
        IView::Probe(sc->mSeekbarView)->SetTag((IVolumePanelStreamControl*)sc);
        mStreamControls->Put(streamType, (IVolumePanelStreamControl*)sc);
    }
}

void VolumePanel::Toggle(
    /* [in] */ StreamControl* sc)
{
    Int32 rm;
    mAudioManager->GetRingerMode(&rm);
    if (rm == IAudioManager::RINGER_MODE_NORMAL) {
        mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_VIBRATE);
        PostVolumeChanged(sc->mStreamType, IAudioManager::FLAG_SHOW_UI | IAudioManager::FLAG_VIBRATE);
    }
    else {
        mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_NORMAL);
        PostVolumeChanged(sc->mStreamType, IAudioManager::FLAG_PLAY_SOUND);
    }
}

void VolumePanel::ReorderSliders(
    /* [in] */ Int32 activeStreamType)
{
    mSliderPanel->RemoveAllViews();
    AutoPtr<IInterface> vpsc;
    mStreamControls->Get(activeStreamType, (IInterface**)&vpsc);
    AutoPtr<StreamControl> active = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));
    if (active == NULL) {
        Logger::E(TAG, "Missing stream type! - %d", activeStreamType);
        mActiveStreamType = -1;
    }
    else {
        mSliderPanel->AddView(IView::Probe(active->mGroup));
        mActiveStreamType = activeStreamType;
        IView::Probe(active->mGroup)->SetVisibility(IView::VISIBLE);
        UpdateSlider(active);
        UpdateTimeoutDelay();
        UpdateZenPanelVisible();
    }
}

void VolumePanel::UpdateSliderProgress(
    /* [in] */ StreamControl* sc,
    /* [in] */ Int32 progress)
{
    Boolean isRinger = IsNotificationOrRing(sc->mStreamType);
    Int32 rm;
    mAudioManager->GetRingerMode(&rm);
    if (isRinger && rm == IAudioManager::RINGER_MODE_SILENT) {
        progress = mLastRingerProgress;
    }
    if (progress < 0) {
        GetStreamVolume(sc->mStreamType, &progress);
    }
    IProgressBar::Probe(sc->mSeekbarView)->SetProgress(progress);
    if (isRinger) {
        mLastRingerProgress = progress;
    }
}

void VolumePanel::UpdateSliderIcon(
    /* [in] */ StreamControl* sc,
    /* [in] */ Boolean muted)
{
    if (IsNotificationOrRing(sc->mStreamType)) {
        Int32 ringerMode;
        mAudioManager->GetRingerMode(&ringerMode);
        if (ringerMode == IAudioManager::RINGER_MODE_SILENT) {
            ringerMode = mLastRingerMode;
        }
        else {
            mLastRingerMode = ringerMode;
        }
        muted = ringerMode == IAudioManager::RINGER_MODE_VIBRATE;
    }
    sc->mIcon->SetImageResource(muted ? sc->mIconMuteRes : sc->mIconRes);
}

void VolumePanel::UpdateSliderSupressor(
    /* [in] */ StreamControl* sc)
{
    AutoPtr<IComponentName> suppressor;
    if (IsNotificationOrRing(sc->mStreamType)) {
        mZenController->GetEffectsSuppressor((IComponentName**)&suppressor);
    }
    else {
        suppressor = NULL;
    }

    if (suppressor == NULL) {
        IView::Probe(sc->mSeekbarView)->SetVisibility(IView::VISIBLE);
        IView::Probe(sc->mSuppressorView)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(sc->mSeekbarView)->SetVisibility(IView::GONE);
        IView::Probe(sc->mSuppressorView)->SetVisibility(IView::VISIBLE);

        AutoPtr<ICharSequence> cs;
        CString::New(GetSuppressorCaption(suppressor), (ICharSequence**)&cs);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, cs);
        String str;
        mContext->GetString(R::string::muted_by, array, &str);
        AutoPtr<ICharSequence> cs2;
        CString::New(str, (ICharSequence**)&cs2);
        sc->mSuppressorView->SetText(cs2);
        sc->mIcon->SetImageResource(sc->mIconSuppressedRes);
    }
}

String VolumePanel::GetSuppressorCaption(
    /* [in] */ IComponentName* suppressor)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IServiceInfo> info;
    pm->GetServiceInfo(suppressor, 0, (IServiceInfo**)&info);
    if (info != NULL) {
        AutoPtr<ICharSequence> seq;
        IPackageItemInfo::Probe(info)->LoadLabel(pm, (ICharSequence**)&seq);
        if (seq != NULL) {
            String str;
            seq->ToString(&str);
            str.Trim();
            if (str.GetLength() > 0) {
                return str;
            }
        }
    }
    // } catch (Throwable e) {
        // Log.w(TAG, "Error loading suppressor caption", e);
    // }
    String packageName;
    suppressor->GetPackageName(&packageName);
    return packageName;
}

void VolumePanel::UpdateSlider(
    /* [in] */ StreamControl* sc)
{
    UpdateSliderProgress(sc, -1);
    Boolean muted;
    IsMuted(sc->mStreamType, &muted);
    // Force reloading the image resource
    sc->mIcon->SetImageDrawable(NULL);
    UpdateSliderIcon(sc, muted);
    UpdateSliderEnabled(sc, muted, FALSE);
    UpdateSliderSupressor(sc);
}

void VolumePanel::UpdateSliderEnabled(
    /* [in] */ StreamControl* sc,
    /* [in] */ Boolean muted,
    /* [in] */ Boolean fixedVolume)
{
    Int32 rm;
    mAudioManager->GetRingerMode(&rm);
    Int32 mst;
    mAudioManager->GetMasterStreamType(&mst);

    Boolean wasEnabled;
    IView::Probe(sc->mSeekbarView)->IsEnabled(&wasEnabled);
    const Boolean isRinger = IsNotificationOrRing(sc->mStreamType);
    if (sc->mStreamType == STREAM_REMOTE_MUSIC) {
        // never disable touch interactions for remote playback, the muting is not tied to
        // the state of the phone.
        IView::Probe(sc->mSeekbarView)->SetEnabled(!fixedVolume);
    }
    else if (isRinger && rm == IAudioManager::RINGER_MODE_SILENT) {
        IView::Probe(sc->mSeekbarView)->SetEnabled(FALSE);
        IView::Probe(sc->mIcon)->SetEnabled(FALSE);
        IView::Probe(sc->mIcon)->SetAlpha(mDisabledAlpha);
        IView::Probe(sc->mIcon)->SetClickable(FALSE);
    }
    else if (fixedVolume ||
        (sc->mStreamType != mst && muted) ||
        (sSafetyWarning != NULL)) {
        IView::Probe(sc->mSeekbarView)->SetEnabled(FALSE);
    }
    else {
        IView::Probe(sc->mSeekbarView)->SetEnabled(TRUE);
        IView::Probe(sc->mIcon)->SetEnabled(TRUE);
        IView::Probe(sc->mIcon)->SetAlpha(1.0f);
    }
    // show the silent hint when the disabled slider is touched in silent mode
    Boolean isEnabled;
    IView::Probe(sc->mSeekbarView)->IsEnabled(&isEnabled);
    if (isRinger && wasEnabled != isEnabled) {
        if (isEnabled) {
            IView::Probe(sc->mGroup)->SetOnTouchListener(NULL);
            IView::Probe(sc->mIcon)->SetClickable(TRUE);
        }
        else {
            AutoPtr<MyOnTouchListener> showHintOnTouch = new MyOnTouchListener(this);
            IView::Probe(sc->mGroup)->SetOnTouchListener(showHintOnTouch);
        }
    }
}

void VolumePanel::ShowSilentHint()
{
    if (mZenPanel != NULL) {
        mZenPanel->ShowSilentHint();
    }
}

Boolean VolumePanel::IsNotificationOrRing(
    /* [in] */ Int32 streamType)
{
    return streamType == IAudioManager::STREAM_RING
        || streamType == IAudioManager::STREAM_NOTIFICATION;
}

ECode VolumePanel::SetCallback(
    /* [in] */ IVolumePanelCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

void VolumePanel::UpdateTimeoutDelay()
{
    Boolean b;
    IsZenPanelVisible(&b);
    mTimeoutDelay = sSafetyWarning != NULL ? TIMEOUT_DELAY_SAFETY_WARNING
        : mActiveStreamType == IAudioManager::STREAM_MUSIC ? TIMEOUT_DELAY_SHORT
        : mZenPanelExpanded ? TIMEOUT_DELAY_EXPANDED
        : b ? TIMEOUT_DELAY_COLLAPSED
        : TIMEOUT_DELAY;
}

ECode VolumePanel::IsZenPanelVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 visibility;
    mZenPanel->GetVisibility(&visibility);
    *result = mZenPanel != NULL && visibility == IView::VISIBLE;
    return NOERROR;
}

void VolumePanel::SetZenPanelVisible(
    /* [in] */ Boolean visible)
{
    if (LOGD) Logger::D(mTag, "setZenPanelVisible %s mZenPanel=%s",
        visible ? "TRUE" : "FALSE", TO_CSTR(mZenPanel));
    Boolean b;
    IsZenPanelVisible(&b);
    const Boolean changing = visible != b;
    if (visible) {
        mZenPanel->SetHidden(FALSE);
        ResetTimeout();
    }
    else {
        mZenPanel->SetHidden(TRUE);
    }
    if (changing) {
        UpdateTimeoutDelay();
        ResetTimeout();
    }
}

ECode VolumePanel::UpdateStates()
{
    Int32 count;
    mSliderPanel->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mSliderPanel->GetChildAt(i, (IView**)&child);
        AutoPtr<IInterface> tag;
        child->GetTag((IInterface**)&tag);
        AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(tag));
        UpdateSlider(sc);
    }
    return NOERROR;
}

void VolumePanel::UpdateZenPanelVisible()
{
    SetZenPanelVisible(mZenModeAvailable && IsNotificationOrRing(mActiveStreamType));
}

ECode VolumePanel::PostVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean result;
    HasMessages(MSG_VOLUME_CHANGED, &result);
    if (result) return E_NULL_POINTER_EXCEPTION;
    synchronized(this) {
        if (mStreamControls == NULL) {
            CreateSliders();
        }
    }
    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_VOLUME_CHANGED, streamType, flags, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostRemoteVolumeChanged(
    /* [in] */ IMediaController* controller,
    /* [in] */ Int32 flags)
{
    Boolean result;
    HasMessages(MSG_REMOTE_VOLUME_CHANGED, &result);
    if (result) return E_NULL_POINTER_EXCEPTION;
    synchronized(this) {
        if (mStreamControls == NULL) {
            CreateSliders();
        }
    }
    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_REMOTE_VOLUME_CHANGED, flags, 0, controller, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostRemoteSliderVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_SLIDER_VISIBILITY_CHANGED,
        STREAM_REMOTE_MUSIC, visible ? 1 : 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostHasNewRemotePlaybackInfo()
{
    Boolean result;
    HasMessages(MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN, &result);
    if (result) return E_NULL_POINTER_EXCEPTION;
    // don't create or prevent resources to be freed, if they disappear, this update came too
    //   late and shouldn't warrant the panel to be displayed longer
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostMasterVolumeChanged(
    /* [in] */ Int32 flags)
{
    return PostVolumeChanged(STREAM_MASTER, flags);
}

ECode VolumePanel::PostMuteChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean result;
    HasMessages(MSG_VOLUME_CHANGED, &result);
    if (result) return E_NULL_POINTER_EXCEPTION;
    synchronized(this) {
        if (mStreamControls == NULL) {
            CreateSliders();
        }
    }
    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_MUTE_CHANGED, streamType, flags, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostMasterMuteChanged(
    /* [in] */ Int32 flags)
{
    return PostMuteChanged(STREAM_MASTER, flags);
}

ECode VolumePanel::PostDisplaySafeVolumeWarning(
    /* [in] */ Int32 flags)
{
    Boolean result;
    HasMessages(MSG_DISPLAY_SAFE_VOLUME_WARNING, &result);
    if (result) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_DISPLAY_SAFE_VOLUME_WARNING, flags, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::PostDismiss(
    /* [in] */ Int64 delay)
{
    ForceTimeout(delay);
    return NOERROR;
}

ECode VolumePanel::PostLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    RemoveMessages(MSG_LAYOUT_DIRECTION);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_LAYOUT_DIRECTION, layoutDirection, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode VolumePanel::OnVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if (LOGD) Logger::D(mTag, "onVolumeChanged(streamType: %d, flags: %d)", streamType, flags);

    if ((flags & IAudioManager::FLAG_SHOW_UI) != 0) {
        synchronized(this) {
            if (mActiveStreamType != streamType) {
                ReorderSliders(streamType);
            }
            OnShowVolumeChanged(streamType, flags, NULL);
        }
    }

    if ((flags & IAudioManager::FLAG_PLAY_SOUND) != 0 && ! mRingIsSilent) {
        RemoveMessages(MSG_PLAY_SOUND);
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_PLAY_SOUND, streamType, flags, (IMessage**)&msg);
        Boolean b;
        SendMessageDelayed(msg, PLAY_SOUND_DELAY, &b);
    }

    if ((flags & IAudioManager::FLAG_REMOVE_SOUND_AND_VIBRATE) != 0) {
        RemoveMessages(MSG_PLAY_SOUND);
        RemoveMessages(MSG_VIBRATE);
        OnStopSounds();
    }

    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_FREE_RESOURCES, (IMessage**)&msg);
    Boolean b2;
    SendMessageDelayed(msg, FREE_DELAY, &b2);
    ResetTimeout();
    return NOERROR;
}

ECode VolumePanel::OnMuteChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if (LOGD) Logger::D(mTag, "onMuteChanged(streamType: %d, flags: %d)",streamType, flags);

    AutoPtr<IInterface> vpsc;
    mStreamControls->Get(streamType, (IInterface**)&vpsc);
    AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));

    if (sc != NULL) {
        Boolean b;
        IsMuted(sc->mStreamType, &b);
        UpdateSliderIcon(sc, b);
    }

    OnVolumeChanged(streamType, flags);
    return NOERROR;
}

ECode VolumePanel::OnShowVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags,
    /* [in] */ IMediaController* controller)
{
    Int32 index;
    GetStreamVolume(streamType, &index);

    mRingIsSilent = FALSE;

    if (LOGD) {
        Logger::D(mTag, "onShowVolumeChanged(streamType: %d, flags: %d), index: %d",
            streamType, flags ,index);
    }

    // get max volume for progress bar

    Int32 max;
    GetStreamMaxVolume(streamType, &max);
    AutoPtr<IInterface> vpsc;
    mStreamControls->Get(streamType, (IInterface**)&vpsc);
    AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));

    switch (streamType) {

        case IAudioManager::STREAM_RING: {
//                setRingerIcon();
            AutoPtr<IRingtoneManagerHelper> rmh;
            CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmh);
            AutoPtr<IUri> ringuri;
            rmh->GetActualDefaultRingtoneUri(mContext, IRingtoneManager::TYPE_RINGTONE, (IUri**)&ringuri);
            if (ringuri == NULL) {
                mRingIsSilent = TRUE;
            }
            break;
        }

        case IAudioManager::STREAM_MUSIC: {
            // Special case for when Bluetooth is active for music
            Int32 dfm;
            mAudioManager->GetDevicesForStream(IAudioManager::STREAM_MUSIC, &dfm);
            if ((dfm &
                    (IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP |
                    IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
                    IAudioManager::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)) != 0) {
                SetMusicIcon(Elastos::Droid::R::drawable::ic_audio_bt, Elastos::Droid::R::drawable::ic_audio_bt_mute);
            }
            else {
                SetMusicIcon(IC_AUDIO_VOL, IC_AUDIO_VOL_MUTE);
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
            AutoPtr<IRingtoneManagerHelper> rmh;
            CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmh);
            AutoPtr<IUri> ringuri;
            rmh->GetActualDefaultRingtoneUri(mContext, IRingtoneManager::TYPE_NOTIFICATION, (IUri**)&ringuri);
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

        case STREAM_REMOTE_MUSIC: {
            if (controller == NULL && sc != NULL) {
                // If we weren't passed one try using the last one set.
                controller = sc->mController;
            }
            if (controller == NULL) {
                // We still don't have one, ignore the command.
                Logger::W(mTag, "sent remote volume change without a controller!");
            }
            else {
                AutoPtr<IMediaControllerPlaybackInfo> vi;
                controller->GetPlaybackInfo((IMediaControllerPlaybackInfo**)&vi);
                vi->GetCurrentVolume(&index);
                vi->GetMaxVolume(&max);
                Int32 vc;
                vi->GetVolumeControl(&vc);
                if ((vc & IVolumeProvider::VOLUME_CONTROL_FIXED) != 0) {
                    // if the remote volume is fixed add the flag for the UI
                    flags |= IAudioManager::FLAG_FIXED_VOLUME;
                }
            }
            if (LOGD) {
                Logger::D(mTag, "showing remote volume %d over %d", index, max);
            }
            break;
        }
    }

    if (sc != NULL) {
        if (streamType == STREAM_REMOTE_MUSIC && controller != sc->mController) {
            if (sc->mController != NULL) {
                sc->mController->UnregisterCallback(mMediaControllerCb);
            }
            sc->mController = controller;
            if (controller != NULL) {
                sc->mController->RegisterCallback(mMediaControllerCb);
            }
        }
        Int32 pbv;
        IProgressBar::Probe(sc->mSeekbarView)->GetMax(&pbv);
        if (pbv != max) {
            IProgressBar::Probe(sc->mSeekbarView)->SetMax(max);
        }
        UpdateSliderProgress(sc, index);
        Boolean isMuted;
        IsMuted(streamType, &isMuted);
        UpdateSliderEnabled(sc, isMuted, (flags & IAudioManager::FLAG_FIXED_VOLUME) != 0);
    }

    if (!IsShowing()) {
        Int32 stream = (streamType == STREAM_REMOTE_MUSIC) ? -1 : streamType;
        // when the stream is for remote playback, use -1 to reset the stream type evaluation
        mAudioManager->ForceVolumeControlStream(stream);
        mDialog->Show();
        if (mCallback != NULL) {
            mCallback->OnVisible(TRUE);
        }
        AnnounceDialogShown();
    }

    // Do a little vibrate if applicable (only when going into vibrate mode)
    Boolean isStreamAffectedByRingerMode;
    mAudioManager->IsStreamAffectedByRingerMode(streamType, &isStreamAffectedByRingerMode);
    Int32 rmode;
    mAudioManager->GetRingerMode(&rmode);
    if ((streamType != STREAM_REMOTE_MUSIC) &&
        ((flags & IAudioManager::FLAG_VIBRATE) != 0) && isStreamAffectedByRingerMode
        && rmode == IAudioManager::RINGER_MODE_VIBRATE) {
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_VIBRATE, (IMessage**)&msg);
        Boolean sendDelayed;
        SendMessageDelayed(msg, VIBRATE_DELAY, &sendDelayed);
    }

    // Pulse the slider icon if an adjustment was suppressed due to silent mode.
    if ((flags & IAudioManager::FLAG_SHOW_SILENT_HINT) != 0) {
        ShowSilentHint();
    }
    return NOERROR;
}

void VolumePanel::AnnounceDialogShown()
{
    mView->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

Boolean VolumePanel::IsShowing()
{
    Boolean b;
    mDialog->IsShowing(&b);
    return b;
}

ECode VolumePanel::OnPlaySound(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    Boolean b;
    HasMessages(MSG_STOP_SOUNDS, &b);
    if (b) {
        RemoveMessages(MSG_STOP_SOUNDS);
        // Force stop right now
        OnStopSounds();
    }

    synchronized(this) {
        AutoPtr<IToneGenerator> toneGen = GetOrCreateToneGenerator(streamType);
        if (toneGen != NULL) {
            Boolean start;
            toneGen->StartTone(IToneGenerator::TONE_PROP_BEEP, &start);
            AutoPtr<IMessage> msg;
            ObtainMessage(MSG_STOP_SOUNDS, (IMessage**)&msg);
            Boolean send;
            SendMessageDelayed(msg, BEEP_DURATION, &send);
        }
    }
    return NOERROR;
}

ECode VolumePanel::OnStopSounds()
{
    synchronized (this) {
        AutoPtr<IAudioSystem> as;
        CAudioSystem::AcquireSingleton((IAudioSystem**)&as);
        Int32 numStreamTypes;
        as->GetNumStreamTypes(&numStreamTypes);
        for (Int32 i = numStreamTypes - 1; i >= 0; i--) {
            AutoPtr<IToneGenerator> toneGen = (*mToneGenerators)[i];
            if (toneGen != NULL) {
                toneGen->StopTone();
            }
        }
    }
    return NOERROR;
}

ECode VolumePanel::OnVibrate()
{
    // Make sure we ended up in vibrate ringer mode
    Int32 rm;
    mAudioManager->GetRingerMode(&rm);
    if (rm != IAudioManager::RINGER_MODE_VIBRATE) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mVibrator->Vibrate(VIBRATE_DURATION, VIBRATION_ATTRIBUTES);
    return NOERROR;
}

ECode VolumePanel::OnRemoteVolumeChanged(
    /* [in] */ IMediaController* controller,
    /* [in] */ Int32 flags)
{
    if (LOGD) Logger::D(mTag, "onRemoteVolumeChanged(controller:%s, flags: %d)", TO_CSTR(controller),flags);

    if (((flags & IAudioManager::FLAG_SHOW_UI) != 0) || IsShowing()) {
        synchronized(this) {
            if (mActiveStreamType != STREAM_REMOTE_MUSIC) {
                ReorderSliders(STREAM_REMOTE_MUSIC);
            }
            OnShowVolumeChanged(STREAM_REMOTE_MUSIC, flags, controller);
        }
    }
    else {
        if (LOGD) Logger::D(mTag, "not calling onShowVolumeChanged(), no FLAG_SHOW_UI or no UI");
    }

    RemoveMessages(MSG_FREE_RESOURCES);
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_FREE_RESOURCES, (IMessage**)&msg);
    Boolean send;
    SendMessageDelayed(msg, FREE_DELAY, &send);
    ResetTimeout();
    return NOERROR;
}

ECode VolumePanel::OnRemoteVolumeUpdateIfShown()
{
    if (LOGD) Logger::D(mTag, "onRemoteVolumeUpdateIfShown()");
    if (IsShowing()
            && (mActiveStreamType == STREAM_REMOTE_MUSIC)
            && (mStreamControls != NULL)) {
        OnShowVolumeChanged(STREAM_REMOTE_MUSIC, 0, NULL);
    }
    return NOERROR;
}

void VolumePanel::ClearRemoteStreamController()
{
    if (mStreamControls != NULL) {
        AutoPtr<IInterface> vpsc;
        mStreamControls->Get(STREAM_REMOTE_MUSIC, (IInterface**)&vpsc);
        AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));
        if (sc != NULL) {
            if (sc->mController != NULL) {
                sc->mController->UnregisterCallback(mMediaControllerCb);
                sc->mController = NULL;
            }
        }
    }
}

/*synchronized*/
ECode VolumePanel::OnSliderVisibilityChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 visible)
{
    synchronized(this) {
        if (LOGD) Logger::D(mTag, "onSliderVisibilityChanged(stream=%d, visi=%d)",streamType,visible);
        Boolean isVisible = (visible == 1);
        for (Int32 i = STREAMS->GetLength() - 1 ; i >= 0 ; i--) {
            AutoPtr<StreamResources> streamRes = (*STREAMS)[i];
            if (streamRes->mStreamType == streamType) {
                streamRes->mShow = isVisible;
                if (!isVisible && (mActiveStreamType == streamType)) {
                    mActiveStreamType = -1;
                }
                break;
            }
        }
    }
    return NOERROR;
}

ECode VolumePanel::OnDisplaySafeVolumeWarning(
    /* [in] */ Int32 flags)
{
    if ((flags & (IAudioManager::FLAG_SHOW_UI
        | IAudioManager::FLAG_SHOW_UI_WARNINGS)) != 0 || IsShowing()) {
        synchronized(sSafetyWarningLock) {
            if (sSafetyWarning != NULL) {
                return E_NULL_POINTER_EXCEPTION;
            }
            sSafetyWarning = new SafetyWarning(mContext, this, mAudioManager);
            IDialog::Probe(sSafetyWarning)->Show();
        }
        UpdateStates();
    }
    Boolean b;
    mAccessibilityManager->IsTouchExplorationEnabled(&b);
    if (b) {
        RemoveMessages(MSG_TIMEOUT);
    }
    else {
        UpdateTimeoutDelay();
        ResetTimeout();
    }
    return NOERROR;
}

AutoPtr<IToneGenerator> VolumePanel::GetOrCreateToneGenerator(
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
    synchronized(this) {
        if ((*mToneGenerators)[streamType] == NULL) {
            ECode ec = CToneGenerator::New(streamType, MAX_VOLUME, (IToneGenerator**)(&(*mToneGenerators)[streamType]));
            if (FAILED(ec)) {
                if (LOGD) {
                    Logger::D(mTag, "ToneGenerator constructor failed with RuntimeException: %08x", ec);
                }
                // return E_RUNTIME_EXCEPTION;
            }
        }
        return (*mToneGenerators)[streamType];
    }
    return NULL;
}

void VolumePanel::SetMusicIcon(
    /* [in] */ Int32 resId,
    /* [in] */ Int32 resMuteId)
{
    AutoPtr<IInterface> vpsc;
    mStreamControls->Get(IAudioManager::STREAM_MUSIC, (IInterface**)&vpsc);
    AutoPtr<StreamControl> sc = (StreamControl*)(IVolumePanelStreamControl::Probe(vpsc));
    if (sc != NULL) {
        sc->mIconRes = resId;
        sc->mIconMuteRes = resMuteId;
        Boolean b;
        IsMuted(sc->mStreamType, &b);
        UpdateSliderIcon(sc, b);
    }
}

ECode VolumePanel::OnFreeResources()
{
    synchronized(this) {
        for (Int32 i = mToneGenerators->GetLength() - 1; i >= 0; i--) {
            if ((*mToneGenerators)[i] != NULL) {
                // (*mToneGenerators)[i].release();
            }
            (*mToneGenerators)[i] = NULL;
        }
    }
    return NOERROR;
}

ECode VolumePanel::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    Int32 arg1,arg2;
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
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
            if (IsShowing()) {
                IDialogInterface::Probe(mDialog)->Dismiss();
                ClearRemoteStreamController();
                mActiveStreamType = -1;
                if (mCallback != NULL) {
                    mCallback->OnVisible(FALSE);
                }
            }
            synchronized(sSafetyWarningLock) {
                if (sSafetyWarning != NULL) {
                    if (LOGD) Logger::D(mTag, "SafetyWarning timeout");
                    IDialogInterface::Probe(sSafetyWarning)->Dismiss();
                }
            }
            break;
        }

        case MSG_RINGER_MODE_CHANGED:
        case MSG_NOTIFICATION_EFFECTS_SUPPRESSOR_CHANGED: {
            if (IsShowing()) {
                UpdateStates();
            }
            break;
        }

        case MSG_REMOTE_VOLUME_CHANGED: {
            OnRemoteVolumeChanged(IMediaController::Probe(obj), arg1);
            break;
        }

        case MSG_REMOTE_VOLUME_UPDATE_IF_SHOWN:
            OnRemoteVolumeUpdateIfShown();
            break;

        case MSG_SLIDER_VISIBILITY_CHANGED:
            OnSliderVisibilityChanged(arg1, arg2);
            break;

        case MSG_DISPLAY_SAFE_VOLUME_WARNING:
            OnDisplaySafeVolumeWarning(arg1);
            break;

        case MSG_LAYOUT_DIRECTION:
            SetLayoutDirection(arg1);
            break;

        case MSG_ZEN_MODE_AVAILABLE_CHANGED:
            mZenModeAvailable = arg1 != 0;
            UpdateZenPanelVisible();
            break;

        case MSG_USER_ACTIVITY:
            if (mCallback != NULL) {
                mCallback->OnInteraction();
            }
            break;
    }
    return NOERROR;
}

void VolumePanel::ResetTimeout()
{
    Boolean touchExploration;
    mAccessibilityManager->IsTouchExplorationEnabled(&touchExploration);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 tm;
    sys->GetCurrentTimeMillis(&tm);
    if (LOGD) Logger::D(mTag, "resetTimeout at %lld delay=%d touchExploration=%s",
        tm, mTimeoutDelay, touchExploration ? "TRUE" : "FALSE");
    if (sSafetyWarning == NULL || !touchExploration) {
        RemoveMessages(MSG_TIMEOUT);
        Boolean b;
        SendEmptyMessageDelayed(MSG_TIMEOUT, mTimeoutDelay, &b);
        RemoveMessages(MSG_USER_ACTIVITY);
        SendEmptyMessage(MSG_USER_ACTIVITY, &b);
    }
}

void VolumePanel::ForceTimeout(
    /* [in] */ Int64 delay)
{
    if (LOGD) Logger::D(mTag, "forceTimeout delay=%ld callers=%d",delay, 3/*Debug.getCallers(3)*/);
    RemoveMessages(MSG_TIMEOUT);
    Boolean b;
    SendEmptyMessageDelayed(MSG_TIMEOUT, delay, &b);
}

ECode VolumePanel::GetZenController(
    /* [out] */ IZenModeController** zc)
{
    VALIDATE_NOT_NULL(zc)
    *zc = mZenController;
    REFCOUNT_ADD(*zc)
    return NOERROR;
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos