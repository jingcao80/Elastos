
#include "elastos/droid/dialer/list/PhoneFavoriteListView.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// PhoneFavoriteListView::DragScroller
//=================================================================
PhoneFavoriteListView::DragScroller::DragScroller(
    /* [in] */ PhoneFavoriteListView* host)
    : mHost(host)
{}

ECode PhoneFavoriteListView::DragScroller::Run()
{
    if (mHost->mLastDragY <= mHost->mTopScrollBound) {
        SmoothScrollBy(-DRAG_SCROLL_PX_UNIT, (Int32) SCROLL_HANDLER_DELAY_MILLIS);
    }
    else if (mHost->mLastDragY >= mHost->mBottomScrollBound) {
        SmoothScrollBy(DRAG_SCROLL_PX_UNIT, (Int32) SCROLL_HANDLER_DELAY_MILLIS);
    }
    mHost->mScrollHandler->postDelayed(this, SCROLL_HANDLER_DELAY_MILLIS);

    return NOERROR;
}


//=================================================================
// PhoneFavoriteListView::DragShadowOverAnimatorListener
//=================================================================
PhoneFavoriteListView::DragShadowOverAnimatorListener::DragShadowOverAnimatorListener(
    /* [in] */ PhoneFavoriteListView* host)
    : mHost(host)
{}

ECode PhoneFavoriteListView::DragShadowOverAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mDragShadowBitmap != NULL) {
        mHost->mDragShadowBitmap->Recycle();
        mHost->mDragShadowBitmap = NULL;
    }
    mHost->mDragShadowOverlay->SetVisibility(GONE);
    mHost->mDragShadowOverlay->SetImageBitmap(NULL);
    return NOERROR;
}

//=================================================================
// PhoneFavoriteListView
//=================================================================
// TODO:
CAR_INTERFACE_IMPL_3(PhoneFavoriteListView, /*GridView*/AbsListView, IPhoneFavoriteListView,
        IOnDragDropListener, IDragItemContainer);

const String PhoneFavoriteListView::LOG_TAG("PhoneFavoriteListView");

PhoneFavoriteListView::PhoneFavoriteListView()
    : SCROLL_HANDLER_DELAY_MILLIS(5)
    , DRAG_SCROLL_PX_UNIT(25)
    , mIsDragScrollerRunning(FALSE)
    , DRAG_SHADOW_ALPHA(0.7f)
    , BOUND_GAP_RATIO(0.2f)
{
    CDragDropController::New(this, (IDragDropController**)&mDragDropController);
    mDragScroller = (IRunnable*)new DragScroller(this);
    mDragShadowOverAnimatorListener = (IAnimatorListenerAdapter*)new DragShadowOverAnimatorListener(this);
}

ECode PhoneFavoriteListView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PhoneFavoriteListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, -1);
}

ECode PhoneFavoriteListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    assert(0 && "TODO");
    // GridView::constructor(context, attrs, defStyle);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::fade_duration, mAnimationDuration);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledPagingTouchSlop(&mTouchSlop);
    mDragDropController->AddOnDragDropListener(this);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    assert(0 && "TODO");
    // GridView::OnConfigurationChanged(newConfig);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledPagingTouchSlop(&mTouchSlop);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        ev->GetX(&mTouchDownForDragStartX);
        ev->GetY(&mTouchDownForDragStartY);
    }

    assert(0 && "TODO");
    // return GridView::OnInterceptTouchEvent(ev);
    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    event->GetAction(&action);

    Int32 eX;
    event->GetX(&eX);
    Int32 eY;
    event->GetY(&eY);
    switch (action) {
        case IDragEvent::ACTION_DRAG_STARTED: {
            String state;
            event->GetLocalState(&state);
            if (!IPhoneFavoriteTileView::DRAG_PHONE_FAVORITE_TILE.Equals(state)) {
                // Ignore any drag events that were not propagated by long pressing
                // on a {@link PhoneFavoriteTileView}
                *result = FALSE;
                return NOERROR;
            }
            Boolean ret;
            if (mDragDropController->HandleDragStarted(eX, eY, &ret), !ret) {
                *result = FALSE;
                return NOERROR;
            }
            break;
        }
        case IDragEvent::ACTION_DRAG_LOCATION:
            mLastDragY = eY;
            mDragDropController->HandleDragHovered(this, eX, eY);
            // Kick off {@link #mScrollHandler} if it's not started yet.
            if (!mIsDragScrollerRunning &&
                    // And if the distance traveled while dragging exceeds the touch slop
                    (Elastos::Core::Math::Abs(mLastDragY - mTouchDownForDragStartY) >= 4 * mTouchSlop)) {
                mIsDragScrollerRunning = TRUE;
                EnsureScrollHandler();
                mScrollHandler->PostDelayed(mDragScroller, SCROLL_HANDLER_DELAY_MILLIS);
            }
            break;
        case IDragEvent::ACTION_DRAG_ENTERED:
            Int32 height;
            GetHeight(&height);
            const Int32 boundGap = (Int32) (height * BOUND_GAP_RATIO);
            Int32 top;
            GetTop(&top);
            mTopScrollBound = (top + boundGap);
            Int32 bottom;
            GetBottom(&bottom);
            mBottomScrollBound = (bottom - boundGap);
            break;
        case IDragEvent::ACTION_DRAG_EXITED:
        case IDragEvent::ACTION_DRAG_ENDED:
        case IDragEvent::ACTION_DROP:
            EnsureScrollHandler();
            mScrollHandler->RemoveCallbacks(mDragScroller);
            mIsDragScrollerRunning = FALSE;
            // Either a successful drop or it's ended with out drop.
            if (action == IDragEvent::ACTION_DROP || action == IDragEvent::ACTION_DRAG_ENDED) {
                mDragDropController->HandleDragFinished(eX, eY, FALSE);
            }
            break;
        default:
            break;
    }
    // This ListView will consume the drag events on behalf of its children.
    *result = TRUE;
    return NOERROR;
}

ECode PhoneFavoriteListView::SetDragShadowOverlay(
    /* [in] */ IImageView* overlay)
{
    mDragShadowOverlay = overlay;
    mDragShadowOverlay->GetParent(*mDragShadowParent);
    return NOERROR;
}

AutoPtr<IView> PhoneFavoriteListView::GetViewAtPosition(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 childIdx = 0; childIdx < count; childIdx++) {
        AutoPtr<IView> child;
        GetChildAt(childIdx, (IView**)&child);
        Int32 top, bottom, left, right;
        child->GetTop(&top);
        child->GetBottom(&bottom);
        child->GetLeft(&left);
        child->GetRight(&right);
        if ( y >= top && y <= bottom &&
                 x >= left && x <= right) {
            return child;
        }
    }
    return NULL;
}

void PhoneFavoriteListView::EnsureScrollHandler()
{
    if (mScrollHandler == NULL) {
        GetHandler((IHandler**)&mScrollHandler);
    }
}

ECode PhoneFavoriteListView::GetDragDropController(
    /* [out] */ IDragDropController** controller)
{
    VALIDATE_NOT_NULL(controller);
    *controller = mDragDropController;
    REFCOUNT_ADD(*controller);
    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragStarted(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* tileView)
{
    if (mDragShadowOverlay == NULL) {
        return NOERROR;
    }

    mDragShadowOverlay->ClearAnimation();
    CreateDraggedChildBitmap(tileView, &mDragShadowBitmap);
    if (mDragShadowBitmap == NULL) {
        return NOERROR;
    }

    tileView->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft = mLocationOnScreen[0];
    mDragShadowTop = mLocationOnScreen[1];

    // x and y are the coordinates of the on-screen touch event. Using these
    // and the on-screen location of the tileView, calculate the difference between
    // the position of the user's finger and the position of the tileView. These will
    // be used to offset the location of the drag shadow so that it appears that the
    // tileView is positioned directly under the user's finger.
    mTouchOffsetToChildLeft = x - mDragShadowLeft;
    mTouchOffsetToChildTop = y - mDragShadowTop;

    mDragShadowParent->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft -= mLocationOnScreen[0];
    mDragShadowTop -= mLocationOnScreen[1];

    mDragShadowOverlay->SetImageBitmap(mDragShadowBitmap);
    mDragShadowOverlay->SetVisibility(IView::VISIBLE);
    mDragShadowOverlay->SetAlpha(DRAG_SHADOW_ALPHA);

    mDragShadowOverlay->SetX(mDragShadowLeft);
    mDragShadowOverlay->SetY(mDragShadowTop);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragHovered(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* tileView)
{
    // Update the drag shadow location.
    mDragShadowParent->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft = x - mTouchOffsetToChildLeft - mLocationOnScreen[0];
    mDragShadowTop = y - mTouchOffsetToChildTop - mLocationOnScreen[1];
    // Draw the drag shadow at its last known location if the drag shadow exists.
    if (mDragShadowOverlay != NULL) {
        mDragShadowOverlay->SetX(mDragShadowLeft);
        mDragShadowOverlay->SetY(mDragShadowTop);
    }
    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragFinished(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mDragShadowOverlay != NULL) {
        mDragShadowOverlay->ClearAnimation();
        AutoPtr<IViewPropertyAnimator> animator;
        mRemoveView->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(0);
        animator->SetDuration(mAnimationDuration);
        animator->SetListener(mDragShadowOverAnimatorListener);
        animator->Start();
    }
    return NOERROR;
}

ECode PhoneFavoriteListView::OnDroppedOnRemove()
{
    return NOERROR;
}

AutoPtr<IBitmap> PhoneFavoriteListView::CreateDraggedChildBitmap(
    /* [in] */ IView* view)
{
    view->SetDrawingCacheEnabled(TRUE);
    AutoPtr<IBitmap> cache;
    view->GetDrawingCache((IBitmap**)&cache);

    AutoPtr<IBitmap> bitmap;
    if (cache != NULL) {
        // try {
             ECode ec = cache->Copy(BitmapConfig_ARGB_8888,
                    FALSE, (IBitmap**)&bitmap);
             if (ec == (ECode)E_OUT_OF_MEMORY) {
                Logger::W(LOG_TAG, "Failed to copy bitmap from Drawing cache %x", ec);
                bitmap = NULL;
             }
        // } catch (final OutOfMemoryError e) {
        //     Log.w(LOG_TAG, "Failed to copy bitmap from Drawing cache", e);
        //     bitmap = null;
        // }
    }

    view->DestroyDrawingCache();
    view->SetDrawingCacheEnabled(FALSE);

    *view = IView::Probe(bitmap);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode PhoneFavoriteListView::GetViewForLocation(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ IPhoneFavoriteSquareTileView* view)
{
    GetLocationOnScreen(mLocationOnScreen);
    // Calculate the X and Y coordinates of the drag event relative to the view
    Int32 viewX = x - mLocationOnScreen[0];
    Int32 viewY = y - mLocationOnScreen[1];
    AutoPtr<IView> child;
    GetViewAtPosition(viewX, viewY, (IView**)&child);

    if (!IPhoneFavoriteSquareTileView::Probe(child)) {
        *view = NULL;
        return NOERROR;
    }

    *view = IPhoneFavoriteSquareTileView::Probe(child);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
