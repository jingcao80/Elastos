#include "elastos/droid/provider/CTelephonyCellBroadcasts.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyCellBroadcasts, Singleton, ITelephonyCellBroadcasts, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyCellBroadcasts);

ECode CTelephonyCellBroadcasts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::CellBroadcasts::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyCellBroadcasts::GetQUERY_COLUMNS(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::CellBroadcasts::QUERY_COLUMNS;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


} // namespace Provider
} // namespace Droid
} // namespace Elastos
