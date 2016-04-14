
#include "AttributedString.h"
#include "StringBuffer.h"
#include "CHashSet.h"
#include "CHashMap.h"
#include <utils/Log.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Text {

//========================================================================
// AttributedString::Range
//========================================================================

AttributedString::Range::Range(
    /* [in] */ Int32 s,
    /* [in] */ Int32 e,
    /* [in] */ IInterface* v)
    : mStart(s)
    , mEnd(e)
    , mValue(v)
{}

AttributedString::Range& AttributedString::Range::operator = (
    /* [in] */ const AttributedString::Range& other)
{
    mStart = other.mStart;
    mEnd = other.mEnd;
    mValue = other.mValue;
    return *this;
}

//========================================================================
// AttributedString::AttributedIterator
//========================================================================
CAR_INTERFACE_IMPL_3(AttributedString::AttributedIterator, Object, IAttributedCharacterIterator, ICharacterIterator, ICloneable)

AttributedString::AttributedIterator::AttributedIterator(
    /* [in] */ AttributedString* attrString)
    : mBegin(0)
    , mOffset(0)
    , mAttrString(attrString)
{
    mEnd = attrString->mText.GetLength();
}

AttributedString::AttributedIterator::AttributedIterator(
    /* [in] */ AttributedString* attrString,
    /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
    /* [in] */ Int32 begin,
    /* [in] */ Int32 end)
{
    if (begin < 0 || end > (Int32)mAttrString->mText.GetLength() || begin > end) {
        ALOGE("AttributedString::AttributedIterator(): IllegalArgumentException.");
        //throw new IllegalArgumentException();
        assert(0);
    }
    mBegin = begin;
    mEnd = end;
    mOffset = begin;
    mAttrString = attrString;

    if (attributes != NULL) {
        Int32 len = (Int32)(attributes->GetLength() * 4 / 3) + 1;
        mAttributesAllowed.Resize(len);
        for (Int32 i = attributes->GetLength(); --i >= 0;) {
            mAttributesAllowed.Insert((*attributes)[i]);
        }
    }
}

AttributedString::AttributedIterator::~AttributedIterator()
{
    mAttributesAllowed.Clear();
}

ECode AttributedString::AttributedIterator::Clone(
    /* [out] */ IInterface** copy)
{
    VALIDATE_NOT_NULL(copy)

    // try {
    AutoPtr<AttributedIterator> clone = new AttributedIterator(mAttrString);
    clone->mBegin = mBegin;
    clone->mEnd = mEnd;
    clone->mOffset = mOffset;

    if (!mAttributesAllowed.IsEmpty()) {
        clone->mAttributesAllowed = mAttributesAllowed;
    }
    *copy = TO_IINTERFACE(clone);
    REFCOUNT_ADD(*copy)
    return NOERROR;
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }
}

ECode AttributedString::AttributedIterator::GetCurrent(
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value);
    if (mOffset == mEnd) {
        *value = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }
    *value = mAttrString->mText.GetChar(mOffset);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetFirst(
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value);
    if (mBegin == mEnd) {
        *value = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }
    mOffset = mBegin;
    *value = mAttrString->mText.GetChar(mOffset);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetBeginIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mBegin;
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetEndIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mEnd;
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mOffset;
    return NOERROR;
}

Boolean AttributedString::AttributedIterator::InRange(
    /* [in] */ Range* range)
{
    if (IAnnotation::Probe(range->mValue) == NULL) {
        return TRUE;
    }
    return range->mStart >= mBegin && range->mStart < mEnd
            && range->mEnd > mBegin && range->mEnd <= mEnd;
}

Boolean AttributedString::AttributedIterator::InRange(
    /* [in] */ List<AutoPtr<Range> >* ranges)
{
    List<AutoPtr<Range> >::Iterator it;
    for (it = ranges->Begin(); it != ranges->End(); ++it) {
        Range* range = *it;
        if (range->mStart >= mBegin && range->mStart < mEnd) {
            return (IAnnotation::Probe(range->mValue) == NULL)
                    || (range->mEnd > mBegin && range->mEnd <= mEnd);
        }
        else if (range->mEnd > mBegin && range->mEnd <= mEnd) {
            return (IAnnotation::Probe(range->mValue) == NULL)
                    || (range->mStart >= mBegin && range->mStart < mEnd);
        }
    }
    return FALSE;
}

ECode AttributedString::AttributedIterator::GetAllAttributeKeys(
    /* [out] */ ISet** allAttributedKeys)
{
    VALIDATE_NOT_NULL(allAttributedKeys);

    AutoPtr<ISet> result;
    CHashSet::New((ISet**)&result);
    ICollection* ci = ICollection::Probe(result);

    *allAttributedKeys = result;
    REFCOUNT_ADD(*allAttributedKeys);

    AttributeRangeMapIterator it = mAttrString->mAttributeMap.Begin();
    if (mBegin == 0 && mEnd == (Int32)mAttrString->mText.GetLength()
            && mAttributesAllowed.IsEmpty()) {
        for(; it != mAttrString->mAttributeMap.End(); ++it) {
            ci->Add(it->mFirst);
        }
        return NOERROR;
    }

    for(; it != mAttrString->mAttributeMap.End(); ++it) {
        if (mAttributesAllowed.Find(it->mFirst) != mAttributesAllowed.End()) {
            AutoPtr<List<AutoPtr<Range> > > ranges = it->mSecond;
            if (InRange(ranges)) {
                ci->Add(it->mFirst);
            }
        }
    }

    return NOERROR;
}

AutoPtr<IInterface> AttributedString::AttributedIterator::CurrentValue(
    /* [in] */ List<AutoPtr<Range> >* ranges)
{
    List<AutoPtr<Range> >::Iterator it = ranges->Begin();
    for (it = ranges->Begin(); it != ranges->End(); ++it) {
        AutoPtr<Range> range = *it;
        if (mOffset >= range->mStart && mOffset < range->mEnd) {
            return InRange(range) ? range->mValue : NULL;
        }
    }

    return NULL;
}

ECode AttributedString::AttributedIterator::GetAttribute(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [out] */ IInterface** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    if (mAttributesAllowed.Find(attribute) == mAttributesAllowed.End()) {
        return NOERROR;
    }
    AutoPtr<List<AutoPtr<Range> > > ranges;
    AttributeRangeMapIterator it = mAttrString->mAttributeMap.Find(attribute);
    if (it == mAttrString->mAttributeMap.End()) {
        return NOERROR;
    }

    ranges = it->mSecond;
    AutoPtr<IInterface> temp = CurrentValue(ranges);
    *instance = temp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetAttributes(
    /* [out] */ IMap** attributes)
{
    VALIDATE_NOT_NULL(attributes);

    Int32 size = (mAttrString->mAttributeMap.GetSize() * 4 / 3) + 1;
    AutoPtr<IMap> result;
    CHashMap::New(size, (IMap**)&result);
    *attributes = result;
    REFCOUNT_ADD(*attributes);

    AttributeRangeMapIterator it = mAttrString->mAttributeMap.Begin();
    AttributeRangeMapIterator itnext = ++mAttrString->mAttributeMap.Begin();
    for(; it != mAttrString->mAttributeMap.End(); ++it,++itnext) {
        if (mAttributesAllowed.IsEmpty()
            || mAttributesAllowed.Find(itnext->mFirst) != mAttributesAllowed.End()) {
            AutoPtr<IInterface> value = CurrentValue(itnext->mSecond);
            if (value != NULL) {
                result->Put(itnext->mFirst->Probe(EIID_IInterface), value);
            }
        }
    }

    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetRunLimit(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    AutoPtr<ISet> allAttributedKeys;
    GetAllAttributeKeys((ISet**)&allAttributedKeys);
    return GetRunLimit(allAttributedKeys, index);
}

Int32 AttributedString::AttributedIterator::RunLimit(
    /* [in] */ List<AutoPtr<Range> >* ranges)
{
    Int32 result = mEnd;

    List<AutoPtr<Range> >::ReverseIterator rit = ranges->RBegin();
    for (rit = ranges->RBegin(); rit != ranges->REnd(); ++rit) {
        AutoPtr<Range> range = *rit;
        if (range->mEnd <= mBegin) {
            break;
        }
        if (mOffset >= range->mStart && mOffset < range->mEnd) {
            return InRange(range) ? range->mEnd : result;
        }
        else if (mOffset >= range->mEnd) {
            break;
        }
        result = range->mStart;
    }

    return result;
}

ECode AttributedString::AttributedIterator::GetRunLimit(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    if ((!mAttributesAllowed.IsEmpty()) && (mAttributesAllowed.Find(attribute) == mAttributesAllowed.End())) {
        *index = mEnd;
        return NOERROR;
    }

    AttributeRangeMapIterator it = mAttrString->mAttributeMap.Find(attribute);
    if (it == mAttrString->mAttributeMap.End()) {
        *index = mEnd;
        return NOERROR;
    }

    AutoPtr<List<AutoPtr<Range> > > ranges = it->mSecond;
    *index = RunLimit(ranges);

    return NOERROR;

}

ECode AttributedString::AttributedIterator::GetRunLimit(
    /* [in] */ ISet* attributes,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = -1;
    VALIDATE_NOT_NULL(attributes);

    AutoPtr<IIterator> it;
    attributes->GetIterator((IIterator**)&it);

    Int32 limit = mEnd;
    Int32 newLimit;
    Boolean hasNext;
    IAttributedCharacterIteratorAttribute* attr;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        attr = IAttributedCharacterIteratorAttribute::Probe(obj);

        GetRunLimit(attr, &newLimit);
        if (newLimit < limit) {
            limit = newLimit;
        }
    }

    *index = limit;
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetRunStart(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    AutoPtr<ISet> allAttributedKeys;
    GetAllAttributeKeys((ISet**)&allAttributedKeys);
    return GetRunStart(allAttributedKeys, index);
}

Int32 AttributedString::AttributedIterator::RunStart(
    /* [in] */ List<AutoPtr<Range> >* ranges)
{
    Int32 result = mBegin;

    List<AutoPtr<Range> >::Iterator it;
    for (it = ranges->Begin(); it != ranges->End(); ++it) {
        AutoPtr<Range> range = *it;
        if (range->mStart >= mEnd) {
            break;
        }
        if (mOffset >= range->mStart && mOffset < range->mEnd) {
            return InRange(range) ? range->mStart : result;
        }
        else if (mOffset < range->mStart) {
            break;
        }
        result = range->mEnd;
    }

    return result;
}

ECode AttributedString::AttributedIterator::GetRunStart(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    if (mAttributesAllowed.Find(attribute) == mAttributesAllowed.End()) {
        *index = mBegin;
        return NOERROR;
    }
    AttributeRangeMapIterator it = mAttrString->mAttributeMap.Find(attribute);
    if (it == mAttrString->mAttributeMap.End()) {
        *index = mBegin;
        return NOERROR;
    }

    AutoPtr<List<AutoPtr<Range> > > ranges = it->mSecond;
    *index = RunStart(ranges);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetRunStart(
    /* [in] */ ISet* attributes,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = -1;
    VALIDATE_NOT_NULL(attributes);

    IIterable* iterable = IIterable::Probe(attributes);
    assert(iterable != NULL);

    AutoPtr<IIterator> it;
    iterable->GetIterator((IIterator**)&it);

    Int32 start = mBegin;
    Int32 newStart;
    Boolean hasNext;
    IAttributedCharacterIteratorAttribute* attr;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        attr = IAttributedCharacterIteratorAttribute::Probe(obj);

        GetRunStart(attr, &newStart);
        if (newStart > start) {
            start = newStart;
        }
    }

    *index = start;
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetLast(
    /* [out] */ Char32* lastValue)
{
    VALIDATE_NOT_NULL(lastValue);
    if (mBegin == mEnd) {
        *lastValue = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }
    mOffset = mEnd - 1;
    *lastValue = mAttrString->mText.GetChar(mOffset);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetNext(
    /* [out] */ Char32* nextValue)
{
    VALIDATE_NOT_NULL(nextValue);
    if (mOffset >= (mEnd - 1)) {
        mOffset = mEnd;
        *nextValue = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }
    *nextValue = mAttrString->mText.GetChar(++mOffset);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::GetPrevious(
    /* [out] */ Char32* previousValue)
{
    VALIDATE_NOT_NULL(previousValue);
    if (mOffset == mBegin) {
        *previousValue = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }

    *previousValue = mAttrString->mText.GetChar(--mOffset);
    return NOERROR;
}

ECode AttributedString::AttributedIterator::SetIndex(
    /* [in] */ Int32 location,
    /* [out] */ Char32* newChar)
{
    VALIDATE_NOT_NULL(newChar);
    *newChar = '\0';

    if (location < mBegin || location > mEnd) {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mOffset = location;
    if (mOffset == mEnd) {
        *newChar = (Char32)ICharacterIterator::DONE;
        return NOERROR;
    }

    *newChar = mAttrString->mText.GetChar(mOffset);
    return NOERROR;
}

//========================================================================
// AttributedString
//========================================================================
CAR_INTERFACE_IMPL(AttributedString, Object, IAttributedString)

AttributedString::~AttributedString()
{
    mAttributeMap.Clear();
}

ECode AttributedString::constructor(
    /* [in] */ IAttributedCharacterIterator* iterator)
{
    VALIDATE_NOT_NULL(iterator)
    ICharacterIterator* ci = ICharacterIterator::Probe(iterator);

    Int32 bi, ei;
    ci->GetBeginIndex(&bi);
    ci->GetEndIndex(&ei);
    if (bi > ei) {
        ALOGE("AttributedString::constructor(): IllegalArgumentException, invalid substring range.");
        //throw new IllegalArgumentException("Invalid substring range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuffer buffer;
    for (Int32 i = bi; i < ei; i++) {
        Char32 cv, nv;
        ci->GetCurrent(&cv);
        buffer += cv;
        ci->GetNext(&nv);
    }
    buffer.ToString(&mText);

    AutoPtr<ISet> attributes;
    iterator->GetAllAttributeKeys((ISet**)&attributes);
    if (attributes == NULL) {
        return NOERROR;
    }

    AutoPtr<IIterator> it;
    attributes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IAttributedCharacterIteratorAttribute* attr;
    Char32 ch, cv;
    Int32 start, limit;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        attr = IAttributedCharacterIteratorAttribute::Probe(obj);

        ci->SetIndex(0, &ch);
        while (ci->GetCurrent(&cv), cv != (Char32)ICharacterIterator::DONE) {
            iterator->GetRunStart(attr, &start);
            iterator->GetRunLimit(attr, &limit);
            AutoPtr<IInterface> value;
            iterator->GetAttribute(attr, (IInterface**)&value);
            if (value != NULL) {
                AddAttribute(attr, value, start, limit);
            }
            ci->SetIndex(limit, &ch);
        }
    }

    return NOERROR;
}

ECode AttributedString::constructor(
    /* [in] */ IAttributedCharacterIterator* iterator,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISet* attributes)
{
    VALIDATE_NOT_NULL(iterator)
    ICharacterIterator* ci = ICharacterIterator::Probe(iterator);
    Int32 beginIndex, endIndex;
    ci->GetBeginIndex(&beginIndex);
    ci->GetEndIndex(&endIndex);
    if (start < beginIndex || end > endIndex || start > end) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException();
    }

    if (attributes == NULL) {
        return NOERROR;
    }

    StringBuffer buffer;
    Char32 newIndex;
    ci->SetIndex(start, &newIndex);
    Int32 index;
    while (ci->GetIndex(&index) ,index < end) {
        Char32 c;
        ci->GetCurrent(&c);
        buffer += c;
        Char32 nextC;
        ci->GetNext(&nextC);
    }
    buffer.ToString(&mText);

    AutoPtr<IIterator> it;
    attributes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IAttributedCharacterIteratorAttribute* attr;
    Char32 c, newChar;
    Int32 id, runStart, limit;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        attr = IAttributedCharacterIteratorAttribute::Probe(obj);

        ci->SetIndex(start, &c);
        while (ci->GetIndex(&id) ,index < end) {
            AutoPtr<IInterface> value;
            iterator->GetAttribute(attr, (IInterface**)&value);
            iterator->GetRunStart(attr, &runStart);
            iterator->GetRunLimit(attr, &limit);
            if ((IAnnotation::Probe(value) != NULL
                    && runStart >= start && limit <= end)
                        || (value != NULL && (IAnnotation::Probe(value) == NULL))) {
                AddAttribute(attr, value,
                    (runStart < start ? start : runStart) - start, (limit > end ? end : limit) - start);
            }
            ci->SetIndex(limit, &newChar);
        }
    }

    return NOERROR;
}

ECode AttributedString::constructor(
    /* [in] */ IAttributedCharacterIterator* iterator,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ISet> container;
    iterator->GetAllAttributeKeys((ISet**)&container);
    return constructor(iterator, start, end, container);
}

ECode AttributedString::constructor(
    /* [in] */ IAttributedCharacterIterator* iterator,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes)
{
    AutoPtr<ISet> container;
    CHashSet::New((ISet**)&container);
    if (attributes != NULL) {
        Boolean modified;
        ICollection* collection = ICollection::Probe(container);
        for (Int32 i = 0; i < attributes->GetLength(); ++i) {
            collection->Add((*attributes)[i], &modified);
        }
    }
    return constructor(iterator, start, end, container);
}

ECode AttributedString::constructor(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        //throw new NullPointerException("value == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mText = value;
    return NOERROR;
}

ECode AttributedString::constructor(
    /* [in] */ const String& value,
    /* [in] */ IMap* attributes)
{
    VALIDATE_NOT_NULL(attributes);

    if (value.IsNull()) {
        // throw new NullPointerException("value == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    ICollection* c = ICollection::Probe(attributes);
    assert(c != NULL);
    Boolean empty;
    c->IsEmpty(&empty);
    if (value.IsEmpty() && !empty) {
        // throw new IllegalArgumentException("Cannot add attributes to empty string");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mText = value;

    AutoPtr<IIterator> it;
    IIterable::Probe(attributes)->GetIterator((IIterator**)&it);
    AutoPtr<List<AutoPtr<Range> > > ranges;
    IAttributedCharacterIteratorAttribute* attr;
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;;
        it->GetNext((IInterface**)&obj);
        IMapEntry* entry = IMapEntry::Probe(obj);

        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);

        ranges = new List<AutoPtr<Range> >(1);
        AutoPtr<Range> range = new Range(0, value.GetLength(), val);
        ranges->PushBack(range);
        attr = IAttributedCharacterIteratorAttribute::Probe(key);
        mAttributeMap[attr] = ranges;
    }

    return NOERROR;
}

ECode AttributedString::AddAttribute(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [in] */ IInterface* value)
{
    if (attribute == NULL) {
        //throw new NullPointerException("attribute == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mText.IsEmpty()) {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<List<AutoPtr<Range> > > ranges;
    AttributeRangeMapIterator it = mAttributeMap.Find(attribute);
    if (it == mAttributeMap.End()) {
        ranges = new List<AutoPtr<Range> >(0);
        mAttributeMap[attribute] = ranges;
    }
    else {
        ranges = it->mSecond;
        ranges->Clear();
    }
    AutoPtr<Range> range = new Range(0, mText.GetLength(), value);
    ranges->PushBack(range);
    return NOERROR;
}

ECode AttributedString::AddAttribute(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [in] */ IInterface* value,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (attribute == NULL) {
        //throw new NullPointerException("attribute == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (start < 0 || end > (Int32)mText.GetLength() || start >= end) {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (value == NULL) {
        return NOERROR;
    }

    AutoPtr<List<AutoPtr<Range> > > ranges;
    AttributeRangeMapIterator mapIt = mAttributeMap.Find(attribute);
    if (mapIt == mAttributeMap.End()) {
        ranges = new List<AutoPtr<Range> >(0);
        AutoPtr<Range> range = new Range(start, end, value);
        ranges->PushBack(range);
        mAttributeMap[attribute] = ranges;
        return NOERROR;
    }
    else {
        ranges = mapIt->mSecond;
    }

    List<AutoPtr<Range> >::Iterator it;
    for (it = ranges->Begin(); it != ranges->End(); ++it) {
        Range& range = **it;
        if (end <= range.mStart) {
            break;
        }
        else if (start < range.mEnd
                || (start == range.mEnd && value == range.mValue)) {
            it = ranges->Erase(it);
            AutoPtr<Range> r1 = new Range(range.mStart, start, range.mValue);
            AutoPtr<Range> r3 = new Range(end, range.mEnd, range.mValue);

            while (end > range.mEnd && (++it) != ranges->End()) {
                range = **it;
                if (end <= range.mEnd) {
                    if (end > range.mStart
                            || (end == range.mStart && value == range.mValue)) {
                        it = ranges->Erase(it);
                        r3 = new Range(end, range.mEnd, range.mValue);
                        break;
                    }
                }
                else {
                    it = ranges->Erase(it);
                }
            }

            if (value == r1->mValue) {
                if (value == r3->mValue) {
                    AutoPtr<Range> r = new Range(r1->mStart < start ? r1->mStart : start,
                            r3->mEnd > end ? r3->mEnd : end, r1->mValue);
                    ranges->Insert(it, r);
                }
                else {
                    AutoPtr<Range> r = new Range(r1->mStart < start ? r1->mStart : start,
                            end, r1->mValue);
                    it = ranges->Insert(it, r);
                    if (r3->mStart < r3->mEnd) {
                        ranges->Insert(it, r3);
                    }
                }
            }
            else {
                if (value == r3->mValue) {
                    if (r1->mStart < r1->mEnd) {
                        it = ranges->Insert(it, r1);
                    }
                    AutoPtr<Range> r = new Range(start, r3->mEnd > end ? r3->mEnd : end,
                            r3->mValue);
                    ranges->Insert(it, r);
                }
                else {
                    if (r1->mStart < r1->mEnd) {
                        ranges->Insert(it, r1);
                    }
                    AutoPtr<Range> r = new Range(start, end, value);
                    it = ranges->Insert(it, r);
                    if (r3->mStart < r3->mEnd) {
                        ranges->Insert(it, r3);
                    }
                }
            }
            return NOERROR;
        }
    }

    AutoPtr<Range> r = new Range(start, end, value);
    ranges->Insert(it, r);
    return NOERROR;
}

ECode AttributedString::AddAttributes(
    /* [in] */ IMap* attributes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    VALIDATE_NOT_NULL(attributes)
    AutoPtr<ISet> entries;
    attributes->GetEntrySet((ISet**)&entries);

    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IMapEntry* entry;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key, value;
        entry->GetKey((IInterface**)&key);
        entry->GetValue((IInterface**)&value);
        AddAttribute(IAttributedCharacterIteratorAttribute::Probe(key), value, start, end);
    }

    return NOERROR;
}

ECode AttributedString::GetIterator(
    /* [out] */ IAttributedCharacterIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    *iterator = (IAttributedCharacterIterator*) new AttributedIterator(this);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode AttributedString::GetIterator(
    /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
    /* [out] */ IAttributedCharacterIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    *iterator = (IAttributedCharacterIterator*)new AttributedIterator(
            this, attributes, 0, mText.GetLength());
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode AttributedString::GetIterator(
    /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IAttributedCharacterIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    *iterator = (IAttributedCharacterIterator*)new AttributedIterator(
            this, attributes, start, end);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
