
#include "org/alljoyn/bus/alljoyn/CDaemonInit.h"
#include "org/alljoyn/bus/alljoyn/DaemonInit.h"

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

CAR_SINGLETON_IMPL(CDaemonInit)

CAR_INTERFACE_IMPL(CDaemonInit, Singleton, IDaemonInit)

ECode CDaemonInit::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IContext> ctx = DaemonInit::GetContext();
    *context = ctx;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CDaemonInit::PrepareDaemon(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DaemonInit::PrepareDaemon(context);
    return NOERROR;
}

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org
