#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/impl/CPhoneWindow.h"
#include "elastos/droid/impl/CPhoneWindowManager.h"
#include "elastos/droid/impl/CPhoneWindowSavedState.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPixelFormat.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/CKeyCharacterMapHelper.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/StandaloneActionMode.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/view/menu/CContextMenuBuilder.h"
#include "elastos/droid/view/menu/CMenuBuilder.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/droid/widget/internal/CActionBarContextView.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/impl/CPhoneWindowRotationWatcher.h"
#include "elastos/droid/R.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::R;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::CPixelFormat;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IMenu;
using Elastos::Droid::View::EIID_IRootViewSurfaceTaker;
using Elastos::Droid::View::EIID_IRotationWatcher;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::EIID_IPhoneWindowRotationWatcher;
using Elastos::Droid::View::Menu::CContextMenuBuilder;
using Elastos::Droid::View::Menu::CMenuBuilder;
using Elastos::Droid::View::Menu::EIID_IMenuBuilder;
using Elastos::Droid::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::View::Menu::IMenuBuilder;
using Elastos::Droid::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::EIID_IFrameLayout;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::Internal::CActionBarContextView;
using Elastos::Droid::Widget::Internal::IActionBarContainer;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

// {45CF35EA-34D4-43a3-A1C7-2649411BAA61}
extern "C" const InterfaceID EIID_RootViewSurfaceTaker =
    {0x45cf35ea, 0x34d4, 0x43a3, {0xa1, 0xc7, 0x26, 0x49, 0x41, 0x1b, 0xaa, 0x61}};

// {eb327183-9b79-488c-b888-4b96d70cc2f5}
extern "C" const InterfaceID EIID_CPhoneWindow =
        {0xeb327183,0x9b79,0x488c,{0xb8,0x88,0x4b,0x96,0xd7,0x0c,0xc2,0xf5}};

AutoPtr<IIWindowManager> CPhoneWindow::WindowManagerHolder::sWindowManager;

String CPhoneWindow::TAG("PhoneWindow");
const Boolean CPhoneWindow::SWEEP_OPEN_MENU = FALSE;

const String CPhoneWindow::FOCUSED_ID_TAG = String("android:focusedViewId");
const String CPhoneWindow::VIEWS_TAG = String("android:views");
const String CPhoneWindow::PANELS_TAG = String("android:Panels");
const String CPhoneWindow::ACTION_BAR_TAG = String("android:ActionBar");

static AutoPtr<IPhoneWindowRotationWatcher> InitStaticWatcher()
{
    AutoPtr<CPhoneWindowRotationWatcher> tmp;
    CPhoneWindowRotationWatcher::NewByFriend((CPhoneWindowRotationWatcher**)&tmp);
    return (IPhoneWindowRotationWatcher*)tmp.Get();
}

AutoPtr<IPhoneWindowRotationWatcher> CPhoneWindow::sRotationWatcher = InitStaticWatcher();

CPhoneWindow::_DecorView::ShowActionModePopupRunnable::ShowActionModePopupRunnable(
    /* [in] */ _DecorView* host)
    : mHost(host)
{
}

ECode CPhoneWindow::_DecorView::ShowActionModePopupRunnable::Run()
{
    AutoPtr<IBinder> token;
    mHost->mActionModeView->GetApplicationWindowToken((IBinder**)&token);
    return mHost->mActionModePopup->ShowAtLocation(token,
            IGravity::TOP | IGravity::FILL_HORIZONTAL, 0, 0);
}

CAR_INTERFACE_IMPL(CPhoneWindow::_DecorView::ActionModeCallbackWrapper, IActionModeCallback)

CPhoneWindow::_DecorView::ActionModeCallbackWrapper::ActionModeCallbackWrapper(
    /* [in] */ IActionModeCallback* wrapped,
    /* [in] */ _DecorView* host)
    : mWrapped(wrapped)
    , mHost(host)
{
}

ECode CPhoneWindow::_DecorView::ActionModeCallbackWrapper::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    return mWrapped->OnCreateActionMode(mode, menu, result);
}

ECode CPhoneWindow::_DecorView::ActionModeCallbackWrapper::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result) {
    return mWrapped->OnPrepareActionMode(mode, menu, result);
}

ECode CPhoneWindow::_DecorView::ActionModeCallbackWrapper::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mWrapped->OnActionItemClicked(mode, item, result);
}

ECode CPhoneWindow::_DecorView::ActionModeCallbackWrapper::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    mWrapped->OnDestroyActionMode(mode);
    if (mHost->mActionModePopup != NULL) {
        mHost->RemoveCallbacks(mHost->mShowActionModePopup);
        mHost->mActionModePopup->Dismiss();
    } else if (mHost->mActionModeView != NULL) {
        mHost->mActionModeView->SetVisibility(IView::GONE);
    }
    if (mHost->mActionModeView != NULL) {
        mHost->mActionModeView->RemoveAllViews();
    }

    AutoPtr<IWindowCallback> cb = mHost->GetCallback();
    if (cb != NULL && !mHost->IsDestroyed()) {
        // try {
        cb->OnActionModeFinished(mHost->mActionMode);
        // } catch (AbstractMethodError ame) {
        //     // Older apps might not implement this callback method.
        // }
    }

    mHost->mActionMode = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL_LIGHT(CPhoneWindow::_DecorView::DecorViewWeakReferenceImpl, IWeakReference)

CPhoneWindow::_DecorView::DecorViewWeakReferenceImpl::DecorViewWeakReferenceImpl(
    /* [in] */ IInterface* object,
    /* [in] */ ElRefBase::WeakRefType* ref)
    : mObject(object)
    , mRef(ref)
{}

CPhoneWindow::_DecorView::DecorViewWeakReferenceImpl::~DecorViewWeakReferenceImpl()
{
    if (mRef) mRef->DecWeak(this);
}

ECode CPhoneWindow::_DecorView::DecorViewWeakReferenceImpl::Resolve(
    /* [in] */ const InterfaceID& riid,
    /* [out] */ IInterface** objectReference)
{
    *objectReference = NULL;
    if (mObject && mRef && mRef->AttemptIncStrong(objectReference)) {
        *objectReference = mObject->Probe(riid);
        REFCOUNT_ADD(*objectReference);
        ((DecorView*)(IFrameLayout*)mObject)->_Release();
    }
    return NOERROR;
}

CPhoneWindow::_DecorView::_DecorView(
    /* [in] */ CPhoneWindow* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 featureId)
    : FrameLayout(context)
    , mHost(host)
    , mFeatureId(featureId)
    , mChanging(FALSE)
    , mWatchingForMenu(FALSE)
    , mDownY(0)
    , mDefaultOpacity(IPixelFormat::OPAQUE)
{
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mDrawingBounds));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mBackgroundPadding));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mFramePadding));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mFrameOffsets));
}

ECode CPhoneWindow::_DecorView::WillYouTakeTheSurface(
    /* [out] */ ISurfaceHolderCallback2** cback)
{
    VALIDATE_NOT_NULL(cback);
    if (mFeatureId < 0) {
        *cback = mHost->mTakeSurfaceCallback;
        REFCOUNT_ADD(*cback);
    }
    else {
        *cback = NULL;
    }

    return NOERROR;
}

ECode CPhoneWindow::_DecorView::SetSurfaceType(
    /* [in] */ Int32 type)
{
    return mHost->SetType(type);
}

ECode CPhoneWindow::_DecorView::SetSurfaceFormat(
    /* [in] */ Int32 format)
{
    return mHost->SetFormat(format);
}

ECode CPhoneWindow::_DecorView::SetSurfaceKeepScreenOn(
    /* [in] */ Boolean keepOn)
{
    if (keepOn) {
        mHost->AddFlags(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON);
    }
    else {
        mHost->ClearFlags(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON);
    }

    return NOERROR;
}

ECode CPhoneWindow::_DecorView::WillYouTakeTheInputQueue(
    /* [out] */ IInputQueueCallback** inputQueueCallback)
{
    VALIDATE_NOT_NULL(inputQueueCallback);
    if (mFeatureId < 0) {
        *inputQueueCallback = mHost->mTakeInputQueueCallback;
        REFCOUNT_ADD(*inputQueueCallback);
    }
    else {
        *inputQueueCallback = NULL;
    }

    return NOERROR;
}

void CPhoneWindow::_DecorView::StartChanging()
{
    mChanging = TRUE;
}

void CPhoneWindow::_DecorView::FinishChanging()
{
    mChanging = FALSE;
    DrawableChanged();
}

void CPhoneWindow::_DecorView::SetWindowBackground(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IDrawable> bg = GetBackground();
    if (bg.Get() != drawable) {
        SetBackgroundDrawable(drawable);
        if (drawable != NULL) {
            Boolean result;
            drawable->GetPadding((IRect*)mBackgroundPadding.Get(), &result);
        } else {
            mBackgroundPadding->SetEmpty();
        }
        DrawableChanged();
    }
}

ECode CPhoneWindow::_DecorView::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    FrameLayout::SetBackgroundDrawable(d);
    AutoPtr<IBinder> token = GetWindowToken();
    if (token != NULL) {
        UpdateWindowResizeState();
    }

    return NOERROR;
}

void CPhoneWindow::_DecorView::SetWindowFrame(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IDrawable> bg = GetBackground();
    if (bg.Get() != drawable) {
        SetForeground(drawable);
        if (drawable != NULL) {
            Boolean result;
            drawable->GetPadding((IRect*)mFramePadding.Get(), &result);
        } else {
            mFramePadding->SetEmpty();
        }
        DrawableChanged();
    }
}

Boolean CPhoneWindow::_DecorView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 keyCode, action;
    event->GetKeyCode(&keyCode);
    event->GetAction(&action);
    Boolean isDown = action == IKeyEvent::ACTION_DOWN;

    Int32 repeat = 0;

    if (isDown && (event->GetRepeatCount(&repeat), repeat) == 0) {
        // First handle chording of panel key: if a panel key is held
        // but not released, try to execute a shortcut in it.
        if ((mHost->mPanelChordingKey > 0) && (mHost->mPanelChordingKey != keyCode)) {
            Boolean handled = DispatchKeyShortcutEvent(event);
            if (handled) {
                return TRUE;
            }
        }

        // If a panel is open, perform a shortcut on it without the
        // chorded panel key
        if ((mHost->mPreparedPanel != NULL) && mHost->mPreparedPanel->mIsOpen) {
            if (mHost->PerformPanelShortcut(mHost->mPreparedPanel, keyCode, event, 0)) {
                return TRUE;
            }
        }
    }

    if (!IsDestroyed()) {
        AutoPtr<IWindowCallback> cb;
        mHost->GetCallback((IWindowCallback**)&cb);

        Boolean handled = FALSE;
        if (cb != NULL && mFeatureId < 0) {
            cb->DispatchKeyEvent(event, &handled);
        }
        else {
            handled = FrameLayout::DispatchKeyEvent(event);
        }

        if (handled) {
            return TRUE;
        }
    }

    return isDown ? mHost->OnKeyDown(mFeatureId, keyCode, event)
            : mHost->OnKeyUp(mFeatureId, keyCode, event);
}

Boolean CPhoneWindow::_DecorView::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* ev)
{
    // If the panel is already prepared, then perform the shortcut using it.
    Boolean handled = FALSE;
    if (mHost->mPreparedPanel != NULL) {
        Int32 keyCode = 0;
        ev->GetKeyCode(&keyCode);
        handled = mHost->PerformPanelShortcut(mHost->mPreparedPanel, keyCode, ev,
                IMenu::FLAG_PERFORM_NO_CLOSE);
        if (handled) {
            if (mHost->mPreparedPanel != NULL) {
                mHost->mPreparedPanel->mIsHandled = TRUE;
            }
            return TRUE;
        }
    }

    // Shortcut not handled by the panel.  Dispatch to the view hierarchy.
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchKeyShortcutEvent(ev, &handled);
    }
    else {
        handled = FrameLayout::DispatchKeyShortcutEvent(ev);
    }

    if (handled) {
        return TRUE;
    }

    // If the panel is not prepared, then we may be trying to handle a shortcut key
    // combination such as Control+C.  Temporarily prepare the panel then mark it
    // unprepared again when finished to ensure that the panel will again be prepared
    // the next time it is shown for real.
    if (mHost->mPreparedPanel == NULL) {
        AutoPtr<PanelFeatureState> st;
        ECode ec = mHost->GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);
        if (FAILED(ec)) return FALSE;

        Boolean tmp = FALSE;
        mHost->PreparePanel(st, ev, &tmp);

        Int32 keyCode = 0;
        ev->GetKeyCode(&keyCode);
        handled = mHost->PerformPanelShortcut(st, keyCode, ev,
                IMenu::FLAG_PERFORM_NO_CLOSE);
        st->mIsPrepared = FALSE;
        if (handled) {
            return TRUE;
        }
    }

    return FALSE;
}

Boolean CPhoneWindow::_DecorView::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    Boolean handled = FALSE;
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchTouchEvent(event, &handled);
    }
    else {
        handled = FrameLayout::DispatchTouchEvent(event);
    }

    return handled;
}

Boolean CPhoneWindow::_DecorView::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    Boolean handled = FALSE;
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchTrackballEvent(event, &handled);
    }
    else {
        handled = FrameLayout::DispatchTrackballEvent(event);
    }

    return handled;
}

Boolean CPhoneWindow::_DecorView::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        Boolean ret = FALSE;
        cb->DispatchGenericMotionEvent(ev, &ret);
        return ret;
    }

    return FrameLayout::DispatchGenericMotionEvent(ev);
}

Boolean CPhoneWindow::_DecorView::SuperDispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return FrameLayout::DispatchKeyEvent(event);
}

Boolean CPhoneWindow::_DecorView::SuperDispatchTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return FrameLayout::DispatchTouchEvent(event);
}

Boolean CPhoneWindow::_DecorView::SuperDispatchTrackballEvent(
    /* [in] */ IMotionEvent* event)
{
    return FrameLayout::DispatchTrackballEvent(event);
}

Boolean CPhoneWindow::_DecorView::SuperDispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return FrameLayout::DispatchGenericMotionEvent(event);
}

Boolean CPhoneWindow::_DecorView::SuperDispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event)
{
    return FrameLayout::DispatchKeyShortcutEvent(event);
}

ECode CPhoneWindow::_DecorView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = OnInterceptTouchEvent(event);
    return NOERROR;
}

Boolean CPhoneWindow::_DecorView::IsOutOfBounds(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return x < -5 || y < -5 || x > (FrameLayout::GetWidth() + 5)
        || y > (FrameLayout::GetHeight() + 5);
}

AutoPtr<IWindowCallback> CPhoneWindow::_DecorView::GetCallback()
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    return cb;
}

Boolean CPhoneWindow::_DecorView::IsDestroyed()
{
    Boolean destoryed = FALSE;
    return (mHost->IsDestroyed(&destoryed), destoryed);
}

Boolean CPhoneWindow::_DecorView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    if (mFeatureId >= 0) {
        if (action == IMotionEvent::ACTION_DOWN) {
            Float fx, fy;
            event->GetX(&fx);
            event->GetY(&fy);
            Int32 x = (Int32)fx;
            Int32 y = (Int32)fy;
            if (IsOutOfBounds(x, y)) {
                mHost->ClosePanel(mFeatureId);
                return TRUE;
            }
        }
    }

    if (!CPhoneWindow::SWEEP_OPEN_MENU) {
        return FALSE;
    }

    Float fy;
    event->GetY(&fy);
    if (mFeatureId >= 0) {
        if (action == IMotionEvent::ACTION_DOWN) {
            // Logger::D(TAG, "Watchiing!");
            mWatchingForMenu = TRUE;
            mDownY = (Int32)fy;
            return FALSE;
        }

        if (!mWatchingForMenu) {
            return FALSE;
        }

        Int32 y = (Int32)fy;
        if (action == IMotionEvent::ACTION_MOVE) {
            if (y > (mDownY + 30)) {
                // Logger::D(TAG, "Closing!");
                mHost->ClosePanel(mFeatureId);
                mWatchingForMenu = FALSE;
                return TRUE;
            }
        }
        else if (action == IMotionEvent::ACTION_UP) {
            mWatchingForMenu = FALSE;
        }

        return FALSE;
    }

    if (action == IMotionEvent::ACTION_DOWN) {
        Int32 y = fy;
        Boolean hasChildren;
        mHost->HasChildren(&hasChildren);
        if (y >= (FrameLayout::GetHeight()-5) && !hasChildren) {
            // Logger::D(TAG, "Watchiing!");
            mWatchingForMenu = TRUE;
        }
        return FALSE;
    }

    if (!mWatchingForMenu) {
        return FALSE;
    }

    Int32 y = fy;
    if (action == IMotionEvent::ACTION_MOVE) {
        if (y < (FrameLayout::GetHeight()-30)) {
            // Logger::D(TAG, "Opening!");
            AutoPtr<IKeyEvent> event;
            CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_MENU, (IKeyEvent**)&event);
            mHost->OpenPanel(IWindow::FEATURE_OPTIONS_PANEL, event);

            mWatchingForMenu = FALSE;
            return TRUE;
        }
    }
    else if (action == IMotionEvent::ACTION_UP) {
        mWatchingForMenu = FALSE;
    }

    return FALSE;
}

ECode CPhoneWindow::_DecorView::SendAccessibilityEvent(
    /* [in] */ Int32 eventType)
{
    AutoPtr<IAccessibilityManager> accessibilityManager;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&accessibilityManager);
    Boolean enable = FALSE;
    if (!(accessibilityManager->IsEnabled(&enable), enable)) {
        return NOERROR;
    }

    // if we are showing a feature that should be announced and one child
    // make this child the event source since this is the feature itself
    // otherwise the callback will take over and announce its client
    if ((mFeatureId == FEATURE_OPTIONS_PANEL ||
            mFeatureId == FEATURE_CONTEXT_MENU ||
            mFeatureId == FEATURE_PROGRESS ||
            mFeatureId == FEATURE_INDETERMINATE_PROGRESS)
            && GetChildCount() == 1) {
        VIEW_PROBE(GetChildAt(0))->SendAccessibilityEvent(eventType);
    } else {
        FrameLayout::SendAccessibilityEvent(eventType);
    }

    return NOERROR;
}

Boolean CPhoneWindow::_DecorView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && !IsDestroyed()) {
        Boolean tmp = FALSE;
        if (cb->DispatchPopulateAccessibilityEvent(event, &tmp), tmp) {
            return TRUE;
        }
    }

    return FrameLayout::DispatchPopulateAccessibilityEvent(event);
}

Boolean CPhoneWindow::_DecorView::SetFrame(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean changed = FrameLayout::SetFrame(l, t, r, b);
    if (changed) {
        AutoPtr<IRect> drawingBounds = mDrawingBounds;
        GetDrawingRect(drawingBounds);

        AutoPtr<IDrawable> fg = GetForeground();
        if (fg != NULL) {
            Int32 left = 0, top = 0, right = 0, bottom = 0;
            Int32 left1 = 0, top1 = 0, right1 = 0, bottom1 = 0;
            mFrameOffsets->GetLeft(&left);
            mFrameOffsets->GetTop(&top);
            mFrameOffsets->GetRight(&right);
            mFrameOffsets->GetBottom(&bottom);

            drawingBounds->GetLeft(&left1);
            drawingBounds->GetTop(&top1);
            drawingBounds->GetRight(&right1);
            drawingBounds->GetBottom(&bottom1);

            drawingBounds->SetLeft(left + left1);
            drawingBounds->SetTop(top + top1);
            drawingBounds->SetRight(right1 - right);
            drawingBounds->SetBottom(bottom1 - bottom);
            fg->SetBounds(drawingBounds);

            Int32 left2 = 0, top2 = 0, right2 = 0, bottom2 = 0;
            mFramePadding->GetLeft(&left2);
            mFramePadding->GetTop(&top2);
            mFramePadding->GetRight(&right2);
            mFramePadding->GetBottom(&bottom2);

            drawingBounds->SetLeft(left1 + left2 - left);
            drawingBounds->SetTop(top1 + top2 - top);
            drawingBounds->SetRight(right1 - right2 + right);
            drawingBounds->SetBottom(bottom1 - bottom2 + bottom);
        }

        AutoPtr<IDrawable> bg = GetBackground();
        if (bg != NULL) {
            bg->SetBounds(drawingBounds);
        }

        if (SWEEP_OPEN_MENU) {
            AutoPtr<IWindowManagerLayoutParams> attr;
            mHost->GetAttributes((IWindowManagerLayoutParams**)&attr);
            Int32 height = 0;
            attr->GetHeight(&height);
            if (mMenuBackground == NULL && mFeatureId < 0
                    && height
                    == IWindowManagerLayoutParams::MATCH_PARENT) {
                AutoPtr<IResources> res;
                GetContext()->GetResources((IResources**)&res);
                res->GetDrawable(R::drawable::menu_background, (IDrawable**)&mMenuBackground);
            }

            if (mMenuBackground != NULL) {
                Int32 left1 = 0, top1 = 0, right1 = 0, bottom1 = 0;
                drawingBounds->GetLeft(&left1);
                drawingBounds->GetTop(&top1);
                drawingBounds->GetRight(&right1);
                drawingBounds->GetBottom(&bottom1);

                mMenuBackground->SetBounds(left1, bottom1 - 6, right1, bottom1 + 20);
            }
        }
    }

    return changed;
}

void CPhoneWindow::_DecorView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<IResources> res;
    GetContext()->GetResources((IResources**)&res);

    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);

    Int32 widthPixels = 0, heightPixels = 0;
    metrics->GetWidthPixels(&widthPixels);
    metrics->GetHeightPixels(&heightPixels);
    Boolean isPortrait = widthPixels < heightPixels;

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean fixedWidth = FALSE;
    if (widthMode == MeasureSpec::AT_MOST) {
        AutoPtr<ITypedValue> tvw = isPortrait ? mHost->mFixedWidthMinor : mHost->mFixedWidthMajor;
        Int32 type = 0;
        if (tvw != NULL && (tvw->GetType(&type), type) != ITypedValue::TYPE_NULL) {
            Int32 w = 0;
            if (type == ITypedValue::TYPE_DIMENSION) {
                tvw->GetDimension(metrics, (Float*)&w);
            } else if (type == ITypedValue::TYPE_FRACTION) {
                tvw->GetFraction(widthPixels, widthPixels, (Float*)&w);
            } else {
                w = 0;
            }

            if (w > 0) {
                Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
                widthMeasureSpec = MeasureSpec::MakeMeasureSpec(
                        Elastos::Core::Math::Min(w, widthSize), MeasureSpec::EXACTLY);
                fixedWidth = TRUE;
            }
        }
    }

    if (heightMode == MeasureSpec::AT_MOST) {
        AutoPtr<ITypedValue> tvh = isPortrait ? mHost->mFixedHeightMajor : mHost->mFixedHeightMinor;
        Int32 type = 0;
        if (tvh != NULL && (tvh->GetType(&type), type) != ITypedValue::TYPE_NULL) {
            Int32 h = 0;
            if (type == ITypedValue::TYPE_DIMENSION) {
                tvh->GetDimension(metrics, (Float*)&h);
            } else if (type == ITypedValue::TYPE_FRACTION) {
                tvh->GetFraction(heightPixels, heightPixels, (Float*)&h);
            } else {
                h = 0;
            }

            if (h > 0) {
                Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);
                heightMeasureSpec = MeasureSpec::MakeMeasureSpec(
                        Elastos::Core::Math::Min(h, heightSize), MeasureSpec::EXACTLY);
            }
        }
    }

    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 width = GetMeasuredWidth();
    Boolean measure = FALSE;

    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY);

    if (!fixedWidth && widthMode == MeasureSpec::AT_MOST) {
        AutoPtr<ITypedValue> tv = isPortrait ? mHost->mMinWidthMinor : mHost->mMinWidthMajor;
        Int32 type = 0;
        if ((tv->GetType(&type), type) != ITypedValue::TYPE_NULL) {
            Int32 min = 0;
            if (type == ITypedValue::TYPE_DIMENSION) {
                tv->GetDimension(metrics, (Float*)&min);
            } else if (type == ITypedValue::TYPE_FRACTION) {
                tv->GetFraction(widthPixels, widthPixels, (Float*)&min);
            } else {
                min = 0;
            }

            if (width < min) {
                widthMeasureSpec = MeasureSpec::MakeMeasureSpec(min, MeasureSpec::EXACTLY);
                measure = TRUE;
            }
        }
    }

    // TODO: Support height?

    if (measure) {
        FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
}

void CPhoneWindow::_DecorView::DrawableChanged()
{
    if (mChanging) {
        return;
    }

    SetPadding(mFramePadding->mLeft + mBackgroundPadding->mLeft, mFramePadding->mTop
            + mBackgroundPadding->mTop, mFramePadding->mRight + mBackgroundPadding->mRight,
            mFramePadding->mBottom + mBackgroundPadding->mBottom);
    RequestLayout();
    Invalidate();

    Int32 opacity = CPixelFormat::OPAQUE;

    // Note: if there is no background, we will assume opaque. The
    // common case seems to be that an application sets there to be
    // no background so it can draw everything itself. For that,
    // we would like to assume OPAQUE and let the app force it to
    // the slower TRANSLUCENT mode if that is really what it wants.
    AutoPtr<IDrawable> bg = GetBackground();
    AutoPtr<IDrawable> fg = GetForeground();
    if (bg != NULL) {
        if (fg == NULL) {
            bg->GetOpacity(&opacity);
        } else if (mFramePadding->mLeft <= 0 && mFramePadding->mTop <= 0
                && mFramePadding->mRight <= 0 && mFramePadding->mBottom <= 0) {
            // If the frame padding is zero, then we can be opaque
            // if either the frame -or- the background is opaque.
            Int32 fop = 0;
            fg->GetOpacity(&fop);
            Int32 bop = 0;
            bg->GetOpacity(&bop);
            // if (false)
            //     Log.v(TAG, "Background opacity: " + bop + ", Frame opacity: " + fop);
            if (fop == CPixelFormat::OPAQUE || bop == CPixelFormat::OPAQUE) {
                opacity = CPixelFormat::OPAQUE;
            } else if (fop == CPixelFormat::UNKNOWN) {
                opacity = bop;
            } else if (bop == CPixelFormat::UNKNOWN) {
                opacity = fop;
            } else {
                opacity = Drawable::Drawable::ResolveOpacity(fop, bop);
            }
        } else {
            // For now we have to assume translucent if there is a
            // frame with padding... there is no way to tell if the
            // frame and background together will draw all pixels.
            // if (false)
            //     Log.v(TAG, "Padding: " + mFramePadding);
            opacity = CPixelFormat::TRANSLUCENT;
        }
    }

    // if (false)
    //     Log.v(TAG, "Background: " + bg + ", Frame: " + fg);
    // if (false)
    //     Log.v(TAG, "Selected default opacity: " + opacity);

    mDefaultOpacity = opacity;
    if (mFeatureId < 0) {
        mHost->SetDefaultWindowFormat(opacity);
    }
}

ECode CPhoneWindow::_DecorView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    FrameLayout::OnWindowFocusChanged(hasWindowFocus);

    // If the user is chording a menu shortcut, release the chord since
    // this window lost focus
    if (!hasWindowFocus && mHost->mPanelChordingKey != 0) {
        mHost->ClosePanel(IWindow::FEATURE_OPTIONS_PANEL);
    }

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->OnWindowFocusChanged(hasWindowFocus);
    }

    return NOERROR;
}

void CPhoneWindow::_DecorView::UpdateWindowResizeState()
{
    AutoPtr<IDrawable> bg = GetBackground();
    Int32 opacity = 0;
    HackTurnOffWindowResizeAnim(bg == NULL || (bg->GetOpacity(&opacity), opacity)
            != IPixelFormat::OPAQUE);
}

ECode CPhoneWindow::_DecorView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();

    UpdateWindowResizeState();

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->OnAttachedToWindow();
    }

    if (mFeatureId == -1) {
        /*
         * The main window has been attached, try to restore any panels
         * that may have been open before. This is called in cases where
         * an activity is being killed for configuration change and the
         * menu was open. When the activity is recreated, the menu
         * should be shown again.
         */
        mHost->OpenPanelsAfterRestore();
    }
    return NOERROR;
}

ECode CPhoneWindow::_DecorView::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && mFeatureId < 0) {
        cb->OnDetachedFromWindow();
    }

    if (mHost->mActionBar != NULL) {
        mHost->mActionBar->DismissPopupMenus();
    }

    if (mActionModePopup != NULL) {
        RemoveCallbacks(mShowActionModePopup);
        Boolean showing = FALSE;
        if (mActionModePopup->IsShowing(&showing), showing) {
            mActionModePopup->Dismiss();
        }

        mActionModePopup = NULL;
    }

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(mHost->GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st));
    if (st != NULL && st->mMenu != NULL && mFeatureId < 0) {
        st->mMenu->Close();
    }

    return NOERROR;
}

ECode CPhoneWindow::_DecorView::OnCloseSystemDialogs(
    /* [in] */ const String& reason)
{
    if (mFeatureId >= 0) mHost->CloseAllPanels();
    return NOERROR;
}

Boolean CPhoneWindow::_DecorView::ShowContextMenuForChild(
    /* [in] */ IView* originalView)
{
    assert(originalView != NULL);

    // Reuse the context menu builder
    if (mHost->mContextMenu == NULL) {
        AutoPtr<IContext> context = GetContext();

        CContextMenuBuilder::New(context, (IContextMenuBuilder**)&mHost->mContextMenu);
        mHost->mContextMenu->SetCallback(mHost->mContextMenuCallback);
    }
    else {
        mHost->mContextMenu->ClearAll();
    }

    AutoPtr<IBinder> token;
    originalView->GetWindowToken((IBinder**)&token);

    AutoPtr<IMenuDialogHelper> helper;
    mHost->mContextMenu->Show(originalView, token, (IMenuDialogHelper**)&helper);
    if (helper != NULL) {
       helper->SetPresenterCallback(mHost->mContextMenuCallback);
    }

    mHost->mContextMenuHelper = helper;
    return helper != NULL;
}

AutoPtr<IActionMode> CPhoneWindow::_DecorView::StartActionModeForChild(
    /* [in] */ IView* originalView,
    /* [in] */ IActionModeCallback* callback)
{
    // originalView can be used here to be sure that we don't obscure
    // relevant content with the context mode UI.
    return StartActionMode(callback);
}

AutoPtr<IActionMode> CPhoneWindow::_DecorView::StartActionMode(
    /* [in] */ IActionModeCallback* callback)
{
    if (mActionMode != NULL) {
        mActionMode->Finish();
    }

    AutoPtr<IActionModeCallback> wrappedCallback = new ActionModeCallbackWrapper(callback, this);
    AutoPtr<IActionMode> mode;

    AutoPtr<IWindowCallback> cb;
    if ((mHost->GetCallback((IWindowCallback**)&cb), cb) != NULL && !IsDestroyed()) {
        // try {
        cb->OnWindowStartingActionMode(wrappedCallback, (IActionMode**)&mode);
        // } catch (AbstractMethodError ame) {
        //     // Older apps might not implement this callback method.
        // }
    }

    if (mode != NULL) {
        mActionMode = mode;
    } else {
        if (mActionModeView == NULL) {
            Boolean isFloating = FALSE;
            if (mHost->IsFloating(&isFloating), isFloating) {
                CActionBarContextView::New(mContext,
                        (IActionBarContextView**)&mActionModeView);
                mActionModePopup = NULL;
                CPopupWindow::New(mContext, NULL,
                        R::attr::actionModePopupWindowStyle, (IPopupWindow**)&mActionModePopup);

                mActionModePopup->SetLayoutInScreenEnabled(TRUE);
                mActionModePopup->SetLayoutInsetDecor(TRUE);
                mActionModePopup->SetWindowLayoutType(
                        IWindowManagerLayoutParams::TYPE_APPLICATION);

                mActionModePopup->SetContentView(mActionModeView);
                mActionModePopup->SetWidth(IWindowManagerLayoutParams::MATCH_PARENT);

                AutoPtr<ITypedValue> heightValue;
                CTypedValue::New((ITypedValue**)&heightValue);

                AutoPtr<IResourcesTheme> theme;
                mContext->GetTheme((IResourcesTheme**)&theme);

                Boolean tmp = FALSE;
                theme->ResolveAttribute(R::attr::actionBarSize, heightValue, TRUE, &tmp);

                Int32 data = 0;
                heightValue->GetData(&data);

                AutoPtr<IResources> res;
                mContext->GetResources((IResources**)&res);
                AutoPtr<IDisplayMetrics> metrics;
                res->GetDisplayMetrics((IDisplayMetrics**)&metrics);

                AutoPtr<ITypedValueHelper> helper;
                CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&helper);

                Int32 height = 0;
                helper->ComplexToDimensionPixelSize(data, metrics, &height);
                mActionModeView->SetContentHeight(height);
                mActionModePopup->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
                mShowActionModePopup = new ShowActionModePopupRunnable(this);
            } else {
                AutoPtr<IViewStub> stub = IViewStub::Probe(FindViewById(R::id::action_mode_bar_stub));
                if (stub != NULL) {
                    stub->Inflate((IView**)&mActionModeView);
                }
            }
        }

        if (mActionModeView != NULL) {
            mActionModeView->KillMode();
            mode = new StandaloneActionMode(mContext, mActionModeView,
                    wrappedCallback, mActionModePopup == NULL);
            Boolean tmp = FALSE;
            AutoPtr<IMenu> menu;
            mode->GetMenu((IMenu**)&menu);
            if (callback->OnCreateActionMode(mode, menu, &tmp), tmp) {
                mode->Invalidate();
                mActionModeView->InitForMode(mode);
                mActionModeView->SetVisibility(IView::VISIBLE);
                mActionMode = mode;
                if (mActionModePopup != NULL) {
                    Post(mShowActionModePopup);
                }

                VIEW_PROBE(mActionModeView)->SendAccessibilityEvent(
                        IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
            } else {
                mActionMode = NULL;
            }
        }
    }

    if (mActionMode != NULL && cb != NULL && !IsDestroyed()) {
        // try {
        cb->OnActionModeStarted(mActionMode);
        // } catch (AbstractMethodError ame) {
        //     // Older apps might not implement this callback method.
        // }
    }

    return mActionMode;
}

IVIEW_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IVIEWGROUP_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IVIEWPARENT_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IVIEWMANAGER_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IDRAWABLECALLBACK_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IKEYEVENTCALLBACK_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CPhoneWindow::DecorView, CPhoneWindow::_DecorView);

CPhoneWindow::DecorView::DecorView(
    /* [in] */ CPhoneWindow* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 featureId,
    /* [in] */ Boolean useSelfRef) :
    _DecorView(host, context, featureId)
    , mUseSelfRef(useSelfRef)
{
}

CPhoneWindow::DecorView::~DecorView()
{
    if (!mUseSelfRef)
        mHost->mDecor = NULL;
}

PInterface CPhoneWindow::DecorView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IFrameLayout*)this;
    }
    else if (riid == EIID_IView) {
        return (IView*)this;
    }
    else if (riid == EIID_IFrameLayout) {
        return (IFrameLayout*)this;
    }
    else if (riid == EIID_IViewGroup) {
        return (IViewGroup*)(IFrameLayout*)this;
    }
    else if (riid == EIID_IViewParent) {
        return (IViewParent*)this;
    }
    else if (riid == EIID_IViewManager) {
        return (IViewManager*)this;
    }
    else if (riid == EIID_IDrawableCallback) {
        return (IDrawableCallback*)this;
    }
    else if (riid == EIID_IKeyEventCallback) {
        return (IKeyEventCallback*)this;
    }
    else if (riid == EIID_IAccessibilityEventSource) {
        return (IAccessibilityEventSource*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    else if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)(FrameLayout*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)(FrameLayout*)this);
    }
    else if (riid == EIID_IRootViewSurfaceTaker) {
        return reinterpret_cast<PInterface>((IRootViewSurfaceTaker*)this);
    }

    return NULL;
}

UInt32 CPhoneWindow::DecorView::AddRef()
{
    if (mUseSelfRef)
        return ElRefBase::AddRef();
    else {
        assert(mHost != NULL);
        return mHost->AddRef();
    }
}

UInt32 CPhoneWindow::DecorView::Release()
{
    if (mUseSelfRef)
        return ElRefBase::Release();
    else {
        assert(mHost != NULL);
        return mHost->Release();
    }
}

UInt32 CPhoneWindow::DecorView::_AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CPhoneWindow::DecorView::_Release()
{
    return ElRefBase::Release();
}

ECode CPhoneWindow::DecorView::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindow::DecorView::GetForegroundGravity(
    /* [out] */ Int32* foregroundGravity)
{
    assert(foregroundGravity != NULL);
    *foregroundGravity = _DecorView::GetForegroundGravity();
    return NOERROR;
}

ECode CPhoneWindow::DecorView::SetForegroundGravity(
    /* [in] */ Int32 foregroundGravity)
{
    return _DecorView::SetForegroundGravity(foregroundGravity);
}

ECode CPhoneWindow::DecorView::SetForeground(
    /* [in] */ IDrawable* drawable)
{
    return _DecorView::SetForeground(drawable);
}

ECode CPhoneWindow::DecorView::GetForeground(
    /* [out] */ IDrawable** foreground)
{
    VALIDATE_NOT_NULL(foreground);
    AutoPtr<IDrawable> d = _DecorView::GetForeground();
    *foreground = d.Get();
    REFCOUNT_ADD(*foreground);

    return NOERROR;
}

ECode CPhoneWindow::DecorView::SetMeasureAllChildren(
    /* [in] */ Boolean measureAll)
{
    return _DecorView::SetMeasureAllChildren(measureAll);
}

ECode CPhoneWindow::DecorView::GetMeasureAllChildren(
    /* [out] */ Boolean* measureAll)
{
    assert(measureAll != NULL);
    *measureAll = _DecorView::GetMeasureAllChildren();
    return NOERROR;
}

ECode CPhoneWindow::DecorView::GetConsiderGoneChildrenWhenMeasuring(
    /* [out] */ Boolean* measureAll)
{
    VALIDATE_NOT_NULL(measureAll)
    *measureAll = _DecorView::GetConsiderGoneChildrenWhenMeasuring();

    return NOERROR;
}

ECode CPhoneWindow::DecorView::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new DecorViewWeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

CPhoneWindow::PanelFeatureState::PanelFeatureState(
    /* [in] */ Int32 featureId)
    : mFeatureId(featureId)
    , mBackground(0)
    , mFullBackground(0)
    , mGravity(0)
    , mX(0)
    , mY(0)
    , mWindowAnimations(0)
    , mIsCompact(FALSE)
    , mListPresenterTheme(0)
    , mIsPrepared(FALSE)
    , mIsHandled(FALSE)
    , mIsOpen(FALSE)
    , mIsInExpandedMode(FALSE)
    , mQwertyMode(FALSE)
    , mRefreshDecorView(FALSE)
    , mRefreshMenuContent(FALSE)
    , mWasLastOpen(FALSE)
    , mWasLastExpanded(FALSE)
{}

Boolean CPhoneWindow::PanelFeatureState::IsInListMode()
{
    return mIsInExpandedMode || mIsCompact;
}

Boolean CPhoneWindow::PanelFeatureState::HasPanelItems()
{
    if (mShownPanelView == NULL) return FALSE;
    if (mCreatedPanelView != NULL) return TRUE;

    if (mIsCompact || mIsInExpandedMode) {
        AutoPtr<IListAdapter> adapter;
        mListMenuPresenter->GetAdapter((IListAdapter**)&adapter);
        Int32 count = 0;
        return (adapter->GetCount(&count), count) > 0;
    } else {
        Int32 count = 0;
        return (IViewGroup::Probe(mShownPanelView)->GetChildCount(&count), count) > 0;
    }
}

void CPhoneWindow::PanelFeatureState::ClearMenuPresenters()
{
    if (mMenu != NULL) {
        mMenu->RemoveMenuPresenter(mIconMenuPresenter);
        mMenu->RemoveMenuPresenter(mListMenuPresenter);
    }

    mIconMenuPresenter = NULL;
    mListMenuPresenter = NULL;
}

void CPhoneWindow::PanelFeatureState::SetStyle(
    /* [in] */ IContext* context)
{
    assert(context != NULL);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Theme),
            ArraySize(R::styleable::Theme));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrIds, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::Theme_panelBackground, 0, &mBackground);
    a->GetResourceId(R::styleable::Theme_panelFullBackground, 0, &mFullBackground);
    a->GetResourceId(R::styleable::Theme_windowAnimationStyle, 0, &mWindowAnimations);

    a->GetBoolean(R::styleable::Theme_panelMenuIsCompact, FALSE, &mIsCompact);
    a->GetResourceId(R::styleable::Theme_panelMenuListTheme,
            R::style::Theme_ExpandedMenu, &mListPresenterTheme);

    a->Recycle();
}

void CPhoneWindow::PanelFeatureState::SetMenu(
    /* [in] */ IMenuBuilder* menu)
{
     if (menu == mMenu) return;

     if (mMenu != NULL) {
         mMenu->RemoveMenuPresenter(mIconMenuPresenter);
         mMenu->RemoveMenuPresenter(mListMenuPresenter);
     }

     mMenu = menu;
     if (menu != NULL) {
         if (mIconMenuPresenter != NULL) menu->AddMenuPresenter(mIconMenuPresenter);
         if (mListMenuPresenter != NULL) menu->AddMenuPresenter(mListMenuPresenter);
     }
}

AutoPtr<IMenuView> CPhoneWindow::PanelFeatureState::GetListMenuView(
    /* [in] */ IContext* context,
    /* [in] */ IMenuPresenterCallback* cb)
{
    if (mMenu == NULL) return NULL;

    if (!mIsCompact) {
        GetIconMenuView(context, cb); // Need this initialized to know where our offset goes
    }

    if (mListMenuPresenter == NULL) {
        CListMenuPresenter::New(R::layout::list_menu_item_layout, mListPresenterTheme, (IListMenuPresenter**)&mListMenuPresenter);
        mListMenuPresenter->SetCallback(cb);
        mListMenuPresenter->SetId(R::id::list_menu_presenter);
        mMenu->AddMenuPresenter(mListMenuPresenter);
    }

    if (mIconMenuPresenter != NULL) {
        Int32 itemNum;
        mIconMenuPresenter->GetNumActualItemsShown(&itemNum);
        mListMenuPresenter->SetItemIndexOffset(itemNum);
    }

    AutoPtr<IMenuView> result;
    mListMenuPresenter->GetMenuView(mDecorView, (IMenuView**)&result);

    return result;
}

AutoPtr<IMenuView> CPhoneWindow::PanelFeatureState::GetIconMenuView(
    /* [in] */ IContext* context,
    /* [in] */ IMenuPresenterCallback* cb)
{
     if (mMenu == NULL) return NULL;

     if (mIconMenuPresenter == NULL) {
         CIconMenuPresenter::New(context, (IIconMenuPresenter**)&mIconMenuPresenter);
         mIconMenuPresenter->SetCallback(cb);
         mIconMenuPresenter->SetId(R::id::icon_menu_presenter);
         mMenu->AddMenuPresenter(mIconMenuPresenter);
     }

     AutoPtr<IMenuView> result;
     mIconMenuPresenter->GetMenuView(mDecorView, (IMenuView**)&result);

     return result;
}

AutoPtr<IParcelable> CPhoneWindow::PanelFeatureState::OnSaveInstanceState()
{
    AutoPtr<CPhoneWindowSavedState> savedState;
    CPhoneWindowSavedState::NewByFriend((CPhoneWindowSavedState**)&savedState);
    savedState->mFeatureId = mFeatureId;
    savedState->mIsOpen = mIsOpen;
    savedState->mIsInExpandedMode = mIsInExpandedMode;

    if (mMenu != NULL) {
        CBundle::New((IBundle**)&savedState->mMenuState);
        mMenu->SavePresenterStates(savedState->mMenuState);
    }

    return savedState;
}

void CPhoneWindow::PanelFeatureState::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<CPhoneWindowSavedState> savedState = (CPhoneWindowSavedState*)state;
    mFeatureId = savedState->mFeatureId;
    mWasLastOpen = savedState->mIsOpen;
    mWasLastExpanded = savedState->mIsInExpandedMode;
    mFrozenMenuState = savedState->mMenuState;

    /*
     * A LocalActivityManager keeps the same instance of this class around.
     * The first time the menu is being shown after restoring, the
     * Activity.onCreateOptionsMenu should be called. But, if it is the
     * same instance then menu != null and we won't call that method.
     * So, clear this.  Also clear any cached views.
     */
    mMenu = NULL;
    mCreatedPanelView = NULL;
    mShownPanelView = NULL;
    mDecorView = NULL;
}

void CPhoneWindow::PanelFeatureState::ApplyFrozenState()
{
    if (mMenu != NULL && mFrozenMenuState != NULL) {
        mMenu->RestorePresenterStates(mFrozenMenuState);
        mFrozenMenuState = NULL;
    }
}

CPhoneWindow::DialogMenuCallback::DialogMenuCallback(
    /* [in] */ Int32 featureId,
    /* [in] */ CPhoneWindow* host)
    : mFeatureId(featureId)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

CAR_INTERFACE_IMPL_2(CPhoneWindow::DialogMenuCallback, IMenuBuilderCallback, IMenuPresenterCallback)

ECode CPhoneWindow::DialogMenuCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (allMenusAreClosing) {
        AutoPtr<IInterface> w;
        mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
        if (w == NULL)
            return NOERROR;

        AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());

        AutoPtr<IMenuBuilder> m;
        if ((menu->GetRootMenu((IMenuBuilder**)&m), m.Get()) != menu) {
            OnCloseSubMenu(ISubMenuBuilder::Probe(menu));
        }

        AutoPtr<IWindowCallback> callback;
        mHost->GetCallback((IWindowCallback**)&callback);
        Boolean destoryed = FALSE;
        if (callback != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
            callback->OnPanelClosed(mFeatureId, (IMenu*)menu);
        }

        if (mHost->mContextMenu != NULL && menu == mHost->mContextMenu->Probe(EIID_IMenuBuilder)) {
            mHost->DismissContextMenu();
        }

        // Dismiss the submenu, if it is showing
        if (mSubMenuHelper != NULL) {
            mSubMenuHelper->Dismiss();
            mSubMenuHelper = NULL;
        }
    }

    return NOERROR;
}

ECode CPhoneWindow::DialogMenuCallback::OnCloseSubMenu(
    /* [in] */ ISubMenuBuilder* menu)
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());

    AutoPtr<IWindowCallback> callback;
    mHost->GetCallback((IWindowCallback**)&callback);

    Boolean destoryed = FALSE;
    if (callback != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
        AutoPtr<IMenuBuilder> rootMenu;
        menu->GetRootMenu((IMenuBuilder**)&rootMenu);
        callback->OnPanelClosed(mFeatureId, (IMenu*)rootMenu.Get());
    }

    return NOERROR;
}

ECode CPhoneWindow::DialogMenuCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* toFinish)
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());

    AutoPtr<IWindowCallback> callback;
    mHost->GetCallback((IWindowCallback**)&callback);

    Boolean destoryed = FALSE;
    if (callback != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
        return callback->OnMenuItemSelected(mFeatureId, item, toFinish);
    }

    *toFinish = FALSE;
    return NOERROR;
}

ECode CPhoneWindow::DialogMenuCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return NOERROR;
}

ECode CPhoneWindow::DialogMenuCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    if (subMenu == NULL) {
        *state = FALSE;
        return NOERROR;
    }

    // Set a simple callback for the submenu
    subMenu->SetCallback(this);

    // The window manager will give us a valid window token
     CMenuDialogHelper::New(subMenu, (IMenuDialogHelper**)&mSubMenuHelper);
    mSubMenuHelper->Show(NULL);

    *state = TRUE;
    return NOERROR;
}

CPhoneWindow::DecorRunnable::DecorRunnable(
    /* [in] */ CPhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CPhoneWindow::DecorRunnable::Run()
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());
    // Invalidate if the panel menu hasn't been created before this.
    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(mHost->GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st));
    Boolean destoryed = FALSE;
    if (!(mHost->IsDestroyed(&destoryed), destoryed) && (st == NULL || st->mMenu == NULL)) {
        mHost->InvalidatePanelMenu(FEATURE_ACTION_BAR);
    }
    return NOERROR;
}

CPhoneWindow::DrawableFeatureState::DrawableFeatureState(
    /* [in] */ Int32 _featureId)
    : mFeatureId(_featureId)
    , mResid(0)
    , mAlpha(255)
    , mCurAlpha(255)
{
}

CPhoneWindow::InvalidatePanelMenuRunnable::InvalidatePanelMenuRunnable(
    /* [in] */ CPhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CPhoneWindow::InvalidatePanelMenuRunnable::Run()
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> host = reinterpret_cast<CPhoneWindow*>(w.Get());
    for (Int32 i = 0; i <= FEATURE_MAX; i++) {
        if ((host->mInvalidatePanelMenuFeatures & (1 << i)) != 0) {
            host->DoInvalidatePanelMenu(i);
        }
    }

    host->mInvalidatePanelMenuPosted = FALSE;
    host->mInvalidatePanelMenuFeatures = 0;
    return NOERROR;
}

AutoPtr<IIWindowManager> CPhoneWindow::WindowManagerHolder::GetWindowManager()
{
    if (sWindowManager == NULL) {
        sWindowManager = IIWindowManager::Probe(ServiceManager::GetService(String("window")));
        assert(sWindowManager != NULL);
    }
    return sWindowManager;
}

CAR_INTERFACE_IMPL(CPhoneWindow::PanelMenuPresenterCallback, IMenuPresenterCallback)

CPhoneWindow::PanelMenuPresenterCallback::PanelMenuPresenterCallback(
    /* [in] */ CPhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CPhoneWindow::PanelMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());

    AutoPtr<IMenu> parentMenu;
    menu->GetRootMenu((IMenuBuilder**)&parentMenu);

    Boolean isSubMenu = parentMenu != menu;
    AutoPtr<PanelFeatureState> panel = mHost->FindMenuPanel(isSubMenu ? parentMenu : menu);
    if (panel != NULL) {
        if (isSubMenu) {
            mHost->CallOnPanelClosed(panel->mFeatureId, panel, parentMenu);
            mHost->ClosePanel(panel, TRUE);
        } else {
            // Close the panel and only do the callback if the menu is being
            // closed completely, not if opening a sub menu
            mHost->ClosePanel(panel, allMenusAreClosing);
        }
    }

    return NOERROR;
}

ECode CPhoneWindow::PanelMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());
    Boolean has = FALSE;
    if (subMenu == NULL && (mHost->HasFeature(FEATURE_ACTION_BAR, &has), has)) {
        AutoPtr<IWindowCallback> cb;
        mHost->GetCallback((IWindowCallback**)&cb);
        Boolean destoryed = FALSE;
        if (cb != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
            Boolean tmp = FALSE;
            cb->OnMenuOpened(FEATURE_ACTION_BAR, subMenu, &tmp);
        }
    }

    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CPhoneWindow::ActionMenuPresenterCallback, IMenuPresenterCallback)

CPhoneWindow::ActionMenuPresenterCallback::ActionMenuPresenterCallback(
    /* [in] */ CPhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CPhoneWindow::ActionMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL) {
        Boolean tmp = FALSE;
        cb->OnMenuOpened(FEATURE_ACTION_BAR, subMenu, &tmp);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CPhoneWindow::ActionMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());
    mHost->CheckCloseActionMenu(menu);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CPhoneWindow::MyMenuBuilderCallback, IMenuBuilderCallback);

CPhoneWindow::MyMenuBuilderCallback::MyMenuBuilderCallback(
    /* [in] */ CPhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CPhoneWindow::MyMenuBuilderCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)
    *state = FALSE;

    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());
    return mHost->OnMenuItemSelected(menu, item, state);
}

ECode CPhoneWindow::MyMenuBuilderCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    AutoPtr<IInterface> w;
    mWeakHost->Resolve(EIID_CPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<CPhoneWindow> mHost = reinterpret_cast<CPhoneWindow*>(w.Get());
    return mHost->OnMenuModeChange(menu);
}

CPhoneWindow::CPhoneWindow()
    : mDecor(NULL)
    , mIsFloating(FALSE)
    , mPanelChordingKey(0)
    , mBackgroundResource(0)
    , mFrameResource(0)
    , mTextColor(0)
    , mTitleColor(0)
    , mAlwaysReadCloseOnTouchAttr(FALSE)
    , mClosingActionMenu(FALSE)
    , mUiOptions(0)
    , mInvalidatePanelMenuPosted(FALSE)
    , mInvalidatePanelMenuFeatures(0)
{
    mVolumeControlStreamType = IAudioManager::USE_DEFAULT_STREAM_TYPE;
    mContextMenuCallback = new DialogMenuCallback(IWindow::FEATURE_CONTEXT_MENU, this);
    CTypedValue::New((ITypedValue**)&mMinWidthMajor);
    CTypedValue::New((ITypedValue**)&mMinWidthMinor);
    mInvalidatePanelMenuRunnable = new InvalidatePanelMenuRunnable(this);
}

CPhoneWindow::~CPhoneWindow()
{
}

ECode CPhoneWindow::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mRetainContext = IActivity::Probe(mContext) == NULL && IService::Probe(mContext) == NULL;
    if (mRetainContext) {
        mContext->AddRef();
    }
    LayoutInflater::From(context, (ILayoutInflater**)&mLayoutInflater);
    return NOERROR;
}

PInterface CPhoneWindow::Probe(
    /* [in] */ REIID riid)
{
    if(riid == EIID_CPhoneWindow) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CPhoneWindow::Probe(riid);
}

UInt32 CPhoneWindow::AddRef()
{
    return _CPhoneWindow::AddRef();
}

UInt32 CPhoneWindow::Release()
{
    DecorView* decor = mDecor;
    UInt32 ref = _CPhoneWindow::Release();
    if (decor != NULL && ref == 1) {
        decor->_Release();
    }
    return ref;
}

ECode CPhoneWindow::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return _CPhoneWindow::GetInterfaceID(pObject, pIID);
}

ECode CPhoneWindow::SetContainer(
    /* [in] */ IWindow* container)
{
    return Window::SetContainer(container);
}

ECode CPhoneWindow::RequestFeature(
    /* [in] */ Int32 featureId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mContentParent != NULL) {
        Slogger::E(TAG, "requestFeature() must be called before adding content");
//        throw new AndroidRuntimeException("requestFeature() must be called before adding content");
        return E_RUNTIME_EXCEPTION;
    }
    const Int32 features = GetFeatures();
    if ((features != DEFAULT_FEATURES) && (featureId == IWindow::FEATURE_CUSTOM_TITLE)) {
        Slogger::E(TAG, "requestFeature() You cannot combine custom titles with other title features");
        /* Another feature is enabled and the user is trying to enable the custom title feature */
//        throw new AndroidRuntimeException("You cannot combine custom titles with other title features");
        return E_RUNTIME_EXCEPTION;
    }
    if (((features & (1 << IWindow::FEATURE_CUSTOM_TITLE)) != 0) &&
         (featureId != IWindow::FEATURE_CUSTOM_TITLE) &&
         (featureId != IWindow::FEATURE_ACTION_MODE_OVERLAY)) {
        Slogger::E(TAG, "requestFeature() You cannot combine custom titles with other title features");
        /* Custom title feature is enabled and the user is trying to enable another feature */
//        throw new AndroidRuntimeException("You cannot combine custom titles with other title features");
        return E_RUNTIME_EXCEPTION;
    }

    if ((features & (1 << FEATURE_NO_TITLE)) != 0 && featureId == FEATURE_ACTION_BAR) {
        return NOERROR; // Ignore. No title dominates.
    }

    if ((features & (1 << FEATURE_ACTION_BAR)) != 0 && featureId == FEATURE_NO_TITLE) {
        // Remove the action bar feature if we have no title. No title dominates.
        RemoveFeature(FEATURE_ACTION_BAR);
    }

    return Window::RequestFeature(featureId, result);
}

ECode CPhoneWindow::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    mUiOptions = uiOptions;
    return NOERROR;
}

ECode CPhoneWindow::SetUiOptions(
    /* [in] */ Int32 uiOptions,
    /* [in] */ Int32 mask)
{
    mUiOptions = (mUiOptions & ~mask) | (uiOptions & mask);
    return NOERROR;
}

ECode CPhoneWindow::SetContentView(
    /* [in] */ Int32 layoutResID)
{
    if (mContentParent == NULL) {
        InstallDecor();
    }
    else {
        mContentParent->RemoveAllViews();
    }

    AutoPtr<IView> root;
    mLayoutInflater->Inflate(layoutResID, mContentParent.Get(), (IView**)&root);

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean destoryed = FALSE;
    if (cb != NULL && !(IsDestroyed(&destoryed), destoryed)) {
        cb->OnContentChanged();
    }

    return NOERROR;
}

ECode CPhoneWindow::SetContentView(
    /* [in] */ IView* view)
{
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(
        IWindowManagerLayoutParams::MATCH_PARENT,
        IWindowManagerLayoutParams::MATCH_PARENT,
        (IViewGroupLayoutParams**)&params);

    return SetContentView(view, params);
}

ECode CPhoneWindow::SetContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    if (mContentParent == NULL) {
        InstallDecor();
    }
    else {
        mContentParent->RemoveAllViews();
    }

    mContentParent->AddView(view, params);

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    if (cb != NULL) {
        cb->OnContentChanged();
    }

    return NOERROR;
}

ECode CPhoneWindow::AddContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    if (mContentParent == NULL) {
        InstallDecor();
    }
    FAIL_RETURN(mContentParent->AddView(view, params));
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean destoryed = FALSE;
    if (cb != NULL && !(IsDestroyed(&destoryed), destoryed)) {
        cb->OnContentChanged();
    }
    return NOERROR;
}

ECode CPhoneWindow::GetCurrentFocus(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    if (mDecor != NULL) {
        return mDecor->FindFocus(view);
    }
    else {
        *view = NULL;
        return NOERROR;
    }
}

ECode CPhoneWindow::TakeSurface(
    /* [in] */ ISurfaceHolderCallback2* cb)
{
    mTakeSurfaceCallback = cb;
    return NOERROR;
}

ECode CPhoneWindow::TakeInputQueue(
    /* [in] */ IInputQueueCallback* callback)
{
    mTakeInputQueueCallback = callback;

    return NOERROR;
}

ECode CPhoneWindow::IsFloating(
    /* [out] */ Boolean* isFloating)
{
    VALIDATE_NOT_NULL(isFloating);
    *isFloating = mIsFloating;
    return NOERROR;
}

ECode CPhoneWindow::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = mLayoutInflater;
    REFCOUNT_ADD(*inflater);
    return NOERROR;
}

ECode CPhoneWindow::SetTitle(
    /* [in] */ ICharSequence* title)
{
    if (mTitleView != NULL) {
        mTitleView->SetText(title);
    }
    else if (mActionBar != NULL) {
        mActionBar->SetWindowTitle(title);
    }

    mTitle = title;
    return NOERROR;
}

ECode CPhoneWindow::SetTitleColor(
    /* [in] */ Int32 textColor)
{
    if (mTitleView != NULL) {
        mTitleView->SetTextColor(textColor);
    }
    mTitleColor = textColor;
    return NOERROR;
}

ECode CPhoneWindow::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Action bars handle their own menu state
    if (mActionBar == NULL) {
        AutoPtr<PanelFeatureState> st;
        FAIL_RETURN(GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st));
        if ((st != NULL) && (st->mMenu != NULL)) {
            if (st->mIsOpen) {
                // Freeze state
                AutoPtr<IBundle> state;
                CBundle::New((IBundle**)&state);
                if (st->mIconMenuPresenter != NULL) {
                    st->mIconMenuPresenter->SaveHierarchyState(state);
                }
                if (st->mListMenuPresenter != NULL) {
                    st->mListMenuPresenter->SaveHierarchyState(state);
                }

                // Remove the menu views since they need to be recreated
                // according to the new configuration
                ClearMenuViews(st);

                // Re-open the same menu
                ReopenMenu(FALSE);

                // Restore state
                if (st->mIconMenuPresenter != NULL) {
                    st->mIconMenuPresenter->RestoreHierarchyState(state);
                }
                if (st->mListMenuPresenter != NULL) {
                    st->mListMenuPresenter->RestoreHierarchyState(state);
                }

            } else {
                // Clear menu views so on next menu opening, it will use
                // the proper layout
                ClearMenuViews(st);
            }
        }
    }

    return NOERROR;
}

/**
 * Called when the panel key is pushed down.
 * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL}.
 * @param event The key event.
 * @return Whether the key was handled.
 */
ECode CPhoneWindow::OnKeyDownPanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IKeyEvent* event,
    /* [out */ Boolean* handled)
{
    VALIDATE_NOT_NULL(handled);
    assert(event != NULL);

    Int32 keyCode = 0, repeat = 0;
    event->GetKeyCode(&keyCode);

    if ((event->GetRepeatCount(&repeat), repeat) == 0) {
        // The panel key was pushed, so set the chording key
        mPanelChordingKey = keyCode;

        AutoPtr<PanelFeatureState> st;
        FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
        if (!st->mIsOpen) {
            return PreparePanel(st, event, handled);
        }
    }

    *handled = FALSE;
    return NOERROR;
}

/**
 * Called when the panel key is released.
 * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL}.
 * @param event The key event.
 */
ECode CPhoneWindow::OnKeyUpPanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IKeyEvent* event)
{
    // The panel key was released, so clear the chording key
    if (mPanelChordingKey != 0) {
        mPanelChordingKey = 0;

        Boolean isCanceled;
        if ((event->IsCanceled(&isCanceled), isCanceled) || (mDecor != NULL && mDecor->mActionMode != NULL)) {
            return NOERROR;
        }

        Boolean playSoundEffect = FALSE, reserved = FALSE;
        AutoPtr<PanelFeatureState> st;
        FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
        if (featureId == FEATURE_OPTIONS_PANEL && mActionBar != NULL &&
                (mActionBar->IsOverflowReserved(&reserved), reserved)) {

            Int32 visiable = 0;
            if ((mActionBar->GetVisibility(&visiable), visiable) == IView::VISIBLE) {
                Boolean showing = FALSE;
                if (!(mActionBar->IsOverflowMenuShowing(&showing), showing)) {
                    Boolean prepared = FALSE, destoryed = FALSE;
                    if (!(IsDestroyed(&destoryed), destoryed) && (PreparePanel(st, event, &prepared), prepared)) {
                        mActionBar->ShowOverflowMenu(&playSoundEffect);
                    }
                } else {
                    mActionBar->HideOverflowMenu(&playSoundEffect);
                }
            }
        } else {
            if (st->mIsOpen || st->mIsHandled) {

                // Play the sound effect if the user closed an open menu (and not if
                // they just released a menu shortcut)
                playSoundEffect = st->mIsOpen;

                // Close menu
                ClosePanel(st, TRUE);

            } else if (st->mIsPrepared) {
                Boolean show = TRUE;
                if (st->mRefreshMenuContent) {
                    // Something may have invalidated the menu since we prepared it.
                    // Re-prepare it to refresh.
                    st->mIsPrepared = FALSE;
                    PreparePanel(st, event, &show);
                }

                if (show) {
                    // Write 'menu opened' to event log
                    // EventLog.writeEvent(50001, 0);

                    // Show menu
                    OpenPanel(st, event);

                    playSoundEffect = TRUE;
                }
            }
        }

        if (playSoundEffect) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> audioService;
            context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
            AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(audioService);
            if (audioManager != NULL) {
                audioManager->PlaySoundEffect(IAudioManager::FX_KEY_CLICK);
            // } else {
            //     Log.w(TAG, "Couldn't get audio manager");
            }
        }
    }

    return NOERROR;
}

void CPhoneWindow::ClearMenuViews(
        /* [in] */ PanelFeatureState* st)
{
    // This can be called on config changes, so we should make sure
    // the views will be reconstructed based on the new orientation, etc.

    // Allow the callback to create a new panel view
    st->mCreatedPanelView = NULL;

    // Causes the decor view to be recreated
    st->mRefreshDecorView = TRUE;

    st->ClearMenuPresenters();
}

ECode CPhoneWindow::OpenPanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IKeyEvent* event)
{
    Boolean reserved = FALSE;
    if (featureId == FEATURE_OPTIONS_PANEL && mActionBar != NULL &&
            (mActionBar->IsOverflowReserved(&reserved), reserved)) {

        Int32 visiable = 0;
        if ((mActionBar->GetVisibility(&visiable), visiable) == IView::VISIBLE) {
            Boolean tmp = FALSE;
            mActionBar->ShowOverflowMenu(&tmp);
        }
    } else {
        AutoPtr<PanelFeatureState> st;
        FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
        OpenPanel(st, event);
    }

    return NOERROR;
}

/**
 * Perform initial setup of a panel. This should at the very least set the
 * style information in the PanelFeatureState and must set
 * PanelFeatureState.decor to the panel's window decor view.
 *
 * @param st The panel being initialized.
 */
Boolean CPhoneWindow::InitializePanelDecor(
    /* [in] */ PanelFeatureState* st)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    st->mDecorView = new DecorView(this, context, st->mFeatureId, TRUE);
    st->mGravity = IGravity::CENTER | IGravity::BOTTOM;
    st->SetStyle(context);

    return TRUE;
}

Int32 CPhoneWindow::GetOptionsPanelGravity()
{
    Int32 gravity = 0;
    // try {
    ECode ec = WindowManagerHolder::GetWindowManager()->GetPreferredOptionsPanelGravity(&gravity);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't getOptionsPanelGravity; using default");
        gravity = IGravity::CENTER | IGravity::BOTTOM;
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "Couldn't getOptionsPanelGravity; using default", ex);
    //     return Gravity.CENTER | Gravity.BOTTOM;
    // }
    return gravity;
}

void CPhoneWindow::OnOptionsPanelRotationChanged()
{
    AutoPtr<PanelFeatureState> st;
    GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st);
    if (st == NULL) return;

    AutoPtr<IWindowManagerLayoutParams> lp;
    if (st->mDecorView != NULL) {
        st->mDecorView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    }

    if (lp != NULL) {
        ((CWindowManagerLayoutParams*)lp.Get())->mGravity = GetOptionsPanelGravity();
        AutoPtr<IViewManager> wm;
        GetWindowManager((IWindowManager**)&wm);
        if (wm != NULL) {
            wm->UpdateViewLayout(st->mDecorView, lp);
        }
    }
}

/**
 * Initializes the panel associated with the panel feature state. You must
 * at the very least set PanelFeatureState.panel to the View implementing
 * its contents. The default implementation gets the panel from the menu.
 *
 * @param st The panel state being initialized.
 * @return Whether the initialization was successful.
 */
Boolean CPhoneWindow::InitializePanelContent(
    /* [in] */ PanelFeatureState* st)
{
    if (st->mCreatedPanelView != NULL) {
        st->mShownPanelView = st->mCreatedPanelView;
        return TRUE;
    }

    if (st->mMenu == NULL) {
        return FALSE;
    }

    if (mPanelMenuPresenterCallback == NULL) {
        mPanelMenuPresenterCallback = new PanelMenuPresenterCallback(this);
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IMenuView> menuView = st->IsInListMode()
            ? st->GetListMenuView(context, mPanelMenuPresenterCallback)
            : st->GetIconMenuView(context, mPanelMenuPresenterCallback);

    st->mShownPanelView = IView::Probe(menuView.Get());

    if (st->mShownPanelView != NULL) {
        // Use the menu View's default animations if it has any
        Int32 defaultAnimations;
        menuView->GetWindowAnimations(&defaultAnimations);
        if (defaultAnimations != 0) {
            st->mWindowAnimations = defaultAnimations;
        }
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void CPhoneWindow::OpenPanel(
    /* [in] */ PanelFeatureState* st,
    /* [in] */ IKeyEvent* event)
{
    // Already open, return
    Boolean destoryed = FALSE;
    if (st->mIsOpen || (IsDestroyed(&destoryed), destoryed)) {
        return;
    }

    // Don't open an options panel for honeycomb apps on xlarge devices.
    // (The app should be using an action bar for menu items.)
    if (st->mFeatureId == FEATURE_OPTIONS_PANEL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);

        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        assert(res != NULL);

        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);

        Int32 screenLayout = 0;
        config->GetScreenLayout(&screenLayout);
        Boolean isXLarge = (screenLayout & IConfiguration::SCREENLAYOUT_SIZE_MASK) ==
                IConfiguration::SCREENLAYOUT_SIZE_XLARGE;

        AutoPtr<IApplicationInfo> appInfo;
        context->GetApplicationInfo((IApplicationInfo**)&appInfo);
        assert(appInfo != NULL);

        Int32 targetSdkVersion = 0;
        appInfo->GetTargetSdkVersion(&targetSdkVersion);
        Boolean isHoneycombApp = targetSdkVersion >=
                Build::VERSION_CODES::HONEYCOMB;

        if (isXLarge && isHoneycombApp) {
            return;
        }
    }

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean isOpened;
    if ((cb != NULL) && (cb->OnMenuOpened(st->mFeatureId, st->mMenu, &isOpened), !isOpened)) {
        // Callback doesn't want the menu to open, reset any state
        ClosePanel(st, TRUE);
        return;
    }

    AutoPtr<IWindowManager> wm;
    GetWindowManager((IWindowManager**)&wm);
    if (wm == NULL) {
        return;
    }

    // Prepare panel (should have been done before, but just in case)
    Boolean prepared;
    PreparePanel(st, event, &prepared);
    if (!prepared) {
        return;
    }

    Int32 width = IViewGroupLayoutParams::WRAP_CONTENT;
    if (st->mDecorView == NULL || st->mRefreshDecorView) {
        if (st->mDecorView == NULL) {
            // Initialize the panel decor, this will populate st.decorView
            if (!InitializePanelDecor(st) || (st->mDecorView == NULL)) {
                return;
            }
        }
        else if (st->mRefreshDecorView) {
            Int32 count;
            st->mDecorView->GetChildCount(&count);
            if (count > 0) {
                // Decor needs refreshing, so remove its views
                st->mDecorView->RemoveAllViews();
            }
        }

        // This will populate st.shownPanelView
        if (!InitializePanelContent(st) || !st->HasPanelItems()) {
            return;
        }

        AutoPtr<IViewGroupLayoutParams> lp;
        st->mShownPanelView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        if (lp == NULL) {
            CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                    IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&lp);
        }

        Int32 backgroundResId, lpWidth;
        lp->GetWidth(&lpWidth);
        if (lpWidth == IViewGroupLayoutParams::MATCH_PARENT) {
            // If the contents is fill parent for the width, set the
            // corresponding background
            backgroundResId = st->mFullBackground;
            width = IWindowManagerLayoutParams::MATCH_PARENT;
        }
        else {
            // Otherwise, set the normal panel background
            backgroundResId = st->mBackground;
        }

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resource;
        context->GetResources((IResources**)&resource);
        AutoPtr<IDrawable> drawable;
        resource->GetDrawable(backgroundResId, (IDrawable**)&drawable);

        st->mDecorView->SetWindowBackground(drawable);

        AutoPtr<IViewParent> shownPanelParent;
        st->mShownPanelView->GetParent((IViewParent**)&shownPanelParent);

        if (IViewManager::Probe(shownPanelParent) != NULL) {
            IViewManager::Probe(shownPanelParent)->RemoveView(st->mShownPanelView);
        }

        IViewGroup::Probe((IViewParent*)st->mDecorView)->AddView(st->mShownPanelView, lp);

        /*
         * Give focus to the view, if it or one of its children does not
         * already have it.
         */
        Boolean focus;
        if (st->mShownPanelView->HasFocus(&focus), !focus) {
            Boolean requested;
            st->mShownPanelView->RequestFocus(&requested);
        }
    }
    else if (!st->IsInListMode()) {
        width = IWindowManagerLayoutParams::MATCH_PARENT;
    } else if (st->mCreatedPanelView != NULL) {
        // If we already had a panel view, carry width=MATCH_PARENT through
        // as we did above when it was created.
        AutoPtr<IViewGroupLayoutParams> lp;
        st->mCreatedPanelView->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 lpWidth = 0;
        if (lp != NULL && (lp->GetWidth(&lpWidth), lpWidth) == IViewGroupLayoutParams::MATCH_PARENT) {
            width = IWindowManagerLayoutParams::MATCH_PARENT;
        }
    }

    st->mIsOpen = TRUE;
    st->mIsHandled = FALSE;

    AutoPtr<IWindowManagerLayoutParams> wlp;
    CWindowManagerLayoutParams::New(
            width, IViewGroupLayoutParams::WRAP_CONTENT,
            st->mX, st->mY, IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG,
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            st->mDecorView->mDefaultOpacity, (IWindowManagerLayoutParams**)&wlp);

    if (st->mIsCompact) {
        ((CWindowManagerLayoutParams*)wlp.Get())->mGravity = GetOptionsPanelGravity();
        sRotationWatcher->AddWindow(THIS_PROBE(IWindow));
    } else {
        ((CWindowManagerLayoutParams*)wlp.Get())->mGravity = st->mGravity;
    }

    ((CWindowManagerLayoutParams*)wlp.Get())->mWindowAnimations = st->mWindowAnimations;

    wm->AddView((IView*)st->mDecorView.Get(), wlp);
   // Log.v(TAG, "Adding main menu to window manager.");
}

ECode CPhoneWindow::ClosePanel(
    /* [in] */ Int32 featureId)
{
    Boolean reserved = FALSE;
    if (featureId == FEATURE_OPTIONS_PANEL && mActionBar != NULL &&
            (mActionBar->IsOverflowReserved(&reserved), reserved)) {
        Boolean tmp = FALSE;
        mActionBar->HideOverflowMenu(&tmp);
    }
    else if (featureId == FEATURE_CONTEXT_MENU) {
        CloseContextMenu();
    }
    else {
        AutoPtr<PanelFeatureState> st;
        FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
        ClosePanel(st, TRUE);
    }

    return NOERROR;
}

ECode CPhoneWindow::ClosePanel(
    /* [in] */ PanelFeatureState* st,
    /* [in] */ Boolean doCallback)
{
    Boolean showing = FALSE;
    if (doCallback && st->mFeatureId == FEATURE_OPTIONS_PANEL &&
            mActionBar != NULL && (mActionBar->IsOverflowMenuShowing(&showing), showing)) {
        CheckCloseActionMenu(st->mMenu);
        return NOERROR;
    }

    AutoPtr<IWindowManager> wm;
    GetWindowManager((IWindowManager**)&wm);
    if ((wm != NULL) && st->mIsOpen) {
        if (st->mDecorView != NULL) {
            wm->RemoveView((IView*)st->mDecorView.Get());
            // Log.v(TAG, "Removing main menu from window manager.");
            if (st->mIsCompact) {
                sRotationWatcher->RemoveWindow(THIS_PROBE(IWindow));
            }
        }

        if (doCallback) {
            CallOnPanelClosed(st->mFeatureId, st, NULL);
        }
    }
    st->mIsPrepared = FALSE;
    st->mIsHandled = FALSE;
    st->mIsOpen = FALSE;

    // This view is no longer shown, so null it out
    st->mShownPanelView = NULL;

    if (st->mIsInExpandedMode) {
        // Next time the menu opens, it should not be in expanded mode, so
        // force a refresh of the decor
        st->mRefreshDecorView = TRUE;
        st->mIsInExpandedMode = FALSE;
    }

    if (mPreparedPanel.Get() == st) {
        mPreparedPanel = NULL;
        mPanelChordingKey = 0;
    }
    // add by xihao: there are several circular references
    // CPhoneWindow->PanelFeatureState->CMenuBuilder->Activity->CPhoneWindow
    // CPhoneWindow->PanelFeatureState->DecorView->CPhoneWindow
    // CMenuBuilder->CListMenuPresenter->CMenuBuilder, and so on
    //
    st->SetMenu(NULL);
    mPanels->Set(st->mFeatureId, NULL);

    return NOERROR;
}

void CPhoneWindow::CheckCloseActionMenu(
    /* [in] */ IMenu* menu)
{
    if (mClosingActionMenu) {
        return;
    }

    mClosingActionMenu = TRUE;
    mActionBar->DismissPopupMenus();
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);

    Boolean destoryed = FALSE;
    if (cb != NULL && !(IsDestroyed(&destoryed), destoryed)) {
        cb->OnPanelClosed(FEATURE_ACTION_BAR, menu);
    }

    mClosingActionMenu = FALSE;
}

/**
 * Helper method for calling the {@link Callback#onPanelClosed(int, Menu)}
 * callback. This method will grab whatever extra state is needed for the
 * callback that isn't given in the parameters. If the panel is not open,
 * this will not perform the callback.
 *
 * @param featureId Feature ID of the panel that was closed. Must be given.
 * @param panel Panel that was closed. Optional but useful if there is no
 *            menu given.
 * @param menu The menu that was closed. Optional, but give if you have.
 */
void CPhoneWindow::CallOnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ PanelFeatureState* panel,
    /* [in] */ IMenu* menu)
{
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    if (cb == NULL) {
        return;
    }

    // Try to get a menu
    if (menu == NULL) {
        // Need a panel to grab the menu, so try to get that
        if (panel == NULL) {
            if ((featureId >= 0) && (featureId < mPanels->GetLength())) {
                panel = (*mPanels)[featureId];
            }
        }

        if (panel != NULL) {
            // menu still may be null, which is okay--we tried our best
            menu = panel->mMenu;
        }
    }

    // If the panel is not open, do not callback
    if ((panel != NULL) && (!panel->mIsOpen)) {
        return;
    }

    Boolean destoryed = FALSE;
    if (!(IsDestroyed(&destoryed), destoryed)) {
        cb->OnPanelClosed(featureId, menu);
    }
}

Boolean CPhoneWindow::LaunchDefaultSearch()
{
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean isDestroyed = FALSE;
    if (cb == NULL || (IsDestroyed(&isDestroyed), isDestroyed)) {
        return FALSE;
    }
    else {
        SendCloseSystemWindows(String("search"));
        Boolean value = FALSE;
        cb->OnSearchRequested(&value);
        return value;
    }
}

/**
 * Closes the context menu. This notifies the menu logic of the close, along
 * with dismissing it from the UI.
 */
void CPhoneWindow::CloseContextMenu()
{
    AutoLock lock(&_m_syncLock);

    if (mContextMenu != NULL) {
        mContextMenu->Close();
        DismissContextMenu();
    }
}

/**
 * Dismisses just the context menu UI. To close the context menu, use
 * {@link #closeContextMenu()}.
 */
void CPhoneWindow::DismissContextMenu()
{
    AutoLock lock(&_m_syncLock);

    mContextMenu = NULL;

    if (mContextMenuHelper != NULL) {
        mContextMenuHelper->Dismiss();
        mContextMenuHelper = NULL;
    }
}

ECode CPhoneWindow::TogglePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
    if (st->mIsOpen) {
        ClosePanel(st, TRUE);
    } else {
        OpenPanel(st, event);
    }
    return NOERROR;
}

ECode CPhoneWindow::InvalidatePanelMenu(
    /* [in] */ Int32 featureId)
{
    mInvalidatePanelMenuFeatures |= 1 << featureId;

    if (!mInvalidatePanelMenuPosted && mDecor != NULL) {
        mDecor->PostOnAnimation(mInvalidatePanelMenuRunnable);
        mInvalidatePanelMenuPosted = TRUE;
    }

    return NOERROR;
}

void CPhoneWindow::DoInvalidatePanelMenu(
    /* [in] */ Int32 featureId)
{
    AutoPtr<PanelFeatureState> st;
    ECode ec = GetPanelState(featureId, TRUE, (PanelFeatureState**)&st);

    if (st) {
        AutoPtr<IBundle> savedActionViewStates;
        if (st->mMenu != NULL) {
            CBundle::New((IBundle**)&savedActionViewStates);
            st->mMenu->SaveActionViewStates(savedActionViewStates);

            Int32 size = 0;
            if ((savedActionViewStates->GetSize(&size), size) > 0) {
                st->mFrozenActionViewState = savedActionViewStates;
            }
            // This will be started again when the panel is prepared.
            st->mMenu->StopDispatchingItemsChanged();
            st->mMenu->Clear();
        }

        st->mRefreshMenuContent = TRUE;
        st->mRefreshDecorView = TRUE;
    }

    // Prepare the options panel if we have an action bar
    if ((featureId == FEATURE_ACTION_BAR || featureId == FEATURE_OPTIONS_PANEL) && mActionBar != NULL) {
        st = NULL;
        GetPanelState(IWindow::FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st);
        if (st != NULL) {
            st->mIsPrepared = FALSE;
            Boolean tmp = FALSE;
            PreparePanel(st, NULL, &tmp);
        }
    }
}

ECode CPhoneWindow::CloseAllPanels()
{
    const AutoPtr<IViewManager> wm;
    GetWindowManager((IWindowManager**)&wm);
    if (wm == NULL) {
        return NOERROR;
    }

    const AutoPtr< ArrayOf<PanelFeatureState*> > panels = mPanels;
    const Int32 N = panels != NULL ? panels->GetLength() : 0;
    for (int i = 0; i < N; i++) {
        const AutoPtr<PanelFeatureState> panel = (*panels)[i];
        if (panel != NULL) {
            ClosePanel(panel, TRUE);
        }
    }

    CloseContextMenu();

    return NOERROR;
}

ECode CPhoneWindow::PerformPanelShortcut(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    assert(succeeded != NULL);
    *succeeded = FALSE;

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st))
    *succeeded = PerformPanelShortcut(st, keyCode, event, flags);
    return NOERROR;
}

Boolean CPhoneWindow::PerformPanelShortcut(
    /* [in] */ PanelFeatureState* st,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags)
{
    Boolean tmp = FALSE;
    if ((event->IsSystem(&tmp), tmp) || (st == NULL)) {
        return FALSE;
    }

    Boolean handled = FALSE;

    // Only try to perform menu shortcuts if preparePanel returned true (possible false
    // return value from application not wanting to show the menu).
    if ((st->mIsPrepared || (PreparePanel(st, event, &tmp), tmp)) && st->mMenu != NULL) {
        // The menu is prepared now, perform the shortcut on it
        st->mMenu->PerformShortcut(keyCode, event, flags, &handled);
    }

    if (handled) {
        // Mark as handled
        st->mIsHandled = TRUE;

        // Only close down the menu if we don't have an action bar keeping it open.
        if ((flags & IMenu::FLAG_PERFORM_NO_CLOSE) == 0 && mActionBar == NULL) {
            ClosePanel(st, TRUE);
        }
    }

    return handled;
}

ECode CPhoneWindow::PerformPanelIdentifierAction(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    assert(succeeded != NULL);
    *succeeded = FALSE;

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(GetPanelState(featureId, TRUE, (PanelFeatureState**)&st));
    AutoPtr<IKeyEvent> event;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_MENU, (IKeyEvent**)&event);

    Boolean tmp = FALSE;
    if (!(PreparePanel(st, event, &tmp), tmp)) {
        return NOERROR;
    }

    if (st->mMenu == NULL) {
        return NOERROR;
    }

    st->mMenu->PerformIdentifierAction(id, flags, succeeded);

    // Only close down the menu if we don't have an action bar keeping it open.
    if (mActionBar == NULL) {
        ClosePanel(st, TRUE);
    }

    return NOERROR;
}

ECode CPhoneWindow::PerformContextMenuIdentifierAction(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    if (mContextMenu != NULL) {
        return mContextMenu->PerformIdentifierAction(id, flags, succeeded);
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode CPhoneWindow::SetBackgroundDrawable(
    /* [in] */ IDrawable* drawable)
{
    if (drawable != mBackgroundDrawable || mBackgroundResource != 0) {
        mBackgroundResource = 0;
        mBackgroundDrawable = drawable;
        if (mDecor != NULL) {
            mDecor->SetWindowBackground(drawable);
        }
    }
    return NOERROR;
}

ECode CPhoneWindow::SetFeatureDrawableResource(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        AutoPtr<DrawableFeatureState> st;
        GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
        if (st->mResid != resId) {
            st->mResid = resId;
            st->mUri = NULL;

            AutoPtr<IResources> resource;
            mContext->GetResources((IResources**)&resource);
            AutoPtr<IDrawable> drawable;
            resource->GetDrawable(resId, (IDrawable**)&drawable);
            st->mLocal = drawable;
            UpdateDrawable(featureId, st, FALSE);
        }
    } else {
        SetFeatureDrawable(featureId, NULL);
    }
    return NOERROR;
}

ECode CPhoneWindow::SetFeatureDrawableUri(
    /* [in] */ Int32 featureId,
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        AutoPtr<DrawableFeatureState> st;
        GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
        Boolean isEquals = FALSE;
        if (st->mUri == NULL || !(st->mUri->Equals(uri, &isEquals), isEquals)) {
            st->mResid = 0;
            st->mUri = uri;
            st->mLocal = LoadImageURI(uri);
            UpdateDrawable(featureId, st, FALSE);
        }
    } else {
        SetFeatureDrawable(featureId, NULL);
    }
    return NOERROR;
}

ECode CPhoneWindow::SetFeatureDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
    st->mResid = 0;
    st->mUri = NULL;
    if ((IDrawable*)st->mLocal != drawable) {
        st->mLocal = drawable;
        UpdateDrawable(featureId, st, FALSE);
    }
    return NOERROR;
}

ECode CPhoneWindow::SetFeatureDrawableAlpha(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 alpha)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
    if (st->mAlpha != alpha) {
        st->mAlpha = alpha;
        UpdateDrawable(featureId, st, FALSE);
    }
    return NOERROR;
}

ECode CPhoneWindow::SetFeatureInt(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value)
{
    // XXX Should do more management (as with drawable features) to
    // deal with interactions between multiple window policies.
    UpdateInt(featureId, value, FALSE);
    return NOERROR;
}

ECode CPhoneWindow::TakeKeyEvents(
    /* [in] */ Boolean get)
{
    return mDecor->SetFocusable(get);
}

ECode CPhoneWindow::SuperDispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mDecor->SuperDispatchKeyEvent(event);
    return NOERROR;
}

ECode CPhoneWindow::SuperDispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mDecor->SuperDispatchKeyShortcutEvent(event);
    return NOERROR;
}

ECode CPhoneWindow::SuperDispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mDecor->SuperDispatchTouchEvent(event);
    return NOERROR;
}

ECode CPhoneWindow::SuperDispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mDecor->SuperDispatchTrackballEvent(event);
    return NOERROR;
}

ECode CPhoneWindow::SuperDispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mDecor->SuperDispatchGenericMotionEvent(event);
    return NOERROR;
}

/**
 * A key was pressed down and not handled by anything else in the window.
 *
 * @see #onKeyUp
 * @see android.view.KeyEvent
 */
Boolean CPhoneWindow::OnKeyDown(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    /* ****************************************************************************
     * HOW TO DECIDE WHERE YOUR KEY HANDLING GOES.
     *
     * If your key handling must happen before the app gets a crack at the event,
     * it goes in PhoneWindowManager.
     *
     * If your key handling should happen in all windows, and does not depend on
     * the state of the current application, other than that the current
     * application can override the behavior by handling the event itself, it
     * should go in PhoneFallbackEventHandler.
     *
     * Only if your handling depends on the window, and the fact that it has
     * a DecorView, should it go here.
     * ****************************************************************************/

    AutoPtr<IDispatcherState> dispatcher;
    if (mDecor != NULL) {
        mDecor->GetKeyDispatcherState((IDispatcherState**)&dispatcher);
    }

    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE: {
            // Similar code is in PhoneFallbackEventHandler in case the window
            // doesn't have one of these.  In this case, we execute it here and
            // eat the event instead, because we have mVolumeControlStreamType
            // and they don't.
            AutoPtr<IAudioManager> am = GetAudioManager();
            am->HandleKeyDown(event, mVolumeControlStreamType);
            return TRUE;
        }

        case IKeyEvent::KEYCODE_MENU: {
            Boolean tmp = FALSE;
            OnKeyDownPanel((featureId < 0) ? FEATURE_OPTIONS_PANEL : featureId, event, &tmp);
            return TRUE;
        }

        case IKeyEvent::KEYCODE_BACK: {
            Int32 repeat = 0;
            if ((event->GetRepeatCount(&repeat), repeat) > 0) break;
            if (featureId < 0) break;
            // Currently don't do anything with long press.
            if (dispatcher != NULL) {
                dispatcher->StartTracking(event, (IInterface*)this->Probe(EIID_IInterface));
            }
            return TRUE;
        }

    }

    return FALSE;
}

AutoPtr<IKeyguardManager> CPhoneWindow::GetKeyguardManager()
{
    if (mKeyguardManager == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&mKeyguardManager);
    }

    return mKeyguardManager;
}

AutoPtr<IAudioManager> CPhoneWindow::GetAudioManager()
{
    if (mAudioManager == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> audioService;
        context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
        mAudioManager = IAudioManager::Probe(audioService);
    }

    return mAudioManager;
}

/**
 * A key was released and not handled by anything else in the window.
 *
 * @see #onKeyDown
 * @see android.view.KeyEvent
 */
Boolean CPhoneWindow::OnKeyUp(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IDispatcherState> dispatcher;
    if (mDecor != NULL) {
        mDecor->GetKeyDispatcherState((IDispatcherState**)&dispatcher);
    }

    if (dispatcher != NULL) {
        dispatcher->HandleUpEvent(event);
    }
    //Log.i(TAG, "Key up: repeat=" + event.getRepeatCount()
    //        + " flags=0x" + Integer.toHexString(event.getFlags()));

    switch (keyCode) {
    case IKeyEvent::KEYCODE_VOLUME_UP:
    case IKeyEvent::KEYCODE_VOLUME_DOWN:
    case IKeyEvent::KEYCODE_VOLUME_MUTE: {
            // Similar code is in PhoneFallbackEventHandler in case the window
            // doesn't have one of these.  In this case, we execute it here and
            // eat the event instead, because we have mVolumeControlStreamType
            // and they don't.
            AutoPtr<IAudioManager> am = GetAudioManager();
            am->HandleKeyUp(event, mVolumeControlStreamType);
            return TRUE;
        }
    case IKeyEvent::KEYCODE_MENU:
        {
            OnKeyUpPanel(featureId < 0 ? IWindow::FEATURE_OPTIONS_PANEL : featureId, event);
            return TRUE;
        }
    case IKeyEvent::KEYCODE_BACK:
        {
            if (featureId < 0) break;

            Boolean isTracking, isCanceled;
            event->IsTracking(&isTracking);
            event->IsCanceled(&isCanceled);
            if (isTracking && !isCanceled) {
                if (featureId == IWindow::FEATURE_OPTIONS_PANEL) {
                   AutoPtr<PanelFeatureState> st;
                   GetPanelState(featureId, FALSE, (PanelFeatureState**)&st);
                   if (st != NULL && st->mIsInExpandedMode) {
                       // If the user is in an expanded menu and hits back, it
                       // should go back to the icon menu
                       ReopenMenu(TRUE);
                       return TRUE;
                   }
                }
                ClosePanel(featureId);
                return TRUE;
            }
        }
        break;
    case IKeyEvent::KEYCODE_SEARCH:
        {
            /*
             * Do this in onKeyUp since the Search key is also used for
             * chording quick launch shortcuts.
             */
            Boolean value = FALSE;
            if ((GetKeyguardManager()->InKeyguardRestrictedInputMode(&value), value)) {
                break;
            }
            Boolean isTracking = FALSE;
            Boolean isCanceled = FALSE;
            if ((event->IsTracking(&isTracking), isTracking)
                    && !(event->IsCanceled(&isCanceled), isCanceled)) {
                LaunchDefaultSearch();
            }
            return TRUE;
        }
    }

    return FALSE;
}

void CPhoneWindow::OnActive()
{
}

ECode CPhoneWindow::GetDecorView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    if (mDecor == NULL) {
        InstallDecor();
    }
    *view = mDecor;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CPhoneWindow::PeekDecorView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = (IView*)mDecor;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CPhoneWindow::SaveHierarchyState(
    /* [out] */ IBundle** instanceState)
{
    VALIDATE_NOT_NULL(instanceState);
    AutoPtr<IBundle> state;
    CBundle::New((IBundle**)&state);
    if (mContentParent == NULL) {
        *instanceState = state;
        REFCOUNT_ADD(*instanceState);
        return NOERROR;
    }

    AutoPtr<IObjectInt32Map> states;
    CObjectInt32Map::New((IObjectInt32Map**)&states);
    mContentParent->SaveHierarchyState(states);
    state->PutSparseParcelableArray(VIEWS_TAG, states);

    // save the focused view id
    AutoPtr<IView> focusedView;
    mContentParent->FindFocus((IView**)&focusedView);
    if (focusedView != NULL) {
        Int32 id = 0;
        if ((focusedView->GetId(&id), id) != IView::NO_ID) {
            state->PutInt32(FOCUSED_ID_TAG, id);
        // } else {
        //    if (false) {
        //        Log.d(TAG, "couldn't save which view has focus because the focused view "
        //                 + focusedView + " has no id.");
        //     }
        }
    }

    // save the panels
    AutoPtr<IObjectInt32Map> panelStates;
    CObjectInt32Map::New((IObjectInt32Map**)&panelStates);
    SavePanelState(panelStates);
    Int32 size = 0;
    if ((panelStates->GetSize(&size), size) > 0) {
        state->PutSparseParcelableArray(PANELS_TAG, panelStates);
    }

    if (mActionBar != NULL) {
        AutoPtr<IObjectInt32Map> actionBarStates;
        CObjectInt32Map::New((IObjectInt32Map**)&panelStates);
        mActionBar->SaveHierarchyState(actionBarStates);
        state->PutSparseParcelableArray(ACTION_BAR_TAG, actionBarStates);
    }

    *instanceState = state;
    REFCOUNT_ADD(*instanceState);
    return NOERROR;
}

ECode CPhoneWindow::RestoreHierarchyState(
    /* [in] */ IBundle* savedInstanceState)
{
    if (mContentParent == NULL) {
        return NOERROR;
    }

    AutoPtr<IObjectInt32Map> savedStates;
    savedInstanceState->GetSparseParcelableArray(VIEWS_TAG, (IObjectInt32Map**)&savedStates);
    if (savedStates != NULL) {
        mContentParent->RestoreHierarchyState(savedStates);
    }

    // restore the focused view
    Int32 focusedViewId = 0;
    savedInstanceState->GetInt32(FOCUSED_ID_TAG, IView::NO_ID, &focusedViewId);
    if (focusedViewId != IView::NO_ID) {
        AutoPtr<IView> needsFocus;
        mContentParent->FindViewById(focusedViewId, (IView**)&needsFocus);
        if (needsFocus != NULL) {
            Boolean isRequest = FALSE;
            needsFocus->RequestFocus(&isRequest);
        // } else {
        //     Log.w(TAG,
        //             "Previously focused view reported id " + focusedViewId
        //                         + " during save, but can't be found during restore.");
        }
    }

    // restore the panels
    AutoPtr<IObjectInt32Map> panelStates;
    savedInstanceState->GetSparseParcelableArray(PANELS_TAG, (IObjectInt32Map**)&panelStates);
    if (panelStates != NULL) {
        RestorePanelState(panelStates);
    }

    if (mActionBar != NULL) {
        AutoPtr<IObjectInt32Map> actionBarStates;
        savedInstanceState->GetSparseParcelableArray(ACTION_BAR_TAG, (IObjectInt32Map**)&actionBarStates);
        if (actionBarStates != NULL) {
            mActionBar->RestoreHierarchyState(actionBarStates);
        // } else {
        //     Log.w(TAG, "Missing saved instance states for action bar views! " +
        //             "State will not be restored.");
        }
    }
    return NOERROR;
}

/**
 * Invoked when the panels should freeze their state.
 *
 * @param icicles Save state into this. This is usually indexed by the
 *            featureId. This will be given to {@link #restorePanelState} in the
 *            future.
 */
void CPhoneWindow::SavePanelState(
    /* [in] */ IObjectInt32Map* icicles)
{
    AutoPtr< ArrayOf<PanelFeatureState*> > panels = mPanels;
    if (panels == NULL) {
        return;
    }

    for (Int32 curFeatureId = panels->GetLength() - 1; curFeatureId >= 0; curFeatureId--) {
        if ((*panels)[curFeatureId] != NULL) {
            AutoPtr<IParcelable> savedState = (*panels)[curFeatureId]->OnSaveInstanceState();
            icicles->Put(curFeatureId, savedState);
        }
    }
}

/**
 * Invoked when the panels should thaw their state from a previously frozen state.
 *
 * @param icicles The state saved by {@link #savePanelState} that needs to be thawed.
 */
void CPhoneWindow::RestorePanelState(
    /* [in] */ IObjectInt32Map* icicles)
{

    Int32 curFeatureId = 0;
    Int32 size = 0;
    icicles->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr< ArrayOf<Int32> > keys;
        icicles->GetKeys((ArrayOf<Int32>**)&keys);
        curFeatureId = (*keys)[i];
        AutoPtr<PanelFeatureState> st;
        GetPanelState(curFeatureId, FALSE /* required */, (PanelFeatureState**)&st);
        if (st == NULL) {
            // The panel must not have been required, and is currently not around, skip it
            continue;
        }
        AutoPtr<IParcelable> state;
        icicles->Get(curFeatureId, (IInterface**)&state);
        st->OnRestoreInstanceState(state);
        InvalidatePanelMenu(curFeatureId);
    }

    /*
     * Implementation note: call openPanelsAfterRestore later to actually open the
     * restored panels.
     */
}

AutoPtr<CPhoneWindow::DecorView> CPhoneWindow::GenerateDecor()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<CPhoneWindow::DecorView> decor = new DecorView(this, context.Get(), -1);
    return decor;
}

ECode CPhoneWindow::GenerateLayout(
    /* [in] */ DecorView* decor,
    /* [out] */ IViewGroup** viewGroup)
{
    VALIDATE_NOT_NULL(viewGroup);
    *viewGroup = NULL;

    // Apply data from current theme.

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(GetWindowStyle((ITypedArray**)&a));

//    if (FALSE) {
//        System.out.println("From style:");
//        String s = "Attrs:";
//        for (Int32 i = 0; i < com.android.internal.R.styleable.Window.length; i++) {
//            s = s + " " + Integer.toHexString(com.android.internal.R.styleable.Window[i]) + "="
//                    + a.getString(i);
//        }
//        System.out.println(s);
//    }

    a->GetBoolean(R::styleable::Window_windowIsFloating,
            FALSE, &mIsFloating);
    Int32 flagsToUpdate = (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR)
            & (~GetForcedWindowFlags());
    if (mIsFloating) {
        SetLayout(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
        SetFlags(0, flagsToUpdate);
    }
    else {
        SetFlags(IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR, flagsToUpdate);
    }

    Boolean value;
    a->GetBoolean(R::styleable::Window_windowNoTitle,
            FALSE, &value);
    if (value) {
        RequestFeature(IWindow::FEATURE_NO_TITLE, &value);
    }
    else if (a->GetBoolean(R::styleable::Window_windowActionBar, FALSE, &value), value) {
        // Don't allow an action bar if there is no title.
        RequestFeature(FEATURE_ACTION_BAR, &value);
    }

    if (a->GetBoolean(R::styleable::Window_windowActionBarOverlay, FALSE, &value), value) {
        RequestFeature(FEATURE_ACTION_BAR_OVERLAY, &value);
    }

    if (a->GetBoolean(R::styleable::Window_windowActionModeOverlay, FALSE, &value), value) {
        RequestFeature(FEATURE_ACTION_MODE_OVERLAY, &value);
    }

    a->GetBoolean(R::styleable::Window_windowFullscreen,
            FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN,
                IWindowManagerLayoutParams::FLAG_FULLSCREEN & (~GetForcedWindowFlags()));
    }

    a->GetBoolean(R::styleable::Window_windowShowWallpaper,
            FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER,
                IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER & (~GetForcedWindowFlags()));
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IApplicationInfo> appInfo;
    context->GetApplicationInfo((IApplicationInfo**)&appInfo);
    assert(appInfo != NULL);

    Int32 targetSdkVersion = 0;
    appInfo->GetTargetSdkVersion(&targetSdkVersion);

    if (a->GetBoolean(R::styleable::Window_windowEnableSplitTouch, targetSdkVersion
                    >= Build::VERSION_CODES::HONEYCOMB, &value), value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH&(~GetForcedWindowFlags()));
    }

    a->GetValue(R::styleable::Window_windowMinWidthMajor, mMinWidthMajor, &value);
    a->GetValue(R::styleable::Window_windowMinWidthMinor, mMinWidthMinor, &value);

    Boolean has = FALSE;
    if (a->HasValue(R::styleable::Window_windowFixedWidthMajor, &has), has) {
        if (mFixedWidthMajor == NULL) {
            CTypedValue::New((ITypedValue**)&mFixedWidthMajor);
        }

        a->GetValue(R::styleable::Window_windowFixedWidthMajor, mFixedWidthMajor, &value);
    }
    if (a->HasValue(R::styleable::Window_windowFixedWidthMinor, &has), has) {
        if (mFixedWidthMinor == NULL) {
            CTypedValue::New((ITypedValue**)&mFixedWidthMinor);
        }

        a->GetValue(R::styleable::Window_windowFixedWidthMinor, mFixedWidthMinor, &value);
    }
    if (a->HasValue(R::styleable::Window_windowFixedHeightMajor, &has), has) {
        if (mFixedHeightMajor == NULL) {
            CTypedValue::New((ITypedValue**)&mFixedHeightMajor);
        }

        a->GetValue(R::styleable::Window_windowFixedHeightMajor, mFixedHeightMajor, &value);
    }
    if (a->HasValue(R::styleable::Window_windowFixedHeightMinor, &has), has) {
        if (mFixedHeightMinor == NULL) {
            CTypedValue::New((ITypedValue**)&mFixedHeightMinor);
        }

        a->GetValue(R::styleable::Window_windowFixedHeightMinor, mFixedHeightMinor, &value);
    }

    const Boolean targetPreHoneycomb = targetSdkVersion < Build::VERSION_CODES::HONEYCOMB;
    const Boolean targetPreIcs = targetSdkVersion < Build::VERSION_CODES::ICE_CREAM_SANDWICH;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean targetHcNeedsOptions = FALSE;
    res->GetBoolean(R::bool_::target_honeycomb_needs_options_menu, &targetHcNeedsOptions);

    Boolean has1 = FALSE, has2 = FALSE;
    Boolean noActionBar = !(HasFeature(FEATURE_ACTION_BAR, &has1), has1)
                        || (HasFeature(FEATURE_NO_TITLE, &has2), has2);

    if (targetPreHoneycomb || (targetPreIcs && targetHcNeedsOptions && noActionBar)) {
        AddFlags(IWindowManagerLayoutParams::FLAG_NEEDS_MENU_KEY);
    } else {
        ClearFlags(IWindowManagerLayoutParams::FLAG_NEEDS_MENU_KEY);
    }

    if (mAlwaysReadCloseOnTouchAttr || targetSdkVersion >= Build::VERSION_CODES::HONEYCOMB) {
        Boolean tmp = FALSE;
        if (a->GetBoolean(R::styleable::Window_windowCloseOnTouchOutside, FALSE, &tmp), tmp) {
            SetCloseOnTouchOutsideIfNotSet(TRUE);
        }
    }

    AutoPtr<IWindowManagerLayoutParams> _params;
    GetAttributes((IWindowManagerLayoutParams**)&_params);
    CWindowManagerLayoutParams* params = (CWindowManagerLayoutParams*)_params.Get();

    if (!HasSoftInputMode()) {
        a->GetInt32(R::styleable::Window_windowSoftInputMode,
                params->mSoftInputMode, &(params->mSoftInputMode));
    }

    a->GetBoolean(R::styleable::Window_backgroundDimEnabled,
            mIsFloating, &value);
    if (value) {
        /* All dialogs should have the window dimmed */
        if ((GetForcedWindowFlags() & IWindowManagerLayoutParams::FLAG_DIM_BEHIND) == 0) {
            params->mFlags |= IWindowManagerLayoutParams::FLAG_DIM_BEHIND;
        }

        if (!HaveDimAmount()) {
            a->GetFloat(R::styleable::Window_backgroundDimAmount,
                    0.5f, &(params->mDimAmount));
        }
    }

    if (params->mWindowAnimations == 0) {
        a->GetResourceId(R::styleable::Window_windowAnimationStyle,
                0, &(params->mWindowAnimations));
    }

    // The rest are only done if this window is not embedded; otherwise,
    // the values are inherited from our container.
    AutoPtr<IWindow> container;
    GetContainer((IWindow**)&container);
    if (container == NULL) {
        if (mBackgroundDrawable == NULL) {
            if (mBackgroundResource == 0) {
                a->GetResourceId(R::styleable::Window_windowBackground,
                        0, &mBackgroundResource);
            }
            if (mFrameResource == 0) {
                a->GetResourceId(R::styleable::Window_windowFrame,
                        0, &mFrameResource);
            }
//            if (FALSE) {
//                System.out.println("Background: "
//                        + Integer.toHexString(mBackgroundResource) + " Frame: "
//                        + Integer.toHexString(mFrameResource));
//            }
        }
        a->GetColor(R::styleable::Window_textColor, 0xFF000000, &mTextColor);
    }

    // Inflate the window decor.
    Int32 layoutResource = 0;
    Int32 features = GetLocalFeatures();
    // System.out.println("Features: 0x" + Integer.toHexString(features));
    if ((features & ((1 << IWindow::FEATURE_LEFT_ICON) | (1 << IWindow::FEATURE_RIGHT_ICON))) != 0) {
        if (mIsFloating) {
            AutoPtr<ITypedValue> res;
            CTypedValue::New((ITypedValue**)&res);
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);

            AutoPtr<IResourcesTheme> theme;
            ctx->GetTheme((IResourcesTheme**)&theme);
            Boolean tmp = FALSE;
            theme->ResolveAttribute(R::attr::dialogTitleIconsDecorLayout, res, TRUE, &tmp);
            res->GetResourceId(&layoutResource);
        } else {
            layoutResource = R::layout::screen_title_icons;
        }

        // XXX Remove this once action bar supports these features.
        RemoveFeature(FEATURE_ACTION_BAR);
        // System.out.println("Title Icons!");
    } else if ((features & ((1 << IWindow::FEATURE_PROGRESS)
        | (1 << IWindow::FEATURE_INDETERMINATE_PROGRESS))) != 0
        && (features & (1 << FEATURE_ACTION_BAR)) == 0) {
        // Special case for a window with only a progress bar (and title).
        // XXX Need to have a no-title version of embedded windows.
        layoutResource = R::layout::screen_progress;
        // System.out.println("Progress!");
    } else if ((features & (1 << IWindow::FEATURE_CUSTOM_TITLE)) != 0) {
        // Special case for a window with a custom title.
        // If the window is floating, we need a dialog layout
        if (mIsFloating) {
            AutoPtr<ITypedValue> res;
            CTypedValue::New((ITypedValue**)&res);

            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);

            AutoPtr<IResourcesTheme> theme;
            ctx->GetTheme((IResourcesTheme**)&theme);

            Boolean tmp = FALSE;
            theme->ResolveAttribute(R::attr::dialogCustomTitleDecorLayout, res, TRUE, &tmp);
            res->GetResourceId(&layoutResource);
        } else {
            layoutResource = R::layout::screen_custom_title;
        }
    } else if ((features & (1 << IWindow::FEATURE_NO_TITLE)) == 0) {
        // If no other features and not embedded, only need a title.
        // If the window is floating, we need a dialog layout
        if (mIsFloating) {
            AutoPtr<ITypedValue> res;
            CTypedValue::New((ITypedValue**)&res);

            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);

            AutoPtr<IResourcesTheme> theme;
            ctx->GetTheme((IResourcesTheme**)&theme);

            Boolean tmp = FALSE;
            theme->ResolveAttribute(R::attr::dialogTitleDecorLayout, res, TRUE, &tmp);
            res->GetResourceId(&layoutResource);
        }
        else if ((features & (1 << FEATURE_ACTION_BAR)) != 0) {
            if ((features & (1 << FEATURE_ACTION_BAR_OVERLAY)) != 0) {
                layoutResource = R::layout::screen_action_bar_overlay;
            } else {
                layoutResource = R::layout::screen_action_bar;
            }
        }
        else {
            layoutResource = R::layout::screen_title;
        }
        // System.out.println("Title!");
    }
    else if ((features & (1 << FEATURE_ACTION_MODE_OVERLAY)) != 0) {
        layoutResource = R::layout::screen_simple_overlay_action_mode;
    }
    else {
        // Embedded, so no decoration is needed.
        layoutResource = R::layout::screen_simple;
        // System.out.println("Simple!");
    }

    mDecor->StartChanging();

    AutoPtr<IView> in;
    mLayoutInflater->Inflate(layoutResource, NULL, (IView**)&in);

    AutoPtr<IViewGroupLayoutParams> vparams;
    CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            (IViewGroupLayoutParams**)&vparams);
    decor->AddView(in.Get(), vparams.Get());

    AutoPtr<IView> tmp;
    FindViewById(ID_ANDROID_CONTENT, (IView**)&tmp);
    if (tmp == NULL) {
        // Logger::E("CPhoneWindow", "Window couldn't find content container view");
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IViewGroup> contentParent = (IViewGroup*)tmp->Probe(EIID_IViewGroup);
    if (contentParent == NULL) {
        // Logger::E("CPhoneWindow", "Window couldn't find content container view");
        return E_RUNTIME_EXCEPTION;
    }

    if ((features & (1 << IWindow::FEATURE_INDETERMINATE_PROGRESS)) != 0) {
        AutoPtr<IProgressBar> progress = GetCircularProgressBar(FALSE);
        if (progress != NULL) {
            progress->SetIndeterminate(TRUE);
        }
    }

    // Remaining setup -- of background and title -- that only applies
    // to top-level windows.
    container = NULL;
    GetContainer((IWindow**)&container);
    if (container == NULL) {
        AutoPtr<IDrawable> drawable = mBackgroundDrawable;
        if (mBackgroundResource != 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            drawable = NULL;
            res->GetDrawable(mBackgroundResource, (IDrawable**)&drawable);
        }
        mDecor->SetWindowBackground(drawable);
        drawable = NULL;
        if (mFrameResource != 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetDrawable(mFrameResource, (IDrawable**)&drawable);
        }
        mDecor->SetWindowFrame(drawable);

        // System.out.println("Text=" + Integer.toHexString(mTextColor) +
        // " Sel=" + Integer.toHexString(mTextSelectedColor) +
        // " Title=" + Integer.toHexString(mTitleColor));

        if (mTitleColor == 0) {
            mTitleColor = mTextColor;
        }

        if (mTitle != NULL) {
            SetTitle(mTitle);
        }
        SetTitleColor(mTitleColor);
    }

    mDecor->FinishChanging();

    *viewGroup = contentParent.Get();
    REFCOUNT_ADD(*viewGroup);

    return NOERROR;
}

ECode CPhoneWindow::SetCloseOnTouchOutsideIfNotSet(
    /* [in] */ Boolean close)
{
    return Window::SetCloseOnTouchOutsideIfNotSet(close);
}

ECode CPhoneWindow::AlwaysReadCloseOnTouchAttr()
{
    mAlwaysReadCloseOnTouchAttr = TRUE;
    return NOERROR;
}

ECode CPhoneWindow::Destroy()
{
    return Window::Destroy();
}

ECode CPhoneWindow::SetWindowManager(
    /* [in] */ IWindowManager* wm,
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& appName,
    /* [in] */ Boolean hardwareAccelerated)
{
    return Window::SetWindowManager(wm, appToken, appName, hardwareAccelerated);
}

ECode CPhoneWindow::AdjustLayoutParamsForSubWindow(
    /* [in] */ IWindowManagerLayoutParams* wp)
{
    return Window::AdjustLayoutParamsForSubWindow(wp);
}

ECode CPhoneWindow::SetDimAmount(
    /* [in] */ Float amount)
{
    return Window::SetDimAmount(amount);
}

ECode CPhoneWindow::SetCloseOnTouchOutside(
    /* [in] */ Boolean close)
{
    return Window::SetCloseOnTouchOutside(close);
}

ECode CPhoneWindow::ShouldCloseOnTouch(
    /* [in] */ IContext* context,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    return Window::ShouldCloseOnTouch(context, event, res);
}

ECode CPhoneWindow::SetUiOptions(
    /* [in] */ Int32 uiOptions,
    /* [in] */ Int32 mask)
{
    return Window::SetUiOptions(uiOptions, mask);
}

void CPhoneWindow::InstallDecor()
{
    if (mDecor == NULL) {
        mDecor = GenerateDecor();
        mDecor->_AddRef();
        mDecor->SetDescendantFocusability(ViewGroup::FOCUS_AFTER_DESCENDANTS);
        mDecor->SetIsRootNamespace(TRUE);

        if (!mInvalidatePanelMenuPosted && mInvalidatePanelMenuFeatures != 0) {
            mDecor->PostOnAnimation(mInvalidatePanelMenuRunnable);
        }
    }

    if (mContentParent == NULL) {
        ASSERT_SUCCEEDED(GenerateLayout(mDecor, (IViewGroup**)&mContentParent));

        // Set up decor part of UI to ignore fitsSystemWindows if appropriate.
        mDecor->MakeOptionalFitsSystemWindows();

        FindViewById(R::id::title, (IView**)&mTitleView);
        if (mTitleView != NULL) {
            Int32 direction = 0;
            mDecor->GetLayoutDirection(&direction);
            mTitleView->SetLayoutDirection(direction);
            if ((GetLocalFeatures() & (1 << IWindow::FEATURE_NO_TITLE)) != 0) {
                AutoPtr<IView> titleContainer;
                FindViewById(R::id::title_container,
                        (IView**)&titleContainer);
                if (titleContainer != NULL) {
                    titleContainer->SetVisibility(IView::GONE);
                }
                else {
                    mTitleView->SetVisibility(IView::GONE);
                }
                if (IFrameLayout::Probe(mContentParent) != NULL) {
                    IFrameLayout::Probe(mContentParent)->SetForeground(NULL);
                }
            }
            else {
                mTitleView->SetText(mTitle);
            }
        }
        else {
            mActionBar = NULL;
            FindViewById(R::id::action_bar, (IView**)&mActionBar);
            if (mActionBar != NULL) {
                AutoPtr<IWindowCallback> cb;
                GetCallback((IWindowCallback**)&cb);
                mActionBar->SetWindowCallback(cb);

                AutoPtr<ICharSequence> title;
                mActionBar->GetTitle((ICharSequence**)&title);
                if (title == NULL) {
                    mActionBar->SetWindowTitle(mTitle);
                }

                Int32 localFeatures = GetLocalFeatures();
                if ((localFeatures & (1 << FEATURE_PROGRESS)) != 0) {
                    mActionBar->InitProgress();
                }
                if ((localFeatures & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0) {
                    mActionBar->InitIndeterminateProgress();
                }

                Boolean splitActionBar = FALSE;
                Boolean splitWhenNarrow =
                        (mUiOptions & IActivityInfo::UIOPTION_SPLIT_ACTION_BAR_WHEN_NARROW) != 0;
                if (splitWhenNarrow) {
                    AutoPtr<IContext> context;
                    GetContext((IContext**)&context);

                    AutoPtr<IResources> res;
                    context->GetResources((IResources**)&res);
                    res->GetBoolean(R::bool_::split_action_bar_is_narrow, &splitActionBar);
                } else {
                    AutoPtr<ITypedArray> a;
                    GetWindowStyle((ITypedArray**)&a);
                    a->GetBoolean(R::styleable::Window_windowSplitActionBar, FALSE, &splitActionBar);
                }

                AutoPtr<IActionBarContainer> splitView;
                FindViewById(R::id::split_action_bar, (IView**)&splitView);
                if (splitView != NULL) {
                    mActionBar->SetSplitView(splitView);
                    mActionBar->SetSplitActionBar(splitActionBar);
                    mActionBar->SetSplitWhenNarrow(splitWhenNarrow);

                    AutoPtr<IActionBarContextView> cab;
                    FindViewById(R::id::action_context_bar, (IView**)&cab);
                    cab->SetSplitView(splitView);
                    cab->SetSplitActionBar(splitActionBar);
                    cab->SetSplitWhenNarrow(splitWhenNarrow);
                } else if (splitActionBar) {
                    // Log.e(TAG, "Requested split action bar with " +
                    //         "incompatible window decor! Ignoring request.");
                }

                // Post the panel invalidate for later; avoid application onCreateOptionsMenu
                // being called in the middle of onCreate or similar.
                AutoPtr<IRunnable> r = new DecorRunnable(this);
                Boolean result;
                mDecor->Post(r, &result);
            }
        }
    }
}

ECode CPhoneWindow::HasFeature(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* hasFeature)
{
    assert(hasFeature != NULL);
    return Window::HasFeature(feature, hasFeature);
}

ECode CPhoneWindow::SetChildDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
    st->mChild = drawable;
    UpdateDrawable(featureId, st, FALSE);
    return NOERROR;
}

ECode CPhoneWindow::SetChildInt(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value)
{
    UpdateInt(featureId, value, FALSE);
    return NOERROR;
}

ECode CPhoneWindow::IsShortcutKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* isShortcutKey)
{
    VALIDATE_NOT_NULL(isShortcutKey);
    *isShortcutKey = FALSE;

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st));
    if (st->mMenu != NULL) {
        st->mMenu->IsShortcutKey(keyCode, event, isShortcutKey);
    }
    return NOERROR;
}

void CPhoneWindow::UpdateDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ DrawableFeatureState* st,
    /* [in] */ Boolean fromResume)
{
    // Do nothing if the decor is not yet installed... an update will
    // need to be forced when we eventually become active.
    if (mContentParent == NULL) {
        return;
    }

    const Int32 featureMask = 1 << featureId;

    if ((GetFeatures() & featureMask) == 0 && !fromResume) {
        return;
    }

    AutoPtr<IDrawable> drawable;
    if (st != NULL) {
        drawable = st->mChild;
        if (drawable == NULL)
            drawable = st->mLocal;
        if (drawable == NULL)
            drawable = st->mDef;
    }

    if ((GetLocalFeatures() & featureMask) == 0) {
        AutoPtr<IWindow> w;
        if ((GetContainer((IWindow**)&w), w) != NULL) {
            Boolean active = FALSE;
            if ((IsActive(&active), active) || fromResume) {
                w->SetChildDrawable(featureId, drawable);
            }
        }
    } else if (st != NULL && (st->mCur != drawable || st->mCurAlpha != st->mAlpha)) {
        // System.out.println("Drawable changed: old=" + st.cur
        // + ", new=" + drawable);
        st->mCur = drawable;
        st->mCurAlpha = st->mAlpha;

        OnDrawableChanged(featureId, drawable, st->mAlpha);
    }
}

void CPhoneWindow::OnDrawableChanged(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 alpha)
{
    AutoPtr<IImageView> view;
    if (featureId == FEATURE_LEFT_ICON) {
        view = GetLeftIconView();
    } else if (featureId == FEATURE_RIGHT_ICON) {
        view = GetRightIconView();
    } else {
        return;
    }

    if (drawable != NULL) {
        drawable->SetAlpha(alpha);
        view->SetImageDrawable(drawable);
        view->SetVisibility(IView::VISIBLE);
    } else {
        view->SetVisibility(IView::GONE);
    }
}

void CPhoneWindow::OnIntChanged(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value)
{
    if (featureId == FEATURE_PROGRESS || featureId == FEATURE_INDETERMINATE_PROGRESS) {
        UpdateProgressBars(value);
    } else if (featureId == FEATURE_CUSTOM_TITLE) {
        AutoPtr<IView> view;
        FindViewById(R::id::title_container, (IView**)&view);
        IFrameLayout* titleContainer = IFrameLayout::Probe(view.Get());
        if (titleContainer != NULL) {
            AutoPtr<IView> resultView;
            mLayoutInflater->Inflate(value, titleContainer, (IView**)&resultView);
        }
    }
}

void CPhoneWindow::SendCloseSystemWindows()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CPhoneWindowManager::SendCloseSystemWindows(context, String(NULL));
}

void CPhoneWindow::SendCloseSystemWindows(
    /* [in] */ const String& reason)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CPhoneWindowManager::SendCloseSystemWindows(context, reason);
}

void CPhoneWindow::UpdateProgressBars(
   /* [in] */ Int32 value)
{
    AutoPtr<IProgressBar> circularProgressBar = GetCircularProgressBar(TRUE);
    AutoPtr<IProgressBar> horizontalProgressBar = GetHorizontalProgressBar(TRUE);

    const Int32 features = GetLocalFeatures();
    if (value == PROGRESS_VISIBILITY_ON) {
        if ((features & (1 << FEATURE_PROGRESS)) != 0) {
            Int32 level = 0;
            horizontalProgressBar->GetProgress(&level);
            Boolean isIndeterminate = FALSE;
            horizontalProgressBar->IsIndeterminate(&isIndeterminate);
            Int32 visibility = (isIndeterminate || level < 10000) ?
                    IView::VISIBLE : IView::INVISIBLE;
            horizontalProgressBar->SetVisibility(visibility);
        }
        if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0) {
            circularProgressBar->SetVisibility(IView::VISIBLE);
        }
    } else if (value == PROGRESS_VISIBILITY_OFF) {
        if ((features & (1 << FEATURE_PROGRESS)) != 0) {
            horizontalProgressBar->SetVisibility(IView::GONE);
        }
        if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0) {
            circularProgressBar->SetVisibility(IView::GONE);
        }
    } else if (value == PROGRESS_INDETERMINATE_ON) {
        horizontalProgressBar->SetIndeterminate(TRUE);
    } else if (value == PROGRESS_INDETERMINATE_OFF) {
        horizontalProgressBar->SetIndeterminate(FALSE);
    } else if (PROGRESS_START <= value && value <= PROGRESS_END) {
        // We want to set the progress value before testing for visibility
        // so that when the progress bar becomes visible again, it has the
        // correct level.
        horizontalProgressBar->SetProgress(value - PROGRESS_START);

        if (value < PROGRESS_END) {
            ShowProgressBars(horizontalProgressBar, circularProgressBar);
        } else {
            HideProgressBars(horizontalProgressBar, circularProgressBar);
        }
    } else if (PROGRESS_SECONDARY_START <= value && value <= PROGRESS_SECONDARY_END) {
        horizontalProgressBar->SetSecondaryProgress(value - PROGRESS_SECONDARY_START);

        ShowProgressBars(horizontalProgressBar, circularProgressBar);
    }
}

void CPhoneWindow::ShowProgressBars(
    /* [in] */ IProgressBar* horizontalProgressBar,
    /* [in] */ IProgressBar* spinnyProgressBar)
{
    const Int32 features = GetLocalFeatures();
    Int32 visibility = 0;
    if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0 &&
            (spinnyProgressBar->GetVisibility(&visibility), visibility) == IView::INVISIBLE) {
        spinnyProgressBar->SetVisibility(IView::VISIBLE);
    }
    // Only show the progress bars if the primary progress is not complete
    Int32 progress = 0;
    horizontalProgressBar->GetProgress(&progress);
    if ((features & (1 << FEATURE_PROGRESS)) != 0 &&
            progress < 10000) {
        horizontalProgressBar->SetVisibility(IView::VISIBLE);
    }
}

void CPhoneWindow::HideProgressBars(
    /* [in] */ IProgressBar*  horizontalProgressBar,
    /* [in] */ IProgressBar*  spinnyProgressBar)
{
    const Int32 features = GetLocalFeatures();
    AutoPtr<IAnimationUtils> animUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animUtils);
    AutoPtr<IAnimation> anim;
    animUtils->LoadAnimation(mContext, R::anim::fade_out, (IAnimation**)&anim);
    anim->SetDuration(1000);
    Int32 spBarVisibility = 0;
    if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0 &&
            (spinnyProgressBar->GetVisibility(&spBarVisibility), spBarVisibility) == IView::VISIBLE) {
        spinnyProgressBar->StartAnimation(anim);
        spinnyProgressBar->SetVisibility(IView::INVISIBLE);
    }
    Int32 hpBarVisibale =0;
    if ((features & (1 << FEATURE_PROGRESS)) != 0 &&
            (horizontalProgressBar->GetVisibility(&hpBarVisibale), hpBarVisibale) == IView::VISIBLE) {
        horizontalProgressBar->StartAnimation(anim);
        horizontalProgressBar->SetVisibility(IView::INVISIBLE);
    }
}

void CPhoneWindow::UpdateInt(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value,
    /* [in] */ Boolean fromResume)
{
    // Do nothing if the decor is not yet installed... an update will
    // need to be forced when we eventually become active.
    if (mContentParent == NULL) {
        return;
    }

    const Int32 featureMask = 1 << featureId;

    if ((GetFeatures() & featureMask) == 0 && !fromResume) {
        return;
    }

    if ((GetLocalFeatures() & featureMask) == 0) {
        AutoPtr<IWindow> w;
        if ((GetContainer((IWindow**)&w), w) != NULL) {
            w->SetChildInt(featureId, value);
        }
    } else {
        OnIntChanged(featureId, value);
    }
}

AutoPtr<IImageView> CPhoneWindow::GetLeftIconView()
{
    if (mLeftIconView != NULL) {
        return mLeftIconView;
    }
    if (mContentParent == NULL) {
        InstallDecor();
    }

    FindViewById(R::id::left_icon, (IView**)&mLeftIconView);
    return mLeftIconView;
}

AutoPtr<IProgressBar> CPhoneWindow::GetCircularProgressBar(
    /* [in] */ Boolean shouldInstallDecor)
{
    if (mCircularProgressBar != NULL) {
        return mCircularProgressBar;
    }
    if (mContentParent == NULL && shouldInstallDecor) {
        InstallDecor();
    }

    FindViewById(R::id::progress_circular, (IView**)&mCircularProgressBar);
    if (mCircularProgressBar != NULL) {
        mCircularProgressBar->SetVisibility(IView::INVISIBLE);
    }
    return mCircularProgressBar;
}

AutoPtr<IProgressBar> CPhoneWindow::GetHorizontalProgressBar(
    /* [in] */ Boolean shouldInstallDecor)
{
    if (mHorizontalProgressBar != NULL) {
        return mHorizontalProgressBar;
    }
    if (mContentParent == NULL && shouldInstallDecor) {
        InstallDecor();
    }

    FindViewById(R::id::progress_horizontal, (IView**)&mHorizontalProgressBar);
    if (mHorizontalProgressBar != NULL) {
        mHorizontalProgressBar->SetVisibility(IView::INVISIBLE);
    }
    return mHorizontalProgressBar;
}

AutoPtr<IImageView> CPhoneWindow::GetRightIconView()
{
    if (mRightIconView != NULL) {
        return mRightIconView;
    }
    if (mContentParent == NULL) {
        InstallDecor();
    }

    FindViewById(R::id::right_icon, (IView**)&mRightIconView);
    return mRightIconView;
}

//@Override
ECode CPhoneWindow::SetVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    mVolumeControlStreamType = streamType;
    return NOERROR;
}

//@Override
ECode CPhoneWindow::GetVolumeControlStream(
    /* [out] */ Int32* streamType)
{
    VALIDATE_NOT_NULL(streamType);
    *streamType = mVolumeControlStreamType;
    return NOERROR;
}

ECode CPhoneWindow::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    return Window::GetContext(context);
}

ECode CPhoneWindow::GetWindowStyle(
    /* [out] */ ITypedArray** attrs)
{
    VALIDATE_NOT_NULL(attrs);
    return Window::GetWindowStyle(attrs);
}

ECode CPhoneWindow::GetContainer(
    /* [out] */ IWindow** container)
{
    VALIDATE_NOT_NULL(container);
    return Window::GetContainer(container);
}

ECode CPhoneWindow::IsDestroyed(
    /* [out] */ Boolean* destroyed)
{
    VALIDATE_NOT_NULL(destroyed);
    return Window::IsDestroyed(destroyed);
}

ECode CPhoneWindow::HasChildren(
    /* [out] */  Boolean* hasChildren)
{
    VALIDATE_NOT_NULL(hasChildren);
    return Window::HasChildren(hasChildren);
}

ECode CPhoneWindow::SetWindowManager(
    /* [in] */ IWindowManager* wm,
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& appName)
{
    return Window::SetWindowManager(wm, appToken, appName);
}

ECode CPhoneWindow::GetWindowManager(
    /* [out] */ IWindowManager** wm)
{
    return Window::GetWindowManager(wm);
}

ECode CPhoneWindow::SetCallback(
    /* [in] */ IWindowCallback* cb)
{
    return Window::SetCallback(cb);
}

ECode CPhoneWindow::GetCallback(
    /* [out] */ IWindowCallback** cb)
{
    VALIDATE_NOT_NULL(cb);
    return Window::GetCallback(cb);
}

ECode CPhoneWindow::SetLayout(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return Window::SetLayout(width, height);
}

ECode CPhoneWindow::SetGravity(
    /* [in] */ Int32 gravity)
{
    return Window::SetGravity(gravity);
}

ECode CPhoneWindow::SetType(
    /* [in] */ Int32 type)
{
    return Window::SetType(type);
}

ECode CPhoneWindow::SetFormat(
    /* [in] */ Int32 format)
{
    return Window::SetFormat(format);
}

ECode CPhoneWindow::SetWindowAnimations(
    /* [in] */ Int32 resId)
{
    return Window::SetWindowAnimations(resId);
}

ECode CPhoneWindow::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    return Window::SetSoftInputMode(mode);
}

ECode CPhoneWindow::AddFlags(
    /* [in] */ Int32 flags)
{
    return Window::AddFlags(flags);
}

ECode CPhoneWindow::ClearFlags(
    /* [in] */ Int32 flags)
{
    return Window::ClearFlags(flags);
}

ECode CPhoneWindow::SetFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    return Window::SetFlags(flags, mask);
}

ECode CPhoneWindow::SetAttributes(
    /* [in] */ IWindowManagerLayoutParams* a)
{
    return Window::SetAttributes(a);
}

ECode CPhoneWindow::GetAttributes(
    /* [out] */ IWindowManagerLayoutParams** params)
{
    return Window::GetAttributes(params);
}

ECode CPhoneWindow::MakeActive()
{
    return Window::MakeActive();
}

ECode CPhoneWindow::IsActive(
    /* [out] */ Boolean* isActive)
{
    VALIDATE_NOT_NULL(isActive);
    return Window::IsActive(isActive);
}

ECode CPhoneWindow::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return Window::FindViewById(id, view);
}

ECode CPhoneWindow::SetBackgroundDrawableResource(
    /* [in] */ Int32 resid)
{
    return Window::SetBackgroundDrawableResource(resid);
}

/**
 * Prepares the panel to either be opened or chorded. This creates the Menu
 * instance for the panel and populates it via the Activity callbacks.
 *
 * @param st The panel state to prepare.
 * @param event The event that triggered the preparing of the panel.
 * @return Whether the panel was prepared. If the panel should not be shown,
 *         returns false.
 */
ECode CPhoneWindow::PreparePanel(
    /* [in] */ PanelFeatureState* st,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* prepared)
{
    VALIDATE_NOT_NULL(prepared)
    *prepared = FALSE;
    VALIDATE_NOT_NULL(st)

    Boolean destoryed = FALSE;
    if (IsDestroyed(&destoryed), destoryed) {
        return NOERROR;
    }

    // Already prepared (isPrepared will be reset to false later)
    if (st->mIsPrepared) {
        *prepared = TRUE;
        return NOERROR;
    }

    if ((mPreparedPanel != NULL) && (mPreparedPanel.Get() != st)) {
        // Another Panel is prepared and possibly open, so close it
        ClosePanel(mPreparedPanel, FALSE);
    }

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);

    if (cb != NULL) {
        AutoPtr<IView> view;
        cb->OnCreatePanelView(st->mFeatureId, (IView**)&view);
        st->mCreatedPanelView = view;
    }

    if (st->mCreatedPanelView == NULL) {
        // Init the panel state's menu--return false if init failed
        if (st->mMenu == NULL || st->mRefreshMenuContent) {
            if (st->mMenu == NULL) {
                if (!InitializePanelMenu(st) || (st->mMenu == NULL)) {
                    *prepared = FALSE;
                    return NOERROR;
                }
            }

            if (mActionBar != NULL) {
                if (mActionMenuPresenterCallback == NULL) {
                    mActionMenuPresenterCallback = new ActionMenuPresenterCallback(this);
                }

                mActionBar->SetMenu(st->mMenu, mActionMenuPresenterCallback);
            }

            // Call callback, and return if it doesn't want to display menu.

            // Creating the panel menu will involve a lot of manipulation;
            // don't dispatch change events to presenters until we're done.
            st->mMenu->StopDispatchingItemsChanged();
            Boolean result = FALSE;
            if ((cb == NULL) || !(cb->OnCreatePanelMenu(st->mFeatureId, st->mMenu, &result), result)) {
                // Ditch the menu created above
                st->SetMenu(NULL);

                if (mActionBar != NULL) {
                    // Don't show it in the action bar either
                    mActionBar->SetMenu(NULL, mActionMenuPresenterCallback);
                }

                *prepared = FALSE;
                return NOERROR;
            }

            st->mRefreshMenuContent = FALSE;
        }

        // Callback and return if the callback does not want to show the menu

        // Preparing the panel menu can involve a lot of manipulation;
        // don't dispatch change events to presenters until we're done.
        st->mMenu->StopDispatchingItemsChanged();

        // Restore action view state before we prepare. This gives apps
        // an opportunity to override frozen/restored state in onPrepare.
        if (st->mFrozenActionViewState != NULL) {
            st->mMenu->RestoreActionViewStates(st->mFrozenActionViewState);
            st->mFrozenActionViewState = NULL;
        }

        Boolean tmp = FALSE;
        if (!(cb->OnPreparePanel(st->mFeatureId, st->mCreatedPanelView, st->mMenu, &tmp), tmp)) {
            if (mActionBar != NULL) {
                // The app didn't want to show the menu for now but it still exists.
                // Clear it out of the action bar.
                mActionBar->SetMenu(NULL, mActionMenuPresenterCallback);
            }

            st->mMenu->StartDispatchingItemsChanged();

            *prepared = FALSE;
            return NOERROR;
        }

        // Set the proper keymap
        Int32 devId = 0;
        AutoPtr<IKeyCharacterMapHelper> kcHelper;
        CKeyCharacterMapHelper::AcquireSingleton((IKeyCharacterMapHelper**)&kcHelper);
        AutoPtr<IKeyCharacterMap> kmap;
        kcHelper->Load(event != NULL ? (event->GetDeviceId(&devId), devId) : IKeyCharacterMap::VIRTUAL_KEYBOARD,
                (IKeyCharacterMap**)&kmap);

        Int32 boardType = 0;
        st->mQwertyMode = (kmap->GetKeyboardType(&boardType), boardType) != IKeyCharacterMap::NUMERIC;
        st->mMenu->SetQwertyMode(st->mQwertyMode);
        st->mMenu->StartDispatchingItemsChanged();
    }

    // Set other state
    st->mIsPrepared = TRUE;
    st->mIsHandled = FALSE;
    mPreparedPanel = st;
    *prepared = TRUE;

    return NOERROR;
}

/**
 * Initializes the menu associated with the given panel feature state. You
 * must at the very least set PanelFeatureState.menu to the Menu to be
 * associated with the given panel state. The default implementation creates
 * a new menu for the panel state.
 *
 * @param st The panel whose menu is being initialized.
 * @return Whether the initialization was successful.
 */
Boolean CPhoneWindow::InitializePanelMenu(
    /* [in] */ PanelFeatureState* st)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    // If we have an action bar, initialize the menu with a context themed for it.
    if ((st->mFeatureId == FEATURE_OPTIONS_PANEL || st->mFeatureId == FEATURE_ACTION_BAR) &&
            mActionBar != NULL) {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);

        AutoPtr<IResourcesTheme> currentTheme;
        context->GetTheme((IResourcesTheme**)&currentTheme);

        Boolean tmp = FALSE;
        currentTheme->ResolveAttribute(R::attr::actionBarWidgetTheme,
                outValue, TRUE, &tmp);
        Int32 targetThemeRes = 0;
        outValue->GetResourceId(&targetThemeRes);

        Int32 tmpId = 0;
        if (targetThemeRes != 0 && (context->GetThemeResId(&tmpId), tmpId) != targetThemeRes) {
            AutoPtr<IContext> temp;
            CContextThemeWrapper::New(context, targetThemeRes, (IContextThemeWrapper**)&temp);
            context = temp;
        }
    }

    AutoPtr<IMenuBuilder> menu;
    CMenuBuilder::New(context, (IMenuBuilder**)&menu);
    menu->SetCallback(new MyMenuBuilderCallback(this));
    st->SetMenu(menu);

    return TRUE;
}

AutoPtr<IDrawable> CPhoneWindow::LoadImageURI(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<IInputStream> inputStream;
    contentResolver->OpenInputStream(uri, (IInputStream**)&inputStream);
    AutoPtr<IDrawable> drawable;
    Drawable::Drawable::CreateFromStream(inputStream, String(NULL), (IDrawable**)&drawable);
    return drawable;
}

ECode CPhoneWindow::GetDrawableState(
    /* [in] */ Int32 featureId,
    /* [in] */ Boolean required,
    /* [out] */ DrawableFeatureState** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;

    if ((GetFeatures() & (1 << featureId)) == 0) {
        if (!required) {
            return NOERROR;
        }

        Slogger::E(TAG, "GetDrawableState: The feature has not been requested: content: %p, featureId: %d, mFeatures: %08x, mInvalidatePanelMenuFeatures: %08x",
            &mFeatures, featureId, mFeatures, mInvalidatePanelMenuFeatures);
        // throw new RuntimeException("The feature has not been requested");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr< ArrayOf<DrawableFeatureState*> > ar = mDrawables;
    if (ar == NULL || ar->GetLength() <= featureId) {
        AutoPtr< ArrayOf<DrawableFeatureState*> > nar = ArrayOf<DrawableFeatureState*>::Alloc(featureId + 1);
        if (ar != NULL) {
            nar->Copy(ar);
        }
        ar = nar;
        mDrawables = nar;
    }

    AutoPtr<DrawableFeatureState> st = (*ar)[featureId];
    if (st == NULL) {
        st = new DrawableFeatureState(featureId);
        ar->Set(featureId, st);
    }
    *state = st;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

/**
 * Gets a panel's state based on its feature ID.
 *
 * @param featureId The feature ID of the panel.
 * @param required Whether the panel is required (if it is required and it
 *            isn't in our features, this throws an exception).
 * @return The panel state.
 */
ECode CPhoneWindow::GetPanelState(
    /* [in] */ Int32 featureId,
    /* [in] */ Boolean required,
    /* [out] */ PanelFeatureState** state)
{
    return GetPanelState(featureId, required, NULL, state);
}

/**
 * Gets a panel's state based on its feature ID.
 *
 * @param featureId The feature ID of the panel.
 * @param required Whether the panel is required (if it is required and it
 *            isn't in our features, this throws an exception).
 * @param convertPanelState Optional: If the panel state does not exist, use
 *            this as the panel state.
 * @return The panel state.
 */
ECode CPhoneWindow::GetPanelState(
    /* [in] */ Int32 featureId,
    /* [in] */ Boolean required,
    /* [in] */ PanelFeatureState* convertPanelState,
    /* [out] */ PanelFeatureState** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;

    if ((GetFeatures() & (1 << featureId)) == 0) {
        if (!required) {
            return NOERROR;
        }
        Slogger::E(TAG, "GetPanelState: The feature has not been requested: content: %p, featureId: %d, mFeatures: %08x, mInvalidatePanelMenuFeatures: %08x",
            &mFeatures, featureId, mFeatures, mInvalidatePanelMenuFeatures);
        //throw new RuntimeException("The feature has not been requested");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr< ArrayOf<PanelFeatureState*> > ar = mPanels;
    if (ar == NULL || ar->GetLength() <= featureId) {
        AutoPtr< ArrayOf<PanelFeatureState*> > nar = ArrayOf<PanelFeatureState*>::Alloc(featureId + 1);
        if (ar != NULL) {
            nar->Copy(ar);
        }
        ar = nar;
        mPanels = nar;
    }

    AutoPtr<PanelFeatureState> st = (*ar)[featureId];
    if (st == NULL) {
        st = (convertPanelState != NULL) ? convertPanelState : new PanelFeatureState(featureId);
        ar->Set(featureId, st);
    }

    *state = st;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

AutoPtr<CPhoneWindow::PanelFeatureState> CPhoneWindow::FindMenuPanel(
    /* [in] */ IMenu* menu)
{
    ArrayOf<PanelFeatureState*>* panels = mPanels;
    const Int32 N = panels != NULL ? panels->GetLength() : 0;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<PanelFeatureState> panel = (*panels)[i];
        if (panel != NULL && (panel->mMenu).Get() == menu) {
            return panel;
        }
    }
    return NULL;
}

ECode CPhoneWindow::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    ReopenMenu(TRUE);
    return NOERROR;
}

ECode CPhoneWindow::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean destoryed = FALSE;
    if (cb != NULL && !(IsDestroyed(&destoryed), destoryed)) {
        AutoPtr<IMenuBuilder> rootMenu;
        menu->GetRootMenu((IMenuBuilder**)&rootMenu);

        AutoPtr<PanelFeatureState> panel = FindMenuPanel((IMenu*)rootMenu.Get());
        if (panel != NULL) {
            return cb->OnMenuItemSelected(panel->mFeatureId, item, state);
        }
    }
    *state = FALSE;
    return NOERROR;
}

void CPhoneWindow::ReopenMenu(
    /* [in] */ Boolean toggleMenuMode)
{
    Boolean tmp = FALSE;
    if (mActionBar != NULL && (mActionBar->IsOverflowReserved(&tmp), tmp)) {
        AutoPtr<IWindowCallback> cb;
        GetCallback((IWindowCallback**)&cb);

        Boolean destoryed = FALSE;
        IsDestroyed(&destoryed);
        if (!(mActionBar->IsOverflowMenuShowing(&tmp), tmp) || !toggleMenuMode) {
            Int32 visiable = 0;

            if (cb != NULL && !destoryed && (mActionBar->GetVisibility(&visiable)) == IView::VISIBLE) {
                AutoPtr<PanelFeatureState> st;
                GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);

                // If we don't have a menu or we're waiting for a full content refresh,
                // forget it. This is a lingering event that no longer matters.
                if (st != NULL && st->mMenu != NULL && !st->mRefreshMenuContent &&
                        (cb->OnPreparePanel(FEATURE_OPTIONS_PANEL, st->mCreatedPanelView, st->mMenu, &tmp), tmp)) {
                    Boolean tmp = FALSE;
                    cb->OnMenuOpened(FEATURE_ACTION_BAR, st->mMenu, &tmp);
                    mActionBar->ShowOverflowMenu(&tmp);
                }
            }
        }
        else {
            Boolean tmp = FALSE;
            mActionBar->HideOverflowMenu(&tmp);
            if (cb != NULL && !destoryed) {
                AutoPtr<PanelFeatureState> st;
                GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);
                if (st != NULL) {
                    cb->OnPanelClosed(FEATURE_ACTION_BAR, st->mMenu);
                }
            }
        }
        return;
    }

    AutoPtr<PanelFeatureState> st;
    ECode ec = GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);
    if (FAILED(ec)) return;

    // Save the future expanded mode state since closePanel will reset it
    Boolean newExpandedMode = toggleMenuMode ? !st->mIsInExpandedMode : st->mIsInExpandedMode;

    st->mRefreshDecorView = TRUE;
    ClosePanel(st, FALSE);

    // Set the expanded mode state
    st->mIsInExpandedMode = newExpandedMode;

    OpenPanel(st, NULL);
}

/**
 * Opens the panels that have had their state restored. This should be
 * called sometime after {@link #restorePanelState} when it is safe to add
 * to the window manager.
 */
void CPhoneWindow::OpenPanelsAfterRestore()
{
    AutoPtr<ArrayOf<PanelFeatureState*> > panels = mPanels;

    if (panels == NULL) {
        return;
    }

    AutoPtr<PanelFeatureState> st;
    for (Int32 i = panels->GetLength() - 1; i >= 0; i--) {
        st = (*panels)[i];
        // We restore the panel if it was last open; we skip it if it
        // now is open, to avoid a race condition if the user immediately
        // opens it when we are resuming.
        if ((st != NULL)) {
            st->ApplyFrozenState();
            if (!st->mIsOpen && st->mWasLastOpen) {
                st->mIsInExpandedMode = st->mWasLastExpanded;
                OpenPanel(st, NULL);
            }
        }
    }
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
