#include "Elastos.Droid.Service.h"
#include "elastos/droid/internal/policy/impl/GlobalActions.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/internal/app/CAlertController.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowManager.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/text/CTextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CWindowManagerGlobalHelper.h"
//TODO #include "elastos/droid/widget/internal/CLockPatternUtils.h"
#include <elastos/core/StringUtils.h>
#include "elastos/utility/etl/HashSet.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::App::CAlertController;
using Elastos::Droid::Internal::App::CAlertControllerAlertParams;
using Elastos::Droid::Internal::Policy::Impl::CPhoneWindowManager;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
//TODO using Elastos::Droid::Widget::Internal::CLockPatternUtils;
//TODO using Elastos::Droid::Widget::Internal::ILockPatternUtils;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//extern "C" const InterfaceID EIID_SilentModeTriStateAction =
//        { 0x7e1e9c2f, 0x50e0, 0x4a07, { 0xb8, 0x2a, 0x5c, 0x73, 0x40, 0x7e, 0xea, 0xe2 } };

const String GlobalActions::TAG("GlobalActions");

const Boolean GlobalActions::SHOW_SILENT_TOGGLE = TRUE;
const String GlobalActions::GLOBAL_ACTION_KEY_POWER("power");
const String GlobalActions::GLOBAL_ACTION_KEY_AIRPLANE("airplane");
const String GlobalActions::GLOBAL_ACTION_KEY_BUGREPORT("bugreport");
const String GlobalActions::GLOBAL_ACTION_KEY_SILENT("silent");
const String GlobalActions::GLOBAL_ACTION_KEY_USERS("users");
const String GlobalActions::GLOBAL_ACTION_KEY_SETTINGS("settings");
const String GlobalActions::GLOBAL_ACTION_KEY_LOCKDOWN("lockdown");


const Int32 GlobalActions::MESSAGE_DISMISS = 0;
const Int32 GlobalActions::MESSAGE_REFRESH = 1;
const Int32 GlobalActions::MESSAGE_SHOW = 2;
const Int32 GlobalActions::DIALOG_DISMISS_DELAY = 300; // ms

// Static variable of emnu 'State'
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::State::On = new State(FALSE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::State::Off = new State(FALSE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::State::TurningOn = new State(TRUE);
const AutoPtr<GlobalActions::ToggleAction::State> GlobalActions::ToggleAction::State::TurningOff = new State(TRUE);


GlobalActions::MyAdapter::MyAdapter(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::MyAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    AutoPtr<IInterface> action;
    GetItem(position, (IInterface**)&action);
    Action* a = (Action*)IGlobalActionsSilentModeTriStateAction::Probe(action);
    *enabled = a->IsEnabled();
    return NOERROR;
}

ECode GlobalActions::MyAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    *enabled = FALSE;
    return NOERROR;
}

ECode GlobalActions::MyAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return BaseAdapter::RegisterDataSetObserver(observer);
}

ECode GlobalActions::MyAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return BaseAdapter::UnregisterDataSetObserver(observer);
}

Int32 GlobalActions::MyAdapter::GetCount()
{
    Int32 count = 0;
    GetCount(&count);
    return count;
}

ECode GlobalActions::MyAdapter::GetCount(
    /* [out] */ Int32* count)
{
    Int32 temp = 0;
    List< AutoPtr<Action> >::Iterator it = mHost->mItems->Begin();
    for (; it != mHost->mItems->End(); ++it) {
        const AutoPtr<Action> action = *it;

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

AutoPtr<IInterface> GlobalActions::MyAdapter::GetItem(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    return item;
}

ECode GlobalActions::MyAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    Int32 filteredPos = 0;
    List< AutoPtr<Action> >::Iterator it = mHost->mItems->Begin();
    for (; it != mHost->mItems->End(); ++it) {
        const AutoPtr<Action> action = *it;
        if (mHost->mKeyguardShowing && !action->ShowDuringKeyguard()) {
            continue;
        }
        if (!mHost->mDeviceProvisioned && !action->ShowBeforeProvisioning()) {
            continue;
        }
        if (filteredPos == position) {
            *item = TO_IINTERFACE(action);
            REFCOUNT_ADD(*item);
            return NOERROR;
        }
        filteredPos++;
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // throw new IllegalArgumentException("position " + position
    //         + " out of range of showable actions"
    //         + ", filtered count=" + getCount()
    //         + ", keyguardshowing=" + mKeyguardShowing
    //         + ", provisioned=" + mDeviceProvisioned);
}

Int64 GlobalActions::MyAdapter::GetItemId(
    /* [in] */ Int32 position)
{
    Int64 itemId = 0;
    GetItemId(position, &itemId);
    return itemId;
}

ECode GlobalActions::MyAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    BaseAdapter::HasStableIds(hasStableIds);
    return NOERROR;
}

AutoPtr<IView> GlobalActions::MyAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    GetView(position, convertView, parent, (IView**)&view);
    return view;
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
    Action* action = ((Action*)IObject::Probe(obj));
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
    AutoPtr<IView> viewTemp = action->Create(mHost->GetUiContext(), convertView, parent, inflater);
    *view = viewTemp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    BaseAdapter::GetItemViewType(position, viewType);
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    BaseAdapter::GetViewTypeCount(count);
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    BaseAdapter::IsEmpty(isEmpty);
    return NOERROR;
}


//ECode GlobalActions::Action::GetInterfaceID(
//    /* [in] */ IInterface *pObject,
//    /* [out] */ InterfaceID *pIID)
//{
//    return NOERROR;
//}
//
//PInterface GlobalActions::Action::Probe(
//    /* [in]  */ REIID riid)
//{
//    if (riid == EIID_IInterface) {
//        return (PInterface)this;
//    }
//    return NULL;
//}
//
//UInt32 GlobalActions::Action::AddRef()
//{
//    return ElRefBase::AddRef();
//}
//
//UInt32 GlobalActions::Action::Release()
//{
//    return ElRefBase::Release();
//}

CAR_INTERFACE_IMPL(GlobalActions::SinglePressAction, Action, IGlobalActionsSinglePressAction);

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* owner,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 messageResId)
    : mOwner(owner)
    , mIconResId(iconResId)
    , mIcon(NULL)
    , mMessageResId(messageResId)
    , mMessage(NULL)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* owner,
    /* [in] */ Int32 iconResId,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* message)
    : mOwner(owner)
    , mIconResId(iconResId)
    , mIcon(icon)
    , mMessageResId(0)
    , mMessage(message)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ GlobalActions* owner,
    /* [in] */ Int32 iconResId,
    /* [in] */ ICharSequence* message)
    : mOwner(owner)
    , mIconResId(iconResId)
    , mIcon(NULL)
    , mMessageResId(0)
    , mMessage(message)
{
}

Boolean GlobalActions::SinglePressAction::IsEnabled()
{
    return TRUE;
}

String GlobalActions::SinglePressAction::GetStatus()
{
    return String(NULL);
}

//Boolean GlobalActions::SinglePressAction::OnLongPress()
//{
//    return FALSE;
//}

AutoPtr<ICharSequence> GlobalActions::SinglePressAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    if (mMessage != NULL)
    {
        return mMessage;
    }
    else
    {
        String result;
        mOwner->mContext->GetString(mMessageResId, &result);
        AutoPtr<ICharSequence> cs;
        CString::New(result, (ICharSequence**)&cs);
        return cs;
    }
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
    String status = GetStatus();
    AutoPtr<ITextUtils> textUtils;
    CTextUtils::AcquireSingleton((ITextUtils**)&textUtils);
    Boolean isEmpty;
    textUtils->IsEmpty(status, &isEmpty);
    if (!isEmpty)
    {
        AutoPtr<ITextView> statusView = ITextView::Probe(viewTemp);
        AutoPtr<ICharSequence> csStatus;
        CString::New(status, (ICharSequence**)&csStatus);
        statusView->SetText(csStatus);
    }
    else
    {
        viewTemp->SetVisibility(IView::GONE);
    }

    if (mIcon != NULL) {
        icon->SetImageDrawable(mIcon);
        icon->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_CENTER_CROP);
    }
    else if (mIconResId != 0) {
        //AutoPtr<IResources> resources;
        //context->GetResources((IResources**)&resources);
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

CAR_INTERFACE_IMPL(GlobalActions::PowerAction, SinglePressAction, IGlobalActionsLongPressAction);

GlobalActions::PowerAction::PowerAction(
    /* [in] */ GlobalActions* owner)
    :SinglePressAction(owner, R::drawable::ic_lock_power_off,R::string::global_action_power_off)
{
}

Boolean GlobalActions::PowerAction::OnLongPress()
{
    mOwner->mWindowManagerFuncs->RebootSafeMode(TRUE);
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

void GlobalActions::PowerAction::OnPress()
{
    // shutdown by making sure radio and power are handled accordingly.
    mOwner->mWindowManagerFuncs->Shutdown(false /* confirm */);
}

GlobalActions::ToggleAction::State::State(
    /* [in] */ Boolean intermediate)
    : inTransition(intermediate)
{
}

Boolean GlobalActions::ToggleAction::State::InTransition()
{
    return inTransition;
}

GlobalActions::ToggleAction::ToggleAction(
    /* [in] */ GlobalActions* owner,
    /* [in] */ Int32 enabledIconResId,
    /* [in] */ Int32 disabledIconResid,
    /* [in] */ Int32 message,
    /* [in] */ Int32 enabledStatusMessageResId,
    /* [in] */ Int32 disabledStatusMessageResId)
    : mState(State::Off)
    , mEnabledIconResId(enabledIconResId)
    , mDisabledIconResid(disabledIconResid)
    , mMessageResId(message)
    , mEnabledStatusMessageResId(enabledStatusMessageResId)
    , mDisabledStatusMessageResId(disabledStatusMessageResId)
    , mOwner(owner)
{
}

void GlobalActions::ToggleAction::WillCreate()
{
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

    Boolean on = ((mState == State::On) || (mState == State::TurningOn));
    if (icon != NULL) {
        //AutoPtr<IResources> resources;
        //context->GetResources((IResources**)&resources);
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

void GlobalActions::ToggleAction::OnPress()
{
    if (mState->InTransition()) {
        Slogger::W(TAG, "shouldn't be able to toggle when in transition");
        return;
    }

    const Boolean nowOn = !(mState == State::On);
    OnToggle(nowOn);
    ChangeStateFromPress(nowOn);
}

//Boolean GlobalActions::ToggleAction::OnLongPress()
//{
//    return FALSE;
//}
AutoPtr<ICharSequence> GlobalActions::ToggleAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    String result;
    mOwner->mContext->GetString(mMessageResId, &result);
    AutoPtr<ICharSequence> cs;
    CString::New(result, (ICharSequence**)&cs);
    return cs;
}

Boolean GlobalActions::ToggleAction::IsEnabled()
{
    return !mState->InTransition();
}

void GlobalActions::ToggleAction::UpdateState(
    /* [in] */ State* state)
{
    mState = state;
}

void GlobalActions::ToggleAction::ChangeStateFromPress(
    /* [in] */ Boolean buttonOn)
{
    mState = buttonOn ? State::On : State::Off;
}


GlobalActions::SilentModeToggleAction::SilentModeToggleAction(
    /* [in] */ GlobalActions* owner)
    : ToggleAction(
            owner,
            R::drawable::ic_audio_vol_mute,
            R::drawable::ic_audio_vol,
            R::string::global_action_toggle_silent_mode,
            R::string::global_action_silent_mode_on_status,
            R::string::global_action_silent_mode_off_status)
{
}

void GlobalActions::SilentModeToggleAction::OnToggle(
    /* [in] */ Boolean on)
{
    if (on) {
        mOwner->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_SILENT);
    } else {
        mOwner->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_NORMAL);
    }
}

Boolean GlobalActions::SilentModeToggleAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::SilentModeToggleAction::ShowBeforeProvisioning()
{
    return FALSE;
}

CAR_INTERFACE_IMPL_2(GlobalActions::SilentModeTriStateAction, GlobalActions::Action, IViewOnClickListener, IGlobalActionsSilentModeTriStateAction)

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
        AutoPtr<IInteger32> pValue;
        CInteger32::New(i, (IInteger32**)&pValue);
        itemView->SetTag(pValue);
        itemView->SetOnClickListener(this);
    }
    return v;
}

void GlobalActions::SilentModeTriStateAction::OnPress()
{
}

//Boolean GlobalActions::SilentModeTriStateAction::OnLongPress()
//{
//    return FALSE;
//}

AutoPtr<ICharSequence> GlobalActions::SilentModeTriStateAction::GetLabelForAccessibility(
    /* [in] */ IContext* context)
{
    return NULL;
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
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MESSAGE_DISMISS, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessageDelayed(message, DIALOG_DISMISS_DELAY, &isSuccess);
    return NOERROR;
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

GlobalActions::GlobalActionsDialog::EnableAccessibilityControllerRunnable::EnableAccessibilityControllerRunnable(
    /* [in] */ GlobalActionsDialog* owner)
    :mOwner(owner)
{
}

ECode GlobalActions::GlobalActionsDialog::EnableAccessibilityControllerRunnable::Run()
{
    return mOwner->Dismiss();
}

GlobalActions::GlobalActionsDialog::GlobalActionsDialog(
    /* [in] */ IContext* context,
    /* [in] */ IAlertControllerAlertParams* params)
    : mContext(context)
{
    Dialog::constructor(context, GetDialogTheme(context));
    AutoPtr<IWindow> w;
    GetWindow((IWindow**)&w);
    CAlertController::New(mContext, this, w, (IAlertController**)&mAlert);
    AutoPtr<IListAdapter> lAdapter;
    params->GetAdapter((IListAdapter**)&lAdapter);
    mAdapter = (MyAdapter*)(BaseAdapter*)(lAdapter.Get());
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfiguration;
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);
    viewConfiguration->GetScaledWindowTouchSlop(&mWindowTouchSlop);
    params->Apply(mAlert);
}

//CAR_INTERFACE_IMPL(GlobalActions::GlobalActionsDialog, IDialogInterface);

ECode GlobalActions::GlobalActionsDialog::Cancel()
{
    return Dialog::Cancel();
}

ECode GlobalActions::GlobalActionsDialog::Dismiss()
{
    return Dialog::Dismiss();
}


Boolean GlobalActions::GlobalActionsDialog::DispatchTouchEvent(
    /* [in] */ IMotionEvent* inEvent)
{
    AutoPtr<IMotionEvent> event = inEvent;
    if (mEnableAccessibilityController != NULL) {
        Int32 action = 0;
        event->GetActionMasked(&action);
        if (action == IMotionEvent::ACTION_DOWN) {
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            AutoPtr<IView> decor;
            window->GetDecorView((IView**)&decor);
            Float x = 0.0f;
            event->GetX(&x);
            Int32 eventX = (Int32)x;
            Float y = 0.0f;
            event->GetY(&y);
            Int32 eventY = (Int32)y;
            Int32 width = 0;
            decor->GetWidth(&width);
            Int32 height = 0;
            decor->GetHeight(&height);
            if (eventX < -mWindowTouchSlop
                    || eventY < -mWindowTouchSlop
                    || eventX >= width + mWindowTouchSlop
                    || eventY >= height + mWindowTouchSlop) {
                mCancelOnUp = TRUE;
            }
        }
        // try {
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
        }
        else {
            if (action == IMotionEvent::ACTION_UP) {
                if (mCancelOnUp) {
                    Dialog::Cancel();
                }
                mCancelOnUp = FALSE;
                mIntercepted = FALSE;
            }
            Boolean res;
            mEnableAccessibilityController->OnTouchEvent(event, &res);
            return res;
        }
        // } finally {
        if (action == IMotionEvent::ACTION_UP) {
            if (mCancelOnUp) {
                Dialog::Cancel();
            }
            mCancelOnUp = FALSE;
            mIntercepted = FALSE;
        }
        // }
    }
    Boolean res;
    Dialog::DispatchTouchEvent(event, &res);
    return res;
}

AutoPtr<IListView> GlobalActions::GlobalActionsDialog::GetListView()
{
    AutoPtr<IListView> listView;
    mAlert->GetListView((IListView**)&listView);
    return listView;
}

// @Override
ECode GlobalActions::GlobalActionsDialog::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 eventType;
    event->GetEventType(&eventType);
    if (eventType == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED)
    {
        Int32 count;
        mAdapter->GetCount(&count);
        for (Int32 i = 0; i < count; ++i)
        {
            AutoPtr<IInterface> item;
            mAdapter->GetItem(i, (IInterface**)&item);
            AutoPtr<Action> action = (Action*)(IObject::Probe(item));
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<ICharSequence> label = action->GetLabelForAccessibility(ctx);
            if (label != NULL)
            {
                //event.getText().add(label);
                AutoPtr<IList> list;
                IAccessibilityRecord* ar = IAccessibilityRecord::Probe(event);
                ar->GetText((IList**)&list);
                list->Add(TO_IINTERFACE(label));
            }
        }
    }
    return Dialog::DispatchPopulateAccessibilityEvent(event, result);
}

Boolean GlobalActions::GlobalActionsDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Boolean isOnKeyDown = FALSE;
    if (mAlert->OnKeyDown(keyCode, event, &isOnKeyDown), isOnKeyDown) {
        return TRUE;
    }
    Dialog::OnKeyDown(keyCode, event, &isOnKeyDown);
    return isOnKeyDown;
}

Boolean GlobalActions::GlobalActionsDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Boolean isOnKeyUp = FALSE;
    if (mAlert->OnKeyUp(keyCode, event, &isOnKeyUp), isOnKeyUp) {
        return TRUE;
    }
    Dialog::OnKeyUp(keyCode, event, &isOnKeyUp);
    return isOnKeyUp;
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
    } else {
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

ECode GlobalActions::GlobalActionsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);
    mAlert->InstallContent();
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
            AutoPtr<IMessage> message;
            mHost->mHandler->ObtainMessage(MESSAGE_DISMISS, (IMessage**)&message);
            Boolean isSuccess = FALSE;
            mHost->mHandler->SendMessage(message, &isSuccess);
        }
    } else if (ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED.Equals(action)) {
        // Airplane mode can be changed after ECM exits if airplane toggle button
        // is pressed during ECM mode
        Boolean bTemp = FALSE;
        if (!((intent->GetBooleanExtra(String("PHONE_IN_ECM_STATE"), FALSE, &bTemp), bTemp)) &&
                mHost->mIsWaitingForEcmExit) {
            mHost->mIsWaitingForEcmExit = FALSE;
            mHost->ChangeAirplaneModeSystemSetting(TRUE);
        }
    }
    return NOERROR;
}


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
        AutoPtr<IMessage> message;
        mHost->mHandler->ObtainMessage(MESSAGE_REFRESH, (IMessage**)&message);
        Boolean isSuccess = FALSE;
        mHost->mHandler->SendMessage(message, &isSuccess);
    }
    return NOERROR;
}

GlobalActions::AirplaneModeObserver::AirplaneModeObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);
}

ECode GlobalActions::AirplaneModeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->OnAirplaneModeChanged();
    return NOERROR;
}

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


GlobalActions::AirplaneModeOn::AirplaneModeOn(
    /* [in] */ GlobalActions* owner)
    : ToggleAction(
            owner,
            R::drawable::ic_lock_airplane_mode,
            R::drawable::ic_lock_airplane_mode_off,
            R::string::global_actions_toggle_airplane_mode,
            R::string::global_actions_airplane_mode_on_status,
            R::string::global_actions_airplane_mode_off_status)
    , mHost(owner)
{
}

void GlobalActions::AirplaneModeOn::OnToggle(
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
}

Boolean GlobalActions::AirplaneModeOn::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::AirplaneModeOn::ShowBeforeProvisioning()
{
    return FALSE;
}

void GlobalActions::AirplaneModeOn::ChangeStateFromPress(
    /* [in] */ Boolean buttonOn)
{
    if (!mOwner->mHasTelephony) {
        return;
    }

    // In ECM mode airplane state cannot be changed
    String value;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, &value);
    Boolean bValue = StringUtils::ParseBoolean(value);
    if (!(bValue)) {
        mState = buttonOn ? ToggleAction::State::TurningOn : ToggleAction::State::TurningOff;
        mHost->mAirplaneState = mState;
    }
}


//GlobalActions::PowerSinglePressAction::PowerSinglePressAction(
//    /* [in] */ GlobalActions* host)
//    : SinglePressAction(R::drawable::ic_lock_power_off,
//            R::string::global_action_power_off)
//    , mHost(host)
//{
//}
//
//void GlobalActions::PowerSinglePressAction::OnPress()
//{
//    // shutdown by making sure radio and power are handled accordingly.
//    mHost->mWindowManagerFuncs->Shutdown(TRUE);
//}
//
//Boolean GlobalActions::PowerSinglePressAction::OnLongPress()
//{
//    mHost->mWindowManagerFuncs->RebootSafeMode(TRUE);
//    return TRUE;
//}
//
//Boolean GlobalActions::PowerSinglePressAction::ShowDuringKeyguard()
//{
//    return TRUE;
//}
//
//Boolean GlobalActions::PowerSinglePressAction::ShowBeforeProvisioning()
//{
//    return TRUE;
//}


ECode GlobalActions::BugRunnable::Run()
{
    // try {
    ActivityManagerNative::GetDefault()->RequestBugReport();
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

CAR_INTERFACE_IMPL(GlobalActions::DialogOnClickListener, Object, IDialogInterfaceOnClickListener);

GlobalActions::DialogOnClickListener::DialogOnClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

ECode GlobalActions::DialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // don't actually trigger the bugreport if we are running stability
    // tests via monkey
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean userAMonkey;
    amHelper->IsUserAMonkey(&userAMonkey);
    if (userAMonkey)
    {
        return NOERROR;
    }
    // Add a little delay before executing, to give the
    // dialog a chance to go away before it takes a
    // screenshot.
    Boolean isSuccess = FALSE;
    AutoPtr<IRunnable> bugRunnable = new BugRunnable();
    return mHost->mHandler->PostDelayed(bugRunnable, 500, &isSuccess);
}


GlobalActions::BugSinglePressAction::BugSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(host, R::drawable::ic_lock_bugreport,
            R::string::bugreport_title)
{
}

void GlobalActions::BugSinglePressAction::OnPress()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mOwner->mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::bugreport_title);
    builder->SetMessage(R::string::bugreport_message);
    builder->SetNegativeButton(R::string::cancel, NULL);
    builder->SetPositiveButton(R::string::report,
            new DialogOnClickListener(mOwner));
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    AutoPtr<IWindow> window;
    IDialog* idialog = IDialog::Probe(dialog);
    idialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    idialog->Show();
}

Boolean GlobalActions::BugSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::BugSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

String GlobalActions::BugSinglePressAction::GetStatus()
{
    AutoPtr<ICharSequence> id;
    CString::New(Build::ID, (ICharSequence**)&id);
    AutoPtr<ICharSequence> release;
    CString::New(Build::VERSION::RELEASE, (ICharSequence**)&release);
    AutoPtr<ArrayOf<IInterface*> > fArgs = ArrayOf<IInterface*>::Alloc(2);
    fArgs->Set(0, TO_IINTERFACE(release));
    fArgs->Set(1, TO_IINTERFACE(id));
    //return mContext.getString(
    //        com.android.internal.R.string.bugreport_status,
    //        Build.VERSION.RELEASE,
    //        Build.ID);
    String result;
    mOwner->mContext->GetString(R::string::bugreport_status, fArgs, &result);
    return result;
}

GlobalActions::SettingsSinglePressAction::SettingsSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(host, R::drawable::ic_settings,
            R::string::global_action_settings)
{
}

void GlobalActions::SettingsSinglePressAction::OnPress()
{
    //Intent intent = new Intent(Settings.ACTION_SETTINGS);
    //intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_SETTINGS, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK|IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    mOwner->mContext->StartActivity(intent);
}

Boolean GlobalActions::SettingsSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::SettingsSinglePressAction::ShowBeforeProvisioning()
{
    return TRUE;
}

GlobalActions::LockdownSinglePressAction::LockdownSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(host, R::drawable::ic_lock_lock, R::string::global_action_lockdown)
{
}

void GlobalActions::LockdownSinglePressAction::OnPress()
{
    //new LockPatternUtils(mContext).requireCredentialEntry(UserHandle.USER_ALL);
    //TODO AutoPtr<ILockPatternUtils> lpu;
    //TODO CLockPatternUtils::New(mContext, (ILockPatternUtils**)&lpu);
    //TODO lpu->RequireCredentialEntry(IUserHandle::USER_ALL);
    //try {
    AutoPtr<IWindowManagerGlobalHelper> wmgh;
    CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&wmgh);
    //WindowManagerGlobal.getWindowManagerService().lockNow(null);
    AutoPtr<IIWindowManager> wm;
    wmgh->GetWindowManagerService((IIWindowManager**)&wm);
    wm->LockNow(NULL);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Error while trying to lock device.", e);
    //}
}

Boolean GlobalActions::LockdownSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::LockdownSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

GlobalActions::MyAdapterViewOnItemLongClickListener::MyAdapterViewOnItemLongClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(GlobalActions::MyAdapterViewOnItemLongClickListener, Object, IAdapterViewOnItemClickListener);

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


GlobalActions::UserSinglePressAction::UserSinglePressAction(
    /* [in] */ GlobalActions* owner,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* message,
    /* [in] */ Int32 userId)
    : SinglePressAction(owner, R::drawable::ic_menu_cc, icon, message)
    , mUserId(userId)
{
}

void GlobalActions::UserSinglePressAction::OnPress()
{
    // try {
    Boolean value = FALSE;
    ECode ec = ActivityManagerNative::GetDefault()->SwitchUser(mUserId, &value);
    // } catch (RemoteException re) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't switch user %d" + mUserId);
    }
    // }
}

Boolean GlobalActions::UserSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::UserSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}

CAR_INTERFACE_IMPL_2(GlobalActions, Object, IDialogInterfaceOnDismissListener, IDialogInterfaceOnClickListener);

GlobalActions::GlobalActions(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs)
{
    // Init global variable of inner class.
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    mThemeChangeReceiver = new ThemeChangeReceiver(this);
    // TODO: PhoneStateListener is not implement.
    // PhoneStateListener mPhoneStateListener;
    mRingerModeReceiver = new RingerModeReceiver(this);
    mHandler = new DialogHandler(this);
    mHandler->constructor();

    mAirplaneModeObserver = new AirplaneModeObserver(mHandler, this);

    mContext = context;
    mWindowManagerFuncs = windowManagerFuncs;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    obj = NULL;
    sm->GetService(IDreamService::DREAM_SERVICE, (IInterface**)&obj);
    mDreamManager = IIDreamManager::Probe(obj);

    // receive broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::UPDATE_POWER_MENU);
    // TODO: Telephone is not implement.
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    ThemeUtils::RegisterThemeChangeReceiver(context, mThemeChangeReceiver);

    obj = NULL;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &mHasTelephony);
    // get notified of phone state changes
    // TODO: Telephone is not implement.
    // TelephonyManager telephonyManager =
    //         (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    // telephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_SERVICE_STATE);

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    AutoPtr<IUri> uri;
    settingsGlobal->GetUriFor(ISettingsGlobal::AIRPLANE_MODE_ON, (IUri**)&uri);
    contentResolver->RegisterContentObserver(
            uri, TRUE,
            mAirplaneModeObserver);

    obj = NULL;
    mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    AutoPtr<IVibrator> vibrator = IVibrator::Probe(obj);
    Boolean hasVibrator = FALSE;
    mHasVibrator = vibrator != NULL && (vibrator->HasVibrator(&hasVibrator), hasVibrator);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean useFixedV;
    resources->GetBoolean(R::bool_::config_useFixedVolume, &useFixedV);

    mShowSilentToggle = SHOW_SILENT_TOGGLE && !useFixedV;
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
        AutoPtr<IMessage> message;
        mHandler->ObtainMessage(MESSAGE_SHOW, (IMessage**)&message);
        Boolean isSuccess = FALSE;
        mHandler->SendMessage(message, &isSuccess);
    } else {
        mDialog = CreateDialog();
        HandleShow();
    }
}

void GlobalActions::AwakenIfNecessary()
{
    if (mDreamManager != NULL) {
        // try {
        Boolean isDreaming = FALSE;
        if (mDreamManager->IsDreaming(&isDreaming), isDreaming) {
            mDreamManager->Awaken();
        }
        // } catch (RemoteException e) {
            // we tried
        // }
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
        //mAdapter.getItem(0) instanceof SinglePressAction
        IGlobalActionsSinglePressAction* ispAction = IGlobalActionsSinglePressAction::Probe(item);
        if (ispAction != NULL) {
            //&& !(mAdapter.getItem(0) instanceof LongPressAction))
            IGlobalActionsLongPressAction* ilpAction = IGlobalActionsLongPressAction::Probe(item);
            if (ilpAction != NULL) {
                //((SinglePressAction) mAdapter.getItem(0)).onPress();
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
        AutoPtr<ICharSequence> title;
        CString::New(String("GlobalActions"), (ICharSequence**)&title);
        attrs->SetTitle(title);
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
    } else {
        mSilentModeAction = new SilentModeTriStateAction(mContext, mAudioManager, mHandler);
    }

    mAirplaneModeOn = new AirplaneModeOn(this);

    OnAirplaneModeChanged();

    mItems = new List< AutoPtr<Action> >();
    AutoPtr<ArrayOf<String> > defaultActions;
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetStringArray(R::array::config_globalActionsList, (ArrayOf<String>**)&defaultActions);

    //ArraySet<String> addedKeys = new ArraySet<String>();
    HashSet<String> addedKeys;
    for (Int32 i = 0; i < defaultActions->GetLength(); ++i)
    {
        String actionKey = (*defaultActions)[i];
        HashSet<String>::Iterator iter = addedKeys.Find(actionKey);
        //if (addedKeys.contains(actionKey))
        if (iter != addedKeys.End())
        {
            // If we already have added this, don't add it again.
            continue;
        }
        if (GLOBAL_ACTION_KEY_POWER.Equals(actionKey))
        {
            //mItems.add(new PowerAction());
            AutoPtr<PowerAction> pAction = new PowerAction(this);
            mItems->PushBack(pAction);
        }
        else if (GLOBAL_ACTION_KEY_AIRPLANE.Equals(actionKey))
        {
            //mItems.add(mAirplaneModeOn);
            mItems->PushBack(mAirplaneModeOn);
        }
        else if (GLOBAL_ACTION_KEY_BUGREPORT.Equals(actionKey))
        {
            //if (Settings.Global.getInt(mContext.getContentResolver(),
            //            Settings.Global.BUGREPORT_IN_POWER_MENU, 0) != 0 && isCurrentUserOwner()) {
            //    mItems.add(getBugReportAction());
            //}
            AutoPtr<IContentResolver> contentResolver;
            mContext->GetContentResolver((IContentResolver**)&contentResolver);
            AutoPtr<ISettingsGlobal> settingsGlobal;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
            Int32 value;
            settingsGlobal->GetInt32(contentResolver, ISettingsGlobal::BUGREPORT_IN_POWER_MENU, 0, &value);
            if (value != 0 && IsCurrentUserOwner())
            {
                mItems->PushBack(GetBugReportAction());
            }

        }
        else if (GLOBAL_ACTION_KEY_SILENT.Equals(actionKey))
        {
            if (mShowSilentToggle)
            {
                //mItems.add(mSilentModeAction);
                mItems->PushBack(mSilentModeAction);
            }
        }
        else if (GLOBAL_ACTION_KEY_USERS.Equals(actionKey))
        {
            Boolean proRes;
            SystemProperties::GetBoolean(String("fw.power_user_switcher"), FALSE, &proRes);
            if (proRes)
            {
                AddUsersToMenu(mItems);
            }
        }
        else if (GLOBAL_ACTION_KEY_SETTINGS.Equals(actionKey))
        {
            //mItems.add(getSettingsAction());
            mItems->PushBack(GetSettingsAction());
        }
        else if (GLOBAL_ACTION_KEY_LOCKDOWN.Equals(actionKey))
        {
            //mItems.add(getLockdownAction());
            mItems->PushBack(GetLockdownAction());
        }
        else
        {
            Logger::E(TAG, "Invalid global action key %s", actionKey.string());
        }
        // Add here so we don't add more than one.
        //addedKeys.add(actionKey);
        addedKeys.Insert(actionKey);
    }

    mAdapter = new MyAdapter(this);

    AutoPtr<IAlertControllerAlertParams> params;
    CAlertControllerAlertParams::New(GetUiContext(), (IAlertControllerAlertParams**)&params);
    params->SetAdapter(mAdapter.Get());
    params->SetOnClickListener(this);
    params->SetForceInverseBackground(TRUE);

    AutoPtr<GlobalActionsDialog> dialog = new GlobalActionsDialog(GetUiContext(), params);
    dialog->SetCanceledOnTouchOutside(FALSE); // Handled by the custom class.

    dialog->GetListView()->SetItemsCanFocus(TRUE);
    AutoPtr<IListView> listv = dialog->GetListView();
    IView* vForList = IView::Probe(listv);
    vForList->SetLongClickable(TRUE);
    IAdapterView* aForList = IAdapterView::Probe(listv);
    aForList->SetOnItemLongClickListener(new MyAdapterViewOnItemLongClickListener(this));
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);

    dialog->SetOnDismissListener(this);

    return dialog;
}

void GlobalActions::CreateProfileDialog()
{
    assert(0 && "TODO");
    // final ProfileManager profileManager = (ProfileManager) mContext
    //         .getSystemService(Context.PROFILE_SERVICE);

    // final Profile[] profiles = profileManager.getProfiles();
    // UUID activeProfile = profileManager.getActiveProfile().getUuid();
    // final CharSequence[] names = new CharSequence[profiles.length];

    // int i = 0;
    // int checkedItem = 0;

    // for (Profile profile : profiles) {
    //     if (profile.getUuid().equals(activeProfile)) {
    //         checkedItem = i;
    //         mChosenProfile = profile;
    //     }
    //     names[i++] = profile.getName();
    // }

    // final AlertDialog.Builder ab = new AlertDialog.Builder(getUiContext());

    // AlertDialog dialog = ab.setSingleChoiceItems(names, checkedItem,
    //         new DialogInterface.OnClickListener() {
    //             public void onClick(DialogInterface dialog, int which) {
    //                if (which < 0)
    //                     return;
    //                 mChosenProfile = profiles[which];
    //                 profileManager.setActiveProfile(mChosenProfile.getUuid());
    //                 dialog.cancel();
    //             }
    //         }).create();
    // dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_DIALOG);
    // dialog.show();
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
    //TODO userManager->IsUserSwitcherEnabled(&isSwitchEnabled);
    if (isSwitchEnabled)
    {
        AutoPtr<IList> users;
        userManager->GetUsers((IList**)&users);
        AutoPtr<IUserInfo> currentUser = GetCurrentUser();

        AutoPtr<IIterator> iterator;
        users->GetIterator((IIterator**)&iterator);
        Boolean hasNext = FALSE;
        while (iterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iterator->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
            Boolean supportST;
            user->SupportsSwitchTo(&supportST);
            if (supportST)
            {
                Int32 userId = 0;
                user->GetId(&userId);
                Int32 currentUserId = 0;
                currentUser->GetId(&currentUserId);
                Boolean isCurrentUser = currentUser == NULL
                    ? userId == 0 : (currentUserId == userId);
                String iconPath;
                user->GetIconPath(&iconPath);
                AutoPtr<IDrawable> icon;
                if (iconPath != NULL) {
                    Elastos::Droid::Graphics::Drawable::Drawable::CreateFromPath(iconPath, (IDrawable**)&icon);
                }
                String userName;
                user->GetName(&userName);
                String str = (userName != NULL ? userName : String("Primary"))
                    + (isCurrentUser ? String(" \u2714") : String(""));
                AutoPtr<ICharSequence> value;
                CString::New(str, (ICharSequence**)&value);
                AutoPtr<UserSinglePressAction> switchToUser = new UserSinglePressAction(this,
                        icon, value, userId);
                items->PushBack(switchToUser);
            }
        }
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
    if (mShowSilentToggle)
    {
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
                silentModeOn ? ToggleAction::State::On : ToggleAction::State::Off);
    }
}

ECode GlobalActions::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mShowSilentToggle)
    {
        ECode ec = mContext->UnregisterReceiver(mRingerModeReceiver);
        if (FAILED(ec))
            Logger::W(TAG, "GlobalActions::OnDismiss");
    }
    return NOERROR;
}

ECode GlobalActions::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IInterface> action;
    mAdapter->GetItem(which, (IInterface**)&action);
    //if (action->Probe(EIID_SilentModeTriStateAction) == NULL)
    if (IGlobalActionsSilentModeTriStateAction::Probe(action) == NULL) {
        dialog->Dismiss();
    }
    ((Action*)IGlobalActionsSilentModeTriStateAction::Probe(action))->OnPress();
    return NOERROR;
}

void GlobalActions::OnAirplaneModeChanged()
{
    // Let the service state callbacks handle the state.
    if (mHasTelephony) {
        return;
    }

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Int32 value = 0;
    Boolean airplaneModeOn = (settingsGlobal->GetInt32(
            contentResolver,
            ISettingsGlobal::AIRPLANE_MODE_ON,
            0, &value), value) == 1;
    mAirplaneState = airplaneModeOn ? ToggleAction::State::On : ToggleAction::State::Off;
    mAirplaneModeOn->UpdateState(mAirplaneState);
}

void GlobalActions::ChangeAirplaneModeSystemSetting(
    /* [in] */ Boolean on)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Boolean isSuccess = FALSE;
    settingsGlobal->PutInt32(
            contentResolver,
            ISettingsGlobal::AIRPLANE_MODE_ON,
            on ? 1 : 0, &isSuccess);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutBooleanExtra(String("state"), on);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);

    mContext->SendBroadcastAsUser(intent, all);
    if (!mHasTelephony) {
        mAirplaneState = on ? ToggleAction::State::On : ToggleAction::State::Off;
    }
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
