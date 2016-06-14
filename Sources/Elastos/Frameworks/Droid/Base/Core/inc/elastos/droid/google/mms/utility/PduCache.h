#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/utility/AbstractCache.h"

// package com.google.android.mms.util;
// import android.content.ContentUris;
// import android.content.UriMatcher;
// import android.net.Uri;
// import android.provider.Telephony.Mms;
// import android.util.Log;
// import java.util.HashMap;
// import java.util.HashSet;

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
    CARAPI_(Boolean) Put(
        /* [in] */ IUri* uri,
        /* [in] */ IPduCacheEntry* entry);

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
    CARAPI_(AutoPtr<IPduCacheEntry>) Purge(
        /* [in] */ IUri* uri);

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

