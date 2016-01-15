
#ifndef __LIBCORE_ICU_CCOLLATIONELEMENTITERATORICUHELPER_H__
#define __LIBCORE_ICU_CCOLLATIONELEMENTITERATORICUHELPER_H__

#include "_Libcore_ICU_CCollationElementIteratorICUHelper.h"
#include "Singleton.h"


namespace Libcore {
namespace ICU {

CarClass(CCollationElementIteratorICUHelper)
    , public Singleton
    , public ICollationElementIteratorICUHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI PrimaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int32 * value);

    CARAPI SecondaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int32 * value);

    CARAPI TertiaryOrder(
        /* [in] */ Int32 order,
        /* [out] */ Int32 * value);

    CARAPI GetInstance(
        /* [in] */ Int64 collatorAddress,
        /* [in] */ const String& source,
        /* [out] */ ICollationElementIteratorICU ** outiterICU);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CCOLLATIONELEMENTITERATORICUHELPER_H__
