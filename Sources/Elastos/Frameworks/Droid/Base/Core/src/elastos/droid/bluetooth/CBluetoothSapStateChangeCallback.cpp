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

#include "elastos/droid/bluetooth/CBluetoothSapStateChangeCallback.h"
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

CAR_INTERFACE_IMPL_2(CBluetoothSapStateChangeCallback, Object, IIBluetoothStateChangeCallback, IBinder)

CAR_OBJECT_IMPL(CBluetoothSapStateChangeCallback)

CBluetoothSapStateChangeCallback::CBluetoothSapStateChangeCallback()
{
}

ECode CBluetoothSapStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    if (BluetoothSap::DBG) Logger::D(BluetoothSap::TAG, "onBluetoothStateChange: on=%d", on);
    if (on) {
        if (mHost->mSapService == NULL) {
            if (BluetoothSap::VDBG) Logger::D(BluetoothSap::TAG, "onBluetoothStateChange call bindService");
            Boolean bind;
            ECode ec;
            if(FAILED(ec = mHost->DoBind(&bind))) {
                Logger::D(BluetoothSap::TAG, "onBluetoothStateChange: could not bind to SAP service");
            }
            Logger::D(BluetoothSap::TAG, "BluetoothSap(), bindService called");
        }
    }
    else {
        if (BluetoothSap::VDBG) Logger::D(BluetoothSap::TAG, "Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mSapService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothSap::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothSapStateChangeCallback::constructor(
    /* [in] */ IBluetoothSap* host)
{
    mHost = (BluetoothSap*)host;
    return NOERROR;
}

ECode CBluetoothSapStateChangeCallback::ToString(
    /* [out] */ String* str)
{
    *str = "CBluetoothSapStateChangeCallback";
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
