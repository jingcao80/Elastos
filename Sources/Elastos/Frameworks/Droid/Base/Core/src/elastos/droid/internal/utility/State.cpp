
#include "elastos/droid/internal/utility/State.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(State, Object, IState)

ECode State::Enter()
{
    return NOERROR;
}

ECode State::Exit()
{
    return NOERROR;
}

ECode State::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode State::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = GetName();
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
