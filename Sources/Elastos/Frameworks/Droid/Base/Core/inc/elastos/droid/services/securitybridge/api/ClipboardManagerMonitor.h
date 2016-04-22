#ifndef _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__
#define _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.SecurityBridge.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IClipData;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

class ClipboardManagerMonitor
    : public Object
    , public IClipboardManagerMonitor
{
public:
    ClipboardManagerMonitor();

    virtual ~ClipboardManagerMonitor();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CARAPI ApprovePasteRequest(
        /* [in] */ Int32 appID,
        /* [in] */ IClipData* clipData,
        /* [out] */ Boolean* result);

    CARAPI NotifyCopy(
        /* [in] */ Int32 appID,
        /* [in] */ IClipData* clipData);
};

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__