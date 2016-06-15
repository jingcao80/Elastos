#include "elastos/droid/provider/CTelephonyMmsRate.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsRate, Singleton, ITelephonyMmsRate, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsRate);

ECode CTelephonyMmsRate::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::Rate::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
