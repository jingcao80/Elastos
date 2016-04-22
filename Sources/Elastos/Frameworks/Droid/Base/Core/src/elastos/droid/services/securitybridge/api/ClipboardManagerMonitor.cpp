#include "elastos/droid/services/securitybridge/api/ClipboardManagerMonitor.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

ClipboardManagerMonitor::ClipboardManagerMonitor()
{}

ClipboardManagerMonitor::~ClipboardManagerMonitor()
{}

ECode ClipboardManagerMonitor::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(ClipboardManagerMonitor, Object, IClipboardManagerMonitor)

ECode ClipboardManagerMonitor::ApprovePasteRequest(
    /* [in] */ Int32 appID,
    /* [in] */ IClipData* clipData,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode ClipboardManagerMonitor::NotifyCopy(
    /* [in] */ Int32 appID,
    /* [in] */ IClipData* clipData)
{
    return NOERROR;
}

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos