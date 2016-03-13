
#include "elastos/droid/launcher2/SmoothPagedView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Float SmoothPagedView::OvershootInterpolatorDEFAULT_TENSION = 1.3f;

CAR_INTERFACE_IMPL_2(SmoothPagedView::OvershootInterpolator, Object, IInterpolator,
        ITimeInterpolator);

SmoothPagedView::OvershootInterpolator::OvershootInterpolator()
    : mTension(DEFAULT_TENSION)
{
}

void SmoothPagedView::OvershootInterpolator::SetDistance(
    /* [in] */ Int32 distance)
{
    mTension = distance > 0 ? DEFAULT_TENSION / distance : DEFAULT_TENSION;
}

void SmoothPagedView::OvershootInterpolator::DisableSettle()
{
    mTension = 0.f;
}

Float SmoothPagedView::OvershootInterpolator::GetInterpolation(
    /* [in] */ Float t)
{
    // _o(t) = t * t * ((tension + 1) * t + tension)
    // o(t) = _o(t - 1) + 1
    t -= 1.0f;
    return t * t * ((mTension + 1) * t + mTension) + 1.0f;
}

const Int32 SmoothPagedView::DEFAULT_MODE = 0;
const Int32 SmoothPagedView::X_LARGE_MODE = 1;

const Float SmoothPagedView::SMOOTHING_SPEED = 0.75f;
const Float SmoothPagedView::SMOOTHING_CONSTANT = (Float)(0.016 / Math::Log(SMOOTHING_SPEED));

SmoothPagedView::SmoothPagedView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mScrollMode(0)
    , mBaseLineFlingVelocity(0f)
    , mFlingVelocityInfluence(0f)
{
    SmoothPagedView(context, attrs, 0);
}

SmoothPagedView::SmoothPagedView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mScrollMode(0)
    , mBaseLineFlingVelocity(0f)
    , mFlingVelocityInfluence(0f)
{
    PagedView(context, attrs, defStyle);

    mUsePagingTouchSlop = FALSE;

    // This means that we'll take care of updating the scroll parameter ourselves (we do it
    // in computeScroll), we only do this in the OVERSHOOT_MODE, ie. on phones
    mDeferScrollUpdate = mScrollMode != X_LARGE_MODE;
}

Int32 SmoothPagedView::GetScrollMode()
{
    return X_LARGE_MODE;
}

ECode SmoothPagedView::Init()
{
    PagedView::Init();

    mScrollMode = getScrollMode();
    if (mScrollMode == DEFAULT_MODE) {
        mBaseLineFlingVelocity = 2500.0f;
        mFlingVelocityInfluence = 0.4f;
        mScrollInterpolator = new OvershootInterpolator();
        mScroller = new Scroller(getContext(), mScrollInterpolator);
    }
}

ECode SmoothPagedView::SnapToDestination()
{
    if (mScrollMode == X_LARGE_MODE) {
        return PagedView::SnapToDestination();
    }
    else {
        Int32 whichPage;
        GetPageNearestToCenterOfScreen(&whichPage);
        return SnapToPageWithVelocity(whichPage, 0);
    }
    return NOERROR;
}

ECode SmoothPagedView::SnapToPageWithVelocity(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 velocity)
{
    if (mScrollMode == X_LARGE_MODE) {
        return PagedView::SnapToPageWithVelocity(whichPage, velocity);
    }
    else {
        return SnapToPageWithVelocity(whichPage, 0, TRUE);
    }
    return NOERROR;
}

void SmoothPagedView::SnapToPageWithVelocity(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 velocity,
    /* [in] */ Boolean settle)
{
    // if (!mScroller.isFinished()) return;

    Int32 count;
    GetChildCount(&count);
    whichPage = Math::Max(0, Math::Min(whichPage, count - 1));

    const Int32 screenDelta = Math::Max(1, Math::Abs(whichPage - mCurrentPage));
    Int32 cOffset;
    GetChildOffset(whichPage, &cOffset);
    Int32 rOffset;
    GetRelativeChildOffset(whichPage, &rOffset);
    const Int32 newX = cOffset - rOffset;
    const Int32 delta = newX - mUnboundedScrollX;
    Int32 duration = (screenDelta + 1) * 100;

    Boolean res;
    mScroller->IsFinished(&res);
    if (!res) {
        mScroller->AbortAnimation();
    }

    if (settle) {
        IOvershootInterpolator::Probe(mScrollInterpolator)->SetDistance(screenDelta);
    }
    else {
        IOvershootInterpolator::Probe(mScrollInterpolator)->DisableSettle();
    }

    velocity = Math::Abs(velocity);
    if (velocity > 0) {
        duration += (duration / (velocity / mBaseLineFlingVelocity)) * mFlingVelocityInfluence;
    }
    else {
        duration += 100;
    }

    SnapToPage(whichPage, delta, duration);
}

ECode SmoothPagedView::SnapToPage(
    /* [in] */ Int32 whichPage)
{
    if (mScrollMode == X_LARGE_MODE) {
       return PagedView::SnapToPage(whichPage);
    }
    else {
       return SnapToPageWithVelocity(whichPage, 0, false);
    }
    return NOERROR;
}

ECode SmoothPagedView::ComputeScroll()
{
    if (mScrollMode == X_LARGE_MODE) {
        return PagedView::ComputeScroll();
    }
    else {
        Boolean scrollComputed;
        ComputeScrollHelper(&scrollComputed);

        if (!scrollComputed && mTouchState == TOUCH_STATE_SCROLLING) {
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 _now;
            system->GetNanoTime(&_now);
            const Float now = _now / NANOTIME_DIV;
            const Float e = (Float)Math::Exp((now - mSmoothingTime) / SMOOTHING_CONSTANT);

            const Float dx = mTouchX - mUnboundedScrollX;
            Int32 tmp;
            GetScrollY(&tmp);
            ScrollTo(Math::Round(mUnboundedScrollX + dx * e), tmp);
            mSmoothingTime = now;

            // Keep generating points as long as we're more than 1px away from the target
            if (dx > 1.f || dx < -1.f) {
                Invalidate();
            }
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos