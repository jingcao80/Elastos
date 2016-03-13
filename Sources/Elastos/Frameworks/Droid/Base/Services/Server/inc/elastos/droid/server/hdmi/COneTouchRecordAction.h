
#ifndef __ELASTOS_DROID_SERVER_HDMI_CONETOUCHRECORDACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CONETOUCHRECORDACTION_H__

#include "_Elastos_Droid_Server_Hdmi_COneTouchRecordAction.h"
#include "elastos/droid/server/hdmi/OneTouchRecordAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs one touch record.
 */
CarClass(COneTouchRecordAction)
    , public OneTouchRecordAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CONETOUCHRECORDACTION_H__
