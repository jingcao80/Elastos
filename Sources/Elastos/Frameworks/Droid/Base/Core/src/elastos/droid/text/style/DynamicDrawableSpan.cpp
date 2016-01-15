#include "elastos/droid/text/style/DynamicDrawableSpan.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

const String DynamicDrawableSpan::TAG("DynamicDrawableSpan");

CAR_INTERFACE_IMPL(DynamicDrawableSpan, ReplacementSpan, IDynamicDrawableSpan)

DynamicDrawableSpan::DynamicDrawableSpan()
    : mVerticalAlignment(IDynamicDrawableSpan::ALIGN_BOTTOM)
{
}

DynamicDrawableSpan::~DynamicDrawableSpan()
{}

ECode DynamicDrawableSpan::constructor()
{
    return NOERROR;
}

ECode DynamicDrawableSpan::constructor(
    /* [in] */ Int32 verticalAlignment)
{
    mVerticalAlignment = verticalAlignment;
    return NOERROR;
}

ECode DynamicDrawableSpan::GetVerticalAlignment(
    /* [out] */ Int32* align)
{
    VALIDATE_NOT_NULL(align)
    *align = mVerticalAlignment;
    return NOERROR;
}

ECode DynamicDrawableSpan::GetSize(
    /* [in] */ IPaint* paint,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IPaintFontMetricsInt* fm,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    AutoPtr<IDrawable> d = GetCachedDrawable();
    AutoPtr<IRect> rect;
    d->GetBounds((IRect**)&rect );

    if (fm != NULL) {
        Int32 bottom;
        fm->SetAscent(-(rect->GetBottom(&bottom), bottom));
        fm->SetDescent(0);

        Int32 ascent;
        fm->SetTop((fm->GetAscent(&ascent), ascent));
        fm->SetBottom(0);
    }
    Int32 right;
    *size = (rect->GetRight(&right), right);
    return NOERROR;
}

ECode DynamicDrawableSpan::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom,
    /* [in] */ IPaint* paint)
{
    AutoPtr<IDrawable> b = GetCachedDrawable();
    Int32 saveRet;
    canvas->Save(&saveRet);

    AutoPtr<IRect> rect;
    b->GetBounds( (IRect**)&rect );
    Int32 rbottom;
    rect->GetBottom(&rbottom);
    Int32 transY = bottom - rbottom;
    if (mVerticalAlignment == IDynamicDrawableSpan::ALIGN_BASELINE) {
        AutoPtr<IPaintFontMetricsInt> fm;
        paint->GetFontMetricsInt((IPaintFontMetricsInt**)&fm);
        Int32 descent;
        fm->GetDescent(&descent);
        transY -= descent;
    }

    canvas->Translate(x, transY);
    b->Draw(canvas);
    canvas->Restore();
    return NOERROR;
}

AutoPtr<IDrawable> DynamicDrawableSpan::GetCachedDrawable()
{
    AutoPtr<IDrawable> wr = mDrawableRef;
    AutoPtr<IDrawable> d;

    if (wr != NULL) {
        d = wr.Get();
    }

    if (d == NULL) {
        GetDrawable((IDrawable**)&d);
        mDrawableRef = d;
    }

    return d;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos