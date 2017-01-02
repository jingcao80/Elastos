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

#ifndef __ELASTOS_DROID_NET_CTRAFFICSTATS_H__
#define __ELASTOS_DROID_NET_CTRAFFICSTATS_H__

#include "_Elastos_Droid_Net_CTrafficStats.h"
#include <elastos/core/Singleton.h>

using Elastos::Net::ISocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that provides network traffic statistics.  These statistics include
 * bytes transmitted and received and network packets transmitted and received,
 * over all interfaces, over the mobile interface, and on a per-UID basis.
 * <p>
 * These statistics may not be available on all platforms.  If the statistics
 * are not supported by this device, {@link #UNSUPPORTED} will be returned.
 */
CarClass(CTrafficStats)
    , public Singleton
    , public ITrafficStats
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetThreadStatsTag(
        /* [in] */ Int32 tag);

    CARAPI SetThreadStatsTagBackup();

    CARAPI GetThreadStatsTag(
        /* [out] */ Int32* result);

    CARAPI ClearThreadStatsTag();

    CARAPI SetThreadStatsUid(
        /* [in] */ Int32 uid);

    CARAPI ClearThreadStatsUid();

    CARAPI TagSocket(
        /* [in] */ ISocket* socket);

    CARAPI UntagSocket(
        /* [in] */ ISocket* socket);

    CARAPI StartDataProfiling(
        /* [in] */ IContext* context);

    CARAPI StopDataProfiling(
        /* [in] */ IContext* context,
        /* [out] */ INetworkStats** result);

    CARAPI IncrementOperationCount(
        /* [in] */ Int32 operationCount);

    CARAPI IncrementOperationCount(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 operationCount);

    CARAPI CloseQuietly(
        /* [in] */ IINetworkStatsSession* session);

    CARAPI GetMobileTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileTxBytes(
        /* [out] */ Int64* result);

    CARAPI GetMobileRxBytes(
        /* [out] */ Int64* result);

    CARAPI GetMobileTcpRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileTcpTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetRxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetTxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetRxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetTotalTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTotalRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTotalTxBytes(
        /* [out] */ Int64* result);

    CARAPI GetTotalRxBytes(
        /* [out] */ Int64* result);

    CARAPI GetUidTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpTxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpRxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CTRAFFICSTATS_H__
