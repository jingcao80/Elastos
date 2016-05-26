#ifndef __ELASTOS_TEXT_STRINGCHARACTERITERATOR_H__
#define __ELASTOS_TEXT_STRINGCHARACTERITERATOR_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::Core::Object;

namespace Elastos {
namespace Text {

class StringCharacterIterator
    : public Object
    , public ICharacterIterator
    , public IStringCharacterIterator
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& value);

    CARAPI constructor(
        /* [in] */ const String& value,
        /* [in] */ Int32 location);

    CARAPI constructor(
        /* [in] */ const String& value,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 location);

    CARAPI Clone(
        /* [out] */ IInterface** copy);

    CARAPI GetCurrent(
        /* [out] */ Char32* currentCharacter);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetFirst(
        /* [out] */ Char32* firstCharacter);

    CARAPI GetBeginIndex(
        /* [out] */ Int32* index);

    CARAPI GetEndIndex(
        /* [out] */ Int32* index);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI GetLast(
        /* [out] */ Char32* endCharacter);

    CARAPI GetNext(
        /* [out] */ Char32* nextCharacter);

    CARAPI GetPrevious(
        /* [out] */ Char32* previousCharacter);

    CARAPI SetIndex(
        /* [in] */ Int32 location,
        /* [out] */ Char32* index);

    CARAPI SetText(
        /* [in] */ const String& value);

protected:
    StringCharacterIterator();

    virtual ~StringCharacterIterator();

    CARAPI CloneImpl(
        /* [in] */ IStringCharacterIterator* obj);

protected:
    String mString;

    Int32 mStart;

    Int32 mEnd;

    Int32 mOffset;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_STRINGCHARACTERITERATOR_H__
