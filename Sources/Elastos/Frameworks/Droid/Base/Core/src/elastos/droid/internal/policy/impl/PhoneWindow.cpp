#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/policy/impl/PhoneWindow.h"
#include "elastos/droid/internal/policy/impl/PhoneWindowManager.h"
#include "elastos/droid/internal/widget/CBackgroundFallback.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowSavedState.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/transition/CTransitionInflaterHelper.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/transition/CSceneHelper.h"
#include "elastos/droid/transition/CScene.h"
#include "elastos/droid/view/CView.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/CKeyCharacterMapHelper.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CGestureDetector.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/internal/view/StandaloneActionMode.h"
#include "elastos/droid/internal/view/menu/CMenuDialogHelper.h"
#include "elastos/droid/internal/view/menu/CContextMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CListMenuPresenter.h"
#include "elastos/droid/internal/view/menu/CIconMenuPresenter.h"
#include "elastos/droid/internal/widget/CActionBarContextView.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/internal/policy/impl/CPhoneWindowRotationWatcher.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Internal::View::EIID_IRootViewSurfaceTaker;
using Elastos::Droid::Internal::View::StandaloneActionMode;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuDialogHelper;
using Elastos::Droid::Internal::View::Menu::CMenuDialogHelper;
using Elastos::Droid::Internal::View::Menu::CListMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IListMenuPresenter;
using Elastos::Droid::Internal::View::Menu::CIconMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IBaseMenuPresenter;
using Elastos::Droid::Internal::View::Menu::CContextMenuBuilder;
using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::Widget::CBackgroundFallback;
using Elastos::Droid::Internal::Widget::IDecorContentParent;
using Elastos::Droid::Internal::Widget::IAbsActionBarView;
using Elastos::Droid::Internal::Widget::EIID_IOnDismissedListener;
using Elastos::Droid::Internal::Widget::EIID_IOnSwipeProgressChangedListener;
using Elastos::Droid::Internal::Widget::CActionBarContextView;
using Elastos::Droid::Internal::Widget::IActionBarContainer;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelperHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelperHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Transition::CTransitionInflaterHelper;
using Elastos::Droid::Transition::CTransitionManager;
using Elastos::Droid::Transition::ITransitionInflaterHelper;
using Elastos::Droid::Transition::ITransitionInflater;
using Elastos::Droid::Transition::CTransitionSet;
using Elastos::Droid::Transition::ITransitionSet;
using Elastos::Droid::Transition::CSceneHelper;
using Elastos::Droid::Transition::ISceneHelper;
using Elastos::Droid::Transition::CScene;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IMenu;
using Elastos::Droid::View::EIID_IWindow;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::EIID_IFrameLayout;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::Toast;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::R;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

AutoPtr<IIWindowManager> PhoneWindow::WindowManagerHolder::sWindowManager;

String PhoneWindow::TAG("PhoneWindow");
const Boolean PhoneWindow::SWEEP_OPEN_MENU = FALSE;

const String PhoneWindow::FOCUSED_ID_TAG("android:focusedViewId");
const String PhoneWindow::VIEWS_TAG("android:views");
const String PhoneWindow::PANELS_TAG("android:Panels");
const String PhoneWindow::ACTION_BAR_TAG("android:ActionBar");

const Int32 PhoneWindow::DEFAULT_BACKGROUND_FADE_DURATION_MS;
const Int32 PhoneWindow::CUSTOM_TITLE_COMPATIBLE_FEATURES = DEFAULT_FEATURES |
                                        (1 << FEATURE_CUSTOM_TITLE) |
                                        (1 << FEATURE_CONTENT_TRANSITIONS) |
                                        (1 << FEATURE_ACTIVITY_TRANSITIONS) |
                                        (1 << FEATURE_ACTION_MODE_OVERLAY);

const Int32 PhoneWindow::FLAG_RESOURCE_SET_ICON;
const Int32 PhoneWindow::FLAG_RESOURCE_SET_LOGO;
const Int32 PhoneWindow::FLAG_RESOURCE_SET_ICON_FALLBACK;

static AutoPtr<ITransition> InitUseDefault_Transition()
{
    AutoPtr<ITransition> tmp;
    CTransitionSet::New((ITransition**)&tmp);
    return tmp;
}

AutoPtr<ITransition> PhoneWindow::USE_DEFAULT_TRANSITION = InitUseDefault_Transition();

static AutoPtr<IPhoneWindowRotationWatcher> InitStaticWatcher()
{
    AutoPtr<IPhoneWindowRotationWatcher> tmp;
    CPhoneWindowRotationWatcher::New((IPhoneWindowRotationWatcher**)&tmp);
    return tmp;
}

AutoPtr<IPhoneWindowRotationWatcher> PhoneWindow::sRotationWatcher = InitStaticWatcher();



//===============================================================================================
// PhoneWindow::SettingsObserver
//===============================================================================================
PhoneWindow::SettingsObserver::SettingsObserver(
    /* [in] */ PhoneWindow* host)
    : mHost(host)
{}

ECode PhoneWindow::SettingsObserver::constructor(
    /* [in] */ IHandler* handler)
{
    return ContentObserver::constructor(handler);
}

ECode PhoneWindow::SettingsObserver::Observe()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::ENABLE_STYLUS_GESTURES, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    CheckGestures();
    return NOERROR;
}

ECode PhoneWindow::SettingsObserver::Unobserve()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    return resolver->UnregisterContentObserver(this);
}

ECode PhoneWindow::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return CheckGestures();
}

ECode PhoneWindow::SettingsObserver::CheckGestures()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 ival;
    Settings::System::GetInt32(resolver, ISettingsSystem::ENABLE_STYLUS_GESTURES, 0, &ival);
    mHost->mEnableGestures = ival == 1;
    return NOERROR;
}

//===============================================================================================
// PhoneWindow::_DecorView::ShowActionModePopupRunnable
//===============================================================================================
PhoneWindow::_DecorView::ShowActionModePopupRunnable::ShowActionModePopupRunnable(
    /* [in] */ _DecorView* host)
    : mHost(host)
{
}

ECode PhoneWindow::_DecorView::ShowActionModePopupRunnable::Run()
{
    AutoPtr<IBinder> token;
    IView* amView = IView::Probe(mHost->mActionModeView);
    amView->GetApplicationWindowToken((IBinder**)&token);
    return mHost->mActionModePopup->ShowAtLocation(token,
            IGravity::TOP | IGravity::FILL_HORIZONTAL, 0, 0);
}

//===============================================================================================
// PhoneWindow::_DecorView::ActionModeCallbackWrapper
//===============================================================================================
CAR_INTERFACE_IMPL(PhoneWindow::_DecorView::ActionModeCallbackWrapper, Object, IActionModeCallback)

PhoneWindow::_DecorView::ActionModeCallbackWrapper::ActionModeCallbackWrapper(
    /* [in] */ IActionModeCallback* wrapped,
    /* [in] */ _DecorView* host)
    : mWrapped(wrapped)
    , mHost(host)
{
}

ECode PhoneWindow::_DecorView::ActionModeCallbackWrapper::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    return mWrapped->OnCreateActionMode(mode, menu, result);
}

ECode PhoneWindow::_DecorView::ActionModeCallbackWrapper::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    mHost->RequestFitSystemWindows();
    return mWrapped->OnPrepareActionMode(mode, menu, result);
}

ECode PhoneWindow::_DecorView::ActionModeCallbackWrapper::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mWrapped->OnActionItemClicked(mode, item, result);
}

ECode PhoneWindow::_DecorView::ActionModeCallbackWrapper::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    mWrapped->OnDestroyActionMode(mode);
    if (mHost->mActionModePopup != NULL) {
        Boolean res;
        mHost->RemoveCallbacks(mHost->mShowActionModePopup, &res);
        mHost->mActionModePopup->Dismiss();
    } else if (mHost->mActionModeView != NULL) {
        IView* amView = IView::Probe(mHost->mActionModeView);
        amView->SetVisibility(IView::GONE);
    }
    if (mHost->mActionModeView != NULL) {
        IViewGroup* amViewGroup = IViewGroup::Probe(mHost->mActionModeView);
        amViewGroup->RemoveAllViews();
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
    mHost->RequestFitSystemWindows();
    return NOERROR;
}

//===============================================================================================
// PhoneWindow::_DecorView::DecorViewWeakReferenceImpl
//===============================================================================================
CAR_INTERFACE_IMPL(PhoneWindow::_DecorView::DecorViewWeakReferenceImpl, Object, IWeakReference)

PhoneWindow::_DecorView::DecorViewWeakReferenceImpl::DecorViewWeakReferenceImpl(
   /* [in] */ IInterface* object,
   /* [in] */ ElRefBase::WeakRefType* ref)
   : mObject(object)
   , mRef(ref)
{}

PhoneWindow::_DecorView::DecorViewWeakReferenceImpl::~DecorViewWeakReferenceImpl()
{
   if (mRef) mRef->DecWeak(this);
}

ECode PhoneWindow::_DecorView::DecorViewWeakReferenceImpl::Resolve(
   /* [in] */ const InterfaceID& riid,
   /* [out] */ IInterface** objectReference)
{
   *objectReference = NULL;
   if (mObject && mRef && mRef->AttemptIncStrong(objectReference)) {
       *objectReference = mObject->Probe(riid);
       REFCOUNT_ADD(*objectReference);
       ((DecorView*)(IFrameLayout::Probe(mObject)))->_Release();
   }
   return NOERROR;
}


//===============================================================================================
// PhoneWindow::_DecorView::StylusGestureFilter::
//===============================================================================================
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_UP = 1;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_DOWN = 2;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_LEFT = 3;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_RIGHT = 4;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::PRESS_LONG = 5;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::TAP_DOUBLE = 6;
const Double PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_MIN_DISTANCE = 25.0;
const Double PhoneWindow::_DecorView::StylusGestureFilter::SWIPE_MIN_VELOCITY = 50.0;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_NO_ACTION = 1000;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_HOME = 1001;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_BACK = 1002;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_MENU = 1003;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_SEARCH = 1004;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_RECENT = 1005;
const Int32 PhoneWindow::_DecorView::StylusGestureFilter::KEY_APP = 1006;
const String PhoneWindow::_DecorView::StylusGestureFilter::TAG("StylusGestureFilter");

PhoneWindow::_DecorView::StylusGestureFilter::StylusGestureFilter(
    /* [in] */ PhoneWindow::_DecorView* host)
    : mHost(host)
{}

ECode PhoneWindow::_DecorView::StylusGestureFilter::constructor()
{
    GestureDetector::SimpleOnGestureListener::constructor();
    return CGestureDetector::New(this, (IGestureDetector**)&mDetector);
}

ECode PhoneWindow::_DecorView::StylusGestureFilter::OnTouchEvent(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    return mDetector->OnTouchEvent(e, res);
}

ECode PhoneWindow::_DecorView::StylusGestureFilter::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    Float x1, y1, x2, y2;
    e1->GetX(&x1);
    e1->GetY(&y1);
    e2->GetX(&x2);
    e2->GetY(&y2);

    using Elastos::Core::Math;
    Float xDistance = Math::Abs(x1 - x2);
    Float yDistance = Math::Abs(y1 - y2);

    velocityX = Math::Abs(velocityX);
    velocityY = Math::Abs(velocityY);
    Boolean result = FALSE;

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);

    if (velocityX > (SWIPE_MIN_VELOCITY * density)
            && xDistance > (SWIPE_MIN_DISTANCE * density)
            && xDistance > yDistance) {
        if (x1 > x2) { // right to left
            // Swipe Left
            mHost->DispatchStylusAction(SWIPE_LEFT);
        } else {
            // Swipe Right
            mHost->DispatchStylusAction(SWIPE_RIGHT);
        }
        result = true;
    } else if (velocityY > (SWIPE_MIN_VELOCITY * density)
            && yDistance > (SWIPE_MIN_DISTANCE * density)
            && yDistance > xDistance) {
        if (y1 > y2) { // bottom to up
            // Swipe Up
            mHost->DispatchStylusAction(SWIPE_UP);
        } else {
            // Swipe Down
            mHost->DispatchStylusAction(SWIPE_DOWN);
        }
        result = TRUE;
    }
    *res = result;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::StylusGestureFilter::OnDoubleTap(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return mHost->DispatchStylusAction(TAP_DOUBLE);
}

ECode PhoneWindow::_DecorView::StylusGestureFilter::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    return mHost->DispatchStylusAction(PRESS_LONG);
}

//===============================================================================================
// PhoneWindow::_DecorView
//===============================================================================================
CAR_INTERFACE_IMPL(PhoneWindow::_DecorView, FrameLayout, IRootViewSurfaceTaker)

PhoneWindow::_DecorView::_DecorView(
    /* [in] */ PhoneWindow* host)
    : mHost(host)
    , mFeatureId(0)
    , mChanging(FALSE)
    , mWatchingForMenu(FALSE)
    , mDownY(0)
    , mLastTopInset(0)
    , mLastBottomInset(0)
    , mLastRightInset(0)
    , mDefaultOpacity(IPixelFormat::OPAQUE)
{}

ECode PhoneWindow::_DecorView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 featureId)
{
    mFeatureId = featureId;

    FAIL_RETURN(FrameLayout::constructor(context))

    mSettingsObserver = new SettingsObserver(mHost);
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mSettingsObserver->constructor(handler);

    mStylusFilter = new StylusGestureFilter(this);
    mStylusFilter->constructor();

    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mDrawingBounds));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mBackgroundPadding));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mFramePadding));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mFrameOffsets));
    return CBackgroundFallback::New((IBackgroundFallback**)&mBackgroundFallback);
}

ECode PhoneWindow::_DecorView::WillYouTakeTheSurface(
    /* [out] */ ISurfaceHolderCallback2** cback)
{
    VALIDATE_NOT_NULL(cback);
    *cback = NULL;

    if (mFeatureId < 0) {
        *cback = mHost->mTakeSurfaceCallback;
        REFCOUNT_ADD(*cback);
    }
    return NOERROR;
}

ECode PhoneWindow::_DecorView::SetSurfaceType(
    /* [in] */ Int32 type)
{
    return mHost->SetType(type);
}

ECode PhoneWindow::_DecorView::SetSurfaceFormat(
    /* [in] */ Int32 format)
{
    return mHost->SetFormat(format);
}

ECode PhoneWindow::_DecorView::SetSurfaceKeepScreenOn(
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

ECode PhoneWindow::_DecorView::WillYouTakeTheInputQueue(
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

ECode PhoneWindow::_DecorView::StartChanging()
{
    mChanging = TRUE;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::FinishChanging()
{
    mChanging = FALSE;
    DrawableChanged();
    return NOERROR;
}

ECode PhoneWindow::_DecorView::SetWindowBackground(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
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
    return NOERROR;
}

ECode PhoneWindow::_DecorView::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    FrameLayout::SetBackgroundDrawable(d);
    AutoPtr<IBinder> token;
    GetWindowToken((IBinder**)&token);
    if (token != NULL) {
        UpdateWindowResizeState();
    }

    return NOERROR;
}

ECode PhoneWindow::_DecorView::SetBackgroundFallback(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    if (resId != 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetDrawable(resId, (IDrawable**)&drawable);
    }
    mBackgroundFallback->SetDrawable(drawable);

    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
    Boolean hasFallback;
    mBackgroundFallback->HasFallback(&hasFallback);
    SetWillNotDraw(bg == NULL && !hasFallback);
    return NOERROR;
}

void PhoneWindow::_DecorView::OnDraw(
    /* [in] */ ICanvas* c)
{
    FrameLayout::OnDraw(c);
    mBackgroundFallback->Draw(mHost->mContentRoot, c, IView::Probe(mHost->mContentParent));
}

ECode PhoneWindow::_DecorView::SetWindowFrame(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
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
    return NOERROR;
}

ECode PhoneWindow::_DecorView::OnWindowSystemUiVisibilityChanged(
    /* [in] */ Int32 visible)
{
    UpdateColorViews(NULL/* insets */);
    return NOERROR;
}

// @Override IView
ECode PhoneWindow::_DecorView::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* _insets,
    /* [out] */ IWindowInsets** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWindowInsets> insets = _insets;
    AutoPtr<IRect> swInsets;
    insets->GetSystemWindowInsets((IRect**)&swInsets);
    mFrameOffsets->Set(swInsets);
    AutoPtr<IWindowInsets> temp = UpdateColorViews(insets);
    insets = UpdateStatusGuard(temp);
    UpdateNavigationGuard(insets);
    AutoPtr<IDrawable> drawable;
    GetForeground((IDrawable**)&drawable);
    if (drawable != NULL) {
        DrawableChanged();
    }
    *result = insets;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override ViewGroup
ECode PhoneWindow::_DecorView::IsTransitionGroup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 keyCode, action;
    event->GetKeyCode(&keyCode);
    event->GetAction(&action);
    Boolean isDown = action == IKeyEvent::ACTION_DOWN;

    Int32 repeat = 0;

    if (isDown && (event->GetRepeatCount(&repeat), repeat) == 0) {
        // First handle chording of panel key: if a panel key is held
        // but not released, try to execute a shortcut in it.
        if ((mHost->mPanelChordingKey > 0) && (mHost->mPanelChordingKey != keyCode)) {
            Boolean handled;
            DispatchKeyShortcutEvent(event, &handled);
            if (handled) {
                *result = TRUE;
                return NOERROR;
            }
        }

        // If a panel is open, perform a shortcut on it without the
        // chorded panel key
        if ((mHost->mPreparedPanel != NULL) && mHost->mPreparedPanel->mIsOpen) {
            if (mHost->PerformPanelShortcut(mHost->mPreparedPanel, keyCode, event, 0)) {
                *result = TRUE;
                return NOERROR;
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
            FrameLayout::DispatchKeyEvent(event, &handled);
        }

        if (handled) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = isDown ? mHost->OnKeyDown(mFeatureId, keyCode, event)
            : mHost->OnKeyUp(mFeatureId, keyCode, event);
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* ev,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
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
            *result = TRUE;
            return NOERROR;
        }
    }

    // Shortcut not handled by the panel.  Dispatch to the view hierarchy.
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchKeyShortcutEvent(ev, &handled);
    }
    else {
        FrameLayout::DispatchKeyShortcutEvent(ev, &handled);
    }

    if (handled) {
        *result = TRUE;
        return NOERROR;
    }

    // If the panel is not prepared, then we may be trying to handle a shortcut key
    // combination such as Control+C.  Temporarily prepare the panel then mark it
    // unprepared again when finished to ensure that the panel will again be prepared
    // the next time it is shown for real.
    if (mHost->mPreparedPanel == NULL) {
        AutoPtr<PanelFeatureState> st;
        ECode ec = mHost->GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);
        if (FAILED(ec))
        {
            *result = FALSE;
            return NOERROR;
        }

        Boolean tmp = FALSE;
        mHost->PreparePanel(st, ev, &tmp);

        Int32 keyCode = 0;
        ev->GetKeyCode(&keyCode);
        handled = mHost->PerformPanelShortcut(st, keyCode, ev,
                IMenu::FLAG_PERFORM_NO_CLOSE);
        st->mIsPrepared = FALSE;
        if (handled) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::MenuAction()
{
    AutoPtr<IKeyEvent> down, up;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_MENU, (IKeyEvent**)&down);
    CKeyEvent::New(IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_MENU, (IKeyEvent**)&up);
    Boolean bval;
    DispatchKeyEvent(down, &bval);
    DispatchKeyEvent(up, &bval);
    return NOERROR;
}

ECode PhoneWindow::_DecorView::BackAction()
{
    AutoPtr<IKeyEvent> down, up;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_BACK, (IKeyEvent**)&down);
    CKeyEvent::New(IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_BACK, (IKeyEvent**)&up);
    Boolean bval;
    DispatchKeyEvent(down, &bval);
    DispatchKeyEvent(up, &bval);
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchStylusAction(
    /* [in] */ Int32 gestureAction)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String pkgName;
    mContext->GetPackageName(&pkgName);
    Boolean isSystemUI = pkgName.Equals("Elastos.Droid.SystemUI");
    assert(0 && "TODO");

    String setting;
    Int32 dispatchAction = -1;
    switch (gestureAction) {
        case StylusGestureFilter::SWIPE_LEFT:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_LEFT_SWIPE, &setting);
            break;
        case StylusGestureFilter::SWIPE_RIGHT:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_RIGHT_SWIPE, &setting);
            break;
        case StylusGestureFilter::SWIPE_UP:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_UP_SWIPE, &setting);
            break;
        case StylusGestureFilter::SWIPE_DOWN:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_DOWN_SWIPE, &setting);
            break;
        case StylusGestureFilter::TAP_DOUBLE:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_DOUBLE_TAP, &setting);
            break;
        case StylusGestureFilter::PRESS_LONG:
            Settings::System::GetString(resolver,
                ISettingsSystem::GESTURES_LONG_PRESS, &setting);
            break;
        default:
            return NOERROR;
    }

    Int32 value;
    ECode ec = StringUtils::Parse(setting, &value);
    if (FAILED(ec)) {
        dispatchAction = StylusGestureFilter::KEY_APP;
    }
    else {
        if (value == StylusGestureFilter::KEY_NO_ACTION) {
            return NOERROR;
        }
        dispatchAction = value;
    }

    // Dispatching action
    switch (dispatchAction) {
        case StylusGestureFilter::KEY_HOME: {
            AutoPtr<IIntent> homeIntent;
            CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&homeIntent);
            homeIntent->AddCategory(IIntent::CATEGORY_HOME);
            homeIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(homeIntent);
            break;
        }

        case StylusGestureFilter::KEY_BACK:
            BackAction();
            break;

        case StylusGestureFilter::KEY_MENU:
            // Menu action on notificationbar / systemui will be converted
            // to back action
            if (isSystemUI) {
                BackAction();
                break;
            }
            MenuAction();
            break;

        case StylusGestureFilter::KEY_SEARCH:
            // Search action on notificationbar / systemui will be converted
            // to back action
            if (isSystemUI) {
                BackAction();
                break;
            }

            mHost->LaunchDefaultSearch();
            break;

        case StylusGestureFilter::KEY_RECENT: {
            AutoPtr<IIStatusBarService> mStatusBarService;
            AutoPtr<IInterface> service = ServiceManager::GetService(String("statusbar"));
            IIStatusBarService* statusBarService = IIStatusBarService::Probe(service);
            statusBarService->ToggleRecentApps();

            break;
        }

        case StylusGestureFilter::KEY_APP: {
            // Launching app on notificationbar / systemui will be preceded
            // with a back Action
            if (isSystemUI) {
                BackAction();
            }

            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IIntent> launchIntent;
            pm->GetLaunchIntentForPackage(setting, (IIntent**)&launchIntent);
            if (launchIntent != NULL) {
                ec = mContext->StartActivity(launchIntent);
                if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
                    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
                    params->Set(0, CoreUtils::Convert(setting));
                    String str;
                    mContext->GetString(R::string::stylus_app_not_installed, params, &str);
                    AutoPtr<IToast> toast;
                    Toast::MakeText(mContext, CoreUtils::Convert(str), IToast::LENGTH_LONG, (IToast**)&toast);
                    toast->Show();
                }
            }
            break;
        }
    }
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);

    // Stylus events with side button pressed are filtered and other
    // events are processed normally.
    Int32 bs;
    event->GetButtonState(&bs);
    if (mHost->mEnableGestures && IMotionEvent::BUTTON_SECONDARY == bs) {
        Boolean bval;
        mStylusFilter->OnTouchEvent(event, &bval);
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    Boolean handled = FALSE;
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchTouchEvent(event, &handled);
    }
    else {
        FrameLayout::DispatchTouchEvent(event, &handled);
    }

    *result = handled;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    Boolean handled = FALSE;
    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        cb->DispatchTrackballEvent(event, &handled);
    }
    else {
        FrameLayout::DispatchTrackballEvent(event, &handled);
    }

    *result = handled;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);

    if (cb != NULL && !IsDestroyed() && mFeatureId < 0) {
        Boolean ret = FALSE;
        cb->DispatchGenericMotionEvent(ev, &ret);
        *result = ret;
        return NOERROR;
    }

    return FrameLayout::DispatchGenericMotionEvent(ev, result);
}

ECode PhoneWindow::_DecorView::SuperDispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Give priority to closing action modes if applicable.
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        Int32 action;
        event->GetAction(&action);
        // Back cancels action modes first.
        if (mActionMode != NULL) {
            if (action == IKeyEvent::ACTION_UP) {
                mActionMode->Finish();
            }
            return TRUE;
        }
    }
    return FrameLayout::DispatchKeyEvent(event, result);
}

ECode PhoneWindow::_DecorView::SuperDispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return FrameLayout::DispatchTouchEvent(event, result);
}

ECode PhoneWindow::_DecorView::SuperDispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return FrameLayout::DispatchTrackballEvent(event, result);
}

ECode PhoneWindow::_DecorView::SuperDispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return FrameLayout::DispatchGenericMotionEvent(event, result);
}

ECode PhoneWindow::_DecorView::SuperDispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return FrameLayout::DispatchKeyShortcutEvent(event, result);
}

ECode PhoneWindow::_DecorView::DispatchApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** result)
{
    VALIDATE_NOT_NULL(result);
    if (mHost->mOutsetBottom != NULL) {
        AutoPtr<IDisplayMetrics> metrics;
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        Float tmp;
        mHost->mOutsetBottom->GetDimension(metrics, &tmp);
        Int32 bottom = (Int32)tmp;
        AutoPtr<IWindowInsets> newInsets;
        Int32 left, top, right;
        insets->GetSystemWindowInsetLeft(&left);
        insets->GetSystemWindowInsetTop(&top);
        insets->GetSystemWindowInsetRight(&right);
        insets->ReplaceSystemWindowInsets(left, top, right, bottom, (IWindowInsets**)&newInsets);
        return FrameLayout::DispatchApplyWindowInsets(newInsets, result);
    }
    else {
        return FrameLayout::DispatchApplyWindowInsets(insets, result);
    }
    return NOERROR;
}

ECode PhoneWindow::_DecorView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return OnInterceptTouchEvent(event, result);
}

Boolean PhoneWindow::_DecorView::IsOutOfBounds(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 width;
    Int32 height;
    FrameLayout::GetWidth(&width);
    FrameLayout::GetHeight(&height);
    return x < -5 || y < -5 || x > (width + 5)
        || y > (height + 5);
}

AutoPtr<IWindowInsets> PhoneWindow::_DecorView::UpdateColorViews(
    /* [in] */ IWindowInsets* _insets)
{
    using Elastos::Core::Math;
    AutoPtr<IWindowInsets> insets = _insets;
    AutoPtr<IWindowManagerLayoutParams> attrs;
    mHost->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    Int32 sysUiVisibility;
    attrs->GetSystemUiVisibility(&sysUiVisibility);
    Int32 winSysUiVisibility;
    GetWindowSystemUiVisibility(&winSysUiVisibility);
    sysUiVisibility |=  winSysUiVisibility;

    Boolean isHighEndGfx;
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->IsHighEndGfx(&isHighEndGfx);
    if (!mHost->mIsFloating && isHighEndGfx) {
        if (insets != NULL) {
            Int32 i1, i2;
            insets->GetStableInsetTop(&i1);
            insets->GetSystemWindowInsetTop(&i2);
            mLastTopInset = Math::Min(i1, i2);
            insets->GetStableInsetBottom(&i1);
            insets->GetSystemWindowInsetBottom(&i2);
            mLastBottomInset = Math::Min(i1, i2);
            insets->GetStableInsetRight(&i1);
            insets->GetSystemWindowInsetRight(&i2);
            mLastRightInset = Math::Min(i1, i2);
        }
        Int32 flags;
        mHost->GetAttributes((IWindowManagerLayoutParams**)&attrs);
        attrs->GetFlags(&flags);
        mStatusColorView = UpdateColorViewInt(mStatusColorView, sysUiVisibility,
                SYSTEM_UI_FLAG_FULLSCREEN, IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS,
                mHost->mStatusBarColor, mLastTopInset, IGravity::TOP,
                STATUS_BAR_BACKGROUND_TRANSITION_NAME,
                R::id::statusBarBackground,
                (flags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0);
        mNavigationColorView = UpdateColorViewInt(mNavigationColorView, sysUiVisibility,
                SYSTEM_UI_FLAG_HIDE_NAVIGATION, IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION,
                mHost->mNavigationBarColor, mLastBottomInset, IGravity::BOTTOM,
                NAVIGATION_BAR_BACKGROUND_TRANSITION_NAME,
                R::id::navigationBarBackground,
                false /* hiddenByWindowFlag */);
    }

    // When we expand the window with FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS, we still need
    // to ensure that the rest of the view hierarchy doesn't notice it, unless they've
    // explicitly asked for it.

    Int32 flags;
    attrs->GetFlags(&flags);
    Boolean consumingNavBar =
        (flags & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) != 0
        && (sysUiVisibility & SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) == 0
        && (sysUiVisibility & SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0;

    Int32 consumedRight = consumingNavBar ? mLastRightInset : 0;
    Int32 consumedBottom = consumingNavBar ? mLastBottomInset : 0;

    IView* contentRootView = IView::Probe(mHost->mContentRoot);
    AutoPtr<IViewGroupMarginLayoutParams> lp;
    AutoPtr<IViewGroupLayoutParams> vglParams;
    if (contentRootView != NULL) {
        contentRootView->GetLayoutParams((IViewGroupLayoutParams**)&vglParams);
        lp = IViewGroupMarginLayoutParams::Probe(vglParams);
    }

    if (lp != NULL) {
        Int32 right;
        lp->GetRightMargin(&right);
        Int32 bottom;
        lp->GetBottomMargin(&bottom);
        if (right != consumedRight || bottom != consumedBottom) {
            lp->SetRightMargin(consumedRight);
            lp->SetBottomMargin(consumedRight);
            contentRootView->SetLayoutParams(vglParams);

            if (insets == NULL) {
                // The insets have changed, but we're not currently in the process
                // of dispatching them.
                RequestApplyInsets();
            }
        }
        if (insets != NULL) {
            AutoPtr<IWindowInsets> tmp;
            Int32 l,t,r,b;
            insets->GetSystemWindowInsetLeft(&l);
            insets->GetSystemWindowInsetTop(&t);
            insets->GetSystemWindowInsetRight(&r);
            insets->GetSystemWindowInsetBottom(&b);
            insets->ReplaceSystemWindowInsets(l, t, r - consumedRight, b - consumedBottom, (IWindowInsets**)&tmp);
            insets = tmp;
        }
    }

    if (insets != NULL) {
        AutoPtr<IWindowInsets> tmp;
        insets->ConsumeStableInsets((IWindowInsets**)&tmp);
        insets = tmp;
    }
    return insets;
}

AutoPtr<IView> PhoneWindow::_DecorView::UpdateColorViewInt(
    /* [in] */ IView* view,
    /* [in] */ Int32 sysUiVis,
    /* [in] */ Int32 systemUiHideFlag,
    /* [in] */ Int32 translucentFlag,
    /* [in] */ Int32 color,
    /* [in] */ Int32 height,
    /* [in] */ Int32 verticalGravity,
    /* [in] */ const String& transitionName,
    /* [in] */ Int32 id,
    /* [in] */ Boolean hiddenByWindowFlag)
{
    AutoPtr<IWindowManagerLayoutParams> attrs;
    mHost->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    Int32 flags;
    attrs->GetFlags(&flags);
    Boolean show = height > 0 && (sysUiVis & systemUiHideFlag) == 0
        && !hiddenByWindowFlag
        && (flags & translucentFlag) == 0
        && (color & IColor::BLACK) != 0
        && (flags & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) != 0;

    if (view == NULL) {
        if (show) {
            AutoPtr<IView> tmpView;
            CView::New(mContext, (IView**)&tmpView);
            view = tmpView;
            view->SetBackgroundColor(color);
            view->SetTransitionName(transitionName);
            view->SetId(id);
            AutoPtr<IViewGroupLayoutParams> vglParams;
            CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, height,
                    IGravity::START | verticalGravity, (IViewGroupLayoutParams**)&vglParams);
            AddView(view, vglParams);
        }
    }
    else {
        Int32 vis = show ? VISIBLE : INVISIBLE;
        view->SetVisibility(vis);
        if (show) {
            AutoPtr<IViewGroupLayoutParams> lp;
            view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 h;
            lp->GetHeight(&h);
            if (h != height) {
                lp->SetHeight(height);
                view->SetLayoutParams(lp);
            }
            view->SetBackgroundColor(color);
        }
    }
    return view;
}

AutoPtr<IWindowInsets> PhoneWindow::_DecorView::UpdateStatusGuard(
    /* [in] */ IWindowInsets* _insets)
{
    AutoPtr<IWindowInsets> insets = _insets;
    Boolean showStatusGuard = FALSE;
    // Show the status guard when the non-overlay contextual action bar is showing
    if (mActionModeView != NULL) {
        AutoPtr<IViewGroupLayoutParams> vglParams;
        IView::Probe(mActionModeView)->GetLayoutParams((IViewGroupLayoutParams**)&vglParams);
        IViewGroupMarginLayoutParams* mlp = IViewGroupMarginLayoutParams::Probe(vglParams);
        if (mlp != NULL) {
            // Insets are magic!
            Boolean mlpChanged = false;
            Boolean isShown;
            IView::Probe(mActionModeView)->IsShown(&isShown);
            if (isShown) {
                if (mHost->mTempRect == NULL) {
                    CRect::New((IRect**)&mHost->mTempRect);
                }
                AutoPtr<IRect> rect = mHost->mTempRect;

                // If the parent doesn't consume the insets, manually
                // apply the default system window insets.
                AutoPtr<IWindowInsets> tmp;
                IView::Probe(mHost->mContentParent)->ComputeSystemWindowInsets(insets, rect, (IWindowInsets**)&tmp);
                Int32 t;
                rect->GetTop(&t);
                Int32 newMargin = 0;
                if (t == 0) {
                    insets->GetSystemWindowInsetTop(&newMargin);
                }
                Int32 topMargin;
                mlp->GetTopMargin(&topMargin);
                if (topMargin != newMargin) {
                    mlpChanged = TRUE;
                    Int32 tm;
                    insets->GetSystemWindowInsetTop(&tm);
                    mlp->SetTopMargin(tm);

                    if (mStatusGuard == NULL) {
                        CView::New(mContext, (IView**)&mStatusGuard);
                        AutoPtr<IResources> resources;
                        mContext->GetResources((IResources**)&resources);
                        Int32 color;
                        resources->GetColor(R::color::input_method_navigation_guard, &color);
                        mStatusGuard->SetBackgroundColor(color);
                        AutoPtr<IViewGroupLayoutParams> vglParams;
                        CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, tm, IGravity::START | IGravity::TOP,
                                (IViewGroupLayoutParams**)&vglParams);
                        Int32 index;
                        IndexOfChild(mStatusColorView, &index);
                        AddView(mStatusGuard, index, vglParams);
                    }
                    else {
                        AutoPtr<IViewGroupLayoutParams> lp;
                        mStatusGuard->GetLayoutParams((IViewGroupLayoutParams**)&vglParams);
                        Int32 lpheight;
                        lp->GetHeight(&lpheight);
                        if (lpheight != tm/*mlp.topMargin*/) {
                            //lp.height = mlp.topMargin;
                            lp->SetHeight(tm);
                            mStatusGuard->SetLayoutParams(lp);
                        }
                    }
                }

                // The action mode's theme may differ from the app, so
                // always show the status guard above it if we have one.
                showStatusGuard = mStatusGuard != NULL;

                // We only need to consume the insets if the action
                // mode is overlaid on the app content (e.g. it's
                // sitting in a FrameLayout, see
                // screen_simple_overlay_action_mode.xml).
                Boolean nonOverlay = (mHost->GetLocalFeatures()
                        & (1 << FEATURE_ACTION_MODE_OVERLAY)) == 0;
                AutoPtr<IWindowInsets> witmp;
                insets->ConsumeSystemWindowInsets(false, nonOverlay && showStatusGuard /* top */, false, false, (IWindowInsets**)&witmp);
                insets = witmp;
            }
            else {
                // reset top margin
                Int32 mlpTop;
                mlp->GetTopMargin(&mlpTop);
                if (mlpTop != 0) {
                    mlpChanged = true;
                    mlp->SetTopMargin(0);
                }
            }
            if (mlpChanged) {
                IView::Probe(mActionModeView)->SetLayoutParams(IViewGroupLayoutParams::Probe(mlp));
            }
        }
    }
    if (mStatusGuard != NULL) {
        mStatusGuard->SetVisibility(showStatusGuard ? IView::VISIBLE : IView::GONE);
    }
    return insets;
}

void PhoneWindow::_DecorView::UpdateNavigationGuard(
    /* [in] */ IWindowInsets* insets)
{
    AutoPtr<IWindowManagerLayoutParams> attrs;
    mHost->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    Int32 type;
    attrs->GetType(&type);
    // IMEs lay out below the nav bar, but the content view must not (for back compat)
    if (type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD) {
        // prevent the content view from including the nav bar height
        if (mHost->mContentParent != NULL) {
            AutoPtr<IViewGroupLayoutParams> vglParams;
            IView::Probe(mHost->mContentParent)->GetLayoutParams((IViewGroupLayoutParams**)&vglParams);
            IViewGroupMarginLayoutParams* mlp = IViewGroupMarginLayoutParams::Probe(vglParams);
            Int32 bot;
            insets->GetSystemWindowInsetBottom(&bot);
            mlp->SetBottomMargin(bot);
            IView::Probe(mHost->mContentParent)->SetLayoutParams(IViewGroupLayoutParams::Probe(mlp));
        }
        // position the navigation guard view, creating it if necessary
        if (mNavigationGuard == NULL) {
            CView::New(mContext, (IView**)&mNavigationGuard);
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            Int32 color;
            resources->GetColor(R::color::input_method_navigation_guard, &color);
            mNavigationGuard->SetBackgroundColor(color);
            Int32 swiBot;
            insets->GetSystemWindowInsetBottom(&swiBot);
            AutoPtr<IViewGroupLayoutParams> vglParams;
            CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, swiBot,
                    IGravity::START | IGravity::BOTTOM,
                    (IViewGroupLayoutParams**)&vglParams);
            Int32 index;
            IndexOfChild(mNavigationColorView, &index);
            AddView(mNavigationGuard, index, vglParams);
        }
        else {
            AutoPtr<IViewGroupLayoutParams> lp;
            mNavigationGuard->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 swiBot;
            insets->GetSystemWindowInsetBottom(&swiBot);
            lp->SetHeight(swiBot);;
            mNavigationGuard->SetLayoutParams(lp);
        }
    }
}

AutoPtr<IWindowCallback> PhoneWindow::_DecorView::GetCallback()
{
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    return cb;
}

Boolean PhoneWindow::_DecorView::IsDestroyed()
{
    Boolean destoryed = FALSE;
    return (mHost->IsDestroyed(&destoryed), destoryed);
}

ECode PhoneWindow::_DecorView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
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
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    if (!PhoneWindow::SWEEP_OPEN_MENU) {
        *result = FALSE;
        return NOERROR;
    }

    Float fy;
    event->GetY(&fy);
    if (mFeatureId >= 0) {
        if (action == IMotionEvent::ACTION_DOWN) {
            // Logger::D(TAG, "Watchiing!");
            mWatchingForMenu = TRUE;
            mDownY = (Int32)fy;
            *result = FALSE;
            return NOERROR;
        }

        if (!mWatchingForMenu) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 y = (Int32)fy;
        if (action == IMotionEvent::ACTION_MOVE) {
            if (y > (mDownY + 30)) {
                // Logger::D(TAG, "Closing!");
                mHost->ClosePanel(mFeatureId);
                mWatchingForMenu = FALSE;
                *result = TRUE;
                return NOERROR;
            }
        }
        else if (action == IMotionEvent::ACTION_UP) {
            mWatchingForMenu = FALSE;
        }

        *result = FALSE;
        return NOERROR;
    }

    if (action == IMotionEvent::ACTION_DOWN) {
        Int32 y = fy;
        Boolean hasChildren;
        mHost->HasChildren(&hasChildren);
        Int32 height;
        FrameLayout::GetHeight(&height);
        if (y >= (height-5) && !hasChildren) {
            // Logger::D(TAG, "Watchiing!");
            mWatchingForMenu = TRUE;
        }
        *result = FALSE;
        return NOERROR;
    }

    if (!mWatchingForMenu) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 y = fy;
    if (action == IMotionEvent::ACTION_MOVE) {
        Int32 height;
        FrameLayout::GetHeight(&height);
        if (y < (height-30)) {
            // Logger::D(TAG, "Opening!");
            AutoPtr<IKeyEvent> event;
            CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_MENU, (IKeyEvent**)&event);
            mHost->OpenPanel(IWindow::FEATURE_OPTIONS_PANEL, event);

            mWatchingForMenu = FALSE;
            *result = TRUE;
            return NOERROR;
        }
    }
    else if (action == IMotionEvent::ACTION_UP) {
        mWatchingForMenu = FALSE;
    }

    *result = FALSE;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::SendAccessibilityEvent(
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
    Int32 childCount;
    GetChildCount(&childCount);
    if ((mFeatureId == FEATURE_OPTIONS_PANEL ||
            mFeatureId == FEATURE_CONTEXT_MENU ||
            mFeatureId == FEATURE_PROGRESS ||
            mFeatureId == FEATURE_INDETERMINATE_PROGRESS)
            && childCount == 1) {
        AutoPtr<IView> view;
        GetChildAt(0, (IView**)&view);
        view->SendAccessibilityEvent(eventType);
    }
    else {
        FrameLayout::SendAccessibilityEvent(eventType);
    }

    return NOERROR;
}

ECode PhoneWindow::_DecorView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && !IsDestroyed()) {
        Boolean tmp = FALSE;
        if (cb->DispatchPopulateAccessibilityEvent(event, &tmp), tmp) {
            *result = TRUE;
            return NOERROR;
        }
    }

    return FrameLayout::DispatchPopulateAccessibilityEvent(event, result);
}

Boolean PhoneWindow::_DecorView::SetFrame(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean changed = FrameLayout::SetFrame(l, t, r, b);
    if (changed) {
        AutoPtr<IRect> drawingBounds = mDrawingBounds;
        GetDrawingRect(drawingBounds);

        AutoPtr<IDrawable> fg;
        GetForeground((IDrawable**)&fg);
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

        AutoPtr<IDrawable> bg;
        GetBackground((IDrawable**)&bg);
        if (bg != NULL) {
            bg->SetBounds(drawingBounds);
        }

        if (SWEEP_OPEN_MENU) {
            AutoPtr<IWindowManagerLayoutParams> attr;
            mHost->GetAttributes((IWindowManagerLayoutParams**)&attr);
            Int32 height = 0;
            IViewGroupLayoutParams* vgLayoutParam = IViewGroupLayoutParams::Probe(attr);
            vgLayoutParam->GetHeight(&height);
            if (mMenuBackground == NULL && mFeatureId < 0
                && height == IViewGroupLayoutParams::MATCH_PARENT) {

                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                context->GetDrawable(R::drawable::menu_background, (IDrawable**)&mMenuBackground);
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

void PhoneWindow::_DecorView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<IResources> res;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetResources((IResources**)&res);

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
                Float fw;
                tvw->GetDimension(metrics, &fw);
                w = (Int32)fw;
            }
            else if (type == ITypedValue::TYPE_FRACTION) {
                Float fw;
                tvw->GetFraction(widthPixels, widthPixels, &fw);
                w = (Int32)fw;
            }
            else {
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
                Float fh;
                tvh->GetDimension(metrics, &fh);
                h = (Int32)fh;
            }
            else if (type == ITypedValue::TYPE_FRACTION) {
                Float fh;
                tvh->GetFraction(heightPixels, heightPixels, &fh);
                h = (Int32)fh;
            }
            else {
                h = 0;
            }

            if (h > 0) {
                Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);
                heightMeasureSpec = MeasureSpec::MakeMeasureSpec(
                        Elastos::Core::Math::Min(h, heightSize), MeasureSpec::EXACTLY);
            }
        }
    }

    if (mHost->mOutsetBottom != NULL) {
        int mode = MeasureSpec::GetMode(heightMeasureSpec);
        if (mode != MeasureSpec::UNSPECIFIED) {
            Float tmp;
            mHost->mOutsetBottom->GetDimension(metrics, &tmp);
            Int32 outset = (Int32) tmp;
            Int32 height = MeasureSpec::GetSize(heightMeasureSpec);
            heightMeasureSpec = MeasureSpec::MakeMeasureSpec(height + outset, mode);
        }
    }

    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 width;
    GetMeasuredWidth(&width);
    Boolean measure = FALSE;

    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY);

    if (!fixedWidth && widthMode == MeasureSpec::AT_MOST) {
        AutoPtr<ITypedValue> tv = isPortrait ? mHost->mMinWidthMinor : mHost->mMinWidthMajor;
        Int32 type = 0;
        if ((tv->GetType(&type), type) != ITypedValue::TYPE_NULL) {
            Int32 min = 0;
            if (type == ITypedValue::TYPE_DIMENSION) {
                Float fmin;
                tv->GetDimension(metrics, &fmin);
                min = (Int32)fmin;
            }
            else if (type == ITypedValue::TYPE_FRACTION) {
                Float fmin;
                tv->GetFraction(widthPixels, widthPixels, &fmin);
                min = (Int32)fmin;
            }
            else {
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

void PhoneWindow::_DecorView::DrawableChanged()
{
    if (mChanging) {
        return;
    }

    SetPadding(mFramePadding->mLeft + mBackgroundPadding->mLeft, mFramePadding->mTop
            + mBackgroundPadding->mTop, mFramePadding->mRight + mBackgroundPadding->mRight,
            mFramePadding->mBottom + mBackgroundPadding->mBottom);
    RequestLayout();
    Invalidate();

    Int32 opacity = IPixelFormat::OPAQUE;

    // Note: if there is no background, we will assume opaque. The
    // common case seems to be that an application sets there to be
    // no background so it can draw everything itself. For that,
    // we would like to assume OPAQUE and let the app force it to
    // the slower TRANSLUCENT mode if that is really what it wants.
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
    AutoPtr<IDrawable> fg;
    GetForeground((IDrawable**)&fg);
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
            if (fop == IPixelFormat::OPAQUE || bop == IPixelFormat::OPAQUE) {
                opacity = IPixelFormat::OPAQUE;
            } else if (fop == IPixelFormat::UNKNOWN) {
                opacity = bop;
            } else if (bop == IPixelFormat::UNKNOWN) {
                opacity = fop;
            } else {
                Drawable::Drawable::ResolveOpacity(fop, bop, &opacity);
            }
        } else {
            // For now we have to assume translucent if there is a
            // frame with padding... there is no way to tell if the
            // frame and background together will draw all pixels.
            // if (false)
            //     Log.v(TAG, "Padding: " + mFramePadding);
            opacity = IPixelFormat::TRANSLUCENT;
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

ECode PhoneWindow::_DecorView::OnWindowFocusChanged(
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

ECode PhoneWindow::_DecorView::UpdateWindowResizeState()
{
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
    Int32 opacity = 0;
    HackTurnOffWindowResizeAnim(bg == NULL || (bg->GetOpacity(&opacity), opacity)
            != IPixelFormat::OPAQUE);
    return NOERROR;
}

ECode PhoneWindow::_DecorView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();

    Logger::I(TAG, "TODO mSettingsObserver->Observe();");
    //assert(0 && "TODO");
    // mSettingsObserver->Observe();

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

ECode PhoneWindow::_DecorView::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();

    //assert(0 && "TODO");
    // mSettingsObserver->Unobserve();

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL && mFeatureId < 0) {
        cb->OnDetachedFromWindow();
    }

    if (mHost->mDecorContentParent != NULL) {
        mHost->mDecorContentParent->DismissPopups();
    }

    if (mActionModePopup != NULL) {
        Boolean res;
        RemoveCallbacks(mShowActionModePopup, &res);
        Boolean showing = FALSE;
        if (mActionModePopup->IsShowing(&showing), showing) {
            mActionModePopup->Dismiss();
        }

        mActionModePopup = NULL;
    }

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(mHost->GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st));
    if (st != NULL && st->mMenu != NULL && mFeatureId < 0) {
        Boolean res = FALSE;
        st->mMenu->Close(res);//TODO should be pointer
    }

    return NOERROR;
}

ECode PhoneWindow::_DecorView::OnCloseSystemDialogs(
    /* [in] */ const String& reason)
{
    if (mFeatureId >= 0) mHost->CloseAllPanels();
    return NOERROR;
}

ECode PhoneWindow::_DecorView::Draw(
    /* [in] */ ICanvas* canvas)
{
    FrameLayout::Draw(canvas);
    if (mMenuBackground != NULL)
    {
        mMenuBackground->Draw(canvas);
    }
    return NOERROR;
}

ECode PhoneWindow::_DecorView::ShowContextMenuForChild(
    /* [in] */ IView* originalView,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(originalView != NULL);

    // Reuse the context menu builder
    if (mHost->mContextMenu == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);

        CContextMenuBuilder::New(context, (IContextMenuBuilder**)&mHost->mContextMenu);
        IMenuBuilder* menuBuild = IMenuBuilder::Probe(mHost->mContextMenu);
        menuBuild->SetCallback(mHost->mContextMenuCallback);
    }
    else {
        IMenuBuilder* menuBuild = IMenuBuilder::Probe(mHost->mContextMenu);
        menuBuild->ClearAll();
    }

    AutoPtr<IBinder> token;
    originalView->GetWindowToken((IBinder**)&token);

    AutoPtr<IMenuDialogHelper> helper;
    mHost->mContextMenu->Show(originalView, token, (IMenuDialogHelper**)&helper);
    if (helper != NULL) {
       helper->SetPresenterCallback(mHost->mContextMenuCallback);
    } else if (mHost->mContextMenuHelper != NULL) {
        // No menu to show, but if we have a menu currently showing it just became blank.
        // Close it.
        mHost->mContextMenuHelper->Dismiss();
    }

    mHost->mContextMenuHelper = helper;
    *result = helper != NULL;
    return NOERROR;
}

ECode PhoneWindow::_DecorView::StartActionModeForChild(
    /* [in] */ IView* originalView,
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** res)
{
    VALIDATE_NOT_NULL(res);
    // originalView can be used here to be sure that we don't obscure
    // relevant content with the context mode UI.
    return StartActionMode(callback, res);
}

ECode PhoneWindow::_DecorView::StartActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** res)
{
    VALIDATE_NOT_NULL(res);
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
    }
    else {
        if (mActionModeView == NULL) {
            Boolean isFloating = FALSE;
            if (mHost->IsFloating(&isFloating), isFloating) {
                // Use the action bar theme.
                AutoPtr<ITypedValue> outValue;
                CTypedValue::New((ITypedValue**)&outValue);
                AutoPtr<IResourcesTheme> baseTheme;
                mContext->GetTheme((IResourcesTheme**)&baseTheme);
                Boolean tmp = FALSE;
                baseTheme->ResolveAttribute(R::attr::actionBarTheme, outValue, TRUE, &tmp);
                AutoPtr<IContext> actionBarContext;
                Int32 resId;
                outValue->GetResourceId(&resId);
                if (resId != 0) {
                    AutoPtr<IResourcesTheme> actionBarTheme;
                    AutoPtr<IResources> resources;
                    mContext->GetResources((IResources**)&resources);
                    resources->NewTheme((IResourcesTheme**)&actionBarTheme);

                    actionBarTheme->SetTo(baseTheme);
                    actionBarTheme->ApplyStyle(resId, TRUE);

                    CContextThemeWrapper::New(mContext, 0, (IContext**)&actionBarContext);
                    AutoPtr<IResourcesTheme> ctxTheme;
                    actionBarContext->GetTheme((IResourcesTheme**)&ctxTheme);
                    ctxTheme->SetTo(actionBarTheme);
                } else {
                    actionBarContext = mContext;
                }
                CActionBarContextView::New(actionBarContext, (IActionBarContextView**)&mActionModeView);
                mActionModePopup = NULL;
                CPopupWindow::New(actionBarContext, NULL,
                    R::attr::actionModePopupWindowStyle, (IPopupWindow**)&mActionModePopup);
                mActionModePopup->SetWindowLayoutType(
                        IWindowManagerLayoutParams::TYPE_APPLICATION);
                mActionModePopup->SetContentView(IView::Probe(mActionModeView));
                mActionModePopup->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);

                AutoPtr<IResourcesTheme> theme;
                actionBarContext->GetTheme((IResourcesTheme**)&theme);

                Boolean btmp = FALSE;
                theme->ResolveAttribute(R::attr::actionBarSize, outValue, TRUE, &btmp);

                Int32 data = 0;
                outValue->GetData(&data);

                AutoPtr<IResources> res;
                actionBarContext->GetResources((IResources**)&res);
                AutoPtr<IDisplayMetrics> metrics;
                res->GetDisplayMetrics((IDisplayMetrics**)&metrics);

                AutoPtr<ITypedValueHelper> helper;
                CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&helper);

                Int32 height = 0;
                helper->ComplexToDimensionPixelSize(data, metrics, &height);
                IAbsActionBarView* absActionBarView = IAbsActionBarView::Probe(mActionModeView);
                absActionBarView->SetContentHeight(height);
                mActionModePopup->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
                mShowActionModePopup = new ShowActionModePopupRunnable(this);
            }
            else {
                AutoPtr<IView> view;
                FindViewById(R::id::action_mode_bar_stub, (IView**)&view);
                AutoPtr<IViewStub> stub = IViewStub::Probe(view);
                if (stub != NULL) {
                    view = NULL;
                    stub->Inflate((IView**)&view);
                    mActionModeView = IActionBarContextView::Probe(view);
                }
            }
        }

        if (mActionModeView != NULL) {
            mActionModeView->KillMode();
            mode = new StandaloneActionMode();
            AutoPtr<IContext> context;
            IView::Probe(mActionModeView)->GetContext((IContext**)&context);
            ((StandaloneActionMode*)(mode.Get()))->constructor(context, mActionModeView,
                    wrappedCallback, mActionModePopup == NULL);
            Boolean tmp = FALSE;
            AutoPtr<IMenu> menu;
            mode->GetMenu((IMenu**)&menu);
            if (callback->OnCreateActionMode(mode, menu, &tmp), tmp) {
                mode->Invalidate();
                mActionModeView->InitForMode(mode);
                IView* amView = IView::Probe(mActionModeView);
                amView->SetVisibility(IView::VISIBLE);
                mActionMode = mode;
                if (mActionModePopup != NULL) {
                    Boolean res;
                    Post(mShowActionModePopup, &res);
                }

                ((View*)amView)->SendAccessibilityEvent(
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

    *res = mActionMode;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

//===============================================================================================
// PhoneWindow::DecorView
//===============================================================================================
PhoneWindow::DecorView::DecorView(
   /* [in] */ PhoneWindow* host)
   : _DecorView(host)
   , mUseSelfRef(FALSE)
{
}

PhoneWindow::DecorView::~DecorView()
{
   if (!mUseSelfRef) {
       mHost->mDecor = NULL;
   }
}

ECode PhoneWindow::DecorView::constructor(
   /* [in] */ IContext* context,
   /* [in] */ Int32 featureId,
   /* [in] */ Boolean useSelfRef)
{
    FAIL_RETURN(_DecorView::constructor(context, featureId));
    mUseSelfRef = useSelfRef;
    return NOERROR;
}

PInterface PhoneWindow::DecorView::Probe(
   /* [in] */ REIID riid)
{
   return _DecorView::Probe(riid);
}

ECode PhoneWindow::DecorView::GetInterfaceID(
   /* [in] */ IInterface *pObject,
   /* [out] */ InterfaceID *pIID)
{
   return _DecorView::GetInterfaceID(pObject, pIID);
}

UInt32 PhoneWindow::DecorView::AddRef()
{
   if (mUseSelfRef)
       return ElRefBase::AddRef();
   else {
       assert(mHost != NULL);
       return mHost->AddRef();
   }
}

UInt32 PhoneWindow::DecorView::Release()
{
   if (mUseSelfRef)
       return ElRefBase::Release();
   else {
       assert(mHost != NULL);
       return mHost->Release();
   }
}

UInt32 PhoneWindow::DecorView::_AddRef()
{
   return ElRefBase::AddRef();
}

UInt32 PhoneWindow::DecorView::_Release()
{
   return ElRefBase::Release();
}

ECode PhoneWindow::DecorView::GetWeakReference(
   /* [out] */ IWeakReference** weakReference)
{
   VALIDATE_NOT_NULL(weakReference)
   *weakReference = new DecorViewWeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
   REFCOUNT_ADD(*weakReference)
   return NOERROR;
}

//=====================================================================
//              PhoneWindow::PanelFeatureState::SavedState
//=====================================================================
CAR_INTERFACE_IMPL_2(PhoneWindow::PanelFeatureState::SavedState, Object, IPhoneWindowSavedState, IParcelable)

PhoneWindow::PanelFeatureState::SavedState::SavedState()
    : mFeatureId(0)
    , mIsOpen(FALSE)
    , mIsInExpandedMode(FALSE)
{
}

ECode PhoneWindow::PanelFeatureState::SavedState::constructor()
{
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt32(mFeatureId);
    dest->WriteBoolean(mIsOpen);
    dest->WriteBoolean(mIsInExpandedMode);

    if (mIsOpen) {
        dest->WriteInterfacePtr(mMenuState);
    }
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mFeatureId);
    source->ReadBoolean(&mIsOpen);
    source->ReadBoolean(&mIsInExpandedMode);

    if (mIsOpen) {
        source->ReadInterfacePtr((Handle32*)&mMenuState);
    }
    return NOERROR;
}


PhoneWindow::PanelFeatureState::PanelFeatureState(
    /* [in] */ PhoneWindow* owner,
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
    , mOwner(owner)
{}

ECode PhoneWindow::PanelFeatureState::IsInListMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsInExpandedMode || mIsCompact;
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::HasPanelItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mShownPanelView == NULL)
    {
        *result = FALSE;
        return NOERROR;
    }

    if (mCreatedPanelView != NULL)
    {
        *result = TRUE;
        return NOERROR;
    }

    if (mIsCompact || mIsInExpandedMode) {
        AutoPtr<IListAdapter> adapter;
        mListMenuPresenter->GetAdapter((IListAdapter**)&adapter);
        Int32 count = 0;
        IAdapter* adr = IAdapter::Probe(adapter);
        *result = (adr->GetCount(&count), count) > 0;
    } else {
        Int32 count = 0;
        *result = (IViewGroup::Probe(mShownPanelView)->GetChildCount(&count), count) > 0;
    }
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::ClearMenuPresenters()
{
    if (mMenu != NULL) {
        mMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mIconMenuPresenter));
        mMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mListMenuPresenter));
    }

    mIconMenuPresenter = NULL;
    mListMenuPresenter = NULL;
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::SetStyle(
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
    return NOERROR;
}

ECode PhoneWindow::PanelFeatureState::SetMenu(
    /* [in] */ IMenuBuilder* menu)
{
    if (menu == mMenu) return NOERROR;

    if (mMenu != NULL) {
        mMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mIconMenuPresenter));
        mMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mListMenuPresenter));
    }

    mMenu = menu;
    if (menu != NULL) {
        if (mIconMenuPresenter != NULL) menu->AddMenuPresenter(IMenuPresenter::Probe(mIconMenuPresenter));
        if (mListMenuPresenter != NULL) menu->AddMenuPresenter(IMenuPresenter::Probe(mListMenuPresenter));
    }
    return NOERROR;
}

AutoPtr<IMenuView> PhoneWindow::PanelFeatureState::GetListMenuView(
    /* [in] */ IContext* context,
    /* [in] */ IMenuPresenterCallback* cb)
{
    if (mMenu == NULL) return NULL;

    if (!mIsCompact) {
        GetIconMenuView(context, cb); // Need this initialized to know where our offset goes
    }

    if (mListMenuPresenter == NULL) {
        CListMenuPresenter::New(R::layout::list_menu_item_layout, mListPresenterTheme, (IListMenuPresenter**)&mListMenuPresenter);
        IMenuPresenter* menuPresenter = IMenuPresenter::Probe(mListMenuPresenter);
        menuPresenter->SetCallback(cb);
        mListMenuPresenter->SetId(R::id::list_menu_presenter);
        mMenu->AddMenuPresenter(IMenuPresenter::Probe(mListMenuPresenter));
    }

    if (mIconMenuPresenter != NULL) {
        Int32 itemNum;
        mIconMenuPresenter->GetNumActualItemsShown(&itemNum);
        mListMenuPresenter->SetItemIndexOffset(itemNum);
    }

    AutoPtr<IMenuView> result;
    IMenuPresenter* menuPresenter = IMenuPresenter::Probe(mListMenuPresenter);
    menuPresenter->GetMenuView(IViewGroup::Probe(mDecorView), (IMenuView**)&result);

    return result;
}

AutoPtr<IMenuView> PhoneWindow::PanelFeatureState::GetIconMenuView(
    /* [in] */ IContext* context,
    /* [in] */ IMenuPresenterCallback* cb)
{
    if (mMenu == NULL) return NULL;

    if (mIconMenuPresenter == NULL) {
        CIconMenuPresenter::New(context, (IIconMenuPresenter**)&mIconMenuPresenter);
        IMenuPresenter* menuPresenter = IMenuPresenter::Probe(mIconMenuPresenter);
        menuPresenter->SetCallback(cb);
        IBaseMenuPresenter* baseMenuPresenter = IBaseMenuPresenter::Probe(mIconMenuPresenter);
        baseMenuPresenter->SetId(R::id::icon_menu_presenter);
        mMenu->AddMenuPresenter(IMenuPresenter::Probe(mIconMenuPresenter));
    }

    AutoPtr<IMenuView> result;
    IMenuPresenter* menuPresenter = IMenuPresenter::Probe(mIconMenuPresenter);
    menuPresenter->GetMenuView(IViewGroup::Probe(mDecorView), (IMenuView**)&result);

    return result;
}

AutoPtr<IParcelable> PhoneWindow::PanelFeatureState::OnSaveInstanceState()
{
    AutoPtr<CPhoneWindowSavedState> isavedState;
    CPhoneWindowSavedState::NewByFriend((CPhoneWindowSavedState**)&isavedState);
    SavedState* savedState = (SavedState*)(IPhoneWindowSavedState*)(isavedState.Get());
    savedState->mFeatureId = mFeatureId;
    savedState->mIsOpen = mIsOpen;
    savedState->mIsInExpandedMode = mIsInExpandedMode;

    if (mMenu != NULL) {
        CBundle::New((IBundle**)&savedState->mMenuState);
        mMenu->SavePresenterStates(savedState->mMenuState);
    }

    return savedState;
}

void PhoneWindow::PanelFeatureState::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<SavedState> savedState = (SavedState*)state;
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

void PhoneWindow::PanelFeatureState::ApplyFrozenState()
{
    if (mMenu != NULL && mFrozenMenuState != NULL) {
        mMenu->RestorePresenterStates(mFrozenMenuState);
        mFrozenMenuState = NULL;
    }
}

CAR_INTERFACE_IMPL_2(PhoneWindow::DialogMenuCallback, Object, IMenuBuilderCallback, IMenuPresenterCallback);

PhoneWindow::DialogMenuCallback::DialogMenuCallback(
    /* [in] */ Int32 featureId,
    /* [in] */ PhoneWindow* host)
    : mFeatureId(featureId)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

//CAR_INTERFACE_IMPL_2(PhoneWindow::DialogMenuCallback, Object, IMenuBuilderCallback, IMenuPresenterCallback)

ECode PhoneWindow::DialogMenuCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (allMenusAreClosing) {
        AutoPtr<IPhoneWindow> w;
        mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
        if (w == NULL)
            return NOERROR;

        AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();

        AutoPtr<IMenuBuilder> m;
        if ((menu->GetRootMenu((IMenuBuilder**)&m), m.Get()) != menu) {
            OnCloseSubMenu(menu);
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

ECode PhoneWindow::DialogMenuCallback::OnCloseSubMenu(
    /* [in] */ IMenuBuilder* menu)
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();

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

ECode PhoneWindow::DialogMenuCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* toFinish)
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();

    AutoPtr<IWindowCallback> callback;
    mHost->GetCallback((IWindowCallback**)&callback);

    Boolean destoryed = FALSE;
    if (callback != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
        return callback->OnMenuItemSelected(mFeatureId, item, toFinish);
    }

    *toFinish = FALSE;
    return NOERROR;
}

ECode PhoneWindow::DialogMenuCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return NOERROR;
}

ECode PhoneWindow::DialogMenuCallback::OnOpenSubMenu(
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

PhoneWindow::DecorRunnable::DecorRunnable(
    /* [in] */ PhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode PhoneWindow::DecorRunnable::Run()
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();
    // Invalidate if the panel menu hasn't been created before this.
    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(mHost->GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st));
    Boolean destoryed = FALSE;
    if (!(mHost->IsDestroyed(&destoryed), destoryed) && (st == NULL || st->mMenu == NULL)) {
        mHost->InvalidatePanelMenu(FEATURE_ACTION_BAR);
    }
    return NOERROR;
}

PhoneWindow::DrawableFeatureState::DrawableFeatureState(
    /* [in] */ Int32 _featureId)
    : mFeatureId(_featureId)
    , mResid(0)
    , mAlpha(255)
    , mCurAlpha(255)
{
}

PhoneWindow::InvalidatePanelMenuRunnable::InvalidatePanelMenuRunnable(
    /* [in] */ PhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode PhoneWindow::InvalidatePanelMenuRunnable::Run()
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> host = (PhoneWindow*)w.Get();
    for (Int32 i = 0; i <= FEATURE_MAX; i++) {
        if ((host->mInvalidatePanelMenuFeatures & (1 << i)) != 0) {
            host->DoInvalidatePanelMenu(i);
        }
    }

    host->mInvalidatePanelMenuPosted = FALSE;
    host->mInvalidatePanelMenuFeatures = 0;
    return NOERROR;
}

AutoPtr<IIWindowManager> PhoneWindow::WindowManagerHolder::GetWindowManager()
{
    if (sWindowManager == NULL) {
        sWindowManager = IIWindowManager::Probe(ServiceManager::GetService(String("window")));
        assert(sWindowManager != NULL);
    }
    return sWindowManager;
}

CAR_INTERFACE_IMPL(PhoneWindow::PanelMenuPresenterCallback, Object, IMenuPresenterCallback)

PhoneWindow::PanelMenuPresenterCallback::PanelMenuPresenterCallback(
    /* [in] */ PhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode PhoneWindow::PanelMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();

    AutoPtr<IMenuBuilder> mb;
    menu->GetRootMenu((IMenuBuilder**)&mb);
    IMenu* parentMenu = IMenu::Probe(mb);
    Boolean isSubMenu = (parentMenu != IMenu::Probe(menu));
    AutoPtr<PanelFeatureState> panel = mHost->FindMenuPanel(isSubMenu ? parentMenu : IMenu::Probe(menu));
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

ECode PhoneWindow::PanelMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();
    Boolean has = FALSE;
    if (subMenu == NULL && (mHost->HasFeature(FEATURE_ACTION_BAR, &has), has)) {
        AutoPtr<IWindowCallback> cb;
        mHost->GetCallback((IWindowCallback**)&cb);
        Boolean destoryed = FALSE;
        if (cb != NULL && !(mHost->IsDestroyed(&destoryed), destoryed)) {
            Boolean tmp = FALSE;
            cb->OnMenuOpened(FEATURE_ACTION_BAR, IMenu::Probe(subMenu), &tmp);
        }
    }

    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(PhoneWindow::ActionMenuPresenterCallback, Object, IMenuPresenterCallback)

PhoneWindow::ActionMenuPresenterCallback::ActionMenuPresenterCallback(
    /* [in] */ PhoneWindow* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode PhoneWindow::ActionMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();

    AutoPtr<IWindowCallback> cb;
    mHost->GetCallback((IWindowCallback**)&cb);
    if (cb != NULL) {
        Boolean tmp = FALSE;
        cb->OnMenuOpened(FEATURE_ACTION_BAR, IMenu::Probe(subMenu), &tmp);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode PhoneWindow::ActionMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    AutoPtr<IPhoneWindow> w;
    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
    if (w == NULL)
        return NOERROR;

    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();
    mHost->CheckCloseActionMenu(IMenu::Probe(menu));
    return NOERROR;
}

//=====================================================================
//       PhoneWindow::InnerSwipeDismissLayoutOnDismissedListener1
//=====================================================================
CAR_INTERFACE_IMPL(PhoneWindow::InnerSwipeDismissLayoutOnDismissedListener1, Object, IOnDismissedListener)

PhoneWindow::InnerSwipeDismissLayoutOnDismissedListener1::InnerSwipeDismissLayoutOnDismissedListener1(
    /* [in] */ PhoneWindow* owner)
    : mOwner(owner)
{
}

ECode PhoneWindow::InnerSwipeDismissLayoutOnDismissedListener1::OnDismissed(
    /* [in] */ ISwipeDismissLayout* layout)
{
    //VALIDATE_NOT_NULL(layout);
    mOwner->DispatchOnWindowDismissed();
    return NOERROR;
}

//=====================================================================
// PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1
//=====================================================================
CAR_INTERFACE_IMPL(PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1, Object, IOnSwipeProgressChangedListener)

const Float PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1::ALPHA_DECREASE = 0.5f;

PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1(
    /* [in] */ PhoneWindow* owner)
    : mIsTranslucent(FALSE)
    , mOwner(owner)
{
}

ECode PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1::OnSwipeProgressChanged(
    /* [in] */ ISwipeDismissLayout* layout,
    /* [in] */ Float progress,
    /* [in] */ Float translate)
{
    //VALIDATE_NOT_NULL(layout);
    AutoPtr<IWindowManagerLayoutParams> newParams;
    mOwner->GetAttributes((IWindowManagerLayoutParams**)&newParams);
    newParams->SetX((Int32) translate);
    newParams->SetAlpha(1 - (progress * ALPHA_DECREASE));
    mOwner->SetAttributes(newParams);

    Int32 flags = 0;
    Int32 x;
    newParams->GetX(&x);
    if (x == 0) {
        flags = IWindowManagerLayoutParams::FLAG_FULLSCREEN;
    } else {
        flags = IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS;
    }
    mOwner->SetFlags(flags, IWindowManagerLayoutParams::FLAG_FULLSCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS);
    return NOERROR;
}

ECode PhoneWindow::InnerSwipeDismissLayoutOnSwipeProgressChangedListener1::OnSwipeCancelled(
    /* [in] */ ISwipeDismissLayout* layout)
{
    //VALIDATE_NOT_NULL(layout);
    AutoPtr<IWindowManagerLayoutParams> newParams;
    mOwner->GetAttributes((IWindowManagerLayoutParams**)&newParams);
    newParams->SetX(0);
    newParams->SetAlpha(1);
    mOwner->SetAttributes(newParams);
    mOwner->SetFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN,
            IWindowManagerLayoutParams::FLAG_FULLSCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS);
    return NOERROR;
}

//CAR_INTERFACE_IMPL(PhoneWindow::MyMenuBuilderCallback, Object, IMenuBuilderCallback);
//
//PhoneWindow::MyMenuBuilderCallback::MyMenuBuilderCallback(
//    /* [in] */ PhoneWindow* host)
//{
//    host->GetWeakReference((IWeakReference**)&mWeakHost);
//}
//
//ECode PhoneWindow::MyMenuBuilderCallback::OnMenuItemSelected(
//    /* [in] */ IMenuBuilder* menu,
//    /* [in] */ IMenuItem* item,
//    /* [out] */ Boolean* state)
//{
//    VALIDATE_NOT_NULL(state)
//    *state = FALSE;
//
//    AutoPtr<IPhoneWindow> w;
//    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
//    if (w == NULL)
//        return NOERROR;
//
//    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();
//    return mHost->OnMenuItemSelected(menu, item, state);
//}
//
//ECode PhoneWindow::MyMenuBuilderCallback::OnMenuModeChange(
//    /* [in] */ IMenuBuilder* menu)
//{
//    AutoPtr<IPhoneWindow> w;
//    mWeakHost->Resolve(EIID_IPhoneWindow, (IInterface**)&w);
//    if (w == NULL)
//        return NOERROR;
//
//    AutoPtr<PhoneWindow> mHost = (PhoneWindow*)w.Get();
//    return mHost->OnMenuModeChange(menu);
//}

PhoneWindow::PhoneWindow()
    : mResourcesSetFlags(0)
    , mIconRes(0)
    , mLogoRes(0)
    , mDecor(NULL)
    , mIsFloating(FALSE)
    , mPanelChordingKey(0)
    , mBackgroundResource(0)
    , mBackgroundFallbackResource(0)
    , mElevation(0.0)
    , mClipToOutline(FALSE)
    , mFrameResource(0)
    , mTextColor(0)
    , mStatusBarColor(0)
    , mNavigationBarColor(0)
    , mForcedStatusBarColor(FALSE)
    , mForcedNavigationBarColor(FALSE)
    , mTitleColor(0)
    , mAlwaysReadCloseOnTouchAttr(FALSE)
    , mEnableGestures(FALSE)
    , mClosingActionMenu(FALSE)
    , mUiOptions(0)
    , mInvalidatePanelMenuPosted(FALSE)
    , mInvalidatePanelMenuFeatures(0)
    , mReturnTransition(USE_DEFAULT_TRANSITION)
    , mReenterTransition(USE_DEFAULT_TRANSITION)
    , mSharedElementReturnTransition(USE_DEFAULT_TRANSITION)
    , mSharedElementReenterTransition(USE_DEFAULT_TRANSITION)
    , mAllowReturnTransitionOverlap(TRUE)
    , mAllowEnterTransitionOverlap(TRUE)
    , mBackgroundFadeDurationMillis(-1)
    , mSharedElementsUseOverlay(TRUE)
{
}

PhoneWindow::~PhoneWindow()
{
}

ECode PhoneWindow::constructor(
    /* [in] */ IContext* context)
{
    mVolumeControlStreamType = IAudioManager::USE_DEFAULT_STREAM_TYPE;
    mContextMenuCallback = new DialogMenuCallback(IWindow::FEATURE_CONTEXT_MENU, this);
    CTypedValue::New((ITypedValue**)&mMinWidthMajor);
    CTypedValue::New((ITypedValue**)&mMinWidthMinor);
    mInvalidatePanelMenuRunnable = new InvalidatePanelMenuRunnable(this);

    mContext = context;
    mRetainContext = IActivity::Probe(mContext) == NULL && IService::Probe(mContext) == NULL;
    if (mRetainContext) {
        mContext->AddRef();
    }
    return LayoutInflater::From(context, (ILayoutInflater**)&mLayoutInflater);
}

PInterface PhoneWindow::Probe(
   /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IPhoneWindow*)this;
    }
    if (riid == EIID_IPhoneWindow) {
        return (IInterface*)(IPhoneWindow*)this;
    }
    else if (riid == EIID_IMenuBuilderCallback) {
        return (IInterface*)(IMenuBuilderCallback*)this;
    }
    return Window::Probe(riid);
}

ECode PhoneWindow::GetInterfaceID(
   /* [in] */ IInterface *pObject,
   /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID)
    if (pObject == ((IInterface*)(IPhoneWindow*)this)) {
        *pIID = EIID_IPhoneWindow;
        return NOERROR;
    }
    else if (pObject == ((IInterface*)(IMenuBuilderCallback*)this)) {
        *pIID = EIID_IMenuBuilderCallback;
        return NOERROR;
    }

   return Window::GetInterfaceID(pObject, pIID);
}

UInt32 PhoneWindow::AddRef()
{
   return Window::AddRef();
}

UInt32 PhoneWindow::Release()
{
   DecorView* decor = mDecor;
   UInt32 ref = Window::Release();
   if (decor != NULL && ref == 1) {
       decor->_Release();
   }
   return ref;
}

ECode PhoneWindow::SetContainer(
    /* [in] */ IWindow* container)
{
    return Window::SetContainer(container);
}

ECode PhoneWindow::RequestFeature(
    /* [in] */ Int32 featureId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mContentParent != NULL) {
        Slogger::E(TAG, "requestFeature() must be called before adding content");
        //throw new AndroidRuntimeException("requestFeature() must be called before adding content");
        return E_RUNTIME_EXCEPTION;
    }
    const Int32 features = GetFeatures();
    Int32 newFeatures = features | (1 << featureId);
    if ((newFeatures & ( 1 << IWindow::FEATURE_CUSTOM_TITLE)) != 0
           && (newFeatures & ~CUSTOM_TITLE_COMPATIBLE_FEATURES) !=0)
    {
        Slogger::E(TAG, "requestFeature() You cannot combine custom titles with other title features");
        /* Another feature is enabled and the user is trying to enable the custom title feature */
        /* Custom title feature is enabled and the user is trying to enable another feature */
        //throw new AndroidRuntimeException("You cannot combine custom titles with other title features");
        return E_RUNTIME_EXCEPTION;
    }

    if ((features & (1 << FEATURE_NO_TITLE)) != 0 && featureId == FEATURE_ACTION_BAR) {
        return NOERROR; // Ignore. No title dominates.
    }

    if ((features & (1 << FEATURE_ACTION_BAR)) != 0 && featureId == FEATURE_NO_TITLE) {
        // Remove the action bar feature if we have no title. No title dominates.
        RemoveFeature(FEATURE_ACTION_BAR);
    }

    if ((features & (1 << FEATURE_ACTION_BAR)) != 0 && featureId == FEATURE_SWIPE_TO_DISMISS) {
        //throw new AndroidRuntimeException(
        Slogger::E(TAG, "You cannot combine swipe dismissal and the action bar.");
        return E_RUNTIME_EXCEPTION;
    }
    if ((features & (1 << FEATURE_SWIPE_TO_DISMISS)) != 0 && featureId == FEATURE_ACTION_BAR) {
        //throw new AndroidRuntimeException(
        Slogger::E(TAG, "You cannot combine swipe dismissal and the action bar.");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pkManager;
    context->GetPackageManager((IPackageManager**)&pkManager);
    Boolean hasSystemFeature;
    pkManager->HasSystemFeature(IPackageManager::FEATURE_WATCH, &hasSystemFeature);
    if (featureId == FEATURE_INDETERMINATE_PROGRESS && hasSystemFeature)
    {
        //throw new AndroidRuntimeException("You cannot use indeterminate progress on a watch.");
        Slogger::E(TAG, "You cannot use indeterminate progress on a watch.");
        return E_RUNTIME_EXCEPTION;
    }

    return Window::RequestFeature(featureId, result);
}

ECode PhoneWindow::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    mUiOptions = uiOptions;
    return NOERROR;
}

ECode PhoneWindow::SetUiOptions(
    /* [in] */ Int32 uiOptions,
    /* [in] */ Int32 mask)
{
    mUiOptions = (mUiOptions & ~mask) | (uiOptions & mask);
    return NOERROR;
}

ECode PhoneWindow::SetContentView(
    /* [in] */ Int32 layoutResID)
{
    Boolean hFeature = FALSE;
    HasFeature(FEATURE_CONTENT_TRANSITIONS, &hFeature);
    if (mContentParent == NULL) {
        InstallDecor();
    }
    else if (!hFeature) {
        mContentParent->RemoveAllViews();
    }

    if (hFeature) {
        AutoPtr<IScene> newScene;
        AutoPtr<ISceneHelper> sceneHelper;
        CSceneHelper::AcquireSingleton((ISceneHelper**)&sceneHelper);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        sceneHelper->GetSceneForLayout(mContentParent, layoutResID, context, (IScene**)&newScene);
        TransitionTo(newScene);
    }
    else {
        AutoPtr<IView> root;
        mLayoutInflater->Inflate(layoutResID, mContentParent.Get(), (IView**)&root);
    }

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean destoryed = FALSE;
    if (cb != NULL && !(IsDestroyed(&destoryed), destoryed)) {
        cb->OnContentChanged();
    }

    return NOERROR;
}

ECode PhoneWindow::SetContentView(
    /* [in] */ IView* view)
{
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::MATCH_PARENT,
        (IViewGroupLayoutParams**)&params);

    return SetContentView(view, params);
}

ECode PhoneWindow::SetContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    // Note: FEATURE_CONTENT_TRANSITIONS may be set in the process of installing the window
    // decor, when theme attributes and the like are crystalized. Do not check the feature
    // before this happens.
    Boolean hFeature = FALSE;
    HasFeature(FEATURE_CONTENT_TRANSITIONS, &hFeature);
    if (mContentParent == NULL) {
        InstallDecor();
    }
    else if (!hFeature)
    {
        mContentParent->RemoveAllViews();
    }

    if (hFeature)
    {
        view->SetLayoutParams(params);
        AutoPtr<IScene> newScene;
        CScene::New(mContentParent, view, (IScene**)&newScene);
        TransitionTo(newScene);
    }
    else
    {
        mContentParent->AddView(view, params);
    }

    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    if (cb != NULL) {
        cb->OnContentChanged();
    }

    return NOERROR;
}

ECode PhoneWindow::AddContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    if (mContentParent == NULL) {
        InstallDecor();
    }

    Boolean hFeature = FALSE;
    HasFeature(FEATURE_CONTENT_TRANSITIONS, &hFeature);
    if (hFeature)
    {
        Logger::V(TAG, "addContentView does not support content transitions");
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

ECode PhoneWindow::GetCurrentFocus(
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

ECode PhoneWindow::TakeSurface(
    /* [in] */ ISurfaceHolderCallback2* cb)
{
    mTakeSurfaceCallback = cb;
    return NOERROR;
}

ECode PhoneWindow::TakeInputQueue(
    /* [in] */ IInputQueueCallback* callback)
{
    mTakeInputQueueCallback = callback;

    return NOERROR;
}

ECode PhoneWindow::IsFloating(
    /* [out] */ Boolean* isFloating)
{
    VALIDATE_NOT_NULL(isFloating);
    *isFloating = mIsFloating;
    return NOERROR;
}

ECode PhoneWindow::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = mLayoutInflater;
    REFCOUNT_ADD(*inflater);
    return NOERROR;
}

ECode PhoneWindow::SetTitle(
    /* [in] */ ICharSequence* title)
{
    if (mTitleView != NULL) {
        mTitleView->SetText(title);
    }
    else if (mDecorContentParent != NULL) {
        mDecorContentParent->SetWindowTitle(title);
    }

    mTitle = title;
    return NOERROR;
}

ECode PhoneWindow::SetTitleColor(
    /* [in] */ Int32 textColor)
{
    if (mTitleView != NULL) {
        mTitleView->SetTextColor(textColor);
    }
    mTitleColor = textColor;
    return NOERROR;
}

ECode PhoneWindow::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Action bars handle their own menu state
    if (mDecorContentParent == NULL)
    {
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
ECode PhoneWindow::OnKeyDownPanel(
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
ECode PhoneWindow::OnKeyUpPanel(
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

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IViewConfigurationHelper> vcHelper;
        CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
        AutoPtr<IViewConfiguration> viewConfig;
        vcHelper->Get(context, (IViewConfiguration**)&viewConfig);
        Boolean hasPerMk;
        viewConfig->HasPermanentMenuKey(&hasPerMk);
        if (featureId == FEATURE_OPTIONS_PANEL && mDecorContentParent != NULL
            && (mDecorContentParent->CanShowOverflowMenu(&reserved), reserved)
            && !hasPerMk)
        {
            Boolean showing = FALSE;
            if (!(mDecorContentParent->IsOverflowMenuShowing(&showing), showing)) {
                Boolean prepared = FALSE, destoryed = FALSE;
                if (!(IsDestroyed(&destoryed), destoryed) && (PreparePanel(st, event, &prepared), prepared)) {
                    mDecorContentParent->ShowOverflowMenu(&playSoundEffect);
                }
            } else {
                mDecorContentParent->HideOverflowMenu(&playSoundEffect);
            }
        }
        else {
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

void PhoneWindow::ClearMenuViews(
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

ECode PhoneWindow::OpenPanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IKeyEvent* event)
{
    Boolean reserved = FALSE;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> viewConfig;
    vcHelper->Get(context, (IViewConfiguration**)&viewConfig);
    Boolean hasPerMk;
    viewConfig->HasPermanentMenuKey(&hasPerMk);
    if (featureId == FEATURE_OPTIONS_PANEL && mDecorContentParent != NULL
        && (mDecorContentParent->CanShowOverflowMenu(&reserved), reserved)
        && !hasPerMk)
    {
        Boolean tmp = FALSE;
        mDecorContentParent->ShowOverflowMenu(&tmp);
    }
    else {
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
Boolean PhoneWindow::InitializePanelDecor(
    /* [in] */ PanelFeatureState* st)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    st->mDecorView = new DecorView(this);
    st->mDecorView->constructor(context, st->mFeatureId, TRUE);
    st->mGravity = IGravity::CENTER | IGravity::BOTTOM;
    st->SetStyle(context);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::Window),
        ArraySize(R::styleable::Window));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(NULL, attrIds, 0, st->mListPresenterTheme, (ITypedArray**)&a);

    Float evelation;
    a->GetDimension(R::styleable::Window_windowElevation, 0, &evelation);
    if (evelation != 0) {
        st->mDecorView->SetElevation(evelation);
    }
    a->Recycle();

    return TRUE;
}

Int32 PhoneWindow::GetOptionsPanelGravity()
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

void PhoneWindow::OnOptionsPanelRotationChanged()
{
    AutoPtr<PanelFeatureState> st;
    GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st);
    if (st == NULL) return;

    AutoPtr<IWindowManagerLayoutParams> lp;
    if (st->mDecorView != NULL) {
        AutoPtr<IViewGroupLayoutParams> vglp;
        st->mDecorView->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        lp = IWindowManagerLayoutParams::Probe(vglp);
    }

    if (lp != NULL) {
        ((CWindowManagerLayoutParams*)lp.Get())->mGravity = GetOptionsPanelGravity();
        AutoPtr<IWindowManager> wm;
        GetWindowManager((IWindowManager**)&wm);
        if (wm != NULL) {
            IViewManager::Probe(wm)->UpdateViewLayout(IView::Probe(st->mDecorView), IViewGroupLayoutParams::Probe(lp));
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
Boolean PhoneWindow::InitializePanelContent(
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
    Boolean res;
    st->IsInListMode(&res);
    AutoPtr<IMenuView> menuView = res
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

void PhoneWindow::OpenPanel(
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
    if ((cb != NULL) && (cb->OnMenuOpened(st->mFeatureId, IMenu::Probe(st->mMenu), &isOpened), !isOpened)) {
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
    Boolean temp;
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
        Boolean res;
        if (!InitializePanelContent(st) || !(st->HasPanelItems(&res), res)) {
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
            width = IViewGroupLayoutParams::MATCH_PARENT;
        }
        else {
            // Otherwise, set the normal panel background
            backgroundResId = st->mBackground;
        }

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(backgroundResId, (IDrawable**)&drawable);

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
    else if (!(st->IsInListMode(&temp), temp)) {
        width = IViewGroupLayoutParams::MATCH_PARENT;
    }
    else if (st->mCreatedPanelView != NULL) {
        // If we already had a panel view, carry width=MATCH_PARENT through
        // as we did above when it was created.
        AutoPtr<IViewGroupLayoutParams> lp;
        st->mCreatedPanelView->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 lpWidth = 0;
        if (lp != NULL && (lp->GetWidth(&lpWidth), lpWidth) == IViewGroupLayoutParams::MATCH_PARENT) {
            width = IViewGroupLayoutParams::MATCH_PARENT;
        }
    }

    st->mIsHandled = FALSE;

    AutoPtr<IWindowManagerLayoutParams> wlp;
    CWindowManagerLayoutParams::New(
            width, IViewGroupLayoutParams::WRAP_CONTENT,
            st->mX, st->mY, IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG,
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            st->mDecorView->mDefaultOpacity, (IWindowManagerLayoutParams**)&wlp);

    if (st->mIsCompact) {
        ((CWindowManagerLayoutParams*)wlp.Get())->mGravity = GetOptionsPanelGravity();
        sRotationWatcher->AddWindow(this);
    } else {
        ((CWindowManagerLayoutParams*)wlp.Get())->mGravity = st->mGravity;
    }

    ((CWindowManagerLayoutParams*)wlp.Get())->mWindowAnimations = st->mWindowAnimations;

    IViewManager::Probe(wm)->AddView((IView*)st->mDecorView.Get(), IViewGroupLayoutParams::Probe(wlp));
    st->mIsOpen = TRUE;
   // Log.v(TAG, "Adding main menu to window manager.");
}

ECode PhoneWindow::ClosePanel(
    /* [in] */ Int32 featureId)
{
    Boolean reserved = FALSE;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> viewConfig;
    vcHelper->Get(context, (IViewConfiguration**)&viewConfig);
    Boolean hasPerMk;
    viewConfig->HasPermanentMenuKey(&hasPerMk);
    if (featureId == FEATURE_OPTIONS_PANEL && mDecorContentParent != NULL
        && (mDecorContentParent->CanShowOverflowMenu(&reserved), reserved)
        && !hasPerMk)
    {
        Boolean tmp = FALSE;
        mDecorContentParent->HideOverflowMenu(&tmp);
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

ECode PhoneWindow::ClosePanel(
    /* [in] */ PanelFeatureState* st,
    /* [in] */ Boolean doCallback)
{
    Boolean showing = FALSE;
    if (doCallback && st->mFeatureId == FEATURE_OPTIONS_PANEL
        && mDecorContentParent != NULL
        && (mDecorContentParent->IsOverflowMenuShowing(&showing), showing)) {
        CheckCloseActionMenu(IMenu::Probe(st->mMenu));
        return NOERROR;
    }

    AutoPtr<IWindowManager> wm;
    GetWindowManager((IWindowManager**)&wm);
    if ((wm != NULL) && st->mIsOpen) {
        if (st->mDecorView != NULL) {
            IViewManager::Probe(wm)->RemoveView((IView*)st->mDecorView.Get());
            // Log.v(TAG, "Removing main menu from window manager.");
            if (st->mIsCompact) {
                sRotationWatcher->RemoveWindow(this);
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
    // PhoneWindow->PanelFeatureState->CMenuBuilder->Activity->PhoneWindow
    // PhoneWindow->PanelFeatureState->DecorView->PhoneWindow
    // CMenuBuilder->CListMenuPresenter->CMenuBuilder, and so on
    //
    st->SetMenu(NULL);
    mPanels->Set(st->mFeatureId, NULL);

    return NOERROR;
}

void PhoneWindow::CheckCloseActionMenu(
    /* [in] */ IMenu* menu)
{
    if (mClosingActionMenu) {
        return;
    }

    mClosingActionMenu = TRUE;
    mDecorContentParent->DismissPopups();
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
void PhoneWindow::CallOnPanelClosed(
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
            menu = IMenu::Probe(panel->mMenu);
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

Boolean PhoneWindow::LaunchDefaultSearch()
{
    Boolean result;
    AutoPtr<IWindowCallback> cb;
    GetCallback((IWindowCallback**)&cb);
    Boolean isDestroyed = FALSE;
    if (cb == NULL || (IsDestroyed(&isDestroyed), isDestroyed)) {
        result = FALSE;
    }
    else {
        SendCloseSystemWindows(String("search"));
        Boolean value = FALSE;
        cb->OnSearchRequested(&value);
        result = value;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    Int32 uiMode;
    configuration->GetUiMode(&uiMode);
    if (!result && (uiMode & IConfiguration::UI_MODE_TYPE_MASK) == IConfiguration::UI_MODE_TYPE_TELEVISION) {
        // On TVs, if the app doesn't implement search, we want to launch assist.
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
        ISearchManager* searchManager = ISearchManager::Probe(obj);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId;
        helper->GetMyUserId(&userId);
        Boolean res;
        searchManager->LaunchAssistAction(0, String(NULL), userId, &res);
        result = res;
    }
    return result;
}

void PhoneWindow::TransitionTo(
    /* [in] */ IScene* scene)
{
    if (mContentScene == NULL)
    {
        scene->Enter();
    }
    else
    {
        mTransitionManager->TransitionTo(scene);
    }
    mContentScene = scene;
}

void PhoneWindow::RegisterSwipeCallbacks()
{
    AutoPtr<IView> v;
    FindViewById(R::id::content, (IView**)&v);
    ISwipeDismissLayout* swipeDismiss = ISwipeDismissLayout::Probe(v);
    AutoPtr<IOnDismissedListener> odListener = new InnerSwipeDismissLayoutOnDismissedListener1(this);
    swipeDismiss->SetOnDismissedListener(odListener);
    AutoPtr<IOnSwipeProgressChangedListener> owpcListener = new InnerSwipeDismissLayoutOnSwipeProgressChangedListener1(this);
    swipeDismiss->SetOnSwipeProgressChangedListener(owpcListener);
}

AutoPtr<IViewRootImpl> PhoneWindow::GetViewRootImpl()
{
    if (mDecor != NULL) {
        AutoPtr<IViewRootImpl> viewRootImpl;
        mDecor->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
        if (viewRootImpl != NULL) {
            return viewRootImpl;
        }
    }
    //throw new IllegalStateException("view not added");
    Logger::E(TAG, "PhoneWindow::GetViewRootImpl error");
    return NULL;
}

Boolean PhoneWindow::IsTranslucent()
{
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(GetWindowStyle((ITypedArray**)&a));
    Int32 resId;
    a->GetResourceId(R::styleable::Window_windowIsTranslucent, 0, &resId);
    Boolean res = FALSE;
    a->GetBoolean(resId, FALSE, &res);
    return res;
}

AutoPtr<ITransition> PhoneWindow::GetTransition(
    /* [in] */ ITransition* currentValue,
    /* [in] */ ITransition* defaultValue,
    /* [in] */ Int32 id)
{
    if (currentValue != defaultValue) {
        return currentValue;
    }
    Int32 transitionId;
    AutoPtr<ITypedArray> a;
    GetWindowStyle((ITypedArray**)&a);
    a->GetResourceId(id, -1, &transitionId);
    AutoPtr<ITransition> transition = defaultValue;
    if (transitionId != -1 && transitionId != R::transition::no_transition) {
        AutoPtr<ITransitionInflater> inflater;
        AutoPtr<ITransitionInflaterHelper> tifHelper;
        CTransitionInflaterHelper::AcquireSingleton((ITransitionInflaterHelper**)&tifHelper);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        tifHelper->From(context, (ITransitionInflater**)&inflater);
        inflater->InflateTransition(transitionId, (ITransition**)&transition);
        ITransitionSet* ts = ITransitionSet::Probe(transition);
        Int32 count = 0;
        if (ts != NULL && (ts->GetTransitionCount(&count), (count == 0))) {
            transition = NULL;
        }
    }
    return transition;
}

//void PhoneWindow::OpenPanel(
//    /* [in] */ PanelFeatureState* st,
//    /* [in] */ IKeyEvent* event)
//{
//    //
//}

/**
 * Closes the context menu. This notifies the menu logic of the close, along
 * with dismissing it from the UI.
 */
void PhoneWindow::CloseContextMenu()
{
    AutoLock lock(this);

    if (mContextMenu != NULL) {
        IMenu::Probe(mContextMenu)->Close();
        DismissContextMenu();
    }
}

/**
 * Dismisses just the context menu UI. To close the context menu, use
 * {@link #closeContextMenu()}.
 */
void PhoneWindow::DismissContextMenu()
{
    AutoLock lock(this);

    mContextMenu = NULL;

    if (mContextMenuHelper != NULL) {
        mContextMenuHelper->Dismiss();
        mContextMenuHelper = NULL;
    }
}

ECode PhoneWindow::TogglePanel(
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

ECode PhoneWindow::InvalidatePanelMenu(
    /* [in] */ Int32 featureId)
{
    mInvalidatePanelMenuFeatures |= 1 << featureId;

    if (!mInvalidatePanelMenuPosted && mDecor != NULL) {
        mDecor->PostOnAnimation(mInvalidatePanelMenuRunnable);
        mInvalidatePanelMenuPosted = TRUE;
    }

    return NOERROR;
}

void PhoneWindow::DoInvalidatePanelMenu(
    /* [in] */ Int32 featureId)
{
    AutoPtr<PanelFeatureState> st;
    /*ECode ec = */GetPanelState(featureId, TRUE, (PanelFeatureState**)&st);

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
            IMenu::Probe(st->mMenu)->Clear();
        }

        st->mRefreshMenuContent = TRUE;
        st->mRefreshDecorView = TRUE;
    }

    // Prepare the options panel if we have an action bar
    if ((featureId == FEATURE_ACTION_BAR || featureId == FEATURE_OPTIONS_PANEL) && mDecorContentParent != NULL)
    {
        st = NULL;
        GetPanelState(IWindow::FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st);
        if (st != NULL) {
            st->mIsPrepared = FALSE;
            Boolean tmp = FALSE;
            PreparePanel(st, NULL, &tmp);
        }
    }
}

ECode PhoneWindow::CloseAllPanels()
{
    AutoPtr<IWindowManager> wm;
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

ECode PhoneWindow::PerformPanelShortcut(
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

Boolean PhoneWindow::PerformPanelShortcut(
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
        IMenu::Probe(st->mMenu)->PerformShortcut(keyCode, event, flags, &handled);
    }

    if (handled) {
        // Mark as handled
        st->mIsHandled = TRUE;

        // Only close down the menu if we don't have an action bar keeping it open.
        if ((flags & IMenu::FLAG_PERFORM_NO_CLOSE) == 0 && mDecorContentParent == NULL)
        {
            ClosePanel(st, TRUE);
        }
    }

    return handled;
}

ECode PhoneWindow::PerformPanelIdentifierAction(
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

    IMenu::Probe(st->mMenu)->PerformIdentifierAction(id, flags, succeeded);

    // Only close down the menu if we don't have an action bar keeping it open.
    if (mDecorContentParent == NULL)
    {
        ClosePanel(st, TRUE);
    }

    return NOERROR;
}

ECode PhoneWindow::PerformContextMenuIdentifierAction(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    if (mContextMenu != NULL) {
        return IMenu::Probe(mContextMenu)->PerformIdentifierAction(id, flags, succeeded);
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode PhoneWindow::SetBackgroundDrawable(
    /* [in] */ IDrawable* drawable)
{
    if (drawable != mBackgroundDrawable || mBackgroundResource != 0) {
        mBackgroundResource = 0;
        mBackgroundDrawable = drawable;
        if (mDecor != NULL) {
            mDecor->SetWindowBackground(drawable);
        }

        if (mBackgroundFallbackResource != 0) {
            mDecor->SetBackgroundFallback(drawable != NULL? 0 : mBackgroundFallbackResource);
        }
    }
    return NOERROR;
}

ECode PhoneWindow::SetFeatureDrawableResource(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        AutoPtr<DrawableFeatureState> st;
        GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
        if (st->mResid != resId) {
            st->mResid = resId;
            st->mUri = NULL;

            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IDrawable> drawable;
            context->GetDrawable(resId, (IDrawable**)&drawable);
            st->mLocal = drawable;
            UpdateDrawable(featureId, st, FALSE);
        }
    } else {
        SetFeatureDrawable(featureId, NULL);
    }
    return NOERROR;
}

ECode PhoneWindow::SetFeatureDrawableUri(
    /* [in] */ Int32 featureId,
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        AutoPtr<DrawableFeatureState> st;
        GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
        Boolean isEquals = FALSE;
        if (st->mUri == NULL || !(IObject::Probe(st->mUri)->Equals(uri, &isEquals), isEquals)) {
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

ECode PhoneWindow::SetFeatureDrawable(
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

ECode PhoneWindow::SetFeatureDrawableAlpha(
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

ECode PhoneWindow::SetFeatureInt(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value)
{
    // XXX Should do more management (as with drawable features) to
    // deal with interactions between multiple window policies.
    UpdateInt(featureId, value, FALSE);
    return NOERROR;
}

ECode PhoneWindow::SetIcon(
    /* [in] */ Int32 resId)
{
    mIconRes = resId;
    mResourcesSetFlags |= FLAG_RESOURCE_SET_ICON;
    mResourcesSetFlags &= ~FLAG_RESOURCE_SET_ICON_FALLBACK;
    if (mDecorContentParent != NULL)
    {
        mDecorContentParent->SetIcon(resId);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetDefaultIcon(
    /* [in] */ Int32 resId)
{
    if ((mResourcesSetFlags & FLAG_RESOURCE_SET_ICON) != 0) {
        return NOERROR;
    }
    mIconRes = resId;
    Boolean tmp;
    if (mDecorContentParent != NULL && (!(mDecorContentParent->HasIcon(&tmp), tmp) ||
                (mResourcesSetFlags & FLAG_RESOURCE_SET_ICON_FALLBACK) != 0))
    {
        if (resId != 0) {
            mDecorContentParent->SetIcon(resId);
            mResourcesSetFlags &= ~FLAG_RESOURCE_SET_ICON_FALLBACK;
        } else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IPackageManager> pkManager;
            context->GetPackageManager((IPackageManager**)&pkManager);
            AutoPtr<IDrawable> daIcon;
            pkManager->GetDefaultActivityIcon((IDrawable**)&daIcon);
            mDecorContentParent->SetIcon(daIcon);
            mResourcesSetFlags |= FLAG_RESOURCE_SET_ICON_FALLBACK;
        }
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetLogo(
    /* [in] */ Int32 resId)
{
    mLogoRes = resId;
    mResourcesSetFlags |= FLAG_RESOURCE_SET_LOGO;
    if (mDecorContentParent != NULL) {
        mDecorContentParent->SetLogo(resId);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetDefaultLogo(
    /* [in] */ Int32 resId)
{
    if ((mResourcesSetFlags & FLAG_RESOURCE_SET_LOGO) != 0) {
        return NOERROR;
    }
    mLogoRes = resId;
    Boolean tmp;
    if (mDecorContentParent != NULL && !(mDecorContentParent->HasLogo(&tmp), tmp)) {
        mDecorContentParent->SetLogo(resId);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetLocalFocus(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean inTouchMode)
{
    AutoPtr<IViewRootImpl> vri = GetViewRootImpl();
    if (vri == NULL)
    {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    vri->WindowFocusChanged(hasFocus, inTouchMode);

    return NOERROR;
}

// @Override
ECode PhoneWindow::InjectInputEvent(
    /* [in] */ IInputEvent* event)
{
    AutoPtr<IViewRootImpl> vri = GetViewRootImpl();
    if (vri == NULL)
    {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return vri->DispatchInputEvent(event);
}


ECode PhoneWindow::TakeKeyEvents(
    /* [in] */ Boolean get)
{
    return mDecor->SetFocusable(get);
}

ECode PhoneWindow::SuperDispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* succeeded)
{
    return mDecor->SuperDispatchKeyEvent(event, succeeded);
}

ECode PhoneWindow::SuperDispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* succeeded)
{
    return mDecor->SuperDispatchKeyShortcutEvent(event, succeeded);
}

ECode PhoneWindow::SuperDispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    return mDecor->SuperDispatchTouchEvent(event, succeeded);
}

ECode PhoneWindow::SuperDispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    return mDecor->SuperDispatchTrackballEvent(event, succeeded);
}

ECode PhoneWindow::SuperDispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* succeeded)
{
    return mDecor->SuperDispatchGenericMotionEvent(event, succeeded);
}

/**
 * A key was pressed down and not handled by anything else in the window.
 *
 * @see #onKeyUp
 * @see android.view.KeyEvent
 */
Boolean PhoneWindow::OnKeyDown(
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
        {
            Int32 direction = keyCode == IKeyEvent::KEYCODE_VOLUME_UP ? IAudioManager::ADJUST_RAISE
                : IAudioManager::ADJUST_LOWER;
            // If we have a session send it the volume command, otherwise
            // use the suggested stream.
            if (mMediaController != NULL) {
                mMediaController->AdjustVolume(direction, IAudioManager::FLAG_SHOW_UI);
            } else {
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IMediaSessionLegacyHelper> mediaSessionLegacyHelper;
                AutoPtr<IMediaSessionLegacyHelperHelper> mediaSessionLegacyHelperHelper;
                CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mediaSessionLegacyHelperHelper);
                mediaSessionLegacyHelperHelper->GetHelper(context, (IMediaSessionLegacyHelper**)&mediaSessionLegacyHelper);
                mediaSessionLegacyHelper->SendAdjustVolumeBy(mVolumeControlStreamType, direction,
                                        IAudioManager::FLAG_SHOW_UI | IAudioManager::FLAG_VIBRATE);
            }
            return TRUE;
        }
        case IKeyEvent::KEYCODE_VOLUME_MUTE: {
            // Similar code is in PhoneFallbackEventHandler in case the window
            // doesn't have one of these.  In this case, we execute it here and
            // eat the event instead, because we have mVolumeControlStreamType
            // and they don't.
            AutoPtr<IAudioManager> am = GetAudioManager();
            am->HandleKeyDown(event, mVolumeControlStreamType);
            return TRUE;
        }
        // These are all the recognized media key codes in
        // KeyEvent.isMediaKey()
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD: {
            if (mMediaController != NULL) {
                Boolean tmp;
                if ((mMediaController->DispatchMediaButtonEvent(event, &tmp), tmp)) {
                    return TRUE;
                }
            }
            return FALSE;
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
                dispatcher->StartTracking(event, TO_IINTERFACE(this));
            }
            return TRUE;
        }

    }

    return FALSE;
}

AutoPtr<IKeyguardManager> PhoneWindow::GetKeyguardManager()
{
    if (mKeyguardManager == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
        mKeyguardManager = IKeyguardManager::Probe(service);
    }

    return mKeyguardManager;
}

AutoPtr<IAudioManager> PhoneWindow::GetAudioManager()
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
Boolean PhoneWindow::OnKeyUp(
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
    case IKeyEvent::KEYCODE_VOLUME_DOWN: {
        // If we have a session send it the volume command, otherwise
        // use the suggested stream.
        if (mMediaController != NULL) {
            mMediaController->AdjustVolume(0, IAudioManager::FLAG_PLAY_SOUND | IAudioManager::FLAG_VIBRATE);
        }
        else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IMediaSessionLegacyHelper> mediaSessionLegacyHelper;
            AutoPtr<IMediaSessionLegacyHelperHelper> mediaSessionLegacyHelperHelper;
            CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mediaSessionLegacyHelperHelper);
            mediaSessionLegacyHelperHelper->GetHelper(context, (IMediaSessionLegacyHelper**)&mediaSessionLegacyHelper);
            mediaSessionLegacyHelper->SendAdjustVolumeBy(
                mVolumeControlStreamType, 0, IAudioManager::FLAG_PLAY_SOUND | IAudioManager::FLAG_VIBRATE);
        }
        return TRUE;
    }
    case IKeyEvent::KEYCODE_VOLUME_MUTE: {
            // Similar code is in PhoneFallbackEventHandler in case the window
            // doesn't have one of these.  In this case, we execute it here and
            // eat the event instead, because we have mVolumeControlStreamType
            // and they don't.
            AutoPtr<IAudioManager> am = GetAudioManager();
            am->HandleKeyUp(event, mVolumeControlStreamType);
            return TRUE;
        }
    // These are all the recognized media key codes in
    // KeyEvent.isMediaKey()
    case IKeyEvent::KEYCODE_MEDIA_PLAY:
    case IKeyEvent::KEYCODE_MEDIA_PAUSE:
    case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
    case IKeyEvent::KEYCODE_MUTE:
    case IKeyEvent::KEYCODE_HEADSETHOOK:
    case IKeyEvent::KEYCODE_MEDIA_STOP:
    case IKeyEvent::KEYCODE_MEDIA_NEXT:
    case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
    case IKeyEvent::KEYCODE_MEDIA_REWIND:
    case IKeyEvent::KEYCODE_MEDIA_RECORD:
    case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD: {
        if (mMediaController != NULL) {
            Boolean tmp;
            if (mMediaController->DispatchMediaButtonEvent(event, &tmp), tmp) {
                return TRUE;
            }
        }
        return FALSE;
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

void PhoneWindow::OnActive()
{
}

ECode PhoneWindow::GetDecorView(
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

ECode PhoneWindow::PeekDecorView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = (IView*)mDecor;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode PhoneWindow::SaveHierarchyState(
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

    AutoPtr<ISparseArray> states;
    CSparseArray::New((ISparseArray**)&states);
    IView::Probe(mContentParent)->SaveHierarchyState(states);
    state->PutSparseParcelableArray(VIEWS_TAG, states);

    // save the focused view id
    AutoPtr<IView> focusedView;
    IView::Probe(mContentParent)->FindFocus((IView**)&focusedView);
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
    AutoPtr<ISparseArray> panelStates;
    CSparseArray::New((ISparseArray**)&panelStates);
    SavePanelState(panelStates);
    Int32 size = 0;
    if (size > 0) {
        state->PutSparseParcelableArray(PANELS_TAG, panelStates);
    }

    if (mDecorContentParent != NULL) {
        AutoPtr<ISparseArray> actionBarStates;
        CSparseArray::New((ISparseArray**)&actionBarStates);
        mDecorContentParent->SaveToolbarHierarchyState(actionBarStates);
        state->PutSparseParcelableArray(ACTION_BAR_TAG, actionBarStates);
    }

    *instanceState = state;
    REFCOUNT_ADD(*instanceState);
    return NOERROR;
}

ECode PhoneWindow::RestoreHierarchyState(
    /* [in] */ IBundle* savedInstanceState)
{
    if (mContentParent == NULL) {
        return NOERROR;
    }

    //AutoPtr<IObjectInt32Map> savedStates;
    AutoPtr<ISparseArray> savedStates;
    savedInstanceState->GetSparseParcelableArray(VIEWS_TAG, (ISparseArray**)&savedStates);
    if (savedStates != NULL) {
        IView::Probe(mContentParent)->RestoreHierarchyState(savedStates);
    }

    // restore the focused view
    Int32 focusedViewId = 0;
    savedInstanceState->GetInt32(FOCUSED_ID_TAG, IView::NO_ID, &focusedViewId);
    if (focusedViewId != IView::NO_ID) {
        AutoPtr<IView> needsFocus;
        IView::Probe(mContentParent)->FindViewById(focusedViewId, (IView**)&needsFocus);
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
    AutoPtr<ISparseArray> panelStates;
    savedInstanceState->GetSparseParcelableArray(PANELS_TAG, (ISparseArray**)&panelStates);
    if (panelStates != NULL) {
        RestorePanelState(panelStates);
    }

    if (mDecorContentParent != NULL) {
        AutoPtr<ISparseArray> actionBarStates;
        savedInstanceState->GetSparseParcelableArray(ACTION_BAR_TAG, (ISparseArray**)&actionBarStates);
        if (actionBarStates != NULL) {
            DoPendingInvalidatePanelMenu();
            mDecorContentParent->RestoreToolbarHierarchyState(actionBarStates);
        }
        else {
            Logger::W(TAG, "Missing saved instance states for action bar views! State will not be restored.");
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
void PhoneWindow::SavePanelState(
    /* [in] */ ISparseArray* icicles)
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
void PhoneWindow::RestorePanelState(
    /* [in] */ ISparseArray* icicles)
{

    Int32 curFeatureId = 0;
    Int32 size = 0;
    icicles->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        //AutoPtr< ArrayOf<Int32> > keys;
        //icicles->GetKeys((ArrayOf<Int32>**)&keys);
        //curFeatureId = (*keys)[i];
        icicles->KeyAt(i, &curFeatureId);
        AutoPtr<PanelFeatureState> st;
        GetPanelState(curFeatureId, FALSE /* required */, (PanelFeatureState**)&st);
        if (st == NULL) {
            // The panel must not have been required, and is currently not around, skip it
            continue;
        }
        AutoPtr<IInterface> istate;
        icicles->Get(curFeatureId, (IInterface**)&istate);
        IParcelable* state = IParcelable::Probe(istate);
        st->OnRestoreInstanceState(state);
        InvalidatePanelMenu(curFeatureId);
    }

    /*
     * Implementation note: call openPanelsAfterRestore later to actually open the
     * restored panels.
     */
}

AutoPtr<PhoneWindow::DecorView> PhoneWindow::GenerateDecor()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<PhoneWindow::DecorView> decor = new DecorView(this);
    decor->constructor(context.Get(), -1, FALSE);
    return decor;
}

ECode PhoneWindow::GenerateLayout(
    /* [in] */ DecorView* decor,
    /* [out] */ IViewGroup** viewGroup)
{
    assert(decor != NULL);
    VALIDATE_NOT_NULL(viewGroup);
    *viewGroup = NULL;

    // Apply data from current theme.

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(GetWindowStyle((ITypedArray**)&a));

//    if (FALSE) {
//        System.out.println("From style:");
//        String s = "Attrs:";
//        for (Int32 i = 0; i < R.styleable.Window.length; i++) {
//            s = s + " " + Integer.toHexString(R.styleable.Window[i]) + "="
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

    if (a->GetBoolean(R::styleable::Window_windowSwipeToDismiss, FALSE, &value), value) {
        RequestFeature(FEATURE_SWIPE_TO_DISMISS, &value);
    }

    a->GetBoolean(R::styleable::Window_windowFullscreen, FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN,
                IWindowManagerLayoutParams::FLAG_FULLSCREEN & (~GetForcedWindowFlags()));
    }

    a->GetBoolean(R::styleable::Window_windowTranslucentStatus, FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS,
                IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS& (~GetForcedWindowFlags()));
    }

    a->GetBoolean(R::styleable::Window_windowTranslucentNavigation, FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION,
                IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION& (~GetForcedWindowFlags()));
    }

    a->GetBoolean(R::styleable::Window_windowOverscan, FALSE, &value);
    if (value) {
        SetFlags(IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN,
                IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN& (~GetForcedWindowFlags()));
    }

    a->GetBoolean(R::styleable::Window_windowShowWallpaper, FALSE, &value);
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

    if (a->GetBoolean(R::styleable::Window_windowContentTransitions, FALSE, &value), value) {
        RequestFeature(FEATURE_CONTENT_TRANSITIONS, &value);
    }
    if (a->GetBoolean(R::styleable::Window_windowActivityTransitions, FALSE, &value), value) {
        RequestFeature(FEATURE_ACTIVITY_TRANSITIONS, &value);
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    IWindowManager* windowService = IWindowManager::Probe(obj);

    if (windowService != NULL) {
        AutoPtr<IDisplay> display;
        windowService->GetDefaultDisplay((IDisplay**)&display);
        Int32 displayId;
        display->GetDisplayId(&displayId);
        Boolean shouldUseBottomOutset = displayId == IDisplay::DEFAULT_DISPLAY
            || (GetForcedWindowFlags() & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0;
        a->HasValue(R::styleable::Window_windowOutsetBottom, &has);
        if (shouldUseBottomOutset && has) {
            if (mOutsetBottom == NULL) {
                CTypedValue::New((ITypedValue**)&mOutsetBottom);
            }
            a->GetValue(R::styleable::Window_windowOutsetBottom, mOutsetBottom, &value);
        }
    }

    const Boolean targetPreHoneycomb = targetSdkVersion < Build::VERSION_CODES::HONEYCOMB;
    const Boolean targetPreIcs = targetSdkVersion < Build::VERSION_CODES::ICE_CREAM_SANDWICH;
    const Boolean targetPreL = targetSdkVersion < Build::VERSION_CODES::LOLLIPOP;

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

    // Non-floating windows on high end devices must put up decor beneath the system bars and
    // therefore must know about visibility changes of those.
    Boolean isHighEndGfx;
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->IsHighEndGfx(&isHighEndGfx);
    if (!mIsFloating && isHighEndGfx) {
        if (!targetPreL && (a->GetBoolean(R::styleable::Window_windowDrawsSystemBarBackgrounds, FALSE, &value), value)) {
            SetFlags(IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS,
                    IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS & ~GetForcedWindowFlags());
        }
    }
    if (!mForcedStatusBarColor) {
        a->GetColor(R::styleable::Window_statusBarColor, 0xFF000000, &mStatusBarColor);
    }
    if (!mForcedNavigationBarColor) {
        a->GetColor(R::styleable::Window_navigationBarColor, 0xFF000000, &mNavigationBarColor);
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
            a->GetResourceId(R::styleable::Window_windowBackgroundFallback, 0, &mBackgroundFallbackResource);
//            if (FALSE) {
//                System.out.println("Background: "
//                        + Integer.toHexString(mBackgroundResource) + " Frame: "
//                        + Integer.toHexString(mFrameResource));
//            }
        }
        a->GetDimension(R::styleable::Window_windowElevation, 0, &mElevation);
        a->GetBoolean(R::styleable::Window_windowClipToOutline, FALSE, &mClipToOutline);
        a->GetColor(R::styleable::Window_textColor, IColor::TRANSPARENT, &mTextColor);
    }

    // Inflate the window decor.
    Int32 layoutResource = 0;
    Int32 features = GetLocalFeatures();
    // System.out.println("Features: 0x" + Integer.toHexString(features));
    if ((features & (1 << IWindow::FEATURE_SWIPE_TO_DISMISS)) != 0) {
        layoutResource = R::layout::screen_swipe_dismiss;
    } else if ((features & ((1 << IWindow::FEATURE_LEFT_ICON) | (1 << IWindow::FEATURE_RIGHT_ICON))) != 0) {
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
            a->GetResourceId(R::styleable::Window_windowActionBarFullscreenDecorLayout,
                    R::layout::screen_action_bar, &layoutResource);
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
    IViewGroup::Probe(decor)->AddView(in, vparams);
    mContentRoot = IViewGroup::Probe(in);

    AutoPtr<IView> tmp;
    FindViewById(ID_ANDROID_CONTENT, (IView**)&tmp);
    if (tmp == NULL) {
        // Logger::E("PhoneWindow", "Window couldn't find content container view");
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IViewGroup> contentParent = IViewGroup::Probe(tmp);
    if (contentParent == NULL) {
        // Logger::E("PhoneWindow", "Window couldn't find content container view");
        return E_RUNTIME_EXCEPTION;
    }

    if ((features & (1 << IWindow::FEATURE_INDETERMINATE_PROGRESS)) != 0) {
        AutoPtr<IProgressBar> progress = GetCircularProgressBar(FALSE);
        if (progress != NULL) {
            progress->SetIndeterminate(TRUE);
        }
    }

    if ((features & (1 << FEATURE_SWIPE_TO_DISMISS)) != 0) {
        RegisterSwipeCallbacks();
    }

    // Remaining setup -- of background and title -- that only applies
    // to top-level windows.
    container = NULL;
    GetContainer((IWindow**)&container);
    if (container == NULL) {
        AutoPtr<IDrawable> background;
        if (mBackgroundResource != 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetDrawable(mBackgroundResource, (IDrawable**)&background);
        } else {
            background = mBackgroundDrawable;
        }

        mDecor->SetWindowBackground(background);
        AutoPtr<IDrawable> frame;
        if (mFrameResource != 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetDrawable(mFrameResource, (IDrawable**)&frame);
        } else {
            frame = NULL;
        }
        mDecor->SetWindowFrame(frame);
        mDecor->SetElevation(mElevation);
        mDecor->SetClipToOutline(mClipToOutline);

        if (mTitle != NULL) {
            SetTitle(mTitle);
        }

        if (mTitleColor == 0) {
            mTitleColor = mTextColor;
        }

        SetTitleColor(mTitleColor);
    }

    mDecor->FinishChanging();

    *viewGroup = contentParent.Get();
    REFCOUNT_ADD(*viewGroup);

    return NOERROR;
}

ECode PhoneWindow::AlwaysReadCloseOnTouchAttr()
{
    mAlwaysReadCloseOnTouchAttr = TRUE;
    return NOERROR;
}

ECode PhoneWindow::DoPendingInvalidatePanelMenu()
{
    if (mInvalidatePanelMenuPosted)
    {
        Boolean res;
        mDecor->RemoveCallbacks(mInvalidatePanelMenuRunnable, &res);
        mInvalidatePanelMenuRunnable->Run();
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetTransitionManager(
    /* [out] */ ITransitionManager** tm)
{
    VALIDATE_NOT_NULL(tm);
    *tm = mTransitionManager;
    REFCOUNT_ADD(*tm);
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetTransitionManager(
    /* [in] */ ITransitionManager* tm)
{
    mTransitionManager = tm;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetContentScene(
    /* [out] */ IScene** scene)
{
    VALIDATE_NOT_NULL(scene);
    *scene = mContentScene;
    REFCOUNT_ADD(*scene);
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetMediaController(
    /* [in] */ IMediaController* controller)
{
    mMediaController = controller;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetMediaController(
    /* [out] */ IMediaController** controller)
{
    VALIDATE_NOT_NULL(controller);
    *controller = mMediaController;
    REFCOUNT_ADD(*controller);
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetEnterTransition(
    /* [in] */ ITransition* enterTransition)
{
    mEnterTransition = enterTransition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetReturnTransition(
    /* [in] */ ITransition* transition)
{
    mReturnTransition = transition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetExitTransition(
    /* [in] */ ITransition* exitTransition)
{
    mExitTransition = exitTransition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetReenterTransition(
    /* [in] */ ITransition* transition)
{
    mReenterTransition = transition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetSharedElementEnterTransition(
    /* [in] */ ITransition* sharedElementEnterTransition)
{
    mSharedElementEnterTransition = sharedElementEnterTransition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetSharedElementReturnTransition(
    /* [in] */ ITransition* transition)
{
    mSharedElementReturnTransition = transition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetSharedElementExitTransition(
    /* [in] */ ITransition* sharedElementExitTransition)
{
    mSharedElementExitTransition = sharedElementExitTransition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetSharedElementReenterTransition(
    /* [in] */ ITransition* transition)
{
    mSharedElementReenterTransition = transition;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = mEnterTransition;
    REFCOUNT_ADD(*transition);
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    if (mReturnTransition == USE_DEFAULT_TRANSITION) {
        return GetEnterTransition(transition);
    }
    else {
        *transition= mReturnTransition;
        REFCOUNT_ADD(*transition);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetExitTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = mExitTransition;
    REFCOUNT_ADD(*transition);
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetReenterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    if (mReenterTransition == USE_DEFAULT_TRANSITION) {
        return GetExitTransition(transition);
    }
    else {
        *transition = mReenterTransition;
        REFCOUNT_ADD(*transition);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetSharedElementEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = mSharedElementEnterTransition;
    REFCOUNT_ADD(*transition);
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetSharedElementReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    if (mSharedElementReturnTransition == USE_DEFAULT_TRANSITION)
    {
        return GetSharedElementEnterTransition(transition);
    }
    else
    {
        *transition = mSharedElementReturnTransition;
        REFCOUNT_ADD(*transition);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetSharedElementExitTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = mSharedElementExitTransition;
    REFCOUNT_ADD(*transition);
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetSharedElementReenterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    if (mSharedElementReenterTransition == USE_DEFAULT_TRANSITION)
    {
        return GetSharedElementExitTransition(transition);
    }
    else
    {
        *transition = mSharedElementReenterTransition;
        REFCOUNT_ADD(*transition);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetAllowEnterTransitionOverlap(
    /* [in] */ Boolean allow)
{
    mAllowEnterTransitionOverlap = allow;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetAllowEnterTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow);
    *allow = mAllowEnterTransitionOverlap;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetAllowReturnTransitionOverlap(
    /* [in] */ Boolean allowReturnTransitionOverlap)
{
    mAllowReturnTransitionOverlap = allowReturnTransitionOverlap;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetAllowReturnTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow);
    *allow = mAllowReturnTransitionOverlap;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetTransitionBackgroundFadeDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    if (mBackgroundFadeDurationMillis < 0)
        *duration = DEFAULT_BACKGROUND_FADE_DURATION_MS;
    else
        *duration = mBackgroundFadeDurationMillis;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetTransitionBackgroundFadeDuration(
    /* [in] */ Int64 fadeDurationMillis)
{
    if (fadeDurationMillis < 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    mBackgroundFadeDurationMillis = fadeDurationMillis;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetSharedElementsUseOverlay(
    /* [in] */ Boolean sharedElementsUseOverlay)
{
    mSharedElementsUseOverlay = sharedElementsUseOverlay;
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetSharedElementsUseOverlay(
    /* [out] */ Boolean* shared)
{
    VALIDATE_NOT_NULL(shared);
    *shared = mSharedElementsUseOverlay;
    return NOERROR;
}


// @Override
ECode PhoneWindow::GetStatusBarColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatusBarColor;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetStatusBarColor(
    /* [in] */ Int32 color)
{
    mStatusBarColor = color;
    mForcedStatusBarColor = TRUE;
    if (mDecor != NULL)
    {
        mDecor->UpdateColorViews(NULL);
    }
    return NOERROR;
}

// @Override
ECode PhoneWindow::GetNavigationBarColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNavigationBarColor;
    return NOERROR;
}

// @Override
ECode PhoneWindow::SetNavigationBarColor(
    /* [in] */ Int32 color)
{
    mNavigationBarColor = color;
    mForcedNavigationBarColor = TRUE;
    if (mDecor != NULL)
    {
        mDecor->UpdateColorViews(NULL);
    }
    return NOERROR;
}

void PhoneWindow::InstallDecor()
{
    if (mDecor == NULL) {
        AutoPtr<DecorView> dv = GenerateDecor();
        mDecor = dv;
        mDecor->AddRef();
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

        AutoPtr<IView> dcpView;
        mDecor->FindViewById(R::id::decor_content_parent, (IView**)&dcpView);
        IDecorContentParent* decorContentParent = IDecorContentParent::Probe(dcpView);

        if (decorContentParent != NULL) {
            mDecorContentParent = decorContentParent;
            AutoPtr<IWindowCallback> cb;
            GetCallback((IWindowCallback**)&cb);
            mDecorContentParent->SetWindowCallback(cb);
            AutoPtr<ICharSequence> title;
            mDecorContentParent->GetTitle((ICharSequence**)&title);
            if (title == NULL) {
                mDecorContentParent->SetWindowTitle(mTitle);
            }

            Int32 localFeatures = GetLocalFeatures();
            for (Int32 i = 0; i < FEATURE_MAX; ++i) {
                if ((localFeatures & (1 << i)) != 0) {
                    mDecorContentParent->InitFeature(i);
                }
            }

            mDecorContentParent->SetUiOptions(mUiOptions);

            Boolean tmpbValue;
            if ((mResourcesSetFlags & FLAG_RESOURCE_SET_ICON) != 0 ||
                    (mIconRes != 0 && !(mDecorContentParent->HasIcon(&tmpbValue), tmpbValue))) {
                mDecorContentParent->SetIcon(mIconRes);
            } else if ((mResourcesSetFlags & FLAG_RESOURCE_SET_ICON) == 0 &&
                    mIconRes == 0 && !(mDecorContentParent->HasIcon(&tmpbValue), tmpbValue)) {
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IPackageManager> pm;
                context->GetPackageManager((IPackageManager**)&pm);
                AutoPtr<IDrawable> icon;
                pm->GetDefaultActivityIcon((IDrawable**)&icon);
                mDecorContentParent->SetIcon(icon);
                mResourcesSetFlags |= FLAG_RESOURCE_SET_ICON_FALLBACK;
            }
            if ((mResourcesSetFlags & FLAG_RESOURCE_SET_LOGO) != 0 ||
                    (mLogoRes != 0 && !(mDecorContentParent->HasLogo(&tmpbValue), tmpbValue))) {
                mDecorContentParent->SetLogo(mLogoRes);
            }

            // Invalidate if the panel menu hasn't been created before this.
            // Panel menu invalidation is deferred avoiding application onCreateOptionsMenu
            // being called in the middle of onCreate or similar.
            // A pending invalidation will typically be resolved before the posted message
            // would run normally in order to satisfy instance state restoration.
            AutoPtr<PanelFeatureState> st;
            GetPanelState(FEATURE_OPTIONS_PANEL, FALSE, (PanelFeatureState**)&st);

            Boolean isDestroyed;
            IsDestroyed(&isDestroyed);
            if (!isDestroyed && (st == NULL || st->mMenu == NULL)) {
                InvalidatePanelMenu(FEATURE_ACTION_BAR);
            }
        } else {
            AutoPtr<IView> obj;
            FindViewById(R::id::title, (IView**)&obj);
            mTitleView = ITextView::Probe(obj);
            if (mTitleView != NULL) {
                Int32 direction = 0;
                mDecor->GetLayoutDirection(&direction);
                IView::Probe(mTitleView)->SetLayoutDirection(direction);
                if ((GetLocalFeatures() & (1 << IWindow::FEATURE_NO_TITLE)) != 0) {
                    AutoPtr<IView> titleContainer;
                    FindViewById(R::id::title_container,
                            (IView**)&titleContainer);
                    if (titleContainer != NULL) {
                        titleContainer->SetVisibility(IView::GONE);
                    }
                    else {
                        IView::Probe(mTitleView)->SetVisibility(IView::GONE);
                    }
                    if (IFrameLayout::Probe(mContentParent) != NULL) {
                        IFrameLayout::Probe(mContentParent)->SetForeground(NULL);
                    }
                }
                else {
                    mTitleView->SetText(mTitle);
                }
            }
        }

        AutoPtr<IDrawable> background;
        mDecor->GetBackground((IDrawable**)&background);
        if (background == NULL && mBackgroundFallbackResource != 0) {
            mDecor->SetBackgroundFallback(mBackgroundFallbackResource);
        }

        // Only inflate or create a new TransitionManager if the caller hasn't
        // already set a custom one.
        Boolean has;
        if (HasFeature(FEATURE_ACTIVITY_TRANSITIONS, &has), has) {
            if (mTransitionManager == NULL) {
                AutoPtr<ITypedArray> a;
                GetWindowStyle((ITypedArray**)&a);
                Int32 transitionRes;
                a->GetResourceId(R::styleable::Window_windowContentTransitionManager, 0, &transitionRes);
                if (transitionRes != 0) {
                    AutoPtr<ITransitionInflater> inflater;
                    AutoPtr<ITransitionInflaterHelper> tifHelper;
                    CTransitionInflaterHelper::AcquireSingleton((ITransitionInflaterHelper**)&tifHelper);
                    AutoPtr<IContext> context;
                    GetContext((IContext**)&context);
                    tifHelper->From(context, (ITransitionInflater**)&inflater);
                    inflater->InflateTransitionManager(transitionRes, mContentParent, (ITransitionManager**)&mTransitionManager);
                } else {
                    CTransitionManager::New((ITransitionManager**)&mTransitionManager);
                }
            }

            mEnterTransition = GetTransition(mEnterTransition, NULL, R::styleable::Window_windowEnterTransition);
            mReturnTransition = GetTransition(mReturnTransition, USE_DEFAULT_TRANSITION, R::styleable::Window_windowReturnTransition);
            mExitTransition = GetTransition(mExitTransition, NULL, R::styleable::Window_windowExitTransition);
            mReenterTransition = GetTransition(mReenterTransition, USE_DEFAULT_TRANSITION, R::styleable::Window_windowReenterTransition);
            mSharedElementEnterTransition = GetTransition(mSharedElementEnterTransition, NULL, R::styleable::Window_windowSharedElementEnterTransition);
            mSharedElementReturnTransition = GetTransition(mSharedElementReturnTransition, USE_DEFAULT_TRANSITION,
                    R::styleable::Window_windowSharedElementReturnTransition);
            mSharedElementExitTransition = GetTransition(mSharedElementExitTransition, NULL,
                    R::styleable::Window_windowSharedElementExitTransition);
            mSharedElementReenterTransition = GetTransition(mSharedElementReenterTransition,
                    USE_DEFAULT_TRANSITION,
                    R::styleable::Window_windowSharedElementReenterTransition);
            AutoPtr<ITypedArray> a;
            GetWindowStyle((ITypedArray**)&a);
            if (mAllowEnterTransitionOverlap == TRUE) {
                a->GetBoolean(R::styleable::Window_windowAllowEnterTransitionOverlap, TRUE, &mAllowEnterTransitionOverlap);
            }
            if (mAllowReturnTransitionOverlap == TRUE) {
                a->GetBoolean(R::styleable::Window_windowAllowReturnTransitionOverlap, TRUE, &mAllowReturnTransitionOverlap);
            }
            if (mBackgroundFadeDurationMillis < 0) {
                Int32 value;
                a->GetInteger(R::styleable::Window_windowTransitionBackgroundFadeDuration,
                        DEFAULT_BACKGROUND_FADE_DURATION_MS, &value);
                mBackgroundFadeDurationMillis = value;
            }
            if (mSharedElementsUseOverlay == TRUE) {
                a->GetBoolean(R::styleable::Window_windowSharedElementsUseOverlay, TRUE, &mSharedElementsUseOverlay);
            }
        }
    }
}

ECode PhoneWindow::SetChildDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
    st->mChild = drawable;
    UpdateDrawable(featureId, st, FALSE);
    return NOERROR;
}

ECode PhoneWindow::SetChildInt(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 value)
{
    UpdateInt(featureId, value, FALSE);
    return NOERROR;
}

ECode PhoneWindow::IsShortcutKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* isShortcutKey)
{
    VALIDATE_NOT_NULL(isShortcutKey);
    *isShortcutKey = FALSE;

    AutoPtr<PanelFeatureState> st;
    FAIL_RETURN(GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st));
    if (st->mMenu != NULL) {
        IMenu::Probe(st->mMenu)->IsShortcutKey(keyCode, event, isShortcutKey);
    }
    return NOERROR;
}

void PhoneWindow::UpdateDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ Boolean fromActive)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, FALSE, (DrawableFeatureState**)&st);
    if (st != NULL)
    {
        UpdateDrawable(featureId, st, fromActive);
    }
}

void PhoneWindow::UpdateDrawable(
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

void PhoneWindow::OnDrawableChanged(
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
        IView::Probe(view)->SetVisibility(IView::VISIBLE);
    } else {
        IView::Probe(view)->SetVisibility(IView::GONE);
    }
}

void PhoneWindow::OnIntChanged(
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
            mLayoutInflater->Inflate(value, IViewGroup::Probe(titleContainer), (IView**)&resultView);
        }
    }
}

void PhoneWindow::SendCloseSystemWindows()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    PhoneWindowManager::SendCloseSystemWindows(context, String(NULL));
}

void PhoneWindow::SendCloseSystemWindows(
    /* [in] */ const String& reason)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    PhoneWindowManager::SendCloseSystemWindows(context, reason);
}

void PhoneWindow::SetFeatureDefaultDrawable(
    /* [in] */ Int32 featureId,
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<DrawableFeatureState> st;
    GetDrawableState(featureId, TRUE, (DrawableFeatureState**)&st);
    if (st->mDef.Get() != drawable)
    {
        st->mDef = drawable;
        UpdateDrawable(featureId, st, FALSE);
    }
}

void PhoneWindow::SetFeatureFromAttrs(
    /* [in] */ Int32 featureId,
    /* [in] */ ITypedArray* attrs,
    /* [in] */ Int32 drawableAttr,
    /* [in] */ Int32 alphaAttr)
{
    Boolean val;
    AutoPtr<IDrawable> d;
    attrs->GetDrawable(drawableAttr, (IDrawable**)&d);
    if (d != NULL) {
        RequestFeature(featureId, &val);
        SetFeatureDefaultDrawable(featureId, d);
    }
    if ((GetFeatures() & (1 << featureId)) != 0) {
        Int32 alpha;
        attrs->GetInt32(alphaAttr, -1, &alpha);
        if (alpha >= 0) {
            SetFeatureDrawableAlpha(featureId, alpha);
        }
    }
}

void PhoneWindow::DispatchWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    Window::DispatchWindowAttributesChanged(attrs);
    if (mDecor != NULL) {
        mDecor->UpdateColorViews(NULL/* insets */);
    }
}

void PhoneWindow::UpdateProgressBars(
   /* [in] */ Int32 value)
{
    AutoPtr<IProgressBar> circularProgressBar = GetCircularProgressBar(TRUE);
    AutoPtr<IProgressBar> horizontalProgressBar = GetHorizontalProgressBar(TRUE);

    const Int32 features = GetLocalFeatures();
    if (value == PROGRESS_VISIBILITY_ON) {
        if ((features & (1 << FEATURE_PROGRESS)) != 0) {
            if (horizontalProgressBar != NULL)
            {
                Int32 level = 0;
                horizontalProgressBar->GetProgress(&level);
                Boolean isIndeterminate = FALSE;
                horizontalProgressBar->IsIndeterminate(&isIndeterminate);
                Int32 visibility = (isIndeterminate || level < 10000) ?
                    IView::VISIBLE : IView::INVISIBLE;
                IView::Probe(horizontalProgressBar)->SetVisibility(visibility);
            }
            else
            {
                Logger::E(TAG, "Horizontal progress bar not located in current window decor");
            }
        }
        if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0) {
            if (circularProgressBar != NULL)
            {
                IView::Probe(circularProgressBar)->SetVisibility(IView::VISIBLE);
            }
            else
            {
                Logger::E(TAG, "Circular progress bar not located in current window decor");
            }
        }
    } else if (value == PROGRESS_VISIBILITY_OFF) {
        if ((features & (1 << FEATURE_PROGRESS)) != 0) {
            if (horizontalProgressBar != NULL)
            {
                IView::Probe(horizontalProgressBar)->SetVisibility(IView::GONE);
            }
            else
            {
                Logger::E(TAG, "Horizontal progress bar not located in current window decor");
            }
        }
        if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0) {
            if (circularProgressBar != NULL)
            {
                IView::Probe(circularProgressBar)->SetVisibility(IView::GONE);
            }
            else
            {
                Logger::E(TAG, "Circular progress bar not located in current window decor");
            }
        }
    } else if (value == PROGRESS_INDETERMINATE_ON) {
        if (horizontalProgressBar != NULL)
        {
            horizontalProgressBar->SetIndeterminate(TRUE);
        }
        else
        {
            Logger::E(TAG, "Horizontal progress bar not located in current window decor");
        }
    } else if (value == PROGRESS_INDETERMINATE_OFF) {
        if (horizontalProgressBar != NULL)
        {
            horizontalProgressBar->SetIndeterminate(FALSE);
        }
        else
        {
            Logger::E(TAG, "Horizontal progress bar not located in current window decor");
        }
    } else if (PROGRESS_START <= value && value <= PROGRESS_END) {
        // We want to set the progress value before testing for visibility
        // so that when the progress bar becomes visible again, it has the
        // correct level.
        if (horizontalProgressBar != NULL)
        {
            horizontalProgressBar->SetProgress(value - PROGRESS_START);
        }
        else
        {
            Logger::E(TAG, "Horizontal progress bar not located in current window decor");
        }

        if (value < PROGRESS_END) {
            ShowProgressBars(horizontalProgressBar, circularProgressBar);
        } else {
            HideProgressBars(horizontalProgressBar, circularProgressBar);
        }
    } else if (PROGRESS_SECONDARY_START <= value && value <= PROGRESS_SECONDARY_END) {
        if (horizontalProgressBar != NULL)
        {
            horizontalProgressBar->SetSecondaryProgress(value - PROGRESS_SECONDARY_START);
        }
        else
        {
            Logger::E(TAG, "Horizontal progress bar not located in current window decor");
        }

        ShowProgressBars(horizontalProgressBar, circularProgressBar);
    }
}

void PhoneWindow::ShowProgressBars(
    /* [in] */ IProgressBar* horizontalProgressBar,
    /* [in] */ IProgressBar* spinnyProgressBar)
{
    const Int32 features = GetLocalFeatures();
    Int32 visibility = 0;
    if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0
        && spinnyProgressBar != NULL
        && (IView::Probe(spinnyProgressBar)->GetVisibility(&visibility), visibility) == IView::INVISIBLE) {
        IView::Probe(spinnyProgressBar)->SetVisibility(IView::VISIBLE);
    }
    // Only show the progress bars if the primary progress is not complete
    if (horizontalProgressBar != NULL) {
        Int32 progress = 0;
        horizontalProgressBar->GetProgress(&progress);
        if ((features & (1 << FEATURE_PROGRESS)) != 0 &&
                progress < 10000) {
            IView::Probe(horizontalProgressBar)->SetVisibility(IView::VISIBLE);
        }
    }
}

void PhoneWindow::HideProgressBars(
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
    if ((features & (1 << FEATURE_INDETERMINATE_PROGRESS)) != 0 && spinnyProgressBar != NULL &&
            (IView::Probe(spinnyProgressBar)->GetVisibility(&spBarVisibility), spBarVisibility) == IView::VISIBLE) {
        IView::Probe(spinnyProgressBar)->StartAnimation(anim);
        IView::Probe(spinnyProgressBar)->SetVisibility(IView::INVISIBLE);
    }
    if (horizontalProgressBar != NULL)
    {
        Int32 hpBarVisibale =0;
        if ((features & (1 << FEATURE_PROGRESS)) != 0 &&
                (IView::Probe(horizontalProgressBar)->GetVisibility(&hpBarVisibale), hpBarVisibale) == IView::VISIBLE) {
            IView::Probe(horizontalProgressBar)->StartAnimation(anim);
            IView::Probe(horizontalProgressBar)->SetVisibility(IView::INVISIBLE);
        }
    }
}

void PhoneWindow::UpdateInt(
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

AutoPtr<IImageView> PhoneWindow::GetLeftIconView()
{
    if (mLeftIconView != NULL) {
        return mLeftIconView;
    }
    if (mContentParent == NULL) {
        InstallDecor();
    }

    AutoPtr<IView> view;
    FindViewById(R::id::left_icon, (IView**)&view);
    mLeftIconView = IImageView::Probe(view);
    return mLeftIconView;
}

AutoPtr<IProgressBar> PhoneWindow::GetCircularProgressBar(
    /* [in] */ Boolean shouldInstallDecor)
{
    if (mCircularProgressBar != NULL) {
        return mCircularProgressBar;
    }
    if (mContentParent == NULL && shouldInstallDecor) {
        InstallDecor();
    }

    AutoPtr<IView> view;
    FindViewById(R::id::progress_circular, (IView**)&view);
    mCircularProgressBar = IProgressBar::Probe(view);
    if (mCircularProgressBar != NULL) {
        IView::Probe(mCircularProgressBar)->SetVisibility(IView::INVISIBLE);
    }
    return mCircularProgressBar;
}

AutoPtr<IProgressBar> PhoneWindow::GetHorizontalProgressBar(
    /* [in] */ Boolean shouldInstallDecor)
{
    if (mHorizontalProgressBar != NULL) {
        return mHorizontalProgressBar;
    }
    if (mContentParent == NULL && shouldInstallDecor) {
        InstallDecor();
    }

    AutoPtr<IView> view;
    FindViewById(R::id::progress_horizontal, (IView**)&view);
    mHorizontalProgressBar = IProgressBar::Probe(view);
    if (mHorizontalProgressBar != NULL) {
        IView::Probe(mHorizontalProgressBar)->SetVisibility(IView::INVISIBLE);
    }
    return mHorizontalProgressBar;
}

AutoPtr<IImageView> PhoneWindow::GetRightIconView()
{
    if (mRightIconView != NULL) {
        return mRightIconView;
    }
    if (mContentParent == NULL) {
        InstallDecor();
    }

    AutoPtr<IView> view;
    FindViewById(R::id::right_icon, (IView**)&view);
    mRightIconView = IImageView::Probe(view);
    return mRightIconView;
}

//@Override
ECode PhoneWindow::SetVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    mVolumeControlStreamType = streamType;
    return NOERROR;
}

//@Override
ECode PhoneWindow::GetVolumeControlStream(
    /* [out] */ Int32* streamType)
{
    VALIDATE_NOT_NULL(streamType);
    *streamType = mVolumeControlStreamType;
    return NOERROR;
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
ECode PhoneWindow::PreparePanel(
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

    Boolean isActionBarMenu = st->mFeatureId == FEATURE_OPTIONS_PANEL || st->mFeatureId == FEATURE_ACTION_BAR;
    if (isActionBarMenu && mDecorContentParent != NULL)
    {
        // Enforce ordering guarantees around events so that the action bar never
        // dispatches menu-related events before the panel is prepared.
        mDecorContentParent->SetMenuPrepared();
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

            if (isActionBarMenu && mDecorContentParent != NULL) {
                if (mActionMenuPresenterCallback == NULL) {
                    mActionMenuPresenterCallback = new ActionMenuPresenterCallback(this);
                }

                mDecorContentParent->SetMenu(IMenu::Probe(st->mMenu), mActionMenuPresenterCallback);
            }

            // Call callback, and return if it doesn't want to display menu.

            // Creating the panel menu will involve a lot of manipulation;
            // don't dispatch change events to presenters until we're done.
            st->mMenu->StopDispatchingItemsChanged();
            Boolean result = FALSE;
            if ((cb == NULL) || !(cb->OnCreatePanelMenu(st->mFeatureId, IMenu::Probe(st->mMenu), &result), result)) {
                // Ditch the menu created above
                st->SetMenu(NULL);

                if (isActionBarMenu && mDecorContentParent != NULL) {
                    // Don't show it in the action bar either
                    mDecorContentParent->SetMenu(NULL, mActionMenuPresenterCallback);
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
        if (!(cb->OnPreparePanel(st->mFeatureId, st->mCreatedPanelView, IMenu::Probe(st->mMenu), &tmp), tmp)) {
            if (isActionBarMenu && mDecorContentParent != NULL) {
                // The app didn't want to show the menu for now but it still exists.
                // Clear it out of the action bar.
                mDecorContentParent->SetMenu(NULL, mActionMenuPresenterCallback);
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
        kcHelper->Load(event != NULL ? (IInputEvent::Probe(event)->GetDeviceId(&devId), devId) : IKeyCharacterMap::VIRTUAL_KEYBOARD,
                (IKeyCharacterMap**)&kmap);

        Int32 boardType = 0;
        st->mQwertyMode = (kmap->GetKeyboardType(&boardType), boardType) != IKeyCharacterMap::NUMERIC;
        IMenu::Probe(st->mMenu)->SetQwertyMode(st->mQwertyMode);
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
Boolean PhoneWindow::InitializePanelMenu(
    /* [in] */ PanelFeatureState* st)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    // If we have an action bar, initialize the menu with a context themed for it.
    if ((st->mFeatureId == FEATURE_OPTIONS_PANEL || st->mFeatureId == FEATURE_ACTION_BAR) && mDecorContentParent != NULL)
    {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);

        AutoPtr<IResourcesTheme> baseTheme;
        context->GetTheme((IResourcesTheme**)&baseTheme);

        Boolean tmp = FALSE;
        baseTheme->ResolveAttribute(R::attr::actionBarTheme, outValue, TRUE, &tmp);

        AutoPtr<IResourcesTheme> widgetTheme;
        Int32 targetThemeRes = 0;
        outValue->GetResourceId(&targetThemeRes);
        if (targetThemeRes != 0)
        {
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            resources->NewTheme((IResourcesTheme**)&widgetTheme);
            widgetTheme->SetTo(baseTheme);
            widgetTheme->ApplyStyle(targetThemeRes, TRUE);
            widgetTheme->ResolveAttribute(R::attr::actionBarWidgetTheme, outValue, TRUE, &tmp);
        }
        else
        {
            baseTheme->ResolveAttribute(R::attr::actionBarWidgetTheme, outValue, TRUE, &tmp);
        }

        if (targetThemeRes != 0)
        {
            if (widgetTheme == NULL)
            {
                AutoPtr<IResources> resources;
                context->GetResources((IResources**)&resources);
                resources->NewTheme((IResourcesTheme**)&widgetTheme);
                widgetTheme->SetTo(baseTheme);
            }
            widgetTheme->ApplyStyle(targetThemeRes, TRUE);
        }

        if (widgetTheme != NULL)
        {
            AutoPtr<IContext> temp;
            CContextThemeWrapper::New(context, 0, (IContext**)&temp);
            context = temp;
            AutoPtr<IResourcesTheme> ctxTheme;
            context->GetTheme((IResourcesTheme**)&ctxTheme);
            ctxTheme->SetTo(widgetTheme);
        }
    }

    AutoPtr<IMenuBuilder> menu;
    CMenuBuilder::New(context, (IMenuBuilder**)&menu);
    menu->SetCallback(this/*new MyMenuBuilderCallback(this)*/);
    st->SetMenu(menu);

    return TRUE;
}

AutoPtr<IDrawable> PhoneWindow::LoadImageURI(
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

ECode PhoneWindow::GetDrawableState(
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
ECode PhoneWindow::GetPanelState(
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
ECode PhoneWindow::GetPanelState(
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
        st = (convertPanelState != NULL) ? convertPanelState : new PanelFeatureState(this, featureId);
        ar->Set(featureId, st);
    }

    *state = st;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

AutoPtr<PhoneWindow::PanelFeatureState> PhoneWindow::FindMenuPanel(
    /* [in] */ IMenu* menu)
{
    ArrayOf<PanelFeatureState*>* panels = mPanels;
    const Int32 N = panels != NULL ? panels->GetLength() : 0;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<PanelFeatureState> panel = (*panels)[i];
        if (panel != NULL && IMenu::Probe((panel->mMenu)) == menu) {
            return panel;
        }
    }
    return NULL;
}

ECode PhoneWindow::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    ReopenMenu(TRUE);
    return NOERROR;
}

ECode PhoneWindow::OnMenuItemSelected(
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

        AutoPtr<PanelFeatureState> panel = FindMenuPanel(IMenu::Probe(rootMenu));
        if (panel != NULL) {
            return cb->OnMenuItemSelected(panel->mFeatureId, item, state);
        }
    }
    *state = FALSE;
    return NOERROR;
}

void PhoneWindow::ReopenMenu(
    /* [in] */ Boolean toggleMenuMode)
{
    Boolean tmp = FALSE;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> viewConfig;
    vcHelper->Get(context, (IViewConfiguration**)&viewConfig);
    Boolean hasPerMk;
    viewConfig->HasPermanentMenuKey(&hasPerMk);
    if (mDecorContentParent != NULL
        && (mDecorContentParent->CanShowOverflowMenu(&tmp), tmp)
        && (!hasPerMk|| (mDecorContentParent->IsOverflowMenuShowing(&tmp), tmp)))
    {
        AutoPtr<IWindowCallback> cb;
        GetCallback((IWindowCallback**)&cb);

        Boolean destoryed = FALSE;
        IsDestroyed(&destoryed);
        if (!(mDecorContentParent->IsOverflowMenuShowing(&tmp), tmp) || !toggleMenuMode) {
            if (cb != NULL && !destoryed ) {
                // If we have a menu invalidation pending, do it now.
                if (mInvalidatePanelMenuPosted &&
                    (mInvalidatePanelMenuFeatures & (1 << FEATURE_OPTIONS_PANEL)) != 0)
                {
                    Boolean res;
                    mDecor->RemoveCallbacks(mInvalidatePanelMenuRunnable, &res);
                    mInvalidatePanelMenuRunnable->Run();
                }

                AutoPtr<PanelFeatureState> st;
                GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);

                // If we don't have a menu or we're waiting for a full content refresh,
                // forget it. This is a lingering event that no longer matters.
                if (st != NULL && st->mMenu != NULL && !st->mRefreshMenuContent &&
                        (cb->OnPreparePanel(FEATURE_OPTIONS_PANEL, st->mCreatedPanelView, IMenu::Probe(st->mMenu), &tmp), tmp)) {
                    Boolean tmp = FALSE;
                    cb->OnMenuOpened(FEATURE_ACTION_BAR, IMenu::Probe(st->mMenu), &tmp);
                    mDecorContentParent->ShowOverflowMenu(&tmp);
                }
            }
        }
        else {
            Boolean tmp = FALSE;
            mDecorContentParent->HideOverflowMenu(&tmp);
            if (cb != NULL && !destoryed) {
                AutoPtr<PanelFeatureState> st;
                GetPanelState(FEATURE_OPTIONS_PANEL, TRUE, (PanelFeatureState**)&st);
                if (st != NULL) {
                    cb->OnPanelClosed(FEATURE_ACTION_BAR, IMenu::Probe(st->mMenu));
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
void PhoneWindow::OpenPanelsAfterRestore()
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
} // namespace Droid
} // namespace Elastos
