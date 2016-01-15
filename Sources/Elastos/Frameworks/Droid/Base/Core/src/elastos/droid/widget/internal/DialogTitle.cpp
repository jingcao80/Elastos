
#include "elastos/droid/widget/internal/DialogTitle.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Utility::ITypedValue;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

DialogTitle::DialogTitle()
{
}

DialogTitle::DialogTitle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : TextView(context, attrs, defStyle)
{
}

DialogTitle::DialogTitle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : TextView(context, attrs)
{
}

DialogTitle::DialogTitle(
    /* [in] */ IContext* context)
    : TextView(context)
{
}

ECode DialogTitle::Init(
    /* [in] */ IContext* context)
{
    return TextView::Init(context);
}

ECode DialogTitle::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::Init(context, attrs);
}

ECode DialogTitle::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TextView::Init(context, attrs, defStyle);
}

void DialogTitle::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    AutoPtr<ILayout> layout = GetLayout();
    if (layout != NULL) {
        Int32 lineCount;
        layout->GetLineCount(&lineCount);

        if (lineCount > 0) {
            Int32 ellipsisCount;
            layout->GetEllipsisCount(lineCount - 1, &ellipsisCount);

            if (ellipsisCount > 0) {
                SetSingleLine(FALSE);
                SetMaxLines(2);

                AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                    const_cast<Int32 *>(R::styleable::TextAppearance),
                    ArraySize(R::styleable::TextAppearance));
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


}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
