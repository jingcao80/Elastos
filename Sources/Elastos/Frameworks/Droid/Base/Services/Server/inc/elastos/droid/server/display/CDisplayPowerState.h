#ifndef __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYPOWERSTATE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYPOWERSTATE_H__

#include "_Elastos_Droid_Server_Display_CDisplayPowerState.h"
#include "elastos/droid/server/display/DisplayPowerState.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

CarClass(CDisplayPowerState)
    , public DisplayPowerState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYPOWERSTATE_H__
