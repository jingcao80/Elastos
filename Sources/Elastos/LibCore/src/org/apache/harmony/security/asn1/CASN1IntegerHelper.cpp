
#include "CASN1IntegerHelper.h"
#include "CASN1Integer.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1IntegerHelper::GetInstance(
    /* [out] */ IASN1Integer** instance)
{
    return CASN1Integer::GetInstance(instance);
}

ECode CASN1IntegerHelper::ToIntValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ Int32* intValue)
{
    return CASN1Integer::ToIntValue(decoded, intValue);
}

ECode CASN1IntegerHelper::ToBigIntegerValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ IBigInteger** bigInteger)
{
    return CASN1Integer::ToBigIntegerValue(decoded, bigInteger);
}

ECode CASN1IntegerHelper::FromIntValue(
    /* [in] */ Int32 value,
    /* [out] */ IInterface** fromIntValue)
{
    return CASN1Integer::FromIntValue(value, fromIntValue);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

