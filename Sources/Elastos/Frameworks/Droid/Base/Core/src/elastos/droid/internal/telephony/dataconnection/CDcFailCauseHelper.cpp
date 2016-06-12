
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcFailCauseHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CDcFailCauseHelper, Singleton, IDcFailCauseHelper)

CAR_SINGLETON_IMPL(CDcFailCauseHelper)

ECode CDcFailCauseHelper::FromInt32(
    /* [in] */ Int32 errorCode,
    /* [out] */ IDcFailCause** result)
{
    return CDcFailCause::FromInt32(errorCode, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
