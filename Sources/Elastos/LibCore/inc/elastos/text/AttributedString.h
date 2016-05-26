#ifndef __ELASTOS_TEXT_ATTRIBUTEDSTRING_H__
#define __ELASTOS_TEXT_ATTRIBUTEDSTRING_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"
#define HASH_FOR_TEXT
#include <elastos/corehash.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;
using Elastos::Text::IAttributedCharacterIteratorAttribute;

namespace Elastos {
namespace Text {

typedef HashMap<AutoPtr<IAttributedCharacterIteratorAttribute>, AutoPtr<IInterface> > AttributeObjectMap;
typedef typename AttributeObjectMap::Iterator AttributeObjectMapIterator;

class AttributedString
    : public Object
    , public IAttributedString
{
public:
    class Range
        : public ElRefBase
    {
    public:
        Range(
            /* [in] */ Int32 s,
            /* [in] */ Int32 e,
            /* [in] */ IInterface* v);

        Range& operator = (
            /* [in] */ const Range& other);

    public:
        Int32 mStart;
        Int32 mEnd;
        AutoPtr<IInterface> mValue;
    };

    class AttributedIterator
        : public Object
        , public IAttributedCharacterIterator
        , public ICharacterIterator
        , public ICloneable
    {
    public:
        CAR_INTERFACE_DECL()

        AttributedIterator(
            /* [in] */ AttributedString* attrString);

        AttributedIterator(
            /* [in] */ AttributedString* attrString,
            /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
            /* [in] */ Int32 begin,
            /* [in] */ Int32 end);

        ~AttributedIterator();

        CARAPI Clone(
            /* [out] */ IInterface** copy);

        CARAPI GetCurrent(
            /* [out] */ Char32* value);

        CARAPI GetFirst(
            /* [out] */ Char32* value);

        CARAPI GetBeginIndex(
            /* [out] */ Int32* index);

        CARAPI GetEndIndex(
            /* [out] */ Int32* index);

        CARAPI GetIndex(
            /* [out] */ Int32* index);

        CARAPI GetAllAttributeKeys(
            /* [out] */ ISet** allAttributedKeys);

        CARAPI GetAttribute(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ IInterface** instance);

        CARAPI GetAttributes(
            /* [out] */ IMap** attributes);

        CARAPI GetRunLimit(
            /* [out] */ Int32* index);

        CARAPI GetRunLimit(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ Int32* index);

        CARAPI GetRunLimit(
            /* [in] */ ISet* attributes,
            /* [out] */ Int32* index);

        CARAPI GetRunStart(
            /* [out] */ Int32* index);

        CARAPI GetRunStart(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ Int32* index);

        CARAPI GetRunStart(
            /* [in] */ ISet* attributes,
            /* [out] */ Int32* index);

        CARAPI GetLast(
            /* [out] */ Char32* lastValue);

        CARAPI GetNext(
            /* [out] */ Char32* nextValue);

        CARAPI GetPrevious(
            /* [out] */ Char32* previousValue);

        CARAPI SetIndex(
            /* [in] */ Int32 location,
            /* [out] */ Char32* newChar);

    private:
        CARAPI_(Boolean) InRange(
            /* [in] */ Range* range);

        CARAPI_(Boolean) InRange(
            /* [in] */ List<AutoPtr<Range> >* ranges);

        CARAPI_(AutoPtr<IInterface>) CurrentValue(
            /* [in] */ List<AutoPtr<Range> >* ranges);

        CARAPI_(Int32) RunLimit(
            /* [in] */ List<AutoPtr<Range> >* ranges);

        CARAPI_(Int32) RunStart(
            /* [in] */ List<AutoPtr<Range> >* ranges);

    private:
        Int32 mBegin;
        Int32 mEnd;
        Int32 mOffset;

        AttributedString* mAttrString;

        HashSet<AutoPtr<IAttributedCharacterIteratorAttribute> > mAttributesAllowed;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~AttributedString();

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIterator* iterator);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIterator* iterator,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIterator* iterator,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI constructor(
        /* [in] */ const String& value);

    CARAPI constructor(
        /* [in] */ const String& value,
        /* [in] */ IMap * attributes);

    CARAPI AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value);

    CARAPI AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI AddAttributes(
        /* [in] */ IMap* attributes,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI GetIterator(
        /* [out] */ IAttributedCharacterIterator** iterator);

    CARAPI GetIterator(
        /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
        /* [out] */ IAttributedCharacterIterator** iterator);

    CARAPI GetIterator(
        /* [in] */ ArrayOf<IAttributedCharacterIteratorAttribute*>* attributes,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ IAttributedCharacterIterator** iterator);

private:
    CARAPI constructor(
        /* [in] */ IAttributedCharacterIterator* iterator,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISet* attributes);

public:
    String mText;

    typedef HashMap<AutoPtr<IAttributedCharacterIteratorAttribute>, AutoPtr<List<AutoPtr<Range> > > > AttributeRangeMap;
    typedef typename AttributeRangeMap::Iterator AttributeRangeMapIterator;

    AttributeRangeMap mAttributeMap;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_ATTRIBUTEDSTRING_H__
