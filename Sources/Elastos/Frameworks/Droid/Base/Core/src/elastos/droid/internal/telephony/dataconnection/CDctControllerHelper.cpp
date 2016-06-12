
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctControllerHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CDctControllerHelper, Singleton, IDctControllerHelper)

CAR_SINGLETON_IMPL(CDctControllerHelper)

ECode CDctControllerHelper::GetInstance(
        /* [out] */ IDctController** result)
{
    return CDctController::GetInstance(result);
}

ECode CDctControllerHelper::MakeDctController(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones,
        /* [in] */ ILooper* looper,
        /* [out] */ IDctController** result)
{
    return CDctController::MakeDctController(phones, looper, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
