
#include "elastos/droid/server/wm/AccessibilityController.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/AppTransition.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::CRegionHelper;
using Elastos::Droid::Graphics::IRegionHelper;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::RegionOp_UNION;
using Elastos::Droid::Graphics::RegionOp_INTERSECT;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::Graphics::RegionOp_REVERSE_DIFFERENCE;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CMagnificationSpecHelper;
using Elastos::Droid::View::IMagnificationSpecHelper;
using Elastos::Droid::View::IMagnificationSpec;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IWindowInfoHelper;
using Elastos::Droid::View::CWindowInfoHelper;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController
//==============================================================================

const String AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::PROPERTY_NAME_ALPHA("alpha");
const Int32 AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::MIN_ALPHA;
const Int32 AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::MAX_ALPHA;
const Int32 AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::MSG_FRAME_SHOWN_STATE_CHANGED;

AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::AnimationController(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper,
    /* [in] */ ViewportWindow* host)
    : Handler(looper)
    , mHost(host)
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> objAnimator;
    AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(2);
    (*attrs)[0] = MIN_ALPHA;
    (*attrs)[1] = MAX_ALPHA;
    helper->OfInt32((IObject*)host, PROPERTY_NAME_ALPHA, attrs, (IObjectAnimator**)&objAnimator);
    mShowHideFrameAnimator = IValueAnimator::Probe(objAnimator);

    AutoPtr<ITimeInterpolator> interpolator;
    CDecelerateInterpolator::New(2.5, (ITimeInterpolator**)&interpolator);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 longAnimationDuration;
    res->GetInteger(Elastos::Droid::R::integer::config_longAnimTime, &longAnimationDuration);
    IAnimator::Probe(mShowHideFrameAnimator)->SetInterpolator(interpolator);
    mShowHideFrameAnimator->SetDuration((Int64)longAnimationDuration);
}


void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::OnFrameShownStateChanged(
    /* [in] */ Boolean shown,
    /* [in] */ Boolean animate)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_FRAME_SHOWN_STATE_CHANGED, shown ? 1 : 0, animate ? 1 : 0, (IMessage**)&msg);
    msg->SendToTarget();
}

ECode AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::AnimationController::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case MSG_FRAME_SHOWN_STATE_CHANGED: {
            Int32 arg1, arg2;
            message->GetArg1(&arg1);
            message->GetArg2(&arg2);
            Boolean shown = arg1 == 1;
            Boolean animate = arg2 == 1;

            AutoPtr<IAnimator> anim = IAnimator::Probe(mShowHideFrameAnimator);
            if (animate) {
                Boolean isRunning;
                if (anim->IsRunning(&isRunning), isRunning) {
                    mShowHideFrameAnimator->Reverse();
                }
                else {
                    if (shown) {
                        anim->Start();
                    }
                    else {
                        mShowHideFrameAnimator->Reverse();
                    }
                }
            }
            else {
                anim->Cancel();
                if (shown) {
                    mHost->SetAlpha(MAX_ALPHA);
                }
                else {
                    mHost->SetAlpha(MIN_ALPHA);
                }
            }
        } break;
    }
    return NOERROR;
}


//==============================================================================
//                  AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow
//==============================================================================

const String AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::SURFACE_TITLE("Magnification Overlay");

AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::ViewportWindow(
    /* [in] */ IContext* context,
    /* [in] */ MagnifiedViewport* host)
    : mShown(FALSE)
    , mAlpha(0)
    , mInvalidated(FALSE)
    , mHost(host)
{
    CRegion::New((IRegion**)&mBounds);
    CRect::New((IRect**)&mDirtyRect);
    CPaint::New((IPaint**)&mPaint);
    CSurface::New((ISurface**)&mSurface);

    // try {
    AutoPtr<IDisplay> display;
    mHost->mWindowManager->GetDefaultDisplay((IDisplay**)&display);
    display->GetRealSize(mHost->mTempPoint);
    Int32 x, y;
    mHost->mTempPoint->GetX(&x);
    mHost->mTempPoint->GetY(&y);
    CSurfaceControl::New(mHost->mHost->mWindowManagerService->mFxSession,
            SURFACE_TITLE, x, y, IPixelFormat::TRANSLUCENT,
            ISurfaceControl::HIDDEN, (ISurfaceControl**)&mSurfaceControl);
    // } catch (OutOfResourcesException oore) {
    //     /* ignore */
    // }
    Int32 stack;
    display->GetLayerStack(&stack);
    mSurfaceControl->SetLayerStack(stack);

    Int32 layer;
    mHost->mHost->mWindowManagerService->mPolicy->WindowTypeToLayerLw(
            IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY, &layer);
    mSurfaceControl->SetLayer(layer * CWindowManagerService::TYPE_LAYER_MULTIPLIER);
    mSurfaceControl->SetPosition(0, 0);
    mSurface->CopyFrom(mSurfaceControl);

    AutoPtr<ILooper> looper;
    mHost->mHost->mWindowManagerService->mH->GetLooper((ILooper**)&looper);
    mAnimationController = new AnimationController(context, looper, this);

    AutoPtr<ITypedValue> typedValue;
    CTypedValue::New((ITypedValue**)&typedValue);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    Boolean resolveResult;
    theme->ResolveAttribute(Elastos::Droid::R::attr::colorActivatedHighlight,
            typedValue, TRUE, &resolveResult);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 resourceId;
    typedValue->GetResourceId(&resourceId);
    Int32 borderColor;
    res->GetColor(resourceId, &borderColor);

    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetStrokeWidth(mHost->mBorderWidth);
    mPaint->SetColor(borderColor);

    mInvalidated = TRUE;
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::SetShown(
    /* [in] */ Boolean shown,
    /* [in] */ Boolean animate)
{
    synchronized(mHost->mHost->mWindowManagerService->mWindowMapLock) {
        if (mShown == shown) {
            return;
        }
        mShown = shown;
        mAnimationController->OnFrameShownStateChanged(shown, animate);
        if (DEBUG_VIEWPORT_WINDOW) {
            Slogger::I(TAG, "ViewportWindow shown: %d", mShown);
        }
    }
}

Int32 AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::GetAlpha()
{
    AutoLock lock(mHost->mHost->mWindowManagerService->mWindowMapLock);
    return mAlpha;
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::SetAlpha(
    /* [in] */ Int32 alpha)
{
    synchronized(mHost->mHost->mWindowManagerService->mWindowMapLock) {
        if (mAlpha == alpha) {
            return;
        }
        mAlpha = alpha;
        Invalidate(NULL);
        if (DEBUG_VIEWPORT_WINDOW) {
            Slogger::I(TAG, "ViewportWindow set alpha: %d", alpha);
        }
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::SetBounds(
    /* [in] */ IRegion* bounds)
{
    synchronized(mHost->mHost->mWindowManagerService->mWindowMapLock) {
        Boolean equals;
        if (IObject::Probe(mBounds)->Equals(bounds, &equals), equals) {
            return;
        }
        Boolean result;
        mBounds->Set(bounds, &result);
        Invalidate(mDirtyRect);
        if (DEBUG_VIEWPORT_WINDOW) {
            Slogger::I(TAG, "ViewportWindow set bounds: %p", bounds);
        }
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::UpdateSize()
{
    synchronized(mHost->mHost->mWindowManagerService->mWindowMapLock) {
        AutoPtr<IDisplay> display;
        mHost->mWindowManager->GetDefaultDisplay((IDisplay**)&display);
        display->GetRealSize(mHost->mTempPoint);
        Int32 x, y;
        mHost->mTempPoint->GetX(&x);
        mHost->mTempPoint->GetY(&y);
        mSurfaceControl->SetSize(x, y);
        Invalidate(mDirtyRect);
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::Invalidate(
    /* [in] */ IRect* dirtyRect)
{
    if (dirtyRect != NULL) {
        mDirtyRect->Set(dirtyRect);
    }
    else {
        mDirtyRect->SetEmpty();
    }
    mInvalidated = TRUE;
    mHost->mHost->mWindowManagerService->ScheduleAnimationLocked();
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::DrawIfNeeded()
{
    synchronized(mHost->mHost->mWindowManagerService->mWindowMapLock) {
        if (!mInvalidated) {
            return;
        }
        mInvalidated = FALSE;
        AutoPtr<ICanvas> canvas;
        // try {
        // Empty dirty rectangle means unspecified.
        Boolean isEmpty;
        if (mDirtyRect->IsEmpty(&isEmpty), isEmpty) {
            Boolean result;
            mBounds->GetBounds(mDirtyRect, &result);
        }
        mDirtyRect->Inset(-mHost->mHalfBorderWidth, -mHost->mHalfBorderWidth);
        mSurface->LockCanvas(mDirtyRect, (ICanvas**)&canvas);
        if (DEBUG_VIEWPORT_WINDOW) {
            Slogger::I(TAG, "Dirty rect: %p", mDirtyRect.Get());
        }
        // } catch (IllegalArgumentException iae) {
        //     /* ignore */
        // } catch (Surface.OutOfResourcesException oore) {
        //     /* ignore */
        // }
        if (canvas == NULL) {
            return;
        }
        if (DEBUG_VIEWPORT_WINDOW) {
            Slogger::I(TAG, "Bounds: %p", mBounds.Get());
        }
        canvas->DrawColor(IColor::TRANSPARENT, PorterDuffMode_CLEAR);
        mPaint->SetAlpha(mAlpha);
        AutoPtr<IPath> path;
        mBounds->GetBoundaryPath((IPath**)&path);
        canvas->DrawPath(path, mPaint);

        mSurface->UnlockCanvasAndPost(canvas);

        if (mAlpha > 0) {
            mSurfaceControl->Show();
        }
        else {
            mSurfaceControl->Hide();
        }
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::ViewportWindow::ReleaseSurface()
{
    mSurfaceControl->Release();
    mSurface->Release();
}


//==============================================================================
//                  AccessibilityController::DisplayMagnifier::MagnifiedViewport
//==============================================================================

const Int32 AccessibilityController::DisplayMagnifier::MagnifiedViewport::DEFAUTLT_BORDER_WIDTH_DIP;

AccessibilityController::DisplayMagnifier::MagnifiedViewport::MagnifiedViewport(
    /* [in] */ DisplayMagnifier* host)
    : mBorderWidth(0)
    , mHalfBorderWidth(0)
    , mDrawBorderInset(0)
    , mFullRedrawNeeded(FALSE)
    , mHost(host)
{
    CSparseArray::New((ISparseArray**)&mTempWindowStates);

    CRectF::New((IRectF**)&mTempRectF);
    CPoint::New((IPoint**)&mTempPoint);
    CMatrix::New((IMatrix**)&mTempMatrix);
    CRegion::New((IRegion**)&mMagnifiedBounds);
    CRegion::New((IRegion**)&mOldMagnifiedBounds);
    AutoPtr<IMagnificationSpecHelper> helper;
    CMagnificationSpecHelper::AcquireSingleton((IMagnificationSpecHelper**)&helper);
    helper->Obtain((IMagnificationSpec**)&mMagnificationSpec);

    AutoPtr<IInterface> service;
    host->mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    mWindowManager = IWindowManager::Probe(service);
    AutoPtr<IResources> res;
    host->mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    AutoPtr<ITypedValueHelper> tvHelper;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&tvHelper);
    tvHelper->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            DEFAUTLT_BORDER_WIDTH_DIP, metrics, &mBorderWidth);
    mHalfBorderWidth = (Int32)Elastos::Core::Math::Ceil(mBorderWidth / 2);
    mDrawBorderInset = (Int32) mBorderWidth / 2;
    mWindow = new ViewportWindow(host->mContext, this);
    RecomputeBoundsLocked();
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::UpdateMagnificationSpecLocked(
    /* [in] */ IMagnificationSpec* spec)
{
    if (spec != NULL) {
        Float scale, offsetX, offsetY;
        spec->GetScale(&scale);
        spec->GetOffsetX(&offsetX);
        spec->GetOffsetY(&offsetY);
        mMagnificationSpec->Initialize(scale, offsetX, offsetY);
    }
    else {
        mMagnificationSpec->Clear();
    }
    // If this message is pending we are in a rotation animation and do not want
    // to show the border. We will do so when the pending message is handled.
    Boolean hasMessages;
    if (mHost->mHandler->HasMessages(
            MyHandler::MESSAGE_SHOW_MAGNIFIED_REGION_BOUNDS_IF_NEEDED, &hasMessages), !hasMessages) {
        SetMagnifiedRegionBorderShownLocked(IsMagnifyingLocked(), TRUE);
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::RecomputeBoundsLocked()
{
    AutoPtr<IDisplay> display;
    mWindowManager->GetDefaultDisplay((IDisplay**)&display);
    display->GetRealSize(mTempPoint);
    Int32 screenWidth, screenHeight;
    mTempPoint->GetX(&screenWidth);
    mTempPoint->GetY(&screenHeight);

    Boolean result;
    AutoPtr<IRegion> magnifiedBounds = mMagnifiedBounds;
    magnifiedBounds->Set(0, 0, 0, 0, &result);

    AutoPtr<IRegion> availableBounds = mHost->mTempRegion1;
    availableBounds->Set(0, 0, screenWidth, screenHeight, &result);

    AutoPtr<IRegion> nonMagnifiedBounds = mHost->mTempRegion4;
    nonMagnifiedBounds->Set(0, 0, 0, 0, &result);

    AutoPtr<ISparseArray> visibleWindows = mTempWindowStates;
    visibleWindows.Clear();
    PopulateWindowsOnScreenLocked(visibleWindows);

    Int32 visibleWindowCount;
    visibleWindows->GetSize(&visibleWindowCount);
    for (Int32 i = visibleWindowCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> value;
        visibleWindows->ValueAt(i, (IInterface**)&value);
        AutoPtr<WindowState> windowState = (WindowState*)IWindowState::Probe(value);
        Int32 winType;
        if (windowState->mAttrs->GetType(&winType), winType == IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY) {
            continue;
        }

        AutoPtr<IRegion> windowBounds = mHost->mTempRegion2;
        AutoPtr<IMatrix> matrix = mTempMatrix;
        PopulateTransformationMatrixLocked(windowState, matrix);
        AutoPtr<IRectF> windowFrame = mTempRectF;

        Boolean canMagnifyWindow;
        if (mHost->mWindowManagerService->mPolicy->CanMagnifyWindow(winType, &canMagnifyWindow), canMagnifyWindow) {
            windowFrame->Set(windowState->mFrame);
            Float left, top;
            windowFrame->GetLeft(&left);
            windowFrame->GetTop(&top);
            windowFrame->Offset(-left, -top);
            matrix->MapRect(windowFrame, &result);
            Float right, bottom;
            windowFrame->GetRight(&right);
            windowFrame->GetBottom(&bottom);
            windowBounds->Set((Int32)left, (Int32)top, (Int32)right, (Int32)bottom, &result);
            magnifiedBounds->Op(windowBounds, RegionOp_UNION, &result);
            magnifiedBounds->Op(availableBounds, RegionOp_INTERSECT, &result);
        }
        else {
            AutoPtr<IRegion> touchableRegion = mHost->mTempRegion3;
            windowState->GetTouchableRegion(touchableRegion);
            AutoPtr<IRect> touchableFrame = mHost->mTempRect1;
            touchableRegion->GetBounds(touchableFrame, &result);
            windowFrame->Set(touchableFrame);
            Int32 left, top;
            windowState->mFrame->GetLeft(&left);
            windowState->mFrame->GetTop(&top);
            windowFrame->Offset(-left, -top);
            matrix->MapRect(windowFrame, &result);
            Float winLeft, winTop, winRight, winBottom;
            windowFrame->GetLeft(&winLeft);
            windowFrame->GetTop(&winTop);
            windowFrame->GetRight(&winRight);
            windowFrame->GetBottom(&winBottom);
            windowBounds->Set((Int32)winLeft, (Int32)winTop, (Int32)winRight, (Int32)winBottom, &result);
            nonMagnifiedBounds->Op(windowBounds, RegionOp_UNION, &result);
            windowBounds->Op(magnifiedBounds, RegionOp_DIFFERENCE, &result);
            availableBounds->Op(windowBounds, RegionOp_DIFFERENCE, &result);
        }

        AutoPtr<IRegion> accountedBounds = mHost->mTempRegion2;
        accountedBounds->Set(magnifiedBounds, &result);
        accountedBounds->Op(nonMagnifiedBounds, RegionOp_UNION, &result);
        accountedBounds->Op(0, 0, screenWidth, screenHeight, RegionOp_INTERSECT, &result);

        Boolean isRect;
        if (accountedBounds->IsRect(&isRect), isRect) {
            AutoPtr<IRect> accountedFrame = mHost->mTempRect1;
            accountedBounds->GetBounds(accountedFrame, &result);
            Int32 w, h;
            if ((accountedFrame->GetWidth(&w), w == screenWidth)
                    && (accountedFrame->GetHeight(&h), h == screenHeight)) {
                break;
            }
        }
    }

    visibleWindows->Clear();

    magnifiedBounds->Op(mDrawBorderInset, mDrawBorderInset,
            screenWidth - mDrawBorderInset, screenHeight - mDrawBorderInset,
            RegionOp_INTERSECT, &result);

    Boolean equals;
    if (IObject::Probe(mOldMagnifiedBounds)->Equals(magnifiedBounds, &equals), !equals) {
        AutoPtr<IRegionHelper> regionH;
        CRegionHelper::AcquireSingleton((IRegionHelper**)&regionH);
        AutoPtr<IRegion> bounds;
        regionH->Obtain((IRegion**)&bounds);
        bounds->Set(magnifiedBounds, &result);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MyHandler::MESSAGE_NOTIFY_MAGNIFIED_BOUNDS_CHANGED,
                bounds, (IMessage**)&msg);
        msg->SendToTarget();

        mWindow->SetBounds(magnifiedBounds);
        AutoPtr<IRect> dirtyRect = mHost->mTempRect1;
        if (mFullRedrawNeeded) {
            mFullRedrawNeeded = FALSE;
            dirtyRect->Set(mDrawBorderInset, mDrawBorderInset,
                    screenWidth - mDrawBorderInset, screenHeight - mDrawBorderInset);
            mWindow->Invalidate(dirtyRect);
        }
        else {
            AutoPtr<IRegion> dirtyRegion = mHost->mTempRegion3;
            dirtyRegion->Set(magnifiedBounds, &result);
            dirtyRegion->Op(mOldMagnifiedBounds, RegionOp_UNION, &result);
            dirtyRegion->Op(nonMagnifiedBounds, RegionOp_INTERSECT, &result);
            dirtyRegion->GetBounds(dirtyRect, &result);
            mWindow->Invalidate(dirtyRect);
        }

        mOldMagnifiedBounds->Set(magnifiedBounds, &result);
    }
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::OnRotationChangedLocked()
{
    // If we are magnifying, hide the magnified border window immediately so
    // the user does not see strange artifacts during rotation. The screenshot
    // used for rotation has already the border. After the rotation is complete
    // we will show the border.
    if (IsMagnifyingLocked()) {
        SetMagnifiedRegionBorderShownLocked(FALSE, FALSE);
        Int64 delay = mHost->mInt64AnimationDuration
                * mHost->mWindowManagerService->GetWindowAnimationScaleLocked();
        AutoPtr<IMessage> message;
        mHost->mHandler->ObtainMessage(
                MyHandler::MESSAGE_SHOW_MAGNIFIED_REGION_BOUNDS_IF_NEEDED, (IMessage**)&message);
        Boolean result;
        mHost->mHandler->SendMessageDelayed(message, delay, &result);
    }
    RecomputeBoundsLocked();
    mWindow->UpdateSize();
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::SetMagnifiedRegionBorderShownLocked(
    /* [in] */ Boolean shown,
    /* [in] */ Boolean animate)
{
    if (shown) {
        mFullRedrawNeeded = TRUE;
        Boolean result;
        mOldMagnifiedBounds->Set(0, 0, 0, 0, &result);
    }
    mWindow->SetShown(shown, animate);
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::GetMagnifiedFrameInContentCoordsLocked(
    /* [in] */ IRect* rect)
{
    AutoPtr<IMagnificationSpec> spec = mMagnificationSpec;
    Boolean result;
    mMagnifiedBounds->GetBounds(rect, &result);
    Float offsetX, offsetY;
    spec->GetOffsetX(&offsetX);
    spec->GetOffsetY(&offsetY);
    rect->Offset((Int32)-offsetX, (Int32)-offsetY);
    Float scale;
    spec->GetScale(&scale);
    rect->Scale(1.0f / scale);
}

Boolean AccessibilityController::DisplayMagnifier::MagnifiedViewport::IsMagnifyingLocked()
{
    Float scale;
    mMagnificationSpec->GetScale(&scale);
    return scale > 1.0f;
}

AutoPtr<IMagnificationSpec> AccessibilityController::DisplayMagnifier::MagnifiedViewport::GetMagnificationSpecLocked()
{
    return mMagnificationSpec;
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::DrawWindowIfNeededLocked()
{
    RecomputeBoundsLocked();
    mWindow->DrawIfNeeded();
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::DestroyWindow()
{
    mWindow->ReleaseSurface();
}

void AccessibilityController::DisplayMagnifier::MagnifiedViewport::PopulateWindowsOnScreenLocked(
    /* [in] */ ISparseArray* outWindows)
{
    AutoPtr<DisplayContent> displayContent = mHost->mWindowManagerService->GetDefaultDisplayContentLocked();
    AutoPtr<List<AutoPtr<WindowState> > > windowList = displayContent->GetWindowList();
    List<AutoPtr<WindowState> >::Iterator it = windowList->Begin();
    for (; it != windowList->End(); ++it) {
        AutoPtr<WindowState> windowState = *it;
        Int32 type;
        if ((windowState->IsOnScreen() ||
                (windowState->mAttrs->GetType(&type), type == IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND))
                && !windowState->mWinAnimator->mEnterAnimationPending) {
            outWindows->Put(windowState->mLayer, (IWindowState*)windowState.Get());
        }
    }
}


//==============================================================================
//                  AccessibilityController::DisplayMagnifier::MyHandler
//==============================================================================

const Int32 AccessibilityController::DisplayMagnifier::MyHandler::MESSAGE_NOTIFY_MAGNIFIED_BOUNDS_CHANGED;
const Int32 AccessibilityController::DisplayMagnifier::MyHandler::MESSAGE_NOTIFY_RECTANGLE_ON_SCREEN_REQUESTED;
const Int32 AccessibilityController::DisplayMagnifier::MyHandler::MESSAGE_NOTIFY_USER_CONTEXT_CHANGED;
const Int32 AccessibilityController::DisplayMagnifier::MyHandler::MESSAGE_NOTIFY_ROTATION_CHANGED;
const Int32 AccessibilityController::DisplayMagnifier::MyHandler::MESSAGE_SHOW_MAGNIFIED_REGION_BOUNDS_IF_NEEDED;

AccessibilityController::DisplayMagnifier::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ DisplayMagnifier* host)
    : Handler(looper)
    , mHost(host)
{}

ECode AccessibilityController::DisplayMagnifier::MyHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case MESSAGE_NOTIFY_MAGNIFIED_BOUNDS_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IRegion> bounds = IRegion::Probe(obj);
            mHost->mCallbacks->OnMagnifedBoundsChanged(bounds);
            bounds->Recycle();
        } break;

        case MESSAGE_NOTIFY_RECTANGLE_ON_SCREEN_REQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            Int32 left, top, right, bottom;
            args->GetInt32Arg(1, &left);
            args->GetInt32Arg(2, &top);
            args->GetInt32Arg(3, &right);
            args->GetInt32Arg(4, &bottom);
            mHost->mCallbacks->OnRectangleOnScreenRequested(left, top, right, bottom);
            args->Recycle();
        } break;

        case MESSAGE_NOTIFY_USER_CONTEXT_CHANGED: {
            mHost->mCallbacks->OnUserContextChanged();
        } break;

        case MESSAGE_NOTIFY_ROTATION_CHANGED: {
            Int32 rotation;
            message->GetArg1(&rotation);
            mHost->mCallbacks->OnRotationChanged(rotation);
        } break;

        case MESSAGE_SHOW_MAGNIFIED_REGION_BOUNDS_IF_NEEDED : {
            synchronized(mHost->mWindowManagerService->mWindowMapLock) {
                if (mHost->mMagnifedViewport->IsMagnifyingLocked()) {
                    mHost->mMagnifedViewport->SetMagnifiedRegionBorderShownLocked(TRUE, TRUE);
                    mHost->mWindowManagerService->ScheduleAnimationLocked();
                }
            }
        } break;
    }
    return NOERROR;
}


//==============================================================================
//                  AccessibilityController::DisplayMagnifier
//==============================================================================

const String AccessibilityController::DisplayMagnifier::TAG("DisplayMagnifier");
const Boolean AccessibilityController::DisplayMagnifier::DEBUG_WINDOW_TRANSITIONS;
const Boolean AccessibilityController::DisplayMagnifier::DEBUG_ROTATION;
const Boolean AccessibilityController::DisplayMagnifier::DEBUG_LAYERS;
const Boolean AccessibilityController::DisplayMagnifier::DEBUG_RECTANGLE_REQUESTED;
const Boolean AccessibilityController::DisplayMagnifier::DEBUG_VIEWPORT_WINDOW;

AccessibilityController::DisplayMagnifier::DisplayMagnifier(
    /* [in] */ CWindowManagerService* windowManagerService,
    /* [in] */ IMagnificationCallbacks* callbacks)
    : mInt64AnimationDuration(0)
{
    CRect::New((IRect**)&mTempRect1);
    CRect::New((IRect**)&mTempRect2);
    CRegion::New((IRegion**)&mTempRegion1);
    CRegion::New((IRegion**)&mTempRegion2);
    CRegion::New((IRegion**)&mTempRegion3);
    CRegion::New((IRegion**)&mTempRegion4);

    mContext = windowManagerService->mContext;
    mWindowManagerService = windowManagerService;
    mCallbacks = callbacks;
    AutoPtr<ILooper> l;
    mWindowManagerService->mH->GetLooper((ILooper**)&l);
    mHandler = new MyHandler(l, this);
    mMagnifedViewport = new MagnifiedViewport(this);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 value;
    res->GetInteger(Elastos::Droid::R::integer::config_longAnimTime, &value);
    mInt64AnimationDuration = (Int64)value;
}

void AccessibilityController::DisplayMagnifier::SetMagnificationSpecLocked(
    /* [in] */ IMagnificationSpec* spec)
{
    mMagnifedViewport->UpdateMagnificationSpecLocked(spec);
    mMagnifedViewport->RecomputeBoundsLocked();
    mWindowManagerService->ScheduleAnimationLocked();
}

void AccessibilityController::DisplayMagnifier::OnRectangleOnScreenRequestedLocked(
    /* [in] */ IRect* rectangle)
{
    if (DEBUG_RECTANGLE_REQUESTED) {
        Slogger::I(TAG, "Rectangle on screen requested: %p", rectangle);
    }
    if (!mMagnifedViewport->IsMagnifyingLocked()) {
        return;
    }
    AutoPtr<IRect> magnifiedRegionBounds = mTempRect2;
    mMagnifedViewport->GetMagnifiedFrameInContentCoordsLocked(magnifiedRegionBounds);
    Boolean contains;
    if (magnifiedRegionBounds->Contains(rectangle, &contains), contains) {
        return;
    }

    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    Int32 left, top, right, bottom;
    rectangle->GetLeft(&left);
    rectangle->GetTop(&top);
    rectangle->GetRight(&right);
    rectangle->GetBottom(&bottom);
    args->SetInt32Arg(1, left);
    args->SetInt32Arg(2, top);
    args->SetInt32Arg(3, right);
    args->SetInt32Arg(4, bottom);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MyHandler::MESSAGE_NOTIFY_RECTANGLE_ON_SCREEN_REQUESTED, args, (IMessage**)&msg);
    msg->SendToTarget();
}

void AccessibilityController::DisplayMagnifier::OnWindowLayersChangedLocked()
{
    if (DEBUG_LAYERS) {
        Slogger::I(TAG, "Layers changed.");
    }
    mMagnifedViewport->RecomputeBoundsLocked();
    mWindowManagerService->ScheduleAnimationLocked();
}

void AccessibilityController::DisplayMagnifier::OnRotationChangedLocked(
    /* [in] */ DisplayContent* displayContent,
    /* [in] */ Int32 rotation)
{
    // if (DEBUG_ROTATION) {
    //     Slog.i(TAG, "Rotaton: " + Surface.rotationToString(rotation)
    //             + " displayId: " + displayContent.getDisplayId());
    // }
    mMagnifedViewport->OnRotationChangedLocked();
    Boolean result;
    mHandler->SendEmptyMessage(MyHandler::MESSAGE_NOTIFY_ROTATION_CHANGED, &result);
}

void AccessibilityController::DisplayMagnifier::OnAppWindowTransitionLocked(
    /* [in] */ WindowState* windowState,
    /* [in] */ Int32 transition)
{
    if (DEBUG_WINDOW_TRANSITIONS) {
        Slogger::I(TAG, "Window transition: %s displayId: %d",
                AppTransition::AppTransitionToString(transition).string(), windowState->GetDisplayId());
    }
    Boolean magnifying = mMagnifedViewport->IsMagnifyingLocked();
    if (magnifying) {
        switch (transition) {
            case AppTransition::TRANSIT_ACTIVITY_OPEN:
            case AppTransition::TRANSIT_TASK_OPEN:
            case AppTransition::TRANSIT_TASK_TO_FRONT:
            case AppTransition::TRANSIT_WALLPAPER_OPEN:
            case AppTransition::TRANSIT_WALLPAPER_CLOSE:
            case AppTransition::TRANSIT_WALLPAPER_INTRA_OPEN: {
                Boolean result;
                mHandler->SendEmptyMessage(MyHandler::MESSAGE_NOTIFY_USER_CONTEXT_CHANGED, &result);
            }
        }
    }
}

void AccessibilityController::DisplayMagnifier::OnWindowTransitionLocked(
    /* [in] */ WindowState* windowState,
    /* [in] */ Int32 transition)
{
    if (DEBUG_WINDOW_TRANSITIONS) {
        Slogger::I(TAG, "Window transition: %s displayId: %d",
                AppTransition::AppTransitionToString(transition).string(), windowState->GetDisplayId());
    }
    Boolean magnifying = mMagnifedViewport->IsMagnifyingLocked();
    Int32 type;
    windowState->mAttrs->GetType(&type);
    switch (transition) {
        case IWindowManagerPolicy::TRANSIT_ENTER:
        case IWindowManagerPolicy::TRANSIT_SHOW: {
            if (!magnifying) {
                break;
            }
            switch (type) {
                case IWindowManagerLayoutParams::TYPE_APPLICATION:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG:
                case IWindowManagerLayoutParams::TYPE_SEARCH_BAR:
                case IWindowManagerLayoutParams::TYPE_PHONE:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
                case IWindowManagerLayoutParams::TYPE_TOAST:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
                case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
                case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
                case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
                case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
                case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY: {
                    AutoPtr<IRect> magnifiedRegionBounds = mTempRect2;
                    mMagnifedViewport->GetMagnifiedFrameInContentCoordsLocked(
                            magnifiedRegionBounds);
                    AutoPtr<IRect> touchableRegionBounds = mTempRect1;
                    windowState->GetTouchableRegion(mTempRegion1);
                    Boolean result;
                    mTempRegion1->GetBounds(touchableRegionBounds, &result);
                    if (magnifiedRegionBounds->Intersect(touchableRegionBounds, &result), !result) {
                        Int32 left, top, right, bottom;
                        touchableRegionBounds->GetLeft(&left);
                        touchableRegionBounds->GetTop(&top);
                        touchableRegionBounds->GetRight(&right);
                        touchableRegionBounds->GetBottom(&bottom);
                        mCallbacks->OnRectangleOnScreenRequested(left, top, right, bottom);
                    }
                } break;
            } break;
        }
    }
}

AutoPtr<IMagnificationSpec> AccessibilityController::DisplayMagnifier::GetMagnificationSpecForWindowLocked(
    /* [in] */ WindowState* windowState)
{
    AutoPtr<IMagnificationSpec> spec = mMagnifedViewport->GetMagnificationSpecLocked();
    Boolean isNop;
    if (spec != NULL && (spec->IsNop(&isNop), !isNop)) {
        AutoPtr<IWindowManagerPolicy> policy = mWindowManagerService->mPolicy;
        Int32 windowType;
        windowState->mAttrs->GetType(&windowType);
        Boolean isTopLevelWindow, canMagnifyWindow;
        if ((policy->IsTopLevelWindow(windowType, &isTopLevelWindow), !isTopLevelWindow)
                && windowState->mAttachedWindow != NULL && (policy->CanMagnifyWindow(windowType, &canMagnifyWindow), !canMagnifyWindow)) {
            return NULL;
        }
        if (policy->CanMagnifyWindow(windowType, &canMagnifyWindow), !canMagnifyWindow) {
            return NULL;
        }
    }
    return spec;
}

void AccessibilityController::DisplayMagnifier::DestroyLocked()
{
    mMagnifedViewport->DestroyWindow();
}

void AccessibilityController::DisplayMagnifier::DrawMagnifiedRegionBorderIfNeededLocked()
{
    mMagnifedViewport->DrawWindowIfNeededLocked();
}


//==============================================================================
//                  AccessibilityController::WindowsForAccessibilityObserver::MyHandler
//==============================================================================

const Int32 AccessibilityController::WindowsForAccessibilityObserver::MyHandler::MESSAGE_COMPUTE_CHANGED_WINDOWS;
const Int32 AccessibilityController::WindowsForAccessibilityObserver::MyHandler::MESSAGE_NOTIFY_WINDOWS_CHANGED;

AccessibilityController::WindowsForAccessibilityObserver::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ WindowsForAccessibilityObserver* host)
    : Handler(looper, NULL, FALSE)
    , mHost(host)
{}

ECode AccessibilityController::WindowsForAccessibilityObserver::MyHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case MESSAGE_COMPUTE_CHANGED_WINDOWS: {
            mHost->ComputeChangedWindows();
        } break;

        case MESSAGE_NOTIFY_WINDOWS_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IList> windows = IList::Probe(obj);
            mHost->mCallback->OnWindowsForAccessibilityChanged(windows);
            mHost->ClearAndRecycleWindows(windows);
        } break;
    }
    return NOERROR;
}


//==============================================================================
//                  AccessibilityController::WindowsForAccessibilityObserver
//==============================================================================

const String AccessibilityController::WindowsForAccessibilityObserver::TAG("WindowsForAccessibilityObserver");

const Boolean AccessibilityController::WindowsForAccessibilityObserver::DEBUG;

AccessibilityController::WindowsForAccessibilityObserver::WindowsForAccessibilityObserver(
    /* [in] */ CWindowManagerService* windowManagerService,
    /* [in] */ IWindowsForAccessibilityCallback* callback)
    : mRecurringAccessibilityEventsIntervalMillis(0)
{
    CRectF::New((IRectF**)&mTempRectF);
    CMatrix::New((IMatrix**)&mTempMatrix);
    CPoint::New((IPoint**)&mTempPoint);
    CRect::New((IRect**)&mTempRect);
    CRegion::New((IRegion**)&mTempRegion);
    CRegion::New((IRegion**)&mTempRegion1);

    mContext = windowManagerService->mContext;
    mWindowManagerService = windowManagerService;
    mCallback = callback;
    AutoPtr<ILooper> l;
    mWindowManagerService->mH->GetLooper((ILooper**)&l);
    mHandler = new MyHandler(l, this);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->GetSendRecurringAccessibilityEventsInterval(&mRecurringAccessibilityEventsIntervalMillis);
    ComputeChangedWindows();
}

void AccessibilityController::WindowsForAccessibilityObserver::ScheduleComputeChangedWindowsLocked()
{
    // If focus changed, compute changed windows immediately as the focused window
    // is used by the accessibility manager service to determine the active window.
    Boolean hasMessages;
    if (mWindowManagerService->mCurrentFocus != NULL
            && mWindowManagerService->mCurrentFocus != mWindowManagerService->mLastFocus) {
        mHandler->RemoveMessages(MyHandler::MESSAGE_COMPUTE_CHANGED_WINDOWS);
        ComputeChangedWindows();
    }
    else if (mHandler->HasMessages(MyHandler::MESSAGE_COMPUTE_CHANGED_WINDOWS, &hasMessages), !hasMessages) {
        Boolean result;
        mHandler->SendEmptyMessageDelayed(MyHandler::MESSAGE_COMPUTE_CHANGED_WINDOWS,
                mRecurringAccessibilityEventsIntervalMillis, &result);
    }
}

void AccessibilityController::WindowsForAccessibilityObserver::ComputeChangedWindows()
{
    if (DEBUG) {
        Slogger::I(TAG, "computeChangedWindows()");
    }

    synchronized(mWindowManagerService->mWindowMapLock) {
        // Do not send the windows if there is no current focus as
        // the window manager is still looking for where to put it.
        // We will do the work when we get a focus change callback.
        if (mWindowManagerService->mCurrentFocus == NULL) {
            return;
        }

        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        AutoPtr<IWindowManager> windowManager = IWindowManager::Probe(service);
        AutoPtr<IDisplay> display;
        windowManager->GetDefaultDisplay((IDisplay**)&display);
        display->GetRealSize(mTempPoint);
        Int32 screenWidth, screenHeight;
        mTempPoint->GetX(&screenWidth);
        mTempPoint->GetY(&screenHeight);

        AutoPtr<IRegion> unaccountedSpace = mTempRegion;
        Boolean result;
        unaccountedSpace->Set(0, 0, screenWidth, screenHeight, &result);

        AutoPtr<ISparseArray> visibleWindows = mTempWindowStates;
        PopulateVisibleWindowsOnScreenLocked(visibleWindows);

        AutoPtr<IList> windows;
        CArrayList::New((IList**)&windows);

        Set<AutoPtr<IBinder> > addedWindows = mTempBinderSet;
        addedWindows.Clear();

        Boolean focusedWindowAdded = FALSE;

        Int32 visibleWindowCount;
        visibleWindows->GetSize(&visibleWindowCount);
        for (Int32 i = visibleWindowCount - 1; i >= 0; i--) {
            AutoPtr<IInterface> value;
            visibleWindows->ValueAt(i, (IInterface**)&value);
            AutoPtr<WindowState> windowState = (WindowState*)IWindowState::Probe(value);

            // Compute the bounds in the screen.
            AutoPtr<IRect> boundsInScreen = mTempRect;
            ComputeWindowBoundsInScreen(windowState, boundsInScreen);

            Int32 flags;
            windowState->mAttrs->GetFlags(&flags);

            // If the window is not touchable, do not report it but take into account
            // the space it takes since the content behind it cannot be touched.
            if ((flags & IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE) != 0) {
                continue;
            }

            // If the window is completely covered by other windows - ignore.
            Boolean result;
            if (unaccountedSpace->QuickReject(boundsInScreen, &result), result) {
                continue;
            }

            // Add windows of certain types not covered by modal windows.
            Int32 type;
            windowState->mAttrs->GetType(&type);
            if (IsReportedWindowType(type)) {
                // Add the window to the ones to be reported.
                AutoPtr<IWindowInfo> window = ObtainPopulatedWindowInfo(windowState, boundsInScreen);
                AutoPtr<IBinder> token;
                window->GetToken((IBinder**)&token);
                addedWindows.Insert(token);
                if (windowState->IsFocused()) {
                    focusedWindowAdded = TRUE;
                }
            }

            // Account for the space this window takes.
            unaccountedSpace->Op(boundsInScreen, unaccountedSpace,
                    RegionOp_REVERSE_DIFFERENCE, &result);

            // We figured out what is touchable for the entire screen - done.
            Boolean isEmpty;
            if (unaccountedSpace->IsEmpty(&isEmpty), isEmpty) {
                break;
            }

            // If a window is modal, no other below can be touched - done.
            if ((flags & (IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL)) == 0) {
                break;
            }
        }

        // Always report the focused window.
        if (!focusedWindowAdded) {
            for (Int32 i = visibleWindowCount - 1; i >= 0; i--) {
                AutoPtr<IInterface> value;
                visibleWindows->ValueAt(i, (IInterface**)&value);
                AutoPtr<WindowState> windowState = (WindowState*)IWindowState::Probe(value);
                if (windowState->IsFocused()) {
                    // Compute the bounds in the screen.
                    AutoPtr<IRect> boundsInScreen = mTempRect;
                    ComputeWindowBoundsInScreen(windowState, boundsInScreen);

                    // Add the window to the ones to be reported.
                    AutoPtr<IWindowInfo> window = ObtainPopulatedWindowInfo(windowState,
                            boundsInScreen);
                    AutoPtr<IBinder> token;
                    window->GetToken((IBinder**)&token);
                    addedWindows.Insert(token);
                    windows->Add(window);
                    break;
                }
            }
        }

        // Remove child/parent references to windows that were not added.
        AutoPtr<IIterator> it;
        windows->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<IWindowInfo> window = IWindowInfo::Probe(next);
            AutoPtr<IBinder> parentToken;
            window->GetParentToken((IBinder**)&parentToken);
            if (addedWindows.Find(parentToken) == addedWindows.End()) {
                window->SetParentToken(NULL);
            }
            AutoPtr<IList> childTokens;
            window->GetChildTokens((IList**)&childTokens);
            if (childTokens != NULL) {
                Int32 childTokenCount;
                childTokens->GetSize(&childTokenCount);
                for (Int32 j = childTokenCount - 1; j >= 0; j--) {
                    AutoPtr<IInterface> v;
                    childTokens->Get(j, (IInterface**)&v);
                    AutoPtr<IBinder> b = IBinder::Probe(v);
                    if (addedWindows.Find(b) == addedWindows.End()) {
                        childTokens->Remove(j);
                    }
                }
                // Leave the child token list if empty.
            }
        }

        visibleWindows->Clear();
        addedWindows.Clear();

        // We computed the windows and if they changed notify the client.
        Boolean windowsChanged = FALSE;
        Int32 size;
        Boolean isEmpty;
        if (windows->GetSize(&size), (Int32)mOldWindows.GetSize() != size) {
            // Different size means something changed.
            windowsChanged = TRUE;
        }
        else if ((mOldWindows.Begin() != mOldWindows.End()) || (windows->IsEmpty(&isEmpty), !isEmpty)) {
            // Since we always traverse windows from high to low layer
            // the old and new windows at the same index should be the
            // same, otherwise something changed.
            List<AutoPtr<IWindowInfo> >::Iterator oldIt = mOldWindows.Begin();
            AutoPtr<IIterator> winIt;
            windows->GetIterator((IIterator**)&winIt);
            while (winIt->HasNext(&hasNext), hasNext) {
                AutoPtr<IWindowInfo> oldWindow = *oldIt;
                AutoPtr<IInterface> next;
                winIt->GetNext((IInterface**)&next);
                AutoPtr<IWindowInfo> newWindow = IWindowInfo::Probe(next);

                // We do not care for layer changes given the window
                // order does not change. This brings no new information
                // to the clients.
                if (WindowChangedNoLayer(oldWindow, newWindow)) {
                    windowsChanged = TRUE;
                    break;
                }
                ++oldIt;
            }
        }

        if (windowsChanged) {
            if (DEBUG) {
                Slogger::I(TAG, "Windows changed:%p", windows.Get());
            }
            // Remember the old windows to detect changes.
            CacheWindows(windows);
            // Announce the change.
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MyHandler::MESSAGE_NOTIFY_WINDOWS_CHANGED,
                    windows, (IMessage**)&msg);
            msg->SendToTarget();
        }
        else {
            if (DEBUG) {
                Slogger::I(TAG, "No windows changed.");
            }
            // Recycle the nodes as we do not need them.
            ClearAndRecycleWindows(windows);
        }
    }
}

void AccessibilityController::WindowsForAccessibilityObserver::ComputeWindowBoundsInScreen(
    /* [in] */ WindowState* windowState,
    /* [in] */ IRect* outBounds)
{
    // Get the touchable frame.
    AutoPtr<IRegion> touchableRegion = mTempRegion1;
    windowState->GetTouchableRegion(touchableRegion);
    AutoPtr<IRect> touchableFrame = mTempRect;
    Boolean result;
    touchableRegion->GetBounds(touchableFrame, &result);

    // Move to origin as all transforms are captured by the matrix.
    AutoPtr<IRectF> windowFrame = mTempRectF;
    windowFrame->Set(touchableFrame);
    Int32 left, top;
    windowState->mFrame->GetLeft(&left);
    windowState->mFrame->GetTop(&top);
    windowFrame->Offset(-left, -top);

    // Map the frame to get what appears on the screen.
    AutoPtr<IMatrix> matrix = mTempMatrix;
    PopulateTransformationMatrixLocked(windowState, matrix);
    matrix->MapRect(windowFrame, &result);

    // Got the bounds.
    Float leftF, topF, rightF, bottomF;
    windowFrame->GetLeft(&leftF);
    windowFrame->GetTop(&topF);
    windowFrame->GetRight(&rightF);
    windowFrame->GetBottom(&bottomF);
    outBounds->Set((Int32)leftF, (Int32)topF, (Int32)rightF, (Int32)bottomF);
}

AutoPtr<IWindowInfo> AccessibilityController::WindowsForAccessibilityObserver::ObtainPopulatedWindowInfo(
    /* [in] */ WindowState* windowState,
    /* [in] */ IRect* boundsInScreen)
{
    AutoPtr<IWindowInfoHelper> helper;
    CWindowInfoHelper::AcquireSingleton((IWindowInfoHelper**)&helper);
    AutoPtr<IWindowInfo> window;
    helper->Obtain((IWindowInfo**)&window);
    Int32 type;
    windowState->mAttrs->GetType(&type);
    window->SetType(type);
    window->SetLayer(windowState->mLayer);
    window->SetToken(IBinder::Probe(windowState->mClient));

    AutoPtr<WindowState> attachedWindow = windowState->mAttachedWindow;
    if (attachedWindow != NULL) {
        window->SetParentToken(IBinder::Probe(windowState->mClient));
    }

    window->SetFocused(windowState->IsFocused());
    AutoPtr<IRect> winBoundsInScreen;
    window->GetBoundsInScreen((IRect**)&winBoundsInScreen);
    winBoundsInScreen->Set(boundsInScreen);

    if (windowState->mChildWindows.Begin() != windowState->mChildWindows.End()) {
        AutoPtr<IList> childTokens;
        window->GetChildTokens((IList**)&childTokens);
        if (childTokens == NULL) {
            CArrayList::New((IList**)&childTokens);
            window->SetChildTokens(childTokens);
        }
        List<AutoPtr<WindowState> >::Iterator it = windowState->mChildWindows.Begin();
        for (; it != windowState->mChildWindows.End(); ++it) {
            AutoPtr<WindowState> child = *it;
            childTokens->Add(IBinder::Probe(child->mClient));
        }
    }

    return window;
}

void AccessibilityController::WindowsForAccessibilityObserver::CacheWindows(
    /* [in] */ IList* windows)
{
    List<AutoPtr<IWindowInfo> >::Iterator it = mOldWindows.Begin();
    while (it != mOldWindows.End()) {
        (*it)->Recycle();
        it = mOldWindows.Erase(it);
    }
    AutoPtr<IWindowInfoHelper> helper;
    CWindowInfoHelper::AcquireSingleton((IWindowInfoHelper**)&helper);
    AutoPtr<IIterator> winit;
    windows->GetIterator((IIterator**)&winit);
    Boolean hasNext;
    while (winit->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        winit->GetNext((IInterface**)&value);
        AutoPtr<IWindowInfo> newWindow = IWindowInfo::Probe(value);
        AutoPtr<IWindowInfo> winInfo;
        helper->Obtain(newWindow, (IWindowInfo**)&winInfo);
        mOldWindows.PushBack(winInfo);
    }
}

Boolean AccessibilityController::WindowsForAccessibilityObserver::WindowChangedNoLayer(
    /* [in] */ IWindowInfo* oldWindow,
    /* [in] */ IWindowInfo* newWindow)
{
    if (oldWindow == newWindow) {
        return FALSE;
    }
    if (oldWindow == NULL) {
        return TRUE;
    }
    if (newWindow == NULL) {
        return TRUE;
    }
    Int32 oldType, newType;
    oldWindow->GetType(&oldType);
    newWindow->GetType(&newType);
    if (oldType != newType) {
        return TRUE;
    }
    Boolean oldFocus, newFocus;
    oldWindow->GetFocused(&oldFocus);
    oldWindow->GetFocused(&newFocus);
    if (oldFocus != newFocus) {
        return TRUE;
    }
    AutoPtr<IBinder> oldToken, newToken;
    oldWindow->GetToken((IBinder**)&oldToken);
    newWindow->GetToken((IBinder**)&newToken);
    Boolean equals;
    if (oldToken == NULL) {
        if (newToken != NULL) {
            return TRUE;
        }
    }
    else if (IObject::Probe(oldToken)->Equals(newToken, &equals), !equals) {
        return TRUE;
    }

    AutoPtr<IBinder> oldParent, newParent;
    oldWindow->GetParentToken((IBinder**)&oldParent);
    newWindow->GetParentToken((IBinder**)&newParent);
    if (oldParent == NULL) {
        if (newParent != NULL) {
            return TRUE;
        }
    }
    else if (IObject::Probe(oldParent)->Equals(newParent, &equals), !equals) {
        return TRUE;
    }

    AutoPtr<IRect> oldBounds, newBounds;
    oldWindow->GetBoundsInScreen((IRect**)&oldBounds);
    newWindow->GetBoundsInScreen((IRect**)&newBounds);
    if (IObject::Probe(oldBounds)->Equals(newParent, &equals), !equals) {
        return TRUE;
    }

    AutoPtr<IList> oldChild, newChild;
    newWindow->GetChildTokens((IList**)&newChild);
    if ((oldWindow->GetChildTokens((IList**)&oldChild), oldChild != NULL) &&
            (newWindow->GetChildTokens((IList**)&newChild), newChild != NULL)
            && (IObject::Probe(oldChild)->Equals(newChild, &equals), !equals)) {
        return TRUE;
    }
    return FALSE;
}

void AccessibilityController::WindowsForAccessibilityObserver::ClearAndRecycleWindows(
    /* [in] */ IList* windows)
{
    Int32 windowCount;
    windows->GetSize(&windowCount);
    for (Int32 i = windowCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> value;
        windows->Get(i, (IInterface**)&value);
        AutoPtr<IWindowInfo> info = IWindowInfo::Probe(value);
        info->Recycle();
        windows->Remove(i);
    }
}

Boolean AccessibilityController::WindowsForAccessibilityObserver::IsReportedWindowType(
    /* [in] */ Int32 windowType)
{
    return (windowType != IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM
            && windowType != IWindowManagerLayoutParams::TYPE_WALLPAPER
            && windowType != IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS
            && windowType != IWindowManagerLayoutParams::TYPE_DISPLAY_OVERLAY
            && windowType != IWindowManagerLayoutParams::TYPE_DRAG
            && windowType != IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER
            && windowType != IWindowManagerLayoutParams::TYPE_POINTER
            && windowType != IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND
            && windowType != IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY
            && windowType != IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA_OVERLAY
            && windowType != IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY
            && windowType != IWindowManagerLayoutParams::TYPE_PRIVATE_PRESENTATION);
}

void AccessibilityController::WindowsForAccessibilityObserver::PopulateVisibleWindowsOnScreenLocked(
    /* [in] */ ISparseArray* outWindows)
{
    AutoPtr<DisplayContent> displayContent = mWindowManagerService->GetDefaultDisplayContentLocked();
    AutoPtr<List<AutoPtr<WindowState> > > windowList = displayContent->GetWindowList();
    List<AutoPtr<WindowState> >::Iterator it = windowList->Begin();
    for (; it != windowList->End(); ++it) {
        AutoPtr<WindowState> windowState = *it;
        Boolean isVisible;
        if (windowState->IsVisibleLw(&isVisible), isVisible) {
            outWindows->Put(windowState->mLayer, (IWindowState*)windowState.Get());
        }
    }
}


//==============================================================================
//                  AccessibilityController
//==============================================================================

const String AccessibilityController::TAG("AccessibilityController");

static AutoPtr<ArrayOf<Float> > InitTempFloats()
{
    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(9);
    return floats;
}
AutoPtr<ArrayOf<Float> > AccessibilityController::sTempFloats = InitTempFloats();

AccessibilityController::AccessibilityController(
    /* [in] */ CWindowManagerService* service)
    : mWindowManagerService(service)
{}

ECode AccessibilityController::SetMagnificationCallbacksLocked(
    /* [in] */ IMagnificationCallbacks* callbacks)
{
    if (callbacks != NULL) {
        if (mDisplayMagnifier != NULL) {
            Slogger::E(TAG, "Magnification callbacks already set!");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mDisplayMagnifier = new DisplayMagnifier(mWindowManagerService, callbacks);
    }
    else {
        if  (mDisplayMagnifier == NULL) {
            Slogger::E(TAG, "Magnification callbacks already cleared!");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mDisplayMagnifier->DestroyLocked();
        mDisplayMagnifier = NULL;
    }
    return NOERROR;
}

ECode AccessibilityController::SetWindowsForAccessibilityCallback(
    /* [in] */ IWindowsForAccessibilityCallback* callback)
{
    if (callback != NULL) {
        if (mWindowsForAccessibilityObserver != NULL) {
            Slogger::E(TAG, "Windows for accessibility callback already set!");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mWindowsForAccessibilityObserver = new WindowsForAccessibilityObserver(
                mWindowManagerService, callback);
    }
    else {
        if (mWindowsForAccessibilityObserver == NULL) {
            Slogger::E(TAG, "Windows for accessibility callback already cleared!");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mWindowsForAccessibilityObserver = NULL;
    }
    return NOERROR;
}

void AccessibilityController::SetMagnificationSpecLocked(
    /* [in] */ IMagnificationSpec* spec)
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->SetMagnificationSpecLocked(spec);
    }
    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::OnRectangleOnScreenRequestedLocked(
    /* [in] */ IRect* rectangle)
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->OnRectangleOnScreenRequestedLocked(rectangle);
    }
    // Not relevant for the window observer.
}

void AccessibilityController::OnWindowLayersChangedLocked()
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->OnWindowLayersChangedLocked();
    }
    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::OnRotationChangedLocked(
    /* [in] */ DisplayContent* displayContent,
    /* [in] */ Int32 rotation)
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->OnRotationChangedLocked(displayContent, rotation);
    }
    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::OnAppWindowTransitionLocked(
    /* [in] */ WindowState* windowState,
    /* [in] */ Int32 transition)
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->OnAppWindowTransitionLocked(windowState, transition);
    }
    // Not relevant for the window observer.
}

void AccessibilityController::OnWindowTransitionLocked(
    /* [in] */ WindowState* windowState,
    /* [in] */ Int32 transition)
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->OnWindowTransitionLocked(windowState, transition);
    }
    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::OnWindowFocusChangedLocked()
{
    // Not relevant for the display magnifier.

    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::OnSomeWindowResizedOrMovedLocked()
{
    // Not relevant for the display magnifier.

    if (mWindowsForAccessibilityObserver != NULL) {
        mWindowsForAccessibilityObserver->ScheduleComputeChangedWindowsLocked();
    }
}

void AccessibilityController::DrawMagnifiedRegionBorderIfNeededLocked()
{
    if (mDisplayMagnifier != NULL) {
        mDisplayMagnifier->DrawMagnifiedRegionBorderIfNeededLocked();
    }
    // Not relevant for the window observer.
}

AutoPtr<IMagnificationSpec> AccessibilityController::GetMagnificationSpecForWindowLocked(
    /* [in] */ WindowState* windowState)
{
    if (mDisplayMagnifier != NULL) {
        return mDisplayMagnifier->GetMagnificationSpecForWindowLocked(windowState);
    }
    return NULL;
}

Boolean AccessibilityController::HasCallbacksLocked()
{
    return (mDisplayMagnifier != NULL
            || mWindowsForAccessibilityObserver != NULL);
}

void AccessibilityController::PopulateTransformationMatrixLocked(
    /* [in] */ WindowState* windowState,
    /* [in] */ IMatrix* outMatrix)
{
    (*sTempFloats)[IMatrix::MSCALE_X] = windowState->mWinAnimator->mDsDx;
    (*sTempFloats)[IMatrix::MSKEW_Y] = windowState->mWinAnimator->mDtDx;
    (*sTempFloats)[IMatrix::MSKEW_X] = windowState->mWinAnimator->mDsDy;
    (*sTempFloats)[IMatrix::MSCALE_Y] = windowState->mWinAnimator->mDtDy;
    windowState->mShownFrame->GetLeft(&(*sTempFloats)[IMatrix::MTRANS_X]);
    windowState->mShownFrame->GetTop(&(*sTempFloats)[IMatrix::MTRANS_Y]);
    (*sTempFloats)[IMatrix::MPERSP_0] = 0;
    (*sTempFloats)[IMatrix::MPERSP_1] = 0;
    (*sTempFloats)[IMatrix::MPERSP_2] = 1;
    outMatrix->SetValues(sTempFloats);
}

} // Wm
} // Server
} // Droid
} // Elastos
