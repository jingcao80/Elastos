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

#include "elastos/droid/hardware/hdmi/HdmiClient.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiClient::MyListener, Object, IIHdmiVendorCommandListener, IBinder)

HdmiClient::MyListener::MyListener(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
    : mListener(listener)
{
}

HdmiClient::MyListener::OnReceived(
    /* [in] */ Int32 srcAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    return mListener->OnReceived(srcAddress, params, hasVendorId);
}

ECode HdmiClient::MyListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

const String HdmiClient::TAG("HdmiClient");

CAR_INTERFACE_IMPL(HdmiClient, Object, IHdmiClient)

HdmiClient::HdmiClient()
{
}

ECode HdmiClient::constructor()
{
    return NOERROR;
}

ECode HdmiClient::constructor(
    /* [in] */ IIHdmiControlService* service)
{
    mService = service;
    return NOERROR;
}

ECode HdmiClient::GetActiveSource(
    /* [out] */ IHdmiDeviceInfo** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    //try {
    ECode ec = mService->GetActiveSource(source);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "getActiveSource threw exception %d", ec);
        *source = NULL;
    }
    //}
    return NOERROR;
}

ECode HdmiClient::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->SendKeyEvent(type, keyCode, isPressed);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "sendKeyEvent threw exception %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiClient::SendVendorCommand(
    /* [in] */ Int32 targetAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->SendVendorCommand(type, targetAddress, params, hasVendorId);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "failed to send vendor command: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiClient::AddVendorCommandListener(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
{
    if (listener == NULL) {
        //throw new IllegalArgumentException("listener cannot be null");
        Logger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->AddVendorCommandListener(GetListenerWrapper(listener), type);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "failed to add vendor command listener: %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiVendorCommandListener> HdmiClient::GetListenerWrapper(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
{
    AutoPtr<IIHdmiVendorCommandListener> lisen = new MyListener(listener);
    return lisen;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
