
#ifndef __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__
#define __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "_Elastos_Text_CCollationElementIterator.h"
#include "Object.h"

using Libcore::ICU::ICollationElementIteratorICU;
using Elastos::Text::ICharacterIterator;

namespace Elastos {
namespace Text {

CarClass(CCollationElementIterator)
    , public Object
    , public ICollationElementIterator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICollationElementIteratorICU* iterator);

    CARAPI GetMaxExpansion(
        /* [in] */ Int32 order,
        /* [out] */ Int32* maxExpansion);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    CARAPI GetPrevious(
        /* [out] */ Int32* previousValue);

    CARAPI Reset();

    CARAPI SetOffset(
        /* [in] */ Int32 newOffset);

    CARAPI SetText(
        /* [in] */ ICharacterIterator* source);

    CARAPI SetText(
        /* [in] */ const String& source);

private:
    AutoPtr<ICollationElementIteratorICU> mIcuIterator;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__
