#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGECALLSETTINGS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGECALLSETTINGS_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatCmdMessageCallSettings.h"
#include "elastos/droid/internal/telephony/cat/CatCmdMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatCmdMessageCallSettings)
    , public CatCmdMessage::CallSettings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGECALLSETTINGS_H__
