
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/text/style/IconMarginSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_5(IconMarginSpan, Object, IIconMarginSpan, ILeadingMarginSpan, IParagraphStyle, ILineHeightSpan, IWrapTogetherSpan)

IconMarginSpan::IconMarginSpan()
    : mPad(0)
{}

IconMarginSpan::~IconMarginSpan()
{}

ECode IconMarginSpan::constructor(
    /* [in] */ IBitmap* b)
{
    mBitmap = b;
    return NOERROR;
}

ECode IconMarginSpan::constructor(
    /* [in] */ IBitmap* b,
    /* [in] */ Int32 pad)
{
    mBitmap = b;
    mPad = pad;
    return NOERROR;
}

ECode IconMarginSpan::GetLeadingMargin(
    /* [in] */ Boolean first,
    /* [out] */ Int32* margin)
{
    VALIDATE_NOT_NULL(margin)
    Int32 width;
    mBitmap->GetWidth(&width);
    *margin = width + mPad;
    return NOERROR;
}

ECode IconMarginSpan::DrawLeadingMargin(
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
    Int32 itop, lLineForOffset;
    layout->GetLineForOffset(st, &lLineForOffset);
    layout->GetLineTop(lLineForOffset, &itop);

    if (dir < 0){
        Int32 bWidth;
        mBitmap->GetWidth(&bWidth);
        x -= bWidth;
    }

    c->DrawBitmap(mBitmap, x, itop, p);
    return NOERROR;
}

ECode IconMarginSpan::ChooseHeight(
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
        mBitmap->GetHeight(&ht);

        Int32 descent, ascent;
        fm->GetDescent(&descent);
        fm->GetAscent(&ascent);
        Int32 need = ht - (v + descent - ascent - istartv);
        if (need > 0) {
            fm->SetDescent(descent + need);
        }

        Int32 bottom, top;
        fm->GetBottom(&bottom);
        fm->GetTop(&top);
        need = ht - (v + bottom - top - istartv);
        if (need > 0) {
            fm->SetBottom(bottom + need);
        }
    }
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
