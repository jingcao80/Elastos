
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CVoicemailContractVoicemails.h"
#include "elastos/droid/provider/VoicemailContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CVoicemailContractVoicemails)

CAR_INTERFACE_IMPL(CVoicemailContractVoicemails, Singleton, IVoicemails)

ECode CVoicemailContractVoicemails::BuildSourceUri(
    /* [in] */ const String& packageName,
    /* [out] */ IUri** uri)
{
    return VoicemailContract::Voicemails::BuildSourceUri(packageName, uri);
}

ECode CVoicemailContractVoicemails::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = VoicemailContract::Voicemails::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
