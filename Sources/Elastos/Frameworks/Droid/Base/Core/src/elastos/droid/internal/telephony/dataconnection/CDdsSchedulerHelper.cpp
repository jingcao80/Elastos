
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsScheduler.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsSchedulerHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CDdsSchedulerHelper, Singleton, IDdsSchedulerHelper)

CAR_SINGLETON_IMPL(CDdsSchedulerHelper)

ECode CDdsSchedulerHelper::GetInstance(
    /* [out] */ IDdsScheduler** result)
{
    return CDdsScheduler::GetInstance(result);
}

ECode CDdsSchedulerHelper::Init()
{
    return CDdsScheduler::Init();
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
