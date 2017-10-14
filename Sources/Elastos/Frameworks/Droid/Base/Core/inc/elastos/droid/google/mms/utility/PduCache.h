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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/utility/AbstractCache.h"

using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class PduCache
    : public AbstractCache//<Uri, PduCacheEntry>
    , public IPduCache
{
public:
    CAR_INTERFACE_DECL();

    // synchronized
    static CARAPI_(AutoPtr<IPduCache>) GetInstance();

    // @Override
    // synchronized
    CARAPI Put(
        /* [in] */ IInterface* uri,
        /* [in] */ IInterface* entry,
        /* [out] */ Boolean* result);

    // synchronized
    virtual CARAPI SetUpdating(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean updating);

    // synchronized
    virtual CARAPI IsUpdating(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI Purge(
        /* [in] */ IInterface* uri,
        /* [out] */ IInterface** result);

    // @Override
    // synchronized
    CARAPI PurgeAll();

private:
    PduCache();

    CARAPI_(AutoPtr<IPduCacheEntry>) PurgeSingleEntry(
        /* [in] */ IUri* key);

    /**
      * @param uri The Uri to be normalized.
      * @return Uri The normalized key of cached entry.
      */
    CARAPI_(AutoPtr<IUri>) NormalizeKey(
        /* [in] */ IUri* uri);

    CARAPI_(void) PurgeByMessageBox(
        /* [in] */ IInteger32* msgBoxId);

    CARAPI_(void) RemoveFromThreads(
        /* [in] */ IUri* key,
        /* [in] */ IPduCacheEntry* entry);

    CARAPI_(void) PurgeByThreadId(
        /* [in] */ Int64 threadId);

    CARAPI_(void) RemoveFromMessageBoxes(
        /* [in] */ IUri* key,
        /* [in] */ IPduCacheEntry* entry);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean LOCAL_LOGV;
    static const Int32 MMS_ALL = 0;
    static const Int32 MMS_ALL_ID = 1;
    static const Int32 MMS_INBOX = 2;
    static const Int32 MMS_INBOX_ID = 3;
    static const Int32 MMS_SENT = 4;
    static const Int32 MMS_SENT_ID = 5;
    static const Int32 MMS_DRAFTS = 6;
    static const Int32 MMS_DRAFTS_ID = 7;
    static const Int32 MMS_OUTBOX = 8;
    static const Int32 MMS_OUTBOX_ID = 9;
    static const Int32 MMS_CONVERSATION = 10;
    static const Int32 MMS_CONVERSATION_ID = 11;
    static AutoPtr<IUriMatcher> URI_MATCHER;
    static AutoPtr<IHashMap> MATCH_TO_MSGBOX_ID_MAP;//Integer,Integer
    static AutoPtr<IPduCache> sInstance;
    /*const*/ AutoPtr<IHashMap> mMessageBoxes;//HashMap<Integer, HashSet<Uri>>
    /*const*/ AutoPtr<IHashMap> mThreads;//HashMap<Long, HashSet<Uri>>
    /*const*/ AutoPtr<IHashSet> mUpdating;//IUri
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHE_H__

