
#include "elastos/droid/internal/widget/PreferenceImageView.h"
#include "elastos/droid/view/View.h"
#include "elastos/core/Math.h"

using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//                         PreferenceImageView
//=====================================================================
CAR_INTERFACE_IMPL(PreferenceImageView, ImageView, IPreferenceImageView)

PreferenceImageView::PreferenceImageView()
{
}

ECode PreferenceImageView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode PreferenceImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, 0);

    return constructor(context, attrs, 0);
}

ECode PreferenceImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PreferenceImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(context, attrs, defStyleAttr, defStyleRes);

    return ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode PreferenceImageView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // final int widthMode = MeasureSpec.getMode(widthMeasureSpec);
    // if (widthMode == MeasureSpec.AT_MOST || widthMode == MeasureSpec.UNSPECIFIED) {
    //     final int widthSize = MeasureSpec.getSize(widthMeasureSpec);
    //     final int maxWidth = getMaxWidth();
    //     if (maxWidth != Integer.MAX_VALUE
    //             && (maxWidth < widthSize || widthMode == MeasureSpec.UNSPECIFIED)) {
    //         widthMeasureSpec = MeasureSpec.makeMeasureSpec(maxWidth, MeasureSpec.AT_MOST);
    //     }
    // }
    //
    // final int heightMode = MeasureSpec.getMode(heightMeasureSpec);
    // if (heightMode == MeasureSpec.AT_MOST || heightMode == MeasureSpec.UNSPECIFIED) {
    //     final int heightSize = MeasureSpec.getSize(heightMeasureSpec);
    //     final int maxHeight = getMaxHeight();
    //     if (maxHeight != Integer.MAX_VALUE
    //             && (maxHeight < heightSize || heightMode == MeasureSpec.UNSPECIFIED)) {
    //         heightMeasureSpec = MeasureSpec.makeMeasureSpec(maxHeight, MeasureSpec.AT_MOST);
    //     }
    // }
    //
    // super.onMeasure(widthMeasureSpec, heightMeasureSpec);


    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    if (widthMode == View::MeasureSpec::AT_MOST || widthMode == View::MeasureSpec::UNSPECIFIED) {
        Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
        Int32 maxWidth = 0;
        GetMaxWidth(&maxWidth);
        if (maxWidth != Elastos::Core::Math::INT32_MAX_VALUE
                && (maxWidth < widthSize || widthMode == View::MeasureSpec::UNSPECIFIED)) {
            widthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(maxWidth, View::MeasureSpec::AT_MOST);
        }
    }

    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    if (heightMode == View::MeasureSpec::AT_MOST || heightMode == View::MeasureSpec::UNSPECIFIED) {
        Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);
        Int32 maxHeight = 0;
        GetMaxHeight(&maxHeight);
        if (maxHeight != Elastos::Core::Math::INT32_MAX_VALUE
                && (maxHeight < heightSize || heightMode == View::MeasureSpec::UNSPECIFIED)) {
            heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(maxHeight, View::MeasureSpec::AT_MOST);
        }
    }

    ImageView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


