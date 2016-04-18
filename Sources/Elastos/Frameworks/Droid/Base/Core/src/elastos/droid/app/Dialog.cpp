
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/internal/app/CWindowDecorActionBar.h"
#include "elastos/droid/internal/policy//CPolicyManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Internal::App::IWindowDecorActionBar;
using Elastos::Droid::Internal::App::CWindowDecorActionBar;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::View::EIID_IOnWindowDismissedCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 Dialog::DISMISS;
const Int32 Dialog::CANCEL;
const Int32 Dialog::SHOW;

const String Dialog::TAG("Dialog");
const String Dialog::DIALOG_SHOWING_TAG("android:dialogShowing");
const String Dialog::DIALOG_HIERARCHY_TAG("android:dialogHierarchy");

//==============================================================================
// Dialog::ListenersHandler
//==============================================================================
Dialog::ListenersHandler::ListenersHandler(
    /* [in] */ IDialog* dialog)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(dialog);
    if (wrs) {
        wrs->GetWeakReference((IWeakReference**)&mDialog);
    }
}

ECode Dialog::ListenersHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<IDialogInterface> dialog;
    mDialog->Resolve(EIID_IDialogInterface, (IInterface**)&dialog);

    switch (what) {
        case Dialog::DISMISS: {
            IDialogInterfaceOnDismissListener* listener = IDialogInterfaceOnDismissListener::Probe(obj);
            if (listener) {
                listener->OnDismiss(dialog);
            }
            break;
        }
        case Dialog::CANCEL: {
            IDialogInterfaceOnCancelListener* listener = IDialogInterfaceOnCancelListener::Probe(obj);
            if (listener) {
                listener->OnCancel(dialog);
            }
            break;
        }
        case Dialog::SHOW: {
            IDialogInterfaceOnShowListener* listener = IDialogInterfaceOnShowListener::Probe(obj);
            if (listener) {
                listener->OnShow(dialog);
            }
            break;
        }
    }
    return NOERROR;
}


//==============================================================================
// Dialog::DismissAction
//==============================================================================

Dialog::DismissAction::DismissAction(
    /* [in] */ Dialog* host)
    : mHost(host)
{}

ECode Dialog::DismissAction::Run()
{
    mHost->DismissDialog();

    return NOERROR;
}


//==============================================================================
// Dialog
//==============================================================================

CAR_INTERFACE_IMPL_6(Dialog, Object, IDialog, IDialogInterface, \
    IWindowCallback, IKeyEventCallback, \
    IViewOnCreateContextMenuListener, IOnWindowDismissedCallback)

Dialog::Dialog()
    : mCancelable(TRUE)
    , mCreated(FALSE)
    , mShowing(FALSE)
    , mCanceled(FALSE)
    , mDismissAction(new DismissAction(this))
{}

Dialog::~Dialog()
{
    mWindow->SetCallback(NULL);
    Slogger::V(TAG, ">> destory ~Dialog(): %p", this);
}

ECode Dialog::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, 0, TRUE);
}

ECode Dialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    return constructor(context, theme, TRUE);
}

ECode Dialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ Boolean createContextThemeWrapper)
{
    VALIDATE_NOT_NULL(context);

    if (createContextThemeWrapper) {
        if (theme == 0) {
            theme = GetResourceId(context, R::attr::dialogTheme);
        }
        mContext = NULL;
        CContextThemeWrapper::New(context, theme, (IContext**)&mContext);
    }
    else {
        mContext = context;
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);

    AutoPtr<IPolicyManager> pm;
    CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
    pm->MakeNewWindow(mContext, (IWindow**)&mWindow);

    mWindow->SetCallback(this);
    mWindow->SetOnWindowDismissedCallback(this);
    mWindow->SetWindowManager(mWindowManager, NULL, String(NULL));
    mWindow->SetGravity(IGravity::CENTER);

    mListenersHandler = new ListenersHandler(this);
    CHandler::New((IHandler**)&mHandler);
    assert(mHandler != NULL);

    return NOERROR;
}

ECode Dialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener)
{
    FAIL_RETURN(constructor(context));
    mCancelable = cancelable;
    SetOnCancelListener(cancelListener);

    return NOERROR;
}

Int32 Dialog::GetResourceId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 attrId)
{
    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);
    AutoPtr<IResourcesTheme> resourcesTheme;
    context->GetTheme((IResourcesTheme**)&resourcesTheme);
    Boolean isFounded;
    resourcesTheme->ResolveAttribute(attrId, outValue, TRUE, &isFounded);
    Int32 resourceId;
    outValue->GetResourceId(&resourceId);
    return resourceId;
}

ECode Dialog::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode Dialog::GetActionBar(
    /* [out] */ IActionBar** actionBar)
{
    VALIDATE_NOT_NULL(actionBar)
    *actionBar = mActionBar;
    REFCOUNT_ADD(*actionBar);
    return NOERROR;
}

ECode Dialog::SetOwnerActivity(
    /* [in] */ IActivity* activity)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(activity);
    VALIDATE_NOT_NULL(wrs);

    mWeakOwnerActivity = NULL;
    wrs->GetWeakReference((IWeakReference**)&mWeakOwnerActivity);

    Int32 streamType;
    activity->GetVolumeControlStream(&streamType);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetVolumeControlStream(streamType);
}

ECode Dialog::GetOwnerActivity(
    /* [out] */ IActivity** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IActivity> activity;
    if (mWeakOwnerActivity) {
        mWeakOwnerActivity->Resolve(EIID_IActivity, (IInterface**)&activity);
    }
    *result = activity;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Dialog::IsShowing(
    /* [out] */ Boolean* showing)
{
    VALIDATE_NOT_NULL(showing)
    *showing = mShowing;
    return NOERROR;
}

ECode Dialog::Create()
{
    if (!mCreated) {
        return DispatchOnCreate(NULL);
    }
    return NOERROR;
}

ECode Dialog::Show()
{
    if (mShowing) {
        if (mDecor != NULL) {
            Boolean hasFeature;
            mWindow->HasFeature(IWindow::FEATURE_ACTION_BAR, &hasFeature);
            if (hasFeature) {
                mWindow->InvalidatePanelMenu(IWindow::FEATURE_ACTION_BAR);
            }
            mDecor->SetVisibility(IView::VISIBLE);
        }
        return NOERROR;
    }

    mCanceled = FALSE;

    if (!mCreated) {
        DispatchOnCreate(NULL);
    }

    OnStart();
    mDecor = NULL;
    mWindow->GetDecorView((IView**)&mDecor);

    Boolean hasFeature = (mActionBar == NULL);
    if (hasFeature) {
        mWindow->HasFeature(IWindow::FEATURE_ACTION_BAR, &hasFeature);
    }

    if (hasFeature) {
        AutoPtr<IApplicationInfo> info;
        mContext->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 icon, logo;
        IPackageItemInfo::Probe(info)->GetIcon(&icon);
        IPackageItemInfo::Probe(info)->GetLogo(&logo);
        mWindow->SetDefaultIcon(icon);
        mWindow->SetDefaultLogo(logo);
        CWindowDecorActionBar::New(this, (IActionBar**)&mActionBar);
    }

    AutoPtr<IWindowManagerLayoutParams> l;
    mWindow->GetAttributes((IWindowManagerLayoutParams**)&l);
    Int32 inputMode;
    l->GetSoftInputMode(&inputMode);
    if ((inputMode & IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION) == 0) {
        AutoPtr<IWindowManagerLayoutParams> nl;
        CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&nl);
        Int32 changes;
        nl->CopyFrom(l, &changes);
        Int32 nlInputMode;
        nl->GetSoftInputMode(&nlInputMode);
        nl->SetSoftInputMode(nlInputMode | IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION);
        l = nl;
    }

    IViewManager::Probe(mWindowManager)->AddView(mDecor, IViewGroupLayoutParams::Probe(l));
    mShowing = TRUE;

    SendShowMessage();

    return NOERROR;
}

ECode Dialog::Hide()
{
    if (mDecor != NULL) {
        mDecor->SetVisibility(IView::GONE);
    }

    return NOERROR;
}

ECode Dialog::Dismiss()
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    AutoPtr<ILooper> handlerLooper;
    mHandler->GetLooper((ILooper**)&handlerLooper);
    if (looper == handlerLooper) {
        DismissDialog();
    }
    else {
        Boolean result;
        mHandler->Post(mDismissAction, &result);
    }

    return NOERROR;
}

void Dialog::DismissDialog()
{
    if (mDecor == NULL || !mShowing) {
        return;
    }

    Boolean destroyed;
    mWindow->IsDestroyed(&destroyed);
    if (destroyed) {
        // Slogger::E(TAG, String("Tried to dismissDialog() but the Dialog's window was already destroyed!"));
        return;
    }

    mWindowManager->RemoveViewImmediate(mDecor);
    if (mActionMode != NULL) {
        mActionMode->Finish();
    }
    mDecor = NULL;
    mWindow->CloseAllPanels();
    OnStop();
    mShowing = FALSE;

    SendDismissMessage();
}

void Dialog::SendDismissMessage()
{
    if (mDismissMessage != NULL) {
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(mDismissMessage, (IMessage**)&msg);
        // Obtain a new message so this dialog can be re-used
        msg->SendToTarget();
    }
}

void Dialog::SendShowMessage()
{
    if (mShowMessage != NULL) {
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(mShowMessage, (IMessage**)&msg);
        // Obtain a new message so this dialog can be re-used
        msg->SendToTarget();
    }
}

// internal method to make sure mcreated is set properly without requiring
// users to call through to super in onCreate
ECode Dialog::DispatchOnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    if (!mCreated) {
        OnCreate(savedInstanceState);
        mCreated = TRUE;
    }

    return NOERROR;
}

ECode Dialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return NOERROR;
}

ECode Dialog::OnStart()
{
    if (mActionBar != NULL) mActionBar->SetShowHideAnimationEnabled(TRUE);
    return NOERROR;
}

ECode Dialog::OnStop()
{
    if (mActionBar != NULL) mActionBar->SetShowHideAnimationEnabled(FALSE);
    return NOERROR;
}

ECode Dialog::OnSaveInstanceState(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutBoolean(DIALOG_SHOWING_TAG, mShowing);
    if (mCreated) {
        AutoPtr<IBundle> wBundle;
        mWindow->SaveHierarchyState((IBundle**)&wBundle);
        bundle->PutBundle(DIALOG_HIERARCHY_TAG, wBundle);
    }
    *result = bundle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Dialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    VALIDATE_NOT_NULL(savedInstanceState);

    AutoPtr<IBundle> dialogHierarchyState;
    savedInstanceState->GetBundle(DIALOG_HIERARCHY_TAG, (IBundle**)&dialogHierarchyState);
    if (dialogHierarchyState == NULL) {
        // dialog has never been shown, or onCreated, nothing to restore.
        return NOERROR;
    }

    DispatchOnCreate(savedInstanceState);
    mWindow->RestoreHierarchyState(dialogHierarchyState);
    Boolean show;
    savedInstanceState->GetBoolean(DIALOG_SHOWING_TAG, &show);
    if (show) {
        Show();
    }

    return NOERROR;
}

ECode Dialog::GetWindow(
    /* [out] */ IWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = mWindow;
    REFCOUNT_ADD(*window)
    return NOERROR;
}

ECode Dialog::GetCurrentFocus(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> focus;
    if (mWindow != NULL) {
        mWindow->GetCurrentFocus((IView**)&focus);
    }

    *view = focus;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode Dialog::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    return mWindow->FindViewById(id, view);
}

ECode Dialog::SetContentView(
    /* [in] */ Int32 layoutResID)
{
    return mWindow->SetContentView(layoutResID);
}

ECode Dialog::SetContentView(
    /* [in] */ IView* view)
{
    return mWindow->SetContentView(view);
}

ECode Dialog::SetContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return mWindow->SetContentView(view, params);
}

ECode Dialog::AddContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return mWindow->AddContentView(view, params);
}

ECode Dialog::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mWindow->SetTitle(title);
    AutoPtr<IWindowManagerLayoutParams> l;
    mWindow->GetAttributes((IWindowManagerLayoutParams**)&l);
    l->SetTitle(title);

    return NOERROR;
}

ECode Dialog::SetTitle(
    /* [in] */ Int32 titleId)
{
    AutoPtr<ICharSequence> title;
    mContext->GetText(titleId, (ICharSequence**)&title);

    return SetTitle(title);
}

ECode Dialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        event->StartTracking();
        *result = TRUE;
    }

    return NOERROR;
}

ECode Dialog::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    return NOERROR;
}

ECode Dialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean isTracking;
    Boolean isCanceled;
    if (keyCode == IKeyEvent::KEYCODE_BACK
        && (event->IsTracking(&isTracking), isTracking)
        && !(event->IsCanceled(&isCanceled), isCanceled)) {
        OnBackPressed();
        *result = TRUE;
    }
    return NOERROR;
}

ECode Dialog::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnBackPressed()
{
    if (mCancelable) {
        Cancel();
    }

    return NOERROR;
}

ECode Dialog::OnKeyShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Int32 action;
    Boolean bval;
    mWindow->ShouldCloseOnTouch(mContext, event, &bval);
    if (mCancelable && mShowing && bval) {
        Cancel();
        *result = TRUE;
    }

    return NOERROR;
}

ECode Dialog::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    if (mDecor != NULL) {
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(mDecor,
            IViewGroupLayoutParams::Probe(params));
    }

    return NOERROR;
}

ECode Dialog::OnContentChanged()
{
    return NOERROR;
}

ECode Dialog::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return NOERROR;
}

ECode Dialog::OnAttachedToWindow()
{
    return NOERROR;
}

ECode Dialog::OnDetachedFromWindow()
{
    return NOERROR;
}

ECode Dialog::OnWindowDismissed()
{
    return Dismiss();
}

ECode Dialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Boolean consumed;
    if ((mOnKeyListener != NULL) &&
        (mOnKeyListener->OnKey(this,
            keyCode, event, &consumed), consumed)) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean res;
    mWindow->SuperDispatchKeyEvent(event, &res);
    if (res) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IDispatcherState> state;
    if (mDecor != NULL) {
        mDecor->GetKeyDispatcherState((IDispatcherState**)&state);
    }

    event->Dispatch(this,
        state, TO_IINTERFACE(this), &res);

    *result = res;
    return NOERROR;
}

ECode Dialog::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean bval;
    mWindow->SuperDispatchKeyShortcutEvent(event, &bval);
    if (bval) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 keycode;
    event->GetKeyCode(&keycode);
    return OnKeyShortcut(keycode, event, result);
}

ECode Dialog::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean res;
    mWindow->SuperDispatchTouchEvent(ev, &res);
    if (res) {
        *result = TRUE;
        return NOERROR;
    }

    return OnTouchEvent(ev, result);
}

ECode Dialog::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean res;
    mWindow->SuperDispatchTrackballEvent(ev, &res);
    if (res) {
        *result = TRUE;
        return NOERROR;
    }

    return OnTrackballEvent(ev, result);
}

ECode Dialog::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean res;
    mWindow->SuperDispatchGenericMotionEvent(ev, &res);
    if (res) {
        *result = TRUE;
        return NOERROR;
    }

    return OnGenericMotionEvent(ev, result);
}

ECode Dialog::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String packageName;
    mContext->GetPackageName(&packageName);
    AutoPtr<ICharSequence> pnCsq;
    CString::New(packageName, (ICharSequence**)&pnCsq);
//TODO    event.setClassName(getClass().getName());
    event->SetPackageName(pnCsq);

    AutoPtr<IWindowManagerLayoutParams> params;
    mWindow->GetAttributes((IWindowManagerLayoutParams**)&params);
    Int32 width, height;
    IViewGroupLayoutParams::Probe(params)->GetWidth(&width);
    IViewGroupLayoutParams::Probe(params)->GetHeight(&height);
    Boolean isFullScreen = (width == IViewGroupLayoutParams::MATCH_PARENT) &&
        (height == IViewGroupLayoutParams::MATCH_PARENT);
    IAccessibilityRecord::Probe(event)->SetFullScreen(isFullScreen);

    return NOERROR;
}

ECode Dialog::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    return NOERROR;
}

ECode Dialog::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (featureId == IWindow::FEATURE_OPTIONS_PANEL) {
        return OnCreateOptionsMenu(menu, result);
    }

    return NOERROR;
}

ECode Dialog::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;

    if (featureId == IWindow::FEATURE_OPTIONS_PANEL && menu != NULL) {
        Boolean goforit, hasVisibleItems;
        OnPrepareOptionsMenu(menu, &goforit);
        *result = goforit && (menu->HasVisibleItems(&hasVisibleItems), hasVisibleItems);
    }
    return NOERROR;
}

ECode Dialog::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;

    if (featureId == IWindow::FEATURE_ACTION_BAR) {
        mActionBar->DispatchMenuVisibilityChanged(TRUE);
    }
    return NOERROR;
}

ECode Dialog::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    if (featureId == IWindow::FEATURE_ACTION_BAR) {
        mActionBar->DispatchMenuVisibilityChanged(FALSE);
    }
    return NOERROR;
}

ECode Dialog::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode Dialog::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode Dialog::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnOptionsMenuClosed(
    /* [in] */ IMenu* menu)
{
    return NOERROR;
}

ECode Dialog::OpenOptionsMenu()
{
    return mWindow->OpenPanel(IWindow::FEATURE_OPTIONS_PANEL, NULL);
}

ECode Dialog::CloseOptionsMenu()
{
    return mWindow->ClosePanel(IWindow::FEATURE_OPTIONS_PANEL);
}

ECode Dialog::InvalidateOptionsMenu()
{
    return mWindow->InvalidatePanelMenu(IWindow::FEATURE_OPTIONS_PANEL);
}

ECode Dialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return NOERROR;
}

ECode Dialog::RegisterForContextMenu(
    /* [in] */ IView* view)
{
    return view->SetOnCreateContextMenuListener(this);
}

ECode Dialog::UnregisterForContextMenu(
    /* [in] */ IView* view)
{
    return view->SetOnCreateContextMenuListener(NULL);
}

ECode Dialog::OpenContextMenu(
    /* [in] */ IView* view)
{
    Boolean res;
    return view->ShowContextMenu(&res);
}

ECode Dialog::OnContextItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Dialog::OnContextMenuClosed(
    /* [in] */ IMenu* menu)
{
    return NOERROR;
}

ECode Dialog::Dialog::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    assert(0 && "TODO");
//    final SearchManager searchManager = (SearchManager) mContext
//            .getSystemService(Context_SEARCH_SERVICE);
//
//    // associate search with owner activity
//    final ComponentName appName = getAssociatedActivity();
//    if (appName != NULL && searchManager->GetSearchableInfo(appName) != NULL) {
//        searchManager.startSearch(NULL, FALSE, appName, NULL, FALSE);
//        dismiss();
//        return TRUE;
//    } else {
//        return FALSE;
//    }
    return NOERROR;
}

ECode Dialog::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = NULL;

    if (mActionBar != NULL) {
        AutoPtr<IActionMode> tmp;
        mActionBar->StartActionMode(callback, (IActionMode**)&tmp);
        *mode =  tmp;
        REFCOUNT_ADD(*mode)
    }
    return NOERROR;
}

ECode Dialog::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    mActionMode = mode;
    return NOERROR;
}

ECode Dialog::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    if (mode == mActionMode) {
        mActionMode = NULL;
    }
    return NOERROR;
}

AutoPtr<IComponentName> Dialog::GetAssociatedActivity()
{
    AutoPtr<IActivity> activity;
    GetOwnerActivity((IActivity**)&activity);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    IContextWrapper* wrapper;
    while (activity == NULL && context != NULL) {
        if (IActivity::Probe(context)) {
            activity = IActivity::Probe(context);  // found it!
        }
        else {
            wrapper = IContextWrapper::Probe(context);
            if (wrapper) {// unwrap one level
                AutoPtr<IContext> tempCtx;
                wrapper->GetBaseContext((IContext**)&tempCtx);
                context = tempCtx;
            }
            else {// done
                context = NULL;
            }
        }
    }
    AutoPtr<IComponentName> name;
    if (activity != NULL) {
        activity->GetComponentName((IComponentName**)&name);
    }
    return name;
}

ECode Dialog::TakeKeyEvents(
    /* [in] */ Boolean get)
{
    return mWindow->TakeKeyEvents(get);
}

ECode Dialog::RequestWindowFeature(
    /* [in] */ Int32 featureId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->RequestFeature(featureId, result);
}

ECode Dialog::SetFeatureDrawableResource(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 resId)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFeatureDrawableResource(featureId, resId);
}

ECode Dialog::SetFeatureDrawableUri(
    /* [in] */ Int32 featureId,
    /* [in] */ IUri* uri)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFeatureDrawableUri(featureId, uri);
}

ECode Dialog::SetFeatureDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFeatureDrawable(featureId, drawable);
}

ECode Dialog::SetFeatureDrawableAlpha(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 alpha)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFeatureDrawableAlpha(featureId, alpha);
}

ECode Dialog::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->GetLayoutInflater(inflater);
}

ECode Dialog::SetCancelable(
    /* [in] */ Boolean flag)
{
    mCancelable = flag;

    return NOERROR;
}

ECode Dialog::SetCanceledOnTouchOutside(
    /* [in] */ Boolean cancel)
{
    if (cancel && !mCancelable) {
        mCancelable = TRUE;
    }

    mWindow->SetCloseOnTouchOutside(cancel);

    return NOERROR;
}

ECode Dialog::Cancel()
{
    if (!mCanceled && mCancelMessage != NULL) {
        mCanceled = TRUE;

        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(mCancelMessage, (IMessage**)&msg);
        // Obtain a new message so this dialog can be re-used
        msg->SendToTarget();

    }

    Dismiss();

    return NOERROR;
}

ECode Dialog::SetOnCancelListener(
    /* [in] */ IDialogInterfaceOnCancelListener* listener)
{
    if (mCancelAndDismissTaken != NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException(
        //        "OnCancelListener is already taken by "
        //        + mCancelAndDismissTaken + " and can not be replaced.");
    }

    mCancelMessage = NULL;
    if (listener != NULL) {
        mListenersHandler->ObtainMessage(CANCEL, listener, (IMessage**)&mCancelMessage);
    }
    return NOERROR;
}

ECode Dialog::SetCancelMessage(
    /* [in] */ IMessage* msg)
{
   mCancelMessage = msg;
   return NOERROR;
}

ECode Dialog::SetOnDismissListener(
    /* [in] */ IDialogInterfaceOnDismissListener* listener)
{
    if (mCancelAndDismissTaken != NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException(
        //        "OnDismissListener is already taken by "
        //        + mCancelAndDismissTaken + " and can not be replaced.");
    }

    mDismissMessage = NULL;
    if (listener != NULL) {
        mListenersHandler->ObtainMessage(DISMISS, listener, (IMessage**)&mDismissMessage);
    }
    return NOERROR;
}

ECode Dialog::SetOnShowListener(
    /* [in] */ IDialogInterfaceOnShowListener* listener)
{
    mShowMessage = NULL;
    if (listener != NULL) {
        mListenersHandler->ObtainMessage(SHOW, listener, (IMessage**)&mShowMessage);
    }

    return NOERROR;
}

ECode Dialog::SetDismissMessage(
    /* [in] */ IMessage* msg)
{
   mDismissMessage = msg;
   return NOERROR;
}

 /** @hide */
ECode Dialog::TakeCancelAndDismissListeners(
    /* [in] */ const String& msg,
    /* [in] */ IDialogInterfaceOnCancelListener* cancel,
    /* [in] */ IDialogInterfaceOnDismissListener* dismiss,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mCancelAndDismissTaken != NULL) {
        mCancelAndDismissTaken = NULL;
    }
    else if (mCancelMessage != NULL || mDismissMessage != NULL) {
        return NOERROR;
    }

    SetOnCancelListener(cancel);
    SetOnDismissListener(dismiss);
    mCancelAndDismissTaken = msg;

    *result = TRUE;
    return NOERROR;
}

ECode Dialog::SetVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetVolumeControlStream(streamType);
}

ECode Dialog::GetVolumeControlStream(
    /* [out] */ Int32* volume)
{
    VALIDATE_NOT_NULL(volume)
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->GetVolumeControlStream(volume);
}

ECode Dialog::SetOnKeyListener(
    /* [in] */ IDialogInterfaceOnKeyListener* onKeyListener)
{
    mOnKeyListener = onKeyListener;

    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
