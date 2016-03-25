#ifndef __ELASTOS_DROID_TEXT_SPANNABLESTRINGBUILDER_H__
#define __ELASTOS_DROID_TEXT_SPANNABLESTRINGBUILDER_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Core::ICharSequence;
using Elastos::Core::IAppendable;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * This is the class for text whose content and markup can both be changed.
 */
class ECO_PUBLIC SpannableStringBuilder
    : public Object
    , public ISpannableStringBuilder
    , public ICharSequence
    , public IGetChars
    , public ISpannable
    , public ISpanned
    , public IEditable
    , public IAppendable
    , public IGraphicsOperations
{
public:
    CAR_INTERFACE_DECL()

    SpannableStringBuilder();

    virtual ~SpannableStringBuilder();

    /**
     * Create a new SpannableStringBuilder with empty contents
     */
    CARAPI constructor();

    /**
     * Create a new SpannableStringBuilder containing a copy of the
     * specified text, including its spans if any.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text);

    /**
     * Create a new SpannableStringBuilder containing a copy of the
     * specified slice of the specified text, including its spans if any.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static AutoPtr<ISpannableStringBuilder> ValueOf(
        /* [in] */ ICharSequence* source);

    /**
     * Return the char at the specified offset within the buffer.
     */
    CARAPI GetCharAt(
        /* [in] */ Int32 where,
        /* [out] */ Char32* c);

    /**
     * Return the number of chars in the buffer.
     */
    CARAPI GetLength(
        /* [out] */ Int32* length);

    // Documentation from interface
    CARAPI Insert(
        /* [in] */ Int32 where,
        /* [in] */ ICharSequence* tb,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    // Documentation from interface
    CARAPI Insert(
        /* [in] */ Int32 where,
        /* [in] */ ICharSequence* tb);

    // Documentation from interface
    CARAPI Delete(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    // Documentation from interface
    CARAPI Clear();

    // Documentation from interface
    CARAPI ClearSpans();

    CARAPI Append(
        /* [in] */ ICharSequence* text,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 flags);

    // Documentation from interface
    CARAPI Append(
        /* [in] */ ICharSequence* text);

    // Documentation from interface
    CARAPI Append(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    // Documentation from interface
    CARAPI AppendChar(
        /* [in] */ Char32 text);

    // Documentation from interface
    CARAPI Replace(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* tb);

    // Documentation from interface
    CARAPI Replace(
        /* [in] */ const Int32 start,
        /* [in] */ const Int32 end,
        /* [in] */ ICharSequence* tb,
        /* [in] */ Int32 tbstart,
        /* [in] */ Int32 tbend);

    /**
     * Mark the specified range of text with the specified object.
     * The flags determine how the span will behave when text is
     * inserted at the start or end of the span's range.
     */
    CARAPI SetSpan(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    /**
     * Remove the specified markup object from the buffer.
     */
    CARAPI RemoveSpan(
        /* [in] */ IInterface* what);

    /**
     * Return the buffer offset of the beginning of the specified
     * markup object, or -1 if it is not attached to this buffer.
     */
    CARAPI GetSpanStart(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* start);

    /**
     * Return the buffer offset of the end of the specified
     * markup object, or -1 if it is not attached to this buffer.
     */
    CARAPI GetSpanEnd(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* end);

    /**
     * Return the flags of the end of the specified
     * markup object, or 0 if it is not attached to this buffer.
     */
    CARAPI GetSpanFlags(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* flags);

    /**
     * Return an array of the spans of the specified type that overlap
     * the specified range of the buffer.  The kind may be Object.class to get
     * a list of all the spans regardless of type.
     */
    //@SuppressWarnings("unchecked")
    CARAPI GetSpans(
        /* [in] */ Int32 queryStart,
        /* [in] */ Int32 queryEnd,
        /* [in] */ const InterfaceID& kind,
        /* [out, callee] */ ArrayOf<IInterface*>** spans);

    /**
     * Return the next offset after <code>start</code> but less than or
     * equal to <code>limit</code> where a span of the specified type
     * begins or ends.
     */
    CARAPI NextSpanTransition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ const InterfaceID& kind,
        /* [out] */ Int32* result);

    /**
     * Return a new CharSequence containing a copy of the specified
     * range of this buffer, including the overlapping spans.
     */
    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** cs);

    /**
     * Copy the specified range of chars from this buffer into the
     * specified array, beginning at the specified offset.
     */
    CARAPI GetChars(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 destoff);

    /**
     * Return a String containing a copy of the chars in this buffer.
     */
    //@Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Return a String containing a copy of the chars in this buffer, limited to the
     * [start, end[ range.
     * @hide
     */
    CARAPI Substring(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ String* subStr);

    /**
     * Don't call this yourself -- exists for Canvas to use internally.
     * {@hide}
     */
    CARAPI DrawText(
        /* [in] */ ICanvas* c,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* p);

    /**
     * Don't call this yourself -- exists for Canvas to use internally.
     * {@hide}
     */
    CARAPI DrawTextRun(
        /* [in] */ ICanvas* c,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ IPaint* p);

    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    CARAPI MeasureText(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IPaint* p,
        /* [out] */ Float* width);

    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    CARAPI GetTextWidths(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<Float>* widths,
        /* [in] */ IPaint* p,
        /* [out] */ Int32* count);

    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    CARAPI GetTextRunAdvances(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Boolean isRtl,
        /* [in] */ ArrayOf<Float>* advances,
        /* [in] */ Int32 advancesPos,
        /* [in] */ IPaint* p,
        /* [out] */ Float* result);

    /**
     * Returns the next cursor position in the run.  This avoids placing the cursor between
     * surrogates, between characters that form conjuncts, between base characters and combining
     * marks, or within a reordering cluster.
     *
     * <p>The context is the shaping context for cursor movement, generally the bounds of the metric
     * span enclosing the cursor in the direction of movement.
     * <code>contextStart</code>, <code>contextEnd</code> and <code>offset</code> are relative to
     * the start of the string.</p>
     *
     * <p>If cursorOpt is CURSOR_AT and the offset is not a valid cursor position,
     * this returns -1.  Otherwise this will never return a value before contextStart or after
     * contextEnd.</p>
     *
     * @param contextStart the start index of the context
     * @param contextEnd the (non-inclusive) end index of the context
     * @param dir either DIRECTION_RTL or DIRECTION_LTR
     * @param offset the cursor position to move from
     * @param cursorOpt how to move the cursor, one of CURSOR_AFTER,
     * CURSOR_AT_OR_AFTER, CURSOR_BEFORE,
     * CURSOR_AT_OR_BEFORE, or CURSOR_AT
     * @param p the Paint object that is requesting this information
     * @return the offset of the next position, or -1
     * @deprecated This is an internal method, refrain from using it in your code
     */
    //@Deprecated
    CARAPI GetTextRunCursor(
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Int32 dir,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 cursorOpt,
        /* [in] */ IPaint* p,
        /* [out] */ Int32* cursor);

    // Documentation from interface
    CARAPI SetFilters(
        /* [in] */ ArrayOf<IInputFilter*>* filters);

    // Documentation from interface
    CARAPI GetFilters(
        /* [out, callee] */ ArrayOf<IInputFilter*>** filters);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

protected:

    CARAPI_(Boolean) IsSpanEquals(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs);

private:
    ECO_LOCAL CARAPI ResizeFor(
        /* [in] */ Int32 size);

    ECO_LOCAL CARAPI_(void) MoveGapTo(
        /* [in] */ Int32 where);

    ECO_LOCAL CARAPI Change(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 csStart,
        /* [in] */ Int32 csEnd);

    ECO_LOCAL CARAPI_(Int32) UpdatedIntervalBound(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 start,
        /* [in] */ Int32 nbNewChars,
        /* [in] */ Int32 flag,
        /* [in] */ Boolean atEnd,
        /* [in] */ Boolean textIsRemoved);

    ECO_LOCAL CARAPI_(void) RemoveSpan(
        /* [in] */ Int32 i);

    ECO_LOCAL static CARAPI_(Boolean) HasNonExclusiveExclusiveSpanAt(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 offset);

    ECO_LOCAL CARAPI_(void) SendToSpanWatchers(
        /* [in] */ Int32 replaceStart,
        /* [in] */ Int32 replaceEnd,
        /* [in] */ Int32 nbNewChars);

    ECO_LOCAL CARAPI_(void) SendBeforeTextChanged(
        /* [in] */ ArrayOf<ITextWatcher*>* watchers,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 after);

    ECO_LOCAL CARAPI_(void) SendTextChanged(
        /* [in] */ ArrayOf<ITextWatcher*>* watchers,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 after);

    ECO_LOCAL CARAPI_(void) SendAfterTextChanged(
        /* [in] */ ArrayOf<ITextWatcher*>* watchers);

    ECO_LOCAL CARAPI_(void) SendSpanAdded(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    ECO_LOCAL CARAPI_(void) SendSpanRemoved(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    ECO_LOCAL CARAPI_(void) SendSpanChanged(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 oldStart,
        /* [in] */ Int32 oldEnd,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    ECO_LOCAL static CARAPI_(String) Region(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    ECO_LOCAL CARAPI CheckRange(
        /* [in] */ const String& operation,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    ECO_LOCAL CARAPI SetSpan(
        /* [in] */ Boolean send,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    ECO_LOCAL CARAPI CheckRange(
        /* [in] */ ICharSequence* tb,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

private:

    ECO_LOCAL static AutoPtr< ArrayOf<IInputFilter*> > NO_FILTERS;
    AutoPtr< ArrayOf<IInputFilter*> > mFilters;

    AutoPtr< ArrayOf<Char32> > mText;
    Int32 mGapStart;
    Int32 mGapLength;

    AutoPtr< ArrayOf<IInterface*> > mSpans;
    AutoPtr< ArrayOf<Int32> > mSpanStarts;
    AutoPtr< ArrayOf<Int32> > mSpanEnds;
    AutoPtr< ArrayOf<Int32> > mSpanFlags;
    Int32 mSpanCount;
    Int32 mSpanCountBeforeAdd;

    // TODO These value are tightly related to the public SPAN_MARK/POINT values in {@link Spanned}
    ECO_LOCAL static const Int32 MARK = 1;
    ECO_LOCAL static const Int32 POINT = 2;
    ECO_LOCAL static const Int32 PARAGRAPH = 3;

    ECO_LOCAL static const Int32 START_MASK = 0xF0;
    ECO_LOCAL static const Int32 END_MASK = 0x0F;
    ECO_LOCAL static const Int32 START_SHIFT = 4;

    // These bits are not (currently) used by SPANNED flags
    ECO_LOCAL static const Int32 SPAN_START_AT_START = 0x1000;
    ECO_LOCAL static const Int32 SPAN_START_AT_END = 0x2000;
    ECO_LOCAL static const Int32 SPAN_END_AT_START = 0x4000;
    ECO_LOCAL static const Int32 SPAN_END_AT_END = 0x8000;
    ECO_LOCAL static const Int32 SPAN_START_END_MASK = 0xF000;

    ECO_LOCAL static AutoPtr<ICharSequence> EMPTY_CS;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_SPANNABLESTRINGBUILDER_H__
