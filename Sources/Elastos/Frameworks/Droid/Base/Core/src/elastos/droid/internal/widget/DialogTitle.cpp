
#include "elastos/droid/internal/widget/DialogTitle.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(DialogTitle, TextView, IDialogTitle)

DialogTitle::DialogTitle()
{
}

DialogTitle::~DialogTitle()
{}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context)
{
    return TextView::constructor(context);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::constructor(context, attrs);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return TextView::constructor(context, attrs, defStyleAttr);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return TextView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

void DialogTitle::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    if (layout != NULL) {
        Int32 lineCount;
        layout->GetLineCount(&lineCount);

        if (lineCount > 0) {
            Int32 ellipsisCount;
            layout->GetEllipsisCount(lineCount - 1, &ellipsisCount);

            if (ellipsisCount > 0) {
                SetSingleLine(FALSE);
                SetMaxLines(2);

                AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TextAppearance);
                AutoPtr<ITypedArray> a;
                ASSERT_SUCCEEDED(mContext->ObtainStyledAttributes(
                        NULL, attrIds, R::attr::textAppearanceMedium,
                        R::style::TextAppearance_Medium, (ITypedArray**)&a));

                Int32 textSize;
                a->GetDimensionPixelSize(
                    R::styleable::TextAppearance_textSize, 0, &textSize);

                if (textSize != 0) {
                    // textSize is already expressed in pixels
                    SetTextSize(ITypedValue::COMPLEX_UNIT_PX, textSize);
                }
                a->Recycle();

                TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
            }
        }
    }
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
