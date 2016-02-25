
#ifndef __ELASTOS_DROID_SERVER_HDMI_CHDMICONTROLSERVICEBINDERSERVICE_H__
#define __ELASTOS_DROID_SERVER_HDMI_CHDMICONTROLSERVICEBINDERSERVICE_H__

#include "_Elastos_Droid_Server_Hdmi_CHdmiControlServiceBinderService.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CarClass(CHdmiControlServiceBinderService)
    , public HdmiControlService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CHDMICONTROLSERVICEBINDERSERVICE_H__
