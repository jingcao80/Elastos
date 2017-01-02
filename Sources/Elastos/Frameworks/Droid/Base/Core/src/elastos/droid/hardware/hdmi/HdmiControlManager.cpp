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

#include "elastos/droid/hardware/hdmi/HdmiControlManager.h"
#include "elastos/droid/hardware/hdmi/HdmiTvClient.h"
#include "elastos/droid/hardware/hdmi/HdmiPlaybackClient.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiControlManager::MyListener, Object, IIHdmiHotplugEventListener, IBinder)

HdmiControlManager::MyListener::MyListener(
    /* [in] */ IHdmiControlManagerHotplugEventListener* listener)
    : mListener(listener)
{
}

HdmiControlManager::MyListener::OnReceived(
    /* [in] */ IHdmiHotplugEvent* event)
{
    return mListener->OnReceived(event);
}

ECode HdmiControlManager::MyListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiControlManager, Object, IHdmiControlManager)

HdmiControlManager::HdmiControlManager()
    : mHasPlaybackDevice(FALSE)
    , mHasTvDevice(FALSE)
{
}

ECode HdmiControlManager::constructor()
{
    return NOERROR;
}

ECode HdmiControlManager::constructor(
    /* [in] */ IIHdmiControlService* service)
{
    mService = service;
    AutoPtr<ArrayOf<Int32> > types;
    if (mService != NULL) {
        //try {
        mService->GetSupportedTypes((ArrayOf<Int32>**)&types);
        //} catch (RemoteException e) {
            // Do nothing.
        //}
    }
    mHasTvDevice = HasDeviceType(types, IHdmiDeviceInfo::DEVICE_TV);
    mHasPlaybackDevice = HasDeviceType(types, IHdmiDeviceInfo::DEVICE_PLAYBACK);
    return NOERROR;
}

ECode HdmiControlManager::GetClient(
    /* [in] */ Int32 type,
    /* [out] */ IHdmiClient** client)
{
    VALIDATE_NOT_NULL(client);
    *client = NULL;

    if (mService == NULL) {
        *client = NULL;
        return NOERROR;
    }
    switch (type) {
        case IHdmiDeviceInfo::DEVICE_TV:
        {
            if (mHasTvDevice) {
                AutoPtr<HdmiClient> obj = new HdmiTvClient();
                obj->constructor(mService);
                *client = obj.Get();
                REFCOUNT_ADD(*client);
                return NOERROR;
            }
            else {
                *client = NULL;
                return NOERROR;
            }
        }
        case IHdmiDeviceInfo::DEVICE_PLAYBACK:
        {
            if (mHasPlaybackDevice) {
                AutoPtr<HdmiClient> obj = new HdmiPlaybackClient();
                obj->constructor(mService);
                *client = obj.Get();
                REFCOUNT_ADD(*client);
                return NOERROR;
            }
            else {
                *client = NULL;
                return NOERROR;
            }
        }
        default:
            *client = NULL;
            return NOERROR;
    }
    return NOERROR;
}

ECode HdmiControlManager::GetPlaybackClient(
    /* [out] */ IHdmiPlaybackClient** client)
{
    VALIDATE_NOT_NULL(client);

    AutoPtr<IHdmiClient> obj;
    GetClient(IHdmiDeviceInfo::DEVICE_PLAYBACK, (IHdmiClient**)&obj);
    *client = IHdmiPlaybackClient::Probe(obj);
    REFCOUNT_ADD(*client);
    return NOERROR;
}

ECode HdmiControlManager::GetTvClient(
    /* [out] */ IHdmiTvClient** client)
{
    VALIDATE_NOT_NULL(client);

    AutoPtr<IHdmiClient> obj;
    GetClient(IHdmiDeviceInfo::DEVICE_TV, (IHdmiClient**)&obj);
    *client = IHdmiTvClient::Probe(obj);
    REFCOUNT_ADD(*client);
    return NOERROR;
}

ECode HdmiControlManager::AddHotplugEventListener(
    /* [in] */ IHdmiControlManagerHotplugEventListener* listener)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    AutoPtr<IIHdmiHotplugEventListener> _listener = GetHotplugEventListenerWrapper(listener);
    mService->AddHotplugEventListener(_listener);
    //} catch (RemoteException e) {
        // Do nothing.
    //}
    return NOERROR;
}

ECode HdmiControlManager::RemoveHotplugEventListener(
    /* [in] */ IHdmiControlManagerHotplugEventListener* listener)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    AutoPtr<IIHdmiHotplugEventListener> _listener = GetHotplugEventListenerWrapper(listener);
    mService->RemoveHotplugEventListener(_listener);
    //} catch (RemoteException e) {
        // Do nothing.
    //}
    return NOERROR;
}

Boolean HdmiControlManager::HasDeviceType(
    /* [in] */ ArrayOf<Int32>* types,
    /* [in] */ Int32 type)
{
    if (types == NULL) {
        return FALSE;
    }

    for (Int32 i = 0; i < types->GetLength(); i++) {
        Int32 t = (*types)[i];

        if (t == type) {
            return TRUE;
        }
    }

    return FALSE;
}

AutoPtr<IIHdmiHotplugEventListener> HdmiControlManager::GetHotplugEventListenerWrapper(
    /* [in] */ IHdmiControlManagerHotplugEventListener* listener)
{
    AutoPtr<IIHdmiHotplugEventListener> tmp = new MyListener(listener);
    return tmp;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
