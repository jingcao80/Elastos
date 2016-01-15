#ifndef __LIBCORE_ICU_COLLATIONKEYICU_H__
#define __LIBCORE_ICU_COLLATIONKEYICU_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "text/CollationKey.h"

using Elastos::Text::CollationKey;
using Elastos::Text::ICollationKey;

namespace Libcore {
namespace ICU {

class CollationKeyICU
    : public CollationKey
    , public ICollationKeyICU
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& source,
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI CompareTo(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

private:
    /**
     * The key.
     */
    AutoPtr<ArrayOf<Byte> > mBytes;

    Int32 mHashCode;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_COLLATIONKEYICU_H__
