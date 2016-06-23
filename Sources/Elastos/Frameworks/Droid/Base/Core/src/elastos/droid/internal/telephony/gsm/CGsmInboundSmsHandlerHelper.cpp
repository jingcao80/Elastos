#include "elastos/droid/internal/telephony/gsm/CGsmInboundSmsHandlerHelper.h"
#include "elastos/droid/internal/telephony/gsm/GsmInboundSmsHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmInboundSmsHandlerHelper, Singleton, IGsmInboundSmsHandlerHelper)

CAR_SINGLETON_IMPL(CGsmInboundSmsHandlerHelper)

ECode CGsmInboundSmsHandlerHelper::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [out] */ IGsmInboundSmsHandler** result)
{
    return GsmInboundSmsHandler::MakeInboundSmsHandler(context, storageMonitor, phone, result);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos