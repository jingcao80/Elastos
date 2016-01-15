
#include "elastos/droid/impl/GlobalActions.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CAlertController.h"
#include "elastos/droid/app/CAlertControllerAlertParams.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/impl/CPhoneWindowManager.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::App::CAlertController;
using Elastos::Droid::Internal::App::CAlertControllerAlertParams;
using Elastos::Droid::Internal::Policy::Impl::CPhoneWindowManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

extern "C" const InterfaceID EIID_SilentModeTriStateAction =
        { 0x7e1e9c2f, 0x50e0, 0x4a07, { 0xb8, 0x2a, 0x5c, 0x73, 0x40, 0x7e, 0xea, 0xe2 } };

const String GlobalActions::TAG("GlobalActions");

const Boolean GlobalActions::SHOW_SILENT_TOGGLE = TRUE;

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

CAR_INTERFACE_IMPL(GlobalActions::MyAdapter, IBaseAdapter)

IBASEADAPTER_METHODS_IMPL(GlobalActions::MyAdapter, BaseAdapter)

ECode GlobalActions::MyAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    AutoPtr<Action> action;
    GetItem(position, (IInterface**)&action);
    *enabled = action->IsEnabled();
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
            *item = action;
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
    *itemId = position;
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    *hasStableIds = BaseAdapter::HasStableIds();
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
    AutoPtr<Action> action;
    GetItem(position, (IInterface**)&action);
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
    AutoPtr<IView> viewTemp = action->Create(mHost->mContext, convertView, parent, inflater);
    *view = viewTemp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    *viewType = BaseAdapter::GetItemViewType(position);
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    *count = BaseAdapter::GetViewTypeCount();
    return NOERROR;
}

CARAPI GlobalActions::MyAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    *isEmpty = BaseAdapter::IsEmpty();
    return NOERROR;
}


ECode GlobalActions::Action::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

PInterface GlobalActions::Action::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

UInt32 GlobalActions::Action::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GlobalActions::Action::Release()
{
    return ElRefBase::Release();
}


GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 messageResId)
    : mIconResId(iconResId)
    , mIcon(NULL)
    , mMessageResId(messageResId)
    , mMessage(NULL)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ Int32 iconResId,
    /* [in] */ AutoPtr<IDrawable> icon,
    /* [in] */ AutoPtr<ICharSequence> message)
    : mIconResId(iconResId)
    , mIcon(icon)
    , mMessageResId(0)
    , mMessage(message)
{
}

GlobalActions::SinglePressAction::SinglePressAction(
    /* [in] */ Int32 iconResId,
    /* [in] */ AutoPtr<ICharSequence> message)
    : mIconResId(iconResId)
    , mIcon(NULL)
    , mMessageResId(0)
    , mMessage(message)
{
}

Boolean GlobalActions::SinglePressAction::IsEnabled()
{
    return TRUE;
}

Boolean GlobalActions::SinglePressAction::OnLongPress()
{
    return FALSE;
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
    viewTemp->SetVisibility(IView::GONE);
    if (mIcon != NULL) {
        icon->SetImageDrawable(mIcon);
        icon->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_CENTER_CROP);
    }
    else if (mIconResId != 0) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IDrawable> drawable;
        resources->GetDrawable(mIconResId, (IDrawable**)&drawable);
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
        messageView->SetEnabled(enabled);
    }

    Boolean on = ((mState == State::On) || (mState == State::TurningOn));
    if (icon != NULL) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IDrawable> drawable;
        resources->GetDrawable((on ? mEnabledIconResId : mDisabledIconResid), (IDrawable**)&drawable);
        icon->SetImageDrawable(drawable);
        icon->SetEnabled(enabled);
    }

    if (statusView != NULL) {
        statusView->SetText(on ? mEnabledStatusMessageResId : mDisabledStatusMessageResId);
        statusView->SetVisibility(IView::VISIBLE);
        statusView->SetEnabled(enabled);
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

Boolean GlobalActions::ToggleAction::OnLongPress()
{
    return FALSE;
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
    /* [in] */ GlobalActions* host)
    : ToggleAction(
            R::drawable::ic_audio_vol_mute,
            R::drawable::ic_audio_vol,
            R::string::global_action_toggle_silent_mode,
            R::string::global_action_silent_mode_on_status,
            R::string::global_action_silent_mode_off_status)
    , mHost(host)
{
}

void GlobalActions::SilentModeToggleAction::OnToggle(
    /* [in] */ Boolean on)
{
    if (on) {
        mHost->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_SILENT);
    } else {
        mHost->mAudioManager->SetRingerMode(IAudioManager::RINGER_MODE_NORMAL);
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

UInt32 GlobalActions::SilentModeTriStateAction::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GlobalActions::SilentModeTriStateAction::Release()
{
    return ElRefBase::Release();
}

ECode GlobalActions::SilentModeTriStateAction::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);
    if (pObject == (IInterface*)(IViewOnClickListener *)this) {
        *pIID = EIID_IViewOnClickListener;
    } else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

PInterface GlobalActions::SilentModeTriStateAction::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    if (riid == EIID_SilentModeTriStateAction) {
        return reinterpret_cast<PInterface>(this);
    }
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

Boolean GlobalActions::SilentModeTriStateAction::OnLongPress()
{
    return FALSE;
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


GlobalActions::GlobalActionsDialog::GlobalActionsDialog(
    /* [in] */ IContext* context,
    /* [in] */ IAlertControllerAlertParams* params)
    : Dialog(context, GetDialogTheme(context))
    , mContext(context)
{
    CAlertController::New(mContext, this, GetWindow(), (IAlertController**)&mAlert);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfiguration;
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);
    viewConfiguration->GetScaledWindowTouchSlop(&mWindowTouchSlop);
    params->Apply(mAlert);
}

CAR_INTERFACE_IMPL(GlobalActions::GlobalActionsDialog, IDialogInterface);

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
            AutoPtr<IWindow> window = GetWindow();
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
            mIntercepted = mEnableAccessibilityController->OnInterceptTouchEvent(event);
            if (mIntercepted) {
                Int64 now = SystemClock::GetUptimeMillis();
                AutoPtr<IMotionEventHelper> helper;
                CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
                event = NULL;
                helper->Obtain(now, now,
                    IMotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0, (IMotionEvent**)&event);
                event->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
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
            return mEnableAccessibilityController->OnTouchEvent(event);
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
    return Dialog::DispatchTouchEvent(event);
}

AutoPtr<IListView> GlobalActions::GlobalActionsDialog::GetListView()
{
    AutoPtr<IListView> listView;
    mAlert->GetListView((IListView**)&listView);
    return listView;
}

Boolean GlobalActions::GlobalActionsDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Boolean isOnKeyDown = FALSE;
    if (mAlert->OnKeyDown(keyCode, event, &isOnKeyDown), isOnKeyDown) {
        return TRUE;
    }
    return Dialog::OnKeyDown(keyCode, event);
}

Boolean GlobalActions::GlobalActionsDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Boolean isOnKeyUp = FALSE;
    if (mAlert->OnKeyUp(keyCode, event, &isOnKeyUp), isOnKeyUp) {
        return TRUE;
    }
    return Dialog::OnKeyUp(keyCode, event);
}

void GlobalActions::GlobalActionsDialog::OnStart()
{
    // If global accessibility gesture can be performed, we will take care
    // of dismissing the dialog on touch outside. This is because the dialog
    // is dismissed on the first down while the global gesture is a long press
    // with two fingers anywhere on the screen.

    if (EnableAccessibilityController::CanEnableAccessibilityViaGesture(mContext)) {
        mEnableAccessibilityController = new EnableAccessibilityController(mContext);
        Dialog::SetCanceledOnTouchOutside(FALSE);
    } else {
        mEnableAccessibilityController = NULL;
        Dialog::SetCanceledOnTouchOutside(TRUE);
    }
    Dialog::OnStart();
}

void GlobalActions::GlobalActionsDialog::OnStop()
{
    if (mEnableAccessibilityController != NULL) {
        mEnableAccessibilityController->OnDestroy();
    }
    Dialog::OnStop();
}

void GlobalActions::GlobalActionsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);
    mAlert->InstallContent();
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
    }
    // TODO: TelephonyIntents is not implement.
    // else if (TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED.equals(action)) {
    //     // Airplane mode can be changed after ECM exits if airplane toggle button
    //     // is pressed during ECM mode
    //     if (!(intent.getBooleanExtra("PHONE_IN_ECM_STATE", false)) &&
    //             mIsWaitingForEcmExit) {
    //         mIsWaitingForEcmExit = false;
    //         changeAirplaneModeSystemSetting(true);
    //     }
    // }
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
    : ContentObserver(handler)
    , mHost(host)
{
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
    /* [in] */ GlobalActions* host)
    : ToggleAction(
            R::drawable::ic_lock_airplane_mode,
            R::drawable::ic_lock_airplane_mode_off,
            R::string::global_actions_toggle_airplane_mode,
            R::string::global_actions_airplane_mode_on_status,
            R::string::global_actions_airplane_mode_off_status)
{
}

void GlobalActions::AirplaneModeOn::OnToggle(
    /* [in] */ Boolean on)
{
    // TODO: TelephonyIntents is not implement.
    // if (mHost->mHasTelephony && Boolean.parseBoolean(
    //         SystemProperties::GetBoolean(TelephonyProperties.PROPERTY_INECM_MODE))) {
    //     mHost->mIsWaitingForEcmExit = TRUE;
        // Launch ECM exit dialog
        // Intent ecmDialogIntent =
        //         new Intent(TelephonyIntents.ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, null);
        // ecmDialogIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        // mContext.startActivity(ecmDialogIntent);
    // } else {
    //     mHost->ChangeAirplaneModeSystemSetting(on);
    // }
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
    if (!mHost->mHasTelephony) {
        return;
    }

    // In ECM mode airplane state cannot be changed
    // TODO: TelephonyProperties is not impelement.
    // if (!(Boolean.parseBoolean(
    //         SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE)))) {
    //     mState = buttonOn ? State.TurningOn : State.TurningOff;
    //     mAirplaneState = mState;
    // }
}


GlobalActions::PowerSinglePressAction::PowerSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(R::drawable::ic_lock_power_off,
            R::string::global_action_power_off)
    , mHost(host)
{
}

void GlobalActions::PowerSinglePressAction::OnPress()
{
    // shutdown by making sure radio and power are handled accordingly.
    mHost->mWindowManagerFuncs->Shutdown(TRUE);
}

Boolean GlobalActions::PowerSinglePressAction::OnLongPress()
{
    mHost->mWindowManagerFuncs->RebootSafeMode(TRUE);
    return TRUE;
}

Boolean GlobalActions::PowerSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::PowerSinglePressAction::ShowBeforeProvisioning()
{
    return TRUE;
}


ECode GlobalActions::BugRunnable::Run()
{
    // try {
    ActivityManagerNative::GetDefault()->RequestBugReport();
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}


GlobalActions::DialogOnClickListener::DialogOnClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(GlobalActions::DialogOnClickListener, IDialogInterfaceOnClickListener);

ECode GlobalActions::DialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // Add a little delay before executing, to give the
    // dialog a chance to go away before it takes a
    // screenshot.
    Boolean isSuccess = FALSE;
    AutoPtr<IRunnable> bugRunnable = new BugRunnable();
    return mHost->mHandler->PostDelayed(bugRunnable, 500, &isSuccess);
}


GlobalActions::BugSinglePressAction::BugSinglePressAction(
    /* [in] */ GlobalActions* host)
    : SinglePressAction(R::drawable::stat_sys_adb,
            R::string::global_action_bug_report)
    , mHost(host)
{
}

void GlobalActions::BugSinglePressAction::OnPress()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::bugreport_title);
    builder->SetMessage(R::string::bugreport_message);
    builder->SetNegativeButton(R::string::cancel, NULL);
    builder->SetPositiveButton(R::string::report,
            new DialogOnClickListener(mHost));
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    dialog->Show();
}

Boolean GlobalActions::BugSinglePressAction::OnLongPress()
{
    return FALSE;
}

Boolean GlobalActions::BugSinglePressAction::ShowDuringKeyguard()
{
    return TRUE;
}

Boolean GlobalActions::BugSinglePressAction::ShowBeforeProvisioning()
{
    return FALSE;
}


GlobalActions::MyAdapterViewOnItemLongClickListener::MyAdapterViewOnItemLongClickListener(
    /* [in] */ GlobalActions* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(GlobalActions::MyAdapterViewOnItemLongClickListener, IAdapterViewOnItemClickListener);

ECode GlobalActions::MyAdapterViewOnItemLongClickListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    AutoPtr<Action> action;
    mHost->mAdapter->GetItem(position, (IInterface**)&action);
    *result = action->OnLongPress();
    return NOERROR;
}


GlobalActions::UserSinglePressAction::UserSinglePressAction(
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* message,
    /* [in] */ Int32 userId)
    : SinglePressAction(R::drawable::ic_menu_cc, icon, message)
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


GlobalActions::GlobalActions(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs)
{
    // Init global variable of inner class.
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    // TODO: PhoneStateListener is not implement.
    // PhoneStateListener mPhoneStateListener;
    mRingerModeReceiver = new RingerModeReceiver(this);
    mHandler = new DialogHandler(this);
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
    mDreamManager = IDreamManager::Probe(obj);

    // receive broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    // TODO: Telephone is not implement.
    // filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    // get notified of phone state changes
    // TODO: Telephone is not implement.
    // TelephonyManager telephonyManager =
    //         (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    // telephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_SERVICE_STATE);
    obj = NULL;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    // mHasTelephony = cm.isNetworkSupported(ConnectivityManager.TYPE_MOBILE);

    mHasTelephony = FALSE;
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
}

CAR_INTERFACE_IMPL_2(GlobalActions, IDialogInterfaceOnDismissListener, IDialogInterfaceOnClickListener)

void GlobalActions::ShowDialog(
    /* [in] */ Boolean keyguardShowing,
    /* [in] */ Boolean isDeviceProvisioned)
{
    mKeyguardShowing = keyguardShowing;
    mDeviceProvisioned = isDeviceProvisioned;
    if (mDialog != NULL) {
        mDialog->Dismiss();
        mDialog = NULL;
        // Show delayed, so that the dismiss of the previous dialog completes
        AutoPtr<IMessage> message;
        mHandler->ObtainMessage(MESSAGE_SHOW, (IMessage**)&message);
        Boolean isSuccess = FALSE;
        mHandler->SendMessage(message, &isSuccess);
    } else {
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
    mDialog = CreateDialog();
    PrepareDialog();

    AutoPtr<IWindow> window = mDialog->GetWindow();
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

    // first: power off
    AutoPtr<PowerSinglePressAction> powerSinglePressAction = new PowerSinglePressAction(this);
    mItems->PushBack(powerSinglePressAction);

    // next: airplane mode
    mItems->PushBack(mAirplaneModeOn);

    // next: bug report, if enabled
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value = 0;
    settingsSecure->GetInt32(contentResolver,
            ISettingsSecure::BUGREPORT_IN_POWER_MENU, 0, &value);
    if (value != 0) {
        AutoPtr<BugSinglePressAction> bugSinglePressAction = new BugSinglePressAction(this);
        mItems->PushBack(bugSinglePressAction);
    }

    // last: silent mode
    if (SHOW_SILENT_TOGGLE) {
        mItems->PushBack(mSilentModeAction);
    }

    // one more thing: optionally add a list of users to switch to
    if (SystemProperties::GetBoolean(String("fw.power_user_switcher"), FALSE)) {
        AddUsersToMenu(mItems);
    }

    mAdapter = new MyAdapter(this);

    AutoPtr<IAlertControllerAlertParams> params;
    CAlertControllerAlertParams::New(mContext, (IAlertControllerAlertParams**)&params);
    params->SetAdapter(mAdapter.Get());
    params->SetOnClickListener(this);
    params->SetForceInverseBackground(TRUE);

    AutoPtr<GlobalActionsDialog> dialog = new GlobalActionsDialog(mContext, params);
    dialog->SetCanceledOnTouchOutside(FALSE); // Handled by the custom class.

    dialog->GetListView()->SetItemsCanFocus(TRUE);
    dialog->GetListView()->SetLongClickable(TRUE);
    dialog->GetListView()->SetOnItemLongClickListener(new MyAdapterViewOnItemLongClickListener(this));
    AutoPtr<IWindow> window = dialog->GetWindow();
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);

    dialog->SetOnDismissListener(this);

    return dialog;
}

void GlobalActions::AddUsersToMenu(
    /* [in] */ List< AutoPtr<Action> >* items)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    AutoPtr<IObjectContainer> users;
    userManager->GetUsers((IObjectContainer**)&users);

    Int32 size = 0;
    if ((users->GetObjectCount(&size), size) > 1) {
        AutoPtr<IUserInfo> currentUser;
        // try {
        ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            currentUser = NULL;
        }
        // }
        AutoPtr<IObjectEnumerator> enumerator;
        users->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            enumerator->Current((IInterface**)&obj);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
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
            AutoPtr<UserSinglePressAction> switchToUser = new UserSinglePressAction(
                    icon, value, userId);
            items->PushBack(switchToUser);
        }
    }
}

void GlobalActions::PrepareDialog()
{
    RefreshSilentMode();
    mAirplaneModeOn->UpdateState(mAirplaneState);
    mAdapter->NotifyDataSetChanged();
    AutoPtr<IWindow> window = mDialog->GetWindow();
    window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    if (SHOW_SILENT_TOGGLE) {
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
    if (SHOW_SILENT_TOGGLE) {
        mContext->UnregisterReceiver(mRingerModeReceiver);
    }
    return NOERROR;
}

ECode GlobalActions::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<Action> action;
    mAdapter->GetItem(which, (IInterface**)&action);
    if (action->Probe(EIID_SilentModeTriStateAction) == NULL) {
        dialog->Dismiss();
    }
    action->OnPress();
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


} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl
