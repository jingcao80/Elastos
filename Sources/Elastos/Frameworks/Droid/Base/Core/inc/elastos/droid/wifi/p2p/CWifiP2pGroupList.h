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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PGROUPLIST_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PGROUPLIST_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pGroupList.h"
#include "elastos/droid/utility/LruCache.h"
#include <elastos/core/Object.h>

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Core::IInteger32)

using Elastos::Droid::Utility::LruCache;
using Elastos::Core::IInteger32;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pGroupList)
    , public Object
    , public IWifiP2pGroupList
{
private:
    class GroupLruCache
        : public LruCache
    {
    public:
        CARAPI constructor(
            /* [in] */ Int32 size,
            /* [in] */ IWifiP2pGroupList* host);

        virtual CARAPI EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ IInterface* netId,           // key
            /* [in] */ IInterface* oldValue,        // value type
            /* [in] */ IInterface* newValue);       // value type
    private:
        CWifiP2pGroupList* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pGroupList();

    ~CWifiP2pGroupList();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiP2pGroupList* source,
        /* [in] */ IWifiP2pGroupListGroupDeleteListener* listener);

    CARAPI GetGroupList(
        /* [out] */ ICollection** result);

    CARAPI GetGroupList(
        /* [out, callee] */ ArrayOf<IWifiP2pGroup*>** list);

    CARAPI Add(
        /* [in] */ IWifiP2pGroup* group);

    CARAPI Remove(
        /* [in] */ Int32 netId);

    CARAPI Remove(
        /* [in] */ const String& deviceAddress);

    CARAPI Clear(
        /* [out] */ Boolean* ret);

    CARAPI GetNetworkId(
        /* [in] */ const String& deviceAddress,
        /* [out] */ Int32* networkId);

    CARAPI GetNetworkId(
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& ssid,
        /* [out] */ Int32* networkId);

    CARAPI GetOwnerAddr(
        /* [in] */ Int32 netId,
        /* [out] */ String* addr);

    CARAPI Contains(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* ret);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static const Int32 CREDENTIAL_MAX_NUM;
    GroupLruCache* mGroups;

    AutoPtr<IWifiP2pGroupListGroupDeleteListener> mListener;

    Boolean mIsClearCalled;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PGROUPLIST_H__
