
#include "elastos/droid/internal/widget/FaceUnlockView.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String FaceUnlockView::FaceUnlockView_TAG("FaceUnlockView");
CAR_INTERFACE_IMPL(FaceUnlockView, RelativeLayout, IFaceUnlockView)

FaceUnlockView::FaceUnlockView()
{
}

ECode FaceUnlockView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode FaceUnlockView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

Int32 FaceUnlockView::ResolveMeasured(
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

void FaceUnlockView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 minimumWidth = GetSuggestedMinimumWidth();
    Int32 minimumHeight = GetSuggestedMinimumHeight();
    Int32 viewWidth = ResolveMeasured(widthMeasureSpec, minimumWidth);
    Int32 viewHeight = ResolveMeasured(heightMeasureSpec, minimumHeight);

    Int32 chosenSize = Elastos::Core::Math::Min(viewWidth, viewHeight);
    Int32 newWidthMeasureSpec =
        MeasureSpec::MakeMeasureSpec(chosenSize, MeasureSpec::AT_MOST);
    Int32 newHeightMeasureSpec =
        MeasureSpec::MakeMeasureSpec(chosenSize, MeasureSpec::AT_MOST);

    RelativeLayout::OnMeasure(newWidthMeasureSpec, newHeightMeasureSpec);
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

