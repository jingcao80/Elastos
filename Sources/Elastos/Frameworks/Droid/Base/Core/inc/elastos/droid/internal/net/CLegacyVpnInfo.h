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

#ifndef __ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFO_H__
#define __ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFO_H__

#include "Elastos.Droid.App.h"
#include "_Elastos_Droid_Internal_Net_CLegacyVpnInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Net::INetworkInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CLegacyVpnInfo)
    , public Object
    , public ILegacyVpnInfo
    , public IParcelable
{
public:
    CLegacyVpnInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetKey(
        /* [out] */ String* key);

    CARAPI SetKey(
        /* [in] */ const String& key);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Return best matching {@link LegacyVpnInfo} state based on given
     * {@link NetworkInfo}.
     */
    static Int32 StateFromNetworkInfo(
        /* [in] */ INetworkInfo* info);

public:
    String mKey;
    Int32 mState;
    AutoPtr<IPendingIntent> mIntent;

private:
    static const String TAG;
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFO_H__
