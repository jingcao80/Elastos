
#include "elastos/droid/contacts/common/util/CEmptyService.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

CAR_OBJECT_IMPL(CEmptyService)

ECode CEmptyService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = NULL;
    return NOERROR;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
