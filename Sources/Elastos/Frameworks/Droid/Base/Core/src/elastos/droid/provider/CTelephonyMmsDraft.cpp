#include "elastos/droid/provider/CTelephonyMmsDraft.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsDraft, Singleton, ITelephonyMmsDraft, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsDraft);

ECode CTelephonyMmsDraft::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::Draft::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
