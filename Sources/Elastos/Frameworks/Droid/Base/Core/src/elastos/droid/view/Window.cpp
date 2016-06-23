
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Window.h"
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

const String Window::PROPERTY_HARDWARE_UI("persist.sys.ui.hw");
const Int32 Window::DEFAULT_FEATURES = (1 << IWindow::FEATURE_OPTIONS_PANEL) | (1 << IWindow::FEATURE_CONTEXT_MENU);

CAR_INTERFACE_IMPL(Window, Object, IWindow);
Window::Window()
    : mForcedWindowFlags(0)
    , mFeatures(DEFAULT_FEATURES)
    , mLocalFeatures(DEFAULT_FEATURES)
    , mDefaultWindowFormat(IPixelFormat::OPAQUE)
    , mCallback(NULL)
    , mContext(NULL)
    , mRetainContext(FALSE)
    , mHaveWindowFormat(FALSE)
    , mHaveDimAmount(FALSE)
    , mHasSoftInputMode(FALSE)
    , mDestroyed(FALSE)
    , mHardwareAccelerated(FALSE)
    , mIsActive(FALSE)
    , mHasChildren(FALSE)
    , mCloseOnTouchOutside(FALSE)
    , mSetCloseOnTouchOutside(FALSE)
{
    CWindowManagerLayoutParams::NewByFriend((CWindowManagerLayoutParams**)&mWindowAttributes);
}

Window::Window(
    /* [in] */ IContext* context)
    : mForcedWindowFlags(0)
    , mFeatures(DEFAULT_FEATURES)
    , mLocalFeatures(DEFAULT_FEATURES)
    , mDefaultWindowFormat(IPixelFormat::OPAQUE)
    , mCallback(NULL)
    , mContext(NULL)
    , mRetainContext(FALSE)
    , mHaveWindowFormat(FALSE)
    , mHaveDimAmount(FALSE)
    , mHasSoftInputMode(FALSE)
    , mDestroyed(FALSE)
    , mHardwareAccelerated(FALSE)
    , mIsActive(FALSE)
    , mHasChildren(FALSE)
    , mCloseOnTouchOutside(FALSE)
    , mSetCloseOnTouchOutside(FALSE)
{
    mRetainContext = IActivity::Probe(mContext) == NULL && IService::Probe(mContext) == NULL;
    if (mRetainContext)
        mContext->AddRef();

    CWindowManagerLayoutParams::NewByFriend((CWindowManagerLayoutParams**)&mWindowAttributes);
}

Window::~Window()
{
    if (mRetainContext)
        mContext->Release();
}
/**
 * Return the Context this window policy is running in, for retrieving
 * resources and other information.
 *
 * @return Context The Context that was supplied to the constructor.
 */
ECode Window::GetContext(
    /* [out] */ IContext** context)
{
    assert(context);
    *context = mContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode Window::GetWindowStyle(
    /* [out] */ ITypedArray** attrs)
{
    {    AutoLock syncLock(this);
        if (mWindowStyle == NULL) {
            AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Window);
            FAIL_RETURN(mContext->ObtainStyledAttributes(attrIds, (ITypedArray**)&mWindowStyle));
        }
        *attrs = mWindowStyle;
        REFCOUNT_ADD(*attrs);
    }

    return NOERROR;
}

ECode Window::SetContainer(
    /* [in] */ IWindow* container)
{
    mContainer = (Window*)container;
    if (mContainer != NULL) {
        // Embedded screens never have a title.
        mFeatures |= 1 << FEATURE_NO_TITLE;
        mLocalFeatures |= 1 << FEATURE_NO_TITLE;
        mContainer->mHasChildren = TRUE;
    }
    return NOERROR;
}

ECode Window::GetContainer(
    /* [out] */ IWindow** container)
{
    assert(container);
    *container = NULL;

    if (mContainer != NULL) {
        *container = mContainer;
        REFCOUNT_ADD(*container);
    }
    return NOERROR;
}

ECode Window::HasChildren(
    /* [out] */  Boolean* hasChildren)
{
    assert(hasChildren);
    *hasChildren = mHasChildren;
    return NOERROR;
}

/** @hide */
ECode Window::Destroy()
{
    mDestroyed = TRUE;
    return NOERROR;
}

/** @hide */
ECode Window::IsDestroyed(
    /* [out] */ Boolean* isDestroyed)
{
    assert(isDestroyed);
    *isDestroyed = mDestroyed;
    return NOERROR;
}

ECode Window::SetWindowManager(
    /* [in] */ IWindowManager* wm,
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& appName)
{
    return SetWindowManager(wm, appToken, appName, FALSE);
}

ECode Window::SetWindowManager(
    /* [in] */ IWindowManager* wm,
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& appName,
    /* [in] */ Boolean hardwareAccelerated)
{
    mAppToken = appToken;
    mAppName = appName;
    Boolean ph = FALSE;
    SystemProperties::GetBoolean(PROPERTY_HARDWARE_UI, FALSE, &ph);
    mHardwareAccelerated = hardwareAccelerated || ph;
    if (wm == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        wm = IWindowManager::Probe(service);
    }
    else {
        wm->AddRef();
    }
    mWindowManager = ((WindowManagerImpl*)wm)->CreateLocalWindowManager(this);
    wm->Release();
    return NOERROR;
}

ECode Window::AdjustLayoutParamsForSubWindow(
    /* [in] */ IWindowManagerLayoutParams* wparams)
{
    CWindowManagerLayoutParams* wp = (CWindowManagerLayoutParams*)wparams;
    AutoPtr<ICharSequence> curTitle;
    wp->GetTitle((ICharSequence**)&curTitle);
    if (wp->mType >= IWindowManagerLayoutParams::FIRST_SUB_WINDOW &&
        wp->mType <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
        if (wp->mToken == NULL) {
            AutoPtr<IView> decor;
            PeekDecorView((IView**)&decor);
            if (decor != NULL) {
                decor->GetWindowToken((IBinder**)&wp->mToken);
            }
        }
        Int32 length;
        if (curTitle == NULL || (curTitle->GetLength(&length), length) == 0) {
            String title;
            if (wp->mType == IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA) {
                title = "Media";
            }
            else if (wp->mType == IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA_OVERLAY) {
                title = "MediaOvr";
            }
            else if (wp->mType == IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL) {
                title = "Panel";
            }
            else if (wp->mType == IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL) {
                title = "SubPanel";
            }
            else if (wp->mType == IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG) {
                title = "AtchDlg";
            }
            else {
                title = StringUtils::ToString(wp->mType);
            }
            if (mAppName != NULL) {
                title += String(":") + mAppName;
            }
            AutoPtr<ICharSequence> tl;
            CString::New(title, (ICharSequence**)&tl);
            wp->SetTitle(tl);
        }
    }
    else {
        if (wp->mToken == NULL) {
            wp->mToken = mContainer == NULL ? mAppToken : mContainer->mAppToken;
        }
        Int32 length;
        if ((curTitle == NULL || (curTitle->GetLength(&length), length) == 0)
            && !mAppName.IsNull()) {
            AutoPtr<ICharSequence> tl;
            CString::New(mAppName, (ICharSequence**)&tl);
            wp->SetTitle(tl);
        }
    }
    if (wp->mPackageName.IsNull()) {
        mContext->GetPackageName(&wp->mPackageName);
    }
    if (mHardwareAccelerated) {
        wp->mFlags |= IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
    }

    return NOERROR;
}

ECode Window::GetWindowManager(
    /* [out] */ IWindowManager** wm)
{
    *wm = mWindowManager.Get();
    REFCOUNT_ADD(*wm);
    return NOERROR;
}

/**
 * Set the Callback interface for this window, used to intercept key
 * events and other dynamic operations in the window.
 *
 * @param callback The desired Callback interface.
 */
ECode Window::SetCallback(
    /* [in] */ IWindowCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

/**
 * Return the current Callback interface for this window.
 */
ECode Window::GetCallback(
    /* [out] */ IWindowCallback** cb)
{
    *cb = mCallback;
    REFCOUNT_ADD(*cb);
    return NOERROR;
}

ECode Window::SetOnWindowDismissedCallback(
    /* [in] */ IOnWindowDismissedCallback* dcb)
{
    mOnWindowDismissedCallback = dcb;
    return NOERROR;
}

ECode Window::DispatchOnWindowDismissed()
{
    if (mOnWindowDismissedCallback != NULL) {
        mOnWindowDismissedCallback->OnWindowDismissed();
    }
    return NOERROR;
}

ECode Window::SetLayout(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mWindowAttributes->mWidth = width;
    mWindowAttributes->mHeight = height;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::SetGravity(
    /* [in] */ Int32 gravity)
{
    mWindowAttributes->mGravity = gravity;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::SetType(
    /* [in] */ Int32 type)
{
    mWindowAttributes->mType = type;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::SetFormat(
    /* [in] */ Int32 format)
{
    if (format != IPixelFormat::UNKNOWN) {
        mWindowAttributes->mFormat = format;
        mHaveWindowFormat = TRUE;
    }
    else {
        mWindowAttributes->mFormat = mDefaultWindowFormat;
        mHaveWindowFormat = FALSE;
    }
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::SetWindowAnimations(
    /* [in] */ Int32 resId)
{
    mWindowAttributes->mWindowAnimations = resId;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    if (mode != IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNSPECIFIED) {
        mWindowAttributes->mSoftInputMode = mode;
        mHasSoftInputMode = TRUE;
    }
    else {
        mHasSoftInputMode = FALSE;
    }
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

ECode Window::AddFlags(
    /* [in] */ Int32 flags)
{
    return SetFlags(flags, flags);
}

ECode Window::AddPrivateFlags(
    /* [in] */ Int32 flags)
{
    SetPrivateFlags(flags, flags);
    return NOERROR;
}

/** @hide */
ECode Window::SetBlurMaskAlphaThreshold(
    /* [in] */ Float alpha)
{
    AutoPtr<IWindowManagerLayoutParams> attrs;
    GetAttributes((IWindowManagerLayoutParams**)&attrs);
    attrs->SetBlurMaskAlphaThreshold(alpha);
    DispatchWindowAttributesChanged(attrs);
    return NOERROR;
}

ECode Window::ClearFlags(
    /* [in] */ Int32 flags)
{
    return SetFlags(0, flags);
}

/** @hide */
ECode Window::ClearPrivateFlags(
    /* [in] */ Int32 flags)
{
    SetPrivateFlags(0, flags);
}

ECode Window::SetFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    mWindowAttributes->mFlags = (mWindowAttributes->mFlags & ~mask) | (flags & mask);
    if ((mask & IWindowManagerLayoutParams::FLAG_NEEDS_MENU_KEY) != 0) {
        mWindowAttributes->mPrivateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SET_NEEDS_MENU_KEY;
    }
    mForcedWindowFlags |= mask;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

void Window::SetPrivateFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    if ((flags & mask & IWindowManagerLayoutParams::PRIVATE_FLAG_PREVENT_POWER_KEY) != 0){
        mContext->EnforceCallingOrSelfPermission(String("android.permission.PREVENT_POWER_KEY"),
                String("No permission to prevent power key"));
    }
    mWindowAttributes->mPrivateFlags = (mWindowAttributes->mPrivateFlags & ~mask) | (flags & mask);
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
}

void Window::DispatchWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    if (mCallback != NULL) {
        mCallback->OnWindowAttributesChanged(attrs);
    }
}

ECode Window::SetDimAmount(
    /* [in] */ Float amount)
{
    mWindowAttributes->mDimAmount = amount;
    mHaveDimAmount = TRUE;
    DispatchWindowAttributesChanged(mWindowAttributes.Get());

    return NOERROR;
}

ECode Window::SetAttributes(
    /* [in] */ IWindowManagerLayoutParams* a)
{
    Int32 changes;
    mWindowAttributes->CopyFrom(a, &changes);
    DispatchWindowAttributesChanged(mWindowAttributes.Get());
    return NOERROR;
}

/**
 * Retrieve the current window attributes associated with this panel.
 *
 * @return WindowManager.LayoutParams Either the existing window
 *         attributes object, or a freshly created one if there is none.
 */
ECode Window::GetAttributes(
    /* [out] */ IWindowManagerLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)
    *params = (IWindowManagerLayoutParams*)mWindowAttributes.Get();
    REFCOUNT_ADD(*params);
    return NOERROR;
}

Int32 Window::GetForcedWindowFlags()
{
    return mForcedWindowFlags;
}

Boolean Window::HasSoftInputMode()
{
    return mHasSoftInputMode;
}

/** @hide */
ECode Window::SetCloseOnTouchOutside(
    /* [in] */ Boolean close)
{
    mCloseOnTouchOutside = close;
    mSetCloseOnTouchOutside = TRUE;
    return NOERROR;
}

ECode Window::SetCloseOnTouchOutsideIfNotSet(
    /* [in] */ Boolean close)
{
    if (!mSetCloseOnTouchOutside) {
        mCloseOnTouchOutside = close;
        mSetCloseOnTouchOutside = TRUE;
    }
    return NOERROR;
}

ECode Window::ShouldCloseOnTouch(
    /* [in] */ IContext* context,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    assert(res);
    Int32 action;
    event->GetAction(&action);
    AutoPtr<IView> view;
    PeekDecorView((IView**)&view);
    if (mCloseOnTouchOutside && action == IMotionEvent::ACTION_DOWN
        && IsOutOfBounds(context, event) && view != NULL) {
        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

Boolean Window::IsOutOfBounds(
    /* [in] */ IContext* context,
    /* [in] */ IMotionEvent* event)
{
    Float fX, fY;
    event->GetX(&fX);
    event->GetY(&fY);
    Int32 x = (Int32)fX;
    Int32 y = (Int32)fY;
    Int32 slop;
    CViewConfiguration::Get(context)->GetScaledWindowTouchSlop(&slop);
    AutoPtr<IView> decorView;
    GetDecorView((IView**)&decorView);
    Int32 width, height;
    decorView->GetWidth(&width);
    decorView->GetHeight(&height);
    return (x < -slop) || (y < -slop) || (x > (width + slop))
            || (y > (height + slop));
}

ECode Window::RequestFeature(
    /* [in] */ Int32 featureId,
    /* [out] */ Boolean* result)
{
    Int32 flag = 1<<featureId;
    mFeatures |= flag;
    mLocalFeatures |= mContainer != NULL ? (flag & ~mContainer->mFeatures) : flag;
    *result = (mFeatures & flag) != 0;
    return NOERROR;
}

void Window::RemoveFeature(
    /* [in] */ Int32 featureId)
{
    Int32 flag = 1<<featureId;
    mFeatures &= ~flag;
    mLocalFeatures &= ~(mContainer != NULL ? (flag&~mContainer->mFeatures) : flag);
}

ECode Window::MakeActive()
{
    if (mContainer != NULL) {
        if (mContainer->mActiveChild != NULL) {
            mContainer->mActiveChild->mIsActive = FALSE;
        }
        mContainer->mActiveChild = this;
    }
    mIsActive = TRUE;
    OnActive();
    return NOERROR;
}

ECode Window::IsActive(
    /* [out] */ Boolean* isActive)
{
    *isActive = mIsActive;
    return NOERROR;
}

/**
 * Finds a view that was identified by the id attribute from the XML that
 * was processed in {@link android.app.Activity#onCreate}.  This will
 * implicitly call {@link #getDecorView} for you, with all of the
 * associated side-effects.
 *
 * @return The view if found or null otherwise.
 */
ECode Window::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** view)
{
    AutoPtr<IView> decor;
    GetDecorView((IView**)&decor);
    return decor->FindViewById(id, view);
}

ECode Window::SetBackgroundDrawableResource(
    /* [in] */ Int32 resid)
{
    AutoPtr<IDrawable> drawable;
    FAIL_RETURN(mContext->GetDrawable(resid, (IDrawable**)&drawable));
    return SetBackgroundDrawable(drawable);
}

Int32 Window::GetFeatures()
{
    return mFeatures;
}

ECode Window::HasFeature(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* hasFeature)
{
    assert(hasFeature);
    *hasFeature = (mFeatures & (1 << feature)) != 0;
    return NOERROR;
}

Int32 Window::GetLocalFeatures()
{
    return mLocalFeatures;
}

void Window::SetDefaultWindowFormat(
    /* [in] */ Int32 format)
{
    mDefaultWindowFormat = format;
    if (!mHaveWindowFormat) {
        mWindowAttributes->mFormat = format;
        DispatchWindowAttributesChanged(mWindowAttributes.Get());
    }
}

Boolean Window::HaveDimAmount()
{
    return mHaveDimAmount;
}

ECode Window::SetMediaController(
    /* [in] */ IMediaController* controller)
{
    return NOERROR;
}

ECode Window::GetMediaController(
    /* [out] */ IMediaController** controller)
{
    VALIDATE_NOT_NULL(controller);
    *controller = NULL;
    return NOERROR;
}

ECode Window::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    return NOERROR;
}

ECode Window::SetUiOptions(
    /* [in] */ Int32 uiOptions,
    /* [in] */ Int32 mask)
{
    return NOERROR;
}

ECode Window::SetIcon(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode Window::SetDefaultIcon(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode Window::SetLogo(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode Window::SetDefaultLogo(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode Window::SetLocalFocus(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean inTouchMode)
{
    return NOERROR;
}

ECode Window::InjectInputEvent(
    /* [in] */ IInputEvent* event)
{
    return NOERROR;
}

ECode Window::GetTransitionManager(
    /* [out] */ ITransitionManager** tm)
{
    VALIDATE_NOT_NULL(tm);
    *tm = NULL;
    return NOERROR;
}

ECode Window::SetTransitionManager(
    /* [in] */ ITransitionManager* tm)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Window::GetContentScene(
    /* [out] */ IScene** scene)
{
    VALIDATE_NOT_NULL(scene);
    *scene = NULL;
    return NOERROR;
}

ECode Window::SetEnterTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::SetReturnTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::SetExitTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::SetReenterTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::GetEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::GetReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::GetExitTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::GetReenterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::SetSharedElementEnterTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::SetSharedElementReturnTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::GetSharedElementEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::GetSharedElementReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::SetSharedElementExitTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::SetSharedElementReenterTransition(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Window::GetSharedElementExitTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::GetSharedElementReenterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition);
    *transition = NULL;
    return NOERROR;
}

ECode Window::SetAllowEnterTransitionOverlap(
    /* [in] */ Boolean allow)
{
    return NOERROR;
}

ECode Window::GetAllowEnterTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow);
    *allow = TRUE;
    return NOERROR;
}

ECode Window::SetAllowReturnTransitionOverlap(
    /* [in] */ Boolean allow)
{
    return NOERROR;
}

ECode Window::SetAllowExitTransitionOverlap(
    /* [in] */ Boolean allow)
{
    return SetAllowReturnTransitionOverlap(allow);
}

ECode Window::GetAllowReturnTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow);
    *allow = TRUE;
    return NOERROR;
}

ECode Window::GetAllowExitTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow);
    return GetAllowReturnTransitionOverlap(allow);
}

ECode Window::GetTransitionBackgroundFadeDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = 0;
    return NOERROR;
}

ECode Window::SetTransitionBackgroundFadeDuration(
    /* [in] */ Int64 fadeDurationMillis)
{
    return NOERROR;
}

ECode Window::GetSharedElementsUseOverlay(
    /* [out] */ Boolean* shared)
{
    VALIDATE_NOT_NULL(shared);
    *shared = TRUE;
    return NOERROR;
}

ECode Window::SetSharedElementsUseOverlay(
    /* [in] */ Boolean sharedElementsUseOverlay)
{
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
