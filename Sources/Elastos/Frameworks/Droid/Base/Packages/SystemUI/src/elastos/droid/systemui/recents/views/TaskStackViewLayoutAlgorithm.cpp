
#include "elastos/droid/systemui/recents/views/TaskStackViewLayoutAlgorithm.h"
#include "elastos/droid/systemui/recents/model/TaskGrouping.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::SystemUI::Recents::Model::ITask;
using Elastos::Droid::SystemUI::Recents::Model::TaskGrouping;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

const Float TaskStackViewLayoutAlgorithm::StackPeekMinScale;
const Float TaskStackViewLayoutAlgorithm::XScale;
const Float TaskStackViewLayoutAlgorithm::LogBase;
const Int32 TaskStackViewLayoutAlgorithm::PrecisionSteps;
AutoPtr<ArrayOf<Float> > TaskStackViewLayoutAlgorithm::sXp;
AutoPtr<ArrayOf<Float> > TaskStackViewLayoutAlgorithm::sPx;

TaskStackViewLayoutAlgorithm::TaskStackViewLayoutAlgorithm(
    /* [in] */ RecentsConfiguration* config)
    : mConfig (config)
    , mMinScrollP(0.0f)
    , mMaxScrollP(0.0f)
    , mInitialScrollP(0.0f)
    , mWithinAffiliationOffset(0)
    , mBetweenAffiliationOffset(0)
{
    CRect::New((IRect**)&mViewRect);
    CRect::New((IRect**)&mStackVisibleRect);
    CRect::New((IRect**)&mStackRect);
    CRect::New((IRect**)&mTaskRect);

    // Precompute the path
    InitializeCurve();
}

/** Computes the stack and task rects */
void TaskStackViewLayoutAlgorithm::ComputeRects(
    /* [in] */ Int32 windowWidth,
    /* [in] */ Int32 windowHeight,
    /* [in] */ IRect* taskStackBounds)
{
    // Compute the stack rects
    mViewRect->Set(0, 0, windowWidth, windowHeight);
    mStackRect->Set(taskStackBounds);
    mStackVisibleRect->Set(taskStackBounds);
    Int32 bottom;
    mViewRect->GetBottom(&bottom);
    mStackVisibleRect->SetBottom(bottom);

    Int32 w;
    mStackRect->GetWidth(&w);
    Int32 widthPadding = (Int32) (mConfig->mTaskStackWidthPaddingPct * w);
    Int32 heightPadding = mConfig->mTaskStackTopPaddingPx;
    mStackRect->Inset(widthPadding, heightPadding);

    // Compute the task rect
    Int32 size;
    mStackRect->GetWidth(&size);
    mStackRect->GetWidth(&w);
    Int32 left;
    mStackRect->GetLeft(&left);
    left += (w - size) / 2; // ?? w == size always
    Int32 top;
    mStackRect->GetTop(&top);
    mTaskRect->Set(left, top, left + size, top + size);

    // Update the affiliation offsets
    Float visibleTaskPct = 0.5f;
    mWithinAffiliationOffset = mConfig->mTaskBarHeight;
    Int32 height;
    mTaskRect->GetHeight(&height);
    mBetweenAffiliationOffset = (Int32)(visibleTaskPct * height);
}

/** Computes the minimum and maximum scroll progress values.  This method may be called before
 * the RecentsConfiguration is set, so we need to pass in the alt-tab state. */
void TaskStackViewLayoutAlgorithm::ComputeMinMaxScroll(
    /* [in] */ IArrayList* tasks,
    /* [in] */ Boolean launchedWithAltTab,
    /* [in] */ Boolean launchedFromHome)
{
    // Clear the progress map
    mTaskProgressMap.Clear();

    // Return early if we have no tasks
    Boolean isEmpty;
    tasks->IsEmpty(&isEmpty);
    if (isEmpty) {
        mMinScrollP = mMaxScrollP = 0;
        return;
    }

    // Note that we should account for the scale difference of the offsets at the screen bottom
    Int32 taskHeight;
    mTaskRect->GetHeight(&taskHeight);
    Int32 bottom;
    mStackVisibleRect->GetBottom(&bottom);
    Float pAtBottomOfStackRect = ScreenYToCurveProgress(bottom);
    Float pWithinAffiliateTop = ScreenYToCurveProgress(bottom - mWithinAffiliationOffset);
    Float scale = CurveProgressToScale(pWithinAffiliateTop);
    Int32 scaleYOffset = (Int32) (((1.0f - scale) * taskHeight) / 2);
    pWithinAffiliateTop = ScreenYToCurveProgress(bottom -
            mWithinAffiliationOffset + scaleYOffset);
    Float pWithinAffiliateOffset = pAtBottomOfStackRect - pWithinAffiliateTop;
    Float pBetweenAffiliateOffset = pAtBottomOfStackRect -
            ScreenYToCurveProgress(bottom - mBetweenAffiliationOffset);
    Float pTaskHeightOffset = pAtBottomOfStackRect -
            ScreenYToCurveProgress(bottom - taskHeight);
    Int32 b;
    mStackRect->GetBottom(&b);
    Float pNavBarOffset = pAtBottomOfStackRect -
            ScreenYToCurveProgress(bottom - (bottom - b));

    // Update the task offsets
    Float pAtBackMostCardTop = 0.5f;
    Float pAtFrontMostCardTop = pAtBackMostCardTop;
    Float pAtSecondFrontMostCardTop = pAtBackMostCardTop;
    Int32 taskCount;
    tasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> item;
        tasks->Get(i, (IInterface**)&item);
        Task* task = (Task*)ITask::Probe(item);
        mTaskProgressMap[task->mKey] = pAtFrontMostCardTop;

        if (i < (taskCount - 1)) {
            // Increment the peek height
            AutoPtr<TaskGrouping> tg = (TaskGrouping*)(task->mGroup).Get();
            Float pPeek = tg->IsFrontMostTask(task) ? pBetweenAffiliateOffset :
                pWithinAffiliateOffset;
            pAtSecondFrontMostCardTop = pAtFrontMostCardTop;
            pAtFrontMostCardTop += pPeek;
        }
    }

    mMaxScrollP = pAtFrontMostCardTop - ((1.0f - pTaskHeightOffset - pNavBarOffset));
    mMinScrollP = taskCount == 1 ? Elastos::Core::Math::Max(mMaxScrollP, 0.0f) : 0.0f;
    if (launchedWithAltTab) {
        if (launchedFromHome) {
            // Center the top most task, since that will be focused first
            mInitialScrollP = pAtSecondFrontMostCardTop - 0.5f;
        }
        else {
            // Center the second top most task, since that will be focused first
            mInitialScrollP = pAtSecondFrontMostCardTop - 0.5f;
        }
    }
    else {
        mInitialScrollP = pAtFrontMostCardTop - 0.825f;
    }
    mInitialScrollP = Elastos::Core::Math::Max(0.0f, mInitialScrollP);
}

/** Update/get the transform */
AutoPtr<TaskViewTransform> TaskStackViewLayoutAlgorithm::GetStackTransform(
    /* [in] */ Task* task,
    /* [in] */ Float stackScroll,
    /* [in] */ TaskViewTransform* transformOut,
    /* [in] */ TaskViewTransform* prevTransform)
{
    // Return early if we have an invalid index
    if (task == NULL || mTaskProgressMap.Find(task->mKey) == mTaskProgressMap.End()) {
        transformOut->Reset();
        return transformOut;
    }
    return GetStackTransform(mTaskProgressMap[task->mKey], stackScroll, transformOut, prevTransform);
}

/** Update/get the transform */
AutoPtr<TaskViewTransform> TaskStackViewLayoutAlgorithm::GetStackTransform(
    /* [in] */ Float taskProgress,
    /* [in] */ Float stackScroll,
    /* [in] */ TaskViewTransform* transformOut,
    /* [in] */ TaskViewTransform* prevTransform)
{
    Float pTaskRelative = taskProgress - stackScroll;
    Float pBounded = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(pTaskRelative, 1.0f));
    // If the task top is outside of the bounds below the screen, then immediately reset it
    if (pTaskRelative > 1.0f) {
        transformOut->Reset();
        transformOut->mRect->Set(mTaskRect);
        return transformOut;
    }
    // The check for the top is trickier, since we want to show the next task if it is at all
    // visible, even if p < 0.
    if (pTaskRelative < 0.0f) {
        if (prevTransform != NULL && Elastos::Core::Math::Compare(prevTransform->mP, 0.0f) <= 0) {
            transformOut->Reset();
            transformOut->mRect->Set(mTaskRect);
            return transformOut;
        }
    }
    Float scale = CurveProgressToScale(pBounded);
    Int32 height;
    mTaskRect->GetHeight(&height);
    Int32 scaleYOffset = (Int32) (((1.0f - scale) * height) / 2);
    Int32 minZ = mConfig->mTaskViewTranslationZMinPx;
    Int32 maxZ = mConfig->mTaskViewTranslationZMaxPx;
    transformOut->mScale = scale;
    Int32 top;
    mStackVisibleRect->GetTop(&top);
    transformOut->mTranslationY = CurveProgressToScreenY(pBounded) - top - scaleYOffset;
    transformOut->mTranslationZ = Elastos::Core::Math::Max(minZ, minZ + (Int32)(pBounded * (maxZ - minZ)));
    transformOut->mRect->Set(mTaskRect);
    transformOut->mRect->Offset(0, transformOut->mTranslationY);
    assert(0);
    // Utilities::ScaleRectAboutCenter(transformOut->mRect, transformOut->mScale);
    transformOut->mVisible = TRUE;
    transformOut->mP = pTaskRelative;
    return transformOut;
}

/**
 * Returns the untransformed task view size.
 */
AutoPtr<IRect> TaskStackViewLayoutAlgorithm::GetUntransformedTaskViewSize()
{
    AutoPtr<IRect> tvSize;
    CRect::New(mTaskRect, (IRect**)&tvSize);
    tvSize->OffsetTo(0, 0);
    return tvSize;
}

/**
 * Returns the scroll to such task top = 1.0f;
 */
Float TaskStackViewLayoutAlgorithm::GetStackScrollForTask(
    /* [in] */ Task* t)
{
    HashMap<AutoPtr<ITaskKey>, Float>::Iterator find = mTaskProgressMap.Find(t->mKey);
    return find != mTaskProgressMap.End() ? find->mSecond : 0.0f;
}

/** Initializes the curve. */
void TaskStackViewLayoutAlgorithm::InitializeCurve()
{
    if (sXp != NULL && sPx != NULL) return;
    sXp = ArrayOf<Float>::Alloc(PrecisionSteps + 1);
    sPx = ArrayOf<Float>::Alloc(PrecisionSteps + 1);

    // Approximate f(x)
    AutoPtr<ArrayOf<Float> > fx = ArrayOf<Float>::Alloc(PrecisionSteps + 1);
    Float step = 1.0f / PrecisionSteps;
    Float x = 0;
    for (Int32 xStep = 0; xStep <= PrecisionSteps; xStep++) {
        (*fx)[xStep] = LogFunc(x);
        x += step;
    }
    // Calculate the arc length for x:1->0
    Float pLength = 0;
    AutoPtr<ArrayOf<Float> > dx = ArrayOf<Float>::Alloc(PrecisionSteps + 1);
    (*dx)[0] = 0;
    for (Int32 xStep = 1; xStep < PrecisionSteps; xStep++) {
        (*dx)[xStep] = (Float) Elastos::Core::Math::Sqrt(
            Elastos::Core::Math::Pow((*fx)[xStep] - (*fx)[xStep - 1], 2) + Elastos::Core::Math::Pow(step, 2));
        pLength += (*dx)[xStep];
    }
    // Approximate p(x), a function of cumulative progress with x, normalized to 0..1
    Float p = 0;
    (*sPx)[0] = 0.0f;
    (*sPx)[PrecisionSteps] = 1.0f;
    for (Int32 xStep = 1; xStep <= PrecisionSteps; xStep++) {
        p += Elastos::Core::Math::Abs((*dx)[xStep] / pLength);
        (*sPx)[xStep] = p;
    }
    // Given p(x), calculate the inverse function x(p). This assumes that x(p) is also a valid
    // function.
    Int32 xStep = 0;
    p = 0;
    (*sXp)[0] = 0.0f;
    (*sXp)[PrecisionSteps] = 1.0f;
    for (Int32 pStep = 0; pStep < PrecisionSteps; pStep++) {
        // Walk forward in sPx and find the x where sPx <= p && p < sPx+1
        while (xStep < PrecisionSteps) {
            if ((*sPx)[xStep] > p) break;
            xStep++;
        }
        // Now, (*sPx)[xStep-1] <= p < (*sPx)[xStep]
        if (xStep == 0) {
            (*sXp)[pStep] = 0;
        }
        else {
            // Find x such that proportionally, x is correct
            Float fraction = (p - (*sPx)[xStep - 1]) / ((*sPx)[xStep] - (*sPx)[xStep - 1]);
            x = (xStep - 1 + fraction) * step;
            (*sXp)[pStep] = x;
        }
        p += step;
    }
}

/** Reverses and scales out x. */
Float TaskStackViewLayoutAlgorithm::Reverse(
    /* [in] */ Float x)
{
    return (-x * XScale) + 1;
}
/** The log function describing the curve. */
Float TaskStackViewLayoutAlgorithm::LogFunc(
    /* [in] */ Float x)
{
    return 1.0f - (Float) (Elastos::Core::Math::Pow(LogBase, Reverse(x))) / (LogBase);
}
/** The inverse of the log function describing the curve. */
Float TaskStackViewLayoutAlgorithm::InvLogFunc(
    /* [in] */ Float y)
{
    return (Float) (Elastos::Core::Math::Log((1.0f - Reverse(y)) * (LogBase - 1) + 1) / Elastos::Core::Math::Log(LogBase));
}

/** Converts from the progress along the curve to a screen coordinate. */
Int32 TaskStackViewLayoutAlgorithm::CurveProgressToScreenY(
    /* [in] */ Float p)
{
    Int32 top, height;
    mStackVisibleRect->GetTop(&top);
    mStackVisibleRect->GetHeight(&height);
    if (p < 0 || p > 1) return top + (Int32) (p * height);
    Float pIndex = p * PrecisionSteps;
    Int32 pFloorIndex = (Int32) Elastos::Core::Math::Floor(pIndex);
    Int32 pCeilIndex = (Int32) Elastos::Core::Math::Ceil(pIndex);
    Float xFraction = 0;
    if (pFloorIndex < PrecisionSteps && (pCeilIndex != pFloorIndex)) {
        Float pFraction = (pIndex - pFloorIndex) / (pCeilIndex - pFloorIndex);
        xFraction = ((*sXp)[pCeilIndex] - (*sXp)[pFloorIndex]) * pFraction;
    }
    Float x = (*sXp)[pFloorIndex] + xFraction;
    return top + (Int32) (x * height);
}

/** Converts from the progress along the curve to a scale. */
Float TaskStackViewLayoutAlgorithm::CurveProgressToScale(
    /* [in] */ Float p)
{
    if (p < 0) return StackPeekMinScale;
    if (p > 1) return 1.0f;
    Float scaleRange = (1.0f - StackPeekMinScale);
    Float scale = StackPeekMinScale + (p * scaleRange);
    return scale;
}

/** Converts from a screen coordinate to the progress along the curve. */
Float TaskStackViewLayoutAlgorithm::ScreenYToCurveProgress(
    /* [in] */ Int32 screenY)
{
    Int32 top, height;
    mStackVisibleRect->GetTop(&top);
    mStackVisibleRect->GetHeight(&height);
    Float x = (Float) (screenY - top) / height;
    if (x < 0 || x > 1) return x;
    Float xIndex = x * PrecisionSteps;
    Int32 xFloorIndex = (Int32) Elastos::Core::Math::Floor(xIndex);
    Int32 xCeilIndex = (Int32) Elastos::Core::Math::Ceil(xIndex);
    Float pFraction = 0;
    if (xFloorIndex < PrecisionSteps && (xCeilIndex != xFloorIndex)) {
        Float xFraction = (xIndex - xFloorIndex) / (xCeilIndex - xFloorIndex);
        pFraction = ((*sPx)[xCeilIndex] - (*sPx)[xFloorIndex]) * xFraction;
    }
    return (*sPx)[xFloorIndex] + pFraction;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
