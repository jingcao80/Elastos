
#include "elastos/droid/launcher2/DragController.h"
#include "elastos/droid/launcher2/DragView.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Launcher2::DragView;
using Elastos::Droid::Launcher2::ShortcutInfo;
using Elastos::Droid::Launcher2::ApplicationInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;


namespace Elastos {
namespace Droid {
namespace Launcher2 {

DragController::ScrollRunnable::ScrollRunnable(
    /* [in] */ DragController* host)
    : mDirection(0)
    , mHost(host)
{
}

ECode DragController::ScrollRunnable::Run()
{
    if (mHost->mDragScroller != NULL) {
        if (mDirection == SCROLL_LEFT) {
            mHost->mDragScroller->ScrollLeft();
        }
        else {
            mHost->mDragScroller->ScrollRight();
        }
        mHost->mScrollState = SCROLL_OUTSIDE_ZONE;
        mHost->mDistanceSinceScroll = 0;
        Boolean tmp;
        mHost->mDragScroller->OnExitScrollArea(&tmp);
        AutoPtr<IDragLayer> dragLayer;
        assert(0 && "need class mLauncher");
        //mHost->mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        assert(0 && "need class mLauncher");
        //dragLayer->OnExitScrollArea();

        Boolean res;
        mHost->IsDragging(&res);
        if (res) {
            // Check the scroll again so that we can requeue the scroller if necessary
            mHost->CheckScrollState((*(mHost->mLastTouch))[0], (*(mHost->mLastTouch))[1]);
        }
    }
    return NOERROR;
}

ECode DragController::ScrollRunnable::SetDirection(
    /* [in] */ Int32 direction)
{
    mDirection = direction;
    return NOERROR;
}

const String DragController::TAG("Launcher.DragController");

const Int32 DragController::SCROLL_DELAY = 500;
const Int32 DragController::RESCROLL_DELAY = 750;
const Int32 DragController::VIBRATE_DURATION = 15;

const Boolean DragController::PROFILE_DRAWING_DURING_DRAG = FALSE;

const Int32 DragController::SCROLL_OUTSIDE_ZONE = 0;
const Int32 DragController::SCROLL_WAITING_IN_ZONE = 1;

const Float DragController::MAX_FLING_DEGREES = 35.0f;

CAR_INTERFACE_IMPL(DragController, Object, IDragController);

DragController::DragController()
{
    CRect::New((IRect**)&mRectTemp);
    mCoordinatesTemp = ArrayOf<Int32>::Alloc(2);
    mDragging = FALSE;
    mMotionDownX = 0;
    mMotionDownY = 0;
    mScrollZone = 0;
    CArrayList::New((IArrayList**)&mDropTargets);
    CArrayList::New((IArrayList**)&mListeners);
    mScrollState = SCROLL_OUTSIDE_ZONE;
    mScrollRunnable = new ScrollRunnable(this);
    mLastTouch = ArrayOf<Int32>::Alloc(2);
    mLastTouchUpTime = -1;
    mDistanceSinceScroll = 0;
    mTmpPoint = ArrayOf<Int32>::Alloc(2);
    CRect::New((IRect**)&mDragLayerRect);
    mFlingToDeleteThresholdVelocity = 0;
}

ECode DragController::constructor(
    /* [in] */ ILauncher* launcher)
{
    AutoPtr<IResources> r;
    assert(0 && "need class mLauncher");
    //launcher->GetResources((IResources**)&r);
    mLauncher = launcher;
    CHandler::New((IHandler**)&mHandler);
    r->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::scroll_zone,
            &mScrollZone);

    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    helper->Obtain((IVelocityTracker**)&mVelocityTracker);

    AutoPtr<IInterface> obj;
    assert(0 && "need class mLauncher");
    //launcher->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    mVibrator = IVibrator::Probe(obj);

    AutoPtr<IDisplayMetrics> metrics;
    r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);

    Int32 value;
    r->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_flingToDeleteMinVelocity,
            &value);
    mFlingToDeleteThresholdVelocity = (Int32)(value * density);
    return NOERROR;
}

ECode DragController::Dragging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDragging;
    return NOERROR;
}

ECode DragController::StartDrag(
    /* [in] */ IView* v,
    /* [in] */ IBitmap* bmp,
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* dragInfo,
    /* [in] */ Int32 dragAction,
    /* [in] */ IPoint* extraPadding,
    /* [in] */ Float initialDragViewScale)
{
    AutoPtr<ArrayOf<Int32> > loc = mCoordinatesTemp;
    AutoPtr<IDragLayer> dragLayer;
    assert(0 && "need class mLauncher");
    //mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    //dragLayer->GetLocationInDragLayer(v, loc);
    Int32 viewExtraPaddingLeft;
    Int32 viewExtraPaddingTop;
    if (extraPadding != NULL) {
        extraPadding->GetX(&viewExtraPaddingLeft);
        extraPadding->GetY(&viewExtraPaddingTop);
    }
    else {
        viewExtraPaddingLeft = 0;
        viewExtraPaddingTop = 0;
    }
    Int32 left;
    v->GetPaddingLeft(&left);
    Int32 width;
    bmp->GetWidth(&width);
    Int32 dragLayerX = (*loc)[0] + left + viewExtraPaddingLeft +
            (Int32)((initialDragViewScale * width - width) / 2);

    Int32 top;
    v->GetPaddingTop(&top);
    Int32 height;
    bmp->GetHeight(&height);
    Int32 dragLayerY = (*loc)[1] + top + viewExtraPaddingTop +
            (Int32)((initialDragViewScale * height - height) / 2);

    StartDrag(bmp, dragLayerX, dragLayerY, source, dragInfo, dragAction, NULL,
            NULL, initialDragViewScale);

    if (dragAction == DRAG_ACTION_MOVE) {
        v->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode DragController::StartDrag(
    /* [in] */ IBitmap* b,
    /* [in] */ Int32 dragLayerX,
    /* [in] */ Int32 dragLayerY,
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* dragInfo,
    /* [in] */ Int32 dragAction,
    /* [in] */ IPoint* dragOffset,
    /* [in] */ IRect* dragRegion,
    /* [in] */ Float initialDragViewScale)
{
    // if (PROFILE_DRAWING_DURING_DRAG) {
    //     Debug::StartMethodTracing(String("Launcher"));
    // }

    // Hide soft keyboard, if visible
    if (mInputMethodManager == NULL) {
        AutoPtr<IInterface> obj;
        assert(0 && "need class mLauncher");
        //mLauncher->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
        mInputMethodManager = IInputMethodManager::Probe(obj);
    }
    Boolean res;
    mInputMethodManager->HideSoftInputFromWindow(mWindowToken, 0, &res);

    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IDragControllerDragListener> listener = IDragControllerDragListener::Probe(obj);

        listener->OnDragStart(source, dragInfo, dragAction);
    }

    Int32 registrationX = mMotionDownX - dragLayerX;
    Int32 registrationY = mMotionDownY - dragLayerY;

    Int32 left;
    Int32 dragRegionLeft = dragRegion == NULL ? 0 : (dragRegion->GetLeft(&left), left);
    Int32 top;
    Int32 dragRegionTop = dragRegion == NULL ? 0 : (dragRegion->GetTop(&top), top);

    mDragging = TRUE;

    mDragObject = new DragObject();

    mDragObject->mDragComplete = FALSE;
    mDragObject->mXOffset = mMotionDownX - (dragLayerX + dragRegionLeft);
    mDragObject->mYOffset = mMotionDownY - (dragLayerY + dragRegionTop);
    mDragObject->mDragSource = source;
    mDragObject->mDragInfo = dragInfo;

    mVibrator->Vibrate(VIBRATE_DURATION);

    Int32 width;
    b->GetWidth(&width);
    Int32 height;
    b->GetHeight(&height);
    AutoPtr<DragView> dragView;
    dragView = new DragView();
    dragView->constructor(mLauncher, b, registrationX,
            registrationY, 0, 0, width, height, initialDragViewScale);
    mDragObject->mDragView = dragView;

    if (dragOffset != NULL) {
        AutoPtr<IPoint> p;
        CPoint::New(dragOffset, (IPoint**)&p);
        dragView->SetDragVisualizeOffset(p);
    }
    if (dragRegion != NULL) {
        AutoPtr<IRect> r;
        CRect::New(dragRegion, (IRect**)&r);
        dragView->SetDragRegion(r);
    }

    dragView->Show(mMotionDownX, mMotionDownY);
    HandleMoveEvent(mMotionDownX, mMotionDownY);
    return NOERROR;
}

ECode DragController::GetViewBitmap(
    /* [in] */ IView* v,
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    v->ClearFocus();
    v->SetPressed(FALSE);

    Boolean willNotCache;
    v->WillNotCacheDrawing(&willNotCache);
    v->SetWillNotCacheDrawing(FALSE);

    // Reset the drawing cache background color to fully transparent
    // for the duration of this operation
    Int32 color;
    v->GetDrawingCacheBackgroundColor(&color);
    v->SetDrawingCacheBackgroundColor(0);
    Float alpha;
    v->GetAlpha(&alpha);
    v->SetAlpha(1.0f);

    if (color != 0) {
        v->DestroyDrawingCache();
    }
    v->BuildDrawingCache();
    AutoPtr<IBitmap> cacheBitmap;
    v->GetDrawingCache((IBitmap**)&cacheBitmap);
    if (cacheBitmap == NULL) {
        //Slogger::E(TAG, "failed getViewBitmap(" + v + ")", new RuntimeException());
        Slogger::E(TAG, "failed getViewBitmap");
        *map = NULL;
        return NOERROR;
    }

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> bitmap;
    helper->CreateBitmap(cacheBitmap, (IBitmap**)&bitmap);

    // Restore the view
    v->DestroyDrawingCache();
    v->SetAlpha(alpha);
    v->SetWillNotCacheDrawing(willNotCache);
    v->SetDrawingCacheBackgroundColor(color);

    *map = bitmap;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode DragController::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDragging;
    return NOERROR;
}

ECode DragController::IsDragging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDragging;
    return NOERROR;
}

ECode DragController::CancelDrag()
{
    if (mDragging) {
        if (mLastDropTarget != NULL) {
            mLastDropTarget->OnDragExit(mDragObject);
        }
        mDragObject->mDeferDragViewCleanupPostAnimation = FALSE;
        mDragObject->mCancelled = TRUE;
        mDragObject->mDragComplete = TRUE;
        mDragObject->mDragSource->OnDropCompleted(NULL, mDragObject, FALSE, FALSE);
    }
    EndDrag();
    return NOERROR;
}

ECode DragController::OnAppsRemoved(
    /* [in] */ IArrayList* appInfos,
    /* [in] */ IContext* context)
{
    // Cancel the current drag if we are removing an app that we are dragging
    if (mDragObject != NULL) {
        AutoPtr<IInterface> rawDragInfo = mDragObject->mDragInfo;
        if (IShortcutInfo::Probe(rawDragInfo) != NULL) {
            AutoPtr<IShortcutInfo> dragInfo = IShortcutInfo::Probe(rawDragInfo);
            Int32 size;
            appInfos->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                appInfos->Get(i, (IInterface**)&obj);
                AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);

                // Added null checks to prevent NPE we've seen in the wild
                if (dragInfo != NULL) {
                    ShortcutInfo* _dragInfo = (ShortcutInfo*)dragInfo.Get();
                    ApplicationInfo* _info = (ApplicationInfo*)info.Get();
                    if (_dragInfo->mIntent != NULL) {
                        AutoPtr<IComponentName> componentName;
                        _dragInfo->mIntent->GetComponent((IComponentName**)&componentName);
                        Boolean isSameComponent;
                        IObject::Probe(componentName)->Equals(_info->mComponentName, &isSameComponent);
                        if (isSameComponent) {
                            return CancelDrag();
                        }
                    }
                }
            }
        }
    }
    return NOERROR;
}

void DragController::EndDrag()
{
    if (mDragging) {
        mDragging = FALSE;
        ClearScrollRunnable();
        Boolean isDeferred = FALSE;
        if (mDragObject->mDragView != NULL) {
            isDeferred = mDragObject->mDeferDragViewCleanupPostAnimation;
            if (!isDeferred) {
                mDragObject->mDragView->Remove();
            }
            mDragObject->mDragView = NULL;
        }

        // Only end the drag if we are not deferred
        if (!isDeferred) {
            Int32 size;
            mListeners->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mListeners->Get(i, (IInterface**)&obj);
                AutoPtr<IDragControllerDragListener> listener = IDragControllerDragListener::Probe(obj);
                listener->OnDragEnd();
            }
        }
    }

    ReleaseVelocityTracker();
}

ECode DragController::OnDeferredEndDrag(
    /* [in] */ IDragView* dragView)
{
    dragView->Remove();

    // If we skipped calling onDragEnd() before, do it now
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IDragControllerDragListener> listener = IDragControllerDragListener::Probe(obj);
        listener->OnDragEnd();
    }
    return NOERROR;
}

ECode DragController::OnDeferredEndFling(
    /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    return _d->mDragSource->OnFlingToDeleteCompleted();
}

AutoPtr<ArrayOf<Int32> > DragController::GetClampedDragLayerPos(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IDragLayer> dragLayer;
    assert(0 && "need class mLauncher");
    //mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    //dragLayer->GetLocalVisibleRect(mDragLayerRect);
    Int32 left;
    mDragLayerRect->GetLeft(&left);
    Int32 right;
    mDragLayerRect->GetRight(&right);
    (*mTmpPoint)[0] = (Int32)Elastos::Core::Math::Max((Float)left,
            Elastos::Core::Math::Min(x, (Float)(right - 1)));
    Int32 top;
    mDragLayerRect->GetTop(&top);
    Int32 bottom;
    mDragLayerRect->GetBottom(&bottom);
    (*mTmpPoint)[1] = (Int32)Elastos::Core::Math::Max((Float)top,
            Elastos::Core::Math::Min(y, (Float)(bottom - 1)));
    return mTmpPoint;
}

ECode DragController::GetLastGestureUpTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = 0;

    if (mDragging) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(time);
        return NOERROR;
    }
    else {
        *time = mLastTouchUpTime;
        return NOERROR;
    }
    return NOERROR;
}

ECode DragController::ResetLastGestureUpTime()
{
    mLastTouchUpTime = -1;
    return NOERROR;
}

ECode DragController::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //@SuppressWarnings("all") // suppress dead code warning
    const Boolean debug = FALSE;
    if (debug) {
        // Slogger::D(Launcher::TAG, "DragController.onInterceptTouchEvent " + ev + " mDragging="
        //         + mDragging);
    }

    // Update the velocity tracker
    AcquireVelocityTrackerAndAddMovement(ev);

    Int32 action;
    ev->GetAction(&action);
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    AutoPtr<ArrayOf<Int32> > dragLayerPos = GetClampedDragLayerPos(x, y);
    Int32 dragLayerX = (*dragLayerPos)[0];
    Int32 dragLayerY = (*dragLayerPos)[1];

    switch (action) {
        case IMotionEvent::ACTION_MOVE:
            break;
        case IMotionEvent::ACTION_DOWN:
            // Remember location of down touch
            mMotionDownX = dragLayerX;
            mMotionDownY = dragLayerY;
            mLastDropTarget = NULL;
            break;
        case IMotionEvent::ACTION_UP:
        {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&mLastTouchUpTime);
            if (mDragging) {
                AutoPtr<IPointF> vec = IsFlingingToDelete(mDragObject->mDragSource);
                if (vec != NULL) {
                    DropOnFlingToDeleteTarget(dragLayerX, dragLayerY, vec);
                }
                else {
                    Drop(dragLayerX, dragLayerY);
                }
            }
            EndDrag();
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
            CancelDrag();
            break;
    }

    *result = mDragging;
    return NOERROR;
}

ECode DragController::SetMoveTarget(
    /* [in] */ IView* view)
{
    mMoveTarget = view;
    return NOERROR;
}

ECode DragController::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    *result = mMoveTarget != NULL &&
            (mMoveTarget->DispatchUnhandledMove(focused, direction, &res), res);
    return NOERROR;
}

void DragController::ClearScrollRunnable()
{
    mHandler->RemoveCallbacks(mScrollRunnable);
    if (mScrollState == SCROLL_WAITING_IN_ZONE) {
        mScrollState = SCROLL_OUTSIDE_ZONE;
        mScrollRunnable->SetDirection(SCROLL_RIGHT);
        Boolean res;
        mDragScroller->OnExitScrollArea(&res);
        AutoPtr<IDragLayer> dragLayer;
        assert(0 && "need class dragLayer");
        //mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        //dragLayer->OnExitScrollArea();
    }
}

void DragController::HandleMoveEvent(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mDragObject->mDragView->Move(x, y);

    // Drop on someone?
    AutoPtr<ArrayOf<Int32> > coordinates = mCoordinatesTemp;
    AutoPtr<IDropTarget> dropTarget = FindDropTarget(x, y, coordinates);
    mDragObject->mX = (*coordinates)[0];
    mDragObject->mY = (*coordinates)[1];
    CheckTouchMove(dropTarget);

    // Check if we are hovering over the scroll areas
    mDistanceSinceScroll +=
            Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow((*mLastTouch)[0] - x, 2) +
            Elastos::Core::Math::Pow((*mLastTouch)[1] - y, 2));
    (*mLastTouch)[0] = x;
    (*mLastTouch)[1] = y;
    CheckScrollState(x, y);
}

ECode DragController::ForceTouchMove()
{
    AutoPtr<ArrayOf<Int32> > dummyCoordinates = mCoordinatesTemp;
    AutoPtr<IDropTarget> dropTarget = FindDropTarget((*mLastTouch)[0],
            (*mLastTouch)[1], dummyCoordinates);
    CheckTouchMove(dropTarget);
    return NOERROR;
}

void DragController::CheckTouchMove(
    /* [in] */ IDropTarget* dropTarget)
{
    if (dropTarget != NULL) {
        AutoPtr<IDropTarget> delegate;
        dropTarget->GetDropTargetDelegate(mDragObject, (IDropTarget**)&delegate);
        if (delegate != NULL) {
            dropTarget = delegate;
        }

        if (TO_IINTERFACE(mLastDropTarget) != TO_IINTERFACE(dropTarget)) {
            if (mLastDropTarget != NULL) {
                mLastDropTarget->OnDragExit(mDragObject);
            }
            dropTarget->OnDragEnter(mDragObject);
        }
        dropTarget->OnDragOver(mDragObject);
    }
    else {
        if (mLastDropTarget != NULL) {
            mLastDropTarget->OnDragExit(mDragObject);
        }
    }
    mLastDropTarget = dropTarget;
}

void DragController::CheckScrollState(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y)
{
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfig;
    helper->Get(IContext::Probe(mLauncher), (IViewConfiguration**)&viewConfig);
    Int32 slop;
    viewConfig->GetScaledWindowTouchSlop(&slop);
    Int32 delay = mDistanceSinceScroll < slop ? RESCROLL_DELAY : SCROLL_DELAY;
    AutoPtr<IDragLayer> dragLayer;
    assert(0 && "need class mLauncher");
    //mLauncher->GetDragLayer((IDragLayer**)&dragLayer);

    Int32 direction;
    assert(0 && "need class dragLayer");
    //dragLayer->GetLayoutDirection(&direction);
    Boolean isRtl = (direction == IView::LAYOUT_DIRECTION_RTL);
    Int32 forwardDirection = isRtl ? SCROLL_RIGHT : SCROLL_LEFT;
    Int32 backwardsDirection = isRtl ? SCROLL_LEFT : SCROLL_RIGHT;

    Int32 width;
    if (x < mScrollZone) {
        if (mScrollState == SCROLL_OUTSIDE_ZONE) {
            mScrollState = SCROLL_WAITING_IN_ZONE;
            Boolean res;
            mDragScroller->OnEnterScrollArea(x, y, forwardDirection, &res);
            if (res) {
                assert(0 && "need class dragLayer");
                //dragLayer->OnEnterScrollArea(forwardDirection);
                mScrollRunnable->SetDirection(forwardDirection);
                Boolean tmp;
                mHandler->PostDelayed(mScrollRunnable, delay, &tmp);
            }
        }
    }
    else if (x > (mScrollView->GetWidth(&width), width) - mScrollZone) {
        if (mScrollState == SCROLL_OUTSIDE_ZONE) {
            mScrollState = SCROLL_WAITING_IN_ZONE;
            Boolean res;
            mDragScroller->OnEnterScrollArea(x, y, backwardsDirection, &res);
            if (res) {
                assert(0 && "need class dragLayer");
                //dragLayer->OnEnterScrollArea(backwardsDirection);
                mScrollRunnable->SetDirection(backwardsDirection);
                Boolean tmp;
                mHandler->PostDelayed(mScrollRunnable, delay, &tmp);
            }
        }
    }
    else {
        ClearScrollRunnable();
    }
}

ECode DragController::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!mDragging) {
        *result = FALSE;
        return NOERROR;
    }

    // Update the velocity tracker
    AcquireVelocityTrackerAndAddMovement(ev);

    Int32 action;
    ev->GetAction(&action);
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    AutoPtr<ArrayOf<Int32> > dragLayerPos = GetClampedDragLayerPos(x, y);
    Int32 dragLayerX = (*dragLayerPos)[0];
    Int32 dragLayerY = (*dragLayerPos)[1];

    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            // Remember where the motion event started
            mMotionDownX = dragLayerX;
            mMotionDownY = dragLayerY;

            Int32 width;

            if ((dragLayerX < mScrollZone) ||
                    (dragLayerX > (mScrollView->GetWidth(&width), width) - mScrollZone)) {
                mScrollState = SCROLL_WAITING_IN_ZONE;
                Boolean tmp;
                mHandler->PostDelayed(mScrollRunnable, SCROLL_DELAY, &tmp);
            }
            else {
                mScrollState = SCROLL_OUTSIDE_ZONE;
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            HandleMoveEvent(dragLayerX, dragLayerY);
            break;
        case IMotionEvent::ACTION_UP:
            // Ensure that we've processed a move event at the current pointer location.
            HandleMoveEvent(dragLayerX, dragLayerY);
            mHandler->RemoveCallbacks(mScrollRunnable);

            if (mDragging) {
                AutoPtr<IPointF> vec = IsFlingingToDelete(mDragObject->mDragSource);
                if (vec != NULL) {
                    DropOnFlingToDeleteTarget(dragLayerX, dragLayerY, vec);
                }
                else {
                    Drop(dragLayerX, dragLayerY);
                }
            }
            EndDrag();
            break;
        case IMotionEvent::ACTION_CANCEL:
            mHandler->RemoveCallbacks(mScrollRunnable);
            CancelDrag();
            break;
    }

    *result = TRUE;
    return NOERROR;
}

AutoPtr<IPointF> DragController::IsFlingingToDelete(
        /* [in] */ IDragSource* source)
{
    if (mFlingToDeleteDropTarget == NULL) return NULL;
    Boolean res;
    source->SupportsFlingToDelete(&res);
    if (!res) return NULL;

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(IContext::Probe(mLauncher), (IViewConfiguration**)&config);
    Int32 maximumFlingVelocity;
    config->GetScaledMaximumFlingVelocity(&maximumFlingVelocity);
    mVelocityTracker->ComputeCurrentVelocity(1000, maximumFlingVelocity);

    Float yvelocity;
    mVelocityTracker->GetYVelocity(&yvelocity);
    if (yvelocity < mFlingToDeleteThresholdVelocity) {
        // Do a quick dot product test to ensure that we are flinging upwards
        Float xvelocity;
        mVelocityTracker->GetXVelocity(&xvelocity);
        AutoPtr<IPointF> vel;
        CPointF::New(xvelocity, yvelocity, (IPointF**)&vel);
        AutoPtr<IPointF> upVec;
        CPointF::New(0.0f, -1.0f, (IPointF**)&upVec);

        Float vx;
        vel->GetX(&vx);
        Float vy;
        vel->GetY(&vy);
        Float ux;
        upVec->GetX(&ux);
        Float uy;
        upVec->GetY(&uy);
        Float vlength;
        vel->Length(&vlength);
        Float ulength;
        upVec->Length(&ulength);
        Float theta = (Float)Elastos::Core::Math::Acos(((vx * ux) + (vy * uy)) /
                (vlength * ulength));
        if (theta <= Elastos::Core::Math::ToRadians(MAX_FLING_DEGREES)) {
            return vel;
        }
    }
    return NULL;
}

void DragController::DropOnFlingToDeleteTarget(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPointF* vel)
{
    AutoPtr<ArrayOf<Int32> > coordinates = mCoordinatesTemp;

    mDragObject->mX = (*coordinates)[0];
    mDragObject->mY = (*coordinates)[1];

    // Clean up dragging on the target if it's not the current fling delete target otherwise,
    // start dragging to it.
    if (mLastDropTarget != NULL && mFlingToDeleteDropTarget != mLastDropTarget) {
        mLastDropTarget->OnDragExit(mDragObject);
    }

    // Drop onto the fling-to-delete target
    Boolean accepted = FALSE;
    mFlingToDeleteDropTarget->OnDragEnter(mDragObject);
    // We must set dragComplete to true _only_ after we "enter" the fling-to-delete target for
    // "drop"
    mDragObject->mDragComplete = TRUE;
    mFlingToDeleteDropTarget->OnDragExit(mDragObject);
    Boolean res;
    mFlingToDeleteDropTarget->AcceptDrop(mDragObject, &res);
    if (res) {
        mFlingToDeleteDropTarget->OnFlingToDelete(mDragObject,
                mDragObject->mX, mDragObject->mY, vel);
        accepted = TRUE;
    }
    mDragObject->mDragSource->OnDropCompleted(IView::Probe(mFlingToDeleteDropTarget),
            mDragObject, TRUE, accepted);
}

void DragController::Drop(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<ArrayOf<Int32> > coordinates = mCoordinatesTemp;
    AutoPtr<IDropTarget> dropTarget = FindDropTarget((Int32)x, (Int32)y, coordinates);

    mDragObject->mX = (*coordinates)[0];
    mDragObject->mY = (*coordinates)[1];
    Boolean accepted = FALSE;
    if (dropTarget != NULL) {
        mDragObject->mDragComplete = TRUE;
        dropTarget->OnDragExit(mDragObject);
        Boolean res;
        dropTarget->AcceptDrop(mDragObject, &res);
        if (res) {
            dropTarget->OnDrop(mDragObject);
            accepted = TRUE;
        }
    }
    assert(0 && "dropTarget is not a view");
    //mDragObject->mDragSource->OnDropCompleted((View) dropTarget, mDragObject, FALSE, accepted);
}

AutoPtr<IDropTarget> DragController::FindDropTarget(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ ArrayOf<Int32>* dropCoordinates)
{
    AutoPtr<IRect> r = mRectTemp;

    AutoPtr<IArrayList> dropTargets = mDropTargets;
    Int32 count;
    dropTargets->GetSize(&count);
    for (Int32 i=count-1; i>=0; i--) {
        AutoPtr<IInterface> obj;
        dropTargets->Get(i, (IInterface**)&obj);
        AutoPtr<IDropTarget> target = IDropTarget::Probe(obj);
        Boolean res;
        target->IsDropEnabled(&res);
        if (!res)
            continue;

        target->GetHitRect(r);

        // Convert the hit rect to DragLayer coordinates
        target->GetLocationInDragLayer(dropCoordinates);
        Int32 left;
        target->GetLeft(&left);
        Int32 top;
        target->GetTop(&top);
        r->Offset((*dropCoordinates)[0] - left, (*dropCoordinates)[1] - top);

        mDragObject->mX = x;
        mDragObject->mY = y;
        Boolean tmp;
        r->Contains(x, y, &tmp);
        if (tmp) {
            AutoPtr<IDropTarget> delegate;
            target->GetDropTargetDelegate(mDragObject, (IDropTarget**)&delegate);
            if (delegate != NULL) {
                target = delegate;
                target->GetLocationInDragLayer(dropCoordinates);
            }

            // Make dropCoordinates relative to the DropTarget
            (*dropCoordinates)[0] = x - (*dropCoordinates)[0];
            (*dropCoordinates)[1] = y - (*dropCoordinates)[1];

            return target;
        }
    }
    return NULL;
}

ECode DragController::SetDragScoller(
    /* [in] */ IDragScroller* scroller)
{
    mDragScroller = scroller;
    return NOERROR;
}

ECode DragController::SetWindowToken(
    /* [in] */ IBinder* token)
{
    mWindowToken = token;
    return NOERROR;
}

ECode DragController::AddDragListener(
    /* [in] */ IDragControllerDragListener* l)
{
    return mListeners->Add(l);
}

ECode DragController::RemoveDragListener(
    /* [in] */ IDragControllerDragListener* l)
{
    return mListeners->Remove(l);
}

ECode DragController::AddDropTarget(
    /* [in] */ IDropTarget* target)
{
    return mDropTargets->Add(target);
}

ECode DragController::RemoveDropTarget(
    /* [in] */ IDropTarget* target)
{
    return mDropTargets->Remove(target);
}

ECode DragController::SetFlingToDeleteDropTarget(
    /* [in] */ IDropTarget* target)
{
     mFlingToDeleteDropTarget = target;
     return NOERROR;
}

void DragController::AcquireVelocityTrackerAndAddMovement(
    /* [in] */ IMotionEvent* ev)
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);
}

void DragController::ReleaseVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

ECode DragController::SetScrollView(
    /* [in] */ IView* v)
{
    mScrollView = v;
    return NOERROR;
}

ECode DragController::GetDragView(
    /* [out] */ IDragView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = mDragObject->mDragView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos