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

#ifndef __ELASTOS_DROID_SERVER_NET_NETWORKSTATSRECORDER_H__
#define __ELASTOS_DROID_SERVER_NET_NETWORKSTATSRECORDER_H__

#include "NetworkStatsCollection.h"
#include "elastos/droid/server/net/NetworkStatsCollection.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Internal::Utility::IFileRotator;
using Elastos::Droid::Internal::Utility::IFileRotatorRewriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::INetworkStatsNonMonotonicObserver;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class NetworkStatsRecorder
    : public Object
{
private:
    /**
     * Rewriter that will combine current {@link NetworkStatsCollection} values
     * with anything read from disk, and write combined set to disk. Clears the
     * original {@link NetworkStatsCollection} when finished writing.
     */
    class CombiningRewriter
        : public Object
        , public IFileRotatorRewriter
    {
    public:
        CombiningRewriter(
            /* [in] */ NetworkStatsCollection* collection);

        CAR_INTERFACE_DECL()

        CARAPI Reset();

        CARAPI Read(
            /* [in] */ IInputStream* in);

        CARAPI ShouldWrite(
            /* [out] */ Boolean* result);

        CARAPI Write(
            /* [in] */ IOutputStream* out);

    private:
        AutoPtr<NetworkStatsCollection> mCollection;
    };

    /**
     * Rewriter that will remove any {@link NetworkStatsHistory} attributed to
     * the requested UID, only writing data back when modified.
     */
    class RemoveUidRewriter
        : public Object
        , public IFileRotatorRewriter
    {
    public:
        RemoveUidRewriter(
            /* [in] */ Int64 bucketDuration,
            /* [in] */ ArrayOf<Int32>* uids);

        CAR_INTERFACE_DECL()

        CARAPI Reset();

        CARAPI Read(
            /* [in] */ IInputStream* in);

        CARAPI ShouldWrite(
            /* [out] */ Boolean* result);

        CARAPI Write(
            /* [in] */ IOutputStream* out);

    private:
        AutoPtr<NetworkStatsCollection> mTemp;
        AutoPtr<ArrayOf<Int32> > mUids;
    };

public:
    NetworkStatsRecorder(
        /* [in] */ IFileRotator* rotator,
        /* [in] */ INetworkStatsNonMonotonicObserver* observer,
        /* [in] */ IDropBoxManager* dropBox,
        /* [in] */ const String& cookie,
        /* [in] */ Int64 bucketDuration,
        /* [in] */ Boolean onlyTags);

    CARAPI_(void) SetPersistThreshold(
        /* [in] */ Int64 thresholdBytes);

    CARAPI_(void) ResetLocked();

    CARAPI GetTotalSinceBootLocked(
        /* [in] */ INetworkTemplate* templ,
        /* [out] */ INetworkStatsEntry** entry);

    /**
     * Load complete history represented by {@link FileRotator}. Caches
     * internally as a {@link WeakReference}, and updated with future
     * {@link #recordSnapshotLocked(NetworkStats, Map, long)} snapshots as long
     * as reference is valid.
     */
    CARAPI GetOrLoadCompleteLocked(
        /* [out] */ NetworkStatsCollection** result);

    /**
     * Record any delta that occurred since last {@link NetworkStats} snapshot,
     * using the given {@link Map} to identify network interfaces. First
     * snapshot is considered bootstrap, and is not counted as delta.
     */
    CARAPI_(void) RecordSnapshotLocked(
        /* [in] */ INetworkStats* snapshot,
        /* [in] */ IMap* ifaceIdent,
        /* [in] */ Int64 currentTimeMillis);

    /**
     * Consider persisting any pending deltas, if they are beyond
     * {@link #mPersistThresholdBytes}.
     */
    CARAPI_(void) MaybePersistLocked(
        /* [in] */ Int64 currentTimeMillis);

    /**
     * Force persisting any pending deltas.
     */
    CARAPI ForcePersistLocked(
        /* [in] */ Int64 currentTimeMillis);

    /**
     * Remove the given UID from all {@link FileRotator} history, migrating it
     * to {@link TrafficStats#UID_REMOVED}.
     */
    CARAPI RemoveUidsLocked(
        /* [in] */ ArrayOf<Int32>* uids);


    CARAPI ImportLegacyNetworkLocked(
        /* [in] */ IFile* file);

    CARAPI ImportLegacyUidLocked(
        /* [in] */ IFile* file);

    CARAPI_(void) DumpLocked(
        /* [in] */ IIndentingPrintWriter* pw,
        /* [in] */ Boolean fullHistory);

private:
    /**
    * Recover from {@link FileRotator} failure by dumping state to
    * {@link DropBoxManager} and deleting contents.
    */
    CARAPI_(void) RecoverFromWtf();

private:
    static const String TAG;
    static const Boolean LOGD = FALSE;
    static const Boolean LOGV = FALSE;

    static const String TAG_NETSTATS_DUMP;

    /** Dump before deleting in {@link #recoverFromWtf()}. */
    static const Boolean DUMP_BEFORE_DELETE = TRUE;

    AutoPtr<IFileRotator> mRotator;
    AutoPtr<INetworkStatsNonMonotonicObserver> mObserver;
    AutoPtr<IDropBoxManager> mDropBox;
    String mCookie;

    Int64 mBucketDuration;
    Boolean mOnlyTags;

    Int64 mPersistThresholdBytes;
    AutoPtr<INetworkStats> mLastSnapshot;

    AutoPtr<NetworkStatsCollection> mPending;
    AutoPtr<NetworkStatsCollection> mSinceBoot;
    AutoPtr<CombiningRewriter> mPendingRewriter;

    AutoPtr<IWeakReference> mComplete;
};

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_NETWORKSTATSRECORDER_H__
