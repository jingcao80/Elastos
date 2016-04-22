#ifndef _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CCLIPBOARDMANAGERMONITOR_H__
#define _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CCLIPBOARDMANAGERMONITOR_H__

#include "_Elastos_Droid_Services_SecurityBridge_Api_CClipboardManagerMonitor.h"
#include "elastos/droid/services/securitybridge/api/ClipboardManagerMonitor.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

CarClass(CClipboardManagerMonitor), public ClipboardManagerMonitor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CCLIPBOARDMANAGERMONITOR_H__