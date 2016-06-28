
#include "elastos/droid/systemui/statusbar/DragDownHelper.h"
#include "R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Core::IInteger32;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

static const String TAG("DragDownHelper");

DragDownHelper::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ DragDownHelper* host,
    /* [in] */ IExpandableView* child)
    : mHost(host)
    , mChild(child)
{}

ECode DragDownHelper::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->SetUserLockedChild(IView::Probe(mChild), FALSE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(DragDownHelper::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

DragDownHelper::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ DragDownHelper* host)
    : mHost(host)
{}

ECode DragDownHelper::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Float value = 0;
    IFloat::Probe(obj)->GetValue(&value);
    mHost->mDragDownCallback->SetEmptyDragAmount(value);
    return NOERROR;
}

const Float DragDownHelper::RUBBERBAND_FACTOR_EXPANDABLE = 0.5f;
const Float DragDownHelper::RUBBERBAND_FACTOR_STATIC = 0.15f;
const Int32 DragDownHelper::SPRING_BACK_ANIMATION_LENGTH_MS = 375;

CAR_INTERFACE_IMPL_2(DragDownHelper, Object, IDragDownHelper, IGefingerpoken);

DragDownHelper::DragDownHelper(
    /* [in] */ IContext* context,
    /* [in] */ IView* host,
    /* [in] */ IExpandHelperCallback* callback,
    /* [in] */ IDragDownCallback* dragDownCallback)
    : mMinDragDistance(0)
    , mInitialTouchX(0)
    , mInitialTouchY(0)
    , mDraggingDown(FALSE)
    , mTouchSlop(0)
    , mDraggedFarEnough(FALSE)
    , mLastHeight(0)
{
    mTemp2 = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::keyguard_drag_down_min_distance, &mMinDragDistance);

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_slow_in,
        (IInterpolator**)&mInterpolator);

    AutoPtr<IViewConfiguration> configuration;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->Get(context, (IViewConfiguration**)&configuration);
    Int32 value = 0;
    configuration->GetScaledTouchSlop(&value);
    mTouchSlop = value;
    mCallback = callback;
    mDragDownCallback = dragDownCallback;
    mHost = host;
}

ECode DragDownHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Float x = 0;
    event->GetX(&x);
    Float y = 0;
    event->GetY(&y);

    Int32 masked = 0;
    switch (event->GetActionMasked(&masked), masked) {
        case IMotionEvent::ACTION_DOWN:
            mDraggedFarEnough = FALSE;
            mDraggingDown = FALSE;
            mStartingChild = NULL;
            mInitialTouchY = y;
            mInitialTouchX = x;
            break;

        case IMotionEvent::ACTION_MOVE: {
            Float h = y - mInitialTouchY;
            if (h > mTouchSlop && h > Elastos::Core::Math::Abs(x - mInitialTouchX)) {
                mDraggingDown = TRUE;
                CaptureStartingChild(mInitialTouchX, mInitialTouchY);
                mInitialTouchY = y;
                mInitialTouchX = x;
                mDragDownCallback->OnTouchSlopExceeded();
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DragDownHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (!mDraggingDown) {
        return NOERROR;
    }
    Float x = 0;
    event->GetX(&x);
    Float y = 0;
    event->GetY(&y);

    Int32 masked = 0;
    switch (event->GetActionMasked(&masked), masked) {
        case IMotionEvent::ACTION_MOVE:
            mLastHeight = y - mInitialTouchY;
            CaptureStartingChild(mInitialTouchX, mInitialTouchY);
            if (mStartingChild != NULL) {
                HandleExpansion(mLastHeight, mStartingChild);
            }
            else {
                mDragDownCallback->SetEmptyDragAmount(mLastHeight);
            }
            if (mLastHeight > mMinDragDistance) {
                if (!mDraggedFarEnough) {
                    mDraggedFarEnough = TRUE;
                    mDragDownCallback->OnThresholdReached();
                }
            }
            else {
                if (mDraggedFarEnough) {
                    mDraggedFarEnough = FALSE;
                    mDragDownCallback->OnDragDownReset();
                }
            }
            *result = TRUE;
            return NOERROR;
        case IMotionEvent::ACTION_UP: {
            Boolean tmp = FALSE;
            if (mDraggedFarEnough && (mDragDownCallback->OnDraggedDown(IView::Probe(mStartingChild), &tmp), tmp)) {
                if (mStartingChild == NULL) {
                    mDragDownCallback->SetEmptyDragAmount(0.f);
                }
                mDraggingDown = FALSE;
            }
            else {
                StopDragging();
                return NOERROR;
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
            StopDragging();
            return NOERROR;
    }
    return NOERROR;
}

void DragDownHelper::CaptureStartingChild(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (mStartingChild == NULL) {
        mStartingChild = FindView(x, y);
        if (mStartingChild != NULL) {
            mCallback->SetUserLockedChild(IView::Probe(mStartingChild), TRUE);
        }
    }
}

void DragDownHelper::HandleExpansion(
    /* [in] */ Float heightDelta,
    /* [in] */ IExpandableView* child)
{
    if (heightDelta < 0) {
        heightDelta = 0;
    }
    Boolean expandable = FALSE;
    child->IsContentExpandable(&expandable);
    Float rubberbandFactor = expandable
            ? RUBBERBAND_FACTOR_EXPANDABLE
            : RUBBERBAND_FACTOR_STATIC;
    Float rubberband = heightDelta * rubberbandFactor;

    Int32 minH = 0, maxH = 0;
    child->GetMinHeight(&minH);
    if (expandable && (rubberband + minH) > (child->GetMaxHeight(&maxH), maxH)) {
        Float overshoot = (rubberband + minH) - maxH;
        overshoot *= (1 - RUBBERBAND_FACTOR_STATIC);
        rubberband -= overshoot;
    }
    child->SetActualHeight((Int32) (minH + rubberband));
}

void DragDownHelper::CancelExpansion(
    /* [in] */ IExpandableView* child)
{
    Int32 minH = 0, height = 0;
    if ((child->GetActualHeight(&height), height) == (child->GetMinHeight(&minH), minH)) {
        return;
    }

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = height;
    (*array)[1] = minH;

    AutoPtr<IObjectAnimator> anim;
    helper->OfInt32(child, String("actualHeight"), array, (IObjectAnimator**)&anim);
    IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(anim)->SetDuration(SPRING_BACK_ANIMATION_LENGTH_MS);
    AutoPtr<AnimatorListenerAdapter1> adapter = new AnimatorListenerAdapter1(this, child);
    IAnimator::Probe(anim)->AddListener(adapter);
    IAnimator::Probe(anim)->Start();
}

void DragDownHelper::CancelExpansion()
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
    (*floats)[0] = mLastHeight;
    (*floats)[1] = 0;
    AutoPtr<IValueAnimator> anim;
    helper->OfFloat(floats, (IValueAnimator**)&anim);
    IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(anim)->SetDuration(SPRING_BACK_ANIMATION_LENGTH_MS);
    AutoPtr<AnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
    anim->AddUpdateListener(listener);
    IAnimator::Probe(anim)->Start();
}

void DragDownHelper::StopDragging()
{
    if (mStartingChild != NULL) {
        CancelExpansion(mStartingChild);
    } else {
        CancelExpansion();
    }
    mDraggingDown = FALSE;
    mDragDownCallback->OnDragDownReset();
}

AutoPtr<IExpandableView> DragDownHelper::FindView(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mHost->GetLocationOnScreen(mTemp2);
    x += (*mTemp2)[0];
    y += (*mTemp2)[1];
    AutoPtr<IExpandableView> view;
    mCallback->GetChildAtRawPosition(x, y, (IExpandableView**)&view);
    return view;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
