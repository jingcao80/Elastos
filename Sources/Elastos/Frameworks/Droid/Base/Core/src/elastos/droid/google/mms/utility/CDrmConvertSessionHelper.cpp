#include "elastos/droid/google/mms/utility/CDrmConvertSessionHelper.h"
#include "elastos/droid/google/mms/utility/DrmConvertSession.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CAR_INTERFACE_IMPL(CDrmConvertSessionHelper, Singleton, IDrmConvertSessionHelper)
CAR_SINGLETON_IMPL(CDrmConvertSessionHelper)

ECode CDrmConvertSessionHelper::Open(
    /* [in] */ IContext* context,
    /* [in] */ const String& mimeType,
    /* [out] */ IDrmConvertSession** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IDrmConvertSession> dcs = DrmConvertSession::Open(context, mimeType);
    *result = dcs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
