
#ifndef __ELASTOS_DROID_TEXT_SPANSET_H__
#define __ELASTOS_DROID_TEXT_SPANSET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * A cached set of spans. Caches the result of {@link Spanned#getSpans(int, int, Class)} and then
 * provides faster access to {@link Spanned#nextSpanTransition(int, int, Class)}.
 *
 * Fields are left public for a convenient direct access.
 *
 * Note that empty spans are ignored by this class.
 * @hide
 */
template<typename E/*implements IInterface*/>
class SpanSet : public Object
{
public:
    SpanSet(
        /* [in] */ const InterfaceID& type);

    CARAPI_(void) Init(
        /* [in] */ ISpanned* spanned,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit);

    /**
     * Returns true if there are spans intersecting the given interval.
     * @param end must be strictly greater than start
     */
    CARAPI_(Boolean) HasSpansIntersecting(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Similar to {@link Spanned#nextSpanTransition(int, int, Class)}
     */
    CARAPI_(Int32) GetNextTransition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit);

    /**
     * Removes all internal references to the spans to avoid memory leaks.
     */
    CARAPI_(void) Recycle();

public:
    Int32 mNumberOfSpans;
    AutoPtr< ArrayOf<E*> > mSpans;
    AutoPtr< ArrayOf<Int32> > mSpanStarts;
    AutoPtr< ArrayOf<Int32> > mSpanEnds;
    AutoPtr< ArrayOf<Int32> > mSpanFlags;

private:
    // private final Class<? extends E> classType;
    InterfaceID mClassType;
};

template <typename E>
SpanSet<E>::SpanSet(
    /* [in] */ const InterfaceID& type)
    : mNumberOfSpans(0)
    , mClassType(type)
{}

//@SuppressWarnings("unchecked")
template <typename E>
void SpanSet<E>::Init(
    /* [in] */ ISpanned* spanned,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit)
{
    AutoPtr< ArrayOf<IInterface*> > allSpans;
    spanned->GetSpans(start, limit, mClassType, (ArrayOf<IInterface*>**)&allSpans);
    assert(allSpans != NULL);
    Int32 length = allSpans->GetLength();

    if (length > 0 && (mSpans == NULL || mSpans->GetLength() < length)) {
        // These arrays may end up being too large because of the discarded empty spans
        mSpans = ArrayOf<E*>::Alloc(length);
        mSpanStarts = ArrayOf<Int32>::Alloc(length);
        mSpanEnds = ArrayOf<Int32>::Alloc(length);
        mSpanFlags = ArrayOf<Int32>::Alloc(length);
    }

    mNumberOfSpans = 0;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<E> span = (E*)(*allSpans)[i];

        Int32 spanStart, spanEnd;
        spanned->GetSpanStart(span, &spanStart);
        spanned->GetSpanEnd(span, &spanEnd);
        if (spanStart == spanEnd) continue;

        Int32 spanFlag;
        spanned->GetSpanFlags(span, &spanFlag);

        mSpans->Set(mNumberOfSpans, span);
        (*mSpanStarts)[mNumberOfSpans] = spanStart;
        (*mSpanEnds)[mNumberOfSpans] = spanEnd;
        (*mSpanFlags)[mNumberOfSpans] = spanFlag;

        mNumberOfSpans++;
    }
}

/**
 * Returns true if there are spans intersecting the given interval.
 * @param end must be strictly greater than start
 */
template <typename E>
Boolean SpanSet<E>::HasSpansIntersecting(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    for (Int32 i = 0; i < mNumberOfSpans; i++) {
        // equal test is valid since both intervals are not empty by construction
        if ((*mSpanStarts)[i] >= end || (*mSpanEnds)[i] <= start) continue;
        return TRUE;
    }
    return FALSE;
}

/**
 * Similar to {@link Spanned#nextSpanTransition(int, int, Class)}
 */
template <typename E>
Int32 SpanSet<E>::GetNextTransition(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit)
{
    for (Int32 i = 0; i < mNumberOfSpans; i++) {
        Int32 spanStart = (*mSpanStarts)[i];
        Int32 spanEnd = (*mSpanEnds)[i];
        if (spanStart > start && spanStart < limit) limit = spanStart;
        if (spanEnd > start && spanEnd < limit) limit = spanEnd;
    }
    return limit;
}

/**
 * Removes all internal references to the spans to avoid memory leaks.
 */
template <typename E>
void SpanSet<E>::Recycle()
{
    // The spans array is guaranteed to be not null when numberOfSpans is > 0
    for (Int32 i = 0; i < mNumberOfSpans; i++) {
        mSpans->Set(i, NULL); // prevent a leak: no reference kept when TextLine is recycled
    }
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__CSPANSET_H__
