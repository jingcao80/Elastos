#ifndef __ELASTOS_DROID_TEXT_STYLE_TextAppearanceSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_TextAppearanceSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Sets the text color, size, style, and typeface to match a TextAppearance
 * resource.
 */
class TextAppearanceSpan
    : public MetricAffectingSpan
    , public ITextAppearanceSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    TextAppearanceSpan();

    virtual ~TextAppearanceSpan();

    CARAPI constructor();

    /**
     * Uses the specified TextAppearance resource to determine the
     * text appearance.  The <code>appearance</code> should be, for example,
     * <code>android.R.style.TextAppearance_Small</code>.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32  appearance);

    /**
     * Uses the specified TextAppearance resource to determine the
     * text appearance, and the specified text color resource
     * to determine the color.  The <code>appearance</code> should be,
     * for example, <code>android.R.style.TextAppearance_Small</code>,
     * and the <code>colorList</code> should be, for example,
     * <code>android.R.styleable.Theme_textColorPrimary</code>.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appearance,
        /* [in] */ Int32 colorList);

    /**
     * Makes text be drawn with the specified typeface, size, style,
     * and colors.
     */
    CARAPI constructor(
        /* [in] */ const String& family,
        /* [in] */ Int32 style,
        /* [in] */ Int32 size,
        /* [in] */ IColorStateList* color,
        /* [in] */ IColorStateList* linkColor);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the typeface family specified by this span, or <code>null</code>
     * if it does not specify one.
     */
    CARAPI GetFamily(
        /* [out] */ String* family);

    /**
     * Returns the text color specified by this span, or <code>null</code>
     * if it does not specify one.
     */
    CARAPI GetTextColor(
        /* [out] */ IColorStateList** color);

    /**
     * Returns the link color specified by this span, or <code>null</code>
     * if it does not specify one.
     */
    CARAPI GetLinkTextColor(
        /* [out] */ IColorStateList** color);

    /**
     * Returns the text size specified by this span, or <code>-1</code>
     * if it does not specify one.
     */
    CARAPI GetTextSize(
        /* [out] */ Int32* size);

    /**
     * Returns the text style specified by this span, or <code>0</code>
     * if it does not specify one.
     */
    CARAPI GetTextStyle(
        /* [out] */ Int32* style);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* ds);

private:
    /*const*/ String mTypeface;
    /*const*/ Int32 mStyle;
    /*const*/ Int32 mTextSize;
    /*const*/ AutoPtr<IColorStateList> mTextColor;
    /*const*/ AutoPtr<IColorStateList> mTextColorLink;

};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_TextAppearanceSpan_H__
