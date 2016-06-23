#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandlerHelper.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmCellBroadcastHandlerHelper, Singleton, IGsmCellBroadcastHandlerHelper)

CAR_SINGLETON_IMPL(CGsmCellBroadcastHandlerHelper)

ECode CGsmCellBroadcastHandlerHelper::MakeGsmCellBroadcastHandler(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone,
    /* [out] */ IGsmCellBroadcastHandler** result)
{
    VALIDATE_NOT_NULL(result)
    *result = CGsmCellBroadcastHandler::MakeGsmCellBroadcastHandler(context, phone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos