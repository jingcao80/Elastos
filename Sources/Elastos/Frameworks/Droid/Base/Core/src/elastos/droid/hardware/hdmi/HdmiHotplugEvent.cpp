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

#include "elastos/droid/hardware/hdmi/HdmiHotplugEvent.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiHotplugEvent, Object, IHdmiHotplugEvent, IParcelable)

HdmiHotplugEvent::HdmiHotplugEvent()
    : mPort(0)
    , mConnected(FALSE)
{
}

ECode HdmiHotplugEvent::constructor()
{
    return NOERROR;
}

ECode HdmiHotplugEvent::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Boolean connected)
{
    mPort = port;
    mConnected = connected;
    return NOERROR;
}

ECode HdmiHotplugEvent::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mPort;
    return NOERROR;
}

ECode HdmiHotplugEvent::IsConnected(
    /* [out] */ Boolean* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mConnected;
    return NOERROR;
}

ECode HdmiHotplugEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mPort);
    source->ReadBoolean(&mConnected);

    return NOERROR;
}

ECode HdmiHotplugEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mPort);
    dest->WriteBoolean(mConnected);

    return NOERROR;
}


} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
