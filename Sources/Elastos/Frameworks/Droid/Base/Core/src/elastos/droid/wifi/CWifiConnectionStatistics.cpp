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

#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/wifi/CWifiConnectionStatistics.h"
#include "elastos/droid/wifi/CWifiNetworkConnectionStatistics.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Wifi {

const String CWifiConnectionStatistics::TAG("CWifiConnnectionStatistics");

CAR_INTERFACE_IMPL_2(CWifiConnectionStatistics, Object, IParcelable, IWifiConnectionStatistics)

CAR_OBJECT_IMPL(CWifiConnectionStatistics)

CWifiConnectionStatistics::CWifiConnectionStatistics()
    : mNum5GhzConnected(0)
    , mNum24GhzConnected(0)
    , mNumAutoJoinAttempt(0)
    , mNumAutoRoamAttempt(0)
    , mNumWifiManagerJoinAttempt(0)
{
}

ECode CWifiConnectionStatistics::constructor()
{
    return CHashMap::New((IHashMap**)&mUntrustedNetworkHistory);
}

ECode CWifiConnectionStatistics::constructor(
    /* [in] */ IWifiConnectionStatistics* source)
{
    FAIL_RETURN(CHashMap::New((IHashMap**)&mUntrustedNetworkHistory));
    if (source != NULL) {
        AutoPtr<IHashMap> sourceHM;
        source->GetUntrustedNetworkHistory((IHashMap**)&sourceHM);
        IMap::Probe(mUntrustedNetworkHistory)->PutAll(IMap::Probe(sourceHM));
    }

    return NOERROR;
}

ECode CWifiConnectionStatistics::GetUntrustedNetworkHistory(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUntrustedNetworkHistory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetUntrustedNetworkHistory(
    /* [in] */ IHashMap* untrustedNetworkHistory)
{
    mUntrustedNetworkHistory = untrustedNetworkHistory;
    return NOERROR;
}

ECode CWifiConnectionStatistics::GetNum5GhzConnected(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNum5GhzConnected;
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetNum5GhzConnected(
    /* [in] */ Int32 num5GhzConnected)
{
    mNum5GhzConnected = num5GhzConnected;
    return NOERROR;
}

ECode CWifiConnectionStatistics::GetNum24GhzConnected(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNum24GhzConnected;
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetNum24GhzConnected(
    /* [in] */ Int32 num24GhzConnected)
{
    mNum24GhzConnected = num24GhzConnected;
    return NOERROR;
}

ECode CWifiConnectionStatistics::GetNumAutoJoinAttempt(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumAutoJoinAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetNumAutoJoinAttempt(
    /* [in] */ Int32 numAutoJoinAttempt)
{
    mNumAutoJoinAttempt = numAutoJoinAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::GetNumAutoRoamAttempt(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumAutoRoamAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetNumAutoRoamAttempt(
    /* [in] */ Int32 numAutoRoamAttempt)
{
    mNumAutoRoamAttempt = numAutoRoamAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::GetNumWifiManagerJoinAttempt(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumWifiManagerJoinAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::SetNumWifiManagerJoinAttempt(
    /* [in] */ Int32 numWifiManagerJoinAttempt)
{
    mNumWifiManagerJoinAttempt = numWifiManagerJoinAttempt;
    return NOERROR;
}

ECode CWifiConnectionStatistics::IncrementOrAddUntrusted(
    /* [in] */ const String& SSID,
    /* [in] */ Int32 connection,
    /* [in] */ Int32 usage)
{
    AutoPtr<IWifiNetworkConnectionStatistics> stats;
    if (TextUtils::IsEmpty(SSID))
        return NOERROR;

    Boolean bContainsKey;
    AutoPtr<IString> iSSID;
    CString::New(SSID, (IString**)&iSSID);
    mUntrustedNetworkHistory->ContainsKey(iSSID, &bContainsKey);
    if (bContainsKey) {
        AutoPtr<IInterface> obj;
        mUntrustedNetworkHistory->Get(iSSID, (IInterface**)&obj);
        stats = IWifiNetworkConnectionStatistics::Probe(obj);
        if (stats != NULL){
            Int32 numConnection;
            stats->GetNumConnection(&numConnection);
            stats->SetNumConnection(connection + numConnection);
            Int32 numUsage;
            stats->GetNumUsage(&numUsage);
            stats->SetNumUsage(usage + numUsage);
        }
    }
    else {
        CWifiNetworkConnectionStatistics::New(connection, usage, (IWifiNetworkConnectionStatistics**)&stats);
    }

    if (stats != NULL) {
        mUntrustedNetworkHistory->Put(iSSID, stats);
    }

    return NOERROR;
}

//@Override
ECode CWifiConnectionStatistics::ToString(
    /* [out] */ String* str)
{
    StringBuilder sbuf;
    sbuf.Append("Connected on: 2.4Ghz=");
    sbuf.Append(mNum24GhzConnected);
    sbuf.Append(" 5Ghz=");
    sbuf.Append(mNum5GhzConnected);
    sbuf.Append("\n");
    sbuf.Append(" join=");
    sbuf.Append(mNumWifiManagerJoinAttempt);
    sbuf.Append("\\");
    sbuf.Append(mNumAutoJoinAttempt);
    sbuf.Append("\n");
    sbuf.Append(" roam=");
    sbuf.Append(mNumAutoRoamAttempt);
    sbuf.Append("\n");

    AutoPtr<ISet> keySet;
    mUntrustedNetworkHistory->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> iKey;
        iter->GetNext((IInterface**)&iKey);
        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        AutoPtr<IWifiNetworkConnectionStatistics> stats;
        AutoPtr<IString> iK;
        CString::New(key, (IString**)&iK);
        AutoPtr<IInterface> obj;
        mUntrustedNetworkHistory->Get(iK, (IInterface**)&obj);
        stats = IWifiNetworkConnectionStatistics::Probe(obj);
        if (stats != NULL) {
            sbuf.Append(key);
            sbuf.Append(" ");
            String statsStr;
            IObject::Probe(stats)->ToString(&statsStr);
            sbuf.Append(statsStr);
            sbuf.Append("\n");
        }
    }

    return sbuf.ToString(str);
}

/** Implement the Parcelable interface */
ECode CWifiConnectionStatistics::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface */
//@Override
ECode CWifiConnectionStatistics::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mNum24GhzConnected);
    dest->WriteInt32(mNum5GhzConnected);
    dest->WriteInt32(mNumAutoJoinAttempt);
    dest->WriteInt32(mNumAutoRoamAttempt);
    dest->WriteInt32(mNumWifiManagerJoinAttempt);

    Int32 size;
    mUntrustedNetworkHistory->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<ISet> keySet;
    IMap::Probe(mUntrustedNetworkHistory)->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> iKey;
        iter->GetNext((IInterface**)&iKey);
        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        AutoPtr<IWifiNetworkConnectionStatistics> num;
        AutoPtr<IString> iK;
        CString::New(key, (IString**)&iK);
        AutoPtr<IInterface> obj;
        mUntrustedNetworkHistory->Get(iK, (IInterface**)&obj);
        num = IWifiNetworkConnectionStatistics::Probe(obj);
        dest->WriteString(key);
        Int32 numConnection;
        num->GetNumConnection(&numConnection);
        dest->WriteInt32(numConnection);
        Int32 numUsage;
        num->GetNumUsage(&numUsage);
        dest->WriteInt32(numUsage);
    }

    return NOERROR;
}

ECode CWifiConnectionStatistics::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNum24GhzConnected);
    source->ReadInt32(&mNum5GhzConnected);
    source->ReadInt32(&mNumAutoRoamAttempt);
    source->ReadInt32(&mNumAutoRoamAttempt);
    source->ReadInt32(&mNumWifiManagerJoinAttempt);
    Int32 n;
    source->ReadInt32(&n);
    while (n-- > 0) {
        String key;
        source->ReadString(&key);
        Int32 numConnection;
        source->ReadInt32(&numConnection);
        Int32 numUsage;
        source->ReadInt32(&numUsage);
        AutoPtr<IWifiNetworkConnectionStatistics> st;
        CWifiNetworkConnectionStatistics::New(numConnection, numUsage, (IWifiNetworkConnectionStatistics**)&st);
        AutoPtr<IString> iSSID;
        CString::New(key, (IString**)&iSSID);
        mUntrustedNetworkHistory->Put(iSSID, st);
    }
    return NOERROR;
}

ECode CWifiConnectionStatistics::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNum24GhzConnected);
    dest->WriteInt32(mNum5GhzConnected);
    dest->WriteInt32(mNumAutoJoinAttempt);
    dest->WriteInt32(mNumAutoRoamAttempt);
    dest->WriteInt32(mNumWifiManagerJoinAttempt);

    Int32 size;
    mUntrustedNetworkHistory->GetSize(&size);
    dest->WriteInt32(size);

    AutoPtr<ISet> keySet;
    mUntrustedNetworkHistory->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> iKey;
        iter->GetNext((IInterface**)&iKey);
        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        AutoPtr<IWifiNetworkConnectionStatistics> stats;
        AutoPtr<IString> iK;
        CString::New(key, (IString**)&iK);
        AutoPtr<IInterface> obj;
        mUntrustedNetworkHistory->Get(iK, (IInterface**)&obj);
        stats = IWifiNetworkConnectionStatistics::Probe(obj);
        Int32 numConnection, numUsage;
        stats->GetNumConnection(&numConnection);
        stats->GetNumUsage(&numUsage);
        dest->WriteString(key);
        dest->WriteInt32(numConnection);
        dest->WriteInt32(numUsage);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
