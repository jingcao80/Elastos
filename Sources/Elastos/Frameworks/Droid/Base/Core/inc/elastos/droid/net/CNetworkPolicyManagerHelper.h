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

#ifndef __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGERHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGERHELPER_H__

#include "_Elastos_Droid_Net_CNetworkPolicyManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Format::ITime;

using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CNetworkPolicyManagerHelper)
    , public Singleton
    , public INetworkPolicyManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ INetworkPolicyManager** result);

    CARAPI ComputeLastCycleBoundary(
        /* [in] */ Int64 currentTime,
        /* [in] */ INetworkPolicy* policy,
        /* [out] */ Int64* result);

    CARAPI ComputeNextCycleBoundary(
        /* [in] */ Int64 currentTime,
        /* [in] */ INetworkPolicy* policy,
        /* [out] */ Int64* result);

    CARAPI SnapToCycleDay(
        /* [in] */ ITime* time,
        /* [in] */ Int32 cycleDay);

    CARAPI IsUidValidForPolicy(
        /* [in] */ IContext* context,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI DumpPolicy(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ Int32 policy);

    CARAPI DumpRules(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ Int32 rules);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGERHELPER_H__
