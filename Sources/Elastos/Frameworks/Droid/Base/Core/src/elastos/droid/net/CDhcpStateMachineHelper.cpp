
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CDhcpStateMachineHelper.h"
#include "elastos/droid/net/DhcpStateMachine.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CDhcpStateMachineHelper, Singleton, IDhcpStateMachineHelper)

CAR_SINGLETON_IMPL(CDhcpStateMachineHelper)

ECode CDhcpStateMachineHelper::MakeDhcpStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ IStateMachine* controller,
    /* [in] */ const String& intf,
    /* [out] */ IDhcpStateMachine** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IDhcpStateMachine> temp = DhcpStateMachine::MakeDhcpStateMachine(context, controller, intf);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
