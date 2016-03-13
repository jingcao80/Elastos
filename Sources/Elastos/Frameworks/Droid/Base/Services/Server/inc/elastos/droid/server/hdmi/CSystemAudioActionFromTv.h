
#ifndef __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMTV_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMTV_H__

#include "_Elastos_Droid_Server_Hdmi_CSystemAudioActionFromTv.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles System Audio initiated by TV devices.
 */
CarClass(CSystemAudioActionFromTv)
    , public SystemAudioActionFromTv
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMTV_H__
