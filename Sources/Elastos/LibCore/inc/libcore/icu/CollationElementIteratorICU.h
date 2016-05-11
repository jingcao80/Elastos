
#ifndef __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__
#define __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Text::ICharacterIterator;

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_CollationElementIteratorICU;

class CollationElementIteratorICU
    : public Object
    , public ICollationElementIteratorICU
{
public:
    CAR_INTERFACE_DECL()

    CollationElementIteratorICU();

    ~CollationElementIteratorICU();

    CARAPI Reset();

    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    CARAPI GetPrevious(
        /* [out] */ Int32* previousValue);

    CARAPI GetMaxExpansion(
        /* [in] */ Int32 order,
        /* [out] */ Int32* maxExpansion);

    CARAPI SetText(
        /* [in] */ const String& source);

    CARAPI SetText(
        /* [in] */ ICharacterIterator* source);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI SetOffset(
        /* [in] */ Int32 offset);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    static CARAPI_(Int32) PrimaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(Int32) SecondaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(Int32) TertiaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(AutoPtr<ICollationElementIteratorICU>) GetInstance(
        /* [in] */ Int64 collatorAddress,
        /* [in] */ const String& source);

private:
    CollationElementIteratorICU(
        /* [in] */ Int64 collelemiteratoraddress);

private:
    Int64 mAddress;

    const static Int32 PRIMARY_ORDER_MASK_;

    const static Int32 SECONDARY_ORDER_MASK_;

    const static Int32 TERTIARY_ORDER_MASK_;

    const static Int32 PRIMARY_ORDER_SHIFT_;

    const static Int32 SECONDARY_ORDER_SHIFT_;

    const static Int32 UNSIGNED_16_BIT_MASK_;
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__
