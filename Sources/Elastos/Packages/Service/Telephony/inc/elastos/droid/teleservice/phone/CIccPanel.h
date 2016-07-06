#ifndef  __ELASTOS_DROID_PHONE_CICCPANEL_H__
#define  __ELASTOS_DROID_PHONE_CICCPANEL_H__

#include "_Elastos_Droid_TeleService_Phone_CIccPanel.h"
#include "elastos/droid/teleservice/phone/IccPanel.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::IccPanel;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CIccPanel)
    , public IccPanel
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CICCPANEL_H__