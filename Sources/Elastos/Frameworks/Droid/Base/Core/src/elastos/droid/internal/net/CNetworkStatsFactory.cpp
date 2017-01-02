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

#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/net/CNetworkStatsFactory.h"
// #include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/CProcFileReader.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsEntry.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/Log.h>
#include <utils/misc.h>
#include <utils/Vector.h>
#include <stdlib.h>

// using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::CProcFileReader;
using Elastos::Droid::Internal::Utility::IProcFileReader;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::CNetworkStatsEntry;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::NetworkStats;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringToIntegral;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

const String CNetworkStatsFactory::TAG("NetworkStatsFactory");
const Boolean CNetworkStatsFactory::USE_NATIVE_PARSING = TRUE;
const Boolean CNetworkStatsFactory::SANITY_CHECK_NATIVE = FALSE;

static AutoPtr<IArrayMap> InitStackedIfaces()
{
    AutoPtr<IArrayMap> arrayMap;
    CArrayMap::New((IArrayMap**)&arrayMap);
    return arrayMap;
}
AutoPtr<IArrayMap> CNetworkStatsFactory::sStackedIfaces = InitStackedIfaces();

static Int32 KernelToTag(
    /* [in] */ const String& string)
{
    Int32 length = string.GetLength();
    if (length > 10) {
        Int32 result;
        StringToIntegral::Decode(string.Substring(0, length - 8), &result);
        return result;
    }
    else {
        return 0;
    }
}

CAR_INTERFACE_IMPL(CNetworkStatsFactory, Object, INetworkStatsFactory)

CAR_OBJECT_IMPL(CNetworkStatsFactory)

ECode CNetworkStatsFactory::NoteStackedIface(
    /* [in] */ const String& stackedIface,
    /* [in] */ const String& baseIface)
{
    AutoLock lock(sStackedIfaces);
    if (baseIface != NULL) {
        sStackedIfaces->Put(CoreUtils::Convert(stackedIface), CoreUtils::Convert(baseIface));
    }
    else {
        sStackedIfaces->Remove(CoreUtils::Convert(stackedIface));
    }
    return NOERROR;
}

ECode CNetworkStatsFactory::constructor()
{
    AutoPtr<IFile> file;
    CFile::New(String("/proc/"), (IFile**)&file);
    return constructor(file);
}

ECode CNetworkStatsFactory::constructor(
    /* [in] */ IFile* procRoot)
{
    CFile::New(procRoot, String("net/xt_qtaguid/iface_stat_all"), (IFile**)&mStatsXtIfaceAll);
    CFile::New(procRoot, String("net/xt_qtaguid/iface_stat_fmt"), (IFile**)&mStatsXtIfaceFmt);
    CFile::New(procRoot, String("net/xt_qtaguid/stats"), (IFile**)&mStatsXtUid);
    return NOERROR;
}

ECode CNetworkStatsFactory::ReadNetworkStatsSummaryDev(
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats);

    AutoPtr<IStrictModeThreadPolicy> savedPolicy;
    assert(0 && "TODO: CStrictMode is not implemented!");
    // CStrictMode::::AllowThreadDiskReads((IStrictModeThreadPolicy**)&savedPolicy);

    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 6, stats);
    assert(*stats != NULL);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);

    AutoPtr<IProcFileReader> reader;
    ECode ec = NOERROR;
    do {
        AutoPtr<IInputStream> fstream;
        ec = CFileInputStream::New(mStatsXtIfaceAll, (IInputStream**)&fstream);
        if (FAILED(ec))
            break;
        ec = CProcFileReader::New(fstream, (IProcFileReader**)&reader);
        if (FAILED(ec))
            break;

        Boolean hasData;
        reader->HasMoreData(&hasData);
        while (hasData) {
            String iface;
            ec = reader->NextString(&iface);
            if (FAILED(ec))
                break;
            entry->SetIface(iface);
            entry->SetUid(INetworkStats::UID_ALL);
            entry->SetSet(INetworkStats::SET_ALL);
            entry->SetTag(INetworkStats::TAG_NONE);

            Int32 nvalue;
            ec = reader->NextInt32(&nvalue);
            if (FAILED(ec))
                break;
            Boolean active = nvalue != 0;

            // always include snapshot values
            Int64 value;
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxPackets(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxPackets(value);

            // fold in active numbers, but only when active
            if (active) {
                Int64 value, evalue;
                ec = reader->NextInt64(&value);
                if (FAILED(ec))
                    break;
                entry->GetRxBytes(&evalue);
                entry->SetRxBytes(value + evalue);
                ec = reader->NextInt64(&value);
                if (FAILED(ec))
                    break;
                entry->GetRxPackets(&evalue);
                entry->SetRxPackets(value + evalue);
                ec = reader->NextInt64(&value);
                if (FAILED(ec))
                    break;
                entry->GetTxBytes(&evalue);
                entry->SetTxBytes(value + evalue);
                ec = reader->NextInt64(&value);
                if (FAILED(ec))
                    break;
                entry->GetTxPackets(&evalue);
                entry->SetTxPackets(value + evalue);
            }

            (*stats)->AddValues(entry);
            ec = reader->FinishLine();
        }
    } while (0);

    if (ec == (ECode)E_NULL_POINTER_EXCEPTION
        || ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "problem parsing stats");
        ec = E_PROTOCOL_EXCEPTION;
    }

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(reader));
    // StrictMode::SetThreadPolicy(savedPolicy);

    return ec;
}

ECode CNetworkStatsFactory::ReadNetworkStatsSummaryXt(
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats);
    *stats = NULL;

    AutoPtr<IStrictModeThreadPolicy> savedPolicy;
    assert(0 && "TODO: CStrictMode is not implemented!");
    // CStrictMode::::AllowThreadDiskReads((IStrictModeThreadPolicy**)&savedPolicy);

    // return null when kernel doesn't support
    Boolean exists;
    mStatsXtIfaceFmt->Exists(&exists);
    if (!exists)
        return NOERROR;

    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 6, stats);
    assert(*stats != NULL);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);

    AutoPtr<IProcFileReader> reader;
    ECode ec = NOERROR;
    do {
        AutoPtr<IInputStream> fstream;
        ec = CFileInputStream::New(mStatsXtIfaceFmt, (IInputStream**)&fstream);
        if (FAILED(ec))
            break;
        ec = CProcFileReader::New(fstream, (IProcFileReader**)&reader);
        if (FAILED(ec))
            break;

        ec = reader->FinishLine();
        if (FAILED(ec))
            break;

        Boolean hasData;
        reader->HasMoreData(&hasData);
        while (hasData) {
            String iface;
            ec = reader->NextString(&iface);
            if (FAILED(ec))
                break;
            entry->SetIface(iface);
            entry->SetUid(INetworkStats::UID_ALL);
            entry->SetSet(INetworkStats::SET_ALL);
            entry->SetTag(INetworkStats::TAG_NONE);

            Int64 value;
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxPackets(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxPackets(value);

            (*stats)->AddValues(entry);
            ec = reader->FinishLine();
        }
    } while (0);

    if (ec == (ECode)E_NULL_POINTER_EXCEPTION
        || ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "problem parsing stats");
        ec = E_PROTOCOL_EXCEPTION;
    }

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(reader));
    // StrictMode::SetThreadPolicy(savedPolicy);

    return ec;
}

ECode CNetworkStatsFactory::ReadNetworkStatsDetail(
    /* [out] */ INetworkStats** stats)
{
    return ReadNetworkStatsDetail(
        INetworkStats::UID_ALL, NULL, INetworkStats::TAG_ALL, NULL, stats);
}

ECode CNetworkStatsFactory::ReadNetworkStatsDetail(
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag,
    /* [in] */ INetworkStats* lastStats,
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats);
    FAIL_RETURN(ReadNetworkStatsDetailInternal(limitUid, limitIfaces, limitTag, lastStats, stats))

    {
        AutoLock lock(sStackedIfaces);
        // Sigh, xt_qtaguid ends up double-counting tx traffic going through
        // clatd interfaces, so we need to subtract it here.
        Int32 size;
        sStackedIfaces->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> key, value;
            sStackedIfaces->GetKeyAt(i, (IInterface**)&key);
            sStackedIfaces->GetValueAt(i, (IInterface**)&value);
            String stackedIface, baseIface;
            IObject::Probe(key)->ToString(&stackedIface);
            IObject::Probe(value)->ToString(&baseIface);

            // Count up the tx traffic and subtract from root UID on the
            // base interface.
            AutoPtr<INetworkStatsEntry> adjust;
            CNetworkStatsEntry::New(baseIface, 0, 0, 0, 0L, 0L, 0L,
                    0L, 0L, (INetworkStatsEntry**)&adjust);
            AutoPtr<INetworkStatsEntry> entry;
            Int32 statsSize;
            (*stats)->GetSize(&statsSize);
            for (Int32 j = 0; j < statsSize; j++) {
                AutoPtr<INetworkStatsEntry> outEntry;
                (*stats)->GetValues(j, entry, (INetworkStatsEntry**)&outEntry);
                entry = outEntry;
                String iface;
                entry->GetIface(&iface);
                if (stackedIface.Equals(iface)) {
                    Int64 txBytes1, txBytes2;
                    adjust->GetTxBytes(&txBytes1);
                    entry->GetTxBytes(&txBytes2);
                    adjust->SetTxBytes(txBytes1 - txBytes2);
                    Int64 txPackets1, txPackets2;
                    adjust->GetTxPackets(&txPackets1);
                    entry->GetTxPackets(&txPackets2);
                    adjust->SetTxPackets(txPackets1 - txPackets2);
                }
            }
            FAIL_RETURN((*stats)->CombineValues(adjust))
        }
    }

    // Double sigh, all rx traffic on clat needs to be tweaked to
    // account for the dropped IPv6 header size post-unwrap.
    AutoPtr<INetworkStatsEntry> entry;
    Int32 size;
    (*stats)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<INetworkStatsEntry> outEntry;
        (*stats)->GetValues(i, entry, (INetworkStatsEntry**)&outEntry);
        entry = outEntry;
        String iface;
        entry->GetIface(&iface);
        if (iface != NULL && iface.StartWith("clat")) {
            // Delta between IPv4 header (20b) and IPv6 header (40b)
            Int64 rxPackets;
            entry->GetRxPackets(&rxPackets);
            entry->SetRxBytes(rxPackets * 20);
            entry->SetRxPackets(0);
            entry->SetTxBytes(0);
            entry->SetTxPackets(0);
            FAIL_RETURN((*stats)->CombineValues(entry))
        }
    }

    return NOERROR;
}

ECode CNetworkStatsFactory::ReadNetworkStatsDetailInternal(
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag,
    /* [in] */ INetworkStats* lastStats,
    /* [out] */ INetworkStats** stats)
{
    if (USE_NATIVE_PARSING) {
        if (lastStats != NULL) {
            *stats = lastStats;
            REFCOUNT_ADD(*stats)
            (*stats)->SetElapsedRealtime(SystemClock::GetElapsedRealtime());
        }
        else {
            CNetworkStats::New(SystemClock::GetElapsedRealtime(), -1, stats);
        }
        String path;
        mStatsXtUid->GetAbsolutePath(&path);
        if (NativeReadNetworkStatsDetail(*stats, path, limitUid, limitIfaces, limitTag) != 0) {
            Logger::E(TAG, "Failed to parse network stats");
            return E_IO_EXCEPTION;
        }
        if (SANITY_CHECK_NATIVE) {
            AutoPtr<INetworkStats> javaStats;
            FAIL_RETURN(JavaReadNetworkStatsDetail(mStatsXtUid, limitUid, limitIfaces, limitTag, stats))
            FAIL_RETURN(AssertEquals(javaStats, *stats))
        }
        return NOERROR;
    }
    else {
        return JavaReadNetworkStatsDetail(mStatsXtUid, limitUid, limitIfaces, limitTag, stats);
    }
}

ECode CNetworkStatsFactory::JavaReadNetworkStatsDetail(
    /* [in] */ IFile* detailPath,
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag,
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    AutoPtr<IStrictModeThreadPolicy> savedPolicy;
    assert(0 && "TODO: CStrictMode is not implemented!");
    // CStrictMode::::AllowThreadDiskReads((IStrictModeThreadPolicy**)&savedPolicy);

    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 24, stats);
    assert(*stats != NULL);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);

    Int32 idx = 1;
    Int32 lastIdx = 1;

    AutoPtr<IProcFileReader> reader;
    ECode ec = NOERROR;
    do {
        AutoPtr<IInputStream> fstream;
        ec = CFileInputStream::New(detailPath, (IInputStream**)&fstream);
        if (FAILED(ec))
            break;
        ec = CProcFileReader::New(fstream, (IProcFileReader**)&reader);
        if (FAILED(ec))
            break;

        ec = reader->FinishLine();
        if (FAILED(ec))
            break;

        Boolean hasData;
        reader->HasMoreData(&hasData);
        while (hasData) {
            ec = reader->NextInt32(&idx);
            if (FAILED(ec))
                break;
            if (idx != lastIdx + 1) {
                Logger::E(TAG, "inconsistent idx=%d after lastIdx=%d", idx, lastIdx);
                ec = E_PROTOCOL_EXCEPTION;
                break;
            }
            lastIdx = idx;

            String iface;
            ec = reader->NextString(&iface);
            if (FAILED(ec))
                break;
            entry->SetIface(iface);
            String str;
            ec = reader->NextString(&str);
            if (FAILED(ec))
                break;
            Int32 tag = KernelToTag(str);
            entry->SetTag(tag);
            Int32 uid;
            ec = reader->NextInt32(&uid);
            if (FAILED(ec))
                break;
            entry->SetUid(uid);
            Int32 set;
            ec = reader->NextInt32(&set);
            if (FAILED(ec))
                break;
            entry->SetSet(set);

            Int64 value;
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetRxPackets(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxBytes(value);
            ec = reader->NextInt64(&value);
            if (FAILED(ec))
                break;
            entry->SetTxPackets(value);

            if ((limitIfaces == NULL || ArrayUtils::Contains(limitIfaces, iface))
                    && (limitUid == INetworkStats::UID_ALL || limitUid == uid)
                    && (limitTag == INetworkStats::TAG_ALL || limitTag == tag)) {
                (*stats)->AddValues(entry);
            }
            ec = reader->FinishLine();
        }
    } while (0);

    if (ec == (ECode)E_NULL_POINTER_EXCEPTION
        || ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "problem parsing stats");
        ec = E_PROTOCOL_EXCEPTION;
    }

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(reader));
    // StrictMode::SetThreadPolicy(savedPolicy);

    return ec;
}

ECode CNetworkStatsFactory::AssertEquals(
    /* [in] */ INetworkStats* expected,
    /* [in] */ INetworkStats* actual)
{
    Int32 expectedSize, actualSize;
    expected->GetSize(&expectedSize);
    actual->GetSize(&actualSize);
    if (expectedSize != actualSize) {
        Logger::E(TAG, "Expected size %d, actual size %d", expectedSize, actualSize);
        return E_ASSERTION_ERROR;
    }

    AutoPtr<INetworkStatsEntry> expectedRow;
    AutoPtr<INetworkStatsEntry> actualRow;
    for (Int32 i = 0; i < expectedSize; i++) {
        AutoPtr<INetworkStatsEntry> outExpectedRow;
        expected->GetValues(i, expectedRow, (INetworkStatsEntry**)&outExpectedRow);
        expectedRow = outExpectedRow;
        AutoPtr<INetworkStatsEntry> outActualRow;
        actual->GetValues(i, actualRow, (INetworkStatsEntry**)&outActualRow);
        actualRow = outActualRow;
        Boolean res;
        IObject::Probe(expectedRow)->Equals(actualRow, &res);
        if (!res) {
            String str1, str2;
            IObject::Probe(expectedRow)->ToString(&str1);
            IObject::Probe(actualRow)->ToString(&str2);
            Logger::E(TAG, "Expected row %d: %s, actual row %s", i, str1.string(), str2.string());
            return E_ASSERTION_ERROR;
        }
    }
    return NOERROR;
}

struct stats_line {
    char iface[32];
    int32_t uid;
    int32_t set;
    int32_t tag;
    int64_t rxBytes;
    int64_t rxPackets;
    int64_t txBytes;
    int64_t txPackets;
};

Int32 CNetworkStatsFactory::NativeReadNetworkStatsDetail(
    /* [in] */ INetworkStats* _stats,
    /* [in] */ const String& path,
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag)
{
    if (path == NULL) {
        return -1;
    }

    FILE *fp = fopen(path.string(), "r");
    if (fp == NULL) {
        return -1;
    }

    android::Vector<stats_line> lines;

    int lastIdx = 1;
    int idx;
    char buffer[384];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        stats_line s;
        int64_t rawTag;
        char* pos = buffer;
        char* endPos;
        // First field is the index.
        idx = (int)strtol(pos, &endPos, 10);
        //ALOGI("Index #%d: %s", idx, buffer);
        if (pos == endPos) {
            // Skip lines that don't start with in index.  In particular,
            // this will skip the initial header line.
            continue;
        }
        if (idx != lastIdx + 1) {
            ALOGE("inconsistent idx=%d after lastIdx=%d: %s", idx, lastIdx, buffer);
            fclose(fp);
            return -1;
        }
        lastIdx = idx;
        pos = endPos;
        // Skip whitespace.
        while (*pos == ' ') {
            pos++;
        }
        // Next field is iface.
        int ifaceIdx = 0;
        while (*pos != ' ' && *pos != 0 && ifaceIdx < (int)(sizeof(s.iface)-1)) {
            s.iface[ifaceIdx] = *pos;
            ifaceIdx++;
            pos++;
        }
        if (*pos != ' ') {
            ALOGE("bad iface: %s", buffer);
            fclose(fp);
            return -1;
        }
        s.iface[ifaceIdx] = 0;
        if (limitIfaces != NULL && limitIfaces->GetLength() > 0) {
            // Is this an iface the caller is interested in?
            int i = 0;
            while (i < (int)limitIfaces->GetLength()) {
                if ((*limitIfaces)[i] == s.iface) {
                    break;
                }
                i++;
            }
            if (i >= (int)limitIfaces->GetLength()) {
                // Nothing matched; skip this line.
                //ALOGI("skipping due to iface: %s", buffer);
                continue;
            }
        }

        // Ignore whitespace
        while (*pos == ' ') pos++;

        // Find end of tag field
        endPos = pos;
        while (*endPos != ' ') endPos++;

        // Three digit field is always 0x0, otherwise parse
        if (endPos - pos == 3) {
            rawTag = 0;
        } else {
            if (sscanf(pos, "%llx", &rawTag) != 1) {
                ALOGE("bad tag: %s", pos);
                fclose(fp);
                return -1;
            }
        }
        s.tag = rawTag >> 32;
        if (limitTag != -1 && s.tag != limitTag) {
            //ALOGI("skipping due to tag: %s", buffer);
            continue;
        }
        pos = endPos;

        // Ignore whitespace
        while (*pos == ' ') pos++;

        // Parse remaining fields.
        if (sscanf(pos, "%u %u %llu %llu %llu %llu",
                &s.uid, &s.set, &s.rxBytes, &s.rxPackets,
                &s.txBytes, &s.txPackets) == 6) {
            if (limitUid != -1 && limitUid != s.uid) {
                //ALOGI("skipping due to uid: %s", buffer);
                continue;
            }
            lines.push_back(s);
        } else {
            //ALOGI("skipping due to bad remaining fields: %s", pos);
        }
    }

    if (fclose(fp) != 0) {
        ALOGE("Failed to close netstats file");
        return -1;
    }

    NetworkStats* stats = (NetworkStats*)_stats;
    int size = lines.size();
    bool grow = size > stats->mCapacity;

    AutoPtr<ArrayOf<String> > iface = grow ? ArrayOf<String>::Alloc(size) : stats->mIface.Get();
    AutoPtr<ArrayOf<Int32> > uid = grow ? ArrayOf<Int32>::Alloc(size) : stats->mUid.Get();
    AutoPtr<ArrayOf<Int32> > set = grow ? ArrayOf<Int32>::Alloc(size) : stats->mSet.Get();
    AutoPtr<ArrayOf<Int32> > tag = grow ? ArrayOf<Int32>::Alloc(size) : stats->mTag.Get();
    AutoPtr<ArrayOf<Int64> > rxBytes = grow ? ArrayOf<Int64>::Alloc(size) : stats->mRxBytes.Get();
    AutoPtr<ArrayOf<Int64> > rxPackets = grow ? ArrayOf<Int64>::Alloc(size) : stats->mRxPackets.Get();
    AutoPtr<ArrayOf<Int64> > txBytes = grow ? ArrayOf<Int64>::Alloc(size) : stats->mTxBytes.Get();
    AutoPtr<ArrayOf<Int64> > txPackets = grow ? ArrayOf<Int64>::Alloc(size) : stats->mTxPackets.Get();

    for (int i = 0; i < size; i++) {
        (*iface)[i] = lines[i].iface;
        (*uid)[i] = lines[i].uid;
        (*set)[i] = lines[i].set;
        (*tag)[i] = lines[i].tag;
        (*rxBytes)[i] = lines[i].rxBytes;
        (*rxPackets)[i] = lines[i].rxPackets;
        (*txBytes)[i] = lines[i].txBytes;
        (*txPackets)[i] = lines[i].txPackets;
    }

    if (grow) {
        stats->mIface = iface;
        stats->mUid = uid;
        stats->mSet = set;
        stats->mTag = tag;
        stats->mRxBytes = rxBytes;
        stats->mRxPackets = rxPackets;
        stats->mTxBytes = txBytes;
        stats->mTxPackets = txPackets;
    }

    return 0;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
