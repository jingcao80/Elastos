
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CVoicemailContractStatus.h"
#include "elastos/droid/provider/VoicemailContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CVoicemailContractStatus)

CAR_INTERFACE_IMPL(CVoicemailContractStatus, Singleton, IVoicemailContractStatus)

ECode CVoicemailContractStatus::BuildSourceUri(
    /* [in] */ const String& packageName,
    /* [out] */ IUri** uri)
{
    return VoicemailContract::Status::BuildSourceUri(packageName, uri);
}

ECode CVoicemailContractStatus::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = VoicemailContract::Status::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
