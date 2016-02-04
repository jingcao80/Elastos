
#include "elastos/droid/server/pm/CUserStopUserCallback.h"

using Elastos::Droid::App::EIID_IStopUserCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CUserStopUserCallback, Object, IStopUserCallback, IBinder)

CAR_OBJECT_IMPL(CUserStopUserCallback)

CARAPI CUserStopUserCallback::constructor(
    /* [in] */ IIUserManager* um)
{
    mUm = (CUserManagerService*)um;
    return NOERROR;
}

ECode CUserStopUserCallback::UserStopped(
    /* [in] */ Int32 userId)
{
    mUm->FinishRemoveUser(userId);
    return NOERROR;
}

ECode CUserStopUserCallback::UserStopAborted(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode CUserStopUserCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
