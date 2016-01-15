
#include "CCollationElementIteratorHelper.h"
#include "CCollationElementIteratorICUHelper.h"

using Libcore::ICU::ICollationElementIteratorICUHelper;
using Libcore::ICU::CCollationElementIteratorICUHelper;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CCollationElementIteratorHelper, Singleton, ICollationElementIteratorHelper)

CAR_SINGLETON_IMPL(CCollationElementIteratorHelper)

ECode CCollationElementIteratorHelper::PrimaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int32* primaryOrder)
{
    VALIDATE_NOT_NULL(primaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper **)&icuhelper));
    return icuhelper->PrimaryOrder(order,primaryOrder);
}

ECode CCollationElementIteratorHelper::SecondaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int16* secondaryOrder)
{
    VALIDATE_NOT_NULL(secondaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper **)&icuhelper));
    Int32 value(0);
    icuhelper->SecondaryOrder(order,&value);
    *secondaryOrder = (Int16)value;

    return NOERROR;
}

ECode CCollationElementIteratorHelper::TertiaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int16* tertiaryOrder)
{
    VALIDATE_NOT_NULL(tertiaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper **)&icuhelper));
    Int32 value(0);
    icuhelper->TertiaryOrder(order,&value);
    *tertiaryOrder = (Int16)value;

    return NOERROR;
}

} // namespace Text
} // namespace Elastos