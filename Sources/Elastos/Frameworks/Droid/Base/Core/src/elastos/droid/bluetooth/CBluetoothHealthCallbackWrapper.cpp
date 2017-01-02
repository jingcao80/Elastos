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

#include "elastos/droid/bluetooth/CBluetoothHealthCallbackWrapper.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothHealthCallbackWrapper, Object, IIBluetoothHealthCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothHealthCallbackWrapper);

CBluetoothHealthCallbackWrapper::CBluetoothHealthCallbackWrapper()
{
}

ECode CBluetoothHealthCallbackWrapper::OnHealthAppConfigurationStatusChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ Int32 status)
{
    return mCallback->OnHealthAppConfigurationStatusChange(config, status);
}

ECode CBluetoothHealthCallbackWrapper::OnHealthChannelStateChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 channelId)
{
    return mCallback->OnHealthChannelStateChange(config, device, prevState, newState,
            fd, channelId);
}

ECode CBluetoothHealthCallbackWrapper::constructor(
    /* [in] */ IBluetoothHealthCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
