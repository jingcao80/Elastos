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

#ifndef __ELASTOS_DROID_SERVER_NET_NETWORKSTATSCOLLECTION_H__
#define __ELASTOS_DROID_SERVER_NET_NETWORKSTATSCOLLECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/net/NetworkIdentitySet.h"
#include <Elastos.Droid.Internal.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Internal::Utility::IFileRotatorReader;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Net::INetworkIdentity;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Net::INetworkStatsHistoryEntry;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class NetworkStatsCollection
    : public Object
    , public IFileRotatorReader
{
public:
    class Key
        : public Object
    {
    public:
        Key(
            /* [in] */ NetworkIdentitySet* ident,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 set,
            /* [in] */ Int32 tag);

        using Object::Equals;

        CARAPI_(Boolean) Equals(
            /* [in] */ const Key* o) const;

    public:
        AutoPtr<NetworkIdentitySet> mIdent;
        Int32 mUid;
        Int32 mSet;
        Int32 mTag;

    private:
        Int32 mHashCode;
    };

    struct HashPK
    {
        size_t operator()(const Key* s) const
        {
            return (size_t)s;
        }
    };

    struct PKEq
    {
        Boolean operator()(const Key* x,
                           const Key* y) const
        {
            return x->Equals(y);
        }
    };

public:
    CAR_INTERFACE_DECL()

    NetworkStatsCollection(
        /* [in] */ Int64 bucketDuration);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    CARAPI_(void) Reset();

    CARAPI_(Int64) GetStartMillis();

    /**
     * Return first atomic bucket in this collection, which is more conservative
     * than {@link #mStartMillis}.
     */
    CARAPI_(Int64) GetFirstAtomicBucketMillis();

    CARAPI_(Int64) GetEndMillis();

    CARAPI_(Int64) GetTotalBytes();

    CARAPI_(Boolean) IsDirty();

    CARAPI_(void) ClearDirty();

    CARAPI_(Boolean) IsEmpty();

    /**
     * Combine all {@link NetworkStatsHistory} in this collection which match
     * the requested parameters.
     */
    CARAPI_(AutoPtr<INetworkStatsHistory>) GetHistory(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 fields);

    /**
     * Combine all {@link NetworkStatsHistory} in this collection which match
     * the requested parameters.
     */
    CARAPI_(AutoPtr<INetworkStatsHistory>) GetHistory(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 fields,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    /**
     * Summarize all {@link NetworkStatsHistory} in this collection which match
     * the requested parameters.
     */
    CARAPI_(AutoPtr<INetworkStats>) GetSummary(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    /**
     * Record given {@link android.net.NetworkStats.Entry} into this collection.
     */
    CARAPI_(void) RecordData(
        /* [in] */ NetworkIdentitySet* ident,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ INetworkStatsEntry* entry);

    /**
     * Record all {@link NetworkStatsHistory} contained in the given collection
     * into this collection.
     */
    CARAPI_(void) RecordCollection(
        /* [in] */ NetworkStatsCollection* another);

    //@Override
    CARAPI Read(
        /* [in] */ IInputStream* inputStream);

    CARAPI Read(
        /* [in] */ IDataInputStream* inputStream);

    CARAPI Write(
        /* [in] */ IDataOutputStream* outputStream);

    //@Deprecated
    CARAPI ReadLegacyNetwork(
        /* [in] */ IFile* file);

    //@Deprecated
    //public void readLegacyUid(File file, boolean onlyTags) throws IOException
    CARAPI ReadLegacyUid(
        /* [in] */ IFile* file,
        /* [in] */ Boolean onlyTags);

    /**
     * Remove any {@link NetworkStatsHistory} attributed to the requested UID,
     * moving any {@link NetworkStats#TAG_NONE} series to
     * {@link TrafficStats#UID_REMOVED}.
     */
    CARAPI_(void) RemoveUids(
        /* [in] */ ArrayOf<Int32>* uids);

    CARAPI_(void) Dump(
        /* [in] */ IIndentingPrintWriter* printWriter);

private:
    /**
     * Record given {@link NetworkStatsHistory} into this collection.
     */
    CARAPI_(void) RecordHistory(
        /* [in] */ Key* key,
        /* [in] */ INetworkStatsHistory* history);

    CARAPI_(AutoPtr<INetworkStatsHistory>) FindOrCreateHistory(
        /* [in] */ NetworkIdentitySet* ident,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag);

    CARAPI_(void) NoteRecordedHistory(
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int64 totalBytes);

    CARAPI_(Int32) EstimateBuckets();

    /**
     * Test if given {@link NetworkTemplate} matches any {@link NetworkIdentity}
     * in the given {@link NetworkIdentitySet}.
     */
    CARAPI_(Boolean) TemplateMatches(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ NetworkIdentitySet* identSet);

private:
    /** File header magic number: "ANET" */
    static const Int32 FILE_MAGIC = 0x414E4554;

    static const Int32 VERSION_NETWORK_INIT = 1;

    static const Int32 VERSION_UID_INIT = 1;
    static const Int32 VERSION_UID_WITH_IDENT = 2;
    static const Int32 VERSION_UID_WITH_TAG = 3;
    static const Int32 VERSION_UID_WITH_SET = 4;

    static const Int32 VERSION_UNIFIED_INIT = 16;

    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq> mStats;

    Int64 mBucketDuration;

    Int64 mStartMillis;
    Int64 mEndMillis;
    Int64 mTotalBytes;
    Boolean mDirty;
};

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_NETWORKSTATSCOLLECTION_H__
