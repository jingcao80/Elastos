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

#include "elastos/droid/hardware/hdmi/HdmiPlaybackClient.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiPlaybackClient::MyCallback, Object,
        IIHdmiControlCallback, IBinder)

HdmiPlaybackClient::MyCallback::MyCallback(
    /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback)
    : mCallback(_callback)
{
}

ECode HdmiPlaybackClient::MyCallback::OnComplete(
    /* [in] */ Int32 result)
{
    return mCallback->OnComplete(result);
}

ECode HdmiPlaybackClient::MyCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

CAR_INTERFACE_IMPL_2(HdmiPlaybackClient::MyCallback2, Object,
        IIHdmiControlCallback, IBinder)

HdmiPlaybackClient::MyCallback2::MyCallback2(
    /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback)
    : mCallback(_callback)
{
}

ECode HdmiPlaybackClient::MyCallback2::OnComplete(
    /* [in] */ Int32 result)
{
    return mCallback->OnComplete(result);
}

ECode HdmiPlaybackClient::MyCallback2::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

const String HdmiPlaybackClient::TAG("HdmiPlaybackClient");

CAR_INTERFACE_IMPL(HdmiPlaybackClient, HdmiClient, IHdmiPlaybackClient)

HdmiPlaybackClient::HdmiPlaybackClient()
{
}

ECode HdmiPlaybackClient::constructor()
{
    return NOERROR;
}

ECode HdmiPlaybackClient::constructor(
    /* [in] */ IIHdmiControlService* service)
{
    return HdmiClient::constructor(service);
}

ECode HdmiPlaybackClient::OneTouchPlay(
    /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback)
{
    // TODO: Use PendingResult.
    //try {
    ECode ec = mService->OneTouchPlay(GetCallbackWrapper(_callback));
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "oneTouchPlay threw exception %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiPlaybackClient::GetDeviceType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = IHdmiDeviceInfo::DEVICE_PLAYBACK;
    return NOERROR;
}

ECode HdmiPlaybackClient::QueryDisplayStatus(
    /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback)
{
    //try {
    ECode ec = mService->QueryDisplayStatus(GetCallbackWrapper(_callback));
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "queryDisplayStatus threw exception %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiControlCallback> HdmiPlaybackClient::GetCallbackWrapper(
    /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback)
{
    AutoPtr<IIHdmiControlCallback> res = new MyCallback(_callback);
    return res;
}

AutoPtr<IIHdmiControlCallback> HdmiPlaybackClient::GetCallbackWrapper(
    /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback)
{
    AutoPtr<IIHdmiControlCallback> res = new MyCallback2(_callback);
    return res;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
