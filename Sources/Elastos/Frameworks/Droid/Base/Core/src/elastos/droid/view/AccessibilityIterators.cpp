
#include "elastos/droid/view/AccessibilityIterators.h"
#include "elastos/droid/view/ViewRootImpl.h"

#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Text::IBreakIteratorHelper;
using Elastos::Text::CBreakIteratorHelper;

using Elastos::Droid::Content::EIID_IComponentCallbacks;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(AccessibilityIterators::AbstractTextSegmentIterator, Object, ITextSegmentIterator)

AccessibilityIterators::AbstractTextSegmentIterator::AbstractTextSegmentIterator()
{
    mSegment = ArrayOf<Int32>::Alloc(2);
}

ECode AccessibilityIterators::AbstractTextSegmentIterator::Initialize(
    /* [in] */ const String& text)
{
    mText = text;
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > AccessibilityIterators::AbstractTextSegmentIterator::GetRange(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (start < 0 || end < 0 || start ==  end) {
        return NULL;
    }
    (*mSegment)[0] = start;
    (*mSegment)[1] = end;
    return mSegment;
}


CAR_INTERFACE_IMPL(AccessibilityIterators::CharacterTextSegmentIterator, AccessibilityIterators::AbstractTextSegmentIterator, IComponentCallbacks)
AutoPtr<AccessibilityIterators::CharacterTextSegmentIterator> AccessibilityIterators::CharacterTextSegmentIterator::sInstance;

AutoPtr<AccessibilityIterators::CharacterTextSegmentIterator> AccessibilityIterators::CharacterTextSegmentIterator::GetInstance(
    /* [in] */ ILocale* locale)
{
    if (sInstance == NULL) {
        sInstance = new CharacterTextSegmentIterator(locale);
    }
    return sInstance;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::Initialize(
    /* [in] */ const String& text)
{
    AccessibilityIterators::AbstractTextSegmentIterator::Initialize(text);
    mImpl->SetText(text);
    return NOERROR;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::Following(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLegth = mText.GetLength();
    if (textLegth <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset >= textLegth) {
        *array = NULL;
        return NOERROR;
    }
    Int32 start = offset;
    if (start < 0) {
        start = 0;
    }
    Boolean isBoundary;
    while (mImpl->IsBoundary(start, &isBoundary), !isBoundary) {
        Int32 param = start;
        mImpl->GetFollowing(param, &start);
        if (start == IBreakIterator::DONE) {
            *array = NULL;
            return NOERROR;
        }
    }
    Int32 end;
    mImpl->GetFollowing(start, &end);
    if (end == IBreakIterator::DONE) {
        *array = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > range = GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::Preceding(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLegth = mText.GetLength();
    if (textLegth <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset <= 0) {
        *array = NULL;
        return NOERROR;
    }
    Int32 end = offset;
    if (end > textLegth) {
        end = textLegth;
    }
    Boolean isBoundary;
    while (mImpl->IsBoundary(end, &isBoundary), !isBoundary) {
        mImpl->GetPreceding(end, &end);
        if (end == IBreakIterator::DONE) {
            *array = NULL;
            return NOERROR;
        }
    }
    Int32 start;
    mImpl->GetPreceding(end, &start);
    if (start == IBreakIterator::DONE) {
        *array = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > range = GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<ILocale> locale;
    newConfig->GetLocale((ILocale**)&locale);
    AutoPtr<IObject> obj = IObject::Probe(mLocale);
    Boolean equals;
    if (obj->Equals(locale, &equals), !equals) {
        mLocale = locale;
        OnLocaleChanged(locale);
    }
    return NOERROR;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::OnLowMemory()
{
    return NOERROR;
}

ECode AccessibilityIterators::CharacterTextSegmentIterator::OnLocaleChanged(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IBreakIteratorHelper> helper;
    CBreakIteratorHelper::AcquireSingleton((IBreakIteratorHelper**)&helper);
    helper->GetCharacterInstance(locale, (IBreakIterator**)&mImpl);
    return NOERROR;
}

AccessibilityIterators::CharacterTextSegmentIterator::CharacterTextSegmentIterator(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    OnLocaleChanged(locale);
    ViewRootImpl::AddConfigCallback(this);
}

AutoPtr<AccessibilityIterators::WordTextSegmentIterator> AccessibilityIterators::WordTextSegmentIterator::sInstance;

AutoPtr<AccessibilityIterators::WordTextSegmentIterator> AccessibilityIterators::WordTextSegmentIterator::GetInstance(
    /* [in] */ ILocale* locale)
{
    if (sInstance == NULL) {
        sInstance = new WordTextSegmentIterator(locale);
    }
    return sInstance;
}

ECode AccessibilityIterators::WordTextSegmentIterator::Following(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLegth = mText.GetLength();
    if (textLegth <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset >= mText.GetLength()) {
        *array = NULL;
        return NOERROR;
    }
    Int32 start = offset;
    if (start < 0) {
        start = 0;
    }
    while (!IsLetterOrDigit(start) && !IsStartBoundary(start)) {
        Int32 param = start;
        mImpl->GetFollowing(param, &start);
        if (start == IBreakIterator::DONE) {
            *array = NULL;
            return NOERROR;
        }
    }
    Int32 end;
    mImpl->GetFollowing(start, &end);
    if (end == IBreakIterator::DONE || !IsEndBoundary(end)) {
        *array = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > range = GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode AccessibilityIterators::WordTextSegmentIterator::Preceding(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLegth = mText.GetLength();
    if (textLegth <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset <= 0) {
        *array = NULL;
        return NOERROR;
    }
    Int32 end = offset;
    if (end > textLegth) {
        end = textLegth;
    }
    while (end > 0 && !IsLetterOrDigit(end - 1) && !IsEndBoundary(end)) {
        Int32 param = end;
        mImpl->GetPreceding(param, &end);
        if (end == IBreakIterator::DONE) {
            *array = NULL;
            return NOERROR;
        }
    }
    Int32 start;
    mImpl->GetPreceding(end, &start);
    if (start == IBreakIterator::DONE || !IsStartBoundary(start)) {
        *array = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > range = GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode AccessibilityIterators::WordTextSegmentIterator::OnLocaleChanged(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IBreakIteratorHelper> helper;
    CBreakIteratorHelper::AcquireSingleton((IBreakIteratorHelper**)&helper);
    helper->GetCharacterInstance(locale, (IBreakIterator**)&mImpl);
    return NOERROR;
}

Boolean AccessibilityIterators::WordTextSegmentIterator::IsStartBoundary(
    /* [in] */ Int32 index)
{
    return IsLetterOrDigit(index)
        && (index == 0 || !IsLetterOrDigit(index - 1));
}

Boolean AccessibilityIterators::WordTextSegmentIterator::IsEndBoundary(
    /* [in] */ Int32 index)
{
    return (index > 0 && IsLetterOrDigit(index - 1))
        && (index == mText.GetLength() || !IsLetterOrDigit(index));
}

Boolean AccessibilityIterators::WordTextSegmentIterator::IsLetterOrDigit(
    /* [in] */ Int32 index)
{
    if (index >= 0 && index < mText.GetLength()) {
        if (index < 0 || index >= mText.GetLength()) {
            assert(0);
        }
        Int32 codePoint = mText.GetChar(index);

        return Character::IsLetterOrDigit(codePoint);
    }
    return FALSE;
}

AccessibilityIterators::WordTextSegmentIterator::WordTextSegmentIterator(
    /* [in] */ ILocale* locale)
    : CharacterTextSegmentIterator(locale)
{}

AutoPtr<AccessibilityIterators::ParagraphTextSegmentIterator> AccessibilityIterators::ParagraphTextSegmentIterator::sInstance;

AutoPtr<AccessibilityIterators::ParagraphTextSegmentIterator> AccessibilityIterators::ParagraphTextSegmentIterator::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new ParagraphTextSegmentIterator();
    }
    return sInstance;
}

ECode AccessibilityIterators::ParagraphTextSegmentIterator::Following(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLength = mText.GetLength();
    if (textLength <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset >= textLength) {
        *array = NULL;
        return NOERROR;
    }
    Int32 start = offset;
    if (start < 0) {
        start = 0;
    }
    while (start < textLength && mText.GetChar(start) == '\n'
            && !IsStartBoundary(start)) {
        start++;
    }
    if (start >= textLength) {
        *array = NULL;
        return NOERROR;
    }
    Int32 end = start + 1;
    while (end < textLength && !IsEndBoundary(end)) {
        end++;
    }
    AutoPtr<ArrayOf<Int32> > range= GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode AccessibilityIterators::ParagraphTextSegmentIterator::Preceding(
    /* [in] */ Int32 offset,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    Int32 textLength = mText.GetLength();
    if (textLength <= 0) {
        *array = NULL;
        return NOERROR;
    }
    if (offset <= 0) {
        *array = NULL;
        return NOERROR;
    }
    Int32 end = offset;
    if (end > textLength) {
        end = textLength;
    }
    while(end > 0 && mText.GetChar(end - 1) == '\n' && !IsEndBoundary(end)) {
        end--;
    }
    if (end <= 0) {
        *array = NULL;
        return NOERROR;
    }
    Int32 start = end - 1;
    while (start > 0 && !IsStartBoundary(start)) {
        start--;
    }
    AutoPtr<ArrayOf<Int32> > range = GetRange(start, end);
    *array = range;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

Boolean AccessibilityIterators::ParagraphTextSegmentIterator::IsStartBoundary(
    /* [in] */ Int32 index)
{
    return (mText.GetChar(index) != '\n'
        && (index == 0 || mText.GetChar(index - 1) == '\n'));
}

Boolean AccessibilityIterators::ParagraphTextSegmentIterator::IsEndBoundary(
    /* [in] */ Int32 index)
{
    return (index > 0 && mText.GetChar(index - 1) != '\n'
        && (index == mText.GetLength() || mText.GetChar(index) == '\n'));
}

} // namespace View
} // namespace Droid
} // namespace Elastos
