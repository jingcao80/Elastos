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

#include "elastos/droid/net/NetworkStatsEntry.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkStats.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkStatsEntry, Object, INetworkStatsEntry)

ECode NetworkStatsEntry::constructor()
{
    String ifaceAll;
    CNetworkStats::GetIFACE_ALL(&ifaceAll);
    return constructor(ifaceAll, INetworkStats::UID_ALL,
            INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE, 0L, 0L, 0L, 0L, 0L);
}

ECode NetworkStatsEntry::constructor(
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations)
{
    String ifaceAll;
    NetworkStats::GetIFACE_ALL(&ifaceAll);
    return constructor(ifaceAll, INetworkStats::UID_ALL,
        INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE,
        rxBytes, rxPackets, txBytes, txPackets, operations);
}

ECode NetworkStatsEntry::constructor(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations)
{
    mIface = iface;
    mUid = uid;
    mSet = set;
    mTag = tag;
    mRxBytes = rxBytes;
    mRxPackets = rxPackets;
    mTxBytes = txBytes;
    mTxPackets = txPackets;
    mOperations = operations;
    return NOERROR;
}

ECode NetworkStatsEntry::IsNegative(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRxBytes < 0 || mRxPackets < 0 || mTxBytes < 0 || mTxPackets < 0 || mOperations < 0;
    return NOERROR;
}

ECode NetworkStatsEntry::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRxBytes == 0 && mRxPackets == 0 && mTxBytes == 0 && mTxPackets == 0
            && mOperations == 0;
    return NOERROR;
}

ECode NetworkStatsEntry::Add(
    /* [in] */ INetworkStatsEntry* another)
{
    Int64 num;
    another->GetRxBytes(&num);
    mRxBytes += num;
    another->GetRxPackets(&num);
    mRxPackets += num;
    another->GetTxBytes(&num);
    mTxBytes += num;
    another->GetTxPackets(&num);
    mTxPackets += num;
    another->GetOperations(&num);
    mOperations += num;
    return NOERROR;
}

ECode NetworkStatsEntry::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder builder;
    builder += String("iface=");
    builder += mIface;
    builder += String(" uid=");
    builder += StringUtils::ToString(mUid);
    builder += String(" set=");
    builder += StringUtils::ToString(mSet);
    builder += String(" tag=");
    builder += StringUtils::ToString(mTag);
    builder += String(" rxBytes=");
    builder += StringUtils::ToString(mRxBytes);
    builder += String(" rxPackets=");
    builder += StringUtils::ToString(mRxPackets);
    builder += String(" txBytes=");
    builder += StringUtils::ToString(mTxBytes);
    builder += String(" txPackets=");
    builder += StringUtils::ToString(mTxPackets);
    builder += String(" operations=");
    builder += StringUtils::ToString(mOperations);
    *result = builder.ToString();
    return NOERROR;
}

ECode NetworkStatsEntry::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }
    if (INetworkStatsEntry::Probe(o) != NULL) {
        AutoPtr<NetworkStatsEntry> e = (NetworkStatsEntry*) INetworkStatsEntry::Probe(o);
        *result = mUid == e->mUid && mSet == e->mSet && mTag == e->mTag && mRxBytes == e->mRxBytes
                && mRxPackets == e->mRxPackets && mTxBytes == e->mTxBytes
                && mTxPackets == e->mTxPackets && mOperations == e->mOperations
                && mIface.Equals(e->mIface);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode NetworkStatsEntry::GetIface(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIface;
    return NOERROR;
}

ECode NetworkStatsEntry::SetIface(
    /* [in] */ const String& iface)
{
    mIface = iface;
    return NOERROR;
}


ECode NetworkStatsEntry::GetUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mUid;
    return NOERROR;
}


ECode NetworkStatsEntry::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}


ECode NetworkStatsEntry::GetSet(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSet;
    return NOERROR;
}


ECode NetworkStatsEntry::SetSet(
    /* [in] */ Int32 set)
{
    mSet = set;
    return NOERROR;
}


ECode NetworkStatsEntry::GetTag(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTag;
    return NOERROR;
}


ECode NetworkStatsEntry::SetTag(
    /* [in] */ Int32 tag)
{
    mTag = tag;
    return NOERROR;
}


ECode NetworkStatsEntry::GetRxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRxBytes;
    return NOERROR;
}


ECode NetworkStatsEntry::SetRxBytes(
    /* [in] */ Int64 rxBytes)
{
    mRxBytes = rxBytes;
    return NOERROR;
}


ECode NetworkStatsEntry::GetRxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRxPackets;
    return NOERROR;
}


ECode NetworkStatsEntry::SetRxPackets(
    /* [in] */ Int64 rxPackets)
{
    mRxPackets = rxPackets;
    return NOERROR;
}


ECode NetworkStatsEntry::GetTxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTxBytes;
    return NOERROR;
}


ECode NetworkStatsEntry::SetTxBytes(
    /* [in] */ Int64 txBytes)
{
    mTxBytes = txBytes;
    return NOERROR;
}


ECode NetworkStatsEntry::GetTxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTxPackets;
    return NOERROR;
}


ECode NetworkStatsEntry::SetTxPackets(
    /* [in] */ Int64 txPackets)
{
    mTxPackets = txPackets;
    return NOERROR;
}


ECode NetworkStatsEntry::GetOperations(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mOperations;
    return NOERROR;
}


ECode NetworkStatsEntry::SetOperations(
    /* [in] */ Int64 operations)
{
    mOperations = operations;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
