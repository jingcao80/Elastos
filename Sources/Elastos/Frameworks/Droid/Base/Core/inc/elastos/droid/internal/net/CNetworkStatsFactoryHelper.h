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

#ifndef __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__

#include "_Elastos_Droid_Internal_Net_CNetworkStatsFactoryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::INetworkStats;
using Elastos::IO::IFile;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CNetworkStatsFactoryHelper)
    , public Singleton
    , public INetworkStatsFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NoteStackedIface(
        /* [in] */ const String& stackedIface,
        /* [in] */ const String& baseIface);

    /**
     * Parse and return {@link NetworkStats} with UID-level details. Values are
     * expected to monotonically increase since device boot.
     */
    // @VisibleForTesting
    CARAPI JavaReadNetworkStatsDetail(
        /* [in] */ IFile* detailPath,
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [out] */ INetworkStats** stats);

    /**
     * Parse statistics from file into given {@link NetworkStats} object. Values
     * are expected to monotonically increase since device boot.
     */
    // @VisibleForTesting
    CARAPI NativeReadNetworkStatsDetail(
        /* [in] */ INetworkStats* stats,
        /* [in] */ const String& path,
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [out] */ Int32* result);
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__
