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

#ifndef __ELASTOS_DROID_INTERNAL_NET_CVPNPROFILE_H__
#define __ELASTOS_DROID_INTERNAL_NET_CVPNPROFILE_H__

#include "_Elastos_Droid_Internal_Net_CVpnProfile.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CVpnProfile)
    , public Object
    , public IVpnProfile
    , public ICloneable
    , public IParcelable
{
public:
    CVpnProfile();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& key);

    CARAPI Encode(
        /* [out, callee] */ ArrayOf<Byte>** value);

    /**
     * Test if profile is valid for lockdown, which requires IPv4 address for
     * both server and DNS. Server hostnames would require using DNS before
     * connection.
     */
    CARAPI IsValidLockdownProfile(
        /* [out] */ Boolean* isValid);

    CARAPI GetKey(
        /* [out] */ String* key);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetServer(
        /* [out] */ String* server);

    CARAPI SetServer(
        /* [in] */ const String& server);

    CARAPI GetUsername(
        /* [out] */ String* username);

    CARAPI SetUsername(
        /* [in] */ const String& username);

    CARAPI GetPassword(
        /* [out] */ String* password);

    CARAPI SetPassword(
        /* [in] */ const String& password);

    CARAPI GetDnsServers(
        /* [out] */ String* dnsServers);

    CARAPI SetDnsServers(
        /* [in] */ const String& dnsServers);

    CARAPI GetSearchDomains(
        /* [out] */ String* searchDomains);

    CARAPI SetSearchDomains(
        /* [in] */ const String& searchDomains);

    CARAPI GetRoutes(
        /* [out] */ String* routes);

    CARAPI SetRoutes(
        /* [in] */ const String& routes);

    CARAPI GetMppe(
        /* [out] */ Boolean* mppe);

    CARAPI SetMppe(
        /* [in] */ Boolean mppe);

    CARAPI GetL2tpSecret(
        /* [out] */ String* value);

    CARAPI SetL2tpSecret(
        /* [in] */ const String& value);

    CARAPI GetIpsecIdentifier(
        /* [out] */ String* ipsecIdentifier);

    CARAPI SetIpsecIdentifier(
        /* [in] */ const String& ipsecIdentifier);

    CARAPI GetIpsecSecret(
        /* [out] */ String* ipsecSecret);

    CARAPI SetIpsecSecret(
        /* [in] */ const String& ipsecSecret);

    CARAPI GetIpsecUserCert(
        /* [out] */ String* ipsecUserCert);

    CARAPI SetIpsecUserCert(
        /* [in] */ const String& ipsecUserCert);

    CARAPI GetIpsecCaCert(
        /* [out] */ String* ipsecCaCert);

    CARAPI SetIpsecCaCert(
        /* [in] */ const String& ipsecCaCert);

    CARAPI GetIpsecServerCert(
        /* [out] */ String* ipsecServerCert);

    CARAPI SetIpsecServerCert(
        /* [in] */ const String& ipsecServerCert);

    CARAPI GetSaveLogin(
        /* [out] */ Boolean* saveLogin);

    CARAPI SetSaveLogin(
        /* [in] */ Boolean saveLogin);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    static CARAPI Decode(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ IVpnProfile** profile);

public:
    // Entity fields.
    String  mKey;            // -1
    String  mName;           // 0
    Int32   mType;           // 1
    String  mServer;         // 2
    String  mUsername;       // 3
    String  mPassword;       // 4
    String  mDnsServers;     // 5
    String  mSearchDomains;  // 6
    String  mRoutes;         // 7
    Boolean mMppe;           // 8
    String  mL2tpSecret;     // 9
    String  mIpsecIdentifier;// 10
    String  mIpsecSecret;    // 11
    String  mIpsecUserCert;  // 12
    String  mIpsecCaCert;    // 13
    String  mIpsecServerCert;// 14

    // Helper fields.
    Boolean mSaveLogin;

private:
    static const String TAG;
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CVPNPROFILE_H__
