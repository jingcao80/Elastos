
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNVIEW_H__

#include "elastos/droid/view/View.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::View;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class LockPatternView
    : public Elastos::Droid::View::View
    , public ILockPatternView
{
public:
    class Cell
        : public Object
        , public ILockPatternViewCell
    {
    public:
        CAR_INTERFACE_DECL()

        Cell();

        CARAPI constructor(
            /* [in] */ Int32 row,
            /* [in] */ Int32 column,
            /* [in] */ Byte size);

        virtual CARAPI GetRow(
            /* [out] */ Int32* result);

        virtual CARAPI GetColumn(
            /* [out] */ Int32* result);

        /**
         * @param row The row of the cell.
         * @param column The column of the cell.
         */
        static CARAPI_(AutoPtr<ILockPatternViewCell>) Of(
            /* [in] */ Int32 row,
            /* [in] */ Int32 column,
            /* [in] */ Byte size);

        static CARAPI UpdateSize(
            /* [in] */ Byte size);

        virtual CARAPI ToString(
            /* [out] */ String* str);

    private:
        static CARAPI CheckRange(
            /* [in] */ Int32 row,
            /* [in] */ Int32 column,
            /* [in] */ Byte size);

    public:
        Int32 mRow;
        Int32 mColumn;
        static AutoPtr<ArrayOf<IArrayOf*> > sCells;
    };

    class CellState
        : public Object
        , public ILockPatternViewCellState
    {
    public:
        CAR_INTERFACE_DECL()

        CellState();

        CARAPI GetScale(
            /* [out] */ Float* scale);

        CARAPI SetScale(
            /* [in] */ Float scale);

        CARAPI GetTranslateY(
            /* [out] */ Float* translateY);

        CARAPI SetTranslateY(
            /* [in] */ Float translateY);

        CARAPI GetAlpha(
            /* [out] */ Float* alpha);

        CARAPI SetAlpha(
            /* [in] */ Float alpha);

        CARAPI GetSize(
            /* [out] */ Float* size);

        CARAPI SetSize(
            /* [in] */ Float size);

        CARAPI GetLineEndX(
            /* [out] */ Float* lineEndX);

        CARAPI SetLineEndX(
            /* [in] */ Float lineEndX);

        CARAPI GetLineEndY(
            /* [out] */ Float* lineEndY);

        CARAPI SetLineEndY(
            /* [in] */ Float lineEndY);

        CARAPI GetLineAnimator(
            /* [out] */ IValueAnimator** animator);

        CARAPI SetLineAnimator(
            /* [in] */ IValueAnimator* animator);

    public:
        Float mScale;
        Float mTranslateY;
        Float mAlpha;
        Float mSize;
        Float mLineEndX;
        Float mLineEndY;
        AutoPtr<IValueAnimator> mLineAnimator;
    };

    /**
     * The parecelable for saving and restoring a lock pattern view.
     */
    class LockPatternViewSavedState
        : public View::BaseSavedState
        , public ILockPatternViewSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        LockPatternViewSavedState();

        CARAPI constructor();

        /**
         * Constructor called from {@link LockPatternView#onSaveInstanceState()}
         */
        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ const String& serializedPattern,
            /* [in] */ Int32 displayMode,
            /* [in] */ Byte patternSize,
            /* [in] */ Boolean inputEnabled,
            /* [in] */ Boolean inStealthMode,
            /* [in] */ Boolean tactileFeedbackEnabled,
            /* [in] */ Boolean visibleDots,
            /* [in] */ Boolean showErrorPath);

        CARAPI GetSerializedPattern(
            /* [out] */ String* pattern);

        CARAPI GetDisplayMode(
            /* [out] */ Int32* mode);

        CARAPI GetPatternSize(
            /* [out] */ Byte* result);

        CARAPI IsInputEnabled(
            /* [out] */ Boolean* result);

        CARAPI IsInStealthMode(
            /* [out] */ Boolean* result);

        CARAPI IsTactileFeedbackEnabled(
            /* [out] */ Boolean* result);

        CARAPI IsVisibleDots(
            /* [out] */ Boolean* result);

        CARAPI IsShowErrorPath(
            /* [out] */ Boolean* result);

    private:
        String mSerializedPattern;
        Int32 mDisplayMode;
        Byte mPatternSize;
        Boolean mInputEnabled;
        Boolean mInStealthMode;
        Boolean mTactileFeedbackEnabled;
        Boolean mVisibleDots;
        Boolean mShowErrorPath;
    };

private:
    class CellRunnable : public Runnable
    {
    public:
        CellRunnable(
            /* [in] */ LockPatternView* host,
            /* [in] */ CellState* state);

        // @Override
        CARAPI Run();

    private:
        LockPatternView* mHost;
        AutoPtr<CellState> mCellState;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener(
            /* [in] */ LockPatternView* host,
            /* [in] */ CellState* state,
            /* [in] */ Float startX,
            /* [in] */ Float startY,
            /* [in] */ Float targetX,
            /* [in] */ Float targetY);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        LockPatternView* mHost;
        AutoPtr<CellState> mCellState;
        Float mStartX;
        Float mStartY;
        Float mTargetX;
        Float mTargetY;
    };

    class ViewAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        ViewAnimatorListenerAdapter(
            /* [in] */ CellState* state);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<CellState> mCellState;
    };

    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener2(
            /* [in] */ LockPatternView* host,
            /* [in] */ CellState* state);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        LockPatternView* mHost;
        AutoPtr<CellState> mCellState;
    };

    class ViewAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        ViewAnimatorListenerAdapter2(
            /* [in] */ IRunnable* run);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mEndRunnable;
    };

public:
    CAR_INTERFACE_DECL()

    LockPatternView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetCellStates(
        /* [out, callee] */ ArrayOf<IArrayOf*>** states);

    virtual CARAPI IsInStealthMode(
        /* [out] */ Boolean* result);

    virtual CARAPI IsTactileFeedbackEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI GetLockPatternSize(
        /* [out] */ Int32* result);

    virtual CARAPI SetInStealthMode(
        /* [in] */ Boolean inStealthMode);

    virtual CARAPI SetVisibleDots(
        /* [in] */ Boolean visibleDots);

    virtual CARAPI IsVisibleDots(
        /* [out] */ Boolean* result);

    virtual CARAPI SetShowErrorPath(
        /* [in] */ Boolean showErrorPath);

    virtual CARAPI IsShowErrorPath(
        /* [out] */ Boolean* result);

    virtual CARAPI SetTactileFeedbackEnabled(
        /* [in] */ Boolean tactileFeedbackEnabled);

    virtual CARAPI SetLockPatternSize(
        /* [in] */ Byte size);

    virtual CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    virtual CARAPI SetOnPatternListener(
        /* [in] */ IOnPatternListener* onPatternListener);

    virtual CARAPI SetPattern(
        /* [in] */ DisplayMode displayMode,
        /* [in] */ IList* pattern);

    virtual CARAPI SetDisplayMode(
        /* [in] */ DisplayMode displayMode);

    virtual CARAPI ClearPattern();

    virtual CARAPI DisableInput();

    virtual CARAPI EnableInput();

    virtual CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) NotifyCellAdded();

    CARAPI_(void) NotifyPatternStarted();

    CARAPI_(void) NotifyPatternDetected();

    CARAPI_(void) NotifyPatternCleared();

    CARAPI_(void) ResetPattern();

    CARAPI_(void) ClearPatternDrawLookup();

    CARAPI_(Int32) ResolveMeasured(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 desired);

    CARAPI_(AutoPtr<Cell>) DetectAndAddHit(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) AddCellToPattern(
        /* [in] */ Cell* newCell);

    CARAPI_(void) StartCellActivatedAnimation(
        /* [in] */ Cell* cell);

    CARAPI_(void) StartLineEndAnimation(
        /* [in] */ CellState* state,
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float targetX,
        /* [in] */ Float targetY);

    CARAPI_(void) StartSizeAnimation(
        /* [in] */ Float start,
        /* [in] */ Float end,
        /* [in] */ Int64 duration,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ CellState* state,
        /* [in] */ IRunnable* endRunnable);

    CARAPI_(AutoPtr<ILockPatternViewCell>) CheckForNewHit(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Int32) GetRowHit(
        /* [in] */ Float y);

    CARAPI_(Int32) GetColumnHit(
        /* [in] */ Float x);

    CARAPI_(void) HandleActionMove(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SendAccessEvent(
        /* [in] */ Int32 resId);

    CARAPI_(void) HandleActionUp(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) CancelLineAnimations();

    CARAPI_(void) HandleActionDown(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Float) GetCenterXForColumn(
        /* [in] */ Int32 column);

    CARAPI_(Float) GetCenterYForRow(
        /* [in] */ Int32 row);

    CARAPI_(Float) CalculateLastSegmentAlpha(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float lastX,
        /* [in] */ Float lastY);

    CARAPI_(Int32) GetCurrentColor(
        /* [in] */ Boolean partOfPattern);

    /**
     * @param partOfPattern Whether this circle is part of the pattern.
     */
    CARAPI_(void) DrawCircle(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float centerX,
        /* [in] */ Float centerY,
        /* [in] */ Float size,
        /* [in] */ Boolean partOfPattern,
        /* [in] */ Float alpha);

protected:
    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    static const String TAG;
    static const Int32 ASPECT_SQUARE;
    static const Int32 ASPECT_LOCK_WIDTH;
    static const Int32 ASPECT_LOCK_HEIGHT;
    static const Int32 MILLIS_PER_CIRCLE_ANIMATING;
    static const Boolean PROFILE_DRAWING;
    /**
     * This can be used to avoid updating the display for very small motions or noisy panels.
     * It didn't seem to have much impact on the devices tested, so currently set to 0.
     */
    static const Float DRAG_THRESHHOLD;

    // private final CellState[][] mCellStates;
    AutoPtr<ArrayOf<IArrayOf*> > mCellStates;

    Int32 mDotSize;
    Int32 mDotSizeActivated;
    Int32 mPathWidth;

    Boolean mDrawingProfilingStarted;

    AutoPtr<IPaint> mPaint;
    AutoPtr<IPaint> mPathPaint;
    Byte mPatternSize;
    AutoPtr<IOnPatternListener> mOnPatternListener;
    AutoPtr<IArrayList> mPattern;/*<Cell*/

    AutoPtr<ArrayOf<IArrayOf*> > mPatternDrawLookup;

    Float mInProgressX;
    Float mInProgressY;

    Int64 mAnimatingPeriodStart;

    DisplayMode mPatternDisplayMode;
    Boolean mInputEnabled;
    Boolean mInStealthMode;
    Boolean mEnableHapticFeedback;
    Boolean mPatternInProgress;
    Boolean mVisibleDots;
    Boolean mShowErrorPath;

    Float mHitFactor;

    Float mSquareWidth;
    Float mSquareHeight;

    AutoPtr<IPath> mCurrentPath;
    AutoPtr<CRect> mInvalidate;

    AutoPtr<CRect> mTmpInvalidateRect;

    Int32 mAspect;
    Int32 mRegularColor;
    Int32 mErrorColor;
    Int32 mSuccessColor;

    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNVIEW_H__
