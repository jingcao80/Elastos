
#ifndef __ELASTOS_TEXT_CFIELDPOSITION_H__
#define __ELASTOS_TEXT_CFIELDPOSITION_H__

#include "_Elastos_Text_CFieldPosition.h"
#include "Object.h"

namespace Elastos {
namespace Text {

CarClass(CFieldPosition)
    , public Object
    , public IFieldPosition
{
public:
    CAR_INTERFACE_DECL()

    CFieldPosition();

    CARAPI constructor(
        /* [in] */ Int32 field);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ Int32 field);

    CARAPI Clear();

    CARAPI GetBeginIndex(
        /* [out] */ Int32* index);

    CARAPI GetEndIndex(
        /* [out] */ Int32* index);

    CARAPI GetField(
        /* [out] */ Int32* field);

    CARAPI GetFieldAttribute(
        /* [out] */ IAttributedCharacterIteratorAttribute** attribute);

    CARAPI SetBeginIndex(
        /* [in] */ Int32 index);

    CARAPI SetEndIndex(
        /* [in] */ Int32 index);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI GetHashCode(
    /* [out] */ Int32 * value);

    CARAPI ToString(
    /* [out] */ String * str);

public:
    Int32 mMyField;
    Int32 mBeginIndex;
    Int32 mEndIndex;

    AutoPtr<IAttributedCharacterIteratorAttribute> mMyAttribute;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CFIELDPOSITION_H__
