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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/utility/PduCache.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/provider/CTelephonyMms.h"
#include "elastos/droid/net/CUriHelper.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Provider::CTelephonyMms;
using Elastos::Droid::Provider::ITelephonyMms;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

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
    if (sInstance == NULL) {
        if (LOCAL_LOGV) {
            Logger::V(TAG, String("Constructing new PduCache instance."));
        }
        sInstance = new PduCache();
    }
    return sInstance;
}

// synchronized
ECode PduCache::Put(
    /* [in] */ IInterface* uri,
    /* [in] */ IInterface* entry,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 msgBoxId = 0;
    IPduCacheEntry::Probe(entry)->GetMessageBox(&msgBoxId);
    AutoPtr<IInterface> p;
    mMessageBoxes->Get(CoreUtils::Convert(msgBoxId), (IInterface**)&p);
    AutoPtr<IHashSet> msgBox = IHashSet::Probe(p);
    if (msgBox == NULL) {
        CHashSet::New((IHashSet**)&msgBox);
        mMessageBoxes->Put(CoreUtils::Convert(msgBoxId), msgBox);
    }

    Int64 threadId = 0;
    IPduCacheEntry::Probe(entry)->GetThreadId(&threadId);
    AutoPtr<IInterface> pT;
    mThreads->Get(CoreUtils::Convert(threadId), (IInterface**)&pT);
    AutoPtr<IHashSet> thread = IHashSet::Probe(pT);
    if (thread == NULL) {
        CHashSet::New((IHashSet**)&thread);
        mThreads->Put(CoreUtils::Convert(threadId), thread);
    }

    AutoPtr<IUri> finalKey = NormalizeKey(IUri::Probe(uri));
    AbstractCache::Put(finalKey, entry, result);
    if (*result) {
        msgBox->Add(finalKey);
        thread->Add(finalKey);
    }
    SetUpdating(IUri::Probe(uri), FALSE);
    return NOERROR;
}

// synchronized
ECode PduCache::SetUpdating(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean updating)
{
    VALIDATE_NOT_NULL(uri);
    if (updating) {
        mUpdating->Add(uri);
    }
    else {
        mUpdating->Remove(uri);
    }
    return NOERROR;
}

// synchronized
ECode PduCache::IsUpdating(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mUpdating->Contains(uri, result);
}

// synchronized
ECode PduCache::Purge(
    /* [in] */ IInterface* uri,
    /* [out] */ IInterface** result)
{
    Int32 match = 0;
    URI_MATCHER->Match(IUri::Probe(uri), &match);
    switch (match) {
        case MMS_ALL_ID: {
            AutoPtr<IPduCacheEntry> entry = PurgeSingleEntry(IUri::Probe(uri));
            *result = entry;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            String msgId;
            IUri::Probe(uri)->GetLastPathSegment(&msgId);
            AutoPtr<IUriHelper> uhlp;
            CUriHelper::AcquireSingleton((IUriHelper**)&uhlp);
            AutoPtr<ITelephonyMms> tmms;
            CTelephonyMms::AcquireSingleton((ITelephonyMms**)&tmms);
            AutoPtr<IUri> content_uri;
            tmms->GetCONTENT_URI((IUri**)&content_uri);
            AutoPtr<IUri> newUri;
            uhlp->WithAppendedPath(content_uri, msgId, (IUri**)&newUri);
            AutoPtr<IPduCacheEntry> entry = PurgeSingleEntry(newUri);
            *result = entry;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        // Implicit batch of purge, return NULL.
        case MMS_ALL:
        case MMS_CONVERSATION:
            PurgeAll();
            *result = NULL;
            return NOERROR;
        case MMS_INBOX:
        case MMS_SENT:
        case MMS_DRAFTS:
        case MMS_OUTBOX: {
            AutoPtr<IInterface> p;
            MATCH_TO_MSGBOX_ID_MAP->Get(CoreUtils::Convert(match), (IInterface**)&p);
            PurgeByMessageBox(IInteger32::Probe(p));
            *result = NULL;
            return NOERROR;
        }
        case MMS_CONVERSATION_ID: {
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            Int64 id = 0;
            cu->ParseId(IUri::Probe(uri), &id);
            PurgeByThreadId(id);
            *result = NULL;
            return NOERROR;
        }
        default:
            *result = NULL;
            return NOERROR;
    }
}

// synchronized
ECode PduCache::PurgeAll()
{
    AbstractCache::PurgeAll();

    mMessageBoxes->Clear();
    mThreads->Clear();
    mUpdating->Clear();
    return NOERROR;
}

PduCache::PduCache()
{
    CHashMap::New((IHashMap**)&mMessageBoxes);
    CHashMap::New((IHashMap**)&mThreads);
    CHashSet::New((IHashSet**)&mUpdating);
}

AutoPtr<IPduCacheEntry> PduCache::PurgeSingleEntry(
    /* [in] */ IUri* key)
{
    mUpdating->Remove(key);
    AutoPtr<IInterface> p;
    AbstractCache::Purge(key, (IInterface**)&p);
    AutoPtr<IPduCacheEntry> entry = IPduCacheEntry::Probe(p);
    if (entry != NULL) {
        RemoveFromThreads(key, entry);
        RemoveFromMessageBoxes(key, entry);
        return entry;
    }
    return NULL;
}

AutoPtr<IUri> PduCache::NormalizeKey(
    /* [in] */ IUri* uri)
{
    Int32 match = 0;
    URI_MATCHER->Match(uri, &match);
    AutoPtr<IUri> normalizedKey;

    switch (match) {
        case MMS_ALL_ID: {
            normalizedKey = uri;
            break;
        }
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            String msgId;
            uri->GetLastPathSegment(&msgId);
            AutoPtr<IUriHelper> uhlp;
            CUriHelper::AcquireSingleton((IUriHelper**)&uhlp);
            AutoPtr<ITelephonyMms> tmms;
            CTelephonyMms::AcquireSingleton((ITelephonyMms**)&tmms);
            AutoPtr<IUri> content_uri;
            tmms->GetCONTENT_URI((IUri**)&content_uri);
            AutoPtr<IUri> uri;
            uhlp->WithAppendedPath(content_uri, msgId, (IUri**)&uri);
            normalizedKey = uri;
            break;
        }
        default:
            return NULL;
    }

    if (LOCAL_LOGV) {
        Logger::V(TAG, String(TO_CSTR(uri)) + String(" -> ") + String(TO_CSTR(normalizedKey)));
    }
    return normalizedKey;
}

void PduCache::PurgeByMessageBox(
    /* [in] */ IInteger32* msgBoxId)
{
    if (LOCAL_LOGV) {
        Int32 id = 0;
        msgBoxId->GetValue(&id);
        Logger::V(TAG, String("Purge cache in message box: ") + StringUtils::ToString(id));
    }

    if (msgBoxId != NULL) {
        AutoPtr<IInterface> pMB;
        mMessageBoxes->Remove(msgBoxId, (IInterface**)&pMB);
        AutoPtr<IHashSet> msgBox = IHashSet::Probe(pMB);
        if (msgBox != NULL) {
            AutoPtr<IIterator> it;
            msgBox->GetIterator((IIterator**)&it);
            Boolean bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IUri> key = IUri::Probe(p);
                mUpdating->Remove(key);
                AutoPtr<IInterface> pE;
                AbstractCache::Purge(key, (IInterface**)&pE);
                AutoPtr<IPduCacheEntry> entry = IPduCacheEntry::Probe(pE);
                if (entry != NULL) {
                    RemoveFromThreads(key, entry);
                }
            }
        }
    }
}

void PduCache::RemoveFromThreads(
    /* [in] */ IUri* key,
    /* [in] */ IPduCacheEntry* entry)
{
    Int64 threadId = 0;
    entry->GetThreadId(&threadId);
    AutoPtr<IInterface> pT;
    mThreads->Get(CoreUtils::Convert(threadId), (IInterface**)&pT);
    AutoPtr<IHashSet> thread = IHashSet::Probe(pT);
    if (thread != NULL) {
        thread->Remove(key);
    }
}

void PduCache::PurgeByThreadId(
    /* [in] */ Int64 threadId)
{
    if (LOCAL_LOGV) {
        Logger::V(TAG, String("Purge cache in thread: ") + StringUtils::ToString(threadId));
    }

    AutoPtr<IInterface> pT;
    mThreads->Remove(CoreUtils::Convert(threadId), (IInterface**)&pT);
    AutoPtr<IHashSet> thread = IHashSet::Probe(pT);
    if (thread != NULL) {
        AutoPtr<IIterator> it;
        thread->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IUri> key = IUri::Probe(p);
            mUpdating->Remove(key);
            AutoPtr<IInterface> pE;
            AbstractCache::Purge(key, (IInterface**)&pE);
            AutoPtr<IPduCacheEntry> entry = IPduCacheEntry::Probe(pE);
            if (entry != NULL) {
                RemoveFromMessageBoxes(key, entry);
            }
        }
    }
}

void PduCache::RemoveFromMessageBoxes(
    /* [in] */ IUri* key,
    /* [in] */ IPduCacheEntry* entry)
{
    Int32 mb = 0;
    entry->GetMessageBox(&mb);
    AutoPtr<IInterface> p;
    mThreads->Get(CoreUtils::Convert(mb), (IInterface**)&p);
    AutoPtr<IHashSet> msgBox = IHashSet::Probe(p);
    if (msgBox != NULL) {
        msgBox->Remove(key);
    }
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
