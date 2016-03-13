
#ifndef __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMAVR_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMAVR_H__

#include "_Elastos_Droid_Server_Hdmi_CSystemAudioActionFromAvr.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

// Seq #33
CarClass(CSystemAudioActionFromAvr)
    , public SystemAudioActionFromAvr
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSYSTEMAUDIOACTIONFROMAVR_H__
