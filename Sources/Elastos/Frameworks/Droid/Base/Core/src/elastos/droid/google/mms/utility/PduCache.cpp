/*
 * Copyright (C) 2008 Esmertec AG.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.android.mms.util;

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::Telephony::IMms;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;

public class PduCache extends AbstractCache<Uri, PduCacheEntry> {
    private static const String TAG = "PduCache";
    private static const Boolean DEBUG = FALSE;
    private static const Boolean LOCAL_LOGV = FALSE;

    private static const Int32 MMS_ALL             = 0;
    private static const Int32 MMS_ALL_ID          = 1;
    private static const Int32 MMS_INBOX           = 2;
    private static const Int32 MMS_INBOX_ID        = 3;
    private static const Int32 MMS_SENT            = 4;
    private static const Int32 MMS_SENT_ID         = 5;
    private static const Int32 MMS_DRAFTS          = 6;
    private static const Int32 MMS_DRAFTS_ID       = 7;
    private static const Int32 MMS_OUTBOX          = 8;
    private static const Int32 MMS_OUTBOX_ID       = 9;
    private static const Int32 MMS_CONVERSATION    = 10;
    private static const Int32 MMS_CONVERSATION_ID = 11;

    private static const UriMatcher URI_MATCHER;
    private static const HashMap<Integer, Integer> MATCH_TO_MSGBOX_ID_MAP;

    private static PduCache sInstance;

    static {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);
        URI_MATCHER->AddURI("mms", NULL,         MMS_ALL);
        URI_MATCHER->AddURI("mms", "#",          MMS_ALL_ID);
        URI_MATCHER->AddURI("mms", "inbox",      MMS_INBOX);
        URI_MATCHER->AddURI("mms", "inbox/#",    MMS_INBOX_ID);
        URI_MATCHER->AddURI("mms", "sent",       MMS_SENT);
        URI_MATCHER->AddURI("mms", "sent/#",     MMS_SENT_ID);
        URI_MATCHER->AddURI("mms", "drafts",     MMS_DRAFTS);
        URI_MATCHER->AddURI("mms", "drafts/#",   MMS_DRAFTS_ID);
        URI_MATCHER->AddURI("mms", "outbox",     MMS_OUTBOX);
        URI_MATCHER->AddURI("mms", "outbox/#",   MMS_OUTBOX_ID);
        URI_MATCHER->AddURI("mms-sms", "conversations",   MMS_CONVERSATION);
        URI_MATCHER->AddURI("mms-sms", "conversations/#", MMS_CONVERSATION_ID);

        MATCH_TO_MSGBOX_ID_MAP = new HashMap<Integer, Integer>();
        MATCH_TO_MSGBOX_ID_MAP->Put(MMS_INBOX,  Mms.MESSAGE_BOX_INBOX);
        MATCH_TO_MSGBOX_ID_MAP->Put(MMS_SENT,   Mms.MESSAGE_BOX_SENT);
        MATCH_TO_MSGBOX_ID_MAP->Put(MMS_DRAFTS, Mms.MESSAGE_BOX_DRAFTS);
        MATCH_TO_MSGBOX_ID_MAP->Put(MMS_OUTBOX, Mms.MESSAGE_BOX_OUTBOX);
    }

    private final HashMap<Integer, HashSet<Uri>> mMessageBoxes;
    private final HashMap<Long, HashSet<Uri>> mThreads;
    private final HashSet<Uri> mUpdating;

    private PduCache() {
        mMessageBoxes = new HashMap<Integer, HashSet<Uri>>();
        mThreads = new HashMap<Long, HashSet<Uri>>();
        mUpdating = new HashSet<Uri>();
    }

    synchronized public static const PduCache GetInstance() {
        If (sInstance == NULL) {
            If (LOCAL_LOGV) {
                Logger::V(TAG, "Constructing new PduCache instance.");
            }
            sInstance = new PduCache();
        }
        return sInstance;
    }

    //@Override
    synchronized public Boolean Put(Uri uri, PduCacheEntry entry) {
        Int32 msgBoxId = entry->GetMessageBox();
        HashSet<Uri> msgBox = mMessageBoxes->Get(msgBoxId);
        If (msgBox == NULL) {
            msgBox = new HashSet<Uri>();
            mMessageBoxes->Put(msgBoxId, msgBox);
        }

        Int64 threadId = entry->GetThreadId();
        HashSet<Uri> thread = mThreads->Get(threadId);
        If (thread == NULL) {
            thread = new HashSet<Uri>();
            mThreads->Put(threadId, thread);
        }

        Uri finalKey = NormalizeKey(uri);
        Boolean result = super->Put(finalKey, entry);
        If (result) {
            msgBox->Add(finalKey);
            thread->Add(finalKey);
        }
        SetUpdating(uri, FALSE);
        return result;
    }

    synchronized CARAPI SetUpdating(Uri uri, Boolean updating) {
        If (updating) {
            mUpdating->Add(uri);
        } else {
            mUpdating->Remove(uri);
        }
    }

    synchronized public Boolean IsUpdating(Uri uri) {
        return mUpdating->Contains(uri);
    }

    //@Override
    synchronized public PduCacheEntry Purge(Uri uri) {
        Int32 match = URI_MATCHER->Match(uri);
        Switch (match) {
            case MMS_ALL_ID:
                return PurgeSingleEntry(uri);
            case MMS_INBOX_ID:
            case MMS_SENT_ID:
            case MMS_DRAFTS_ID:
            case MMS_OUTBOX_ID:
                String msgId = uri->GetLastPathSegment();
                return PurgeSingleEntry(Uri->WithAppendedPath(Mms.CONTENT_URI, msgId));
            // Implicit batch of purge, return NULL.
            case MMS_ALL:
            case MMS_CONVERSATION:
                PurgeAll();
                return NULL;
            case MMS_INBOX:
            case MMS_SENT:
            case MMS_DRAFTS:
            case MMS_OUTBOX:
                PurgeByMessageBox(MATCH_TO_MSGBOX_ID_MAP->Get(match));
                return NULL;
            case MMS_CONVERSATION_ID:
                PurgeByThreadId(ContentUris->ParseId(uri));
                return NULL;
            default:
                return NULL;
        }
    }

    private PduCacheEntry PurgeSingleEntry(Uri key) {
        mUpdating->Remove(key);
        PduCacheEntry entry = super->Purge(key);
        If (entry != NULL) {
            RemoveFromThreads(key, entry);
            RemoveFromMessageBoxes(key, entry);
            return entry;
        }
        return NULL;
    }

    //@Override
    synchronized CARAPI PurgeAll() {
        super->PurgeAll();

        mMessageBoxes->Clear();
        mThreads->Clear();
        mUpdating->Clear();
    }

    /**
     * @param uri The Uri to be normalized.
     * @return Uri The normalized key of cached entry.
     */
    private Uri NormalizeKey(Uri uri) {
        Int32 match = URI_MATCHER->Match(uri);
        Uri normalizedKey = NULL;

        Switch (match) {
            case MMS_ALL_ID:
                normalizedKey = uri;
                break;
            case MMS_INBOX_ID:
            case MMS_SENT_ID:
            case MMS_DRAFTS_ID:
            case MMS_OUTBOX_ID:
                String msgId = uri->GetLastPathSegment();
                normalizedKey = Uri->WithAppendedPath(Mms.CONTENT_URI, msgId);
                break;
            default:
                return NULL;
        }

        If (LOCAL_LOGV) {
            Logger::V(TAG, uri + " -> " + normalizedKey);
        }
        return normalizedKey;
    }

    private void PurgeByMessageBox(Integer msgBoxId) {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Purge cache in message box: " + msgBoxId);
        }

        If (msgBoxId != NULL) {
            HashSet<Uri> msgBox = mMessageBoxes->Remove(msgBoxId);
            If (msgBox != NULL) {
                For (Uri key : msgBox) {
                    mUpdating->Remove(key);
                    PduCacheEntry entry = super->Purge(key);
                    If (entry != NULL) {
                        RemoveFromThreads(key, entry);
                    }
                }
            }
        }
    }

    private void RemoveFromThreads(Uri key, PduCacheEntry entry) {
        HashSet<Uri> thread = mThreads->Get(entry->GetThreadId());
        If (thread != NULL) {
            thread->Remove(key);
        }
    }

    private void PurgeByThreadId(Int64 threadId) {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Purge cache in thread: " + threadId);
        }

        HashSet<Uri> thread = mThreads->Remove(threadId);
        If (thread != NULL) {
            For (Uri key : thread) {
                mUpdating->Remove(key);
                PduCacheEntry entry = super->Purge(key);
                If (entry != NULL) {
                    RemoveFromMessageBoxes(key, entry);
                }
            }
        }
    }

    private void RemoveFromMessageBoxes(Uri key, PduCacheEntry entry) {
        HashSet<Uri> msgBox = mThreads->Get(Long->ValueOf(entry->GetMessageBox()));
        If (msgBox != NULL) {
            msgBox->Remove(key);
        }
    }
}
