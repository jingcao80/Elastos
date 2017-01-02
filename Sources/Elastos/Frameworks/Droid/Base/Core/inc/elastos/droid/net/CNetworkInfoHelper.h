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

#ifndef __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Net_CNetworkInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CNetworkInfoHelper)
    , public Singleton
    , public INetworkInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NetworkInfoDetailedStateFromString(
        /* [in] */ const String& str,
        /* [out] */ NetworkInfoDetailedState* state);

    CARAPI NetworkInfoDetailedStateToString(
        /* [in] */ NetworkInfoDetailedState state,
        /* [out] */ String* str);

    CARAPI Index(
        /* [in] */ NetworkInfoDetailedState state,
        /* [out] */ Int32* index);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__
