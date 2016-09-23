
#include "elastos/droid/phone/common/dialpad/DialpadTextView.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

DialpadTextView::DialpadTextView()
{
    CRect::New((IRect**)&mTextBounds);
}

ECode DialpadTextView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::constructor(ctx, attrs);
}

ECode DialpadTextView::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<ITextPaint> paint;
    GetPaint((ITextPaint**)&paint);

    // Without this, the draw does not respect the style's specified text color.
    Int32 color;
    GetCurrentTextColor(&color);
    IPaint::Probe(paint)->SetColor(color);

    // The text bounds values are relative and can be negative,, so rather than specifying a
    // standard origin such as 0, 0, we need to use negative of the left/top bounds.
    // For example, the bounds may be: Left: 11, Right: 37, Top: -77, Bottom: 0
    Int32 left, top;
    mTextBounds->GetLeft(&left);
    mTextBounds->GetTop(&top);
    canvas->DrawText(mTextStr, -left, -top, IPaint::Probe(paint));
    return NOERROR;
}

ECode DialpadTextView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    text->ToString(&mTextStr);
    AutoPtr<ITextPaint> paint;
    GetPaint((ITextPaint**)&paint);
    IPaint::Probe(paint)->GetTextBounds(mTextStr, 0, mTextStr.GetLength(), mTextBounds);

    Int32 width, height;
    mTextBounds->GetWidth(&width);
    mTextBounds->GetHeight(&height);
    width = ResolveSize(width, widthMeasureSpec);
    height = ResolveSize(height, heightMeasureSpec);
    SetMeasuredDimension(width, height);
    return NOERROR;
}

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
