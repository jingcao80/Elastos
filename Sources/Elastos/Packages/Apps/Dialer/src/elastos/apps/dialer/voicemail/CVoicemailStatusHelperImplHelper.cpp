
#include "voicemail/CVoicemailStatusHelperImplHelper.h"
#include "voicemail/VoicemailStatusHelperImpl.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

CAR_INTERFACE_IMPL(CVoicemailStatusHelperImplHelper, Singleton, IVoicemailStatusHelperImplHelper);

CAR_SINGLETON_IMPL(CVoicemailStatusHelperImplHelper);

ECode CVoicemailStatusHelperImplHelper::GetPROJECTION(
    /* [out, callee] */ ArrayOf<String>** protection)
{
    VALIDATE_NOT_NULL(protection);
    *protection = VoicemailStatusHelperImpl::PROJECTION;
    REFCOUNT_ADD(*protection);
    return NOERROR;
}

} // Voicemail
} // Dialer
} // Apps
} // Elastos
