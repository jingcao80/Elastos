
#include "elastos/droid/launcher2/SmoothPagedView.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::View::Animation::COvershootInterpolator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::Widget::CScroller;
using Elastos::Droid::Widget::IScroller;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Float SmoothPagedView::OvershootInterpolator::DEFAULT_TENSION = 1.3f;

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

ECode SmoothPagedView::OvershootInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    // _o(t) = t * t * ((tension + 1) * t + tension)
    // o(t) = _o(t - 1) + 1
    t -= 1.0f;
    *result = t * t * ((mTension + 1) * t + mTension) + 1.0f;
    return NOERROR;
}

ECode SmoothPagedView::OvershootInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = FALSE;
    return NOERROR;
}

const Int32 SmoothPagedView::DEFAULT_MODE = 0;
const Int32 SmoothPagedView::X_LARGE_MODE = 1;

const Float SmoothPagedView::SMOOTHING_SPEED = 0.75f;
const Float SmoothPagedView::SMOOTHING_CONSTANT = (Float)(0.016 / Elastos::Core::Math::Log(SMOOTHING_SPEED));


SmoothPagedView::SmoothPagedView()
    : mScrollMode(0)
    , mBaseLineFlingVelocity(0.0f)
    , mFlingVelocityInfluence(0.0f)
{
}

ECode SmoothPagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode SmoothPagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    PagedView::constructor(context, attrs, defStyle);

    mUsePagingTouchSlop = FALSE;

    // This means that we'll take care of updating the scroll parameter ourselves (we do it
    // in computeScroll), we only do this in the OVERSHOOT_MODE, ie. on phones
    mDeferScrollUpdate = mScrollMode != X_LARGE_MODE;
    return NOERROR;
}

Int32 SmoothPagedView::GetScrollMode()
{
    return X_LARGE_MODE;
}

ECode SmoothPagedView::Init()
{
    PagedView::Init();

    mScrollMode = GetScrollMode();
    if (mScrollMode == DEFAULT_MODE) {
        mBaseLineFlingVelocity = 2500.0f;
        mFlingVelocityInfluence = 0.4f;
        mScrollInterpolator = new OvershootInterpolator();
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CScroller::New(context, mScrollInterpolator, (IScroller**)&mScroller);
    }
    return NOERROR;
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
        SnapToPageWithVelocity(whichPage, 0, TRUE);
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
    whichPage = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(whichPage, count - 1));

    const Int32 screenDelta = Elastos::Core::Math::Max(1, Elastos::Core::Math::Abs(whichPage - mCurrentPage));
    Int32 cOffset = GetChildOffset(whichPage);
    Int32 rOffset = GetRelativeChildOffset(whichPage);
    const Int32 newX = cOffset - rOffset;
    const Int32 delta = newX - mUnboundedScrollX;
    Int32 duration = (screenDelta + 1) * 100;

    Boolean res;
    mScroller->IsFinished(&res);
    if (!res) {
        mScroller->AbortAnimation();
    }

    if (settle) {
        ((OvershootInterpolator*)IObject::Probe(mScrollInterpolator))->SetDistance(screenDelta);
    }
    else {
        ((OvershootInterpolator*)IObject::Probe(mScrollInterpolator))->DisableSettle();
    }

    velocity = Elastos::Core::Math::Abs(velocity);
    if (velocity > 0) {
        duration += (duration / (velocity / mBaseLineFlingVelocity)) * mFlingVelocityInfluence;
    }
    else {
        duration += 100;
    }

    PagedView::SnapToPage(whichPage, delta, duration);
}

ECode SmoothPagedView::SnapToPage(
    /* [in] */ Int32 whichPage)
{
    if (mScrollMode == X_LARGE_MODE) {
        return PagedView::SnapToPage(whichPage);
    }
    else {
        SnapToPageWithVelocity(whichPage, 0, FALSE);
    }
    return NOERROR;
}

ECode SmoothPagedView::ComputeScroll()
{
    if (mScrollMode == X_LARGE_MODE) {
        return PagedView::ComputeScroll();
    }
    else {
        Boolean scrollComputed = ComputeScrollHelper();
        if (!scrollComputed && mTouchState == TOUCH_STATE_SCROLLING) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 _now;
            system->GetNanoTime(&_now);
            const Float now = _now / NANOTIME_DIV;
            const Float e = (Float)Elastos::Core::Math::Exp((now - mSmoothingTime) / SMOOTHING_CONSTANT);

            const Float dx = mTouchX - mUnboundedScrollX;
            Int32 tmp;
            GetScrollY(&tmp);
            ScrollTo(Elastos::Core::Math::Round(mUnboundedScrollX + dx * e), tmp);
            mSmoothingTime = now;

            // Keep generating points as long as we're more than 1px away from the target
            if (dx > 1.0f || dx < -1.0f) {
                Invalidate();
            }
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos