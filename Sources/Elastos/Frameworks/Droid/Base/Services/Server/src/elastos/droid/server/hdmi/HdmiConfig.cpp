
#include "elastos/droid/server/hdmi/HdmiConfig.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const Int32 HdmiConfig::TIMEOUT_MS = 2000;
const Int32 HdmiConfig::IRT_MS = 300;
const Int32 HdmiConfig::DEVICE_POLLING_RETRY = 1;
const Int32 HdmiConfig::HOTPLUG_DETECTION_RETRY = 1;
const Int32 HdmiConfig::ADDRESS_ALLOCATION_RETRY = 3;
const Int32 HdmiConfig::RETRANSMISSION_COUNT = 1;
const Boolean HdmiConfig::HIDE_DEVICES_BEHIND_LEGACY_SWITCH = TRUE;

HdmiConfig::HdmiConfig()
{}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
