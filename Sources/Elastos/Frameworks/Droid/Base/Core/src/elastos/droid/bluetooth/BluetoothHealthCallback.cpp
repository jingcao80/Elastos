
#include "BluetoothHealthCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String BluetoothHealthCallback::TAG("BluetoothHealthCallback");

ECode BluetoothHealthCallback::OnHealthAppConfigurationStatusChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ Int32 status)
{
    Logger::D(TAG, "onHealthAppConfigurationStatusChange: %pStatus: %d", config, status);
    return NOERROR;
}

ECode BluetoothHealthCallback::OnHealthChannelStateChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 channelId)
{
    Logger::D(TAG, "onHealthChannelStateChange: %pDevice: %pprevState:%dnewState:%dParcelFd:%pChannelId:%,"
            , config, device, prevState, newState, fd, channelId);
    return NOERROR;
}

}
}
}

