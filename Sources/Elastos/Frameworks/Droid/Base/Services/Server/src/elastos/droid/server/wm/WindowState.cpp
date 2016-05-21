
#include "elastos/droid/server/wm/WindowState.h"
#include "elastos/droid/server/wm/CWindowId.h"
#include "elastos/droid/server/wm/AccessibilityController.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::IInternalInsetsInfo;
using Elastos::Droid::View::EIID_IWindowState;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  WindowState::DeathRecipient
//==============================================================================

CAR_INTERFACE_IMPL(WindowState::DeathRecipient, Object, IProxyDeathRecipient);

WindowState::DeathRecipient::DeathRecipient(
    /* [in] */ WindowState* owner)
{
    owner->GetWeakReference((IWeakReference**)&mWeakOwner);
}

ECode WindowState::DeathRecipient::ProxyDied()
{
    // try {
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    AutoPtr<WindowState> owner = (WindowState*)iowner.Get();
    Slogger::I(TAG, "WindowState:: %p, DeathRecipient::ProxyDied(): ", owner.Get());

    AutoLock lock(owner->mService->mWindowMapLock);
    AutoPtr<WindowState> win;
    owner->mService->WindowForClientLocked(owner->mSession, owner->mClient, FALSE, (WindowState**)&win);
    Slogger::I(TAG, "WIN DEATH: %p", win.Get());
    if (win != NULL) {
        owner->mService->RemoveWindowLocked(owner->mSession, win);
    }
    else if (owner->mHasSurface) {
        Slogger::E(TAG, "!!! LEAK !!! Window removed but surface still valid.");
        owner->mService->RemoveWindowLocked(owner->mSession, owner);
    }
    // } catch (IllegalArgumentException ex) {
    //     // This will happen if the window has already been
    //     // removed.
    // }
    return NOERROR;
}


//==============================================================================
//                  WindowState::ResizeRunnable
//==============================================================================

WindowState::ResizeRunnable::ResizeRunnable(
    /* [in] */ WindowState* host,
    /* [in] */ IRect* frame,
    /* [in] */ IRect* overscanInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
    : mHost(host)
    , mFrame(frame)
    , mOverscanInsets(overscanInsets)
    , mContentInsets(contentInsets)
    , mVisibleInsets(visibleInsets)
    , mStableInsets(stableInsets)
    , mReportDraw(reportDraw)
    , mNewConfig(newConfig)
{}

ECode WindowState::ResizeRunnable::Run()
{
    return mHost->mClient->Resized(mFrame, mOverscanInsets, mContentInsets,
            mVisibleInsets, mStableInsets,  mReportDraw, mNewConfig);
}


//==============================================================================
//                  WindowState
//==============================================================================

const String WindowState::TAG("WindowState");

CAR_INTERFACE_IMPL(WindowState, Object, IWindowState)

WindowState::WindowState(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ CSession* s,
    /* [in] */ IIWindow* c,
    /* [in] */ WindowToken* token,
    /* [in] */ WindowState* attachedWindow,
    /* [in] */ Int32 appOp,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ DisplayContent* displayContent)
    : mService(service)
    , mPolicy(service->mPolicy)
    , mContext(service->mContext)
    , mSession(s)
    , mClient(c)
    , mAppOp(appOp)
    , mOwnerUid(0)
    , mToken(token)
    , mAppToken(NULL)
    , mSeq(seq)
    , mViewVisibility(viewVisibility)
    , mSystemUiVisibility(0)
    , mPolicyVisibility(TRUE)
    , mPolicyVisibilityAfterAnim(TRUE)
    , mAppOpVisibility(TRUE)
    , mAppFreezing(FALSE)
    , mAttachedHidden(FALSE)
    , mWallpaperVisible(FALSE)
    , mRequestedWidth(0)
    , mRequestedHeight(0)
    , mLastRequestedWidth(0)
    , mLastRequestedHeight(0)
    , mLayer(0)
    , mHaveFrame(FALSE)
    , mObscured(FALSE)
    , mTurnOnScreen(FALSE)
    , mLayoutSeq(-1)
    , mConfigHasChanged(FALSE)
    , mVisibleInsetsChanged(FALSE)
    , mContentInsetsChanged(FALSE)
    , mOverscanInsetsChanged(FALSE)
    , mStableInsetsChanged(FALSE)
    , mGivenInsetsPending(FALSE)
    , mTouchableInsets(IInternalInsetsInfo::TOUCHABLE_INSETS_FRAME)
    , mGlobalScale(1)
    , mInvGlobalScale(1)
    , mHScale(1)
    , mVScale(1)
    , mLastHScale(1)
    , mLastVScale(1)
    , mContentChanged(FALSE)
    , mWallpaperX(-1)
    , mWallpaperY(-1)
    , mWallpaperXStep(-1)
    , mWallpaperYStep(-1)
    , mWallpaperDisplayOffsetX(Elastos::Core::Math::INT32_MIN_VALUE)
    , mWallpaperDisplayOffsetY(Elastos::Core::Math::INT32_MIN_VALUE)
    , mXOffset(0)
    , mYOffset(0)
    , mRelayoutCalled(FALSE)
    , mLayoutNeeded(FALSE)
    , mExiting(FALSE)
    , mDestroying(FALSE)
    , mRemoveOnExit(FALSE)
    , mOrientationChanging(FALSE)
    , mLastFreezeDuration(0)
    , mRemoved(FALSE)
    , mRebuilding(FALSE)
    , mWasExiting(FALSE)
    , mHasSurface(FALSE)
    , mNotOnAppsDisplay(FALSE)
    , mDisplayContent(displayContent)
    , mUnderStatusBar(TRUE)
    , mShowToOwnerOnly(FALSE)
{
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&mAttrs);
    CArrayList::New((WindowList**)&mChildWindows);

    Int32 result;
    mAttrs->CopyFrom(attrs, &result);

    CRectF::New((IRectF**)&mShownFrame);
    CRect::New((IRect**)&mVisibleInsets);
    CRect::New((IRect**)&mLastVisibleInsets);
    CRect::New((IRect**)&mContentInsets);
    CRect::New((IRect**)&mLastContentInsets);
    CRect::New((IRect**)&mOverscanInsets);
    CRect::New((IRect**)&mLastOverscanInsets);
    CRect::New((IRect**)&mStableInsets);
    CRect::New((IRect**)&mLastStableInsets);
    CRect::New((IRect**)&mGivenContentInsets);
    CRect::New((IRect**)&mGivenVisibleInsets);
    CRegion::New((IRegion**)&mGivenTouchableRegion);
    CRect::New((IRect**)&mSystemDecorRect);
    CRect::New((IRect**)&mLastSystemDecorRect);

    CMatrix::New((IMatrix**)&mTmpMatrix);

    CRect::New((IRect**)&mFrame);
    CRect::New((IRect**)&mLastFrame);
    CRect::New((IRect**)&mCompatFrame);
    CRect::New((IRect**)&mContainingFrame);
    CRect::New((IRect**)&mDisplayFrame);
    CRect::New((IRect**)&mOverscanFrame);
    CRect::New((IRect**)&mContentFrame);
    CRect::New((IRect**)&mParentFrame);
    CRect::New((IRect**)&mVisibleFrame);
    CRect::New((IRect**)&mDecorFrame);
    CRect::New((IRect**)&mStableFrame);

    CWindowId::New(this, (IIWindowId**)&mWindowId);

    AutoPtr<DeathRecipient> deathRecipient = new DeathRecipient(this);

    Int32 flags;
    mAttrs->GetPrivateFlags(&flags);
    mEnforceSizeCompat = (flags & IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW) != 0;
    // if (WindowManagerService.localLOGV) Slog.v(
    //     TAG, "Window " + this + " client=" + c.asBinder()
    //     + " token=" + token + " (" + mAttrs.token + ")" + " params=" + a);
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)c->Probe(EIID_IProxy);
    if (proxy != NULL && FAILED(proxy->LinkToDeath(deathRecipient.Get(), 0))) {
        mDeathRecipient = NULL;
        mAttachedWindow = NULL;
        mLayoutAttached = FALSE;
        mIsImWindow = FALSE;
        mIsWallpaper = FALSE;
        mIsFloatingLayer = FALSE;
        mBaseLayer = 0;
        mSubLayer = 0;
        mInputWindowHandle = NULL;
        mWinAnimator = NULL;
        return;
    }
    else {
        mDeathRecipient = deathRecipient;

        Int32 type;
        mAttrs->GetType(&type);
        if ((type >= IWindowManagerLayoutParams::FIRST_SUB_WINDOW
            && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW)) {
            // The multiplier here is to reserve space for multiple
            // windows in the same type layer.
            Int32 layer, attachedWinType;
            attachedWindow->mAttrs->GetType(&attachedWinType);
            mPolicy->WindowTypeToLayerLw(attachedWinType, &layer);
            mBaseLayer = layer * CWindowManagerService::TYPE_LAYER_MULTIPLIER +
                    CWindowManagerService::TYPE_LAYER_OFFSET;
            mPolicy->SubWindowTypeToLayerLw(type, &mSubLayer);
            mAttachedWindow = attachedWindow;
            if (CWindowManagerService::DEBUG_ADD_REMOVE) Slogger::V(TAG, "Adding %p to %p", this, mAttachedWindow.Get());

            AutoPtr<WindowList> childWindows = mAttachedWindow->mChildWindows;
            Int32 numChildWindows;
            childWindows->GetSize(&numChildWindows);
            if (numChildWindows == 0) {
                childWindows->Add((IWindowState*)this);
            }
            else {
                Boolean added = FALSE;
                for (Int32 i = 0; i < numChildWindows; i++) {
                    AutoPtr<IInterface> obj;
                    childWindows->Get(i, (IInterface**)&obj);
                    Int32 childSubLayer = To_WindowState(obj)->mSubLayer;
                    if (mSubLayer < childSubLayer
                            || (mSubLayer == childSubLayer && childSubLayer < 0)) {
                        // We insert the child window into the list ordered by the sub-layer. For
                        // same sub-layers, the negative one should go below others; the positive
                        // one should go above others.
                        childWindows->Add(i, (IWindowState*)this);
                        added = TRUE;
                        break;
                    }
                }
                if (!added) {
                    childWindows->Add((IWindowState*)this);
                }
            }

            mLayoutAttached = type != IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG;
            mIsImWindow = attachedWinType == IWindowManagerLayoutParams::TYPE_INPUT_METHOD
                    || attachedWinType == IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG;
            mIsWallpaper = attachedWinType == IWindowManagerLayoutParams::TYPE_WALLPAPER;
            mIsFloatingLayer = mIsImWindow || mIsWallpaper;
        }
        else {
            // The multiplier here is to reserve space for multiple
            // windows in the same type layer.
            Int32 layer;
            mPolicy->WindowTypeToLayerLw(type, &layer);
            mBaseLayer = layer * CWindowManagerService::TYPE_LAYER_MULTIPLIER
                    + CWindowManagerService::TYPE_LAYER_OFFSET;
            mSubLayer = 0;
            mAttachedWindow = NULL;
            mLayoutAttached = FALSE;
            mIsImWindow = type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD
                || type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG;
            mIsWallpaper = type == IWindowManagerLayoutParams::TYPE_WALLPAPER;
            mIsFloatingLayer = mIsImWindow || mIsWallpaper;
        }

        WindowState* appWin = this;
        while (appWin->mAttachedWindow != NULL) {
            appWin = appWin->mAttachedWindow;
        }
        AutoPtr<WindowToken> appToken = appWin->mToken;
        while (appToken->mAppWindowToken == NULL) {
            AutoPtr<WindowToken> parent;
            HashMap<AutoPtr<IBinder>, AutoPtr<WindowToken> >::Iterator it = mService->mTokenMap.End();
            if (appToken->mToken != NULL)
                it = mService->mTokenMap.Find(appToken->mToken);
            if (it != mService->mTokenMap.End()) {
                parent = it->mSecond;
            }
            if ((parent == NULL) || (appToken == parent)) {
                break;
            }
            appToken = parent;
        }
        mRootToken = appToken;
        mAppToken = appToken->mAppWindowToken;
        if (mAppToken != NULL) {
            AutoPtr<DisplayContent> appDisplay = GetDisplayContent();
            mNotOnAppsDisplay = displayContent != appDisplay;
        }

        mWinAnimator = new WindowStateAnimator(this);
        attrs->GetAlpha(&mWinAnimator->mAlpha);

        mInputWindowHandle = new InputWindowHandle(
            mAppToken != NULL ? mAppToken->mInputApplicationHandle : NULL, this,
            displayContent->GetDisplayId());
    }
}

void WindowState::Attach()
{
    // if (WindowManagerService.localLOGV) Slog.v(
    //         TAG, "Attaching " + this + " token=" + mToken
    //         + ", list=" + mToken.windows);
    mSession->WindowAddedLocked();
}

ECode WindowState::GetOwningUid(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mOwnerUid;
    return NOERROR;
}

ECode WindowState::GetOwningPackage(
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package)
    return mAttrs->GetPackageName(package);
}

ECode WindowState::ComputeFrameLw(
    /* [in] */ IRect* pf,
    /* [in] */ IRect* df,
    /* [in] */ IRect* of,
    /* [in] */ IRect* cf,
    /* [in] */ IRect* vf,
    /* [in] */ IRect* dcf,
    /* [in] */ IRect* sf)
{
    mHaveFrame = TRUE;
    AutoPtr<TaskStack> stack = mAppToken != NULL ? GetStack() : NULL;
    if (stack != NULL && !stack->IsFullscreen()) {
        GetStackBounds(stack, mContainingFrame);
        if (mUnderStatusBar) {
            Int32 pfT;
            pf->GetTop(&pfT);
            mContainingFrame->SetTop(pfT);
        }
    }
    else {
        mContainingFrame->Set(pf);
    }

    mDisplayFrame->Set(df);

    Int32 pw, ph;
    mContainingFrame->GetWidth(&pw);
    mContainingFrame->GetHeight(&ph);

    AutoPtr<IViewGroupLayoutParams> viewGroupLp = IViewGroupLayoutParams::Probe(mAttrs);

    Int32 w, h, flags;
    mAttrs->GetFlags(&flags);
    if ((flags & IWindowManagerLayoutParams::FLAG_SCALED) != 0) {
        Int32 attrsW;
        viewGroupLp->GetWidth(&attrsW);
        if (attrsW < 0) {
            w = pw;
        }
        else if (mEnforceSizeCompat) {
            w = (Int32)(attrsW * mGlobalScale + 0.5);
        }
        else {
            w = attrsW;
        }
        Int32 attrsH;
        viewGroupLp->GetHeight(&attrsH);
        if (attrsH < 0) {
            h = ph;
        }
        else if (mEnforceSizeCompat) {
            h = (Int32)(attrsH * mGlobalScale + 0.5);
        } else {
            h = attrsH;
        }
    }
    else {
        Int32 attrsW;
        viewGroupLp->GetWidth(&attrsW);
        if (attrsW == IViewGroupLayoutParams::MATCH_PARENT) {
            w = pw;
        }
        else if (mEnforceSizeCompat) {
            w = (Int32)(mRequestedWidth * mGlobalScale + 0.5);
        }
        else {
            w = mRequestedWidth;
        }
        Int32 attrsH;
        viewGroupLp->GetHeight(&attrsH);
        if (attrsH == IViewGroupLayoutParams::MATCH_PARENT) {
            h = ph;
        }
        else if (mEnforceSizeCompat) {
            h = (Int32)(mRequestedHeight * mGlobalScale + 0.5);
        }
        else {
            h = mRequestedHeight;
        }
    }

    Boolean equals = FALSE;
    if (IObject::Probe(mParentFrame)->Equals(pf, &equals), !equals) {
        // Int32 pl, pt, pr, pb, l, t, r, b;
        // mParentFrame->Get(&pl, &pt, &pr, &pb);
        // pf->Get(&l, &t, &r, &b);
        // Slogger::I(TAG, "Window %p content frame from (%d, %d, %d, %d) to (%d, %d, %d, %d)",
        //         this, pl, pt, pr, pb, l, t, r, b);

        mParentFrame->Set(pf);
        mContentChanged = TRUE;
    }
    if (mRequestedWidth != mLastRequestedWidth || mRequestedHeight != mLastRequestedHeight) {
        mLastRequestedWidth = mRequestedWidth;
        mLastRequestedHeight = mRequestedHeight;
        mContentChanged = TRUE;
    }

    mOverscanFrame->Set(of);
    mContentFrame->Set(cf);
    mVisibleFrame->Set(vf);
    mDecorFrame->Set(dcf);
    mStableFrame->Set(sf);

    Int32 fw, fh;
    mFrame->GetWidth(&fw);
    mFrame->GetHeight(&fh);

    // System.out.println("In: w=" + w + " h=" + h + " container=" +
    //                    container + " x=" + mAttrs.x + " y=" + mAttrs.y);

    Float x, y;
    Int32 attrsX, attrsY;
    mAttrs->GetX(&attrsX);
    mAttrs->GetY(&attrsY);
    if (mEnforceSizeCompat) {
        x = attrsX * mGlobalScale;
        y = attrsY * mGlobalScale;
    }
    else {
        x = attrsX;
        y = attrsY;
    }

    Int32 attrsGravity;
    Float attrsHMargin, attrsVMargin;
    mAttrs->GetGravity(&attrsGravity);
    mAttrs->GetHorizontalMargin(&attrsHMargin);
    mAttrs->GetVerticalMargin(&attrsVMargin);
    AutoPtr<IGravity> gravity;
    CGravity::AcquireSingleton((IGravity**)&gravity);
    gravity->Apply(attrsGravity, w, h, mContainingFrame,
            (Int32)(x + attrsHMargin * pw),  (Int32)(y + attrsVMargin * ph), mFrame);

    // System.out.println("Out: " + mFrame);

    // Now make sure the window fits in the overall display.
    gravity->ApplyDisplay(attrsGravity, df, mFrame);

    // Make sure the content and visible frames are inside of the
    // final window frame.
    Int32 cfL, cfT, cfR, cfB, fL, fT, fR, fB;
    mContentFrame->GetLeft(&cfL);
    mContentFrame->GetTop(&cfT);
    mContentFrame->GetRight(&cfR);
    mContentFrame->GetBottom(&cfB);
    mFrame->GetLeft(&fL);
    mFrame->GetTop(&fT);
    mFrame->GetRight(&fR);
    mFrame->GetBottom(&fB);
    mContentFrame->Set(Elastos::Core::Math::Max(cfL, fL),
            Elastos::Core::Math::Max(cfT, fT),
            Elastos::Core::Math::Min(cfR, fR),
            Elastos::Core::Math::Min(cfB, fB));

    Int32 vfL, vfT, vfR, vfB;
    mVisibleFrame->GetLeft(&vfL);
    mVisibleFrame->GetTop(&vfT);
    mVisibleFrame->GetRight(&vfR);
    mVisibleFrame->GetBottom(&vfB);
    mVisibleFrame->Set(Elastos::Core::Math::Max(vfL, fL),
            Elastos::Core::Math::Max(vfT, fT),
            Elastos::Core::Math::Min(vfR, fR),
            Elastos::Core::Math::Min(vfB, fB));

    Int32 sfL, sfT, sfR, sfB;
    mStableFrame->GetLeft(&sfL);
    mStableFrame->GetTop(&sfT);
    mStableFrame->GetRight(&sfR);
    mStableFrame->GetBottom(&sfB);
    mStableFrame->Set(Elastos::Core::Math::Max(sfL, fL),
            Elastos::Core::Math::Max(sfT, fT),
            Elastos::Core::Math::Min(sfR, fR),
            Elastos::Core::Math::Min(sfB, fB));

    Int32 ofL, ofT, ofR, ofB;
    mOverscanFrame->GetLeft(&ofL);
    mOverscanFrame->GetTop(&ofT);
    mOverscanFrame->GetRight(&ofR);
    mOverscanFrame->GetBottom(&ofB);
    mOverscanInsets->Set(Elastos::Core::Math::Max(ofL - fL, 0),
            Elastos::Core::Math::Max(ofT - fT, 0),
            Elastos::Core::Math::Max(fR - ofR, 0),
            Elastos::Core::Math::Max(fB - ofB, 0));

    mContentInsets->Set(cfL - fL, cfT - fT, fR - cfR, fB - cfB);

    mVisibleInsets->Set(vfL - fL, vfT - fT, fR - vfR, fB - vfB);

    mStableInsets->Set(Elastos::Core::Math::Max(sfL - fL, 0),
            Elastos::Core::Math::Max(sfT - fT, 0),
            Elastos::Core::Math::Max(fR - sfR, 0),
            Elastos::Core::Math::Max(fB - sfB, 0));

    mCompatFrame->Set(mFrame);
    if (mEnforceSizeCompat) {
        // If there is a size compatibility scale being applied to the
        // window, we need to apply this to its insets so that they are
        // reported to the app in its coordinate space.
        mOverscanInsets->Scale(mInvGlobalScale);
        mContentInsets->Scale(mInvGlobalScale);
        mVisibleInsets->Scale(mInvGlobalScale);
        mStableInsets->Scale(mInvGlobalScale);

        // Also the scaled frame that we report to the app needs to be
        // adjusted to be in its coordinate space.
        mCompatFrame->Scale(mInvGlobalScale);
    }

    Int32 newFW, newFH;
    if (mIsWallpaper && ((mFrame->GetWidth(&newFW), fw != newFW) || (mFrame->GetHeight(&newFH), fh != newFH))) {
        AutoPtr<DisplayContent> displayContent = GetDisplayContent();
        if (displayContent != NULL) {
            AutoPtr<IDisplayInfo> displayInfo = displayContent->GetDisplayInfo();
            Int32 logicalW, logicalH;
            displayInfo->GetLogicalWidth(&logicalW);
            displayInfo->GetLogicalHeight(&logicalH);
            mService->UpdateWallpaperOffsetLocked(this, logicalW, logicalH, FALSE);
        }
    }

    // if (DEBUG_LAYOUT || WindowManagerService.localLOGV) Slog.v(TAG,
    //         "Resolving (mRequestedWidth="
    //         + mRequestedWidth + ", mRequestedheight="
    //         + mRequestedHeight + ") to" + " (pw=" + pw + ", ph=" + ph
    //         + "): frame=" + mFrame.toShortString()
    //         + " ci=" + mContentInsets.toShortString()
    //         + " vi=" + mVisibleInsets.toShortString()
    //         + " vi=" + mStableInsets.toShortString());
    return NOERROR;
}

ECode WindowState::GetFrameLw(
    /* [out] */ IRect** frame)
{
    VALIDATE_NOT_NULL(frame)
    *frame = mFrame;
    REFCOUNT_ADD(*frame)
    return NOERROR;
}

ECode WindowState::GetShownFrameLw(
    /* [out] */ IRectF** shownFrame)
{
    VALIDATE_NOT_NULL(shownFrame)
    *shownFrame = mShownFrame;
    REFCOUNT_ADD(*shownFrame)
    return NOERROR;
}

ECode WindowState::GetOverscanFrameLw(
    /* [out] */ IRect** displayFrame)
{
    VALIDATE_NOT_NULL(displayFrame)
    *displayFrame = mOverscanFrame;
    REFCOUNT_ADD(*displayFrame)
    return NOERROR;
}

ECode WindowState::GetDisplayFrameLw(
    /* [out] */ IRect** displayFrame)
{
    VALIDATE_NOT_NULL(displayFrame)
    *displayFrame = mDisplayFrame;
    REFCOUNT_ADD(*displayFrame)
    return NOERROR;
}

ECode WindowState::GetContentFrameLw(
    /* [out] */ IRect** contentFrame)
{
    VALIDATE_NOT_NULL(contentFrame)
    *contentFrame = mContentFrame;
    REFCOUNT_ADD(*contentFrame)
    return NOERROR;
}

ECode WindowState::GetVisibleFrameLw(
    /* [out] */ IRect** visibleFrame)
{
    VALIDATE_NOT_NULL(visibleFrame)
    *visibleFrame = mVisibleFrame;
    REFCOUNT_ADD(*visibleFrame)
    return NOERROR;
}

ECode WindowState::GetGivenInsetsPendingLw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGivenInsetsPending;
    return NOERROR;
}

ECode WindowState::GetGivenContentInsetsLw(
    /* [out] */ IRect** insetsRect)
{
    VALIDATE_NOT_NULL(insetsRect)
    *insetsRect = mGivenContentInsets;
    REFCOUNT_ADD(*insetsRect)
    return NOERROR;
}

ECode WindowState::GetGivenVisibleInsetsLw(
    /* [out] */ IRect** visibleArea)
{
    VALIDATE_NOT_NULL(visibleArea)
    *visibleArea = mGivenVisibleInsets;
    REFCOUNT_ADD(*visibleArea)
    return NOERROR;
}

ECode WindowState::GetAttrs(
    /* [out] */ IWindowManagerLayoutParams** attrs)
{
    VALIDATE_NOT_NULL(attrs)
    *attrs = mAttrs;
    REFCOUNT_ADD(*attrs)
    return NOERROR;
}

ECode WindowState::GetNeedsMenuLw(
    /* [in] */ IWindowState* bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = -1;
    AutoPtr<WindowState> ws = this;
    AutoPtr<WindowList> windows = GetWindowList();
    while (TRUE) {
        Int32 flags;
        ws->mAttrs->GetPrivateFlags(&flags);
        if ((flags
                & IWindowManagerLayoutParams::PRIVATE_FLAG_SET_NEEDS_MENU_KEY) != 0) {
            ws->mAttrs->GetFlags(&flags);
            *result = (flags & IWindowManagerLayoutParams::FLAG_NEEDS_MENU_KEY) != 0;
            return NOERROR;
        }
        // If we reached the bottom of the range of windows we are considering,
        // assume no menu is needed.
        if ((IWindowState*)ws.Get() == bottom) {
            *result = FALSE;
            return NOERROR;
        }
        // The current window hasn't specified whether menu key is needed;
        // look behind it.
        // First, we may need to determine the starting position.
        if (index < 0) {
            windows->IndexOf((IWindowState*)ws.Get(), &index);
        }
        index--;
        if (index < 0) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        windows->Get(index, (IInterface**)&obj);
        ws = To_WindowState(obj);
    }
}

ECode WindowState::GetSystemUiVisibility(
    /* [out] */ Int32* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mSystemUiVisibility;
    return NOERROR;
}

ECode WindowState::GetSurfaceLayer(
    /* [out] */ Int32* surfaceLayer)
{
    VALIDATE_NOT_NULL(surfaceLayer)
    *surfaceLayer = mLayer;
    return NOERROR;
}

ECode WindowState::GetAppToken(
    /* [out] */ IApplicationToken** token)
{
    VALIDATE_NOT_NULL(token)
    *token = mAppToken != NULL ? mAppToken->mAppToken : NULL;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

ECode WindowState::IsVoiceInteraction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAppToken != NULL ? mAppToken->mVoiceInteraction : FALSE;
    return NOERROR;
}

Boolean WindowState::SetInsetsChanged()
{
    Boolean equals1, equals2, equals3, equals4;
    IObject::Probe(mLastOverscanInsets)->Equals((IInterface*)mOverscanInsets.Get(), &equals1);
    IObject::Probe(mLastContentInsets)->Equals((IInterface*)mLastContentInsets.Get(), &equals2);
    IObject::Probe(mLastVisibleInsets)->Equals((IInterface*)mLastVisibleInsets.Get(), &equals3);
    IObject::Probe(mLastStableInsets)->Equals((IInterface*)mLastStableInsets.Get(), &equals4);
    mOverscanInsetsChanged |= !equals1;
    mContentInsetsChanged |= !equals2;
    mVisibleInsetsChanged |= !equals3;
    mStableInsetsChanged |= !equals4;
    return mOverscanInsetsChanged || mContentInsetsChanged || mVisibleInsetsChanged;
}

AutoPtr<DisplayContent> WindowState::GetDisplayContent()
{
    if (mAppToken == NULL || mNotOnAppsDisplay) {
        return mDisplayContent;
    }
    AutoPtr<TaskStack> stack = GetStack();
    return stack == NULL ? mDisplayContent : stack->GetDisplayContent();
}

Int32 WindowState::GetDisplayId()
{
    AutoPtr<DisplayContent> displayContent = GetDisplayContent();
    if (displayContent == NULL) {
        return -1;
    }
    return displayContent->GetDisplayId();
}

AutoPtr<TaskStack> WindowState::GetStack()
{
    AutoPtr<AppWindowToken> wtoken = mAppToken == NULL ? mService->mFocusedApp.Get() : mAppToken;
    if (wtoken != NULL) {
        AutoPtr<IInterface> value;
        mService->mTaskIdToTask->Get(wtoken->mGroupId, (IInterface**)&value);
        AutoPtr<Task> task = (Task*)(IObject*)value.Get();
        if (task != NULL) {
            if (task->mStack != NULL) {
                return task->mStack;
            }
            Slogger::E(TAG, "getStack: mStack null for task=%p", task.Get());
        }
        else {
            Slogger::E(TAG, "getStack: %p couldn't find taskId=%d", this, wtoken->mGroupId
                /*+ " Callers=" + Debug.getCallers(4)*/);
        }
    }
    return mDisplayContent->GetHomeStack();
}

void WindowState::GetStackBounds(
    /* [in] */ IRect* bounds)
{
    GetStackBounds(GetStack(), bounds);
}

void WindowState::GetStackBounds(
    /* [in] */ TaskStack* stack,
    /* [in] */ IRect* bounds)
{
    if (stack != NULL) {
        stack->GetBounds(bounds);
        return;
    }
    bounds->Set(mFrame);
}

Int64 WindowState::GetInputDispatchingTimeoutNanos()
{
    return mAppToken != NULL
            ? mAppToken->mInputDispatchingTimeoutNanos
            : CWindowManagerService::DEFAULT_INPUT_DISPATCHING_TIMEOUT_NANOS;
}

ECode WindowState::HasAppShownWindows(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAppToken != NULL && (mAppToken->mFirstWindowDrawn || mAppToken->mStartingDisplayed);
    return NOERROR;
}

Boolean WindowState::IsIdentityMatrix(
    /* [in] */ Float dsdx,
    /* [in] */ Float dtdx,
    /* [in] */ Float dsdy,
    /* [in] */ Float dtdy)
{
    if (dsdx < 0.99999f || dsdx > 1.00001f) return FALSE;
    if (dtdy < 0.99999f || dtdy > 1.00001f) return FALSE;
    if (dtdx < -0.000001f || dtdx > 0.000001f) return FALSE;
    if (dsdy < -0.000001f || dsdy > 0.000001f) return FALSE;
    return TRUE;
}

void WindowState::Prelayout()
{
    if (mEnforceSizeCompat) {
        mGlobalScale = mService->mCompatibleScreenScale;
        mInvGlobalScale = 1 / mGlobalScale;
    }
    else {
        mGlobalScale = mInvGlobalScale = 1;
    }
}

ECode WindowState::IsVisibleLw(
    /* [out] */ Boolean* isVisible)
{
    VALIDATE_NOT_NULL(isVisible);
    AutoPtr<AppWindowToken> atoken = mAppToken;
    *isVisible = mHasSurface && mPolicyVisibility && !mAttachedHidden
            && (atoken == NULL || !atoken->mHiddenRequested)
            && !mExiting && !mDestroying;
    return NOERROR;
}

ECode WindowState::IsVisibleOrBehindKeyguardLw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mRootToken->mWaitingToShow && mService->mAppTransition->IsTransitionSet()) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<AppWindowToken> atoken = mAppToken;
    Boolean animating = atoken != NULL
            ? (atoken->mAppAnimator->mAnimation != NULL) : FALSE;
    *result = mHasSurface && !mDestroying && !mExiting
            && (atoken == NULL ? mPolicyVisibility : !atoken->mHiddenRequested)
            && ((!mAttachedHidden && mViewVisibility == IView::VISIBLE
                            && !mRootToken->mHidden)
                    || mWinAnimator->mAnimation != NULL || animating);
    return NOERROR;
}

Boolean WindowState::IsWinVisibleLw()
{
    AutoPtr<AppWindowToken> atoken = mAppToken;
    return mHasSurface && mPolicyVisibility && !mAttachedHidden
            && (atoken == NULL || !atoken->mHiddenRequested || atoken->mAppAnimator->mAnimating)
            && !mExiting && !mDestroying;
}

Boolean WindowState::IsVisibleNow()
{
    return mHasSurface && mPolicyVisibility && !mAttachedHidden
            && !mRootToken->mHidden && !mExiting && !mDestroying;
}

Boolean WindowState::IsPotentialDragTarget()
{
    return IsVisibleNow() && !mRemoved
            && mInputChannel != NULL && mInputWindowHandle != NULL;
}

Boolean WindowState::IsVisibleOrAdding()
{
    AutoPtr<AppWindowToken> atoken = mAppToken;
    return (mHasSurface || (!mRelayoutCalled && mViewVisibility == IView::VISIBLE))
            && mPolicyVisibility && !mAttachedHidden
            && (atoken == NULL || !atoken->mHiddenRequested)
            && !mExiting && !mDestroying;
}

Boolean WindowState::IsOnScreen()
{
    if (!mHasSurface || !mPolicyVisibility || mDestroying) {
        return FALSE;
    }
    AutoPtr<AppWindowToken> atoken = mAppToken;
    if (atoken != NULL) {
        return ((!mAttachedHidden && !atoken->mHiddenRequested)
                || mWinAnimator->mAnimation != NULL || atoken->mAppAnimator->mAnimation != NULL);
    }
    return !mAttachedHidden || mWinAnimator->mAnimation != NULL;
}

Boolean WindowState::IsReadyForDisplay()
{
    if (mRootToken->mWaitingToShow && mService->mAppTransition->IsTransitionSet()) {
        return FALSE;
    }
    return mHasSurface && mPolicyVisibility && !mDestroying
            && ((!mAttachedHidden && mViewVisibility == IView::VISIBLE
            && !mRootToken->mHidden)
            || mWinAnimator->mAnimation != NULL
            || ((mAppToken != NULL) && (mAppToken->mAppAnimator->mAnimation != NULL)));
}

Boolean WindowState::IsReadyForDisplayIgnoringKeyguard()
{
    if (mRootToken->mWaitingToShow && mService->mAppTransition->IsTransitionSet()) {
        return FALSE;
    }
    AutoPtr<AppWindowToken> atoken = mAppToken;
    if (atoken == NULL && !mPolicyVisibility) {
        // If this is not an app window, and the policy has asked to force
        // hide, then we really do want to hide.
        return FALSE;
    }
    return mHasSurface && !mDestroying
            && ((!mAttachedHidden && mViewVisibility == IView::VISIBLE
                            && !mRootToken->mHidden)
                    || mWinAnimator->mAnimation != NULL
                    || ((atoken != NULL) && (atoken->mAppAnimator->mAnimation != NULL)
                            && !mWinAnimator->IsDummyAnimation()));
}

ECode WindowState::IsDisplayedLw(
    /* [out] */ Boolean* isDisplayed)
{
    VALIDATE_NOT_NULL(isDisplayed)
    AutoPtr<AppWindowToken> atoken = mAppToken;
    *isDisplayed = IsDrawnLw() && mPolicyVisibility
        && ((!mAttachedHidden &&
                (atoken == NULL || !atoken->mHiddenRequested))
                    || mWinAnimator->mAnimating
                    || (atoken != NULL && atoken->mAppAnimator->mAnimation != NULL));
    return NOERROR;
}

ECode WindowState::IsAnimatingLw(
    /* [out] */ Boolean* isAnimating)
{
    VALIDATE_NOT_NULL(isAnimating)
    *isAnimating = mWinAnimator->mAnimation != NULL
            || (mAppToken != NULL && mAppToken->mAppAnimator->mAnimation != NULL);
    return NOERROR;
}

ECode WindowState::IsGoneForLayoutLw(
    /* [out] */ Boolean* isGone)
{
    VALIDATE_NOT_NULL(isGone)
    AutoPtr<AppWindowToken> atoken = mAppToken;
    Boolean isAnimatingLw;
    *isGone = mViewVisibility == IView::GONE
            || !mRelayoutCalled
            || (atoken == NULL && mRootToken->mHidden)
            || (atoken != NULL && (atoken->mHiddenRequested || atoken->mHidden))
            || mAttachedHidden
            || (mExiting && (IsAnimatingLw(&isAnimatingLw), !isAnimatingLw))
            || mDestroying;
    return NOERROR;
}

Boolean WindowState::IsDrawFinishedLw()
{
    return mHasSurface && !mDestroying &&
            (mWinAnimator->mDrawState == WindowStateAnimator::COMMIT_DRAW_PENDING
            || mWinAnimator->mDrawState == WindowStateAnimator::READY_TO_SHOW
            || mWinAnimator->mDrawState == WindowStateAnimator::HAS_DRAWN);
}

Boolean WindowState::IsDrawnLw()
{
    return mHasSurface && !mDestroying &&
            (mWinAnimator->mDrawState == WindowStateAnimator::READY_TO_SHOW
            || mWinAnimator->mDrawState == WindowStateAnimator::HAS_DRAWN);
}

Boolean WindowState::IsOpaqueDrawn()
{
    Int32 format, type;
    mAttrs->GetFormat(&format);
    mAttrs->GetType(&type);
    return (format == IPixelFormat::OPAQUE
            || type == IWindowManagerLayoutParams::TYPE_WALLPAPER)
            && IsDrawnLw() && mWinAnimator->mAnimation == NULL
            && (mAppToken == NULL || mAppToken->mAppAnimator->mAnimation == NULL);
}

Boolean WindowState::ShouldAnimateMove()
{
    Int32 top, lastTop, left, lastLeft;
    mFrame->GetTop(&top);
    mFrame->GetLeft(&left);
    mLastFrame->GetTop(&lastTop);
    mLastFrame->GetLeft(&lastLeft);
    Int32 privateFlags;
    return mContentChanged && !mExiting && !mWinAnimator->mLastHidden && mService->OkToDisplay()
            && (top != lastTop || left != lastLeft)
            && (mAttrs->GetPrivateFlags(&privateFlags), (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_NO_MOVE_ANIMATION) == 0)
            && (mAttachedWindow == NULL || !mAttachedWindow->ShouldAnimateMove());
}

Boolean WindowState::IsFullscreen(
    /* [in] */ Int32 screenWidth,
    /* [in] */ Int32 screenHeight)
{
    Int32 left, top, right, bottom;
    mFrame->GetLeft(&left);
    mFrame->GetTop(&top);
    mFrame->GetRight(&right);
    mFrame->GetBottom(&bottom);
    return left <= 0 && top <= 0 && right >= screenWidth && bottom >= screenHeight;
}

Boolean WindowState::IsConfigChanged()
{
    Int32 result;
    Boolean configChanged = mConfiguration != mService->mCurConfiguration
            && (mConfiguration == NULL || (mConfiguration->Diff(mService->mCurConfiguration, &result), result != 0));

    Int32 privateFlags;
    if (mAttrs->GetPrivateFlags(&privateFlags), (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
        // Retain configuration changed status until resetConfiguration called.
        mConfigHasChanged |= configChanged;
        configChanged = mConfigHasChanged;
    }

    return configChanged;
}

void WindowState::RemoveLocked()
{
    DisposeInputChannel();

    if (mAttachedWindow != NULL) {
        // if (WindowManagerService.DEBUG_ADD_REMOVE) Slog.v(TAG, "Removing " + this + " from " + mAttachedWindow);
        mAttachedWindow->mChildWindows->Remove((IWindowState*)this);
    }
    mWinAnimator->DestroyDeferredSurfaceLocked();
    mWinAnimator->DestroySurfaceLocked();
    mSession->WindowRemovedLocked();
    AutoPtr<IProxy> proxy = (IProxy*)mClient->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean res;
        proxy->UnlinkToDeath(mDeathRecipient.Get(), 0, &res);
    }
}

ECode WindowState::SetConfiguration(
    /* [in] */ IConfiguration* newConfig)
{
    mConfiguration = newConfig;
    mConfigHasChanged = FALSE;
    return NOERROR;
}

void WindowState::SetInputChannel(
    /* [in] */ IInputChannel* inputChannel)
{
    // if (mInputChannel != null) {
    //     throw new IllegalStateException("Window already has an input channel.");
    // }
    assert(mInputChannel == NULL);

    mInputChannel = inputChannel;
    mInputWindowHandle->mInputChannel = inputChannel;
}

void WindowState::DisposeInputChannel()
{
    if (mInputChannel != NULL) {
        mService->mInputManager->UnregisterInputChannel(mInputChannel);

        mInputChannel->Dispose();
        mInputChannel = NULL;
    }

    mInputWindowHandle->mInputChannel = NULL;
}

Boolean WindowState::CanReceiveKeys()
{
    Int32 flags;
    mAttrs->GetFlags(&flags);
    return IsVisibleOrAdding()
            && (mViewVisibility == IView::VISIBLE)
            && ((flags & IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE) == 0);
}

ECode WindowState::HasDrawnLw(
    /* [out] */ Boolean* drawn)
{
    VALIDATE_NOT_NULL(drawn)
    *drawn = mWinAnimator->mDrawState == WindowStateAnimator::HAS_DRAWN;
    return NOERROR;
}

ECode WindowState::ShowLw(
    /* [in] */ Boolean doAnimation,
    /* [out] */ Boolean* canShown)
{
    VALIDATE_NOT_NULL(canShown)
    *canShown = ShowLw(doAnimation, TRUE);
    return NOERROR;
}

Boolean WindowState::ShowLw(
    /* [in] */ Boolean doAnimation,
    /* [in] */ Boolean requestAnim)
{
    if (IsHiddenFromUserLocked()) {
        return FALSE;
    }
    if (!mAppOpVisibility) {
        // Being hidden due to app op request.
        return FALSE;
    }
    if (mPolicyVisibility && mPolicyVisibilityAfterAnim) {
        // Already showing.
        return FALSE;
    }
    // if (DEBUG_VISIBILITY) Slog.v(TAG, "Policy visibility true: " + this);
    if (doAnimation) {
        // if (DEBUG_VISIBILITY) Slog.v(TAG, "doAnimation: mPolicyVisibility="
        //         + mPolicyVisibility + " mAnimation=" + mWinAnimator.mAnimation);
        if (!mService->OkToDisplay()) {
            doAnimation = FALSE;
        }
        else if (mPolicyVisibility && mWinAnimator->mAnimation == NULL) {
            // Check for the case where we are currently visible and
            // not animating; we do not want to do animation at such a
            // point to become visible when we already are.
            doAnimation = FALSE;
        }
    }
    mPolicyVisibility = TRUE;
    mPolicyVisibilityAfterAnim = TRUE;
    if (doAnimation) {
        mWinAnimator->ApplyAnimationLocked(IWindowManagerPolicy::TRANSIT_ENTER, TRUE);
    }
    if (requestAnim) {
        mService->ScheduleAnimationLocked();
    }
    return TRUE;
}

ECode WindowState::HideLw(
    /* [in] */ Boolean doAnimation,
    /* [out] */ Boolean* canHidden)
{
    VALIDATE_NOT_NULL(canHidden);
    *canHidden = HideLw(doAnimation, TRUE);
    return NOERROR;
}

Boolean WindowState::HideLw(
    /* [in] */ Boolean doAnimation,
    /* [in] */ Boolean requestAnim)
{
    if (doAnimation) {
        if (!mService->OkToDisplay()) {
            doAnimation = FALSE;
        }
    }
    Boolean current = doAnimation ? mPolicyVisibilityAfterAnim
            : mPolicyVisibility;
    if (!current) {
        // Already hiding.
        return FALSE;
    }
    if (doAnimation) {
        mWinAnimator->ApplyAnimationLocked(IWindowManagerPolicy::TRANSIT_EXIT, FALSE);
        if (mWinAnimator->mAnimation == NULL) {
            doAnimation = FALSE;
        }
    }
    if (doAnimation) {
        mPolicyVisibilityAfterAnim = FALSE;
    }
    else {
        // if (DEBUG_VISIBILITY) Slog.v(TAG, "Policy visibility false: " + this);
        mPolicyVisibilityAfterAnim = FALSE;
        mPolicyVisibility = FALSE;
        // Window is no longer visible -- make sure if we were waiting
        // for it to be displayed before enabling the display, that
        // we allow the display to be enabled now.
        mService->EnableScreenIfNeededLocked();
        if (mService->mCurrentFocus.Get() == this) {
            if (CWindowManagerService::DEBUG_FOCUS_LIGHT)
                Slogger::I(TAG, "WindowState.hideLw: setting mFocusMayChange true");
            mService->mFocusMayChange = TRUE;
        }
    }
    if (requestAnim) {
        mService->ScheduleAnimationLocked();
    }
    return TRUE;
}

void WindowState::SetAppOpVisibilityLw(
    /* [in] */ Boolean state)
{
    if (mAppOpVisibility != state) {
        mAppOpVisibility = state;
        if (state) {
            // If the policy visibility had last been to hide, then this
            // will incorrectly show at this point since we lost that
            // information.  Not a big deal -- for the windows that have app
            // ops modifies they should only be hidden by policy due to the
            // lock screen, and the user won't be changing this if locked.
            // Plus it will quickly be fixed the next time we do a layout.
            ShowLw(TRUE, TRUE);
        }
        else {
            HideLw(TRUE, TRUE);
        }
    }
}

ECode WindowState::IsAlive(
    /* [out] */ Boolean* isAlive)
{
    VALIDATE_NOT_NULL(isAlive)
    *isAlive = FALSE;
    AutoPtr<IProxy> proxy = (IProxy*)IProxy::Probe(mClient);
    if (proxy != NULL) {
        return proxy->IsStubAlive(isAlive);
    }
    return NOERROR;
}

Boolean WindowState::IsClosing()
{
    List<AutoPtr<AppWindowToken> >::Iterator it = mService->mClosingApps.Begin();
    for (; it != mService->mClosingApps.End(); ++it) {
        if ((*it).Get() == mAppToken) break;
    }
    return mExiting || (it != mService->mClosingApps.End());
}

ECode WindowState::IsDefaultDisplay(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<DisplayContent> displayContent = GetDisplayContent();
    if (displayContent == NULL) {
        // Only a window that was on a non-default display can be detached from it.
        return NOERROR;
    }
    *result = displayContent->mIsDefaultDisplay;
    return NOERROR;
}

void WindowState::SetShowToOwnerOnlyLocked(
    /* [in] */ Boolean showToOwnerOnly)
{
    mShowToOwnerOnly = showToOwnerOnly;
}

Boolean WindowState::IsHiddenFromUserLocked()
{
    // Attached windows are evaluated based on the window that they are attached to.
    AutoPtr<WindowState> win = this;
    while (win->mAttachedWindow != NULL) {
        win = win->mAttachedWindow;
    }
    Int32 type;
    win->mAttrs->GetType(&type);
    if (type < IWindowManagerLayoutParams::FIRST_SYSTEM_WINDOW
            && win->mAppToken != NULL && win->mAppToken->mShowWhenLocked) {
        // Save some cycles by not calling getDisplayInfo unless it is an application
        // window intended for all users.
        AutoPtr<DisplayContent> displayContent = win->GetDisplayContent();
        if (displayContent == NULL) {
            return TRUE;
        }
        AutoPtr<IDisplayInfo> displayInfo = displayContent->GetDisplayInfo();
        Int32 left, top, right, bottom, appWidth, appHeight;
        displayInfo->GetAppWidth(&appWidth);
        displayInfo->GetAppHeight(&appHeight);
        if ((win->mFrame->GetLeft(&left), left <= 0) &&
                (win->mFrame->GetTop(&top), top <= 0) &&
                (win->mFrame->GetRight(&right), right >= appWidth) &&
                (win->mFrame->GetBottom(&bottom), bottom >= appHeight)) {
            // Is a fullscreen window, like the clock alarm. Show to everyone.
            return FALSE;
        }
    }

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetUserId(win->mOwnerUid, &id);
    return win->mShowToOwnerOnly && !mService->IsCurrentProfileLocked(id);
}

void WindowState::ApplyInsets(
    /* [in] */ IRegion* outRegion,
    /* [in] */ IRect* frame,
    /* [in] */ IRect* inset)
{
    Int32 left, top, right, bottom, insetLeft, insetTop, insetRight, insetBottom;
    frame->GetLeft(&left);
    frame->GetTop(&top);
    frame->GetRight(&right);
    frame->GetBottom(&bottom);
    inset->GetLeft(&insetLeft);
    inset->GetTop(&insetTop);
    inset->GetRight(&insetRight);
    inset->GetBottom(&insetBottom);
    Boolean result;
    outRegion->Set(left + insetLeft, top + insetTop,
            right - insetRight, bottom - insetBottom, &result);
}

void WindowState::GetTouchableRegion(
    /* [in] */ IRegion* outRegion)
{
    AutoPtr<IRect> frame = mFrame;
    switch (mTouchableInsets) {
        default:
        case IInternalInsetsInfo::TOUCHABLE_INSETS_FRAME:
            Boolean result;
            outRegion->Set(frame, &result);
            break;
        case IInternalInsetsInfo::TOUCHABLE_INSETS_CONTENT:
            ApplyInsets(outRegion, frame, mGivenContentInsets);
            break;
        case IInternalInsetsInfo::TOUCHABLE_INSETS_VISIBLE:
            ApplyInsets(outRegion, frame, mGivenVisibleInsets);
            break;
        case IInternalInsetsInfo::TOUCHABLE_INSETS_REGION: {
            AutoPtr<IRegion> givenTouchableRegion = mGivenTouchableRegion;
            Boolean result;
            outRegion->Set(givenTouchableRegion, &result);
            Int32 left, top;
            frame->GetLeft(&left);
            frame->GetTop(&top);
            outRegion->Translate(left, top);
            break;
        }
    }
}

AutoPtr<WindowList> WindowState::GetWindowList()
{
    AutoPtr<DisplayContent> displayContent = GetDisplayContent();
    return displayContent == NULL ? NULL : displayContent->GetWindowList();
}

void WindowState::ReportFocusChangedSerialized(
    /* [in] */ Boolean focused,
    /* [in] */ Boolean inTouchMode)
{
    // try {
    mClient->WindowFocusChanged(focused, inTouchMode);
    // } catch (RemoteException e) {
    // }
    if (mFocusCallbacks != NULL) {
        Int32 N;
        mFocusCallbacks->BeginBroadcast(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> item;
            mFocusCallbacks->GetBroadcastItem(i, (IInterface**)&item);
            AutoPtr<IIWindowFocusObserver> obs = IIWindowFocusObserver::Probe(item);
            // try {
            AutoPtr<IBinder> b = IBinder::Probe(mWindowId);
            if (focused) {
                obs->FocusGained(b);
            } else {
                obs->FocusLost(b);
            }
            // } catch (RemoteException e) {
            // }
        }
        mFocusCallbacks->FinishBroadcast();
    }
}

void WindowState::ReportResized()
{
    // try {
    if (CWindowManagerService::DEBUG_RESIZE || CWindowManagerService::DEBUG_ORIENTATION)
        Slogger::V(TAG, "Reporting new frame to %p: %p", this, mCompatFrame.Get());

    Boolean configChanged = IsConfigChanged();
    if ((CWindowManagerService::DEBUG_RESIZE || CWindowManagerService::DEBUG_ORIENTATION
            || CWindowManagerService::DEBUG_CONFIGURATION) && configChanged) {
        Slogger::I(TAG, "Sending new config to window %p: %dx%d / %p", this,
                mWinAnimator->mSurfaceW, mWinAnimator->mSurfaceH, mService->mCurConfiguration.Get());
    }
    SetConfiguration(mService->mCurConfiguration);
    if (CWindowManagerService::DEBUG_ORIENTATION && mWinAnimator->mDrawState == WindowStateAnimator::DRAW_PENDING)
        Slogger::I(TAG, "Resizing %p WITH DRAW PENDING", this);

    AutoPtr<IRect> frame = mFrame;
    AutoPtr<IRect> overscanInsets = mLastOverscanInsets;
    AutoPtr<IRect> contentInsets = mLastContentInsets;
    AutoPtr<IRect> visibleInsets = mLastVisibleInsets;
    AutoPtr<IRect> stableInsets = mLastStableInsets;
    Boolean reportDraw = mWinAnimator->mDrawState == WindowStateAnimator::DRAW_PENDING;
    AutoPtr<IConfiguration> newConfig = configChanged ? mConfiguration : NULL;
    Int32 type;
    mAttrs->GetType(&type);
    if (type != IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING
            /* && mClient instanceof IWindow.Stub*/) {
        // To prevent deadlock simulate one-way call if win.mClient is a local object.
        AutoPtr<IRunnable> runnable = new ResizeRunnable(this, frame, overscanInsets, contentInsets,
                visibleInsets, stableInsets,  reportDraw, newConfig);
        Boolean result;
        mService->mH->Post(runnable, &result);
    }
    else {
        mClient->Resized(frame, overscanInsets, contentInsets, visibleInsets, stableInsets,
                reportDraw, newConfig);
    }

    //TODO (multidisplay): Accessibility supported only for the default display.
    if (mService->mAccessibilityController != NULL
            && GetDisplayId() == IDisplay::DEFAULT_DISPLAY) {
        mService->mAccessibilityController->OnSomeWindowResizedOrMovedLocked();
    }

    mOverscanInsetsChanged = FALSE;
    mContentInsetsChanged = FALSE;
    mVisibleInsetsChanged = FALSE;
    mStableInsetsChanged = FALSE;
    mWinAnimator->mSurfaceResized = FALSE;
    // } catch (RemoteException e) {
    //     mOrientationChanging = false;
    //     mLastFreezeDuration = (int)(SystemClock.elapsedRealtime()
    //             - mService.mDisplayFreezeTime);
    // }
}

void WindowState::RegisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoLock lock(mService->mWindowMapLock);
    if (mFocusCallbacks == NULL) {
        CRemoteCallbackList::New((IRemoteCallbackList**)&mFocusCallbacks);
    }
    Boolean result;
    mFocusCallbacks->Register(observer, &result);
}

void WindowState::UnregisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoLock lock(mService->mWindowMapLock);
    if (mFocusCallbacks != NULL) {
        Boolean result;
        mFocusCallbacks->Unregister(observer, &result);
    }
}

Boolean WindowState::IsFocused()
{
    AutoLock lock(mService->mWindowMapLock);
    return mService->mCurrentFocus.Get() == this;
}

String WindowState::MakeInputChannelName()
{
    StringBuilder sb(128);
    sb += StringUtils::ToHexString((Int32)this);
    sb += " ";
    AutoPtr<ICharSequence> cs;
    mAttrs->GetTitle((ICharSequence**)&cs);
    String aStr;
    cs->ToString(&aStr);
    sb += aStr;
    return sb.ToString();
}

ECode WindowState::ToString(
    /* [out]*/ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ICharSequence> title;
    mAttrs->GetTitle((ICharSequence**)&title);
    Int32 len;
    if (title == NULL || (title->GetLength(&len), len <= 0)) {
        String pkg;
        mAttrs->GetPackageName(&pkg);
        title = NULL;
        CString::New(pkg, (ICharSequence**)&title);
    }
    Boolean equals;
    if (mStringNameCache.IsNull() || !Object::Equals(mLastTitle, title) || mWasExiting != mExiting) {
        mLastTitle = title;
        String info;
        if (mLastTitle != NULL) {
            mLastTitle->ToString(&info);
        }
        mWasExiting = mExiting;
        StringBuilder sb("Window{");
        sb += StringUtils::ToHexString((Int32)this);
        sb += " userId=";
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 uid;
        helper->GetUserId(mSession->mUid, &uid);
        sb += uid;
        sb += " ";
        sb.Append(info.string());
        // sb += " token=";
        // sb += mToken ? mToken->mStringName : "NULL";
        // sb += " rootToken=";
        // sb += mRootToken ? mRootToken->mStringName : "NULL";
        sb += mExiting ? " EXITING}" : "}";
        mStringNameCache = sb.ToString();
    }
    *str = mStringNameCache;
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
