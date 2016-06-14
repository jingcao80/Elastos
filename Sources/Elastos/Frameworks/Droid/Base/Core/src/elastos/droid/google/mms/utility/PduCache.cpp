#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/utility/PduCache.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                               PduCache
//=====================================================================
CAR_INTERFACE_IMPL(PduCache, AbstractCache, IPduCache);

const String PduCache::TAG("PduCache");
const Boolean PduCache::DEBUG = FALSE;
const Boolean PduCache::LOCAL_LOGV = FALSE;
const Int32 PduCache::MMS_ALL;
const Int32 PduCache::MMS_ALL_ID;
const Int32 PduCache::MMS_INBOX;
const Int32 PduCache::MMS_INBOX_ID;
const Int32 PduCache::MMS_SENT;
const Int32 PduCache::MMS_SENT_ID;
const Int32 PduCache::MMS_DRAFTS;
const Int32 PduCache::MMS_DRAFTS_ID;
const Int32 PduCache::MMS_OUTBOX;
const Int32 PduCache::MMS_OUTBOX_ID;
const Int32 PduCache::MMS_CONVERSATION;
const Int32 PduCache::MMS_CONVERSATION_ID;
AutoPtr<IUriMatcher> PduCache::URI_MATCHER;
AutoPtr<IHashMap> PduCache::MATCH_TO_MSGBOX_ID_MAP;
AutoPtr<IPduCache> PduCache::sInstance;

// synchronized
AutoPtr<IPduCache> PduCache::GetInstance()
{
    // ==================before translated======================
    // if (sInstance == null) {
    //     if (LOCAL_LOGV) {
    //         Log.v(TAG, "Constructing new PduCache instance.");
    //     }
    //     sInstance = new PduCache();
    // }
    // return sInstance;
    assert(0);
    AutoPtr<PduCache> empty;
    return empty;
}

// synchronized
Boolean PduCache::Put(
    /* [in] */ IUri* uri,
    /* [in] */ IPduCacheEntry* entry)
{
    // ==================before translated======================
    // int msgBoxId = entry.getMessageBox();
    // HashSet<Uri> msgBox = mMessageBoxes.get(msgBoxId);
    // if (msgBox == null) {
    //     msgBox = new HashSet<Uri>();
    //     mMessageBoxes.put(msgBoxId, msgBox);
    // }
    //
    // long threadId = entry.getThreadId();
    // HashSet<Uri> thread = mThreads.get(threadId);
    // if (thread == null) {
    //     thread = new HashSet<Uri>();
    //     mThreads.put(threadId, thread);
    // }
    //
    // Uri finalKey = normalizeKey(uri);
    // boolean result = super.put(finalKey, entry);
    // if (result) {
    //     msgBox.add(finalKey);
    //     thread.add(finalKey);
    // }
    // setUpdating(uri, false);
    // return result;
    assert(0);
    return FALSE;
}

// synchronized
ECode PduCache::SetUpdating(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean updating)
{
    VALIDATE_NOT_NULL(uri);
    // ==================before translated======================
    // if (updating) {
    //     mUpdating.add(uri);
    // } else {
    //     mUpdating.remove(uri);
    // }
    assert(0);
    return NOERROR;
}

// synchronized
ECode PduCache::IsUpdating(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUpdating.contains(uri);
    assert(0);
    return NOERROR;
}

// synchronized
AutoPtr<IPduCacheEntry> PduCache::Purge(
    /* [in] */ IUri* uri)
{
    // ==================before translated======================
    // int match = URI_MATCHER.match(uri);
    // switch (match) {
    //     case MMS_ALL_ID:
    //         return purgeSingleEntry(uri);
    //     case MMS_INBOX_ID:
    //     case MMS_SENT_ID:
    //     case MMS_DRAFTS_ID:
    //     case MMS_OUTBOX_ID:
    //         String msgId = uri.getLastPathSegment();
    //         return purgeSingleEntry(Uri.withAppendedPath(Mms.CONTENT_URI, msgId));
    //     // Implicit batch of purge, return null.
    //     case MMS_ALL:
    //     case MMS_CONVERSATION:
    //         purgeAll();
    //         return null;
    //     case MMS_INBOX:
    //     case MMS_SENT:
    //     case MMS_DRAFTS:
    //     case MMS_OUTBOX:
    //         purgeByMessageBox(MATCH_TO_MSGBOX_ID_MAP.get(match));
    //         return null;
    //     case MMS_CONVERSATION_ID:
    //         purgeByThreadId(ContentUris.parseId(uri));
    //         return null;
    //     default:
    //         return null;
    // }
    assert(0);
    AutoPtr<IPduCacheEntry> empty;
    return empty;
}

// synchronized
ECode PduCache::PurgeAll()
{
    // ==================before translated======================
    // super.purgeAll();
    //
    // mMessageBoxes.clear();
    // mThreads.clear();
    // mUpdating.clear();
    assert(0);
    return NOERROR;
}

PduCache::PduCache()
{
    // ==================before translated======================
    // mMessageBoxes = new HashMap<Integer, HashSet<Uri>>();
    // mThreads = new HashMap<Long, HashSet<Uri>>();
    // mUpdating = new HashSet<Uri>();
}

AutoPtr<IPduCacheEntry> PduCache::PurgeSingleEntry(
    /* [in] */ IUri* key)
{
    // ==================before translated======================
    // mUpdating.remove(key);
    // PduCacheEntry entry = super.purge(key);
    // if (entry != null) {
    //     removeFromThreads(key, entry);
    //     removeFromMessageBoxes(key, entry);
    //     return entry;
    // }
    // return null;
    assert(0);
    AutoPtr<IPduCacheEntry> empty;
    return empty;
}

AutoPtr<IUri> PduCache::NormalizeKey(
    /* [in] */ IUri* uri)
{
    // ==================before translated======================
    // int match = URI_MATCHER.match(uri);
    // Uri normalizedKey = null;
    //
    // switch (match) {
    //     case MMS_ALL_ID:
    //         normalizedKey = uri;
    //         break;
    //     case MMS_INBOX_ID:
    //     case MMS_SENT_ID:
    //     case MMS_DRAFTS_ID:
    //     case MMS_OUTBOX_ID:
    //         String msgId = uri.getLastPathSegment();
    //         normalizedKey = Uri.withAppendedPath(Mms.CONTENT_URI, msgId);
    //         break;
    //     default:
    //         return null;
    // }
    //
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, uri + " -> " + normalizedKey);
    // }
    // return normalizedKey;
    assert(0);
    AutoPtr<IUri> empty;
    return empty;
}

void PduCache::PurgeByMessageBox(
    /* [in] */ IInteger32* msgBoxId)
{
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Purge cache in message box: " + msgBoxId);
    // }
    //
    // if (msgBoxId != null) {
    //     HashSet<Uri> msgBox = mMessageBoxes.remove(msgBoxId);
    //     if (msgBox != null) {
    //         for (Uri key : msgBox) {
    //             mUpdating.remove(key);
    //             PduCacheEntry entry = super.purge(key);
    //             if (entry != null) {
    //                 removeFromThreads(key, entry);
    //             }
    //         }
    //     }
    // }
    assert(0);
}

void PduCache::RemoveFromThreads(
    /* [in] */ IUri* key,
    /* [in] */ IPduCacheEntry* entry)
{
    // ==================before translated======================
    // HashSet<Uri> thread = mThreads.get(entry.getThreadId());
    // if (thread != null) {
    //     thread.remove(key);
    // }
    assert(0);
}

void PduCache::PurgeByThreadId(
    /* [in] */ Int64 threadId)
{
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Purge cache in thread: " + threadId);
    // }
    //
    // HashSet<Uri> thread = mThreads.remove(threadId);
    // if (thread != null) {
    //     for (Uri key : thread) {
    //         mUpdating.remove(key);
    //         PduCacheEntry entry = super.purge(key);
    //         if (entry != null) {
    //             removeFromMessageBoxes(key, entry);
    //         }
    //     }
    // }
    assert(0);
}

void PduCache::RemoveFromMessageBoxes(
    /* [in] */ IUri* key,
    /* [in] */ IPduCacheEntry* entry)
{
    // ==================before translated======================
    // HashSet<Uri> msgBox = mThreads.get(Long.valueOf(entry.getMessageBox()));
    // if (msgBox != null) {
    //     msgBox.remove(key);
    // }
    assert(0);
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
