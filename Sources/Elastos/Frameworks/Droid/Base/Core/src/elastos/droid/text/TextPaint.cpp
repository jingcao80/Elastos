
#include "Elastos.Droid.Text.h"
#include "elastos/droid/text/TextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {


TextPaint::TextPaint()
    : mBgColor(0)
    , mBaselineShift(0)
    , mLinkColor(0)
    , mDensity(1.0f)
    , mUnderlineColor(0)
    , mUnderlineThickness(0)
{
}

TextPaint::~TextPaint()
{
}

// Special value 0 means no background paint
ECode TextPaint::SetBgColor(
    /* [in] */ Int32 bgColor)
{
    mBgColor = bgColor;
    return NOERROR;
}

ECode TextPaint::GetBgColor(
    /* [out] */ Int32* bgColor)
{
    VALIDATE_NOT_NULL(bgColor);
    *bgColor = mBgColor;
    return NOERROR;
}

ECode TextPaint::SetBaselineShift(
    /* [in] */ Int32 baselineShift)
{
    mBaselineShift = baselineShift;
    return NOERROR;
}

ECode TextPaint::GetBaselineShift(
    /* [out] */ Int32* baselineShift)
{
    VALIDATE_NOT_NULL(baselineShift);
    *baselineShift = mBaselineShift;
    return NOERROR;
}

ECode TextPaint::SetLinkColor(
    /* [in] */ Int32 linkColor)
{
    mLinkColor = linkColor;
    return NOERROR;
}

ECode TextPaint::GetLinkColor(
    /* [out] */ Int32* linkColor)
{
    VALIDATE_NOT_NULL(linkColor);
    *linkColor = mLinkColor;
    return NOERROR;
}

ECode TextPaint::SetDrawableState(
    /* [in] */ ArrayOf<Int32>* drawableState)
{
    mDrawableState = drawableState;
    return NOERROR;
}

ECode TextPaint::GetDrawableState(
    /* [out] */ ArrayOf<Int32>** state)
{
    VALIDATE_NOT_NULL(state);
    *state = mDrawableState;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode TextPaint::SetDensity(
    /* [in] */ Float density)
{
    mDensity = density;
    return NOERROR;
}

ECode TextPaint::GetDensity(
    /* [out] */ Float* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mDensity;
    return NOERROR;
}

/**
 * Special value 0 means no custom underline
 * @hide
 */
ECode TextPaint::SetUnderlineColor(
    /* [in] */ Int32 underlineColor)
{
    mUnderlineColor = underlineColor;
    return NOERROR;
}

ECode TextPaint::GetUnderlineColor(
    /* [out] */ Int32* underline)
{
    VALIDATE_NOT_NULL(underline);
    *underline = mUnderlineColor;
    return NOERROR;
}


/**
 * Defined as a multiplier of the default underline thickness. Use 1.0f for default thickness.
 * @hide
 */
ECode TextPaint::SetUnderlineThickness(
    /* [in] */ Float underlineThickness)
{
    mUnderlineThickness = underlineThickness;
    return NOERROR;
}

ECode TextPaint::GetUnderlineThickness(
    /* [out] */ Float* underlineThickness)
{
    VALIDATE_NOT_NULL(underlineThickness);
    *underlineThickness = mUnderlineThickness;
    return NOERROR;
}

/**
 * Copy the fields from tp into this TextPaint, including the
 * fields inherited from Paint.
 */
ECode TextPaint::Set(
    /* [in] */ ITextPaint* tp)
{
    //assert(tp != NULL);
    VALIDATE_NOT_NULL(tp);

    IPaint* p = IPaint::Probe(tp);
    Paint::Set(p);

    tp->GetBgColor(&mBgColor);
    tp->GetBaselineShift(&mBaselineShift);
    tp->GetLinkColor(&mLinkColor);
    mDrawableState = NULL;
    tp->GetDrawableState((ArrayOf<Int32>**)&mDrawableState);
    tp->GetDensity(&mDensity);
    tp->GetUnderlineColor(&mUnderlineColor);
    tp->GetUnderlineThickness(&mUnderlineThickness);

    return NOERROR;
}

/**
 * Defines a custom underline for this Paint.
 * @param color underline solid color
 * @param thickness underline thickness
 * @hide
 */
ECode TextPaint::SetUnderlineText(
    /* [in] */ Int32 color,
    /* [in] */ Float thickness)
{
    mUnderlineColor = color;
    mUnderlineThickness = thickness;
    return NOERROR;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos
