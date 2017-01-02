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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CTrafficStats.h"
#include "elastos/droid/net/TrafficStats.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CTrafficStats, Singleton, ITrafficStats)

CAR_SINGLETON_IMPL(CTrafficStats)

ECode CTrafficStats::SetThreadStatsTag(
        /* [in] */ Int32 tag)
{
    return TrafficStats::SetThreadStatsTag(tag);
}

ECode CTrafficStats::SetThreadStatsTagBackup()
{
    return TrafficStats::SetThreadStatsTagBackup();
}

ECode CTrafficStats::GetThreadStatsTag(
        /* [out] */ Int32* result)
{
    return TrafficStats::GetThreadStatsTag(result);
}

ECode CTrafficStats::ClearThreadStatsTag()
{
    return TrafficStats::ClearThreadStatsTag();
}

ECode CTrafficStats::SetThreadStatsUid(
        /* [in] */ Int32 uid)
{
    return TrafficStats::SetThreadStatsUid(uid);
}

ECode CTrafficStats::ClearThreadStatsUid()
{
    return TrafficStats::ClearThreadStatsUid();
}

ECode CTrafficStats::TagSocket(
        /* [in] */ ISocket* socket)
{
    return TrafficStats::TagSocket(socket);
}

ECode CTrafficStats::UntagSocket(
        /* [in] */ ISocket* socket)
{
    return TrafficStats::UntagSocket(socket);
}

ECode CTrafficStats::StartDataProfiling(
        /* [in] */ IContext* context)
{
    return TrafficStats::StartDataProfiling(context);
}

ECode CTrafficStats::StopDataProfiling(
        /* [in] */ IContext* context,
        /* [out] */ INetworkStats** result)
{
    return TrafficStats::StopDataProfiling(context, result);
}

ECode CTrafficStats::IncrementOperationCount(
        /* [in] */ Int32 operationCount)
{
    return TrafficStats::IncrementOperationCount(operationCount);
}

ECode CTrafficStats::IncrementOperationCount(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 operationCount)
{
    return TrafficStats::IncrementOperationCount(tag, operationCount);
}

ECode CTrafficStats::CloseQuietly(
        /* [in] */ IINetworkStatsSession* session)
{
    return TrafficStats::CloseQuietly(session);
}

ECode CTrafficStats::GetMobileTxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileTxPackets(result);
}

ECode CTrafficStats::GetMobileRxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileRxPackets(result);
}

ECode CTrafficStats::GetMobileTxBytes(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileTxBytes(result);
}

ECode CTrafficStats::GetMobileRxBytes(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileRxBytes(result);
}

ECode CTrafficStats::GetMobileTcpRxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileTcpRxPackets(result);
}

ECode CTrafficStats::GetMobileTcpTxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetMobileTcpTxPackets(result);
}

ECode CTrafficStats::GetTxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTxPackets(iface, result);
}

ECode CTrafficStats::GetRxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetRxPackets(iface, result);
}

ECode CTrafficStats::GetTxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTxBytes(iface, result);
}

ECode CTrafficStats::GetRxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetRxBytes(iface, result);
}

ECode CTrafficStats::GetTotalTxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTotalTxPackets(result);
}

ECode CTrafficStats::GetTotalRxPackets(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTotalRxPackets(result);
}

ECode CTrafficStats::GetTotalTxBytes(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTotalTxBytes(result);
}

ECode CTrafficStats::GetTotalRxBytes(
        /* [out] */ Int64* result)
{
    return TrafficStats::GetTotalRxBytes(result);
}

ECode CTrafficStats::GetUidTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTxBytes(uid, result);
}

ECode CTrafficStats::GetUidRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidRxBytes(uid, result);
}

ECode CTrafficStats::GetUidTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTxPackets(uid, result);
}

ECode CTrafficStats::GetUidRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidRxPackets(uid, result);
}

ECode CTrafficStats::GetUidTcpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTcpTxBytes(uid, result);
}

ECode CTrafficStats::GetUidTcpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTcpRxBytes(uid, result);
}

ECode CTrafficStats::GetUidUdpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidUdpTxBytes(uid, result);
}

ECode CTrafficStats::GetUidUdpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidUdpRxBytes(uid, result);
}

ECode CTrafficStats::GetUidTcpTxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTcpTxSegments(uid, result);
}

ECode CTrafficStats::GetUidTcpRxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidTcpRxSegments(uid, result);
}

ECode CTrafficStats::GetUidUdpTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidUdpTxPackets(uid, result);
}

ECode CTrafficStats::GetUidUdpRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result)
{
    return TrafficStats::GetUidUdpRxPackets(uid, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
