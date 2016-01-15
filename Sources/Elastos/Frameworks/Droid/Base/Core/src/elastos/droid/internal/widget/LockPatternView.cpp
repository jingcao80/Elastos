
#include "elastos/droid/internal/widget/LockPatternView.h"
#include "elastos/droid/internal/widget/CLockPatternViewCell.h"
#include "elastos/droid/internal/widget/LockPatternUtils.h"
#include "elastos/droid/internal/widget/CLockPatternViewSavedState.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/animation/ValueAnimator.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::ValueAnimator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PaintJoin_ROUND;
using Elastos::Droid::Graphics::PaintCap_ROUND;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Core::CArrayOf;
using Elastos::Core::CString;
using Elastos::Core::IFloat;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String LockPatternView::TAG("LockPatternView");
const Int32 LockPatternView::ASPECT_SQUARE = 0;
const Int32 LockPatternView::ASPECT_LOCK_WIDTH = 1;
const Int32 LockPatternView::ASPECT_LOCK_HEIGHT = 2;
const Int32 LockPatternView::MILLIS_PER_CIRCLE_ANIMATING = 700;
const Boolean LockPatternView::PROFILE_DRAWING = FALSE;
const Float LockPatternView::DRAG_THRESHHOLD = 0.0f;

Boolean static InitStatic()
{
    for (Int32 i = 0; i < 3; i++) {
        for (Int32 j = 0; j < 3; j++) {
            AutoPtr<CLockPatternViewCell> cell;
            CLockPatternViewCell::NewByFriend(i, j, (CLockPatternViewCell**)&cell);
            LockPatternView::Cell::sCells[i][j] = cell;
        }
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::Cell
/////////////////////////////////////////////////////////////
Boolean LockPatternView::Cell::sInit = InitStatic();
AutoPtr<ILockPatternViewCell> LockPatternView::Cell::sCells[3][3];
CAR_INTERFACE_IMPL(LockPatternView::Cell, Object, ILockPatternViewCell);
LockPatternView::Cell::Cell()
    : mRow(0)
    , mColumn(0)
{}

ECode LockPatternView::Cell::constructor(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    CheckRange(row, column);
    mRow = row;
    mColumn = column;
    return NOERROR;
}

ECode LockPatternView::Cell::CheckRange(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    if (row < 0 || row > 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (column < 0 || column > 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LockPatternView::Cell::GetRow(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRow;
    return NOERROR;
}

ECode LockPatternView::Cell::GetColumn(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mColumn;
    return NOERROR;
}

AutoPtr<ILockPatternViewCell> LockPatternView::Cell::Of(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    CheckRange(row, column);
    return sCells[row][column];
}

ECode LockPatternView::Cell::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("(row=");
    sb.Append(mRow);
    sb.Append(",clmn=");
    sb.Append(mColumn);
    sb.Append(")");
    return sb.ToString(str);
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::CellState
/////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(LockPatternView::CellState, Object, ILockPatternViewCellState);
LockPatternView::CellState::CellState()
    : mScale(1.0f)
    , mTranslateY(0.0f)
    , mAlpha(1.0f)
    , mSize(0)
    , mLineEndX(Elastos::Core::Math::FLOAT_MIN_VALUE)
    , mLineEndY(Elastos::Core::Math::FLOAT_MIN_VALUE)
{}

ECode LockPatternView::CellState::GetScale(
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);
    *scale = mScale;
    return NOERROR;
}

ECode LockPatternView::CellState::SetScale(
    /* [in] */ Float scale)
{
    mScale = scale;
    return NOERROR;
}

ECode LockPatternView::CellState::GetTranslateY(
    /* [out] */ Float* translateY)
{
    VALIDATE_NOT_NULL(translateY);
    *translateY = mTranslateY;
    return NOERROR;
}

ECode LockPatternView::CellState::SetTranslateY(
    /* [in] */ Float translateY)
{
    mTranslateY = translateY;
    return NOERROR;
}

ECode LockPatternView::CellState::GetAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode LockPatternView::CellState::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode LockPatternView::CellState::GetSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mSize;
    return NOERROR;
}

ECode LockPatternView::CellState::SetSize(
    /* [in] */ Float size)
{
    mSize = size;
    return NOERROR;
}

ECode LockPatternView::CellState::GetLineEndX(
    /* [out] */ Float* lineEndX)
{
    VALIDATE_NOT_NULL(lineEndX);
    *lineEndX = mLineEndX;
    return NOERROR;
}

ECode LockPatternView::CellState::SetLineEndX(
    /* [in] */ Float lineEndX)
{
    mLineEndX = lineEndX;
    return NOERROR;
}

ECode LockPatternView::CellState::GetLineEndY(
    /* [out] */ Float* lineEndY)
{
    VALIDATE_NOT_NULL(lineEndY);
    *lineEndY = mLineEndY;
    return NOERROR;
}

ECode LockPatternView::CellState::SetLineEndY(
    /* [in] */ Float lineEndY)
{
    mLineEndY = lineEndY;
    return NOERROR;
}

ECode LockPatternView::CellState::GetLineAnimator(
    /* [out] */ IValueAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = mLineAnimator;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode LockPatternView::CellState::SetLineAnimator(
    /* [in] */ IValueAnimator* animator)
{
    mLineAnimator = animator;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::SavedState
/////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(LockPatternView::SavedState, View::BaseSavedState, ILockPatternViewSavedState);
LockPatternView::SavedState::SavedState()
    : mDisplayMode(0)
    , mInputEnabled(FALSE)
    , mInStealthMode(FALSE)
    , mTactileFeedbackEnabled(FALSE)
{}

ECode LockPatternView::SavedState::constructor()
{
    return NOERROR;
}

ECode LockPatternView::SavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ const String& serializedPattern,
    /* [in] */ Int32 displayMode,
    /* [in] */ Boolean inputEnabled,
    /* [in] */ Boolean inStealthMode,
    /* [in] */ Boolean tactileFeedbackEnabled)
{
    BaseSavedState::constructor(superState);
    mSerializedPattern = serializedPattern;
    mDisplayMode = displayMode;
    mInputEnabled = inputEnabled;
    mInStealthMode = inStealthMode;
    mTactileFeedbackEnabled = tactileFeedbackEnabled;
    return NOERROR;
}

ECode LockPatternView::SavedState::GetSerializedPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = mSerializedPattern;
    return NOERROR;
}

ECode LockPatternView::SavedState::GetDisplayMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mDisplayMode;
    return NOERROR;
}

ECode LockPatternView::SavedState::IsInputEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInputEnabled;
    return NOERROR;
}

ECode LockPatternView::SavedState::IsInStealthMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInStealthMode;
    return NOERROR;
}

ECode LockPatternView::SavedState::IsTactileFeedbackEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTactileFeedbackEnabled;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::CellRunnable
/////////////////////////////////////////////////////////////
LockPatternView::CellRunnable::CellRunnable(
    /* [in] */ LockPatternView* host,
    /* [in] */ CellState* state)
    : mHost(host)
    , mCellState(state)
{}

ECode LockPatternView::CellRunnable::Run()
{
    mHost->StartSizeAnimation(mHost->mDotSizeActivated, mHost->mDotSize, 192, mHost->mFastOutSlowInInterpolator,
            mCellState, NULL);
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::AnimatorUpdateListener
/////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(LockPatternView::AnimatorUpdateListener, Object, IAnimatorUpdateListener);
LockPatternView::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ LockPatternView* host,
    /* [in] */ CellState* state,
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float targetX,
    /* [in] */ Float targetY)
    : mHost(host)
    , mCellState(state)
    , mStartX(0)
    , mStartY(0)
    , mTargetX(0)
    , mTargetY(0)
{}

ECode LockPatternView::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> v;
    animation->GetAnimatedValue((IInterface**)&v);
    assert(IFloat::Probe(v));
    Float t = 0;
    IFloat::Probe(v)->GetValue(&t);
    mCellState->mLineEndX = (1 - t) * mStartX + t * mTargetX;
    mCellState->mLineEndY = (1 - t) * mStartY + t * mTargetY;
    mHost->Invalidate();
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::ViewAnimatorListenerAdapter
/////////////////////////////////////////////////////////////
LockPatternView::ViewAnimatorListenerAdapter::ViewAnimatorListenerAdapter(
    /* [in] */ CellState* state)
    : mCellState(state)
{}

ECode LockPatternView::ViewAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mCellState->mLineAnimator = NULL;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::AnimatorUpdateListener2
/////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(LockPatternView::AnimatorUpdateListener2, Object, IAnimatorUpdateListener);
LockPatternView::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ LockPatternView* host,
    /* [in] */ CellState* state)
    : mHost(host)
    , mCellState(state)
{}

ECode LockPatternView::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> v;
    animation->GetAnimatedValue((IInterface**)&v);
    assert(IFloat::Probe(v));
    IFloat::Probe(v)->GetValue(&mCellState->mSize);
    mHost->Invalidate();
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                  LockPatternView::ViewAnimatorListenerAdapter2
/////////////////////////////////////////////////////////////
LockPatternView::ViewAnimatorListenerAdapter2::ViewAnimatorListenerAdapter2(
    /* [in] */ IRunnable* run)
    : mEndRunnable(run)
{}

ECode LockPatternView::ViewAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mEndRunnable->Run();
    return NOERROR;
}

CAR_INTERFACE_IMPL(LockPatternView, View, ILockPatternView);
LockPatternView::LockPatternView()
    : mDotSize(0)
    , mDotSizeActivated(0)
    , mPathWidth(0)
    , mDrawingProfilingStarted(FALSE)
    , mInProgressX(-1)
    , mInProgressY(-1)
    , mAnimatingPeriodStart(0)
    , mPatternDisplayMode(DisplayMode_Correct)
    , mInputEnabled(TRUE)
    , mInStealthMode(FALSE)
    , mEnableHapticFeedback(TRUE)
    , mPatternInProgress(FALSE)
    , mHitFactor(0.6f)
    , mSquareWidth(0)
    , mSquareHeight(0)
    , mAspect(0)
    , mRegularColor(0)
    , mErrorColor(0)
    , mSuccessColor(0)
{
    CPaint::New((IPaint**)&mPaint);
    CPaint::New((IPaint**)&mPathPaint);
    CArrayList::New(9, (IArrayList**)&mPattern);
    memset(mPatternDrawLookup, 0, sizeof(mPatternDrawLookup));
    CPaint::New((IPaint**)&mCurrentPath);
    CRect::NewByFriend((CRect**)&mInvalidate);
    CRect::NewByFriend((CRect**)&mTmpInvalidateRect);
}

ECode LockPatternView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode LockPatternView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::LockPatternView),
            ArraySize(R::styleable::LockPatternView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    String aspect;
    a->GetString(R::styleable::LockPatternView_aspect, &aspect);

    if (aspect.Equals("square")) {
        mAspect = ASPECT_SQUARE;
    }
    else if (aspect.Equals("lock_width")) {
        mAspect = ASPECT_LOCK_WIDTH;
    }
    else if (aspect.Equals("lock_height")) {
        mAspect = ASPECT_LOCK_HEIGHT;
    }
    else {
        mAspect = ASPECT_SQUARE;
    }

    SetClickable(TRUE);

    mPathPaint->SetAntiAlias(TRUE);
    mPathPaint->SetDither(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetColor(R::color::lock_pattern_view_regular_color, &mRegularColor);
    res->GetColor(R::color::lock_pattern_view_error_color, &mErrorColor);
    res->GetColor(R::color::lock_pattern_view_success_color, &mSuccessColor);
    a->GetColor(R::styleable::LockPatternView_regularColor, mRegularColor, &mRegularColor);
    a->GetColor(R::styleable::LockPatternView_errorColor, mErrorColor, &mErrorColor);
    a->GetColor(R::styleable::LockPatternView_successColor, mSuccessColor, &mSuccessColor);

    Int32 pathColor = 0;
    a->GetColor(R::styleable::LockPatternView_pathColor, mRegularColor, &pathColor);
    mPathPaint->SetColor(pathColor);

    mPathPaint->SetStyle(PaintStyle_STROKE);
    mPathPaint->SetStrokeJoin(PaintJoin_ROUND);
    mPathPaint->SetStrokeCap(PaintCap_ROUND);

    res->GetDimensionPixelSize(R::dimen::lock_pattern_dot_line_width, &mPathWidth);
    mPathPaint->SetStrokeWidth(mPathWidth);

    res->GetDimensionPixelSize(R::dimen::lock_pattern_dot_size, &mDotSize);
    res->GetDimensionPixelSize(R::dimen::lock_pattern_dot_size_activated, &mDotSizeActivated);

    mPaint->SetAntiAlias(TRUE);
    mPaint->SetDither(TRUE);

    const Int32 LEN = 3;
    mCellStates = ArrayOf<IArrayOf*>::Alloc(LEN);
    for (Int32 i = 0; i < LEN; i++) {
        AutoPtr<IArrayOf> item;
        CArrayOf::New(EIID_ILockPatternViewCellState, LEN, (IArrayOf**)&item);

        for (Int32 j = 0; j < LEN; j++) {
            AutoPtr<ILockPatternViewCellState> state = new CellState();
            ((CellState*)state.Get())->mSize = mDotSize;
            item->Set(j, state);
        }

        mCellStates->Set(i, item);
    }

    AnimationUtils::LoadInterpolator(context, R::interpolator::fast_out_slow_in,
        (IInterpolator**)&mFastOutSlowInInterpolator);
    AnimationUtils::LoadInterpolator(context, R::interpolator::linear_out_slow_in,
        (IInterpolator**)&mLinearOutSlowInInterpolator);
    return NOERROR;
}

ECode LockPatternView::GetCellStates(
    /* [out, callee] */ ArrayOf<IArrayOf*>** states)
{
    VALIDATE_NOT_NULL(states);
    *states = mCellStates;
    REFCOUNT_ADD(*states);
    return NOERROR;
}

ECode LockPatternView::IsInStealthMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInStealthMode;
    return NOERROR;
}

ECode LockPatternView::IsTactileFeedbackEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEnableHapticFeedback;
    return NOERROR;
}

ECode LockPatternView::SetInStealthMode(
    /* [in] */ Boolean inStealthMode)
{
    mInStealthMode = inStealthMode;
    return NOERROR;
}

ECode LockPatternView::SetTactileFeedbackEnabled(
    /* [in] */ Boolean tactileFeedbackEnabled)
{
    mEnableHapticFeedback = tactileFeedbackEnabled;
    return NOERROR;
}

ECode LockPatternView::SetOnPatternListener(
    /* [in] */ IOnPatternListener* onPatternListener)
{
    mOnPatternListener = onPatternListener;
    return NOERROR;
}

ECode LockPatternView::SetPattern(
    /* [in] */ DisplayMode displayMode,
    /* [in] */ IList* pattern)
{
    mPattern->Clear();
    mPattern->AddAll(ICollection::Probe(pattern));
    ClearPatternDrawLookup();
    Int32 size = 0;
    pattern->GetSize(&size);
    Int32 row = 0, column = 0;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        pattern->Get(i, (IInterface**)&item);
        Cell* cell = (Cell*)IObject::Probe(item);
        cell->GetRow(&row);
        cell->GetColumn(&column);
        mPatternDrawLookup[row][column] = TRUE;
    }

    SetDisplayMode(displayMode);
    return NOERROR;
}

ECode LockPatternView::SetDisplayMode(
    /* [in] */ DisplayMode displayMode)
{
    mPatternDisplayMode = displayMode;
    if (displayMode == DisplayMode_Animate) {
        Int32 size = 0;
        if ((mPattern->GetSize(&size), size) == 0) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mAnimatingPeriodStart = SystemClock::GetElapsedRealtime();
        AutoPtr<IInterface> item;
        mPattern->Get(0, (IInterface**)&item);
        Cell* first = (Cell*)IObject::Probe(item);
        Int32 row = 0, column = 0;
        first->GetColumn(&column);
        first->GetRow(&row);
        mInProgressX = GetCenterXForColumn(column);
        mInProgressY = GetCenterYForRow(row);
        ClearPatternDrawLookup();
    }
    Invalidate();
    return NOERROR;
}

ECode LockPatternView::ClearPattern()
{
    ResetPattern();
    return NOERROR;
}

ECode LockPatternView::DisableInput()
{
    mInputEnabled = FALSE;
    return NOERROR;
}

ECode LockPatternView::EnableInput()
{
    mInputEnabled = TRUE;
    return NOERROR;
}

ECode LockPatternView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    VALIDATE_NOT_NULL(handled);
    AutoPtr<IAccessibilityManager> am;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&am);
    Boolean res = FALSE;
    if ((am->IsTouchExplorationEnabled(&res), res)) {
        Int32 action = 0;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                event->SetAction(IMotionEvent::ACTION_DOWN);
                break;
            case IMotionEvent::ACTION_HOVER_MOVE:
                event->SetAction(IMotionEvent::ACTION_MOVE);
                break;
            case IMotionEvent::ACTION_HOVER_EXIT:
                event->SetAction(IMotionEvent::ACTION_UP);
                break;
        }
        OnTouchEvent(event, &res);
        event->SetAction(action);
    }
    return View::OnHoverEvent(event, handled);
}


ECode LockPatternView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    Boolean enabled = FALSE;
    if (!mInputEnabled || (IsEnabled(&enabled), !enabled)) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 action = 0;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            HandleActionDown(event);
            return NOERROR;
        case IMotionEvent::ACTION_UP:
            HandleActionUp(event);
            return NOERROR;
        case IMotionEvent::ACTION_MOVE:
            HandleActionMove(event);
            return NOERROR;
        case IMotionEvent::ACTION_CANCEL:
            if (mPatternInProgress) {
                mPatternInProgress = FALSE;
                ResetPattern();
                NotifyPatternCleared();
            }
            if (PROFILE_DRAWING) {
                if (mDrawingProfilingStarted) {
                    //Debug.stopMethodTracing();
                    mDrawingProfilingStarted = FALSE;
                }
            }
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void LockPatternView::NotifyCellAdded()
{
    SendAccessEvent(R::string::lockscreen_access_pattern_cell_added);
    if (mOnPatternListener != NULL) {
        mOnPatternListener->OnPatternCellAdded(IList::Probe(mPattern));
    }
}

void LockPatternView::NotifyPatternStarted()
{
    SendAccessEvent(R::string::lockscreen_access_pattern_start);
    if (mOnPatternListener != NULL) {
        mOnPatternListener->OnPatternStart();
    }
}

void LockPatternView::NotifyPatternDetected()
{
    SendAccessEvent(R::string::lockscreen_access_pattern_detected);
    if (mOnPatternListener != NULL) {
        mOnPatternListener->OnPatternDetected(IList::Probe(mPattern));
    }
}

void LockPatternView::NotifyPatternCleared()
{
    SendAccessEvent(R::string::lockscreen_access_pattern_cleared);
    if (mOnPatternListener != NULL) {
        mOnPatternListener->OnPatternCleared();
    }
}

void LockPatternView::ResetPattern()
{
    mPattern->Clear();
    ClearPatternDrawLookup();
    mPatternDisplayMode = DisplayMode_Correct;
    Invalidate();
}

void LockPatternView::ClearPatternDrawLookup()
{
    for (Int32 i = 0; i < 3; i++) {
        for (Int32 j = 0; j < 3; j++) {
            mPatternDrawLookup[i][j] = FALSE;
        }
    }
}

void LockPatternView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    Int32 width = w - mPaddingLeft - mPaddingRight;
    mSquareWidth = width / 3.0f;

    Int32 height = h - mPaddingTop - mPaddingBottom;
    mSquareHeight = height / 3.0f;
}

Int32 LockPatternView::ResolveMeasured(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 desired)
{
    Int32 result = 0;
    Int32 specSize = MeasureSpec::GetSize(measureSpec);
    switch (MeasureSpec::GetMode(measureSpec)) {
        case MeasureSpec::UNSPECIFIED:
            result = desired;
            break;
        case MeasureSpec::AT_MOST:
            result = Elastos::Core::Math::Max(specSize, desired);
            break;
        case MeasureSpec::EXACTLY:
        default:
            result = specSize;
    }
    return result;
}

AutoPtr<LockPatternView::Cell> LockPatternView::DetectAndAddHit(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<ILockPatternViewCell> c = CheckForNewHit(x, y);
    AutoPtr<Cell> cell = (Cell*)c.Get();
    if (cell != NULL) {

        AutoPtr<Cell> fillInGapCell;
        AutoPtr<IArrayList> pattern = mPattern;
        Int32 size = 0;
        if ((pattern->GetSize(&size), size) != 0) {
            AutoPtr<IInterface> item;
            pattern->Get(size - 1, (IInterface**)&item);
            Cell* lastCell = (Cell*)IObject::Probe(item);
            Int32 dRow = cell->mRow - lastCell->mRow;
            Int32 dColumn = cell->mColumn - lastCell->mColumn;

            Int32 fillInRow = lastCell->mRow;
            Int32 fillInColumn = lastCell->mColumn;

            if (Elastos::Core::Math::Abs(dRow) == 2 && Elastos::Core::Math::Abs(dColumn) != 1) {
                fillInRow = lastCell->mRow + ((dRow > 0) ? 1 : -1);
            }

            if (Elastos::Core::Math::Abs(dColumn) == 2 && Elastos::Core::Math::Abs(dRow) != 1) {
                fillInColumn = lastCell->mColumn + ((dColumn > 0) ? 1 : -1);
            }

            fillInGapCell = (Cell*)Cell::Of(fillInRow, fillInColumn).Get();
        }

        if (fillInGapCell && !mPatternDrawLookup[fillInGapCell->mRow][fillInGapCell->mColumn]) {
            AddCellToPattern(fillInGapCell);
        }
        AddCellToPattern(cell);
        if (mEnableHapticFeedback) {
            Boolean tmp = FALSE;
            PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY,
                    IHapticFeedbackConstants::FLAG_IGNORE_VIEW_SETTING
                    | IHapticFeedbackConstants::FLAG_IGNORE_GLOBAL_SETTING, &tmp);
        }
        return cell;
    }
    return NULL;
}

void LockPatternView::AddCellToPattern(
    /* [in] */ Cell* newCell)
{
    Int32 row = 0, column = 0;
    newCell->GetRow(&row);
    newCell->GetColumn(&column);
    mPatternDrawLookup[row][column] = TRUE;
    Int32 size = 0;
    mPattern->GetSize(&size);
    mPattern->Set(size, (IObject*)newCell->Probe(EIID_IObject));
    if (!mInStealthMode) {
        StartCellActivatedAnimation(newCell);
    }
    NotifyCellAdded();
}

void LockPatternView::StartCellActivatedAnimation(
    /* [in] */ Cell* cell)
{
    AutoPtr<IArrayOf> rows = (*mCellStates)[cell->mRow];
    AutoPtr<IInterface> item;
    rows->Get(cell->mColumn, (IInterface**)&item);
    AutoPtr<CellState> cellState = (CellState*)ILockPatternViewCellState::Probe(item);
    AutoPtr<IRunnable> run = new CellRunnable(this, cellState);
    StartSizeAnimation(mDotSize, mDotSizeActivated, 96, mLinearOutSlowInInterpolator,
            cellState, run);
    StartLineEndAnimation(cellState, mInProgressX, mInProgressY,
            GetCenterXForColumn(cell->mColumn), GetCenterYForRow(cell->mRow));
}

void LockPatternView::StartLineEndAnimation(
    /* [in] */ CellState* state,
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float targetX,
    /* [in] */ Float targetY)
{
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = 0;
    (*values)[1] = 1;
    AutoPtr<IValueAnimator> valueAnimator = ValueAnimator::OfFloat(values);
    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this,
            state, startX, startY, targetX, targetY);
    valueAnimator->AddUpdateListener(listener);

    AutoPtr<IAnimatorListener> l = new ViewAnimatorListenerAdapter(state);
    IAnimator::Probe(valueAnimator)->AddListener(l);
    IAnimator::Probe(valueAnimator)->SetInterpolator(ITimeInterpolator::Probe(mFastOutSlowInInterpolator));
    IAnimator::Probe(valueAnimator)->SetDuration(100);
    IAnimator::Probe(valueAnimator)->Start();
    state->mLineAnimator = valueAnimator;
}

void LockPatternView::StartSizeAnimation(
    /* [in] */ Float start,
    /* [in] */ Float end,
    /* [in] */ Int64 duration,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ CellState* state,
    /* [in] */ IRunnable* endRunnable)
{
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = start;
    (*values)[1] = end;
    AutoPtr<IValueAnimator> valueAnimator = ValueAnimator::OfFloat(values);
    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener2(this, state);
    valueAnimator->AddUpdateListener(listener);
    if (endRunnable != NULL) {
        AutoPtr<IAnimatorListener> l = new ViewAnimatorListenerAdapter2(endRunnable);
        IAnimator::Probe(valueAnimator)->AddListener(l);
    }
    IAnimator::Probe(valueAnimator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    IAnimator::Probe(valueAnimator)->SetDuration(duration);
    IAnimator::Probe(valueAnimator)->Start();
}

AutoPtr<ILockPatternViewCell> LockPatternView::CheckForNewHit(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 rowHit = GetRowHit(y);
    if (rowHit < 0) {
        return NULL;
    }
    Int32 columnHit = GetColumnHit(x);
    if (columnHit < 0) {
        return NULL;
    }

    if (mPatternDrawLookup[rowHit][columnHit]) {
        return NULL;
    }
    return Cell::Of(rowHit, columnHit);
}

Int32 LockPatternView::GetRowHit(
    /* [in] */ Float y)
{
    Float squareHeight = mSquareHeight;
    Float hitSize = squareHeight * mHitFactor;

    Float offset = mPaddingTop + (squareHeight - hitSize) / 2.0f;
    for (Int32 i = 0; i < 3; i++) {

        Float hitTop = offset + squareHeight * i;
        if (y >= hitTop && y <= hitTop + hitSize) {
            return i;
        }
    }
    return -1;
}

Int32 LockPatternView::GetColumnHit(
    /* [in] */ Float x)
{
    Float squareWidth = mSquareWidth;
    Float hitSize = squareWidth * mHitFactor;

    Float offset = mPaddingLeft + (squareWidth - hitSize) / 2.0f;
    for (Int32 i = 0; i < 3; i++) {

        Float hitLeft = offset + squareWidth * i;
        if (x >= hitLeft && x <= hitLeft + hitSize) {
            return i;
        }
    }
    return -1;
}

void LockPatternView::HandleActionMove(
    /* [in] */ IMotionEvent* event)
{
    // Handle all recent motion events so we don't skip any cells even when the device
    // is busy...
    const Float radius = mPathWidth;
    Int32 historySize = 0;
    event->GetHistorySize(&historySize);
    mTmpInvalidateRect->SetEmpty();
    Boolean invalidateNow = FALSE;
    for (Int32 i = 0; i < historySize + 1; i++) {
        Float x, y;
        if (i < historySize) {
            event->GetHistoricalX(i, &x);
            event->GetHistoricalY(i, &y);
        }
        else {
            event->GetX(&x);
            event->GetY(&y);
        }

        AutoPtr<Cell> hitCell = DetectAndAddHit(x, y);
        Int32 patternSize = 0;
        mPattern->GetSize(&patternSize);
        if (hitCell && patternSize == 1) {
            mPatternInProgress = TRUE;
            NotifyPatternStarted();
        }
        Float dx = Elastos::Core::Math::Abs(x - mInProgressX);
        Float dy = Elastos::Core::Math::Abs(y - mInProgressY);
        if (dx > DRAG_THRESHHOLD || dy > DRAG_THRESHHOLD) {
            invalidateNow = true;
        }

        if (mPatternInProgress && patternSize > 0) {
            AutoPtr<IArrayList> pattern = mPattern;
            AutoPtr<IInterface> item;
            pattern->Get(patternSize - 1, (IInterface**)&item);
            Cell* lastCell = (Cell*)IObject::Probe(item);
            Float lastCellCenterX = GetCenterXForColumn(lastCell->mColumn);
            Float lastCellCenterY = GetCenterYForRow(lastCell->mRow);

            // Adjust for drawn segment from last cell to (x,y). Radius accounts for line width.
            Float left = Elastos::Core::Math::Min(lastCellCenterX, x) - radius;
            Float right = Elastos::Core::Math::Max(lastCellCenterX, x) + radius;
            Float top = Elastos::Core::Math::Min(lastCellCenterY, y) - radius;
            Float bottom = Elastos::Core::Math::Max(lastCellCenterY, y) + radius;

            // Invalidate between the pattern's new cell and the pattern's previous cell
            if (hitCell != NULL) {
                Float width = mSquareWidth * 0.5f;
                Float height = mSquareHeight * 0.5f;
                Float hitCellCenterX = GetCenterXForColumn(hitCell->mColumn);
                Float hitCellCenterY = GetCenterYForRow(hitCell->mRow);

                left = Elastos::Core::Math::Min(hitCellCenterX - width, left);
                right = Elastos::Core::Math::Max(hitCellCenterX + width, right);
                top = Elastos::Core::Math::Min(hitCellCenterY - height, top);
                bottom = Elastos::Core::Math::Max(hitCellCenterY + height, bottom);
            }

            // Invalidate between the pattern's last cell and the previous location
            mTmpInvalidateRect->Union(Elastos::Core::Math::Round(left),
                    Elastos::Core::Math::Round(top),
                    Elastos::Core::Math::Round(right),
                    Elastos::Core::Math::Round(bottom));
        }
    }

    event->GetX(&mInProgressX);
    event->GetY(&mInProgressY);

    // To save updates, we only invalidate if the user moved beyond a certain amount.
    if (invalidateNow) {
        mInvalidate->Union(mTmpInvalidateRect);
        Invalidate(mInvalidate);
        mInvalidate->Set(mTmpInvalidateRect);
    }
}

void LockPatternView::SendAccessEvent(
    /* [in] */ Int32 resId)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> csq;
    CString::New(str, (ICharSequence**)&csq);
    AnnounceForAccessibility(csq);
}

void LockPatternView::HandleActionUp(
    /* [in] */ IMotionEvent* event)
{
    Int32 size = 0;
    if ((mPattern->GetSize(&size), size) != 0) {
        mPatternInProgress = FALSE;
        CancelLineAnimations();
        NotifyPatternDetected();
        Invalidate();
    }
    if (PROFILE_DRAWING) {
        if (mDrawingProfilingStarted) {
            //Debug.stopMethodTracing();
            mDrawingProfilingStarted = FALSE;
        }
    }
}

void LockPatternView::CancelLineAnimations()
{
    for (Int32 i = 0; i < 3; i++) {
        AutoPtr<IArrayOf> rows = (*mCellStates)[i];
        for (Int32 j = 0; j < 3; j++) {
            AutoPtr<IInterface> item;
            rows->Get(j, (IInterface**)&item);
            CellState* state = (CellState*)ILockPatternViewCellState::Probe(item);
            if (state->mLineAnimator != NULL) {
                IAnimator::Probe(state->mLineAnimator)->Cancel();
                state->mLineEndX = Elastos::Core::Math::FLOAT_MIN_VALUE;
                state->mLineEndY = Elastos::Core::Math::FLOAT_MIN_VALUE;
            }
        }
    }
}

void LockPatternView::HandleActionDown(
    /* [in] */ IMotionEvent* event)
{
    ResetPattern();
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);
    AutoPtr<Cell> hitCell = DetectAndAddHit(x, y);
    if (hitCell) {
        mPatternInProgress = TRUE;
        mPatternDisplayMode = DisplayMode_Correct;
        NotifyPatternStarted();
    }
    else if (mPatternInProgress) {
        mPatternInProgress = FALSE;
        NotifyPatternCleared();
    }
    if (hitCell) {
        Float startX = GetCenterXForColumn(hitCell->mColumn);
        Float startY = GetCenterYForRow(hitCell->mRow);

        Float widthOffset = mSquareWidth / 2.0f;
        Float heightOffset = mSquareHeight / 2.0f;

        Invalidate((Int32) (startX - widthOffset), (Int32) (startY - heightOffset),
                (Int32) (startX + widthOffset), (Int32) (startY + heightOffset));
    }
    mInProgressX = x;
    mInProgressY = y;
    if (PROFILE_DRAWING) {
        if (!mDrawingProfilingStarted) {
            //Debug.startMethodTracing("LockPatternDrawing");
            mDrawingProfilingStarted = TRUE;
        }
    }
}

Float LockPatternView::GetCenterXForColumn(
    /* [in] */ Int32 column)
{
    return mPaddingLeft + column * mSquareWidth + mSquareWidth / 2.0f;
}

Float LockPatternView::GetCenterYForRow(
    /* [in] */ Int32 row)
{
    return mPaddingTop + row * mSquareHeight + mSquareHeight / 2.0f;
}

Float LockPatternView::CalculateLastSegmentAlpha(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float lastX,
    /* [in] */ Float lastY)
{
    Float diffX = x - lastX;
    Float diffY = y - lastY;
    Float dist = (Float) Elastos::Core::Math::Sqrt(diffX * diffX + diffY * diffY);
    Float frac = dist / mSquareWidth;
    return Elastos::Core::Math::Min(1.f, Elastos::Core::Math::Max(0.f, (frac - 0.3f) * 4.f));
}

Int32 LockPatternView::GetCurrentColor(
    /* [in] */ Boolean partOfPattern)
{
    if (!partOfPattern || mInStealthMode || mPatternInProgress) {
        // unselected circle
        return mRegularColor;
    }
    else if (mPatternDisplayMode == DisplayMode_Wrong) {
        // the pattern is wrong
        return mErrorColor;
    }
    else if (mPatternDisplayMode == DisplayMode_Correct ||
            mPatternDisplayMode == DisplayMode_Animate) {
        return mSuccessColor;
    }
    // throw new IllegalStateException("unknown display mode " + mPatternDisplayMode);
    assert(0 && "unknown display mode");
    return -1;
}

void LockPatternView::DrawCircle(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Float size,
    /* [in] */ Boolean partOfPattern,
    /* [in] */ Float alpha)
{
    mPaint->SetColor(GetCurrentColor(partOfPattern));
    mPaint->SetAlpha((Int32) (alpha * 255));
    canvas->DrawCircle(centerX, centerY, size / 2, mPaint);
}

void LockPatternView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 minimumWidth = GetSuggestedMinimumWidth();
    Int32 minimumHeight = GetSuggestedMinimumHeight();
    Int32 viewWidth = ResolveMeasured(widthMeasureSpec, minimumWidth);
    Int32 viewHeight = ResolveMeasured(heightMeasureSpec, minimumHeight);

    switch (mAspect) {
        case ASPECT_SQUARE:
            viewWidth = viewHeight = Elastos::Core::Math::Min(viewWidth, viewHeight);
            break;
        case ASPECT_LOCK_WIDTH:
            viewHeight = Elastos::Core::Math::Min(viewWidth, viewHeight);
            break;
        case ASPECT_LOCK_HEIGHT:
            viewWidth = Elastos::Core::Math::Min(viewWidth, viewHeight);
            break;
    }
}

void LockPatternView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IArrayList> pattern = mPattern;
    Int32 count = 0;
    pattern->GetSize(&count);

    if (mPatternDisplayMode == DisplayMode_Animate) {
        Int32 oneCycle = (count + 1) * MILLIS_PER_CIRCLE_ANIMATING;
        Int32 spotInCycle = (Int32) (SystemClock::GetElapsedRealtime() - mAnimatingPeriodStart) % oneCycle;
        Int32 numCircles = spotInCycle / MILLIS_PER_CIRCLE_ANIMATING;

        ClearPatternDrawLookup();
        Int32 row = 0, column = 0;
        for (Int32 i = 0; i < numCircles; i++) {
            AutoPtr<IInterface> item;
            pattern->Get(i, (IInterface**)&item);
            Cell* cell = (Cell*)IObject::Probe(item);
            cell->GetRow(&row);
            cell->GetColumn(&column);
            mPatternDrawLookup[row][column] = TRUE;
        }

        Boolean needToUpdateInProgressPoint = numCircles > 0 && numCircles < count;

        if (needToUpdateInProgressPoint) {
            Float percentageOfNextCircle = ((Float) (spotInCycle % MILLIS_PER_CIRCLE_ANIMATING)) /
                            MILLIS_PER_CIRCLE_ANIMATING;

            AutoPtr<IInterface> item;
            pattern->Get(numCircles - 1, (IInterface**)&item);
            Cell* currentCell = (Cell*)IObject::Probe(item);
            Float centerX = GetCenterXForColumn(currentCell->mColumn);
            Float centerY = GetCenterYForRow(currentCell->mRow);

            item = NULL;
            pattern->Get(numCircles, (IInterface**)&item);
            Cell* nextCell = (Cell*)IObject::Probe(item);
            Float dx = percentageOfNextCircle * (GetCenterXForColumn(nextCell->mColumn) - centerX);
            Float dy = percentageOfNextCircle * (GetCenterYForRow(nextCell->mRow) - centerY);
            mInProgressX = centerX + dx;
            mInProgressY = centerY + dy;
        }
        Invalidate();
    }

    AutoPtr<IPath> currentPath = mCurrentPath;
    currentPath->Rewind();

    // draw the circles
    for (Int32 i = 0; i < 3; i++) {
        Float centerY = GetCenterYForRow(i);
        AutoPtr<IArrayOf> rows = (*mCellStates)[i];
        for (Int32 j = 0; j < 3; j++) {
            AutoPtr<IInterface> item;
            rows->Get(j, (IInterface**)&item);
            CellState* cellState = (CellState*)ILockPatternViewCellState::Probe(item);
            Float centerX = GetCenterXForColumn(j);
            Float size = cellState->mSize * cellState->mScale;
            Float translationY = cellState->mTranslateY;
            DrawCircle(canvas, (Int32) centerX, (Int32) centerY + translationY,
                    size, mPatternDrawLookup[i][j], cellState->mAlpha);
        }
    }

    // TODO: the path should be created and cached every time we hit-detect a cell
    // only the last segment of the path should be computed here
    // draw the path of the pattern (unless we are in stealth mode)
    const Boolean drawPath = !mInStealthMode;
    if (drawPath) {
        mPathPaint->SetColor(GetCurrentColor(TRUE /* partOfPattern */));

        Boolean anyCircles = FALSE;
        Float lastX = 0.f;
        Float lastY = 0.f;
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> item;
            pattern->Get(i, (IInterface**)&item);
            Cell* cell = (Cell*)IObject::Probe(item);

            // only draw the part of the pattern stored in
            // the lookup table (this is only different in the case
            // of animation).
            if (!mPatternDrawLookup[cell->mRow][cell->mColumn]) {
                break;
            }
            anyCircles = TRUE;

            Float centerX = GetCenterXForColumn(cell->mColumn);
            Float centerY = GetCenterYForRow(cell->mRow);
            if (i != 0) {
                AutoPtr<IArrayOf> rows = (*mCellStates)[cell->mRow];
                AutoPtr<IInterface> item;
                rows->Get(cell->mColumn, (IInterface**)&item);
                CellState* state = (CellState*)ILockPatternViewCellState::Probe(item);
                currentPath->Rewind();
                currentPath->MoveTo(lastX, lastY);
                if (state->mLineEndX != Elastos::Core::Math::FLOAT_MIN_VALUE
                    && state->mLineEndY != Elastos::Core::Math::FLOAT_MIN_VALUE) {
                    currentPath->LineTo(state->mLineEndX, state->mLineEndY);
                }
                else {
                    currentPath->LineTo(centerX, centerY);
                }
                canvas->DrawPath(currentPath, mPathPaint);
            }
            lastX = centerX;
            lastY = centerY;
        }

        // draw last in progress section
        if ((mPatternInProgress || mPatternDisplayMode == DisplayMode_Animate)
                && anyCircles) {
            currentPath->Rewind();
            currentPath->MoveTo(lastX, lastY);
            currentPath->LineTo(mInProgressX, mInProgressY);

            mPathPaint->SetAlpha((int) (CalculateLastSegmentAlpha(
                    mInProgressX, mInProgressY, lastX, lastY) * 255.f));
            canvas->DrawPath(currentPath, mPathPaint);
        }
    }
}

AutoPtr<IParcelable> LockPatternView::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = View::OnSaveInstanceState();
    AutoPtr<IParcelable> state;
    CLockPatternViewSavedState::New(superState,
            LockPatternUtils::PatternToString(IList::Probe(mPattern)),
            mPatternDisplayMode/*.ordinal()*/,
            mInputEnabled, mInStealthMode, mEnableHapticFeedback, (IParcelable**)&state);
    return state;
}

void LockPatternView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<SavedState> ss = (SavedState*) state;
    AutoPtr<IParcelable> value;
    ss->GetSuperState((IParcelable**)&value);
    View::OnRestoreInstanceState(value);
    String pattern;
    ss->GetSerializedPattern(&pattern);
    SetPattern(DisplayMode_Correct,
            LockPatternUtils::StringToPattern(pattern));
    Int32 mode = 0;
    ss->GetDisplayMode(&mode);
    mPatternDisplayMode = mode/*DisplayMode.values()[mode]*/;
    ss->IsInputEnabled(&mInputEnabled);
    ss->IsInStealthMode(&mInStealthMode);
    ss->IsTactileFeedbackEnabled(&mEnableHapticFeedback);
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
