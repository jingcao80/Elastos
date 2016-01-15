
#ifndef __LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__
#define __LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__

#include <Object.h>
#include "_Libcore_ICU_CRuleBasedCollatorICU.h"

using Elastos::Text::ICollationKey;
using Elastos::Text::ICharacterIterator;
using Elastos::Core::ICloneable;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CRuleBasedCollatorICU)
    , public Object
    , public ICloneable
    , public IRuleBasedCollatorICU
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRuleBasedCollatorICU();

    ~CRuleBasedCollatorICU();

    CARAPI constructor(
        /* [in] */ const String& rules);

    CARAPI constructor(
        /* [in] */ ILocale * locale);

    CARAPI Clone(
        /* [out] */ IInterface ** outruleicu);

    CARAPI Compare(
        /* [in] */ const String& source,
        /* [in] */ const String& target,
        /* [out] */ Int32 * value);

    CARAPI GetDecomposition(
        /* [out] */ Int32 * value);

    CARAPI SetDecomposition(
        /* [in] */ Int32 mode);

    CARAPI GetStrength(
        /* [out] */ Int32 * value);

    CARAPI SetStrength(
        /* [in] */ Int32 strength);

    CARAPI SetAttribute(
        /* [in] */ Int32 type,
        /* [in] */ Int32 value);

    CARAPI GetAttribute(
        /* [in] */ Int32 type,
        /* [out] */ Int32 * value);

    CARAPI GetCollationKey(
        /* [in] */ const String& source,
        /* [out] */ ICollationKey ** outkey);

    CARAPI GetRules(
        /* [out] */ String * str);

    CARAPI GetCollationElementIterator(
        /* [in] */ const String& source,
        /* [out] */ ICollationElementIteratorICU ** outicu);

    CARAPI GetCollationElementIterator(
        /* [in] */ ICharacterIterator * it,
        /* [out] */ ICollationElementIteratorICU ** outicu);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI Equals(
        /* [in] */ const String& source,
        /* [in] */ const String& target,
        /* [out] */ Boolean * value);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

private:
    CRuleBasedCollatorICU(
        /* [in] */ Int64 address);

    CARAPI_(String) CharacterIteratorToString(
        /* [in] */ ICharacterIterator * it);

private:
    Int64 mAddress;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__
