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

#include "elastos/droid/bluetooth/CBluetoothA2dpStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothA2dpStateChangeCallback, Object, IIBluetoothStateChangeCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothA2dpStateChangeCallback);

CBluetoothA2dpStateChangeCallback::CBluetoothA2dpStateChangeCallback()
{
}

ECode CBluetoothA2dpStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    if (BluetoothA2dp::DBG) Logger::D(BluetoothA2dp::TAG, "onBluetoothStateChange: up=%d", up);
    if (!up) {
        if (BluetoothA2dp::VDBG) Logger::D(BluetoothA2dp::TAG, "Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothA2dp::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    else {
        AutoLock lock(mHost->mConnectionLock);
        // try {
        if (mHost->mService == NULL) {
            if (BluetoothA2dp::VDBG) Logger::D(BluetoothA2dp::TAG, "Binding service...");
            //AutoPtr<IIntent> intent;
            //CIntent::New(String("IBluetoothHeadset")/*IBluetoothHeadset.class.getName()*/, (IIntent**)&intent);
            //Boolean result;
            //ECode ec = mHost->mContext->BindService(intent, mHost->mConnection, 0, &result);
            //if (FAILED(ec)) {
            //    Logger::E(BluetoothA2dp::TAG, "0x%08x", ec);
            //}
            //else if (!result) {
            //    Logger::E(BluetoothA2dp::TAG, "Could not bind to Bluetooth Headset Service");
            //}
            Boolean bindResult;
            mHost->DoBind(&bindResult);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothA2dpStateChangeCallback::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothA2dp*)(IBluetoothA2dp::Probe(host));
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
