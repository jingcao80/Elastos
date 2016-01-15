#include "elastos/droid/utility/CRationalHelper.h"
#include "elastos/droid/utility/CRational.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CRationalHelper, Singleton, IRationalHelper)

CAR_SINGLETON_IMPL(CRationalHelper)

ECode CRationalHelper::Gcd(
    /* [in] */ Int32 numerator,
    /* [in] */ Int32 denominator,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = CRational::Gcd(numerator, denominator);
    return NOERROR;
}

ECode CRationalHelper::ParseRational(
    /* [in] */ const String& string,
    /* [out] */ IRational** outration)
{
    return CRational::ParseRational(string, outration);;
}


}
}
}
