
#include "elastos/droid/launcher2/CellLayout.h"
#include <elastos/utility/Arrays.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/R.h>
#include "R.h"

using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_ADD;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::INinePatchDrawable;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::EIID_IComparator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CStack;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CellLayout::ReorderHintAnimation::AnimatorUpdateListener,
    Object, IAnimatorUpdateListener)

CellLayout::ReorderHintAnimation::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ ReorderHintAnimation* host)
    : mHost(host)
{}

ECode CellLayout::ReorderHintAnimation::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> val;
    animation->GetAnimatedValue((IInterface**)&val);
    Float r;
    IFloat::Probe(val)->GetValue(&r);
    Float x = r * mHost->mFinalDeltaX + (1 - r) * mHost->mInitDeltaX;
    Float y = r * mHost->mFinalDeltaY + (1 - r) * mHost->mInitDeltaY;
    mHost->mChild->SetTranslationX(x);
    mHost->mChild->SetTranslationY(y);
    Float s = r * mHost->mFinalScale + (1 - r) * mHost->mInitScale;
    mHost->mChild->SetScaleX(s);
    mHost->mChild->SetScaleY(s);
    return NOERROR;
}

CellLayout::ReorderHintAnimation::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ ReorderHintAnimation* host)
    : mHost(host)
{}

ECode CellLayout::ReorderHintAnimation::MyAnimatorListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    // We make sure to end only after a full period
    mHost->mInitDeltaX = 0.0f;
    mHost->mInitDeltaY = 0.0f;
    mHost->mInitScale = mHost->GetChildrenScale();
    return NOERROR;
}

const Int32 CellLayout::ReorderHintAnimation::DURATION;

CellLayout::ReorderHintAnimation::ReorderHintAnimation(
    /* [in] */ IView* child,
    /* [in] */ Int32 cellX0,
    /* [in] */ Int32 cellY0,
    /* [in] */ Int32 cellX1,
    /* [in] */ Int32 cellY1,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ CellLayout* host)
    : mHost(host)
{
    RegionToCenterPoint(cellX0, cellY0, spanX, spanY, mTmpPoint);
    const Int32 x0 = (*mTmpPoint)[0];
    const Int32 y0 = (*mTmpPoint)[1];
    RegionToCenterPoint(cellX1, cellY1, spanX, spanY, mTmpPoint);
    const Int32 x1 = (*mTmpPoint)[0];
    const Int32 y1 = (*mTmpPoint)[1];
    const Int32 dX = x1 - x0;
    const Int32 dY = y1 - y0;
    mFinalDeltaX = 0.0f;
    mFinalDeltaY = 0.0f;
    if (dX == dY && dX == 0) {
    }
    else {
        if (dY == 0) {
            mFinalDeltaX = - Elastos::Core::Math::Signum(dX) * mReorderHintAnimationMagnitude;
        }
        else if (dX == 0) {
            mFinalDeltaY = - Elastos::Core::Math::Signum(dY) * mReorderHintAnimationMagnitude;
        }
        else {
            Double angle = Elastos::Core::Math::Atan( (Float) (dY) / dX);
            mFinalDeltaX = (Int32) (- Elastos::Core::Math::Signum(dX) *
                    Elastos::Core::Math::Abs(Elastos::Core::Math::Cos(angle) * mReorderHintAnimationMagnitude));
            mFinalDeltaY = (Int32) (- Elastos::Core::Math::Signum(dY) *
                    Elastos::Core::Math::Abs(Elastos::Core::Math::Sin(angle) * mReorderHintAnimationMagnitude));
        }
    }
    child->GetTranslationX(&mInitDeltaX);
    child->GetTranslationY(&mInitDeltaY);
    Int32 width;
    child->GetWidth(&width);
    mFinalScale = GetChildrenScale() - 4.0f / width;
    child->GetScaleX(&mInitScale);
    mChild = child;
}

void CellLayout::ReorderHintAnimation::Animate()
{
    Boolean contains;
    mHost->mShakeAnimators->ContainsKey(mChild, &contains);
    if (contains) {
        AutoPtr<IInterface> value;
        mHost->mShakeAnimators->Get(mChild, (IInterface**)&value);
        ReorderHintAnimation* oldAnimation = (ReorderHintAnimation*)IObject::Probe(value);
        oldAnimation->Cancel();
        mHost->mShakeAnimators->Remove(mChild);
        if (mFinalDeltaX == 0 && mFinalDeltaY == 0) {
            CompleteAnimationImmediately();
            return;
        }
    }
    if (mFinalDeltaX == 0 && mFinalDeltaY == 0) {
        return;
    }
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(mChild, array);
    mA = va;
    va->SetRepeatMode(IValueAnimator::ANIMATION_REVERSE);
    va->SetRepeatCount(IValueAnimator::ANIMATION_INFINITE);
    va->SetDuration(DURATION);
    IAnimator::Probe(va)->SetStartDelay((Int32) (Elastos::Core::Math::Random() * 60));

    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
    va->AddUpdateListener(listener);
    AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(this);
    IAnimator::Probe(va)->AddListener(adapter);
    mHost->mShakeAnimators->Put(mChild, this);
    IAnimator::Probe(va)->Start();
}

void CellLayout::ReorderHintAnimation::Cancel()
{
    if (mA != NULL) {
        mA->Cancel();
    }
}

void CellLayout::ReorderHintAnimation::CompleteAnimationImmediately()
{
    if (mA != NULL) {
        mA->Cancel();
    }

    AutoPtr<IAnimatorSet> s = LauncherAnimUtils::CreateAnimatorSet();
    mA = IAnimator::Probe(s);
    AutoPtr<ArrayOf<IAnimator*> > items = ArrayOf<IAnimator*>::Alloc(4);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = GetChildrenScale();
    AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(mChild, String("scaleX"), array);
    (*items)[0] = va;
    va = LauncherAnimUtils::OfFloat(mChild, String("scaleY"), array);
    (*items)[1] = va;
    array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = 0.0f;
    va = LauncherAnimUtils::OfFloat(mChild, String("translationX"), array);
    (*items)[2] = va;
    va = LauncherAnimUtils::OfFloat(mChild, String("translationY"), array);
    (*items)[3] = va;
    s->PlayTogether(items);
    mA->SetDuration(REORDER_ANIMATION_DURATION);
    AutoPtr<ITimeInterpolator> interpolator;
    CDecelerateInterpolator::New(1.5f, (ITimeInterpolator**)&interpolator);
    mA->SetInterpolator(interpolator);
    mA->Start();
}

CellLayout::CellLayoutAnimationController::CellLayoutAnimationController(
    /* [in] */ IAnimation* animation,
    /* [in] */ Float delay)
{
    LayoutAnimationController::constructor(animation, delay);
}

Int64 CellLayout::CellLayoutAnimationController::GetDelayForView(
    /* [in] */ IView* view)
{
    return (Int32) (Elastos::Core::Math::Random() * 150);
}

CAR_INTERFACE_IMPL(CellLayout::LayoutParams, MarginLayoutParams, ICellLayoutLayoutParams)

CellLayout::LayoutParams::LayoutParams()
    : mCellX(0)
    , mCellY(0)
    , mTmpCellX(0)
    , mTmpCellY(0)
    , mUseTmpCoords(FALSE)
    , mCellHSpan(0)
    , mCellVSpan(0)
    , mIsLockedToGrid(TRUE)
    , mCanReorder(TRUE)
    , mX(0)
    , mY(0)
    , mDropped(FALSE)
{}

ECode CellLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    MarginLayoutParams::constructor(c, attrs);
    mCellHSpan = 1;
    mCellVSpan = 1;
    return NOERROR;
}

ECode CellLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    MarginLayoutParams::constructor(source);
    mCellHSpan = 1;
    mCellVSpan = 1;
    return NOERROR;
}

ECode CellLayout::LayoutParams::constructor(
    /* [in] */ LayoutParams* source)
{
    MarginLayoutParams::constructor(source);
    mCellX = source->mCellX;
    mCellY = source->mCellY;
    mCellHSpan = source->mCellHSpan;
    mCellVSpan = source->mCellVSpan;
    return NOERROR;
}

ECode CellLayout::LayoutParams::constructor(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 cellHSpan,
    /* [in] */ Int32 cellVSpan)
{
    MarginLayoutParams::constructor(MATCH_PARENT, MATCH_PARENT);
    mCellX = cellX;
    mCellY = cellY;
    mCellHSpan = cellHSpan;
    mCellVSpan = cellVSpan;
}

void CellLayout::LayoutParams::Setup(
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ Int32 widthGap,
    /* [in] */ Int32 heightGap,
    /* [in] */ Boolean invertHorizontally,
    /* [in] */ Int32 colCount)
{
    if (mIsLockedToGrid) {
        Int32 myCellHSpan = mCellHSpan;
        Int32 myCellVSpan = mCellVSpan;
        Int32 myCellX = mUseTmpCoords ? mTmpCellX : mCellX;
        Int32 myCellY = mUseTmpCoords ? mTmpCellY : mCellY;

        if (invertHorizontally) {
            myCellX = colCount - myCellX - mCellHSpan;
        }

        mWidth = myCellHSpan * cellWidth + ((myCellHSpan - 1) * widthGap) -
                mLeftMargin - mRightMargin;
        mHeight = myCellVSpan * cellHeight + ((myCellVSpan - 1) * heightGap) -
                mTopMargin - mBottomMargin;
        mX = (Int32) (myCellX * (cellWidth + widthGap) + mLeftMargin);
        mY = (Int32) (myCellY * (cellHeight + heightGap) + mTopMargin);
    }
}

ECode CellLayout::LayoutParams::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    str->AppendFormat("(%d, %d)", mCellX, mCellY);
    return NOERROR;
}

void CellLayout::LayoutParams::SetX(
    /* [in] */ Int32 x)
{
    mX = x;
}

Int32 CellLayout::LayoutParams::GetX()
{
    return mX;
}

void CellLayout::LayoutParams::SetY(
    /* [in] */ Int32 y)
{
    mY = y;
}

Int32 CellLayout::LayoutParams::GetY()
{
    return mY;
}

CellLayout::CellInfo::CellInfo()
    : mCellX(-1)
    , mCellY(-1)
    , mSpanX(0)
    , mSpanY(0)
    , mScreen(0)
    , mContainer(0)
{}

ECode CellLayout::CellInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    str->AppendFormat("Cell[view=%s, x=%d, y=%d]", TO_CSTR(mCell), mCellX, mCellY);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CellLayout::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

CellLayout::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ CellLayout* host,
    /* [in] */ InterruptibleInOutAnimator* anim,
    /* [in] */ Int32 thisIndex)
    : mHost(host)
    , mAnim(anim)
    , mThisIndex(thisIndex)
{}

ECode CellLayout::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IBitmap> outline = IBitmap::Probe(mAnim->GetTag());

    // If an animation is started and then stopped very quickly, we can still
    // get spurious updates we've cleared the tag. Guard against this.
    if (outline == NULL) {
        // @SuppressWarnings("all") // suppress dead code warning
        const Boolean debug = FALSE;
        if (debug) {
            AutoPtr<IInterface> val;
            animation->GetAnimatedValue((IInterface**)&val);
            Boolean isStopped;
            mAnim->IsStopped(&isStopped);
            Logger::D(TAG, "anim %d update: %s, isStopped %d",
                mThisIndex, TO_CSTR(val), isStopped);
        }
        // Try to prevent it from continuing to run
        IAnimation::Probe(animation)->Cancel();
    }
    else {
        AutoPtr<IInterface> val;
        animation->GetAnimatedValue((IInterface**)&val);
        IFloat(val)->GetValue(&(*mHost->mDragOutlineAlphas)[mThisIndex]);
        mHost->Invalidate((*mHost->mDragOutlines)[mThisIndex]);
    }
}

CellLayout::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ InterruptibleInOutAnimator* anim)
    : mAnim(anim)
{}

ECode CellLayout::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IInterface> val;
    IValueAnimator::Probe(animation)->GetAnimatedValue((IInterface**)&val);
    Float value;
    IFloat(val)->GetValue(&value);
    if (value == 0.0f) {
        mAnim->SetTag(NULL);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CellLayout::AnimatorUpdateListener2, Object, IAnimatorUpdateListener)

CellLayout::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ LayoutParams* lp,
    /* [in] */ Int32 oldX,
    /* [in] */ Int32 oldY,
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY,
    /* [in] */ IView* child)
    : mLp(lp)
    , mOldX(oldX)
    , mOldY(oldY)
    , mNewX(newX)
    , mNewY(newY)
    , mChild(child)
{}

ECode CellLayout::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> val;
    animation->GetAnimatedValue((IInterface**)&val);
    Float r;
    IFloat(val)->GetValue(&r);
    mLp->mX = (Int32) ((1 - r) * mOldX + r * mNewX);
    mLp->mY = (Int32) ((1 - r) * mOldY + r * mNewY);
    mChild->RequestLayout();
    return NOERROR;
}

CellLayout::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ CellLayout* mHost,
    /* [in] */ LayoutParams* lp,
    /* [in] */ IView* child)
    : mHost(host)
    : mLp(lp)
    , mChild(child)
    , mCancelled(FALSE)
{}

ECode CellLayout::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    // If the animation was cancelled, it means that another animation
    // has interrupted this one, and we don't want to lock the item into
    // place just yet.
    if (!mCancelled) {
        mLp->mIsLockedToGrid = TRUE;
        mChild->RequestLayout();
    }
    Boolean contain;
    mHost->mReorderAnimators->ContainsKey((IObject*)mLp, &contain);
    if (contain) {
        mHost->mReorderAnimators->Remove((IObject*)mLp);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CellLayout::ViewCluster::PositionComparator, Object, IComparator)

CellLayout::ViewCluster::PositionComparator::PositionComparator(
    /* [in] */ ViewCluster* host)
    : mHost(host)
    , mWhichEdge(0)
{}

ECode CellLayout::ViewCluster::PositionComparator::Compare(
    /* [in] */ IInterface* left,
    /* [in] */ IInterface* right,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> value;
    mHost->mConfig->mMap->Get(left, (IInterface**)&value);
    CellAndSpan* l = (CellAndSpan*)IObject::Probe(value);
    value = NULL;
    mHost->mConfig->mMap->Get(right, (IInterface**)&value);
    CellAndSpan* r = (CellAndSpan*)IObject::Probe(value);
    switch (mWhichEdge) {
        case LEFT:
            *result = (r->mX + r->mSpanX) - (l->mX + l->mSpanX);
            return NOERROR;
        case RIGHT:
            *result = l->mX - r->mX;
            return NOERROR;
        case TOP:
            *result = (r->mY + r->mSpanY) - (l->mY + l->mSpanY);
            return NOERROR;
        case BOTTOM:
        default:
            *result = l->mY - r->mY;
            return NOERROR;
    }
}

CellLayout::ViewCluster::ViewCluster(
    /* [in] */ IArrayList* views,
    /* [in] */ ItemConfiguration* config,
    /* [in] */ CellLayout* host)
    : mConfig(config)
    , mHost(host)
{
    CRect::New((IRect**)&mBoundingRect);
    mLeftEdge = ArrayOf<Int32>::Alloc(mHost->mCountY);
    mRightEdge = ArrayOf<Int32>::Alloc(mHost->mCountY);
    mTopEdge = ArrayOf<Int32>::Alloc(mHost->mCountX);
    mBottomEdge = ArrayOf<Int32>::Alloc(mHost->mCountX);
    views->Clone((IArrayList**)&mViews);
    mComparator = new PositionComparator(this);
    ResetEdges();
}

void CellLayout::ViewCluster::ResetEdges()
{
    for (Int32 i = 0; i < mHost->mCountX; i++) {
        (*mTopEdge)[i] = -1;
        (*mBottomEdge)[i] = -1;
    }
    for (Int32 i = 0; i < mHost->mCountY; i++) {
        (*mLeftEdge)[i] = -1;
        (*mRightEdge)[i] = -1;
    }
    mLeftEdgeDirty = TRUE;
    mRightEdgeDirty = TRUE;
    mBottomEdgeDirty = TRUE;
    mTopEdgeDirty = TRUE;
    mBoundingRectDirty = TRUE;
}

void CellLayout::ViewCluster::ComputeEdge(
    /* [in] */ Int32 which,
    /* [in] */ ArrayOf<Int32>* edge)
{
    Int32 count;
    mViews->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> key, value;
        mViews->Get(i, (IInterface**)&key);
        mConfig->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* cs = (CellAndSpan*)IObject::Probe(value);
        switch (which) {
            case LEFT:
                Int32 left = cs->mX;
                for (Int32 j = cs->mY; j < cs->mY + cs->mSpanY; j++) {
                    if (left < (*edge)[j] || (*edge)[j] < 0) {
                        (*edge)[j] = left;
                    }
                }
                break;
            case RIGHT:
                Int32 right = cs->mX + cs->mSpanX;
                for (Int32 j = cs->mY; j < cs->mY + cs->mSpanY; j++) {
                    if (right > (*edge)[j]) {
                        (*edge)[j] = right;
                    }
                }
                break;
            case TOP:
                Int32 top = cs->mY;
                for (Int32 j = cs->mX; j < cs->mX + cs->mSpanX; j++) {
                    if (top < (*edge)[j] || (*edge)[j] < 0) {
                        (*edge)[j] = top;
                    }
                }
                break;
            case BOTTOM:
                Int32 bottom = cs->mY + cs->mSpanY;
                for (Int32 j = cs->mX; j < cs->mX + cs->mSpanX; j++) {
                    if (bottom > (*edge)[j]) {
                        (*edge)[j] = bottom;
                    }
                }
                break;
        }
    }
}

Boolean CellLayout::ViewCluster::IsViewTouchingEdge(
    /* [in] */ IView* v,
    /* [in] */ Int32 whichEdge)
{
    AutoPtr<IInterface> value;
    mConfig->mMap->Get(v, (IInterface**)&value);
    CellAndSpan* cs = (CellAndSpan*)IObject::Probe(value);

    AutoPtr<ArrayOf<Int32> > edge = GetEdge(whichEdge);

    switch (whichEdge) {
        case LEFT:
            for (Int32 i = cs->mY; i < cs->mY + cs->mSpanY; i++) {
                if ((*edge)[i] == cs->mX + cs->mSpanX) {
                    return TRUE;
                }
            }
            break;
        case RIGHT:
            for (Int32 i = cs->mY; i < cs->mY + cs->mSpanY; i++) {
                if ((*edge)[i] == cs->mX) {
                    return TRUE;
                }
            }
            break;
        case TOP:
            for (Int32 i = cs->mX; i < cs->mX + cs->mSpanX; i++) {
                if ((*edge)[i] == cs->mY + cs->mSpanY) {
                    return TRUE;
                }
            }
            break;
        case BOTTOM:
            for (Int32 i = cs->mX; i < cs->mX + cs->mSpanX; i++) {
                if ((*edge)[i] == cs->mY) {
                    return TRUE;
                }
            }
            break;
    }
    return FALSE;
}

void CellLayout::ViewCluster::Shift(
    /* [in] */ Int32 whichEdge,
    /* [in] */ Int32 delta)
{
    Int32 count;
    mViews->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> key, value;
        mViews->Get(i, (IInterface**)&key);
        mConfig->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        switch (whichEdge) {
            case LEFT:
                c->mX -= delta;
                break;
            case RIGHT:
                c->mX += delta;
                break;
            case TOP:
                c->mY -= delta;
                break;
            case BOTTOM:
            default:
                c->mY += delta;
                break;
        }
    }
    ResetEdges();
}

void CellLayout::ViewCluster::AddView(
    /* [in] */ IView* v)
{
    mViews->Add(v);
    ResetEdges();
}

AutoPtr<IRect> CellLayout::ViewCluster::GetBoundingRect()
{
    if (mBoundingRectDirty) {
        Boolean first = TRUE;
        Int32 count;
        mViews->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> key, value;
            mViews->Get(i, (IInterface**)&key);
            mConfig->mMap->Get(key, (IInterface**)&value);
            CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
            if (first) {
                mBoundingRect->Set(c->mX, c->mY, c->mX + c->mSpanX, c->mY + c->mSpanY);
                first = FALSE;
            }
            else {
                mBoundingRect.union(c->mX, c->mY, c->mX + c->mSpanX, c->mY + c->mSpanY);
            }
        }
    }
    return mBoundingRect;
}

AutoPtr<ArrayOf<Int32> > CellLayout::ViewCluster::GetEdge(
    /* [in] */ Int32 which)
{
    switch (which) {
        case LEFT:
            return GetLeftEdge();
        case RIGHT:
            return GetRightEdge();
        case TOP:
            return GetTopEdge();
        case BOTTOM:
        default:
            return GetBottomEdge();
    }
}

AutoPtr<ArrayOf<Int32> > CellLayout::ViewCluster::GetLeftEdge()
{
    if (mLeftEdgeDirty) {
        ComputeEdge(LEFT, mLeftEdge);
    }
    return mLeftEdge;
}

AutoPtr<ArrayOf<Int32> > CellLayout::ViewCluster::GetRightEdge()
{
    if (mRightEdgeDirty) {
        ComputeEdge(RIGHT, mRightEdge);
    }
    return mRightEdge;
}

AutoPtr<ArrayOf<Int32> > CellLayout::ViewCluster::GetTopEdge()
{
    if (mTopEdgeDirty) {
        ComputeEdge(TOP, mTopEdge);
    }
    return mTopEdge;
}

AutoPtr<ArrayOf<Int32> > CellLayout::ViewCluster::GetBottomEdge()
{
    if (mBottomEdgeDirty) {
        ComputeEdge(BOTTOM, mBottomEdge);
    }
    return mBottomEdge;
}

void CellLayout::ViewCluster::SortConfigurationForEdgePush(
    /* [in] */ Int32 edge)
{
    mComparator->mWhichEdge = edge;
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(mConfig->mSortedViews, comparator);
}

CellLayout::ItemConfiguration::ItemConfiguration()
    : mIsSolution(FALSE)
    , mDragViewX(0)
    , mDragViewY(0)
    , mDragViewSpanX(0)
    , mDragViewSpanY(0)
{
    CHashMap::New((IHashMap**)&mMap);
    CArrayList::New((IArrayList**)&mSortedViews);
    CHashMap::New((IHashMap**)&mSavedMap);
}

void CellLayout::ItemConfiguration::Save()
{
    // Copy current state into savedMap
    AutoPtr<ISet> keySet;
    mMap->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> key = (*array)[i];
        AutoPtr<IInterface> val, savedVal;
        mMap->Get(key, (IInterface**)&val);
        mSavedMap->Get(key, (IInterface**)&savedVal);
        ((CellAndSpan*)IObject::Probe(val))->Copy(
            (CellAndSpan*)IObject::Probe(savedVal));
    }
}

void CellLayout::ItemConfiguration::Restore()
{
    // Restore current state from savedMap
    AutoPtr<ISet> keySet;
    mSavedMap->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> key = (*array)[i];
        AutoPtr<IInterface> val, savedVal;
        mMap->Get(key, (IInterface**)&val);
        mSavedMap->Get(key, (IInterface**)&savedVal);
        ((CellAndSpan*)IObject::Probe(savedVal))->Copy(
            (CellAndSpan*)IObject::Probe(val));
    }
}

void CellLayout::ItemConfiguration::Add(
    /* [in] */ IView* v,
    /* [in] */ CellAndSpan* cs)
{
    mMap->Put(v, (IObject*)cs);
    AutoPtr<CellAndSpan> scs = new CellAndSpan();
    mSavedMap->Put(v, (IObject*)scs);
    mSortedViews->Add(v);
}

Int32 CellLayout::ItemConfiguration::Area()
{
    return mDragViewSpanX * mDragViewSpanY;
}

CellLayout::CellAndSpan::CellAndSpan()
    : mX(0)
    , mY(0)
    , mSpanX(0)
    , mSpanY(0)
{
}

CellLayout::CellAndSpan::CellAndSpan(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
    : mX(x)
    , mY(y)
    , mSpanX(spanX)
    , mSpanY(spanY)
{
}

void CellLayout::CellAndSpan::Copy(
    /* [in] */ CellAndSpan* copy)
{
    copy->mX = mX;
    copy->mY = mY;
    copy->mSpanX = mSpanX;
    copy->mSpanY = mSpanY;
}

ECode CellLayout::CellAndSpan::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    str->AppendFormat("(%d, %d: %d, %d)", mX, mY, mSpanX, mSpanY);
    return NOERROR;
}

const String CellLayout::TAG("CellLayout");

const Int32 CellLayout::MODE_DRAG_OVER;
const Int32 CellLayout::MODE_ON_DROP;
const Int32 CellLayout::MODE_ON_DROP_EXTERNAL;
const Int32 CellLayout::MODE_ACCEPT_DROP;

const Int32 CellLayout::LANDSCAPE;
const Int32 CellLayout::PORTRAIT;

const Boolean CellLayout::DESTRUCTIVE_REORDER;
const Boolean CellLayout::DEBUG_VISUALIZE_OCCUPIED;
const Float CellLayout::REORDER_HINT_MAGNITUDE;
const Int32 CellLayout::REORDER_ANIMATION_DURATION;
const Int32 CellLayout::INVALID_DIRECTION;

static AutoPtr<IPorterDuffXfermode> InitAddBlendMode()
{
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_ADD, (IPorterDuffXfermode**)&mode);
    return mode
}
const AutoPtr<IPorterDuffXfermode> CellLayout::sAddBlendMode = InitAddBlendMode();

static AutoPtr<IPaint> InitPaint()
{
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    return paint
}
const AutoPtr<IPaint> CellLayout::sPaint = InitPaint();

CAR_INTERFACE_IMPL(CellLayout, ViewGroup, ICellLayout)

CellLayout::CellLayout()
    : mCellWidth(0)
    , mCellHeight(0)
    , mCountX(0)
    , mCountY(0)
    , mOriginalWidthGap(0)
    , mOriginalHeightGap(0)
    , mWidthGap(0)
    , mHeightGap(0)
    , mMaxGap(0)
    , mScrollingTransformsDirty(FALSE)
    , mLastDownOnOccupiedCell(FALSE)
    , mForegroundAlpha(0)
    , mBackgroundAlpha(0.0f)
    , mBackgroundAlphaMultiplier(1.0f)
    , mForegroundPadding(0)
    , mIsDragOverlapping(FALSE)
    , mDragOutlineCurrent(0)
    , mItemPlacementDirty(FALSE)
    , mDragging(FALSE)
    , mIsHotseat(FALSE)
    , mHotseatScale(1.0f)
    , mReorderHintAnimationMagnitude(0.0f)
{
    mTempLocation = ArrayOf<Int32>::Alloc(2);
    mPreviousReorderDirection = ArrayOf<Int32>::Alloc(2);
    CRect::New((IRect**)&mRect);
    mCellInfo = new CellInfo();
    mTmpXY = ArrayOf<Int32>::Alloc(2);
    mTmpPoint = ArrayOf<Int32>::Alloc(2);
    mFolderLeaveBehindCell = ArrayOf<Int32>::Alloc(2);
    (*mFolderLeaveBehindCell)[0] = -1;
    (*mFolderLeaveBehindCell)[1] = -1;
    CArrayList::New((IArrayList**)&mFolderOuterRings);
    CPoint::New((IPoint**)&mDragCenter);
    mDragOutlines = ArrayOf<IRect*>::Alloc(4) ;
    mDragOutlineAlphas = ArrayOf<Float>::Alloc(4) ;
    mDragOutlineAnims = ArrayOf<InterruptibleInOutAnimator*>::Alloc(4);
    CPaint::New((IPaint**)&mDragOutlinePaint);
    CHashMap::New((IHashMap**)&mReorderAnimators);
    CHashMap::New((IHashMap**)&mShakeAnimators);
    mDragCell = ArrayOf<Int32>::Alloc(2);
    CArrayList::New((IArrayList**)&mIntersectingViews);
    CRect::New((IRect**)&mOccupiedRect);
    mDirectionVector = ArrayOf<Int32>::Alloc(2);
    CRect::New((IRect**)&mTemp);
    CStack::New((IStack**)&mTempRectStack);
}

ECode CellLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CellLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CellLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyle));
    mDragEnforcer = new DropTarget::DragEnforcer(context);

    // A ViewGroup usually does not draw, but CellLayout needs to draw a rectangle to show
    // the user where a dragged item will land when dropped.
    SetWillNotDraw(FALSE);
    SetClipToPadding(FALSE);
    mLauncher = (Launcher*)context;

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, R::styleable::CellLayout, defStyle, 0, (ITypedArray**)&a);

    a->GetDimensionPixelSize(R::styleable::CellLayout_cellWidth, 10, &mCellWidth);
    a->GetDimensionPixelSize(R::styleable::CellLayout_cellHeight, 10, &mCellHeight);
    a->GetDimensionPixelSize(R::styleable::CellLayout_widthGap, 0, &mOriginalWidthGap);
    mWidthGap = mOriginalWidthGap;
    a->GetDimensionPixelSize(R::styleable::CellLayout_heightGap, 0, &mOriginalHeightGap);
    mHeightGap = mOriginalHeightGap;
    a->GetDimensionPixelSize(R::styleable::CellLayout_maxGap, 0, &mMaxGap);
    mCountX = LauncherModel::GetCellCountX();
    mCountY = LauncherModel::GetCellCountY();
    mOccupied = ArrayOf<ArrayOf<Boolean>* >::Alloc(mCountX);
    for (Int32 i = 0; i < mCountX; i++) {
        AutoPtr<ArrayOf<Boolean> > item = ArrayOf<Boolean>::Alloc(mCountY);
        mOccupied->Set(i, item);
    }
    mTmpOccupied = mOccupied->Clone();
    (*mPreviousReorderDirection)[0] = INVALID_DIRECTION;
    (*mPreviousReorderDirection)[1] = INVALID_DIRECTION;

    a->Recycle();

    SetAlwaysDrawnWithCacheEnabled(FALSE);

    AutoPtr<IResources> res = GetResources();
    res->GetInteger(R::integer::hotseat_item_scale_percentage, &mHotseatScale);
    mHotseatScale /= 100.0f;;

    res->GetDrawable(R::drawable::homescreen_blue_normal_holo, (IDrawable**)&mNormalBackground);
    res->GetDrawable(R::drawable::homescreen_blue_strong_holo, (IDrawable**)&mActiveGlowBackground);

    res->GetDrawable(R::drawable::overscroll_glow_left, (IDrawable**)&mOverScrollLeft);
    res->GetDrawable(R::drawable::overscroll_glow_right, (IDrawable**)&mOverScrollRight);
    res->GetDimensionPixelSize(R::dimen::workspace_overscroll_drawable_padding, &mForegroundPadding);

    Int32 iconSize;
    res->GetDimensionPixelSize(R::dimen::app_icon_size, &iconSize);
    mReorderHintAnimationMagnitude = (REORDER_HINT_MAGNITUDE * iconSize);

    mNormalBackground->SetFilterBitmap(TRUE);
    mActiveGlowBackground->SetFilterBitmap(TRUE);

    // Initialize the data structures used for the drag visualization.

    CDecelerateInterpolator::New(2.5f, (ITimeInterpolator**)&mEaseOutInterpolator); // Quint ease out

    (*mDragCell)[0] = (*mDragCell)[1] = -1;
    for (Int32 i = 0; i < mDragOutlines->GetLength(); i++) {
        AutoPtr<IRect> rect;
        CRect::New(-1, -1, -1, -1, (IRect**)&rect);
        mDragOutlines->Set(i, rect);
    }

    // When dragging things around the home screens, we show a green outline of
    // where the item will land. The outlines gradually fade out, leaving a trail
    // behind the drag path.
    // Set up all the animations that are used to implement this fading.
    Int32 duration;
    res->GetInteger(R::integer::config_dragOutlineFadeTime, &duration);
    Float fromAlphaValue = 0.0f;
    Int32 value;
    res->GetInteger(R::integer::config_dragOutlineMaxAlpha, &value);
    Float toAlphaValue = (Float)value;

    Arrays::Fill(mDragOutlineAlphas, fromAlphaValue);

    for (Int32 i = 0; i < mDragOutlineAnims->GetLength(); i++) {
        AutoPtr<InterruptibleInOutAnimator> anim =
            new InterruptibleInOutAnimator(this, duration, fromAlphaValue, toAlphaValue);
        anim->GetAnimator()->SetInterpolator(mEaseOutInterpolator);
        Int32 thisIndex = i;
        AutoPtr<AnimatorUpdateListener> listener = new AnimatorUpdateListener(this, anim, thisIndex);
        anim->GetAnimator()->AddUpdateListener(listener);
        // The animation holds a reference to the drag outline bitmap as long is it's
        // running. This way the bitmap can be GCed when the animations are complete.
        AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(anim);
        anim->GetAnimator()->AddListener(adapter);
        (*mDragOutlineAnims)[i] = anim;
    }

    CRect::New((IRect**)&mBackgroundRect);
    CRect::New((IRect**)&mForegroundRect);

    mShortcutsAndWidgets = new ShortcutAndWidgetContainer(context);
    mShortcutsAndWidgets->SetCellDimensions(mCellWidth, mCellHeight, mWidthGap, mHeightGap,
            mCountX);

    AddView(mShortcutsAndWidgets);
    return NOERROR;
}

Int32 CellLayout::WidthInPortrait(
    /* [in] */ IResources* r,
    /* [in] */ Int32 numCells)
{
    // We use this method from Workspace to figure out how many rows/columns Launcher should
    // have. We ignore the left/right padding on CellLayout because it turns out in our design
    // the padding extends outside the visible screen size, but it looked fine anyway.
    Int32 cellWidth;
    r->GetDimensionPixelSize(R::dimen::workspace_cell_width, &cellWidth);
    Int32 wgap, hgap;
    r->GetDimensionPixelSize(R::dimen::workspace_width_gap, &wgap);
    r->GetDimensionPixelSize(R::dimen::workspace_height_gap, &hgap);
    Int32 minGap = Elastos::Core::Math::Min(wgap, hgap);

    return  minGap * (numCells - 1) + cellWidth * numCells;
}

Int32 CellLayout::HeightInLandscape(
    /* [in] */ IResources* r,
    /* [in] */ Int32 numCells)
{
    // We use this method from Workspace to figure out how many rows/columns Launcher should
    // have. We ignore the left/right padding on CellLayout because it turns out in our design
    // the padding extends outside the visible screen size, but it looked fine anyway.
    Int32 cellHeight;
    r->GetDimensionPixelSize(R::dimen::workspace_cell_height, cellHeight);
    Int32 wgap, hgap;
    r->GetDimensionPixelSize(R::dimen::workspace_width_gap, &wgap);
    r->GetDimensionPixelSize(R::dimen::workspace_height_gap, &hgap);
    Int32 minGap = Elastos::Core::Math::Min(wgap, hgap);

    return minGap * (numCells - 1) + cellHeight * numCells;
}

void CellLayout::EnableHardwareLayers()
{
    mShortcutsAndWidgets->SetLayerType(LAYER_TYPE_HARDWARE, sPaint);
}

void CellLayout::DisableHardwareLayers()
{
    mShortcutsAndWidgets->SetLayerType(LAYER_TYPE_NONE, sPaint);
}

void CellLayout::BuildHardwareLayer()
{
    mShortcutsAndWidgets->BuildLayer();
}

Float CellLayout::GetChildrenScale()
{
    return mIsHotseat ? mHotseatScale : 1.0f;
}

void CellLayout::SetGridSize(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mCountX = x;
    mCountY = y;
    mOccupied = ArrayOf<ArrayOf<Boolean>* >::Alloc(mCountX);
    for (Int32 i = 0; i < mCountX; i++) {
        AutoPtr<ArrayOf<Boolean> > item = ArrayOf<Boolean>::Alloc(mCountY);
        mOccupied->Set(i, item);
    }
    mTmpOccupied = mOccupied->Clone();
    mTempRectStack->Clear();
    mShortcutsAndWidgets->SetCellDimensions(mCellWidth, mCellHeight, mWidthGap, mHeightGap,
            mCountX);
    RequestLayout();
}

// Set whether or not to invert the layout horizontally if the layout is in RTL mode.
void CellLayout::SetInvertIfRtl(
    /* [in] */ Boolean invert)
{
    mShortcutsAndWidgets->SetInvertIfRtl(invert);
}

void CellLayout::InvalidateBubbleTextView(
    /* [in] */ BubbleTextView* icon)
{
    Int32 padding;
    icon->GetPressedOrFocusedBackgroundPadding(&padding);
    Int32 l, t, r, b;
    icon->GetLeft(&l);
    icon->GetTop(&t);
    icon->GetRight(&r);
    icon->GetBottom(&b);
    Int32 pl, pt;
    GetPaddingLeft(&pl);
    GetPaddingTop(&pt);
    Invalidate(l + pl - padding, t + pt - padding,
            r + pl + padding, b + pt + padding);
}

void CellLayout::SetOverScrollAmount(
    /* [in] */ Float r,
    /* [in] */ Boolean left)
{
    if (left && mOverScrollForegroundDrawable != mOverScrollLeft) {
        mOverScrollForegroundDrawable = mOverScrollLeft;
    }
    else if (!left && mOverScrollForegroundDrawable != mOverScrollRight) {
        mOverScrollForegroundDrawable = mOverScrollRight;
    }

    mForegroundAlpha = (Int32) Elastos::Core::Math::Round((r * 255));
    mOverScrollForegroundDrawable->SetAlpha(mForegroundAlpha);
    Invalidate();
}

void CellLayout::SetPressedOrFocusedIcon(
    /* [in] */ BubbleTextView* icon)
{
    // We draw the pressed or focused BubbleTextView's background in CellLayout because it
    // requires an expanded clip rect (due to the glow's blur radius)
    AutoPtr<BubbleTextView> oldIcon = mPressedOrFocusedIcon;
    mPressedOrFocusedIcon = icon;
    if (oldIcon != NULL) {
        InvalidateBubbleTextView(oldIcon);
    }
    if (mPressedOrFocusedIcon != NULL) {
        InvalidateBubbleTextView(mPressedOrFocusedIcon);
    }
}

void CellLayout::SetIsDragOverlapping(
    /* [in] */ Boolean isDragOverlapping)
{
    if (mIsDragOverlapping != isDragOverlapping) {
        mIsDragOverlapping = isDragOverlapping;
        Invalidate();
    }
}

Boolean CellLayout::GetIsDragOverlapping()
{
    return mIsDragOverlapping;
}

void CellLayout::SetOverscrollTransformsDirty(
    /* [in] */ Boolean dirty)
{
    mScrollingTransformsDirty = dirty;
}

void CellLayout::ResetOverscrollTransforms()
{
    if (mScrollingTransformsDirty) {
        SetOverscrollTransformsDirty(FALSE);
        SetTranslationX(0);
        SetRotationY(0);
        // It doesn't matter if we pass TRUE or FALSE here, the important thing is that we
        // pass 0, which results in the overscroll drawable not being drawn any more.
        Int32 w, h;
        GetMeasuredWidth(&w);
        GetMeasuredHeight(&h);
        SetOverScrollAmount(0, FALSE);
        SetPivotX(w / 2);
        SetPivotY(h / 2);
    }
}

void CellLayout::ScaleRect(
    /* [in] */ IRect* r,
    /* [in] */ Float scale)
{
    if (scale != 1.0f) {
        Int32 left, top, right, bottom;
        r->GetLeft(&left);
        r->GetTop(&top);
        r->GetRight(&right);
        r->GetBottom(&bottom);
        r->SetLeft((Int32)(left * scale + 0.5f));
        r->SetTop((Int32)(top * scale + 0.5f));
        r->SetRight((Int32)(right * scale + 0.5f));
        r->SetBottom((Int32)(bottom * scale + 0.5f));
    }
}

void CellLayout::ScaleRectAboutCenter(
    /* [in] */ IRect* in,
    /* [in] */ IRect* out,
    /* [in] */ Float scale)
{
    Int32 cx, cy;
    in->GetCenterX(&cx);
    in->GetCenterY(&cy);
    out->Set(in);
    out->Offset(-cx, -cy);
    ScaleRect(out, scale);
    out->Offset(cx, cy);
}

void CellLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    // When we're large, we are either drawn in a "hover" state (ie when dragging an item to
    // a neighboring page) or with just a normal background (if backgroundAlpha > 0.0f)
    // When we're small, we are either drawn normally or in the "accepts drops" state (during
    // a drag). However, we also drag the mini hover background *over* one of those two
    // backgrounds
    if (mBackgroundAlpha > 0.0f) {
        AutoPtr<IDrawable> bg;

        if (mIsDragOverlapping) {
            // In the mini case, we draw the active_glow bg *over* the active background
            bg = mActiveGlowBackground;
        }
        else {
            bg = mNormalBackground;
        }

        bg->SetAlpha((Int32) (mBackgroundAlpha * mBackgroundAlphaMultiplier * 255));
        bg->SetBounds(mBackgroundRect);
        bg->Draw(canvas);
    }

    AutoPtr<IPaint> paint = mDragOutlinePaint;
    for (Int32 i = 0; i < mDragOutlines->GetLength(); i++) {
        Float alpha = (*mDragOutlineAlphas)[i];
        if (alpha > 0) {
            AutoPtr<IRect> r = (*mDragOutlines)[i];
            ScaleRectAboutCenter(r, mTemp, GetChildrenScale());
            AutoPtr<IBitmap> b = IBitmap::Probe((*mDragOutlineAnims)[i]->GetTag());
            paint->SetAlpha((Int32)(alpha + .5f));
            canvas->DrawBitmap(b, NULL, mTemp, paint);
        }
    }

    // We draw the pressed or focused BubbleTextView's background in CellLayout because it
    // requires an expanded clip rect (due to the glow's blur radius)
    if (mPressedOrFocusedIcon != NULL) {
        Int32 padding;
        mPressedOrFocusedIcon->GetPressedOrFocusedBackgroundPadding(&padding);
        AutoPtr<IBitmap> b;
        mPressedOrFocusedIcon->GetPressedOrFocusedBackground((IBitmap**)&b);
        if (b != NULL) {
            Int32 l, t, pl, pt;
            mPressedOrFocusedIcon->GetLeft(&l);
            mPressedOrFocusedIcon->GetTop(&t);
            GetPaddingLeft(&pl);
            GetPaddingTop(&pt);
            canvas->DrawBitmap(b, l + pl - padding, t + pt - padding, NULL);
        }
    }

    if (DEBUG_VISUALIZE_OCCUPIED) {
        AutoPtr<ArrayOf<Int32> > pt = ArrayOf<Int32>::Alloc(2);
        AutoPtr<IColorDrawable> cd;
        CColorDrawable::New(IColor::RED, (IColorDrawable**)&cd);
        cd->SetBounds(0, 0,  mCellWidth, mCellHeight);
        for (Int32 i = 0; i < mCountX; i++) {
            for (Int32 j = 0; j < mCountY; j++) {
                if ((*(*mOccupied)[i])[j]) {
                    CellToPoint(i, j, pt);
                    canvas->Save();
                    canvas->Translate((*pt)[0], (*pt)[1]);
                    cd->Draw(canvas);
                    canvas->Restore();
                }
            }
        }
    }

    Int32 previewOffset = FolderIcon::FolderRingAnimator::sPreviewSize;

    // The folder outer / inner ring image(s)
    Int32 size;
    mFolderOuterRings->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mFolderOuterRings->Get(i, (IInterface**)&item);
        FolderIcon::FolderRingAnimator* fra = (FolderIcon::FolderRingAnimator*)IObject::Probe(item);

        // Draw outer ring
        AutoPtr<IDrawable> d = FolderIcon::FolderRingAnimator::sSharedOuterRingDrawable;
        Int32 width = (Int32) fra->GetOuterRingSize();
        Int32 height = width;
        CellToPoint(fra->mCellX, fra->mCellY, mTempLocation);

        Int32 centerX = (*mTempLocation)[0] + mCellWidth / 2;
        Int32 centerY = (*mTempLocation)[1] + previewOffset / 2;

        canvas->Save();
        canvas->Translate(centerX - width / 2, centerY - height / 2);
        d->SetBounds(0, 0, width, height);
        d->Draw(canvas);
        canvas->Restore();

        // Draw inner ring
        d = FolderIcon::FolderRingAnimator::sSharedInnerRingDrawable;
        width = (Int32) fra->GetInnerRingSize();
        height = width;
        CellToPoint(fra->mCellX, fra->mCellY, mTempLocation);

        centerX = (*mTempLocation)[0] + mCellWidth / 2;
        centerY = (*mTempLocation)[1] + previewOffset / 2;
        canvas->Save();
        canvas->Translate(centerX - width / 2, centerY - width / 2);
        d->SetBounds(0, 0, width, height);
        d->Draw(canvas);
        canvas->Restore();
    }

    if ((*mFolderLeaveBehindCell)[0] >= 0 && (*mFolderLeaveBehindCell)[1] >= 0) {
        AutoPtr<IDrawable> d = FolderIcon::sSharedFolderLeaveBehind;
        Int32 width, height;
        d->GetIntrinsicWidth(&width);
        d->GetIntrinsicHeight(&height);

        CellToPoint((*mFolderLeaveBehindCell)[0], (*mFolderLeaveBehindCell)[1], mTempLocation);
        Int32 centerX = (*mTempLocation)[0] + mCellWidth / 2;
        Int32 centerY = (*mTempLocation)[1] + previewOffset / 2;

        canvas->Save();
        canvas->Translate(centerX - width / 2, centerY - width / 2);
        d->SetBounds(0, 0, width, height);
        d->Draw(canvas);
        canvas->Restore();
    }
}

void CellLayout::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    ViewGroup::DispatchDraw(canvas);
    if (mForegroundAlpha > 0) {
        mOverScrollForegroundDrawable->SetBounds(mForegroundRect);
        AutoPtr<IPaint> p;
        INinePatchDrawable::Probe(mOverScrollForegroundDrawable)->GetPaint((IPaint**)&p);
        p->SetXfermode(sAddBlendMode);
        mOverScrollForegroundDrawable->Draw(canvas);
        p->SetXfermode(NULL);
    }
}

void CellLayout::ShowFolderAccept(
    /* [in] */ FolderIcon::FolderRingAnimator* fra)
{
    mFolderOuterRings->Add((IObject*)fra);
}

void CellLayout::HideFolderAccept(
    /* [in] */ FolderIcon::FolderRingAnimator* fra)
{
    if (mFolderOuterRings->Contains((IObject*)fra)) {
        mFolderOuterRings->Remove((IObject*)fra);
    }
    Invalidate();
}

void CellLayout::SetFolderLeaveBehindCell(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    (*mFolderLeaveBehindCell)[0] = x;
    (*mFolderLeaveBehindCell)[1] = y;
    Invalidate();
}

void CellLayout::ClearFolderLeaveBehind()
{
    (*mFolderLeaveBehindCell)[0] = -1;
    (*mFolderLeaveBehindCell)[1] = -1;
    Invalidate();
}

ECode CellLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

void CellLayout::RestoreInstanceState(
    /* [in] */ ISparseArray* states)
{
    DispatchRestoreInstanceState(states);
}

ECode CellLayout::CancelLongPress()
{
    ViewGroup::CancelLongPress();

    // Cancel long press for all children
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->CancelLongPress();
    }
    return NOERROR;
}

void CellLayout::SetOnInterceptTouchListener(
    /* [in] */ IViewOnTouchListener* listener)
{
    mInterceptTouchListener = listener;
}

Int32 CellLayout::GetCountX()
{
    return mCountX;
}

Int32 CellLayout::GetCountY()
{
    return mCountY;
}

void CellLayout::SetIsHotseat(
    /* [in] */ Boolean isHotseat)
{
    mIsHotseat = isHotseat;
}

Boolean CellLayout::AddViewToCellLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ Int32 childId,
    /* [in] */ LayoutParams* params,
    /* [in] */ Boolean markCells)
{
    LayoutParams* lp = params;

    // Hotseat icons - remove text
    if (IBubbleTextView::Probe(child)) {
        AutoPtr<IBubbleTextView> bubbleChild = (BubbleTextView*)IBubbleTextView::Probe(child);

        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Int32 color;
        if (mIsHotseat) {
            res->GetColor(Elastos::Droid::R::color::transparent, &color);
            bubbleChild->SetTextColor(color);
        }
        else {
            res->GetColor(R::color::workspace_icon_text_color, &color);
            bubbleChild->SetTextColor(color);
        }
    }

    child->SetScaleX(GetChildrenScale());
    child->SetScaleY(GetChildrenScale());

    // Generate an id for each view, this assumes we have at most 256x256 cells
    // per workspace screen
    if (lp->mCellX >= 0 && lp->mCellX <= mCountX - 1 && lp->mCellY >= 0 && lp->mCellY <= mCountY - 1) {
        // If the horizontal or vertical span is set to -1, it is taken to
        // mean that it spans the extent of the CellLayout
        if (lp->mCellHSpan < 0) lp->mCellHSpan = mCountX;
        if (lp->mCellVSpan < 0) lp->mCellVSpan = mCountY;

        child->SetId(childId);

        mShortcutsAndWidgets->AddView(child, index, lp);

        if (markCells) MarkCellsAsOccupiedForView(child);

        return TRUE;
    }
    return FALSE;
}

ECode CellLayout::RemoveAllViews()
{
    ClearOccupiedCells();
    return mShortcutsAndWidgets->RemoveAllViews();
    return NOERROR;
}

ECode CellLayout::RemoveAllViewsInLayout()
{
    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    if (childCount > 0) {
        ClearOccupiedCells();
        mShortcutsAndWidgets->RemoveAllViewsInLayout();
    }
    return NOERROR;
}

ECode CellLayout::RemoveViewWithoutMarkingCells(
    /* [in] */ IView* view)
{
    mShortcutsAndWidgets->RemoveView(view);
    return NOERROR;
}

ECode CellLayout::RemoveView(
    /* [in] */ IView* view)
{
    MarkCellsAsUnoccupiedForView(view);
    mShortcutsAndWidgets->RemoveView(view);
    return NOERROR;
}

ECode CellLayout::RemoveViewAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IView> child;
    mShortcutsAndWidgets->GetChildAt(index, (IView**)&child);
    MarkCellsAsUnoccupiedForView(child);
    mShortcutsAndWidgets->RemoveViewAt(index);
    return NOERROR;
}

ECode CellLayout::RemoveViewInLayout(
    /* [in] */ IView* view)
{
    MarkCellsAsUnoccupiedForView(view);
    mShortcutsAndWidgets->RemoveViewInLayout(view);
    return NOERROR;
}

ECode CellLayout::RemoveViews(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    for (Int32 i = start; i < start + count; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        MarkCellsAsUnoccupiedForView(child);
    }
    mShortcutsAndWidgets->RemoveViews(start, count);
    return NOERROR;
}

ECode CellLayout::RemoveViewsInLayout(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    for (Int32 i = start; i < start + count; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        MarkCellsAsUnoccupiedForView(child);
    }
    mShortcutsAndWidgets->RemoveViewsInLayout(start, count);
    return NOERROR;
}

ECode CellLayout::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    IViewGroup::Probe(parent)->IndexOfChild(this, &mCellInfo->mScreen);
    return NOERROR;
}

void CellLayout::SetTagToCellInfoForPoint(
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY)
{
    CellInfo* cellInfo = mCellInfo;
    AutoPtr<IRect> frame = mRect;
    Int32 x, y;
    GetScrollX(&x);
    x += touchX;
    GetScrollY(&y);
    x += touchY;
    Int32 count;
    mShortcutsAndWidgets->GetChildCount(&count);

    Boolean found = FALSE;
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();

        Int32 v;
        child->GetVisibility(&v);
        AutoPtr<IAnimation> anim;
        if ((v == VISIBLE || (child->GetAnimation((IAnimation**)&anim), anim != NULL) &&
                lp->mIsLockedToGrid) {
            child->GetHitRect(frame);

            Float scale;
            child->GetScaleX(&scale);
            Int32 l, t, r, b;
            child->GetLeft(&l);
            child->GetTop(&t);
            child->GetRight(&r);
            child->GetBottom(&b);
            frame = NULL;
            CRect::New(l, t, r, b, (IRect**)&frame);
            // The child hit rect is relative to the CellLayoutChildren parent, so we need to
            // offset that by this CellLayout's padding to test an (x,y) point that is relative
            // to this view.
            Int32 pl, pt;
            GetPaddingLeft(&pl);
            GetPaddingTop(&pt);
            frame->Offset(pl, pt);
            Int32 width, height;
            frame->GetWidth(&width);
            frame->GetHeight(&height);
            frame->Inset((Int32) (width * (1.0f - scale) / 2),
                    (Int32) (height * (1.0f - scale) / 2));

            Boolean contains;
            frame->Contains(x, y, &contains);
            if (contains) {
                cellInfo->mCell = child;
                cellInfo->mCellX = lp->mCellX;
                cellInfo->mCellY = lp->mCellY;
                cellInfo->mSpanX = lp->mCellHSpan;
                cellInfo->mSpanY = lp->mCellVSpan;
                found = TRUE;
                break;
            }
        }
    }

    mLastDownOnOccupiedCell = found;

    if (!found) {
        AutoPtr<ArrayOf<Int32> > cellXY = mTmpXY;
        PointToCellExact(x, y, cellXY);

        cellInfo->mCell = NULL;
        cellInfo->mCellX = (*cellXY)[0];
        cellInfo->mCellY = (*cellXY)[1];
        cellInfo->mSpanX = 1;
        cellInfo->mSpanY = 1;
    }
    SetTag((IObject*)cellInfo);
}

ECode CellLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // First we clear the tag to ensure that on every touch down we start with a fresh slate,
    // even in the case where we return early. Not clearing here was causing bugs whereby on
    // long-press we'd end up picking up an item from a previous drag operation.
    Int32 action;
    ev->GetAction(&action);

    if (action == IMotionEvent::ACTION_DOWN) {
        ClearTagCellInfo();
    }

    Boolean res;
    if (mInterceptTouchListener != NULL && (mInterceptTouchListener->OnTouch(this, ev, &res), res)) {
        *result = TRUE;
        return NOERROR;
    }

    if (action == IMotionEvent::ACTION_DOWN) {
        Float x, y;
        ev->GetX(&x);
        ev->GetY(&y);
        SetTagToCellInfoForPoint((Int32)x, (Int32)y);
    }

    *result = FALSE;
    return NOERROR;
}

void CellLayout::ClearTagCellInfo()
{
    CellInfo* cellInfo = mCellInfo;
    cellInfo->mCell = NULL;
    cellInfo->mCellX = -1;
    cellInfo->mCellY = -1;
    cellInfo->mSpanX = 0;
    cellInfo->mSpanY = 0;
    SetTag((IObject*)cellInfo);
}

AutoPtr<CellLayout::CellInfo> CellLayout::GetTag()
{
    AutoPtr<IInterface> tag;
    ViewGroup::GetTag((IInterface**)&tag);
    return (CellInfo*)IObject::Probe(tag);
}

void CellLayout::PointToCellExact(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ ArrayOf<Int32>* result)
{
    Int32 hStartPadding, vStartPadding;
    GetPaddingLeft(&hStartPadding);
    GetPaddingTop(&vStartPadding);

    (*result)[0] = (x - hStartPadding) / (mCellWidth + mWidthGap);
    (*result)[1] = (y - vStartPadding) / (mCellHeight + mHeightGap);

    const Int32 xAxis = mCountX;
    const Int32 yAxis = mCountY;

    if ((*result)[0] < 0) (*result)[0] = 0;
    if ((*result)[0] >= xAxis) (*result)[0] = xAxis - 1;
    if ((*result)[1] < 0) (*result)[1] = 0;
    if ((*result)[1] >= yAxis) (*result)[1] = yAxis - 1;
}

void CellLayout::PointToCellRounded(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ ArrayOf<Int32>* result)
{
    PointToCellExact(x + (mCellWidth / 2), y + (mCellHeight / 2), result);
}

void CellLayout::CellToPoint(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ ArrayOf<Int32>* result)
{
    Int32 hStartPadding, vStartPadding;
    GetPaddingLeft(&hStartPadding);
    GetPaddingTop(&vStartPadding);

    (*result)[0] = hStartPadding + cellX * (mCellWidth + mWidthGap);
    (*result)[1] = vStartPadding + cellY * (mCellHeight + mHeightGap);
}

void CellLayout::CellToCenterPoint(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ ArrayOf<Int32>* result)
{
    RegionToCenterPoint(cellX, cellY, 1, 1, result);
}

void CellLayout::RegionToCenterPoint(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* result)
{
    Int32 hStartPadding, vStartPadding;
    GetPaddingLeft(&hStartPadding);
    GetPaddingTop(&vStartPadding);
    (*result)[0] = hStartPadding + cellX * (mCellWidth + mWidthGap) +
            (spanX * mCellWidth + (spanX - 1) * mWidthGap) / 2;
    (*result)[1] = vStartPadding + cellY * (mCellHeight + mHeightGap) +
            (spanY * mCellHeight + (spanY - 1) * mHeightGap) / 2;
}

void CellLayout::RegionToRect(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IRect* result)
 {
    Int32 hStartPadding, vStartPadding;
    GetPaddingLeft(&hStartPadding);
    GetPaddingTop(&vStartPadding);
    Int32 left = hStartPadding + cellX * (mCellWidth + mWidthGap);
    Int32 top = vStartPadding + cellY * (mCellHeight + mHeightGap);
    result->Set(left, top, left + (spanX * mCellWidth + (spanX - 1) * mWidthGap),
            top + (spanY * mCellHeight + (spanY - 1) * mHeightGap));
}

Float CellLayout::GetDistanceFromCell(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ ArrayOf<Int32>* cell)
{
    CellToCenterPoint((*cell)[0], (*cell)[1], mTmpPoint);
    Float distance = (Float) Elastos::Core::Math::Sqrt( Elastos::Core::Math::Pow(x - (*mTmpPoint)[0], 2) +
            Elastos::Core::Math::Pow(y - (*mTmpPoint)[1], 2));
    return distance;
}

Int32 CellLayout::GetCellWidth()
{
    return mCellWidth;
}

Int32 CellLayout::GetCellHeight()
{
    return mCellHeight;
}

Int32 CellLayout::GetWidthGap()
{
    return mWidthGap;
}

Int32 CellLayout::GetHeightGap()
{
    return mHeightGap;
}

AutoPtr<IRect> CellLayout::GetContentRect(
    /* [in] */ IRect* r)
{
    if (r == NULL) {
        CRect::New((IRect**)&r);
    }
    Int32 left, top, right, bottom;
    GetPaddingLeft(&left);
    GetPaddingTop(&top);
    GetPaddingRight(&right);
    GetPaddingBottom(&bottom);
    Int32 w, h;
    GetWidth(&w);
    GetHeight(&h);
    right = left + w - left - right;
    bottom = top + h - top - bottom;
    r->Set(left, top, right, bottom);
    return r;
}

void CellLayout::GetMetrics(
    /* [in] */ IRect* metrics,
    /* [in] */ IResources* res,
    /* [in] */ Int32 measureWidth,
    /* [in] */ Int32 measureHeight,
    /* [in] */ Int32 countX,
    /* [in] */ Int32 countY,
    /* [in] */ Int32 orientation)
{
    Int32 numWidthGaps = countX - 1;
    Int32 numHeightGaps = countY - 1;

    Int32 widthGap;
    Int32 heightGap;
    Int32 cellWidth;
    Int32 cellHeight;
    Int32 paddingLeft;
    Int32 paddingRight;
    Int32 paddingTop;
    Int32 paddingBottom;

    Int32 maxGap;
    res->GetDimensionPixelSize(R::dimen::workspace_max_gap, &maxGap);
    if (orientation == LANDSCAPE) {
        res->GetDimensionPixelSize(R::dimen::workspace_cell_width_land, &cellWidth);
        res->GetDimensionPixelSize(R::dimen::workspace_cell_height_land, &cellHeight);
        res->GetDimensionPixelSize(R::dimen::workspace_width_gap_land, &widthGap);
        res->GetDimensionPixelSize(R::dimen::workspace_height_gap_land, &heightGap);
        res->GetDimensionPixelSize(R::dimen::cell_layout_left_padding_land, &paddingLeft);
        res->GetDimensionPixelSize(R::dimen::cell_layout_right_padding_land, &paddingRight);
        res->GetDimensionPixelSize(R::dimen::cell_layout_top_padding_land, &paddingTop);
        res->GetDimensionPixelSize(R::dimen::cell_layout_bottom_padding_land, &paddingBottom);
    }
    else {
        // PORTRAIT
        res->GetDimensionPixelSize(R::dimen::workspace_cell_width_port, &cellWidth);
        res->GetDimensionPixelSize(R::dimen::workspace_cell_height_port, &cellHeight);
        res->GetDimensionPixelSize(R::dimen::workspace_width_gap_port, &widthGap);
        res->GetDimensionPixelSize(R::dimen::workspace_height_gap_port, &heightGap);
        res->GetDimensionPixelSize(R::dimen::cell_layout_left_padding_port, &paddingLeft);
        res->GetDimensionPixelSize(R::dimen::cell_layout_right_padding_port, &paddingRight);
        res->GetDimensionPixelSize(R::dimen::cell_layout_top_padding_port, &paddingTop);
        res->GetDimensionPixelSize(R::dimen::cell_layout_bottom_padding_port, &paddingBottom);
    }

    if (widthGap < 0 || heightGap < 0) {
        Int32 hSpace = measureWidth - paddingLeft - paddingRight;
        Int32 vSpace = measureHeight - paddingTop - paddingBottom;
        Int32 hFreeSpace = hSpace - (countX * cellWidth);
        Int32 vFreeSpace = vSpace - (countY * cellHeight);
        widthGap = Elastos::Core::Math::Min(maxGap, numWidthGaps > 0 ? (hFreeSpace / numWidthGaps) : 0);
        heightGap = Elastos::Core::Math::Min(maxGap, numHeightGaps > 0 ? (vFreeSpace / numHeightGaps) : 0);
    }
    metrics->Set(cellWidth, cellHeight, widthGap, heightGap);
}

void CellLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSpecSize = MeasureSpec::GetSize(widthMeasureSpec);

    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSpecSize =  MeasureSpec::GetSize(heightMeasureSpec);

    if (widthSpecMode == MeasureSpec::UNSPECIFIED || heightSpecMode == MeasureSpec::UNSPECIFIED) {
        // throw new RuntimeException("CellLayout cannot have UNSPECIFIED dimensions");
        Logger::E(TAG, "CellLayout cannot have UNSPECIFIED dimensions");
        return;
    }

    Int32 numWidthGaps = mCountX - 1;
    Int32 numHeightGaps = mCountY - 1;
    Int32 left, top, right, bottom;
    GetPaddingLeft(&left);
    GetPaddingTop(&top);
    GetPaddingRight(&right);
    GetPaddingBottom(&bottom);
    if (mOriginalWidthGap < 0 || mOriginalHeightGap < 0) {
        Int32 hSpace = widthSpecSize - left - right;
        Int32 vSpace = heightSpecSize - top - bottom;
        Int32 hFreeSpace = hSpace - (mCountX * mCellWidth);
        Int32 vFreeSpace = vSpace - (mCountY * mCellHeight);
        mWidthGap = Elastos::Core::Math::Min(mMaxGap, numWidthGaps > 0 ? (hFreeSpace / numWidthGaps) : 0);
        mHeightGap = Elastos::Core::Math::Min(mMaxGap, numHeightGaps > 0 ? (vFreeSpace / numHeightGaps) : 0);
        mShortcutsAndWidgets->SetCellDimensions(mCellWidth, mCellHeight, mWidthGap, mHeightGap,
                mCountX);
    }
    else {
        mWidthGap = mOriginalWidthGap;
        mHeightGap = mOriginalHeightGap;
    }

    // Initial values correspond to widthSpecMode == MeasureSpec::EXACTLY
    Int32 newWidth = widthSpecSize;
    Int32 newHeight = heightSpecSize;
    if (widthSpecMode == MeasureSpec::AT_MOST) {
        newWidth = left + right + (mCountX * mCellWidth) +
            ((mCountX - 1) * mWidthGap);
        newHeight = top + bottom + (mCountY * mCellHeight) +
            ((mCountY - 1) * mHeightGap);
        SetMeasuredDimension(newWidth, newHeight);
    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(newWidth - left -
                right, MeasureSpec::EXACTLY);
        Int32 childheightMeasureSpec = MeasureSpec::MakeMeasureSpec(newHeight - top -
                bottom, MeasureSpec::EXACTLY);
        child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    }
    SetMeasuredDimension(newWidth, newHeight);
}

ECode CellLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 count;
    GetChildCount(&count);
    Int32 left, top, right, bottom;
    GetPaddingLeft(&left);
    GetPaddingTop(&top);
    GetPaddingRight(&right);
    GetPaddingBottom(&bottom);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->Layout(left, top, r - l - right, b - t - bottom);
    }
    return NOERROR;
}

void CellLayout::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    ViewGroup::OnSizeChanged(w, h, oldw, oldh);
    mBackgroundRect->Set(0, 0, w, h);
    mForegroundRect->Set(mForegroundPadding, mForegroundPadding,
            w - mForegroundPadding, h - mForegroundPadding);
}

void CellLayout::SetChildrenDrawingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    mShortcutsAndWidgets->SetChildrenDrawingCacheEnabled(enabled);
}

void CellLayout::SetChildrenDrawnWithCacheEnabled(
    /* [in] */ Boolean enabled)
{
    mShortcutsAndWidgets->SetChildrenDrawnWithCacheEnabled(enabled);
}

Float CellLayout::GetBackgroundAlpha()
{
    return mBackgroundAlpha;
}

void CellLayout::SetBackgroundAlphaMultiplier(
    /* [in] */ Float multiplier)
{
    if (mBackgroundAlphaMultiplier != multiplier) {
        mBackgroundAlphaMultiplier = multiplier;
        Invalidate();
    }
}

Float CellLayout::GetBackgroundAlphaMultiplier()
{
    return mBackgroundAlphaMultiplier;
}

void CellLayout::SetBackgroundAlpha(
    /* [in] */ Float alpha)
{
    if (mBackgroundAlpha != alpha) {
        mBackgroundAlpha = alpha;
        Invalidate();
    }
}

void CellLayout::SetShortcutAndWidgetAlpha(
    /* [in] */ Float alpha)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->SetAlpha(alpha);
    }
}

AutoPtr<ShortcutAndWidgetContainer> CellLayout::GetShortcutsAndWidgets()
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        return (ShortcutAndWidgetContainer*)child;
    }
    return NULL;
}

AutoPtr<IView> CellLayout::GetChildAt(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return mShortcutsAndWidgets->GetChildAt(x, y);
}

Boolean CellLayout::AnimateChildToPosition(
    /* [in] */ IView* child,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 duration,
    /* [in] */ Int32 delay,
    /* [in] */ Boolean permanent,
    /* [in] */ Boolean adjustOccupied)
{
    AutoPtr<ShortcutAndWidgetContainer> clc = GetShortcutsAndWidgets();
    AutoPtr<ArrayOf<ArrayOf<Boolean>* > > occupied = mOccupied;
    if (!permanent) {
        occupied = mTmpOccupied;
    }

    Int32 index;
    clc->IndexOfChild(child, &index);
    if (index != -1) {
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        AutoPtr<IInterface> tag;
        child->GetTag((IInterface**)&tag);
        ItemInfo* info = (ItemInfo*)IObject::Probe(tag);

        // We cancel any existing animations
        if (mReorderAnimators->ContainsKey(_lp)) {
            AutoPtr<IInterface> value;
            mReorderAnimators->Get(lp, (IInterface**)&value);
            IAnimator::Probe(value)->Cancel();
            mReorderAnimators->Remove(lp);
        }

        Int32 oldX = lp->mX;
        Int32 oldY = lp->mY;
        if (adjustOccupied) {
            (*(*occupied)[lp->mCellX])[lp->mCellY] = FALSE;
            (*(*occupied)[cellX])[cellY] = TRUE;
        }
        lp->mIsLockedToGrid = TRUE;
        if (permanent) {
            lp->mCellX = info->mCellX = cellX;
            lp->mCellY = info->mCellY = cellY;
        }
        else {
            lp->mTmpCellX = cellX;
            lp->mTmpCellY = cellY;
        }
        clc->SetupLp(lp);
        lp->mIsLockedToGrid = FALSE;
        Int32 newX = lp->mX;
        Int32 newY = lp->mY;

        lp->mX = oldX;
        lp->mY = oldY;

        // Exit early if we're not actually moving the view
        if (oldX == newX && oldY == newY) {
            lp->mIsLockedToGrid = TRUE;
            return TRUE;
        }

        AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
        (*params)[0] = 0.0f;
        (*params)[1] = 1.0f;
        AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(child, params);
        va->SetDuration(duration);
        mReorderAnimators->Put(_lp, va);

        AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener2(
            lp, oldX, oldY, newX, newY, child);
        va->AddUpdateListener(listener);
        AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter2(this, lp, child);
        IAnimator::Probe(va)->AddListener(adapter);
        IAnimator::Probe(va)->SetStartDelay(delay);
        IAnimator::Probe(va)->Start();
        return TRUE;
    }
    return FALSE;
}

void CellLayout::EstimateDropCell(
    /* [in] */ Int32 originX,
    /* [in] */ Int32 originY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* result)
{
    Int32 countX = mCountX;
    Int32 countY = mCountY;

    // pointToCellRounded takes the top left of a cell but will pad that with
    // cellWidth/2 and cellHeight/2 when finding the matching cell
    PointToCellRounded(originX, originY, result);

    // If the item isn't fully on this screen, snap to the edges
    Int32 rightOverhang = (*result)[0] + spanX - countX;
    if (rightOverhang > 0) {
        (*result)[0] -= rightOverhang; // Snap to right
    }
    (*result)[0] = Elastos::Core::Math::Max(0, (*result)[0]); // Snap to left
    Int32 bottomOverhang = (*result)[1] + spanY - countY;
    if (bottomOverhang > 0) {
        (*result)[1] -= bottomOverhang; // Snap to bottom
    }
    (*result)[1] = Elastos::Core::Math::Max(0, (*result)[1]); // Snap to top
}

void CellLayout::VisualizeDropLocation(
    /* [in] */ IView* v,
    /* [in] */ IBitmap* dragOutline,
    /* [in] */ Int32 originX,
    /* [in] */ Int32 originY,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Boolean resize,
    /* [in] */ IPoint* dragOffset,
    /* [in] */ IRect* dragRegion)
{
    Int32 oldDragCellX = (*mDragCell)[0];
    Int32 oldDragCellY = (*mDragCell)[1];

    if (v != NULL && dragOffset == NULL) {
        Int32 w, h;
        v->GetWidth(&w);
        v->GetHeight(&h);
        mDragCenter->Set(originX + (w / 2), originY + (h / 2));
    }
    else {
        mDragCenter->Set(originX, originY);
    }

    if (dragOutline == NULL && v == NULL) {
        return;
    }

    Int32 width, height;
    dragOutline->GetWidth(&width);
    dragOutline->GetHeight(&height);
    if (cellX != oldDragCellX || cellY != oldDragCellY) {
        (*mDragCell)[0] = cellX;
        (*mDragCell)[1] = cellY;
        // Find the top left corner of the rect the object will occupy
        AutoPtr<ArrayOf<Int32> > topLeft = mTmpPoint;
        CellToPoint(cellX, cellY, topLeft);

        Int32 left = (*topLeft)[0];
        Int32 top = (*topLeft)[1];

        if (v != NULL && dragOffset == NULL) {
            // When drawing the drag outline, it did not account for margin offsets
            // added by the view's parent.
            AutoPtr<IViewGroupLayoutParams> _lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
            MarginLayoutParams* lp = (MarginLayoutParams*)_lp.Get();
            left += lp->mLeftMargin;
            top += lp->mTopMargin;

            // Offsets due to the size difference between the View and the dragOutline.
            // There is a size difference to account for the outer blur, which may lie
            // outside the bounds of the view.
            Int32 h;
            v->GetHeight(&h);
            top += (h - height) / 2;
            // We center about the x axis
            left += ((mCellWidth * spanX) + ((spanX - 1) * mWidthGap) - width) / 2;
        }
        else {
            if (dragOffset != NULL && dragRegion != NULL) {
                // Center the drag region *horizontally* in the cell and apply a drag
                // outline offset
                Int32 dw, x, y;
                dragRegion->GetWidth(&dw);
                dragOffset->GetX(&x);
                dragOffset->GetY(&y);
                left += x + ((mCellWidth * spanX) + ((spanX - 1) * mWidthGap) - dw) / 2;
                top += y;
            }
            else {
                // Center the drag outline in the cell
                left += ((mCellWidth * spanX) + ((spanX - 1) * mWidthGap)
                        - width) / 2;
                top += ((mCellHeight * spanY) + ((spanY - 1) * mHeightGap)
                        - height) / 2;
            }
        }
        Int32 oldIndex = mDragOutlineCurrent;
        (*mDragOutlineAnims)[oldIndex]->AnimateOut();
        mDragOutlineCurrent = (oldIndex + 1) % mDragOutlines->GetLength();
        AutoPtr<IRect> r = (*mDragOutlines)[mDragOutlineCurrent];
        r->Set(left, top, left + width, top + height);
        if (resize) {
            CellToRect(cellX, cellY, spanX, spanY, r);
        }

        (*mDragOutlineAnims)[mDragOutlineCurrent]->SetTag(dragOutline);
        (*mDragOutlineAnims)[mDragOutlineCurrent]->AnimateIn();
    }
}

void CellLayout::ClearDragOutlines()
{
    Int32 oldIndex = mDragOutlineCurrent;
    (*mDragOutlineAnims)[oldIndex]->AnimateOut();
    (*mDragCell)[0] = (*mDragCell)[1] = -1;
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestVacantArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* result)
{
    return FindNearestVacantArea(pixelX, pixelY, spanX, spanY, NULL, result);
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestVacantArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* result,
    /* [in] */ ArrayOf<Int32>* resultSpan)
{
    return FindNearestVacantArea(pixelX, pixelY, minSpanX, minSpanY, spanX, spanY, NULL,
            result, resultSpan);
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY, View ignoreView,
    /* [in] */ Boolean ignoreOccupied,
    /* [in] */ ArrayOf<Int32>* result)
{
    return FindNearestArea(pixelX, pixelY, spanX, spanY,
            spanX, spanY, ignoreView, ignoreOccupied, result, NULL, mOccupied);
}

void CellLayout::LazyInitTempRectStack()
{
    Boolean isEmpty;
    mTempRectStack->IsEmpty(&isEmpty);
    if (isEmpty) {
        for (Int32 i = 0; i < mCountX * mCountY; i++) {
            AutoPtr<IRect> rect;
            CRect::New((IRect**)&rect);
            mTempRectStack->Push(rect);
        }
    }
}

void CellLayout::RecycleTempRects(
    /* [in] */ IStack* used)
{
    Boolean isEmpty;
    used->IsEmpty(&isEmpty);
    while (!isEmpty) {
        AutoPtr<IInterface> item;
        used->Pop((IInterface**)&item);
        mTempRectStack->Push(item);
    }
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* ignoreView,
    /* [in] */ Boolean ignoreOccupied,
    /* [in] */ ArrayOf<Int32>* result,
    /* [in] */ ArrayOf<Int32>* resultSpan,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    LazyInitTempRectStack();
    // mark space take by ignoreView as available (method checks if ignoreView is NULL)
    MarkCellsAsUnoccupiedForView(ignoreView, occupied);

    // For items with a spanX / spanY > 1, the passed in point (pixelX, pixelY) corresponds
    // to the center of the item, but we are searching based on the top-left cell, so
    // we translate the point over to correspond to the top-left.
    pixelX -= (mCellWidth + mWidthGap) * (spanX - 1) / 2f;
    pixelY -= (mCellHeight + mHeightGap) * (spanY - 1) / 2f;

    // Keep track of best-scoring drop area
    AutoPtr<ArrayOf<Int32> > bestXY = result != NULL ? result : ArrayOf<Int32>::Alloc(2);
    Double bestDistance = Elastos::Core::Math::DOUBLE_MAX_VALUE;
    AutoPtr<IRect> bestRect;
    CRect::New(-1, -1, -1, -1, (IRect**)&bestRect);
    AutoPtr<IStack> validRegions;
    CStack::New((IStack**)&validRegions);

    Int32 countX = mCountX;
    Int32 countY = mCountY;

    if (minSpanX <= 0 || minSpanY <= 0 || spanX <= 0 || spanY <= 0 ||
            spanX < minSpanX || spanY < minSpanY) {
        return bestXY;
    }

    for (Int32 y = 0; y < countY - (minSpanY - 1); y++) {
        Boolean toInner = FALSE;
        for (Int32 x = 0; x < countX - (minSpanX - 1); x++) { // inner
            Int32 ySize = -1;
            Int32 xSize = -1;
            if (ignoreOccupied) {
                toInner = FALSE;
                // First, let's see if this thing fits anywhere
                for (Int32 i = 0; i < minSpanX; i++) {
                    for (Int32 j = 0; j < minSpanY; j++) {
                        if ((*(*occupied)[x + i])[y + j]) {
                            toInner = TRUE;
                            break;
                        }
                    }
                    if (toInner) break;
                }
                if (toInner) continue;

                xSize = minSpanX;
                ySize = minSpanY;

                // We know that the item will fit at _some_ acceptable size, now let's see
                // how big we can make it. We'll alternate between incrementing x and y spans
                // until we hit a limit.
                Boolean incX = TRUE;
                Boolean hitMaxX = xSize >= spanX;
                Boolean hitMaxY = ySize >= spanY;
                while (!(hitMaxX && hitMaxY)) {
                    if (incX && !hitMaxX) {
                        for (Int32 j = 0; j < ySize; j++) {
                            if (x + xSize > countX -1 || (*(*occupied)[x + xSize])[y + j]) {
                                // We can't move out horizontally
                                hitMaxX = TRUE;
                            }
                        }
                        if (!hitMaxX) {
                            xSize++;
                        }
                    }
                    else if (!hitMaxY) {
                        for (Int32 i = 0; i < xSize; i++) {
                            if (y + ySize > countY - 1 || (*(*occupied)[x + i])[y + ySize]) {
                                // We can't move out vertically
                                hitMaxY = TRUE;
                            }
                        }
                        if (!hitMaxY) {
                            ySize++;
                        }
                    }
                    hitMaxX |= xSize >= spanX;
                    hitMaxY |= ySize >= spanY;
                    incX = !incX;
                }
                incX = TRUE;
                hitMaxX = xSize >= spanX;
                hitMaxY = ySize >= spanY;
            }
            AutoPtr<ArrayOf<Int32> > cellXY = mTmpXY;
            CellToCenterPoint(x, y, cellXY);

            // We verify that the current rect is not a sub-rect of any of our previous
            // candidates. In this case, the current rect is disqualified in favour of the
            // containing rect.
            AutoPtr<IInterface> item;
            mTempRectStack->Pop((IInterface**)&item)
            AutoPtr<IRect> currentRect = IRect::Probe(item);
            currentRect->Set(x, y, x + xSize, y + ySize);
            Boolean contained = FALSE;
            Int32 size;
            validRegions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                validRegions->Get(i, (IInterface**)&item);
                IRect* r = IRect::Probe(item);
                Boolean contains;
                r->Contains(currentRect, &contains);
                if (contains) {
                    contained = TRUE;
                    break;
                }
            }
            validRegions->Push(currentRect);
            Double distance = Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow((*cellXY)[0] - pixelX, 2)
                    + Elastos::Core::Math::Pow((*cellXY)[1] - pixelY, 2));

            Boolean res;
            if ((distance <= bestDistance && !contained) ||
                (currentRect->Contains(bestRect, &res), res)) {
                bestDistance = distance;
                (*bestXY)[0] = x;
                (*bestXY)[1] = y;
                if (resultSpan != NULL) {
                    (*resultSpan)[0] = xSize;
                    (*resultSpan)[1] = ySize;
                }
                bestRect->Set(currentRect);
            }
        }
    }
    // re-mark space taken by ignoreView as occupied
    MarkCellsAsOccupiedForView(ignoreView, occupied);

    // Return -1, -1 if no suitable location found
    if (bestDistance == Elastos::Core::Math::DOUBLE_MAX_VALUE) {
        (*bestXY)[0] = -1;
        (*bestXY)[1] = -1;
    }
    RecycleTempRects(validRegions);
    return bestXY;
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestArea(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* blockOccupied,
    /* [in] */ ArrayOf<Int32>* result)
{
    // Keep track of best-scoring drop area
    AutoPtr<ArrayOf<Int32> > bestXY = result != NULL ? result : ArrayOf<Int32>::Alloc(2);
    Float bestDistance = Elastos::Core::Math::FLOAT_MAX_VALUE;
    Int32 bestDirectionScore = Elastos::Core::Math::INT32_MIN_VALUE;

    Int32 countX = mCountX;
    Int32 countY = mCountY;

    for (Int32 y = 0; y < countY - (spanY - 1); y++) {
        Boolean toInner = FALSE:
        for (Int32 x = 0; x < countX - (spanX - 1); x++) {// inner
            toInner = FALSE:
            // First, let's see if this thing fits anywhere
            for (Int32 i = 0; i < spanX; i++) {
                for (Int32 j = 0; j < spanY; j++) {
                    if ((*(*occupied)[x + i])[y + j] && (blockOccupied == NULL || (*(*blockOccupied)[i])[j])) {
                        toInner = TRUE;
                        break;
                    }
                }
                if (toInner) break;
            }
            if (toInner) continue;

            Float distance = (Float)
                    Elastos::Core::Math::Sqrt((x - cellX) * (x - cellX) + (y - cellY) * (y - cellY));
            AutoPtr<ArrayOf<Int32> > curDirection = mTmpPoint;
            ComputeDirectionVector(x - cellX, y - cellY, curDirection);
            // The direction score is just the dot product of the two candidate direction
            // and that passed in.
            Int32 curDirectionScore = (*direction)[0] * (*curDirection)[0] +
                    (*direction)[1] * (*curDirection)[1];
            Boolean exactDirectionOnly = FALSE;
            Boolean directionMatches = (*direction)[0] == (*curDirection)[0] &&
                    (*direction)[0] == (*curDirection)[0];
            if ((directionMatches || !exactDirectionOnly) &&
                Elastos::Core::Math::Compare(distance,  bestDistance) < 0
                || (Elastos::Core::Math::Compare(distance, bestDistance) == 0
                && curDirectionScore > bestDirectionScore)) {
                bestDistance = distance;
                bestDirectionScore = curDirectionScore;
                (*bestXY)[0] = x;
                (*bestXY)[1] = y;
            }
        }
    }

    // Return -1, -1 if no suitable location found
    if (bestDistance == Elastos::Core::Math::FLOAT_MAX_VALUE) {
        (*bestXY)[0] = -1;
        (*bestXY)[1] = -1;
    }
    return bestXY;
}

BooleanCellLayout:: AddViewToTempLocation(
    /* [in] */ IView* v,
    /* [in] */ IRect* rectOccupiedByPotentialDrop,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ ItemConfiguration* currentState)
{
    AutoPtr<IInterface> value;
    currentState->mMap->Get(v, (IInterface**)&value);
    CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
    Boolean success = FALSE;
    MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, FALSE);
    MarkCellsForRect(rectOccupiedByPotentialDrop, mTmpOccupied, TRUE);

    FindNearestArea(c->mX, c->mY, c->mSpanX, c->mSpanY, direction, mTmpOccupied, NULL, mTempLocation);

    if ((*mTempLocation)[0] >= 0 && (*mTempLocation)[1] >= 0) {
        c->mX = (*mTempLocation)[0];
        c->mY = (*mTempLocation)[1];
        success = TRUE;
    }
    MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, TRUE);
    return success;
}

Boolean CellLayout::PushViewsToTempLocation(
    /* [in] */ IArrayList* views,
    /* [in] */ IRect* rectOccupiedByPotentialDrop,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ IView* dragView,
    /* [in] */ ItemConfiguration* currentState)
{
    AutoPtr<ViewCluster> cluster = new ViewCluster(views, currentState, this);
    AutoPtr<IRect> clusterRect = cluster->GetBoundingRect();
    Int32 whichEdge;
    Int32 pushDistance;
    Boolean fail = FALSE;

    // Determine the edge of the cluster that will be leading the push and how far
    // the cluster must be shifted.
    if ((*direction)[0] < 0) {
        whichEdge = ViewCluster::LEFT;
        Int32 right, left;
        clusterRect->GetRight(&right);
        rectOccupiedByPotentialDrop->GetLeft(&left);
        pushDistance = right - left;
    }
    else if ((*direction)[0] > 0) {
        whichEdge = ViewCluster::RIGHT;
        Int32 right, left;
        rectOccupiedByPotentialDrop->GetRight(&right);
        clusterRect->GetLeft(&left);
        pushDistance = right - left;
    }
    else if ((*direction)[1] < 0) {
        whichEdge = ViewCluster::TOP;
        Int32 top, bottom;
        rectOccupiedByPotentialDrop->GetTop(&top);
        clusterRect->GetBottom(&bottom);
        pushDistance = bottom - top;
    }
    else {
        whichEdge = ViewCluster::BOTTOM;
        Int32 top, bottom;
        clusterRect->GetTop(&top);
        rectOccupiedByPotentialDrop->GetBottom(&bottom);
        pushDistance = bottom - top;
    }

    // Break early for invalid push distance.
    if (pushDistance <= 0) {
        return FALSE;
    }

    // Mark the occupied state as FALSE for the group of views we want to move.
    Int32 size;
    views->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        views->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, FALSE);
    }

    // We save the current configuration -- if we fail to find a solution we will revert
    // to the initial state. The process of finding a solution modifies the configuration
    // in place, hence the need for revert in the failure case.
    currentState->Save();

    // The pushing algorithm is simplified by considering the views in the order in which
    // they would be pushed by the cluster. For example, if the cluster is leading with its
    // left edge, we consider sort the views by their right edge, from right to left.
    cluster->SortConfigurationForEdgePush(whichEdge);

    while (pushDistance > 0 && !fail) {
        currentState->mSortedViews->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            currentState->mSortedViews->Get(i, (IInterface**)&item);
            IView* v = IView::Probe(item);
            // For each view that isn't in the cluster, we see if the leading edge of the
            // cluster is contacting the edge of that view. If so, we add that view to the
            // cluster.
            Boolean contains;
            cluster->mViews->Contains(v, &contains);
            if (!contains && v != dragView) {
                if (cluster->IsViewTouchingEdge(v, whichEdge)) {
                    AutoPtr<IViewGroupLayoutParams> _lp;
                    v->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
                    LayoutParams* lp = (LayoutParams*)_lp.Get();
                    if (!lp->mCanReorder) {
                        // The push solution includes the all apps button, this is not viable.
                        fail = TRUE;
                        break;
                    }
                    cluster->AddView(v);
                    AutoPtr<IInterface> value;
                    currentState->mMap->Get(v, (IInterface**)&value);
                    CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);

                    // Adding view to cluster, mark it as not occupied.
                    MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, FALSE);
                }
            }
        }
        pushDistance--;

        // The cluster has been completed, now we move the whole thing over in the appropriate
        // direction.
        cluster->Shift(whichEdge, 1);
    }

    Boolean foundSolution = FALSE;
    clusterRect = cluster->GetBoundingRect();

    // Due to the nature of the algorithm, the only check required to verify a valid solution
    // is to ensure that completed shifted cluster lies completely within the cell layout.
    Int32 l, r, t, b;
    if (!fail && (clusterRect->GetLeft(&l), l >= 0) &&
        (clusterRect->GetRight(&r), r <= mCountX) && (clusterRect->GetTop(&t), t >= 0) &&
        (clusterRect->GetBottom(&b), b <= mCountY)) {
        foundSolution = TRUE;
    }
    else {
        currentState->Restore();
    }

    // In either case, we set the occupied array as marked for the location of the views
    cluster->mViews->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        cluster->mViews->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, TRUE);
    }

    return foundSolution;
}

Boolean CellLayout::AddViewsToTempLocation(
    /* [in] */ IArrayList* views,
    /* [in] */ IRect* rectOccupiedByPotentialDrop,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ IView* dragView,
    /* [in] */ ItemConfiguration* currentState)
{
    Int32 size;
    views->GetSize(&size);
    if (size == 0) return TRUE;

    Boolean success = FALSE;
    AutoPtr<IRect> boundingRect;
    // We construct a rect which represents the entire group of views passed in
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        views->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        if (boundingRect == NULL) {
            CRect::New(c->mX, c->mY, c->mX + c->mSpanX, c->mY + c->mSpanY, (IRect**)&boundingRect);
        }
        else {
            boundingRect->Union(c->mX, c->mY, c->mX + c->mSpanX, c->mY + c->mSpanY);
        }
    }

    // Mark the occupied state as FALSE for the group of views we want to move.
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        views->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, FALSE);
    }

    Int32 w, h;
    boundingRect->GetWidth(&w);
    boundingRect->GetHeight(&h);
    AutoPtr<ArrayOf<ArrayOf<Boolean>* > > blockOccupied = ArrayOf<ArrayOf<Boolean>* >::Alloc(w);
    for (Int32 i = 0; i < w; i++) {
        AutoPtr<ArrayOf<Boolean> > item = ArrayOf<Boolean>::Alloc(h);
        blockOccupied->Set(i, item);
    }
    Int32 top, left;
    boundingRect->GetTop(&top);
    boundingRect->GetLeft(&left);
    // We mark more precisely which parts of the bounding rect are truly occupied, allowing
    // for interlocking.
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        views->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        MarkCellsForView(c->mX - left, c->mY - top, c->mSpanX, c->mSpanY, blockOccupied, TRUE);
    }

    MarkCellsForRect(rectOccupiedByPotentialDrop, mTmpOccupied, TRUE);

    FindNearestArea(left, top, w, h, direction, mTmpOccupied, blockOccupied, mTempLocation);

    // If we successfuly found a location by pushing the block of views, we commit it
    if ((*mTempLocation)[0] >= 0 && (*mTempLocation)[1] >= 0) {
        Int32 deltaX = (*mTempLocation)[0] - left;
        Int32 deltaY = (*mTempLocation)[1] - top;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> key, value;
            views->Get(i, (IInterface**)&key);
            currentState->mMap->Get(key, (IInterface**)&value);
            CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
            c->mX += deltaX;
            c->mY += deltaY;
        }
        success = TRUE;
    }

    // In either case, we set the occupied array as marked for the location of the views
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key, value;
        views->Get(i, (IInterface**)&key);
        currentState->mMap->Get(key, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, TRUE);
    }
    return success;
}

void CellLayout::MarkCellsForRect(
    /* [in] */ IRect* r,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
    /* [in] */ Boolean value)
{
    Int32 left, top, w, h;
    r->GetLeft(&left);
    r->GetTop(&top);
    r->GetWidth(&w);
    r->GetHeight(&h);
    MarkCellsForView(left, top, w, h, occupied, value);
}

Boolean CellLayout::AttemptPushInDirection(
    /* [in] */ IArrayList* intersectingViews,
    /* [in] */ IRect* occupied,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ IView* ignoreView,
    /* [in] */ ItemConfiguration* solution)
{
    if ((Elastos::Core::Math::Abs((*direction)[0]) + Elastos::Core::Math::Abs((*direction)[1])) > 1) {
        // If the direction vector has two non-zero components, we try pushing
        // separately in each of the components.
        Int32 temp = (*direction)[1];
        (*direction)[1] = 0;

        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        (*direction)[1] = temp;
        temp = (*direction)[0];
        (*direction)[0] = 0;

        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        // Revert the direction
        (*direction)[0] = temp;

        // Now we try pushing in each component of the opposite direction
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;
        temp = (*direction)[1];
        (*direction)[1] = 0;
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }

        (*direction)[1] = temp;
        temp = (*direction)[0];
        (*direction)[0] = 0;
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        // revert the direction
        (*direction)[0] = temp;
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;

    }
    else {
        // If the direction vector has a single non-zero component, we push first in the
        // direction of the vector
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        // Then we try the opposite direction
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        // Switch the direction back
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;

        // If we have failed to find a push solution with the above, then we try
        // to find a solution by pushing along the perpendicular axis.

        // Swap the components
        Int32 temp = (*direction)[1];
        (*direction)[1] = (*direction)[0];
        (*direction)[0] = temp;
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }

        // Then we try the opposite direction
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;
        if (PushViewsToTempLocation(intersectingViews, occupied, direction,
                ignoreView, solution)) {
            return TRUE;
        }
        // Switch the direction back
        (*direction)[0] *= -1;
        (*direction)[1] *= -1;

        // Swap the components back
        temp = (*direction)[1];
        (*direction)[1] = (*direction)[0];
        (*direction)[0] = temp;
    }
    return FALSE;
}

Boolean CellLayout::RearrangementExists(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ IView* ignoreView,
    /* [in] */ ItemConfiguration* solution)
{
    // Return early if get invalid cell positions
    if (cellX < 0 || cellY < 0) return FALSE;

    mIntersectingViews->Clear();
    mOccupiedRect->Set(cellX, cellY, cellX + spanX, cellY + spanY);

    // Mark the desired location of the view currently being dragged.
    if (ignoreView != NULL) {
        AutoPtr<IInterface> value;
        solution->mMap->Get(ignoreView, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        if (c != NULL) {
            c->mX = cellX;
            c->mY = cellY;
        }
    }
    AutoPtr<IRect> r0, r1;
    CRect::New(cellX, cellY, cellX + spanX, cellY + spanY, (IRect**)&r0);
    CRect::New((IRect**)&r1);
    AutoPtr<ISet> keySet;
    solution->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<IRectHelper> rHelper;
    CRectHelper::AcquireSingleton((IRectHelper**)&rHelper);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IView> child = IView::Probe((*array)[i]);
        if (child.Get() == ignoreView) continue;
        AutoPtr<IInterface> value;
        solution->mMap->Get(child, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        r1->Set(c->mX, c->mY, c->mX + c->mSpanX, c->mY + c->mSpanY);
        Boolean res;
        rHelper->Intersects(r0, r1, &res);
        if (res) {
            if (!lp->mCanReorder) {
                return FALSE;
            }
            mIntersectingViews->Add(child);
        }
    }

    // First we try to find a solution which respects the push mechanic->m That is,
    // we try to find a solution such that no displaced item travels through another item
    // without also displacing that item.
    if (AttemptPushInDirection(mIntersectingViews, mOccupiedRect, direction, ignoreView,
            solution)) {
        return TRUE;
    }

    // Next we try moving the views as a block, but without requiring the push mechanic->m
    if (AddViewsToTempLocation(mIntersectingViews, mOccupiedRect, direction, ignoreView,
            solution)) {
        return TRUE;
    }

    // Ok, they couldn't move as a block, let's move them individually
    Int32 size;
    mIntersectingViews->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> v;
        mIntersectingViews->Get(i, (IInterface**)&v);
        if (!AddViewToTempLocation(IView::Probe(v), mOccupiedRect, direction, solution)) {
            return FALSE;
        }
    }
    return TRUE;
}

void CellLayout::ComputeDirectionVector(
    /* [in] */ Float deltaX,
    /* [in] */ Float deltaY,
    /* [in] */ ArrayOf<Int32>* result)
{
    Double angle = Elastos::Core::Math::Atan(((Float) deltaY) / deltaX);

    (*result)[0] = 0;
    (*result)[1] = 0;
    if (Elastos::Core::Math::Abs(Elastos::Core::Math::Cos(angle)) > 0.5f) {
        (*result)[0] = (Int32) Elastos::Core::Math::Signum(deltaX);
    }
    if (Elastos::Core::Math::Abs(Elastos::Core::Math::Sin(angle)) > 0.5f) {
        (*result)[1] = (Int32) Elastos::Core::Math::Signum(deltaY);
    }
}

void CellLayout::CopyOccupiedArray(
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    for (Int32 i = 0; i < mCountX; i++) {
        for (Int32 j = 0; j < mCountY; j++) {
            (*(*occupied)[i])[j] = (*(*mOccupied)[i])[j];
        }
    }
}

AutoPtr<CellLayout::ItemConfiguration> CellLayout::SimpleSwap(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ IView* dragView,
    /* [in] */ Boolean decX,
    /* [in] */ ItemConfiguration* solution)
{
    // Copy the current state into the solution. This solution will be manipulated as necessary.
    CopyCurrentStateToSolution(solution, FALSE);
    // Copy the current occupied array into the temporary occupied array. This array will be
    // manipulated as necessary to find a solution.
    CopyOccupiedArray(mTmpOccupied);

    // We find the nearest cell into which we would place the dragged item, assuming there's
    // nothing in its way.
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);
    result = FindNearestArea(pixelX, pixelY, spanX, spanY, result);

    Boolean success = FALSE;
    // First we try the exact nearest position of the item being dragged,
    // we will then want to try to move this around to other neighbouring positions
    success = RearrangementExists((*result)[0], (*result)[1], spanX, spanY, direction, dragView,
            solution);

    if (!success) {
        // We try shrinking the widget down to size in an alternating pattern, shrink 1 in
        // x, then 1 in y etc->m
        if (spanX > minSpanX && (minSpanY == spanY || decX)) {
            return SimpleSwap(pixelX, pixelY, minSpanX, minSpanY, spanX - 1, spanY, direction,
                    dragView, FALSE, solution);
        }
        else if (spanY > minSpanY) {
            return SimpleSwap(pixelX, pixelY, minSpanX, minSpanY, spanX, spanY - 1, direction,
                    dragView, TRUE, solution);
        }
        solution->mIsSolution = FALSE;
    }
    else {
        solution->mIsSolution = TRUE;
        solution->mDragViewX = (*result)[0];
        solution->mDragViewY = (*result)[1];
        solution->mDragViewSpanX = spanX;
        solution->mDragViewSpanY = spanY;
    }
    return solution;
}

void CellLayout::CopyCurrentStateToSolution(
    /* [in] */ ItemConfiguration* solution,
    /* [in] */ Boolean temp)
{
    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        AutoPtr<CellAndSpan> c;
        if (temp) {
            c = new CellAndSpan(lp->mTmpCellX, lp->mTmpCellY, lp->mCellHSpan, lp->mCellVSpan);
        }
        else {
            c = new CellAndSpan(lp->mCellX, lp->mCellY, lp->mCellHSpan, lp->mCellVSpan);
        }
        solution->Add(child, c);
    }
}

void CellLayout::CopySolutionToTempState(
    /* [in] */ ItemConfiguration* solution,
    /* [in] */ IView* dragView)
{
    for (Int32 i = 0; i < mCountX; i++) {
        for (Int32 j = 0; j < mCountY; j++) {
            (*(*mTmpOccupied)[i])[j] = FALSE;
        }
    }

    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        if (child.Get() == dragView) continue;
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        AutoPtr<IInterface> value;
        solution->mMap->Get(child, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        if (c != NULL) {
            lp->mTmpCellX = c->mX;
            lp->mTmpCellY = c->mY;
            lp->mCellHSpan = c->mSpanX;
            lp->mCellVSpan = c->mSpanY;
            MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, mTmpOccupied, TRUE);
        }
    }
    MarkCellsForView(solution->mDragViewX, solution->mDragViewY, solution->mDragViewSpanX,
            solution->mDragViewSpanY, mTmpOccupied, TRUE);
}

void CellLayout::AnimateItemsToSolution(
    /* [in] */ ItemConfiguration* solution,
    /* [in] */ IView* dragView,
    /* [in] */ Boolean commitDragView)
{
    AutoPtr<ArrayOf<ArrayOf<Boolean>* > > occupied = DESTRUCTIVE_REORDER ? mOccupied : mTmpOccupied;
    for (Int32 i = 0; i < mCountX; i++) {
        for (Int32 j = 0; j < mCountY; j++) {
            (*(*occupied)[i])[j] = FALSE;
        }
    }

    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        if (child.Get() == dragView) continue;
        AutoPtr<IInterface> value;
        solution->mMap->Get(child, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        if (c != NULL) {
            AnimateChildToPosition(child, c->mX, c->mY, REORDER_ANIMATION_DURATION, 0,
                    DESTRUCTIVE_REORDER, FALSE);
            MarkCellsForView(c->mX, c->mY, c->mSpanX, c->mSpanY, occupied, TRUE);
        }
    }
    if (commitDragView) {
        MarkCellsForView(solution->mDragViewX, solution->mDragViewY, solution->mDragViewSpanX,
                solution->mDragViewSpanY, occupied, TRUE);
    }
}

// This method starts or changes the reorder hint animations
void CellLayout::BeginOrAdjustHintAnimations(
    /* [in] */ ItemConfiguration* solution,
    /* [in] */ IView* dragView,
    /* [in] */ Int32 delay)
{
    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        if (child.Get() == dragView) continue;
        AutoPtr<IInterface> value;
        solution->mMap->Get(child, (IInterface**)&value);
        CellAndSpan* c = (CellAndSpan*)IObject::Probe(value);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        if (c != NULL) {
            AutoPtr<ReorderHintAnimation> rha = new ReorderHintAnimation(child, lp->mCellX, lp->mCellY,
                    c->mX, c->mY, c->mSpanX, c->mSpanY, this);
            rha->Animate();
        }
    }
}

void CellLayout::CompleteAndClearReorderHintAnimations()
{
    AutoPtr<ICollection> values;
    mShakeAnimators->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        ((ReorderHintAnimation*)IObject::Probe((*array)[i]))->CompleteAnimationImmediately();
    }
    mShakeAnimators->Clear();
}

void CellLayout::CommitTempPlacement()
{
    for (Int32 i = 0; i < mCountX; i++) {
        for (Int32 j = 0; j < mCountY; j++) {
            (*(*mOccupied)[i])[j] = (*(*mTmpOccupied)[i])[j];
        }
    }
    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        AutoPtr<IInterface> tag;
        child->GetTag((IInterface**)&tag);
        ItemInfo* info = (ItemInfo*)IObject::Probe(tag);
        // We do a NULL check here because the item info can be NULL in the case of the
        // AllApps button in the hotseat.
        if (info != NULL) {
            if (info->mCellX != lp->mTmpCellX || info->mCellY != lp->mTmpCellY ||
                    info->mSpanX != lp->mCellHSpan || info->mSpanY != lp->mCellVSpan) {
                info->mRequiresDbUpdate = TRUE;
            }
            info->mCellX = lp->mCellX = lp->mTmpCellX;
            info->mCellY = lp->mCellY = lp->mTmpCellY;
            info->mSpanX = lp->mCellHSpan;
            info->mSpanY = lp->mCellVSpan;
        }
    }
    mLauncher->GetWorkspace()->UpdateItemLocationsInDatabase(this);
}

void CellLayout::SetUseTempCoords(
    /* [in] */ Boolean useTempCoords)
{
    Int32 childCount;
    mShortcutsAndWidgets->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        lp->mUseTmpCoords = useTempCoords;
    }
}

AutoPtr<CellLayout::ItemConfiguration> CellLayout::FindConfigurationNoShuffle(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ ItemConfiguration* solution)
{
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int32> > resultSpan = ArrayOf<Int32>::Alloc(2);
    FindNearestVacantArea(pixelX, pixelY, minSpanX, minSpanY, spanX, spanY, NULL, result,
            resultSpan);
    if ((*result)[0] >= 0 && (*result)[1] >= 0) {
        CopyCurrentStateToSolution(solution, FALSE);
        solution->mDragViewX = (*result)[0];
        solution->mDragViewY = (*result)[1];
        solution->mDragViewSpanX = (*resultSpan)[0];
        solution->mDragViewSpanY = (*resultSpan)[1];
        solution->mIsSolution = TRUE;
    }
    else {
        solution->mIsSolution = FALSE;
    }
    return solution;
}

void CellLayout::PrepareChildForDrag(
    /* [in] */ IView* child)
{
    MarkCellsAsUnoccupiedForView(child);
}

void CellLayout::GetDirectionVectorForDrop(
    /* [in] */ Int32 dragViewCenterX,
    /* [in] */ Int32 dragViewCenterY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ ArrayOf<Int32>* resultDirection)
{
    AutoPtr<ArrayOf<Int32> > targetDestination = ArrayOf<Int32>::Alloc(2);

    FindNearestArea(dragViewCenterX, dragViewCenterY, spanX, spanY, targetDestination);
    AutoPtr<IRect> dragRect;
    CRect::New((IRect**)&dragRect);
    RegionToRect((*targetDestination)[0], (*targetDestination)[1], spanX, spanY, dragRect);
    Int32 cx, cy;
    dragRect->GetCenterX(&cx);
    dragRect->GetCenterY(&cy);
    dragRect->Offset(dragViewCenterX - cx, dragViewCenterY - cy);

    AutoPtr<IRect> dropRegionRect;
    CRect::New((IRect**)&dropRegionRect);
    GetViewsIntersectingRegion((*targetDestination)[0], (*targetDestination)[1], spanX, spanY,
            dragView, dropRegionRect, mIntersectingViews);

    Int32 dropRegionSpanX, dropRegionSpanY;
    dropRegionRect->GetWidth(&dropRegionSpanX);
    dropRegionRect->GetHeight(&dropRegionSpanY);
    Int32 left, top;
    dropRegionRect->GetLeft(&left);
    dropRegionRect->GetTop(&top);

    RegionToRect(left, top, dropRegionSpanX, dropRegionSpanY, dropRegionRect);

    dropRegionRect->GetCenterX(&cx);
    dropRegionRect->GetCenterY(&cy);
    Int32 deltaX = (cx - dragViewCenterX) / spanX;
    Int32 deltaY = (cy - dragViewCenterY) / spanY;

    if (dropRegionSpanX == mCountX || spanX == mCountX) {
        deltaX = 0;
    }
    if (dropRegionSpanY == mCountY || spanY == mCountY) {
        deltaY = 0;
    }

    if (deltaX == 0 && deltaY == 0) {
        // No idea what to do, give a random direction.
        (*resultDirection)[0] = 1;
        (*resultDirection)[1] = 0;
    }
    else {
        ComputeDirectionVector(deltaX, deltaY, resultDirection);
    }
}

// For a given cell and span, fetch the set of views intersecting the region.
void CellLayout::GetViewsIntersectingRegion(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ IRect* boundingRect,
    /* [in] */ IArrayList* intersectingViews)
{
    if (boundingRect != NULL) {
        boundingRect->Set(cellX, cellY, cellX + spanX, cellY + spanY);
    }
    intersectingViews->Clear();
    AutoPtr<IRect> r0, r1;
    CRect::New(cellX, cellY, cellX + spanX, cellY + spanY, (IRect**)&r0);
    CRect::New((IRect**)&r1);
    Int32 count;
    mShortcutsAndWidgets->GetChildCount(&count);
    AutoPtr<IRectHelper> rHelper;
    CRectHelper::AcquireSingleton((IRectHelper**)&rHelper);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        if (child.Get() == dragView) continue;
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        r1->Set(lp->mCellX, lp->mCellY, lp->mCellX + lp->mCellHSpan, lp->mCellY + lp->mCellVSpan);
        Boolean res;
        rHelper->Intersects(r0, r1, &res);
        if (res) {
            mIntersectingViews->Add(child);
            if (boundingRect != NULL) {
                boundingRect->Union(r1);
            }
        }
    }
}

Boolean CellLayout::IsNearestDropLocationOccupied(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ ArrayOf<Int32>* _result)
{
    AutoPtr<ArrayOf<Int32> > result = FindNearestArea(pixelX, pixelY, spanX, spanY, _result);
    GetViewsIntersectingRegion((*result)[0], (*result)[1], spanX, spanY, dragView, NULL,
            mIntersectingViews);
    return !mIntersectingViews->IsEmpty();
}

void CellLayout::RevertTempState()
{
    if (!IsItemPlacementDirty() || DESTRUCTIVE_REORDER) return;
    Int32 count;
    mShortcutsAndWidgets->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mShortcutsAndWidgets->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        if (lp->mTmpCellX != lp->mCellX || lp->mTmpCellY != lp->mCellY) {
            lp->mTmpCellX = lp->mCellX;
            lp->mTmpCellY = lp->mCellY;
            AnimateChildToPosition(child, lp->mCellX, lp->mCellY, REORDER_ANIMATION_DURATION,
                    0, FALSE, FALSE);
        }
    }
    CompleteAndClearReorderHintAnimations();
    SetItemPlacementDirty(FALSE);
}

Boolean CellLayout::createAreaForResize(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ ArrayOf<Int32>* direction,
    /* [in] */ Boolean commit)
{
    AutoPtr<ArrayOf<Int32> > pixelXY = ArrayOf<Int32>::Alloc(2);
    RegionToCenterPoint(cellX, cellY, spanX, spanY, pixelXY);

    // First we determine if things have moved enough to cause a different layout
    AutoPtr<ItemConfiguration> ic = new ItemConfiguration();
    AutoPtr<ItemConfiguration> swapSolution = SimpleSwap((*pixelXY)[0], (*pixelXY)[1], spanX, spanY,
             spanX,  spanY, direction, dragView, TRUE, ic);

    SetUseTempCoords(TRUE);
    if (swapSolution != NULL && swapSolution->mIsSolution) {
        // If we're just testing for a possible location (MODE_ACCEPT_DROP), we don't bother
        // committing anything or animating anything as we just want to determine if a solution
        // exists
        CopySolutionToTempState(swapSolution, dragView);
        SetItemPlacementDirty(TRUE);
        AnimateItemsToSolution(swapSolution, dragView, commit);

        if (commit) {
            CommitTempPlacement();
            CompleteAndClearReorderHintAnimations();
            SetItemPlacementDirty(FALSE);
        }
        else {
            BeginOrAdjustHintAnimations(swapSolution, dragView,
                    REORDER_ANIMATION_DURATION);
        }
        mShortcutsAndWidgets->RequestLayout();
    }
    return swapSolution->mIsSolution;
}

AutoPtr<ArrayOf<Int32> > CellLayout::CreateArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* dragView,
    /* [in] */ ArrayOf<Int32>* _result,
    /* [in] */ ArrayOf<Int32>* resultSpan,
    /* [in] */ Int32 mode)
{
    // First we determine if things have moved enough to cause a different layout
    AutoPtr<ArrayOf<Int32> > result = FindNearestArea(pixelX, pixelY, spanX, spanY, _result);

    if (resultSpan == NULL) {
        resultSpan = ArrayOf<Int32>::Alloc(2);
    }

    // When we are checking drop validity or actually dropping, we don't recompute the
    // direction vector, since we want the solution to match the preview, and it's possible
    // that the exact position of the item has changed to result in a new reordering outcome.
    if ((mode == MODE_ON_DROP || mode == MODE_ON_DROP_EXTERNAL || mode == MODE_ACCEPT_DROP)
           && (*mPreviousReorderDirection)[0] != INVALID_DIRECTION) {
        (*mDirectionVector)[0] = (*mPreviousReorderDirection)[0];
        (*mDirectionVector)[1] = (*mPreviousReorderDirection)[1];
        // We reset this vector after drop
        if (mode == MODE_ON_DROP || mode == MODE_ON_DROP_EXTERNAL) {
            (*mPreviousReorderDirection)[0] = INVALID_DIRECTION;
            (*mPreviousReorderDirection)[1] = INVALID_DIRECTION;
        }
    }
    else {
        GetDirectionVectorForDrop(pixelX, pixelY, spanX, spanY, dragView, mDirectionVector);
        (*mPreviousReorderDirection)[0] = (*mDirectionVector)[0];
        (*mPreviousReorderDirection)[1] = (*mDirectionVector)[1];
    }
    AutoPtr<ItemConfiguration> ic = new ItemConfiguration();
    AutoPtr<ItemConfiguration> swapSolution = SimpleSwap(pixelX, pixelY, minSpanX, minSpanY,
             spanX,  spanY, mDirectionVector, dragView,  TRUE,  ic);

    ic = new ItemConfiguration();
    // We attempt the approach which doesn't shuffle views at all
    AutoPtr<ItemConfiguration> noShuffleSolution = FindConfigurationNoShuffle(pixelX, pixelY, minSpanX,
            minSpanY, spanX, spanY, dragView, ic);

    AutoPtr<ItemConfiguration> finalSolution;
    if (swapSolution->mIsSolution && swapSolution->Area() >= noShuffleSolution->Area()) {
        finalSolution = swapSolution;
    }
    else if (noShuffleSolution->mIsSolution) {
        finalSolution = noShuffleSolution;
    }

    Boolean foundSolution = TRUE;
    if (!DESTRUCTIVE_REORDER) {
        SetUseTempCoords(TRUE);
    }

    if (finalSolution != NULL) {
        (*result)[0] = finalSolution->mDragViewX;
        (*result)[1] = finalSolution->mDragViewY;
        (*resultSpan)[0] = finalSolution->mDragViewSpanX;
        (*resultSpan)[1] = finalSolution->mDragViewSpanY;

        // If we're just testing for a possible location (MODE_ACCEPT_DROP), we don't bother
        // committing anything or animating anything as we just want to determine if a solution
        // exists
        if (mode == MODE_DRAG_OVER || mode == MODE_ON_DROP || mode == MODE_ON_DROP_EXTERNAL) {
            if (!DESTRUCTIVE_REORDER) {
                CopySolutionToTempState(finalSolution, dragView);
            }
            SetItemPlacementDirty(TRUE);
            AnimateItemsToSolution(finalSolution, dragView, mode == MODE_ON_DROP);

            if (!DESTRUCTIVE_REORDER &&
                    (mode == MODE_ON_DROP || mode == MODE_ON_DROP_EXTERNAL)) {
                CommitTempPlacement();
                CompleteAndClearReorderHintAnimations();
                SetItemPlacementDirty(FALSE);
            }
            else {
                BeginOrAdjustHintAnimations(finalSolution, dragView,
                        REORDER_ANIMATION_DURATION);
            }
        }
    }
    else {
        foundSolution = FALSE;
        (*result)[0] = (*result)[1] = (*resultSpan)[0] = (*resultSpan)[1] = -1;
    }

    if ((mode == MODE_ON_DROP || !foundSolution) && !DESTRUCTIVE_REORDER) {
        SetUseTempCoords(FALSE);
    }

    mShortcutsAndWidgets->RequestLayout();
    return result;
}

void CellLayout::SetItemPlacementDirty(
    /* [in] */ Boolean dirty)
{
    mItemPlacementDirty = dirty;
}

Boolean CellLayout::IsItemPlacementDirty()
{
    return mItemPlacementDirty;
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestVacantArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* ignoreView,
    /* [in] */ ArrayOf<Int32>* result)
{
    return FindNearestArea(pixelX, pixelY, spanX, spanY, ignoreView, TRUE, result);
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestVacantArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY, View ignoreView,
    /* [in] */ ArrayOf<Int32>* result,
    /* [in] */ ArrayOf<Int32>* resultSpan)
{
    return FindNearestArea(pixelX, pixelY, minSpanX, minSpanY, spanX, spanY, ignoreView, TRUE,
            result, resultSpan, mOccupied);
}

AutoPtr<ArrayOf<Int32> > CellLayout::FindNearestArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<Int32>* result)
{
    return FindNearestArea(pixelX, pixelY, spanX, spanY, NULL, FALSE, result);
}

Boolean CellLayout::ExistsEmptyCell()
{
    return FindCellForSpan(NULL, 1, 1);
}

Boolean CellLayout::FindCellForSpan(
    /* [in] */ ArrayOf<Int32>* cellXY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
{
    return FindCellForSpanThatIntersectsIgnoring(cellXY, spanX, spanY, -1, -1, NULL, mOccupied);
}

Boolean CellLayout::FindCellForSpanIgnoring(
    /* [in] */ ArrayOf<Int32>* cellXY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IView* ignoreView)
{
    return FindCellForSpanThatIntersectsIgnoring(cellXY, spanX, spanY, -1, -1,
            ignoreView, mOccupied);
}

Boolean CellLayout::FindCellForSpanThatIntersects(
    /* [in] */ ArrayOf<Int32>* cellXY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Int32 intersectX,
    /* [in] */ Int32 intersectY)
{
    return FindCellForSpanThatIntersectsIgnoring(
            cellXY, spanX, spanY, intersectX, intersectY, NULL, mOccupied);
}

Boolean CellLayout::FindCellForSpanThatIntersectsIgnoring(
    /* [in] */ ArrayOf<Int32>* cellXY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Int32 intersectX,
    /* [in] */ Int32 intersectY,
    /* [in] */ IView* ignoreView,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    // mark space take by ignoreView as available (method checks if ignoreView is NULL)
    MarkCellsAsUnoccupiedForView(ignoreView, occupied);

    Boolean foundCell = FALSE;
    while (TRUE) {
        Int32 startX = 0;
        if (intersectX >= 0) {
            startX = Elastos::Core::Math::Max(startX, intersectX - (spanX - 1));
        }
        Int32 endX = mCountX - (spanX - 1);
        if (intersectX >= 0) {
            endX = Elastos::Core::Math::Min(endX, intersectX + (spanX - 1) + (spanX == 1 ? 1 : 0));
        }
        Int32 startY = 0;
        if (intersectY >= 0) {
            startY = Elastos::Core::Math::Max(startY, intersectY - (spanY - 1));
        }
        Int32 endY = mCountY - (spanY - 1);
        if (intersectY >= 0) {
            endY = Elastos::Core::Math::Min(endY, intersectY + (spanY - 1) + (spanY == 1 ? 1 : 0));
        }

        for (Int32 y = startY; y < endY && !foundCell; y++) {
            Boolean toInner = FALSE;
            for (Int32 x = startX; x < endX; x++) { // inner
                toInner = FALSE;
                for (Int32 i = 0; i < spanX; i++) {
                    for (Int32 j = 0; j < spanY; j++) {
                        if (occupied[x + i][y + j]) {
                            // small optimization: we can skip to after the column we just found
                            // an occupied cell
                            x += i;
                            toInner = TRUE;
                            break;
                        }
                    }
                    if (toInner) break;
                }
                if (toInner) continue;

                if (cellXY != NULL) {
                    (*cellXY)[0] = x;
                    (*cellXY)[1] = y;
                }
                foundCell = TRUE;
                break;
            }
        }
        if (intersectX == -1 && intersectY == -1) {
            break;
        }
        else {
            // if we failed to find anything, try again but without any requirements of
            // intersecting
            intersectX = -1;
            intersectY = -1;
            continue;
        }
    }

    // re-mark space taken by ignoreView as occupied
    MarkCellsAsOccupiedForView(ignoreView, occupied);
    return foundCell;
}

void CellLayout::OnDragEnter()
{
    mDragEnforcer->OnDragEnter();
    mDragging = TRUE;
}

void CellLayout::OnDragExit()
{
    mDragEnforcer->OnDragExit();
    // This can actually be called when we aren't in a drag, e.g. when adding a new
    // item to this layout via the customize drawer.
    // Guard against that case.
    if (mDragging) {
        mDragging = FALSE;
    }

    // Invalidate the drag data
    (*mDragCell)[0] = (*mDragCell)[1] = -1;
    (*mDragOutlineAnims)[mDragOutlineCurrent]->AnimateOut();
    mDragOutlineCurrent = (mDragOutlineCurrent + 1) % mDragOutlineAnims->GetLength();
    RevertTempState();
    SetIsDragOverlapping(FALSE);
}

void CellLayout::OnDropChild(
    /* [in] */ IView* child)
{
    if (child != NULL) {
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        LayoutParams* lp = (LayoutParams*)_lp.Get();
        lp->mDropped = TRUE;
        child->RequestLayout();
    }
}

void CellLayout::CellToRect(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 cellHSpan,
    /* [in] */ Int32 cellVSpan,
    /* [in] */ IRect* resultRect)
{
    const Int32 cellWidth = mCellWidth;
    const Int32 cellHeight = mCellHeight;
    const Int32 widthGap = mWidthGap;
    const Int32 heightGap = mHeightGap;

    Int32 hStartPadding, vStartPadding;
    GetPaddingLeft(&hStartPadding);
    GetPaddingTop(&vStartPadding);

    Int32 width = cellHSpan * cellWidth + ((cellHSpan - 1) * widthGap);
    Int32 height = cellVSpan * cellHeight + ((cellVSpan - 1) * heightGap);

    Int32 x = hStartPadding + cellX * (cellWidth + widthGap);
    Int32 y = vStartPadding + cellY * (cellHeight + heightGap);

    resultRect->Set(x, y, x + width, y + height);
}

AutoPtr<ArrayOf<Int32> > CellLayout::RectToCell(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ArrayOf<Int32>* result)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    return RectToCell(res, width, height, result);
}

AutoPtr<ArrayOf<Int32> > CellLayout::RectToCell(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ArrayOf<Int32>* result)
{
    // Always assume we're working with the smallest span to make sure we
    // reserve enough space in both orientations.
    Int32 actualWidth = resources->GetDimensionPixelSize(R::dimen::workspace_cell_width);
    Int32 actualHeight = resources->GetDimensionPixelSize(R::dimen::workspace_cell_height);
    Int32 smallerSize = Elastos::Core::Math::Min(actualWidth, actualHeight);

    // Always round up to next largest cell
    Int32 spanX = (Int32) Elastos::Core::Math::Ceil(width / (Float) smallerSize);
    Int32 spanY = (Int32) Elastos::Core::Math::Ceil(height / (Float) smallerSize);

    if (result == NULL) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        (*array)[0] = spanX;
        (*array)[1] = spanY;
        return array;
    }
    (*result)[0] = spanX;
    (*result)[1] = spanY;
    return result;
}

AutoPtr<ArrayOf<Int32> > CellLayout::CellSpansToSize(
    /* [in] */ Int32 hSpans,
    /* [in] */ Int32 vSpans)
{
    AutoPtr<ArrayOf<Int32> > size = ArrayOf<Int32>::Alloc(2);
    (*size)[0] = hSpans * mCellWidth + (hSpans - 1) * mWidthGap;
    (*size)[1] = vSpans * mCellHeight + (vSpans - 1) * mHeightGap;
    return size;
}

void CellLayout::CalculateSpans(
    /* [in] */ ItemInfo* info)
{
    Int32 minWidth;
    Int32 minHeight;

    if (ILauncherAppWidgetInfo::Probe(info)) {
        AutoPtr<LauncherAppWidgetInfo> lawInfo = (LauncherAppWidgetInfo*)info;
        minWidth = lawInfo->mMinWidth;
        minHeight = lawInfo->mMinHeight;
    }
    else if (IPendingAddWidgetInfo::Probe(info)) {
        AutoPtr<PendingAddWidgetInfo> pawInfo = (PendingAddWidgetInfo*)info;
        pawInfo->mInfo->GetMinWidth(&minWidth);
        pawInfo->mInfo->GetMinHeight(&minHeight);
    }
    else {
        // It's not a widget, so it must be 1x1
        info->mSpanX = info->mSpanY = 1;
        return;
    }
    AutoPtr<ArrayOf<Int32> > spans = RectToCell(minWidth, minHeight, NULL);
    info->mSpanX = (*spans)[0];
    info->mSpanY = (*spans)[1];
}

Boolean CellLayout::GetVacantCell(
    /* [in] */ ArrayOf<Int32>* vacant,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
{
    return FindVacantCell(vacant, spanX, spanY, mCountX, mCountY, mOccupied);
}

Boolean CellLayout::FindVacantCell(
    /* [in] */ ArrayOf<Int32>* vacant,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Int32 xCount,
    /* [in] */ Int32 yCount,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    for (Int32 y = 0; y < yCount; y++) {
        for (Int32 x = 0; x < xCount; x++) {
            Boolean available = !(*(*occupied)[x])[y];
            Boolean breakOut = FALSE
            for (Int32 i = x; i < x + spanX - 1 && x < xCount; i++) { // out
                for (Int32 j = y; j < y + spanY - 1 && y < yCount; j++) {
                    available = available && !(*(*occupied)[i])[j];
                    if (!available) {
                        breakOut = TRUE;
                        break;
                }
                if (breakOut)
                    break;
            }

            if (available) {
                (*vacant)[0] = x;
                (*vacant)[1] = y;
                return TRUE;
            }
        }
    }

    return FALSE;
}

void CellLayout::ClearOccupiedCells()
{
    for (Int32 x = 0; x < mCountX; x++) {
        for (Int32 y = 0; y < mCountY; y++) {
            (*(*mOccupied)[x])[y] = FALSE;
        }
    }
}

void CellLayout::OnMove(
    /* [in] */ IView* view,
    /* [in] */ Int32 newCellX,
    /* [in] */ Int32 newCellY,
    /* [in] */ Int32 newSpanX,
    /* [in] */ Int32 newSpanY)
{
    MarkCellsAsUnoccupiedForView(view);
    MarkCellsForView(newCellX, newCellY, newSpanX, newSpanY, mOccupied, TRUE);
}

void CellLayout::MarkCellsAsOccupiedForView(
    /* [in] */ IView* view)
{
    MarkCellsAsOccupiedForView(view, mOccupied);
}

void CellLayout::MarkCellsAsOccupiedForView(
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    AutoPtr<IViewParent> vp;
    if (view == NULL || (view->GetParent((IViewParent**)&vp),
        vp.Get() != IViewParent::Probe(mShortcutsAndWidgets)))
        return;
    AutoPtr<IViewGroupLayoutParams> _lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
    LayoutParams* lp = (LayoutParams*)_lp.Get();
    MarkCellsForView(lp->mCellX, lp->mCellY, lp->mCellHSpan, lp->mCellVSpan, occupied, TRUE);
}

void CellLayout::MarkCellsAsUnoccupiedForView(
    /* [in] */ IView* view)
{
    MarkCellsAsUnoccupiedForView(view, mOccupied);
}

void CellLayout::MarkCellsAsUnoccupiedForView(
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied)
{
    AutoPtr<IViewParent> vp;
    if (view == NULL || (view->GetParent((IViewParent**)&vp),
        vp.Get() != IViewParent::Probe(mShortcutsAndWidgets)))
        return;
    AutoPtr<IViewGroupLayoutParams> _lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
    LayoutParams* lp = (LayoutParams*)_lp.Get();
    MarkCellsForView(lp->mCellX, lp->mCellY, lp->mCellHSpan, lp->mCellVSpan, occupied, FALSE);
}

void CellLayout::MarkCellsForView(
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
    /* [in] */ Boolean value)
{
    if (cellX < 0 || cellY < 0) return;
    for (Int32 x = cellX; x < cellX + spanX && x < mCountX; x++) {
        for (Int32 y = cellY; y < cellY + spanY && y < mCountY; y++) {
            (*(*occupied)[x])[y] = value;
        }
    }
}

Int32 CellLayout::GetDesiredWidth()
{
    Int32 pl, pr;
    GetPaddingLeft(&pl);
    GetPaddingRight(&pr);
    return pl + pr + (mCountX * mCellWidth) +
            (Elastos::Core::Math::Max((mCountX - 1), 0) * mWidthGap);
}

Int32 CellLayout::GetDesiredHeight()
{
    Int32 pt, pb;
    GetPaddingTop(&pt);
    GetPaddingBottom(&pb);
    return pt + pb + (mCountY * mCellHeight) +
            (Elastos::Core::Math::Max((mCountY - 1), 0) * mHeightGap);
}

ECode CellLayout::IsOccupied(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (x < mCountX && y < mCountY) {
        *result = (*(*mOccupied)[x])[y];
        return NOERROR;
    }
    else {
        Logger::E(TAG, "Position exceeds the bound of this CellLayout");
        return E_RUNTIME_EXCEPTION;
    }
}

ECode CellLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** lp)
{
    VALIDATE_NOT_NULL(lp);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    *lp = new LayoutParams(context, attrs);
    REFCOUNT_ADD(*lp);
    return NOERROR;
}

Boolean CellLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ICellLayoutLayoutParams::Probe(p) != NULL;
}

AutoPtr<IViewGroupLayoutParams> CellLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return new LayoutParams(p);
}

Boolean CellLayout::LastDownOnOccupiedCell()
{
    return mLastDownOnOccupiedCell;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
