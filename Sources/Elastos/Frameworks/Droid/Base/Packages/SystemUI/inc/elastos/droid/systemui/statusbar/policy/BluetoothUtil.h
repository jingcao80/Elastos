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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHUTIL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHUTIL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothA2dpSink;
using Elastos::Droid::Bluetooth::IBluetoothHeadsetClient;
using Elastos::Droid::Bluetooth::IBluetoothInputDevice;
using Elastos::Droid::Bluetooth::IBluetoothMap;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothPan;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BluetoothUtil
{
private:
    class Profile1
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile1(
            /* [in] */ IBluetoothA2dp* a2dp);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothA2dp> mA2dp;
    };

    class Profile2
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile2(
            /* [in] */ IBluetoothHeadset* headset);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothHeadset> mHeadset;
    };

    class Profile3
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile3(
            /* [in] */ IBluetoothA2dpSink* sink);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothA2dpSink> mSink;
    };

    class Profile4
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile4(
            /* [in] */ IBluetoothHeadsetClient* client);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothHeadsetClient> mClient;
    };

    class Profile5
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile5(
            /* [in] */ IBluetoothInputDevice* input);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothInputDevice> mInput;
    };

    class Profile6
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile6(
            /* [in] */ IBluetoothMap* map);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothMap> mMap;
    };

    class Profile7
        : public Object
        , public IBluetoothUtilProfile
    {
    public:
        CAR_INTERFACE_DECL()

        Profile7(
            /* [in] */ IBluetoothPan* pan);

        CARAPI Connect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

        CARAPI Disconnect(
            /* [in] */ IBluetoothDevice* device,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IBluetoothPan> mPan;
    };

public:
    static CARAPI_(String) ProfileToString(
        /* [in] */ Int32 profile);

    static CARAPI_(String) ProfileStateToString(
        /* [in] */ Int32 state);

    static CARAPI_(String) UuidToString(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(String) ConnectionStateToString(
        /* [in] */ Int32 connectionState);

    static CARAPI_(String) DeviceToString(
        /* [in] */ IBluetoothDevice* device);

    static CARAPI_(String) UuidsToString(
        /* [in] */ IBluetoothDevice* device);

    static CARAPI_(Int32) UuidToProfile(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) GetProfile(
        /* [in] */ IBluetoothProfile* p);

private:
    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothA2dp* a2dp);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothHeadset* headset);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothA2dpSink* sink);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothHeadsetClient* client);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothInputDevice* input);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothMap* map);

    static CARAPI_(AutoPtr<IBluetoothUtilProfile>) NewProfile(
        /* [in] */ IBluetoothPan* pan);
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHUTIL_H__
