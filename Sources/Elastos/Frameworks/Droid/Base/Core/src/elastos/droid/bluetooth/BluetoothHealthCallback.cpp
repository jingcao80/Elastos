//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/bluetooth/BluetoothHealthCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String BluetoothHealthCallback::TAG("BluetoothHealthCallback");

CAR_INTERFACE_IMPL(BluetoothHealthCallback, Object, IBluetoothHealthCallback);

BluetoothHealthCallback::BluetoothHealthCallback()
{
}

BluetoothHealthCallback::~BluetoothHealthCallback()
{
}

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
