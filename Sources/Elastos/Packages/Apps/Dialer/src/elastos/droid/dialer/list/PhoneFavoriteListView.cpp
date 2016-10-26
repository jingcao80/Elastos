
#include "elastos/droid/dialer/list/PhoneFavoriteListView.h"
#include "elastos/droid/dialer/list/PhoneFavoriteTileView.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::List::EIID_IOnDragDropListener;
using Elastos::Droid::Dialer::List::EIID_IDragItemContainer;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// PhoneFavoriteListView::InnerContainer
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteListView::InnerContainer, Object, IDragItemContainer)

ECode PhoneFavoriteListView::InnerContainer::GetViewForLocation(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ IPhoneFavoriteSquareTileView** view)
{
    return mHost->GetViewForLocation(x, y, view);
}


//=================================================================
// PhoneFavoriteListView::DragScroller
//=================================================================
ECode PhoneFavoriteListView::DragScroller::Run()
{
    if (mHost->mLastDragY <= mHost->mTopScrollBound) {
        mHost->SmoothScrollBy(-DRAG_SCROLL_PX_UNIT, (Int32)SCROLL_HANDLER_DELAY_MILLIS);
    }
    else if (mHost->mLastDragY >= mHost->mBottomScrollBound) {
        mHost->SmoothScrollBy(DRAG_SCROLL_PX_UNIT, (Int32)SCROLL_HANDLER_DELAY_MILLIS);
    }
    Boolean result;
    mHost->mScrollHandler->PostDelayed(this, SCROLL_HANDLER_DELAY_MILLIS, &result);

    return NOERROR;
}


//=================================================================
// PhoneFavoriteListView::DragShadowOverAnimatorListener
//=================================================================
ECode PhoneFavoriteListView::DragShadowOverAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mDragShadowBitmap != NULL) {
        mHost->mDragShadowBitmap->Recycle();
        mHost->mDragShadowBitmap = NULL;
    }
    IView::Probe(mHost->mDragShadowOverlay)->SetVisibility(GONE);
    mHost->mDragShadowOverlay->SetImageBitmap(NULL);
    return NOERROR;
}

//=================================================================
// PhoneFavoriteListView
//=================================================================
const String PhoneFavoriteListView::TAG("PhoneFavoriteListView");
const Int64 PhoneFavoriteListView::SCROLL_HANDLER_DELAY_MILLIS;
const Int32 PhoneFavoriteListView::DRAG_SCROLL_PX_UNIT;
const Float PhoneFavoriteListView::DRAG_SHADOW_ALPHA;
const Float PhoneFavoriteListView::BOUND_GAP_RATIO;

CAR_INTERFACE_IMPL_2(PhoneFavoriteListView, GridView
        , IPhoneFavoriteListView
        , IOnDragDropListener);

PhoneFavoriteListView::PhoneFavoriteListView()
    : mTouchSlop(0)
    , mTopScrollBound(0)
    , mBottomScrollBound(0)
    , mLastDragY(0)
    , mIsDragScrollerRunning(FALSE)
    , mTouchDownForDragStartX(0)
    , mTouchDownForDragStartY(0)
    , mAnimationDuration(0)
{
    mLocationOnScreen = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IDragItemContainer> container = (IDragItemContainer*)new InnerContainer(this);
    mDragScroller = (IRunnable*)new DragScroller(this);
    mDragShadowOverAnimatorListener = (AnimatorListenerAdapter*)new DragShadowOverAnimatorListener(this);
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
    GridView::constructor(context, attrs, defStyle);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(Elastos::Droid::Dialer::R::integer::fade_duration, &mAnimationDuration);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledPagingTouchSlop((Int32*)&mTouchSlop);
    mDragDropController->AddOnDragDropListener(this);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    GridView::OnConfigurationChanged(newConfig);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledPagingTouchSlop((Int32*)&mTouchSlop);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        ev->GetX((Float*)&mTouchDownForDragStartX);
        ev->GetY((Float*)&mTouchDownForDragStartY);
    }

    return GridView::OnInterceptTouchEvent(ev, result);
}

ECode PhoneFavoriteListView::OnDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    event->GetAction(&action);

    Float eX;
    event->GetX(&eX);
    Float eY;
    event->GetY(&eY);
    switch (action) {
        case IDragEvent::ACTION_DRAG_STARTED: {
            AutoPtr<IInterface> localState;
            event->GetLocalState((IInterface**)&localState);
            String state;
            ICharSequence::Probe(localState)->ToString(&state);
            if (!PhoneFavoriteTileView::DRAG_PHONE_FAVORITE_TILE.Equals(state)) {
                // Ignore any drag events that were not propagated by long pressing
                // on a {@link PhoneFavoriteTileView}
                *result = FALSE;
                return NOERROR;
            }
            if (!mDragDropController->HandleDragStarted(eX, eY)) {
                *result = FALSE;
                return NOERROR;
            }
            break;
        }
        case IDragEvent::ACTION_DRAG_LOCATION: {
            mLastDragY = eY;
            mDragDropController->HandleDragHovered(IView::Probe(this), eX, eY);
            // Kick off {@link #mScrollHandler} if it's not started yet.
            if (!mIsDragScrollerRunning &&
                    // And if the distance traveled while dragging exceeds the touch slop
                    (Elastos::Core::Math::Abs(mLastDragY - mTouchDownForDragStartY) >= 4 * mTouchSlop)) {
                mIsDragScrollerRunning = TRUE;
                EnsureScrollHandler();
                Boolean result;
                mScrollHandler->PostDelayed(mDragScroller, SCROLL_HANDLER_DELAY_MILLIS, &result);
            }
            break;
        }
        case IDragEvent::ACTION_DRAG_ENTERED: {
            Int32 height;
            GetHeight(&height);
            Int32 boundGap =(Int32) (height * BOUND_GAP_RATIO);
            Int32 top;
            GetTop(&top);
            mTopScrollBound = (top + boundGap);
            Int32 bottom;
            GetBottom(&bottom);
            mBottomScrollBound = (bottom - boundGap);
            break;
        }
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
    AutoPtr<IViewParent> parent;
    IView::Probe(mDragShadowOverlay)->GetParent((IViewParent**)&parent);
    mDragShadowParent = IView::Probe(parent);
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
        if ((child->GetTop(&top), y >= top) && (child->GetBottom(&bottom), y <= bottom) &&
                (child->GetLeft(&left), x >= left) && (child->GetRight(&right), x <= right)) {
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
    VALIDATE_NOT_NULL(controller)
    *controller = mDragDropController;
    REFCOUNT_ADD(*controller)
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

    IView::Probe(mDragShadowOverlay)->ClearAnimation();
    mDragShadowBitmap = CreateDraggedChildBitmap(IView::Probe(tileView));
    if (mDragShadowBitmap == NULL) {
        return NOERROR;
    }

    IView::Probe(tileView)->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft = (*mLocationOnScreen)[0];
    mDragShadowTop = (*mLocationOnScreen)[1];

    // x and y are the coordinates of the on-screen touch event. Using these
    // and the on-screen location of the tileView, calculate the difference between
    // the position of the user's finger and the position of the tileView. These will
    // be used to offset the location of the drag shadow so that it appears that the
    // tileView is positioned directly under the user's finger.
    mTouchOffsetToChildLeft = x - mDragShadowLeft;
    mTouchOffsetToChildTop = y - mDragShadowTop;

    mDragShadowParent->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft -= (*mLocationOnScreen)[0];
    mDragShadowTop -= (*mLocationOnScreen)[1];

    mDragShadowOverlay->SetImageBitmap(mDragShadowBitmap);
    AutoPtr<IView> view = IView::Probe(mDragShadowOverlay);
    view->SetVisibility(IView::VISIBLE);
    mDragShadowOverlay->SetAlpha(DRAG_SHADOW_ALPHA);

    view->SetX(mDragShadowLeft);
    view->SetY(mDragShadowTop);

    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragHovered(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* tileView)
{
    // Update the drag shadow location.
    AutoPtr<IView> view = IView::Probe(mDragShadowParent);
    view->GetLocationOnScreen(mLocationOnScreen);
    mDragShadowLeft = x - mTouchOffsetToChildLeft - (*mLocationOnScreen)[0];
    mDragShadowTop = y - mTouchOffsetToChildTop - (*mLocationOnScreen)[1];
    // Draw the drag shadow at its last known location if the drag shadow exists.
    if (mDragShadowOverlay != NULL) {
        view->SetX(mDragShadowLeft);
        view->SetY(mDragShadowTop);
    }
    return NOERROR;
}

ECode PhoneFavoriteListView::OnDragFinished(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mDragShadowOverlay != NULL) {
        AutoPtr<IView> view = IView::Probe(mDragShadowOverlay);
        view->ClearAnimation();
        AutoPtr<IViewPropertyAnimator> animator;
        view->Animate((IViewPropertyAnimator**)&animator);
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
        ECode ec = cache->Copy(BitmapConfig_ARGB_8888, FALSE, (IBitmap**)&bitmap);
        if (ec == (ECode)E_OUT_OF_MEMORY) {
            Logger::W(TAG, "Failed to copy bitmap from Drawing cache %x", ec);
            bitmap = NULL;
        }
        // } catch (final OutOfMemoryError e) {
        //     Log.w(TAG, "Failed to copy bitmap from Drawing cache", e);
        //     bitmap = null;
        // }
    }

    view->DestroyDrawingCache();
    view->SetDrawingCacheEnabled(FALSE);
    return bitmap;
}

ECode PhoneFavoriteListView::GetViewForLocation(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ IPhoneFavoriteSquareTileView** view)
{
    GetLocationOnScreen(mLocationOnScreen);
    // Calculate the X and Y coordinates of the drag event relative to the view
    Int32 viewX = x - (*mLocationOnScreen)[0];
    Int32 viewY = y - (*mLocationOnScreen)[1];
    AutoPtr<IView> child = GetViewAtPosition(viewX, viewY);

    if (IPhoneFavoriteSquareTileView::Probe(child) != NULL) {
        *view = NULL;
        return NOERROR;
    }

    *view = IPhoneFavoriteSquareTileView::Probe(child);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
