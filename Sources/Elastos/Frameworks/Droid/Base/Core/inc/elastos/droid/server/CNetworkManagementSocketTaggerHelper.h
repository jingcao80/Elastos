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

#ifndef __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__
#define __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__

#include "_Elastos_Droid_Server_CNetworkManagementSocketTaggerHelper.h"
#include "Elastos.Droid.Core.Server.h"
#include "elastos/droid/server/NetworkManagementSocketTagger.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Server::INetworkManagementSocketTagger;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CNetworkManagementSocketTaggerHelper)
    , public Singleton
    , public INetworkManagementSocketTagger
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Install();

    CARAPI SetThreadSocketStatsTag(
        /* [in] */ Int32 tag);

    CARAPI GetThreadSocketStatsTag(
        /* [out] */ Int32* tag);

    CARAPI SetThreadSocketStatsUid(
        /* [in] */ Int32 uid);

    CARAPI SetKernelCounterSet(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 counterSet);

    CARAPI ResetKernelUidStats(
        /* [in] */ Int32 uid);

    /**
     * Convert {@code /proc/} tag format to {@link Integer}. Assumes incoming
     * format like {@code 0x7fffffff00000000}.
     */
    CARAPI KernelToTag(
        /* [in] */ const String& string,
        /* [out] */ Int32* tag);
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__
