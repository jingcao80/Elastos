#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/Space.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(Space, Elastos::Droid::View::View, ISpace)

Space::Space()
{}

ECode Space::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode Space::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode Space::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode Space::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyleAttr, defStyleRes))
    Int32 visibility;
    if ((GetVisibility(&visibility), visibility) == VISIBLE) {
        SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}


/**
 * Draw nothing.
 *
 * @param canvas an unused parameter.
 */
//@Override
ECode Space::Draw(
    /* [in] */ ICanvas* canvas)
{
    return NOERROR;
}

void Space::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(
        GetDefaultSize2(GetSuggestedMinimumWidth(), widthMeasureSpec),
        GetDefaultSize2(GetSuggestedMinimumHeight(), heightMeasureSpec));
}

/**
 * Compare to: {@link View#getDefaultSize(Int32, Int32)}
 * If mode is AT_MOST, return the child size instead of the parent size
 * (unless it is too big).
 */
Int32 Space::GetDefaultSize2(
    /* [in] */ Int32 size,
    /* [in] */ Int32 measureSpec)
{
    using Elastos::Core::Math;
    Int32 result = size;
    Int32 specMode = MeasureSpec::GetMode(measureSpec);
    Int32 specSize = MeasureSpec::GetSize(measureSpec);

    switch (specMode) {
        case MeasureSpec::UNSPECIFIED:
            result = size;
            break;
        case MeasureSpec::AT_MOST:
            result = Math::Min(size, specSize);
            break;
        case MeasureSpec::EXACTLY:
            result = specSize;
            break;
    }
    return result;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
