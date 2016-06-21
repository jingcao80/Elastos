#include "elastos/droid/internal/telephony/cdma/CCdmaInboundSmsHandlerHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaInboundSmsHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaInboundSmsHandlerHelper, Singleton, ICdmaInboundSmsHandlerHelper)
CAR_SINGLETON_IMPL(CCdmaInboundSmsHandlerHelper)


ECode CCdmaInboundSmsHandlerHelper::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher,
    /* [out] */ ICdmaInboundSmsHandler** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaInboundSmsHandler> cish = CdmaInboundSmsHandler::MakeInboundSmsHandler(context, storageMonitor, phone, smsDispatcher);
    *result = cish;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
