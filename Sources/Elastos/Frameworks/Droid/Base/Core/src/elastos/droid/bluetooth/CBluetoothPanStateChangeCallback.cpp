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

#include "elastos/droid/bluetooth/CBluetoothPanStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothPanStateChangeCallback, Object, IIBluetoothStateChangeCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothPanStateChangeCallback);

CBluetoothPanStateChangeCallback::CBluetoothPanStateChangeCallback()
{
}

ECode CBluetoothPanStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    //Handle enable request to bind again.
    if (on) {
        Logger::D(BluetoothPan::TAG, "onBluetoothStateChange(on) call bindService");
        //AutoPtr<IIntent> intent;
        //CIntent::New(String("IBluetoothPan"), (IIntent**)&intent);
        //Boolean result;
        //if (mHost->mContext->BindService(intent, mHost->mConnection, 0, &result), !result) {
        //    Logger::E(BluetoothPan::TAG, "Could not bind to Bluetooth HID Service");
        //}
        Boolean bind;
        mHost->DoBind(&bind);
        Logger::D(BluetoothPan::TAG, "BluetoothPan(), bindService called");
    }
    else {
        if (BluetoothPan::VDBG) Logger::D(BluetoothPan::TAG,"Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mPanService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothPan::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(BluetoothPan::TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothPanStateChangeCallback::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothPan*)(IBluetoothPan::Probe(host));
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
