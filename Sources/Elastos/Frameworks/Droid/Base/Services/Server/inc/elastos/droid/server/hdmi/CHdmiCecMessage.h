
#ifndef __ELASTOS_DROID_SERVER_HDMI_CHDMICECMESSAGE_H__
#define __ELASTOS_DROID_SERVER_HDMI_CHDMICECMESSAGE_H__

#include "_Elastos_Droid_Server_Hdmi_CHdmiCecMessage.h"
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * A class to encapsulate HDMI-CEC message used for the devices connected via
 * HDMI cable to communicate with one another. A message is defined by its
 * source and destination address, command (or opcode), and optional parameters.
 */
CarClass(CHdmiCecMessage)
    , public HdmiCecMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CHDMICECMESSAGE_H__
