#include "elastos/droid/text/style/DrawableMarginSpan.h"
#include "elastos/droid/graphics/CPaintFontMetricsInt.h"

using Elastos::Droid::Graphics::CPaintFontMetricsInt;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_5(DrawableMarginSpan, Object, IDrawableMarginSpan, ILineHeightSpan, IParagraphStyle, IWrapTogetherSpan, ILeadingMarginSpan)

DrawableMarginSpan::DrawableMarginSpan()
    : mPad(0)
{}

DrawableMarginSpan::~DrawableMarginSpan()
{}

ECode DrawableMarginSpan::constructor(
    /* [in] */ IDrawable* b)
{
    mDrawable = b;
    return NOERROR;
}

ECode DrawableMarginSpan::constructor(
    /* [in] */ IDrawable* b,
    /* [in] */ Int32 pad)
{
    mDrawable = b;
    mPad = pad;
    return NOERROR;
}

ECode DrawableMarginSpan::GetLeadingMargin(
    /* [in] */ Boolean first,
    /* [out] */ Int32* margin)
{
    VALIDATE_NOT_NULL(margin)
    Int32 intrinsicWidth;
    mDrawable->GetIntrinsicWidth(&intrinsicWidth);
    *margin = intrinsicWidth + mPad;
    return NOERROR;
}

ECode DrawableMarginSpan::DrawLeadingMargin(
    /* [in] */ ICanvas* c,
    /* [in] */ IPaint* p,
    /* [in] */ Int32 x,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 top,
    /* [in] */ Int32 baseline,
    /* [in] */ Int32 bottom,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean first,
    /* [in] */ ILayout* layout)
{
    ISpanned* spanned = ISpanned::Probe(text);
    Int32 st;
    spanned->GetSpanStart(TO_IINTERFACE(this), &st);
    Int32 ix = (Int32)x;
    Int32 lineForOffset;
    Int32 itop;
    layout->GetLineForOffset(st, &lineForOffset);
    layout->GetLineTop(lineForOffset, &itop);

    Int32 dw;
    mDrawable->GetIntrinsicWidth(&dw);
    Int32 dh;
    mDrawable->GetIntrinsicHeight(&dh);

    // XXX What to do about Paint?
    mDrawable->SetBounds(ix, itop, ix+dw, itop+dh);
    mDrawable->Draw(c);
    return NOERROR;
}

ECode DrawableMarginSpan::ChooseHeight(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 istartv,
    /* [in] */ Int32 v,
    /* [in] */ IPaintFontMetricsInt* fm)
{
    Int32 se;
    ISpanned* spanned = ISpanned::Probe(text);
    spanned->GetSpanEnd(TO_IINTERFACE(this), &se);
    if (end == se) {
        Int32 ht;
        mDrawable->GetIntrinsicHeight(&ht);

        CPaintFontMetricsInt* pfm = (CPaintFontMetricsInt*)fm;
        Int32 descent, ascent;
        pfm->GetDescent(&descent);
        pfm->GetAscent(&ascent);
        Int32 need = ht - (v + descent - ascent - istartv);
        if (need > 0) {
            pfm->SetDescent(descent + need);
        }

        Int32 bottom, top;
        pfm->GetBottom(&bottom);
        pfm->GetTop(&top);
        need = ht - (v + bottom - top - istartv);
        if (need > 0) {
            pfm->SetBottom(bottom + need);
        }
    }
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos