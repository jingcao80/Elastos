#ifndef __ELASTOS_DROID_TEXT_STATICLAYOUT_H__
#define __ELASTOS_DROID_TEXT_STATICLAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/text/MeasuredText.h"

using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Text::Style::ILineHeightSpan;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * StaticLayout is a Layout for text that will not be edited after it
 * is laid out.  Use {@link DynamicLayout} for text that may change.
 * <p>This is used by widgets to control text layout. You should not need
 * to use this class directly unless you are implementing your own widget
 * or custom display object, or would be tempted to call
 * {@link android.graphics.Canvas#drawText(java.lang.CharSequence, int, int,
 * float, float, android.graphics.Paint)
 * Canvas.drawText()} directly.</p>
 */
class StaticLayout
    : public Layout
    , public IStaticLayout
{
public:
    CAR_INTERFACE_DECL()

    StaticLayout();

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 bufstart,
        /* [in] */ Int32 bufend,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 bufstart,
        /* [in] */ Int32 bufend,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 bufstart,
        /* [in] */ Int32 bufend,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Int32 ellipsizedWidth);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 bufstart,
        /* [in] */ Int32 bufend,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Int32 ellipsizedWidth,
        /* [in] */ Int32 maxLines) ;

    /* package */
    CARAPI constructor(
        /* [in] */ ICharSequence* text);

public:
    /* package */
    CARAPI Generate(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 bufStart,
        /* [in] */ Int32 bufEnd,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerWidth,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad,
        /* [in] */ Boolean trackpad,
        /* [in] */ Float ellipsizedWidth,
        /* [in] */ TextUtilsTruncateAt ellipsize);

    // Override the base class so we can directly access our members,
    // rather than relying on member functions.
    // The logic mirrors that of Layout.getLineForVertical
    // FIXME: It may be faster to do a linear search for layouts without many lines.
    //@Override
    CARAPI GetLineForVertical(
        /* [in] */ Int32 vertical,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineCount(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineTop(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineDescent(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineStart(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetParagraphDirection(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineContainsTab(
        /* [in] */ Int32 line,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetLineDirections(
        /* [in] */ Int32 line,
        /* [out] */ ILayoutDirections** result);

    //@Override
    CARAPI GetTopPadding(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetBottomPadding(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsisCount(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsisStart(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsizedWidth(
        /* [out] */ Int32* result);

    CARAPI Prepare();

    CARAPI Finish();

private:

    CARAPI_(Int32) Out(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 above,
        /* [in] */ Int32 below,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 v,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ ArrayOf<IInterface*>* chooseHt,//ILineHeightSpan
        /* [in] */ ArrayOf<Int32>* chooseHtv,
        /* [in] */ IPaintFontMetricsInt* fm,
        /* [in] */ Boolean hasTabOrEmoji,
        /* [in] */ Boolean needMultiply,
        /* [in] */ ArrayOf<Byte>* chdirs,
        /* [in] */ Int32 dir,
        /* [in] */ Boolean easy,
        /* [in] */ Int32 bufEnd,
        /* [in] */ Boolean includePad,
        /* [in] */ Boolean trackPad,
        /* [in] */ ArrayOf<Char32>* chs,
        /* [in] */ ArrayOf<Float>* widths,
        /* [in] */ Int32 widthStart,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Float ellipsisWidth,
        /* [in] */ Float textWidth,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Boolean moreChars);

    CARAPI_(void) CalculateEllipsis(
        /* [in] */ Int32 lineStart,
        /* [in] */ Int32 lineEnd,
        /* [in] */ ArrayOf<Float>* widths,
        /* [in] */ Int32 widthStart,
        /* [in] */ Float avail,
        /* [in] */ TextUtilsTruncateAt where,
        /* [in] */ Int32 line,
        /* [in] */ Float textWidth,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Boolean forceEllipsis);

    // returns an array with terminal sentinel value -1 to indicate end
    // this is so that arrays can be recycled instead of allocating new arrays
    // every time
    static /*native*/ AutoPtr<ArrayOf<Int32> > nLineBreakOpportunities(
        /* [in] */ const String& locale,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 length,
        /* [in] */ ArrayOf<Int32>* recycle);

public:
    static const String TAG;// = "StaticLayout";

private:
    Int32 mLineCount;
    Int32 mTopPadding;
    Int32 mBottomPadding;
    Int32 mColumns;
    Int32 mEllipsizedWidth;

    static const Int32 COLUMNS_NORMAL = 3;
    static const Int32 COLUMNS_ELLIPSIZE = 5;
    static const Int32 START = 0;
    static const Int32 DIR = START;
    static const Int32 TAB = START;
    static const Int32 TOP = 1;
    static const Int32 DESCENT = 2;
    static const Int32 ELLIPSIS_START = 3;
    static const Int32 ELLIPSIS_COUNT = 4;

    AutoPtr< ArrayOf<Int32> > mLines;
    AutoPtr< ArrayOf<ILayoutDirections*> > mLineDirections;
    Int32 mMaximumVisibleLineCount;

    static const Int32 START_MASK = 0x1FFFFFFF;
    static const Int32 DIR_SHIFT  = 30;
    static const Int32 TAB_MASK   = 0x20000000;

    static const Int32 TAB_INCREMENT = 20; // same as Layout, but that's private

    static const Char32 CHAR_NEW_LINE = '\n';
    static const Char32 CHAR_TAB = '\t';
    static const Char32 CHAR_SPACE = ' ';
    static const Char32 CHAR_ZWSP = 0x200B;

    static const Double EXTRA_ROUNDING = 0.5;

    static const Int32 CHAR_FIRST_HIGH_SURROGATE = 0xD800;
    static const Int32 CHAR_LAST_LOW_SURROGATE = 0xDFFF;

    /*
     * This is reused across calls to generate()
     */
    AutoPtr<MeasuredText> mMeasured;
    AutoPtr<IPaintFontMetricsInt> mFontMetricsInt;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_STATICLAYOUT_H__
