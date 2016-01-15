#include "elastos/droid/text/style/TextAppearanceSpan.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/graphics/CTypeface.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Graphics::CTypeface;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(TextAppearanceSpan, MetricAffectingSpan, ITextAppearanceSpan, IParcelableSpan, IParcelable)

TextAppearanceSpan::TextAppearanceSpan()
    : mStyle(0)
    , mTextSize(0)
{}

TextAppearanceSpan::~TextAppearanceSpan()
{}

ECode TextAppearanceSpan::constructor()
{
    return NOERROR;
}

ECode TextAppearanceSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appearance)
{
    return constructor(context, appearance, -1);
}

ECode TextAppearanceSpan::constructor(

    /* [in] */ IContext* context,
    /* [in] */ Int32 appearance,
    /* [in] */ Int32 colorList)
{
    AutoPtr<IColorStateList> textColor;

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::TextAppearance),
            ArraySize(R::styleable::TextAppearance));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(appearance, attrIds, (ITypedArray**)&a);

    a->GetColorStateList(R::styleable::TextAppearance_textColor, (IColorStateList**)&textColor);
    a->GetColorStateList(R::styleable::TextAppearance_textColorLink, (IColorStateList**)&mTextColorLink);
    a->GetDimensionPixelSize(R::styleable::TextAppearance_textSize, -1, &mTextSize);

    a->GetInt32(R::styleable::TextAppearance_textStyle, 0, &mStyle);
    String family;
    a->GetString(R::styleable::TextAppearance_fontFamily, &family);
    if (!family.IsNull()) {
        mTypeface = family;
    }
    else {
        Int32 tf;
        a->GetInt32(R::styleable::TextAppearance_typeface, 0, &tf);

        switch (tf) {
            case 1:
                mTypeface = "sans";
                break;

            case 2:
                mTypeface = "serif";
                break;

            case 3:
                mTypeface = "monospace";
                break;

            default:
                mTypeface = NULL;
                break;
        }
    }

    a->Recycle();

    if (colorList >= 0) {
        a = NULL;
        attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32 *>(R::styleable::Theme),
                ArraySize(R::styleable::Theme));
        context->ObtainStyledAttributes(R::style::Theme, attrIds, (ITypedArray**)&a);

        textColor = NULL;
        a->GetColorStateList(colorList, (IColorStateList**)&textColor);
        a->Recycle();
    }

    mTextColor = textColor;
    return NOERROR;
}

ECode TextAppearanceSpan::constructor(
    /* [in] */ const String& family,
    /* [in] */ Int32 style,
    /* [in] */ Int32 size,
    /* [in] */ IColorStateList* color,
    /* [in] */ IColorStateList* linkColor)
{
    mTypeface = family;
    mStyle = style;
    mTextSize = size;
    mTextColor = color;
    mTextColorLink = linkColor;
    return NOERROR;
}

ECode TextAppearanceSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::TEXT_APPEARANCE_SPAN;
    return NOERROR;
}

ECode TextAppearanceSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mTypeface));
    FAIL_RETURN(source->ReadInt32(&mStyle));
    FAIL_RETURN(source->ReadInt32(&mTextSize));
    Int32 n;
    FAIL_RETURN(source->ReadInt32(&n));
    mTextColor = NULL;
    if (n != 0) {
        CColorStateList::New((IColorStateList**)&mTextColor);
        FAIL_RETURN(IParcelable::Probe(mTextColor)->ReadFromParcel(source));
    }

    FAIL_RETURN(source->ReadInt32(&n));
    mTextColorLink = NULL;
    if (n != 0) {
        CColorStateList::New((IColorStateList**)&mTextColorLink);
        FAIL_RETURN(IParcelable::Probe(mTextColorLink)->ReadFromParcel(source));
    }

    return NOERROR;
}

ECode TextAppearanceSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mTypeface));
    FAIL_RETURN(dest->WriteInt32(mStyle));
    FAIL_RETURN(dest->WriteInt32(mTextSize));
    if (mTextColor != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        FAIL_RETURN(IParcelable::Probe(mTextColor)->WriteToParcel(dest));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(0));
    }
    if (mTextColorLink != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        FAIL_RETURN(IParcelable::Probe(mTextColorLink)->WriteToParcel(dest));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(0));
    }
    return NOERROR;
}

ECode TextAppearanceSpan::GetFamily(
    /* [out] */ String* family)
{
    VALIDATE_NOT_NULL(family)
    *family = mTypeface;
    return NOERROR;
}

ECode TextAppearanceSpan::GetTextColor(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = mTextColor;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode TextAppearanceSpan::GetLinkTextColor(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = mTextColorLink;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode TextAppearanceSpan::GetTextSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mTextSize;
    return NOERROR;
}

ECode TextAppearanceSpan::GetTextStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style)
    *style = mStyle;
    return NOERROR;
}

ECode TextAppearanceSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    FAIL_RETURN(UpdateMeasureState(ds));

    CTextPaint* p = (CTextPaint*)ds;
    if (mTextColor != NULL) {
        AutoPtr< ArrayOf<Int32> > drawableState;
        p->GetDrawableState((ArrayOf<Int32>**)&drawableState);
        Int32 colorForState;
        mTextColor->GetColorForState(drawableState.Get(), 0, &colorForState);
        p->SetColor(colorForState);
    }

    if (mTextColorLink != NULL) {
        AutoPtr< ArrayOf<Int32> > drawableState;
        p->GetDrawableState((ArrayOf<Int32>**)&drawableState);
        Int32 linkColor;
        mTextColorLink->GetColorForState(drawableState.Get(), 0, &linkColor);
        p->SetLinkColor(linkColor);
    }
    return NOERROR;
}

ECode TextAppearanceSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    CTextPaint* p = (CTextPaint*)ds;

    if (!mTypeface.IsNull() || mStyle != 0) {
        AutoPtr<ITypeface> tf;
        p->GetTypeface((ITypeface**)&tf);
        Int32 style = 0;

        if (tf != NULL) {
            tf->GetStyle(&style);
        }

        style |= mStyle;

        if (!mTypeface.IsNull()) {
            CTypeface::Create(mTypeface, style, (ITypeface**)&tf);
        }
        else if (tf == NULL) {
            CTypeface::DefaultFromStyle(style, (ITypeface**)&tf);
        }
        else {
            CTypeface::Create(tf, style, (ITypeface**)&tf);
        }

        Int32 tfStyle;
        tf->GetStyle(&tfStyle);
        Int32 fake = style & ~tfStyle;

        if ((fake & ITypeface::BOLD) != 0) {
            p->SetFakeBoldText(TRUE);
        }

        if ((fake & ITypeface::ITALIC) != 0) {
            p->SetTextSkewX(-0.25f);
        }

        p->SetTypeface(tf);
    }

    if (mTextSize > 0) {
        p->SetTextSize(mTextSize);
    }
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
