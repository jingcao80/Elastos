
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_CMEDIA_PROJECTION_CALLBACK_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_CMEDIA_PROJECTION_CALLBACK_H__

#include "_Elastos_Droid_Server_Display_CMediaProjectionCallback.h"
#include "elastos/droid/server/display/VirtualDisplayAdapter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

CarClass(CMediaProjectionCallback)
    , public VirtualDisplayAdapter::MediaProjectionCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_DISPLAY_CMEDIA_PROJECTION_CALLBACK_H__
