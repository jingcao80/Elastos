//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/policy/impl/GlobalActions.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CBitmapShader.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/internal/app/CAlertController.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowManager.h"
#include "elastos/droid/internal/utility/UserIcons.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CWindowManagerGlobalHelper.h"
#include "elastos/droid/internal/widget/CLockPatternUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include "elastos/utility/etl/HashSet.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IBitmapShader;
using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::MatrixScaleToFit_CENTER;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Internal::Utility::UserIcons;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::App::CAlertController;
using Elastos::Droid::Internal::App::CAlertControllerAlertParams;
using Elastos::Droid::Internal::Policy::Impl::CPhoneWindowManager;
using Elastos::Droid::Internal::Utility::Cm::IPowerMenuConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IUUID;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

const String GlobalActions::TAG("GlobalActions");

const Boolean GlobalActions::SHOW_SILENT_TOGGLE = TRUE;

const Int32 GlobalActions::MESSAGE_DISMISS = 0;
const Int32 GlobalActions::MESSAGE_REFRESH = 1;
const Int32 GlobalActions::MESSAGE_SHOW = 2;
const Int32 GlobalActions::DIALOG_DISMISS_DELAY = 300; // ms

GlobalActions::MyAdapter::MyAdapter(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::MyAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 temp = 0;
    List< AutoPtr<Action> >::Iterator it = mHost->mItems->Begin();
    for (; it != mHost->mItems->End(); ++it) {
        AutoPtr<Action> action = *it;
        if (mHost->mKeyguardShowing && !action->ShowDuringKeyguard()) {
            continue;
        }
        if (!mHost->mDeviceProvisioned && !action->ShowBeforeProvisioning()) {
            continue;
        }
        temp++;
    }
    *count = temp;
    return NOERROR;
}

ECode GlobalActions::MyAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    AutoPtr<IInterface> action;
    GetItem(position, (IInterface**)&action);
    Action* a = (Action*)IObject::Probe(action);
    *enabled = a->IsEnabled();
    return NOERROR;
}

ECode GlobalActions::MyAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = FALSE;
    return NOERROR;
}

ECode GlobalActions::MyAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    Int32 filteredPos = 0;
    List< AutoPtr<Action> >::Iterator it = mHost->mItems->Begin();
    for (; it != mHost->mItems->End(); ++it) {
        AutoPtr<Action> action = *it;
        if (mHost->mKeyguardShowing && !action->ShowDuringKeyguard()) {
            continue;
        }
        if (!mHost->mDeviceProvisioned && !action->ShowBeforeProvisioning()) {
            continue;
        }
        if (filteredPos == position) {
            *item = (IObject*)action.Get();
            REFCOUNT_ADD(*item);
            return NOERROR;
        }
        filteredPos++;
    }

    Int32 count;
    GetCount(&count);
    Logger::E(TAG, "position: %d  out of range of showable actions, "
        "filtered count=%d, keyguardshowing=%d, provisioned=%d",
        position, count, mHost->mKeyguardShowing, mHost->mDeviceProvisioned);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode GlobalActions::MyAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

ECode GlobalActions::MyAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    Action* action = (Action*)IObject::Probe(obj);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> viewTemp = action->Create(
        mHost->GetUiContext(), convertView, parent, inflater);
    *view = viewTemp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

//==========================================================================
// GlobalActions::SinglePressAction
//==========================================================================
CAR_INTERFACE_IMPL(GlobalActions::SinglePressAction, Action, IGlobalActionsSinglePressAction);

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* host,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 messageResId)
    : mHost(host)
    , mIconResId(iconResId)
    , mMessageResId(messageResId)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* host,
    /* [in] */ Int32 iconResId,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* message)
    : mHost(host)
    , mIconResId(iconResId)
    , mIcon(icon)
    , mMessageResId(0)
    , mMessage(message)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* host,
    /* [in] */ Int32 iconResId,
    /* [in] */ ICharSequence* message)
    : mHost(host)
    , mIconResId(iconResId)
    , mMessageResId(0)
    , mMessage(message)
{
}

Boolean GlobalActions::SinglePressAction::IsEnabled()
{
    return TRUE;
}

AutoPtr<ICharSequence> GlobalActions::SinglePressAction::GetStatus()
{
    return mStatusMessage;
}

ECode GlobalActions::SinglePressAction::SetStatus(
    /* [in] */ ICharSequence* status)
{
    mStatusMessage= status;
    return NOERROR;
}

AutoPtr<ICharSequence> GlobalActions::SinglePressAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    if (mMessage != NULL) {
        return mMessage;
    }

    String result;
    mHost->mContext->GetString(mMessageResId, &result);
    return CoreUtils::Convert(result);
}

AutoPtr<IView> GlobalActions::SinglePressAction::Create(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ ILayoutInflater* inflater)
{
    AutoPtr<IView> v;
    inflater->Inflate(R::layout::global_actions_item, parent, FALSE, (IView**)&v);

    AutoPtr<IView> viewTemp;
    v->FindViewById(R::id::icon, (IView**)&viewTemp);
    AutoPtr<IImageView> icon = IImageView::Probe(viewTemp);

    viewTemp = NULL;
    v->FindViewById(R::id::message, (IView**)&viewTemp);
    AutoPtr<ITextView> messageView = ITextView::Probe(viewTemp);

    viewTemp = NULL;
    v->FindViewById(R::id::status, (IView**)&viewTemp);
    AutoPtr<ICharSequence> status = GetStatus();
    if (!TextUtils::IsEmpty(status)) {
        AutoPtr<ITextView> statusView = ITextView::Probe(viewTemp);
        statusView->SetText(status);
    }
    else {
        viewTemp->SetVisibility(IView::GONE);
    }

    if (mIcon != NULL) {
        icon->SetImageDrawable(mIcon);
        icon->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_CENTER_CROP);
    }
    else if (mIconResId != 0) {
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(mIconResId, (IDrawable**)&drawable);
        icon->SetImageDrawable(drawable);
    }

    if (mMessage != NULL) {
        messageView->SetText(mMessage);
    }
    else {
        messageView->SetText(mMessageResId);
    }

    return v;
}

//======================================================================
// GlobalActions::PowerAction
//======================================================================
CAR_INTERFACE_IMPL(GlobalActions::PowerAction, SinglePressAction, IGlobalActionsLongPressAction);

GlobalActions::PowerAction::PowerAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(
        host,
        R::drawable::ic_lock_power_off, R::string::global_action_power_off)
{
}

Boolean GlobalActions::PowerAction::OnLongPress()
{
    Logger::I(TAG, " >> PowerAction:: OnLongPress");
    mHost->mWindowManagerFuncs->RebootSafeMode(TRUE);
    return TRUE;
}

Boolean GlobalActions::PowerAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::PowerAction::ShowBeforeProvisioning()
{
    return TRUE;
}

ECode GlobalActions::PowerAction::OnPress()
{
    Logger::I(TAG, " >> PowerAction:: OnPress");
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 ival;
    Settings::System::GetInt32(cr, String("enable_quickboot"), 0, &ival);
    Boolean quickbootEnabled = ival == 1;

    // go to quickboot mode if enabled
    if (quickbootEnabled) {
        mHost->StartQuickBoot();
        return NOERROR;
    }

    // shutdown by making sure radio and power are handled accordingly.
    return mHost->mWindowManagerFuncs->Shutdown(false /* confirm */);
}

//======================================================================
// GlobalActions::RebootAction
//======================================================================
GlobalActions::RebootAction::RebootAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(
        host,
        R::drawable::ic_lock_power_reboot, R::string::global_action_reboot)
{
}

Boolean GlobalActions::RebootAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::RebootAction::ShowBeforeProvisioning()
{
    return TRUE;
}

ECode GlobalActions::RebootAction::OnPress()
{
    Logger::I(TAG, " >> RebootAction:: OnPress");

    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::POWER_SERVICE);
    AutoPtr<IIPowerManager> pm = IIPowerManager::Probe(obj);
    if (pm) {
        ECode ec = pm->Reboot(TRUE, String(NULL), FALSE);
        if (FAILED(ec)) {
            Logger::E(TAG, "PowerManager service died!");
            return NOERROR;
        }
    }

    return NOERROR;
}


//======================================================================
// GlobalActions::ToggleAction::State
//======================================================================

GlobalActions::ToggleAction::State::State(
    /* [in] */ Boolean intermediate)
    : mInTransition(intermediate)
{
}

Boolean GlobalActions::ToggleAction::State::InTransition()
{
    return mInTransition;
}

//======================================================================
// GlobalActions::ToggleAction
//======================================================================
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::Off
    = new GlobalActions::ToggleAction::State(FALSE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::TurningOn
    = new GlobalActions::ToggleAction::State(TRUE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::TurningOff
    = new GlobalActions::ToggleAction::State(TRUE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::On
    = new GlobalActions::ToggleAction::State(FALSE);

GlobalActions::ToggleAction::ToggleAction(
    /* [in] */ GlobalActions* host,
    /* [in] */ Int32 enabledIconResId,
    /* [in] */ Int32 disabledIconResid,
    /* [in] */ Int32 message,
    /* [in] */ Int32 enabledStatusMessageResId,
    /* [in] */ Int32 disabledStatusMessageResId)
    : mState(Off)
    , mEnabledIconResId(enabledIconResId)
    , mDisabledIconResid(disabledIconResid)
    , mMessageResId(message)
    , mEnabledStatusMessageResId(enabledStatusMessageResId)
    , mDisabledStatusMessageResId(disabledStatusMessageResId)
    , mHost(host)
{
}

void GlobalActions::ToggleAction::WillCreate()
{
}

AutoPtr<ICharSequence> GlobalActions::ToggleAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    String result;
    mHost->mContext->GetString(mMessageResId, &result);
    return CoreUtils::Convert(result);
}

AutoPtr<IView> GlobalActions::ToggleAction::Create(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ ILayoutInflater* inflater)
{
    WillCreate();

    AutoPtr<IView> v;
    inflater->Inflate(R::layout::global_actions_item, parent, FALSE, (IView**)&v);

    AutoPtr<IView> viewTemp;
    v->FindViewById(R::id::icon, (IView**)&viewTemp);
    AutoPtr<IImageView> icon = IImageView::Probe(viewTemp);

    viewTemp = NULL;
    v->FindViewById(R::id::message, (IView**)&viewTemp);
    AutoPtr<ITextView> messageView = ITextView::Probe(viewTemp);

    viewTemp = NULL;
    v->FindViewById(R::id::status, (IView**)&viewTemp);
    AutoPtr<ITextView> statusView = ITextView::Probe(viewTemp);
    const Boolean enabled = IsEnabled();

    if (messageView != NULL) {
        messageView->SetText(mMessageResId);
        IView* view = IView::Probe(messageView);
        view->SetEnabled(enabled);
    }

    Boolean on = ((mState == ToggleAction::On) || (mState == ToggleAction::TurningOn));
    if (icon != NULL) {
        AutoPtr<IDrawable> drawable;
        context->GetDrawable((on ? mEnabledIconResId : mDisabledIconResid), (IDrawable**)&drawable);
        icon->SetImageDrawable(drawable);
        IView* view = IView::Probe(icon);
        view->SetEnabled(enabled);
    }

    if (statusView != NULL) {
        IView* view = IView::Probe(statusView);
        statusView->SetText(on ? mEnabledStatusMessageResId : mDisabledStatusMessageResId);
        view->SetVisibility(IView::VISIBLE);
        view->SetEnabled(enabled);
    }
    v->SetEnabled(enabled);

    return v;
}

ECode GlobalActions::ToggleAction::OnPress()
{
    if (mState->InTransition()) {
        Logger::W(TAG, "shouldn't be able to toggle when in transition");
        return NOERROR;
    }

    const Boolean nowOn = !(mState == ToggleAction::On);
    OnToggle(nowOn);
    ChangeStateFromPress(nowOn);
    return NOERROR;
}

Boolean GlobalActions::ToggleAction::IsEnabled()
{
    return !mState->InTransition();
}

void GlobalActions::ToggleAction::ChangeStateFromPress(
    /* [in] */ Boolean buttonOn)
{
    mState = buttonOn ? ToggleAction::On : ToggleAction::Off;
}

void GlobalActions::ToggleAction::UpdateState(
    /* [in] */ State* state)
{
    mState = state;
}

//======================================================================
// GlobalActions::SilentModeToggleAction
//======================================================================
GlobalActions::SilentModeToggleAction::SilentModeToggleAction(
    /* [in] */ GlobalActions* host)
    : ToggleAction(
            host,
            R::drawable::ic_audio_vol_mute,
            R::drawable::ic_audio_vol,
            R::string::global_action_toggle_silent_mode,
            R::string::global_action_silent_mode_on_status,
            R::string::global_action_silent_mode_off_status)
{
}

ECode GlobalActions::SilentModeToggleAction::OnToggle(
    /* [in] */ Boolean on)
{
    if (on) {
        mHost->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_SILENT);
    }
    else {
        mHost->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_NORMAL);
    }
    return NOERROR;
}

Boolean GlobalActions::SilentModeToggleAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::SilentModeToggleAction::ShowBeforeProvisioning()
{
    return FALSE;
}

//======================================================================
// GlobalActions::SilentModeTriStateAction
//======================================================================
CAR_INTERFACE_IMPL_2(GlobalActions::SilentModeTriStateAction, GlobalActions::Action, \
    IViewOnClickListener, IGlobalActionsSilentModeTriStateAction)

GlobalActions::SilentModeTriStateAction::SilentModeTriStateAction(
    /* [in] */ IContext* context,
    /* [in] */ IAudioManager* audioManager,
    /* [in] */ IHandler* handler)
    : mAudioManager(audioManager)
    , mHandler(handler)
    , mContext(context)
{
    // Init ITEM_IDS.
    ITEM_IDS = ArrayOf<Int32>::Alloc(3);
    (*ITEM_IDS)[0] = R::id::option1;
    (*ITEM_IDS)[1] = R::id::option2;
    (*ITEM_IDS)[2] = R::id::option3;
}

Int32 GlobalActions::SilentModeTriStateAction::RingerModeToIndex(
    /* [in] */ Int32 ringerMode)
{
    // They just happen to coincide
    return ringerMode;
}

Int32 GlobalActions::SilentModeTriStateAction::IndexToRingerMode(
    /* [in] */ Int32 index)
{
    // They just happen to coincide
    return index;
}

AutoPtr<ICharSequence> GlobalActions::SilentModeTriStateAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    return NULL;
}

AutoPtr<IView> GlobalActions::SilentModeTriStateAction::Create(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ ILayoutInflater* inflater)
{
    AutoPtr<IView> v;
    inflater->Inflate(R::layout::global_actions_silent_mode, parent, FALSE, (IView**)&v);

    Int32 ringerMode = 0;
    mAudioManager->GetRingerMode(&ringerMode);
    Int32 selectedIndex = RingerModeToIndex(ringerMode);
    for (Int32 i = 0; i < 3; i++) {
        AutoPtr<IView> itemView;
        v->FindViewById((*ITEM_IDS)[i], (IView**)&itemView);
        itemView->SetSelected(selectedIndex == i);
        // Set up click handler
        itemView->SetTag(CoreUtils::Convert(i));
        itemView->SetOnClickListener(this);
    }
    return v;
}

ECode GlobalActions::SilentModeTriStateAction::OnPress()
{
    return NOERROR;
}

Boolean GlobalActions::SilentModeTriStateAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::SilentModeTriStateAction::ShowBeforeProvisioning()
{
    return FALSE;
}

Boolean GlobalActions::SilentModeTriStateAction::IsEnabled()
{
    return TRUE;
}

void GlobalActions::SilentModeTriStateAction::WillCreate()
{
}

ECode GlobalActions::SilentModeTriStateAction::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    AutoPtr<IInteger32> integer = IInteger32::Probe(tag);
    if (integer == NULL) {
        return NOERROR;
    }

    Int32 index = 0;
    integer->GetValue(&index);
    mAudioManager->SetRingerMode(IndexToRingerMode(index));
    Boolean isSuccess = FALSE;
    mHandler->SendEmptyMessageDelayed(MESSAGE_DISMISS, DIALOG_DISMISS_DELAY, &isSuccess);
    return NOERROR;
}

//======================================================================
// GlobalActions::GlobalActionsDialog::EnableAccessibilityControllerRunnable
//======================================================================
GlobalActions::GlobalActionsDialog::EnableAccessibilityControllerRunnable::EnableAccessibilityControllerRunnable(
    /* [in] */ GlobalActionsDialog* host)
    :mHost(host)
{
}

ECode GlobalActions::GlobalActionsDialog::EnableAccessibilityControllerRunnable::Run()
{
    return mHost->Dismiss();
}

//======================================================================
// GlobalActions::GlobalActionsDialog
//======================================================================
GlobalActions::GlobalActionsDialog::GlobalActionsDialog()
    : mWindowTouchSlop(0)
    , mIntercepted(FALSE)
    , mCancelOnUp(FALSE)
{}

ECode GlobalActions::GlobalActionsDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAlertControllerAlertParams* params)
{
    mContext = context;
    Dialog::constructor(context, GetDialogTheme(context));

    AutoPtr<IWindow> w;
    GetWindow((IWindow**)&w);
    CAlertController::New(mContext, this, w, (IAlertController**)&mAlert);
    AutoPtr<IListAdapter> lAdapter;
    params->GetAdapter((IListAdapter**)&lAdapter);
    mAdapter = (MyAdapter*)lAdapter.Get();
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfiguration;
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);
    viewConfiguration->GetScaledWindowTouchSlop(&mWindowTouchSlop);
    params->Apply(mAlert);

    // if (false && context.getResources().getBoolean(R.bool.config_ui_blur_enabled)) {
    //     getWindow().addFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND);
    //     getWindow().clearFlags(WindowManager.LayoutParams.FLAG_DIM_BEHIND);
    // }
    return NOERROR;
}

Int32 GlobalActions::GlobalActionsDialog::GetDialogTheme(
    /* [in] */ IContext* context)
{
    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);

    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    Boolean isFound = FALSE;
    theme->ResolveAttribute(R::attr::alertDialogTheme,
            outValue, TRUE, &isFound);
    Int32 resourceId = 0;
    outValue->GetResourceId(&resourceId);
    return resourceId;
}

ECode GlobalActions::GlobalActionsDialog::OnStart()
{
    // If global accessibility gesture can be performed, we will take care
    // of dismissing the dialog on touch outside. This is because the dialog
    // is dismissed on the first down while the global gesture is a long press
    // with two fingers anywhere on the screen.

    if (EnableAccessibilityController::CanEnableAccessibilityViaGesture(mContext)) {
        mEnableAccessibilityController = new EnableAccessibilityController();
        AutoPtr<IRunnable> eacRun = new GlobalActionsDialog::EnableAccessibilityControllerRunnable(this);
        mEnableAccessibilityController->constructor(mContext, eacRun);
        Dialog::SetCanceledOnTouchOutside(FALSE);
    }
    else {
        mEnableAccessibilityController = NULL;
        Dialog::SetCanceledOnTouchOutside(TRUE);
    }
    return Dialog::OnStart();
}

ECode GlobalActions::GlobalActionsDialog::OnStop()
{
    if (mEnableAccessibilityController != NULL) {
        mEnableAccessibilityController->OnDestroy();
    }
    return Dialog::OnStop();
}

ECode GlobalActions::GlobalActionsDialog::DispatchTouchEvent(
    /* [in] */ IMotionEvent* inEvent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IMotionEvent> event = inEvent;
    if (mEnableAccessibilityController != NULL) {
        Int32 action = 0;
        event->GetActionMasked(&action);
        if (action == IMotionEvent::ACTION_DOWN) {
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            AutoPtr<IView> decor;
            window->GetDecorView((IView**)&decor);
            Float x = 0.0f, y = 0.0f;
            event->GetX(&x);
            event->GetY(&y);
            Int32 eventX = (Int32)x;
            Int32 eventY = (Int32)y;
            Int32 width = 0, height = 0;
            decor->GetWidth(&width);
            decor->GetHeight(&height);
            if (eventX < -mWindowTouchSlop
                    || eventY < -mWindowTouchSlop
                    || eventX >= width + mWindowTouchSlop
                    || eventY >= height + mWindowTouchSlop) {
                mCancelOnUp = TRUE;
            }
        }

        if (!mIntercepted) {
            mEnableAccessibilityController->OnInterceptTouchEvent(event, &mIntercepted);
            if (mIntercepted) {
                Int64 now = SystemClock::GetUptimeMillis();
                AutoPtr<IMotionEventHelper> helper;
                CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
                event = NULL;
                helper->Obtain(now, now,
                    IMotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0, (IMotionEvent**)&event);
                IInputEvent* inputE = IInputEvent::Probe(event);
                inputE->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
                mCancelOnUp = TRUE;
            }

            if (action == IMotionEvent::ACTION_UP) {
                if (mCancelOnUp) {
                    Dialog::Cancel();
                }
                mCancelOnUp = FALSE;
                mIntercepted = FALSE;
            }
        }
        else {
            mEnableAccessibilityController->OnTouchEvent(event, result);

            if (action == IMotionEvent::ACTION_UP) {
                if (mCancelOnUp) {
                    Dialog::Cancel();
                }
                mCancelOnUp = FALSE;
                mIntercepted = FALSE;
            }
            return NOERROR;
        }
    }

    return Dialog::DispatchTouchEvent(event, result);
}

AutoPtr<IListView> GlobalActions::GlobalActionsDialog::GetListView()
{
    AutoPtr<IListView> listView;
    mAlert->GetListView((IListView**)&listView);
    return listView;
}

ECode GlobalActions::GlobalActionsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);
    mAlert->InstallContent();
    return NOERROR;
}

// @Override
ECode GlobalActions::GlobalActionsDialog::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 eventType;
    event->GetEventType(&eventType);
    if (eventType == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        Int32 count;
        mAdapter->GetCount(&count);
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IInterface> item;
            mAdapter->GetItem(i, (IInterface**)&item);
            AutoPtr<Action> action = (Action*)(IObject::Probe(item));
            AutoPtr<ICharSequence> label = action->GetLabelForAccessibility(ctx);
            if (label != NULL)  {
                AutoPtr<IList> list;
                IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
                list->Add(label);
            }
        }
    }
    return Dialog::DispatchPopulateAccessibilityEvent(event, result);
}

ECode GlobalActions::GlobalActionsDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isOnKeyDown = FALSE;
    if (mAlert->OnKeyDown(keyCode, event, &isOnKeyDown), isOnKeyDown) {
        *result = TRUE;
        return NOERROR;
    }
    return Dialog::OnKeyDown(keyCode, event, result);
}

ECode GlobalActions::GlobalActionsDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isOnKeyUp = FALSE;
    if (mAlert->OnKeyUp(keyCode, event, &isOnKeyUp), isOnKeyUp) {
        *result = TRUE;
        return NOERROR;
    }
    return Dialog::OnKeyUp(keyCode, event, result);
}

//======================================================================
// GlobalActions::MyBroadcastReceiver
//======================================================================
GlobalActions::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)
            || IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        String reason;
        intent->GetStringExtra(CPhoneWindowManager::SYSTEM_DIALOG_REASON_KEY, &reason);
        if (!CPhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS.Equals(reason)) {
            Boolean isSuccess = FALSE;
            mHost->mHandler->SendEmptyMessage(MESSAGE_DISMISS, &isSuccess);
        }
    }
    else if (ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED.Equals(action)) {
        // Airplane mode can be changed after ECM exits if airplane toggle button
        // is pressed during ECM mode
        Boolean bTemp = FALSE;
        intent->GetBooleanExtra(String("PHONE_IN_ECM_STATE"), FALSE, &bTemp);
        if (!bTemp && mHost->mIsWaitingForEcmExit) {
            mHost->mIsWaitingForEcmExit = FALSE;
            mHost->ChangeAirplaneModeSystemSetting(TRUE);
        }
    }
    else if (IIntent::UPDATE_POWER_MENU.Equals(action)) {
        mHost->UpdatePowerMenuActions();
    }
    return NOERROR;
}

//======================================================================
// GlobalActions::ThemeChangeReceiver
//======================================================================
GlobalActions::ThemeChangeReceiver::ThemeChangeReceiver(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::ThemeChangeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mUiContext = NULL;
    return NOERROR;
}

//======================================================================
// GlobalActions::RingerModeReceiver
//======================================================================
GlobalActions::RingerModeReceiver::RingerModeReceiver(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::RingerModeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IAudioManager::RINGER_MODE_CHANGED_ACTION.Equals(action)) {
        Boolean isSuccess = FALSE;
        mHost->mHandler->SendEmptyMessage(MESSAGE_REFRESH, &isSuccess);
    }
    return NOERROR;
}

//======================================================================
// GlobalActions::AirplaneModeObserver
//======================================================================
GlobalActions::AirplaneModeObserver::AirplaneModeObserver(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

GlobalActions::AirplaneModeObserver::constructor(
    /* [in] */ IHandler* handler)
{
    return ContentObserver::constructor(handler);
}

ECode GlobalActions::AirplaneModeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->OnAirplaneModeChanged();
    return NOERROR;
}

//======================================================================
// GlobalActions::DialogHandler
//======================================================================
GlobalActions::DialogHandler::DialogHandler(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::DialogHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
    case GlobalActions::MESSAGE_DISMISS:
        if (mHost->mDialog != NULL) {
            mHost->mDialog->Dismiss();
            mHost->mDialog = NULL;
        }
        break;
    case GlobalActions::MESSAGE_REFRESH:
        mHost->RefreshSilentMode();
        mHost->mAdapter->NotifyDataSetChanged();
        break;
    case GlobalActions::MESSAGE_SHOW:
        mHost->HandleShow();
        break;
    }
    return NOERROR;
}

//====================================================================
// GlobalActions::AirplaneModeOnAction
//====================================================================
GlobalActions::AirplaneModeOnAction::AirplaneModeOnAction(
    /* [in] */ GlobalActions* host)
    : ToggleAction(
            host,
            R::drawable::ic_lock_airplane_mode,
            R::drawable::ic_lock_airplane_mode_off,
            R::string::global_actions_toggle_airplane_mode,
            R::string::global_actions_airplane_mode_on_status,
            R::string::global_actions_airplane_mode_off_status)
    , mHost(host)
{
}

ECode GlobalActions::AirplaneModeOnAction::OnToggle(
    /* [in] */ Boolean on)
{
    String value;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, &value);
    Boolean bValue = StringUtils::ParseBoolean(value);
    if (mHost->mHasTelephony && bValue) {
        mHost->mIsWaitingForEcmExit = TRUE;
        // Launch ECM exit dialog
        AutoPtr<IIntent> ecmDialogIntent;
        CIntent::New(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, NULL, (IIntent**)&ecmDialogIntent);
        ecmDialogIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mHost->mContext->StartActivity(ecmDialogIntent);
    } else {
        mHost->ChangeAirplaneModeSystemSetting(on);
    }
    return NOERROR;
}

Boolean GlobalActions::AirplaneModeOnAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::AirplaneModeOnAction::ShowBeforeProvisioning()
{
    return FALSE;
}

void GlobalActions::AirplaneModeOnAction::ChangeStateFromPress(
    /* [in] */ Boolean buttonOn)
{
    if (!mHost->mHasTelephony) {
        return;
    }

    // In ECM mode airplane state cannot be changed
    String value;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, &value);
    Boolean bValue = StringUtils::ParseBoolean(value);
    if (!(bValue)) {
        mState = buttonOn ? ToggleAction::TurningOn : ToggleAction::TurningOff;
        mHost->mAirplaneState = mState;
    }
}

//====================================================================
// GlobalActions::ProfileChooseAction
//====================================================================

GlobalActions::ProfileChooseAction::ProfileChooseAction(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
    AutoPtr<IInterface> obj;
    mHost->mContext->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&obj);
    mProfileManager = IProfileManager::Probe(obj);
}

Boolean GlobalActions::ProfileChooseAction::IsEnabled()
{
    return TRUE;
}

AutoPtr<IView> GlobalActions::ProfileChooseAction::Create(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [in] */ ILayoutInflater* inflater)
{
    AutoPtr<IView> v;
    inflater->Inflate(R::layout::global_actions_item, parent, FALSE, (IView**)&v);

    AutoPtr<IView> tmp;
    v->FindViewById(R::id::icon, (IView**)&tmp);
    AutoPtr<IImageView> icon = IImageView::Probe(tmp);

    tmp = NULL;
    v->FindViewById(R::id::message, (IView**)&tmp);
    AutoPtr<ITextView> messageView = ITextView::Probe(tmp);


    tmp = NULL;
    v->FindViewById(R::id::status, (IView**)&tmp);
    AutoPtr<ITextView> statusView = ITextView::Probe(tmp);

    if (statusView != NULL) {
        IView::Probe(statusView)->SetVisibility(IView::VISIBLE);
        AutoPtr<IProfile> profile;
        mProfileManager->GetActiveProfile((IProfile**)&profile);
        String name;
        profile->GetName(&name);
        statusView->SetText(CoreUtils::Convert(name));
    }

    if (icon != NULL) {
        AutoPtr<IDrawable> drawable;
        mHost->mContext->GetDrawable(R::drawable::ic_lock_profile, (IDrawable**)&drawable);
        icon->SetImageDrawable(drawable);
    }

    if (messageView != NULL) {
        messageView->SetText(R::string::global_action_choose_profile);
    }

    return v;
}

//====================================================================
// GlobalActions::ProfileAction
//====================================================================
GlobalActions::ProfileAction::ProfileAction(
    /* [in] */ GlobalActions* host)
    : ProfileChooseAction(host)
{}

ECode GlobalActions::ProfileAction::OnPress()
{
    mHost->CreateProfileDialog();
    return NOERROR;
}

Boolean GlobalActions::ProfileAction::OnLongPress()
{
    return TRUE;
}

Boolean GlobalActions::ProfileAction::ShowDuringKeyguard()
{
    return FALSE;
}

Boolean GlobalActions::ProfileAction::ShowBeforeProvisioning()
{
    return FALSE;
}

AutoPtr<ICharSequence> GlobalActions::ProfileAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    return NULL;
}

//==========================================================================
// GlobalActions::BugRunnable
//==========================================================================
ECode GlobalActions::BugRunnable::Run()
{
    return ActivityManagerNative::GetDefault()->RequestBugReport();
}

//==========================================================================
// GlobalActions::BugReportDialogOnClickListener
//==========================================================================
CAR_INTERFACE_IMPL(GlobalActions::BugReportDialogOnClickListener, \
    Object, IDialogInterfaceOnClickListener);

GlobalActions::BugReportDialogOnClickListener::BugReportDialogOnClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::BugReportDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // don't actually trigger the bugreport if we are running stability
    // tests via monkey
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean userAMonkey;
    amHelper->IsUserAMonkey(&userAMonkey);
    if (userAMonkey) {
        return NOERROR;
    }
    // Add a little delay before executing, to give the
    // dialog a chance to go away before it takes a
    // screenshot.
    Boolean isSuccess = FALSE;
    AutoPtr<IRunnable> bugRunnable = new BugRunnable();
    return mHost->mHandler->PostDelayed(bugRunnable, 500, &isSuccess);
}

//==========================================================================
// GlobalActions::BugSinglePressAction
//==========================================================================
GlobalActions::BugSinglePressAction::BugSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(host,
        R::drawable::ic_lock_bugreport, R::string::bugreport_title)
{
}

ECode GlobalActions::BugSinglePressAction::OnPress()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::bugreport_title);
    builder->SetMessage(R::string::bugreport_message);
    builder->SetNegativeButton(R::string::cancel, NULL);
    builder->SetPositiveButton(R::string::report,
            new BugReportDialogOnClickListener(mHost));
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    AutoPtr<IWindow> window;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    return IDialog::Probe(dialog)->Show();
}

Boolean GlobalActions::BugSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::BugSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

AutoPtr<ICharSequence> GlobalActions::BugSinglePressAction::GetStatus()
{
    AutoPtr<ICharSequence> id = CoreUtils::Convert(Build::ID);
    AutoPtr<ICharSequence> release = CoreUtils::Convert(Build::VERSION::RELEASE);
    AutoPtr<ArrayOf<IInterface*> > fArgs = ArrayOf<IInterface*>::Alloc(2);
    fArgs->Set(0, release);
    fArgs->Set(1, id);
    String result;
    mHost->mContext->GetString(R::string::bugreport_status, fArgs, &result);
    return CoreUtils::Convert(result);
}

//==========================================================================
// GlobalActions::SettingsSinglePressAction
//==========================================================================
GlobalActions::SettingsSinglePressAction::SettingsSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(
        host,
        R::drawable::ic_settings, R::string::global_action_settings)
{
}

ECode GlobalActions::SettingsSinglePressAction::OnPress()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_SETTINGS, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    return mHost->mContext->StartActivity(intent);
}

Boolean GlobalActions::SettingsSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::SettingsSinglePressAction::ShowBeforeProvisioning()
{
    return TRUE;
}

//==========================================================================
// GlobalActions::LockdownSinglePressAction
//==========================================================================
GlobalActions::LockdownSinglePressAction::LockdownSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(
        host,
        R::drawable::ic_lock_lock, R::string::global_action_lockdown)
{
}

ECode GlobalActions::LockdownSinglePressAction::OnPress()
{
    AutoPtr<ILockPatternUtils> lpu;
    CLockPatternUtils::New(mHost->mContext, (ILockPatternUtils**)&lpu);
    lpu->RequireCredentialEntry(IUserHandle::USER_ALL);
    //try {
    AutoPtr<IWindowManagerGlobalHelper> wmgh;
    CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&wmgh);
    AutoPtr<IIWindowManager> wm;
    wmgh->GetWindowManagerService((IIWindowManager**)&wm);
    return wm->LockNow(NULL);
}

Boolean GlobalActions::LockdownSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::LockdownSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

//==========================================================================
// GlobalActions::MyAdapterViewOnItemLongClickListener
//==========================================================================
CAR_INTERFACE_IMPL(GlobalActions::MyAdapterViewOnItemLongClickListener, Object, IAdapterViewOnItemClickListener);

GlobalActions::MyAdapterViewOnItemLongClickListener::MyAdapterViewOnItemLongClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::MyAdapterViewOnItemLongClickListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    AutoPtr<IInterface> action;
    mHost->mAdapter->GetItem(position, (IInterface**)&action);
    IGlobalActionsLongPressAction* lpAction = IGlobalActionsLongPressAction::Probe(action);
    if (lpAction != NULL)
    {
        LongPressAction* a = (LongPressAction*)lpAction;
        *result = a->OnLongPress();
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//==========================================================================
// GlobalActions::UserSinglePressAction
//==========================================================================
GlobalActions::UserSinglePressAction::UserSinglePressAction(
    /* [in] */ GlobalActions* host,
    /* [in] */ Int32 iconId,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* message,
    /* [in] */ Int32 userId)
    : SinglePressAction(host, iconId, icon, message)
    , mUserId(userId)
{
}

ECode GlobalActions::UserSinglePressAction::OnPress()
{
    Boolean value = FALSE;
    ECode ec = ActivityManagerNative::GetDefault()->SwitchUser(mUserId, &value);
    if (FAILED(ec)) {
        Logger::E(TAG, "Couldn't switch user %d" + mUserId);
    }
    return NOERROR;
}

Boolean GlobalActions::UserSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::UserSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

//==========================================================================
// GlobalActions::ScreenshotAction
//==========================================================================
GlobalActions::ScreenshotAction::ScreenshotAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(
        host,
        R::drawable::ic_lock_screenshot, R::string::global_action_screenshot)
{
}

ECode GlobalActions::ScreenshotAction::OnPress()
{
    mHost->TakeScreenshot();
    return NOERROR;
}

Boolean GlobalActions::ScreenshotAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::ScreenshotAction::ShowBeforeProvisioning()
{
    return TRUE;
}

//==========================================================================
// GlobalActions::ProfileDialogOnClickListener
//==========================================================================
CAR_INTERFACE_IMPL(GlobalActions::ProfileDialogOnClickListener, Object, IDialogInterfaceOnClickListener)

GlobalActions::ProfileDialogOnClickListener::ProfileDialogOnClickListener(
    /* [in] */ GlobalActions* host,
    /* [in] */ ArrayOf<IProfile*>* profiles)
    : mHost(host)
    , mProfiles(profiles)
{}

ECode GlobalActions::ProfileDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which < 0)
        return NOERROR;
    mHost->mChosenProfile = (*mProfiles)[which];

    AutoPtr<IUUID> uuid;
    mHost->mChosenProfile->GetUuid((IUUID**)&uuid);

    AutoPtr<IInterface> obj;
    mHost->mContext->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&obj);
    AutoPtr<IProfileManager> profileManager = IProfileManager::Probe(obj);
    profileManager->SetActiveProfile(uuid);
    return dialog->Cancel();
}

//==========================================================================
// GlobalActions::ScreenshotTimeoutRunnable
//==========================================================================
GlobalActions::ScreenshotTimeoutRunnable::ScreenshotTimeoutRunnable(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{}

ECode GlobalActions::ScreenshotTimeoutRunnable::Run()
{
    AutoLock syncLock(mHost->mScreenshotLock);

    if (mHost->mScreenshotConnection != NULL) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
    }
    return NOERROR;
}

//==========================================================================
// GlobalActions::TakeScreenshotHandler
//==========================================================================
GlobalActions::TakeScreenshotHandler::TakeScreenshotHandler(
    /* [in] */ GlobalActions* host,
    /* [in] */ IServiceConnection* conn)
    : mHost(host)
    , mConnection(conn)
{}

ECode GlobalActions::TakeScreenshotHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return Handler::constructor(looper);
}

ECode GlobalActions::TakeScreenshotHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoLock syncLock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection == mConnection) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
        mHost->mHandler->RemoveCallbacks(mHost->mScreenshotTimeout);
    }
    return NOERROR;
}

//==========================================================================
// GlobalActions::TakeScreenshotHandler
//==========================================================================
CAR_INTERFACE_IMPL(GlobalActions::TakeScreenshotServiceConnection, Object, IServiceConnection)

GlobalActions::TakeScreenshotServiceConnection::TakeScreenshotServiceConnection(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{}

ECode GlobalActions::TakeScreenshotServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoLock syncLock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection != this) {
        return NOERROR;
    }
    AutoPtr<IMessenger> messenger;
    CMessenger::New(IIMessenger::Probe(service), (IMessenger**)&messenger);
    AutoPtr<IMessage> msg;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain(NULL, 1, (IMessage**)&msg);

    AutoPtr<IServiceConnection> myConn = this;
    AutoPtr<ILooper> looper;
    mHost->mHandler->GetLooper((ILooper**)&looper);
    AutoPtr<TakeScreenshotHandler> h = new TakeScreenshotHandler(mHost, this);
    h->constructor(looper);
    AutoPtr<IMessenger> newMessager;
    CMessenger::New(h.Get(), (IMessenger**)&newMessager);
    msg->SetReplyTo(newMessager);
    msg->SetArg1(0);
    msg->SetArg2(0);

    /*  remove for the time being
    if (mStatusBar != null && mStatusBar.isVisibleLw())
        msg.arg1 = 1;
    if (mNavigationBar != null && mNavigationBar.isVisibleLw())
        msg.arg2 = 1;
     */

    /* wait for the dialog box to close */
    // try {
        Thread::Sleep(1000);
    // } catch (InterruptedException ie) {
    //     // Do nothing
    // }

    /* take the screenshot */
    // try {
        messenger->Send(msg);
    // } catch (RemoteException e) {
    //     // Do nothing
    // }
    return NOERROR;
}

ECode GlobalActions::TakeScreenshotServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}

//==========================================================================
// GlobalActions::DialogListener
//==========================================================================
CAR_INTERFACE_IMPL_2(GlobalActions::DialogListener, Object, \
    IDialogInterfaceOnDismissListener, IDialogInterfaceOnClickListener)

GlobalActions::DialogListener::DialogListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{}

ECode GlobalActions::DialogListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mHost->mShowSilentToggle) {
        ECode ec = mHost->mContext->UnregisterReceiver(mHost->mRingerModeReceiver);
        if (FAILED(ec))
            Logger::W(TAG, "GlobalActions::OnDismiss");
    }
    return NOERROR;
}

ECode GlobalActions::DialogListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IInterface> action;
    mHost->mAdapter->GetItem(which, (IInterface**)&action);
    if (IGlobalActionsSilentModeTriStateAction::Probe(action) == NULL) {
        dialog->Dismiss();
    }

    ((Action*)IObject::Probe(action))->OnPress();
    return NOERROR;
}

//==========================================================================
// GlobalActions::MyPhoneStateListener
//==========================================================================
GlobalActions::MyPhoneStateListener::MyPhoneStateListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{}

ECode GlobalActions::MyPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    if (!mHost->mHasTelephony) return NOERROR;
    Int32 state;
    serviceState->GetState(&state);
    Boolean inAirplaneMode = state == IServiceState::STATE_POWER_OFF;
    mHost->mAirplaneState = inAirplaneMode ? ToggleAction::On : ToggleAction::Off;
    mHost->mAirplaneModeOn->UpdateState(mHost->mAirplaneState);
    mHost->mAdapter->NotifyDataSetChanged();
    return NOERROR;
}

//==========================================================================
// GlobalActions
//==========================================================================
GlobalActions::GlobalActions()
    : mKeyguardShowing(FALSE)
    , mDeviceProvisioned(FALSE)
    , mAirplaneState(ToggleAction::Off)
    , mIsWaitingForEcmExit(FALSE)
    , mHasTelephony(FALSE)
    , mHasVibrator(FALSE)
    , mShowSilentToggle(FALSE)
    , mProfilesEnabled(FALSE)
{}

ECode GlobalActions::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs)
{
    // Init global variable of inner class.
    mScreenshotTimeout = new ScreenshotTimeoutRunnable(this);
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    mThemeChangeReceiver = new ThemeChangeReceiver(this);
    AutoPtr<MyPhoneStateListener> psl = new MyPhoneStateListener(this);
    psl->constructor();
    mPhoneStateListener = psl.Get();
    mRingerModeReceiver = new RingerModeReceiver(this);
    mAirplaneModeObserver = new AirplaneModeObserver(this);
    mAirplaneModeObserver->constructor(mHandler);
    mScreenshotTimeout = new ScreenshotTimeoutRunnable(this);
    mHandler = new DialogHandler(this);
    mHandler->constructor();

    mContext = context;
    mWindowManagerFuncs = windowManagerFuncs;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    obj = ServiceManager::GetService(IDreamService::DREAM_SERVICE);
    mDreamManager = IIDreamManager::Probe(obj);

    // receive broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::UPDATE_POWER_MENU);
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    ThemeUtils::RegisterThemeChangeReceiver(context, mThemeChangeReceiver);

    obj = NULL;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &mHasTelephony);

    obj = NULL;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
    if (telephonyManager) {
        telephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_SERVICE_STATE);
    }

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::AIRPLANE_MODE_ON, (IUri**)&uri);
    contentResolver->RegisterContentObserver(uri, TRUE, mAirplaneModeObserver);

    obj = NULL;
    mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    AutoPtr<IVibrator> vibrator = IVibrator::Probe(obj);
    if (vibrator != NULL) {
        vibrator->HasVibrator(&mHasVibrator);
    }

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean useFixedV;
    resources->GetBoolean(R::bool_::config_useFixedVolume, &useFixedV);
    mShowSilentToggle = SHOW_SILENT_TOGGLE && !useFixedV;

    UpdatePowerMenuActions();
    return NOERROR;
}

void GlobalActions::ShowDialog(
    /* [in] */ Boolean keyguardShowing,
    /* [in] */ Boolean isDeviceProvisioned)
{
    mKeyguardShowing = keyguardShowing;
    mDeviceProvisioned = isDeviceProvisioned;
    if (mDialog != NULL && mUiContext == NULL) {
        mDialog->Dismiss();
        mDialog = NULL;
        // Show delayed, so that the dismiss of the previous dialog completes
        Boolean isSuccess = FALSE;
        mHandler->SendEmptyMessage(MESSAGE_SHOW, &isSuccess);
    }
    else {
        mDialog = CreateDialog();
        HandleShow();
    }
}

void GlobalActions::AwakenIfNecessary()
{
    if (mDreamManager != NULL) {
        Boolean isDreaming = FALSE;
        if (mDreamManager->IsDreaming(&isDreaming), isDreaming) {
            mDreamManager->Awaken();
        }
    }
}

void GlobalActions::HandleShow()
{
    AwakenIfNecessary();
    PrepareDialog();

    // If we only have 1 item and it's a simple press action, just do this action.
    Int32 count;
    mAdapter->GetCount(&count);
    if (count == 1) {
        AutoPtr<IInterface> item;
        mAdapter->GetItem(0, (IInterface**)&item);
        IGlobalActionsSinglePressAction* ispAction = IGlobalActionsSinglePressAction::Probe(item);
        if (ispAction != NULL) {
            IGlobalActionsLongPressAction* ilpAction = IGlobalActionsLongPressAction::Probe(item);
            if (ilpAction != NULL) {
                SinglePressAction* spAction = (SinglePressAction*)ispAction;
                spAction->OnPress();
                return;
            }
        }
    }

    {
        AutoPtr<IWindow> window;
        mDialog->GetWindow((IWindow**)&window);
        AutoPtr<IWindowManagerLayoutParams> attrs;
        window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
        attrs->SetTitle(CoreUtils::Convert("GlobalActions"));
        window->SetAttributes(attrs);
        mDialog->Show();
        AutoPtr<IView> decorView;
        window->GetDecorView((IView**)&decorView);
        decorView->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_EXPAND);
    }
}

AutoPtr<IContext> GlobalActions::GetUiContext()
{
    if (mUiContext == NULL) {
        mUiContext = ThemeUtils::CreateUiContext(mContext);
        mUiContext->SetTheme(R::style::Theme_DeviceDefault_Light_DarkActionBar);
    }
    return mUiContext != NULL ? mUiContext : mContext;
}

AutoPtr<GlobalActions::GlobalActionsDialog> GlobalActions::CreateDialog()
{
    // Simple toggle style if there's no vibrator, otherwise use a tri-state
    if (!mHasVibrator) {
        mSilentModeAction = new SilentModeToggleAction(this);
    }
    else {
        mSilentModeAction = new SilentModeTriStateAction(mContext, mAudioManager, mHandler);
    }

    mAirplaneModeOn = new AirplaneModeOnAction(this);
    OnAirplaneModeChanged();

    mItems = new List< AutoPtr<Action> >();

    AutoPtr<ArrayOf<String> > actionsArray;
    if (mActions == NULL) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        resources->GetStringArray(R::array::config_globalActionsList, (ArrayOf<String>**)&actionsArray);
    }
    else {
        StringUtils::Split(mActions, "\\|", (ArrayOf<String>**)&actionsArray);
    }

    // Always add the power off option
    AutoPtr<Action> action = new PowerAction(this);
    mItems->PushBack(action);

    HashSet<String> addedKeys;
    for (Int32 i = 0; i < actionsArray->GetLength(); ++i) {
        String actionKey = (*actionsArray)[i];
        HashSet<String>::Iterator iter = addedKeys.Find(actionKey);
        if (iter != addedKeys.End()) {
            // If we already have added this, don't add it again.
            continue;
        }

        if (IPowerMenuConstants::GLOBAL_ACTION_KEY_POWER.Equals(actionKey)) {
            continue;
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_REBOOT.Equals(actionKey)) {
            action = new RebootAction(this);
            mItems->PushBack(action);
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_SCREENSHOT.Equals(actionKey)) {
            mItems->PushBack(GetScreenshotAction());
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_AIRPLANE.Equals(actionKey)) {
            mItems->PushBack(mAirplaneModeOn);
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_BUGREPORT.Equals(actionKey)) {
            AutoPtr<IContentResolver> contentResolver;
            mContext->GetContentResolver((IContentResolver**)&contentResolver);
            Int32 value;
            Settings::Global::GetInt32(contentResolver, ISettingsGlobal::BUGREPORT_IN_POWER_MENU, 0, &value);
            if (value != 0 && IsCurrentUserOwner()) {
                mItems->PushBack(GetBugReportAction());
            }
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_SILENT.Equals(actionKey)) {
            if (mShowSilentToggle) {
                mItems->PushBack(mSilentModeAction);
            }
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_USERS.Equals(actionKey)) {
            Boolean proRes;
            SystemProperties::GetBoolean(String("fw.power_user_switcher"), FALSE, &proRes);
            if (proRes) {
                AddUsersToMenu(mItems);
            }
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_SETTINGS.Equals(actionKey)) {
            mItems->PushBack(GetSettingsAction());
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_LOCKDOWN.Equals(actionKey)) {
            mItems->PushBack(GetLockdownAction());
        }
        else if (IPowerMenuConstants::GLOBAL_ACTION_KEY_PROFILE.Equals(actionKey)) {
            if (!mProfilesEnabled) continue;
            action = new ProfileAction(this);
            mItems->PushBack(action);
        }
        else {
            Logger::E(TAG, "Invalid global action key %s", actionKey.string());
        }

        // Add here so we don't add more than one.
        addedKeys.Insert(actionKey);
    }

    mAdapter = new MyAdapter(this);
    AutoPtr<DialogListener> diodl = new DialogListener(this);

    AutoPtr<IAlertControllerAlertParams> params;
    CAlertControllerAlertParams::New(GetUiContext(), (IAlertControllerAlertParams**)&params);
    params->SetAdapter(mAdapter.Get());
    params->SetOnClickListener(diodl);
    params->SetForceInverseBackground(TRUE);

    AutoPtr<GlobalActionsDialog> dialog = new GlobalActionsDialog();
    dialog->constructor(GetUiContext(), params);
    dialog->SetCanceledOnTouchOutside(FALSE); // Handled by the custom class.

    AutoPtr<IListView> listv = dialog->GetListView();
    listv->SetItemsCanFocus(TRUE);
    IView* vForList = IView::Probe(listv);
    vForList->SetLongClickable(TRUE);
    IAdapterView* aForList = IAdapterView::Probe(listv);
    AutoPtr<IAdapterViewOnItemLongClickListener> avoic = new MyAdapterViewOnItemLongClickListener(this);
    aForList->SetOnItemLongClickListener(avoic);
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);

    dialog->SetOnDismissListener(diodl);

    return dialog;
}

void GlobalActions::CreateProfileDialog()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&obj);
    AutoPtr<IProfileManager> profileManager = IProfileManager::Probe(obj);

    AutoPtr<ArrayOf<IProfile*> > profiles;
    profileManager->GetProfiles((ArrayOf<IProfile*>**)&profiles);

    AutoPtr<IProfile> activeProfile;
    profileManager->GetActiveProfile((IProfile**)&activeProfile);
    AutoPtr<IUUID> activeUUID;
    activeProfile->GetUuid((IUUID**)&activeUUID);

    AutoPtr<ArrayOf<ICharSequence*> > names;
    Int32 checkedItem = 0;

    if (profiles) {
        names = ArrayOf<ICharSequence*>::Alloc(profiles->GetLength());
        String name;
        for (Int32 i = 0; i < profiles->GetLength(); ++i) {
            IProfile* profile = (*profiles)[i];
            AutoPtr<IUUID> uuid;
            profile->GetUuid((IUUID**)&uuid);
            if (Object::Equals(activeUUID, uuid)) {
                checkedItem = i;
                mChosenProfile = profile;
            }

            profile->GetName(&name);
            names->Set(i, CoreUtils::Convert(name));
        }
    }

    AutoPtr<IAlertDialogBuilder> ab;
    CAlertDialogBuilder::New(GetUiContext(), (IAlertDialogBuilder**)&ab);
    AutoPtr<IDialogInterfaceOnClickListener> listener = new ProfileDialogOnClickListener(this, profiles);
    ab->SetSingleChoiceItems(names, checkedItem, listener);
    AutoPtr<IAlertDialog> dialog;
    ab->Create((IAlertDialog**)&dialog);

    AutoPtr<IWindow> window;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
    IDialog::Probe(dialog)->Show();
}


AutoPtr<GlobalActions::Action> GlobalActions::GetScreenshotAction()
{
    AutoPtr<Action> action = new ScreenshotAction(this);
    return action;
}

AutoPtr<GlobalActions::Action> GlobalActions::GetBugReportAction()
{
    AutoPtr<Action> action = new BugSinglePressAction(this);
    return action;
}

AutoPtr<GlobalActions::Action> GlobalActions::GetSettingsAction()
{
    AutoPtr<Action> action = new SettingsSinglePressAction(this);
    return action;
}

AutoPtr<GlobalActions::Action> GlobalActions::GetLockdownAction()
{
    AutoPtr<Action> action = new LockdownSinglePressAction(this);
    return action;
}

AutoPtr<IUserInfo> GlobalActions::GetCurrentUser()
{
    AutoPtr<IUserInfo> currentUser;
    ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
    if (FAILED(ec))
        return NULL;
    return currentUser;
}

Boolean GlobalActions::IsCurrentUserOwner()
{
    AutoPtr<IUserInfo> currentUser = GetCurrentUser();
    if (currentUser == NULL)
        return TRUE;
    Boolean res;
    currentUser->IsPrimary(&res);
    return res;
}

void GlobalActions::AddUsersToMenu(
    /* [in] */ List< AutoPtr<Action> >* items)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    Boolean isSwitchEnabled = FALSE;
    userManager->IsUserSwitcherEnabled(&isSwitchEnabled);
    if (isSwitchEnabled) {
        AutoPtr<IList> users;
        userManager->GetUsers((IList**)&users);
        if (users == NULL) return;

        AutoPtr<IUserInfo> currentUser = GetCurrentUser();
        Int32 avatarSize;
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetDimensionPixelSize(R::dimen::global_actions_avatar_size, &avatarSize);

        AutoPtr<IIterator> iterator;
        users->GetIterator((IIterator**)&iterator);
        Boolean hasNext = FALSE;
        Boolean supportST;
        while (iterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iterator->GetNext((IInterface**)&obj);
            IUserInfo* user = IUserInfo::Probe(obj);
            user->SupportsSwitchTo(&supportST);
            Int32 userId = 0;
            user->GetId(&userId);
            if (supportST) {
                Boolean isCurrentUser = FALSE;
                if (currentUser == NULL) {
                    isCurrentUser = userId == 0;
                }
                else {
                    Int32 currentUserId = 0;
                    currentUser->GetId(&currentUserId);
                    isCurrentUser = (currentUserId == userId);
                }

                AutoPtr<IBitmap> rawAvatar;
                userManager->GetUserIcon(userId, (IBitmap**)&rawAvatar);
                if (rawAvatar == NULL) {
                    Boolean isGuest;
                    user->IsGuest(&isGuest);
                    AutoPtr<IDrawable> defaultIcon;
                    UserIcons::GetDefaultUserIcon(
                        isGuest ? IUserHandle::USER_NULL : userId, /*light=*/ FALSE,
                        (IDrawable**)&defaultIcon);
                    UserIcons::ConvertToBitmap(defaultIcon, (IBitmap**)&rawAvatar);
                }
                AutoPtr<IDrawable> avatar;
                CBitmapDrawable::New(res, CreateCircularClip(rawAvatar, avatarSize, avatarSize),
                    (IDrawable**)&avatar);

                String userName;
                user->GetName(&userName);
                String str = (userName != NULL ? userName : String("Primary"));
                AutoPtr<ICharSequence> value = CoreUtils::Convert(str);
                AutoPtr<UserSinglePressAction> switchToUser = new UserSinglePressAction(this,
                        R::drawable::ic_lock_user, avatar, value, userId);
                if (isCurrentUser) {
                    mContext->GetString(R::string::global_action_current_user, &str);
                    switchToUser->SetStatus(CoreUtils::Convert(str));
                }
                items->PushBack(switchToUser);
            }
        }
    }
}

void GlobalActions::TakeScreenshot()
{
    AutoLock syncLock(mScreenshotLock);
    if (mScreenshotConnection != NULL) {
        return;
    }

    AutoPtr<IComponentName> cn;
    CComponentName::New(
        String("Elastos.Droid.SystemUI"),
        String("Elastos.Droid.SystemUI.Screenshot.CTakeScreenshotService"),
        (IComponentName**)&cn);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(cn);
    AutoPtr<IServiceConnection> conn = new TakeScreenshotServiceConnection(this);
    Boolean bval;
    mContext->BindServiceAsUser(intent, conn, IContext::BIND_AUTO_CREATE,
        UserHandle::CURRENT, &bval);
    if (bval) {
        mScreenshotConnection = conn;
        mHandler->PostDelayed(mScreenshotTimeout, 10000, &bval);
    }
}

void GlobalActions::PrepareDialog()
{
    RefreshSilentMode();
    mAirplaneModeOn->UpdateState(mAirplaneState);
    mAdapter->NotifyDataSetChanged();
    AutoPtr<IWindow> window;
    mDialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    if (mShowSilentToggle) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IAudioManager::RINGER_MODE_CHANGED_ACTION, (IIntentFilter**)&filter);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(mRingerModeReceiver, filter, (IIntent**)&intent);
    }
}

void GlobalActions::RefreshSilentMode()
{
    if (!mHasVibrator) {
        Int32 ringerMode = 0;
        mAudioManager->GetRingerMode(&ringerMode);
        Boolean silentModeOn = ringerMode != IAudioManager::RINGER_MODE_NORMAL;
        ((ToggleAction*)mSilentModeAction.Get())->UpdateState(
                silentModeOn ? ToggleAction::On : ToggleAction::Off);
    }
}

void GlobalActions::UpdatePowerMenuActions()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Settings::Secure::GetStringForUser(resolver,
        ISettingsSecure::POWER_MENU_ACTIONS, UserHandle::USER_CURRENT, &mActions);

    Int32 ival;
    Settings::System::GetInt32(resolver,
        ISettingsSystem::SYSTEM_PROFILES_ENABLED, 1, &ival);
    mProfilesEnabled = ival != 0;
}

void GlobalActions::OnAirplaneModeChanged()
{
    // Let the service state callbacks handle the state.
    if (mHasTelephony) {
        return;
    }

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 value = 0;
    Settings::Global::GetInt32(contentResolver,
        ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    Boolean airplaneModeOn = value == 1;
    mAirplaneState = airplaneModeOn ? ToggleAction::On : ToggleAction::Off;
    mAirplaneModeOn->UpdateState(mAirplaneState);
}

void GlobalActions::ChangeAirplaneModeSystemSetting(
    /* [in] */ Boolean on)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Boolean isSuccess = FALSE;
    Settings::Global::PutInt32(contentResolver,
        ISettingsGlobal::AIRPLANE_MODE_ON, on ? 1 : 0, &isSuccess);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutBooleanExtra(String("state"), on);

    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    if (!mHasTelephony) {
        mAirplaneState = on ? ToggleAction::On : ToggleAction::Off;
    }
}

void GlobalActions::StartQuickBoot()
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("org.codeaurora.action.QUICKBOOT"), (IIntent**)&intent);
    intent->PutExtra(String("mode"), 0);
    mContext->StartActivityAsUser(intent, UserHandle::CURRENT);
}

AutoPtr<IBitmap> GlobalActions::CreateCircularClip(
    /* [in] */ IBitmap* input,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (input == NULL) return NULL;

    Int32 inWidth, inHeight;
    input->GetWidth(&inWidth);
    input->GetHeight(&inHeight);
    AutoPtr<IBitmap> output;
    CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&output);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(output, (ICanvas**)&canvas);
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    AutoPtr<IShader> bs;
    CBitmapShader::New(input, ShaderTileMode_CLAMP, ShaderTileMode_CLAMP, (IShader**)&bs);
    paint->SetShader(bs);
    paint->SetAntiAlias(TRUE);
    AutoPtr<IRectF> srcRect, dstRect;
    CRectF::New(0, 0, inWidth, inHeight, (IRectF**)&dstRect);
    CRectF::New(0, 0, width, height, (IRectF**)&dstRect);
    AutoPtr<IMatrix> m;
    CMatrix::New((IMatrix**)&m);
    Boolean bval;
    m->SetRectToRect(srcRect, dstRect, MatrixScaleToFit_CENTER, &bval);
    canvas->SetMatrix(m);
    canvas->DrawCircle(inWidth / 2, inHeight / 2, inWidth / 2, paint);
    return output;
}


} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
