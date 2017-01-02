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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_SECURITYCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_SECURITYCONTROLLERIMPL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/net/ConnectivityManagerNetworkCallback.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Net::ConnectivityManagerNetworkCallback;
using Elastos::Droid::Net::IConnectivityManagerNetworkCallback;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class SecurityControllerImpl
    : public Object
    , public ISecurityController
{
private:
    class NetworkCallback: public ConnectivityManagerNetworkCallback
    {
    public:
        NetworkCallback(
            /* [in] */ SecurityControllerImpl* host);

        // @Override
        CARAPI OnAvailable(
            /* [in] */ INetwork* network);

        // TODO Find another way to receive VPN lost.  This may be delayed depending on
        // how long the VPN connection is held on to.
        // @Override
        CARAPI OnLost(
            /* [in] */ INetwork* network);

    private:
        SecurityControllerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SecurityControllerImpl(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI HasDeviceOwner(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasProfileOwner(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetDeviceOwnerName(
        /* [out] */ String* result);

    // @Override
    CARAPI GetProfileOwnerName(
        /* [out] */ String* result);

    // @Override
    CARAPI IsVpnEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsLegacyVpn(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetVpnApp(
        /* [out] */ String* result);

    // @Override
    CARAPI GetLegacyVpnName(
        /* [out] */ String* result);

    // @Override
    CARAPI DisconnectFromVpn();

    // @Override
    CARAPI RemoveCallback(
        /* [in] */ ISecurityControllerCallback* callback);

    // @Override
    CARAPI AddCallback(
        /* [in] */ ISecurityControllerCallback* callback);

    // @Override
    CARAPI OnUserSwitched(
        /* [in] */ Int32 newUserId);

private:
    CARAPI_(void) SetCurrentNetid(
        /* [in] */ Int32 netId);

    CARAPI_(void) FireCallbacks();

    CARAPI_(void) UpdateState();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static AutoPtr<INetworkRequest> REQUEST;
    static const Int32 NO_NETWORK;

    AutoPtr<IContext> mContext;
    AutoPtr<IConnectivityManager> mConnectivityManager;
    AutoPtr<IIConnectivityManager> mConnectivityService;
    AutoPtr<IDevicePolicyManager> mDevicePolicyManager;
    AutoPtr<IArrayList> mCallbacks;  /*<SecurityControllerCallback*/

    AutoPtr<IVpnConfig> mVpnConfig;
    String mVpnName;
    Int32 mCurrentVpnNetworkId;
    Int32 mCurrentUserId;
    AutoPtr<IConnectivityManagerNetworkCallback> mNetworkCallback;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_SECURITYCONTROLLERIMPL_H__
