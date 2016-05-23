/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
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

package com.google.android.mms.pdu;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using com::Google::Android::Mms::IContentType;
using com::Google::Android::Mms::Utility::IDownloadDrmHelper;
using com::Google::Android::Mms::Utility::IDrmConvertSession;
using com::Google::Android::Mms::Utility::IPduCache;
using com::Google::Android::Mms::Utility::IPduCacheEntry;
using com::Google::Android::Mms::Utility::ISqliteWrapper;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Drm::IDrmManagerClient;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::Telephony::IMms;
using Elastos::Droid::Provider::Telephony::IMmsSms;
using Elastos::Droid::Provider::Telephony::IThreads;
using Elastos::Droid::Provider::Telephony::Mms::IAddr;
using Elastos::Droid::Provider::Telephony::Mms::IPart;
using Elastos::Droid::Provider::Telephony::MmsSms::IPendingMessages;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Map::IEntry;

using com::Google::Android::Mms::Pdu::IEncodedStringValue;

/**
 * This class is the high-level manager of PDU storage.
 */
public class PduPersister {
    private static const String TAG = "PduPersister";
    private static const Boolean DEBUG = FALSE;
    private static const Boolean LOCAL_LOGV = FALSE;

    private static const Int64 DUMMY_THREAD_ID = Long.MAX_VALUE;

    /**
     * The uri of temporary drm objects.
     */
    public static const String TEMPORARY_DRM_OBJECT_URI =
        "content://mms/" + Long.MAX_VALUE + "/part";
    /**
     * Indicate that we transiently failed to process a MM.
     */
    public static const Int32 PROC_STATUS_TRANSIENT_FAILURE   = 1;
    /**
     * Indicate that we permanently failed to process a MM.
     */
    public static const Int32 PROC_STATUS_PERMANENTLY_FAILURE = 2;
    /**
     * Indicate that we have successfully processed a MM.
     */
    public static const Int32 PROC_STATUS_COMPLETED           = 3;

    private static PduPersister sPersister;
    private static const PduCache PDU_CACHE_INSTANCE;

    private static const Int32[] ADDRESS_FIELDS = new Int32[] {
            PduHeaders.BCC,
            PduHeaders.CC,
            PduHeaders.FROM,
            PduHeaders.TO
    };

    private static const String[] PDU_PROJECTION = new String[] {
        Mms._ID,
        Mms.MESSAGE_BOX,
        Mms.THREAD_ID,
        Mms.RETRIEVE_TEXT,
        Mms.SUBJECT,
        Mms.CONTENT_LOCATION,
        Mms.CONTENT_TYPE,
        Mms.MESSAGE_CLASS,
        Mms.MESSAGE_ID,
        Mms.RESPONSE_TEXT,
        Mms.TRANSACTION_ID,
        Mms.CONTENT_CLASS,
        Mms.DELIVERY_REPORT,
        Mms.MESSAGE_TYPE,
        Mms.MMS_VERSION,
        Mms.PRIORITY,
        Mms.READ_REPORT,
        Mms.READ_STATUS,
        Mms.REPORT_ALLOWED,
        Mms.RETRIEVE_STATUS,
        Mms.STATUS,
        Mms.DATE,
        Mms.DELIVERY_TIME,
        Mms.EXPIRY,
        Mms.MESSAGE_SIZE,
        Mms.SUBJECT_CHARSET,
        Mms.RETRIEVE_TEXT_CHARSET,
    };

    private static const Int32 PDU_COLUMN_ID                    = 0;
    private static const Int32 PDU_COLUMN_MESSAGE_BOX           = 1;
    private static const Int32 PDU_COLUMN_THREAD_ID             = 2;
    private static const Int32 PDU_COLUMN_RETRIEVE_TEXT         = 3;
    private static const Int32 PDU_COLUMN_SUBJECT               = 4;
    private static const Int32 PDU_COLUMN_CONTENT_LOCATION      = 5;
    private static const Int32 PDU_COLUMN_CONTENT_TYPE          = 6;
    private static const Int32 PDU_COLUMN_MESSAGE_CLASS         = 7;
    private static const Int32 PDU_COLUMN_MESSAGE_ID            = 8;
    private static const Int32 PDU_COLUMN_RESPONSE_TEXT         = 9;
    private static const Int32 PDU_COLUMN_TRANSACTION_ID        = 10;
    private static const Int32 PDU_COLUMN_CONTENT_CLASS         = 11;
    private static const Int32 PDU_COLUMN_DELIVERY_REPORT       = 12;
    private static const Int32 PDU_COLUMN_MESSAGE_TYPE          = 13;
    private static const Int32 PDU_COLUMN_MMS_VERSION           = 14;
    private static const Int32 PDU_COLUMN_PRIORITY              = 15;
    private static const Int32 PDU_COLUMN_READ_REPORT           = 16;
    private static const Int32 PDU_COLUMN_READ_STATUS           = 17;
    private static const Int32 PDU_COLUMN_REPORT_ALLOWED        = 18;
    private static const Int32 PDU_COLUMN_RETRIEVE_STATUS       = 19;
    private static const Int32 PDU_COLUMN_STATUS                = 20;
    private static const Int32 PDU_COLUMN_DATE                  = 21;
    private static const Int32 PDU_COLUMN_DELIVERY_TIME         = 22;
    private static const Int32 PDU_COLUMN_EXPIRY                = 23;
    private static const Int32 PDU_COLUMN_MESSAGE_SIZE          = 24;
    private static const Int32 PDU_COLUMN_SUBJECT_CHARSET       = 25;
    private static const Int32 PDU_COLUMN_RETRIEVE_TEXT_CHARSET = 26;

    private static const String[] PART_PROJECTION = new String[] {
        Part._ID,
        Part.CHARSET,
        Part.CONTENT_DISPOSITION,
        Part.CONTENT_ID,
        Part.CONTENT_LOCATION,
        Part.CONTENT_TYPE,
        Part.FILENAME,
        Part.NAME,
        Part.TEXT
    };

    private static const Int32 PART_COLUMN_ID                  = 0;
    private static const Int32 PART_COLUMN_CHARSET             = 1;
    private static const Int32 PART_COLUMN_CONTENT_DISPOSITION = 2;
    private static const Int32 PART_COLUMN_CONTENT_ID          = 3;
    private static const Int32 PART_COLUMN_CONTENT_LOCATION    = 4;
    private static const Int32 PART_COLUMN_CONTENT_TYPE        = 5;
    private static const Int32 PART_COLUMN_FILENAME            = 6;
    private static const Int32 PART_COLUMN_NAME                = 7;
    private static const Int32 PART_COLUMN_TEXT                = 8;

    private static const HashMap<Uri, Integer> MESSAGE_BOX_MAP;
    // These map are used for convenience in Persist() and Load().
    private static const HashMap<Integer, Integer> CHARSET_COLUMN_INDEX_MAP;
    private static const HashMap<Integer, Integer> ENCODED_STRING_COLUMN_INDEX_MAP;
    private static const HashMap<Integer, Integer> TEXT_STRING_COLUMN_INDEX_MAP;
    private static const HashMap<Integer, Integer> OCTET_COLUMN_INDEX_MAP;
    private static const HashMap<Integer, Integer> LONG_COLUMN_INDEX_MAP;
    private static const HashMap<Integer, String> CHARSET_COLUMN_NAME_MAP;
    private static const HashMap<Integer, String> ENCODED_STRING_COLUMN_NAME_MAP;
    private static const HashMap<Integer, String> TEXT_STRING_COLUMN_NAME_MAP;
    private static const HashMap<Integer, String> OCTET_COLUMN_NAME_MAP;
    private static const HashMap<Integer, String> LONG_COLUMN_NAME_MAP;

    static {
        MESSAGE_BOX_MAP = new HashMap<Uri, Integer>();
        MESSAGE_BOX_MAP->Put(Mms.Inbox.CONTENT_URI,  Mms.MESSAGE_BOX_INBOX);
        MESSAGE_BOX_MAP->Put(Mms.Sent.CONTENT_URI,   Mms.MESSAGE_BOX_SENT);
        MESSAGE_BOX_MAP->Put(Mms.Draft.CONTENT_URI,  Mms.MESSAGE_BOX_DRAFTS);
        MESSAGE_BOX_MAP->Put(Mms.Outbox.CONTENT_URI, Mms.MESSAGE_BOX_OUTBOX);

        CHARSET_COLUMN_INDEX_MAP = new HashMap<Integer, Integer>();
        CHARSET_COLUMN_INDEX_MAP->Put(PduHeaders.SUBJECT, PDU_COLUMN_SUBJECT_CHARSET);
        CHARSET_COLUMN_INDEX_MAP->Put(PduHeaders.RETRIEVE_TEXT, PDU_COLUMN_RETRIEVE_TEXT_CHARSET);

        CHARSET_COLUMN_NAME_MAP = new HashMap<Integer, String>();
        CHARSET_COLUMN_NAME_MAP->Put(PduHeaders.SUBJECT, Mms.SUBJECT_CHARSET);
        CHARSET_COLUMN_NAME_MAP->Put(PduHeaders.RETRIEVE_TEXT, Mms.RETRIEVE_TEXT_CHARSET);

        // Encoded string field code -> column index/name map.
        ENCODED_STRING_COLUMN_INDEX_MAP = new HashMap<Integer, Integer>();
        ENCODED_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.RETRIEVE_TEXT, PDU_COLUMN_RETRIEVE_TEXT);
        ENCODED_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.SUBJECT, PDU_COLUMN_SUBJECT);

        ENCODED_STRING_COLUMN_NAME_MAP = new HashMap<Integer, String>();
        ENCODED_STRING_COLUMN_NAME_MAP->Put(PduHeaders.RETRIEVE_TEXT, Mms.RETRIEVE_TEXT);
        ENCODED_STRING_COLUMN_NAME_MAP->Put(PduHeaders.SUBJECT, Mms.SUBJECT);

        // Text string field code -> column index/name map.
        TEXT_STRING_COLUMN_INDEX_MAP = new HashMap<Integer, Integer>();
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.CONTENT_LOCATION, PDU_COLUMN_CONTENT_LOCATION);
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.CONTENT_TYPE, PDU_COLUMN_CONTENT_TYPE);
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.MESSAGE_CLASS, PDU_COLUMN_MESSAGE_CLASS);
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.MESSAGE_ID, PDU_COLUMN_MESSAGE_ID);
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.RESPONSE_TEXT, PDU_COLUMN_RESPONSE_TEXT);
        TEXT_STRING_COLUMN_INDEX_MAP->Put(PduHeaders.TRANSACTION_ID, PDU_COLUMN_TRANSACTION_ID);

        TEXT_STRING_COLUMN_NAME_MAP = new HashMap<Integer, String>();
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.CONTENT_LOCATION, Mms.CONTENT_LOCATION);
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.CONTENT_TYPE, Mms.CONTENT_TYPE);
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.MESSAGE_CLASS, Mms.MESSAGE_CLASS);
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.MESSAGE_ID, Mms.MESSAGE_ID);
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.RESPONSE_TEXT, Mms.RESPONSE_TEXT);
        TEXT_STRING_COLUMN_NAME_MAP->Put(PduHeaders.TRANSACTION_ID, Mms.TRANSACTION_ID);

        // Octet field code -> column index/name map.
        OCTET_COLUMN_INDEX_MAP = new HashMap<Integer, Integer>();
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.CONTENT_CLASS, PDU_COLUMN_CONTENT_CLASS);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.DELIVERY_REPORT, PDU_COLUMN_DELIVERY_REPORT);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.MESSAGE_TYPE, PDU_COLUMN_MESSAGE_TYPE);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.MMS_VERSION, PDU_COLUMN_MMS_VERSION);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.PRIORITY, PDU_COLUMN_PRIORITY);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.READ_REPORT, PDU_COLUMN_READ_REPORT);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.READ_STATUS, PDU_COLUMN_READ_STATUS);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.REPORT_ALLOWED, PDU_COLUMN_REPORT_ALLOWED);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.RETRIEVE_STATUS, PDU_COLUMN_RETRIEVE_STATUS);
        OCTET_COLUMN_INDEX_MAP->Put(PduHeaders.STATUS, PDU_COLUMN_STATUS);

        OCTET_COLUMN_NAME_MAP = new HashMap<Integer, String>();
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.CONTENT_CLASS, Mms.CONTENT_CLASS);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.DELIVERY_REPORT, Mms.DELIVERY_REPORT);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.MESSAGE_TYPE, Mms.MESSAGE_TYPE);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.MMS_VERSION, Mms.MMS_VERSION);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.PRIORITY, Mms.PRIORITY);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.READ_REPORT, Mms.READ_REPORT);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.READ_STATUS, Mms.READ_STATUS);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.REPORT_ALLOWED, Mms.REPORT_ALLOWED);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.RETRIEVE_STATUS, Mms.RETRIEVE_STATUS);
        OCTET_COLUMN_NAME_MAP->Put(PduHeaders.STATUS, Mms.STATUS);

        // Long field code -> column index/name map.
        LONG_COLUMN_INDEX_MAP = new HashMap<Integer, Integer>();
        LONG_COLUMN_INDEX_MAP->Put(PduHeaders.DATE, PDU_COLUMN_DATE);
        LONG_COLUMN_INDEX_MAP->Put(PduHeaders.DELIVERY_TIME, PDU_COLUMN_DELIVERY_TIME);
        LONG_COLUMN_INDEX_MAP->Put(PduHeaders.EXPIRY, PDU_COLUMN_EXPIRY);
        LONG_COLUMN_INDEX_MAP->Put(PduHeaders.MESSAGE_SIZE, PDU_COLUMN_MESSAGE_SIZE);

        LONG_COLUMN_NAME_MAP = new HashMap<Integer, String>();
        LONG_COLUMN_NAME_MAP->Put(PduHeaders.DATE, Mms.DATE);
        LONG_COLUMN_NAME_MAP->Put(PduHeaders.DELIVERY_TIME, Mms.DELIVERY_TIME);
        LONG_COLUMN_NAME_MAP->Put(PduHeaders.EXPIRY, Mms.EXPIRY);
        LONG_COLUMN_NAME_MAP->Put(PduHeaders.MESSAGE_SIZE, Mms.MESSAGE_SIZE);

        PDU_CACHE_INSTANCE = PduCache->GetInstance();
     }

    private final Context mContext;
    private final ContentResolver mContentResolver;
    private final DrmManagerClient mDrmManagerClient;
    private final TelephonyManager mTelephonyManager;

    private PduPersister(Context context) {
        mContext = context;
        mContentResolver = context->GetContentResolver();
        mDrmManagerClient = new DrmManagerClient(context);
        mTelephonyManager = (TelephonyManager)context
                .GetSystemService(Context.TELEPHONY_SERVICE);
     }

    /** Get(or create if not exist) an instance of PduPersister */
    public static PduPersister GetPduPersister(Context context) {
        If ((sPersister == NULL)) {
            sPersister = new PduPersister(context);
        } else If (!context->Equals(sPersister.mContext)) {
            sPersister->Release();
            sPersister = new PduPersister(context);
        }

        return sPersister;
    }

    private void SetEncodedStringValueToHeaders(
            Cursor c, Int32 columnIndex,
            PduHeaders headers, Int32 mapColumn) {
        String s = c->GetString(columnIndex);
        If ((s != NULL) && (s->Length() > 0)) {
            Int32 charsetColumnIndex = CHARSET_COLUMN_INDEX_MAP->Get(mapColumn);
            Int32 charset = c->GetInt(charsetColumnIndex);
            EncodedStringValue value = new EncodedStringValue(
                    charset, GetBytes(s));
            headers->SetEncodedStringValue(value, mapColumn);
        }
    }

    private void SetTextStringToHeaders(
            Cursor c, Int32 columnIndex,
            PduHeaders headers, Int32 mapColumn) {
        String s = c->GetString(columnIndex);
        If (s != NULL) {
            headers->SetTextString(GetBytes(s), mapColumn);
        }
    }

    private void SetOctetToHeaders(
            Cursor c, Int32 columnIndex,
            PduHeaders headers, Int32 mapColumn) throws InvalidHeaderValueException {
        If (!c->IsNull(columnIndex)) {
            Int32 b = c->GetInt(columnIndex);
            headers->SetOctet(b, mapColumn);
        }
    }

    private void SetLongToHeaders(
            Cursor c, Int32 columnIndex,
            PduHeaders headers, Int32 mapColumn) {
        If (!c->IsNull(columnIndex)) {
            Int64 l = c->GetLong(columnIndex);
            headers->SetLongInteger(l, mapColumn);
        }
    }

    private Integer GetIntegerFromPartColumn(Cursor c, Int32 columnIndex) {
        If (!c->IsNull(columnIndex)) {
            return c->GetInt(columnIndex);
        }
        return NULL;
    }

    private Byte[] GetByteArrayFromPartColumn(Cursor c, Int32 columnIndex) {
        If (!c->IsNull(columnIndex)) {
            return GetBytes(c->GetString(columnIndex));
        }
        return NULL;
    }

    private PduPart[] LoadParts(Int64 msgId) throws MmsException {
        Cursor c = SqliteWrapper->Query(mContext, mContentResolver,
                Uri->Parse("content://mms/" + msgId + "/part"),
                PART_PROJECTION, NULL, NULL, NULL);

        PduPart[] parts = NULL;

        try {
            If ((c == NULL) || (c->GetCount() == 0)) {
                If (LOCAL_LOGV) {
                    Logger::V(TAG, "LoadParts(" + msgId + "): no part to load.");
                }
                return NULL;
            }

            Int32 partCount = c->GetCount();
            Int32 partIdx = 0;
            parts = new PduPart[partCount];
            While (c->MoveToNext()) {
                PduPart part = new PduPart();
                Integer charset = GetIntegerFromPartColumn(
                        c, PART_COLUMN_CHARSET);
                If (charset != NULL) {
                    part->SetCharset(charset);
                }

                Byte[] contentDisposition = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_CONTENT_DISPOSITION);
                If (contentDisposition != NULL) {
                    part->SetContentDisposition(contentDisposition);
                }

                Byte[] contentId = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_CONTENT_ID);
                If (contentId != NULL) {
                    part->SetContentId(contentId);
                }

                Byte[] contentLocation = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_CONTENT_LOCATION);
                If (contentLocation != NULL) {
                    part->SetContentLocation(contentLocation);
                }

                Byte[] contentType = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_CONTENT_TYPE);
                If (contentType != NULL) {
                    part->SetContentType(contentType);
                } else {
                    throw new MmsException("Content-Type must be set.");
                }

                Byte[] fileName = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_FILENAME);
                If (fileName != NULL) {
                    part->SetFilename(fileName);
                }

                Byte[] name = GetByteArrayFromPartColumn(
                        c, PART_COLUMN_NAME);
                If (name != NULL) {
                    part->SetName(name);
                }

                // Construct a Uri for this part.
                Int64 partId = c->GetLong(PART_COLUMN_ID);
                Uri partURI = Uri->Parse("content://mms/part/" + partId);
                part->SetDataUri(partURI);

                // For images/audio/video, we won't keep their data in Part
                // because their renderer accept Uri as source.
                String type = ToIsoString(contentType);
                If (!ContentType->IsImageType(type)
                        && !ContentType->IsAudioType(type)
                        && !ContentType->IsVideoType(type)) {
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    InputStream is = NULL;

                    // Store simple string values directly in the database instead of an
                    // external file.  This makes the text searchable and retrieval slightly
                    // faster.
                    If (ContentType.TEXT_PLAIN->Equals(type) || ContentType.APP_SMIL->Equals(type)
                            || ContentType.TEXT_HTML->Equals(type)) {
                        String text = c->GetString(PART_COLUMN_TEXT);
                        Byte [] blob = new EncodedStringValue(text != NULL ? text : "")
                            .GetTextString();
                        baos->Write(blob, 0, blob.length);
                    } else {

                        try {
                            is = mContentResolver->OpenInputStream(partURI);

                            Byte[] buffer = new Byte[256];
                            Int32 len = is->Read(buffer);
                            While (len >= 0) {
                                baos->Write(buffer, 0, len);
                                len = is->Read(buffer);
                            }
                        } Catch (IOException e) {
                            Logger::E(TAG, "Failed to load part data", e);
                            c->Close();
                            throw new MmsException(e);
                        } finally {
                            If (is != NULL) {
                                try {
                                    is->Close();
                                } Catch (IOException e) {
                                    Logger::E(TAG, "Failed to close stream", e);
                                } // Ignore
                            }
                        }
                    }
                    part->SetData(baos->ToByteArray());
                }
                parts[partIdx++] = part;
            }
        } finally {
            If (c != NULL) {
                c->Close();
            }
        }

        return parts;
    }

    private void LoadAddress(Int64 msgId, PduHeaders headers) {
        Cursor c = SqliteWrapper->Query(mContext, mContentResolver,
                Uri->Parse("content://mms/" + msgId + "/addr"),
                new String[] { Addr.ADDRESS, Addr.CHARSET, Addr.TYPE },
                NULL, NULL, NULL);

        If (c != NULL) {
            try {
                While (c->MoveToNext()) {
                    String addr = c->GetString(0);
                    If (!TextUtils->IsEmpty(addr)) {
                        Int32 addrType = c->GetInt(2);
                        Switch (addrType) {
                            case PduHeaders.FROM:
                                headers->SetEncodedStringValue(
                                        new EncodedStringValue(c->GetInt(1), GetBytes(addr)),
                                        addrType);
                                break;
                            case PduHeaders.TO:
                            case PduHeaders.CC:
                            case PduHeaders.BCC:
                                headers->AppendEncodedStringValue(
                                        new EncodedStringValue(c->GetInt(1), GetBytes(addr)),
                                        addrType);
                                break;
                            default:
                                Logger::E(TAG, "Unknown address type: " + addrType);
                                break;
                        }
                    }
                }
            } finally {
                c->Close();
            }
        }
    }

    /**
     * Load a PDU from storage by given Uri.
     *
     * @param uri The Uri of the PDU to be loaded.
     * @return A generic PDU object, it may be cast to dedicated PDU.
     * @throws MmsException Failed to load some fields of a PDU.
     */
    public GenericPdu Load(Uri uri) throws MmsException {
        GenericPdu pdu = NULL;
        PduCacheEntry cacheEntry = NULL;
        Int32 msgBox = 0;
        Int64 threadId = -1;
        try {
            {    AutoLock syncLock(PDU_CACHE_INSTANCE);
                If (PDU_CACHE_INSTANCE->IsUpdating(uri)) {
                    If (LOCAL_LOGV) {
                        Logger::V(TAG, "load: " + uri + " blocked by IsUpdating()");
                    }
                    try {
                        PDU_CACHE_INSTANCE->Wait();
                    } Catch (InterruptedException e) {
                        Logger::E(TAG, "load: ", e);
                    }
                    cacheEntry = PDU_CACHE_INSTANCE->Get(uri);
                    If (cacheEntry != NULL) {
                        return cacheEntry->GetPdu();
                    }
                }
                // Tell the cache to indicate to other callers that this item
                // is currently being updated.
                PDU_CACHE_INSTANCE->SetUpdating(uri, TRUE);
            }

            Cursor c = SqliteWrapper->Query(mContext, mContentResolver, uri,
                    PDU_PROJECTION, NULL, NULL, NULL);
            PduHeaders headers = new PduHeaders();
            Set<Entry<Integer, Integer>> set;
            Int64 msgId = ContentUris->ParseId(uri);

            try {
                If ((c == NULL) || (c->GetCount() != 1) || !c->MoveToFirst()) {
                    throw new MmsException("Bad uri: " + uri);
                }

                msgBox = c->GetInt(PDU_COLUMN_MESSAGE_BOX);
                threadId = c->GetLong(PDU_COLUMN_THREAD_ID);

                set = ENCODED_STRING_COLUMN_INDEX_MAP->EntrySet();
                For (Entry<Integer, Integer> e : set) {
                    SetEncodedStringValueToHeaders(
                            c, e->GetValue(), headers, e->GetKey());
                }

                set = TEXT_STRING_COLUMN_INDEX_MAP->EntrySet();
                For (Entry<Integer, Integer> e : set) {
                    SetTextStringToHeaders(
                            c, e->GetValue(), headers, e->GetKey());
                }

                set = OCTET_COLUMN_INDEX_MAP->EntrySet();
                For (Entry<Integer, Integer> e : set) {
                    SetOctetToHeaders(
                            c, e->GetValue(), headers, e->GetKey());
                }

                set = LONG_COLUMN_INDEX_MAP->EntrySet();
                For (Entry<Integer, Integer> e : set) {
                    SetLongToHeaders(
                            c, e->GetValue(), headers, e->GetKey());
                }
            } finally {
                If (c != NULL) {
                    c->Close();
                }
            }

            // Check whether 'msgId' has been assigned a valid value.
            If (msgId == -1L) {
                throw new MmsException("Error! ID of the message: -1.");
            }

            // Load address information of the MM.
            LoadAddress(msgId, headers);

            Int32 msgType = headers->GetOctet(PduHeaders.MESSAGE_TYPE);
            PduBody body = new PduBody();

            // For PDU which type is M_retrieve.conf or Send.req, we should
            // load multiparts and put them into the body of the PDU.
            If ((msgType == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF)
                    || (msgType == PduHeaders.MESSAGE_TYPE_SEND_REQ)) {
                PduPart[] parts = LoadParts(msgId);
                If (parts != NULL) {
                    Int32 partsNum = parts.length;
                    For (Int32 i = 0; i < partsNum; i++) {
                        body->AddPart(parts[i]);
                    }
                }
            }

            Switch (msgType) {
            case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
                pdu = new NotificationInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_DELIVERY_IND:
                pdu = new DeliveryInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_READ_ORIG_IND:
                pdu = new ReadOrigInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
                pdu = new RetrieveConf(headers, body);
                break;
            case PduHeaders.MESSAGE_TYPE_SEND_REQ:
                pdu = new SendReq(headers, body);
                break;
            case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
                pdu = new AcknowledgeInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
                pdu = new NotifyRespInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
                pdu = new ReadRecInd(headers);
                break;
            case PduHeaders.MESSAGE_TYPE_SEND_CONF:
            case PduHeaders.MESSAGE_TYPE_FORWARD_REQ:
            case PduHeaders.MESSAGE_TYPE_FORWARD_CONF:
            case PduHeaders.MESSAGE_TYPE_MBOX_STORE_REQ:
            case PduHeaders.MESSAGE_TYPE_MBOX_STORE_CONF:
            case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_REQ:
            case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_CONF:
            case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_REQ:
            case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_CONF:
            case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_REQ:
            case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_CONF:
            case PduHeaders.MESSAGE_TYPE_MBOX_DESCR:
            case PduHeaders.MESSAGE_TYPE_DELETE_REQ:
            case PduHeaders.MESSAGE_TYPE_DELETE_CONF:
            case PduHeaders.MESSAGE_TYPE_CANCEL_REQ:
            case PduHeaders.MESSAGE_TYPE_CANCEL_CONF:
                throw new MmsException(
                        "Unsupported PDU type: " + Integer->ToHexString(msgType));

            default:
                throw new MmsException(
                        "Unrecognized PDU type: " + Integer->ToHexString(msgType));
            }
        } finally {
            {    AutoLock syncLock(PDU_CACHE_INSTANCE);
                If (pdu != NULL) {
                    Assert(PDU_CACHE_INSTANCE->Get(uri) == NULL);
                    // Update the cache entry with the real info
                    cacheEntry = new PduCacheEntry(pdu, msgBox, threadId);
                    PDU_CACHE_INSTANCE->Put(uri, cacheEntry);
                }
                PDU_CACHE_INSTANCE->SetUpdating(uri, FALSE);
                PDU_CACHE_INSTANCE->NotifyAll(); // tell anybody waiting on this entry to go ahead
            }
        }
        return pdu;
    }

    private void PersistAddress(
            Int64 msgId, Int32 type, EncodedStringValue[] array) {
        ContentValues values = new ContentValues(3);

        For (EncodedStringValue addr : array) {
            values->Clear(); // Clear all values first.
            values->Put(Addr.ADDRESS, ToIsoString(addr->GetTextString()));
            values->Put(Addr.CHARSET, addr->GetCharacterSet());
            values->Put(Addr.TYPE, type);

            Uri uri = Uri->Parse("content://mms/" + msgId + "/addr");
            SqliteWrapper->Insert(mContext, mContentResolver, uri, values);
        }
    }

    private static String GetPartContentType(PduPart part) {
        return part->GetContentType() == NULL ? NULL : ToIsoString(part->GetContentType());
    }

    public Uri PersistPart(PduPart part, Int64 msgId, HashMap<Uri, InputStream> preOpenedFiles)
            throws MmsException {
        Uri uri = Uri->Parse("content://mms/" + msgId + "/part");
        ContentValues values = new ContentValues(8);

        Int32 charset = part->GetCharset();
        If (charset != 0 ) {
            values->Put(Part.CHARSET, charset);
        }

        String contentType = GetPartContentType(part);
        If (contentType != NULL) {
            // There is no "image/jpg" in Android (and it's an invalid mimetype).
            // Change it to "image/jpeg"
            If (ContentType.IMAGE_JPG->Equals(contentType)) {
                contentType = ContentType.IMAGE_JPEG;
            }

            values->Put(Part.CONTENT_TYPE, contentType);
            // To ensure the SMIL part is always the first part.
            If (ContentType.APP_SMIL->Equals(contentType)) {
                values->Put(Part.SEQ, -1);
            }
        } else {
            throw new MmsException("MIME type of the part must be set.");
        }

        If (part->GetFilename() != NULL) {
            String fileName = new String(part->GetFilename());
            values->Put(Part.FILENAME, fileName);
        }

        If (part->GetName() != NULL) {
            String name = new String(part->GetName());
            values->Put(Part.NAME, name);
        }

        Object value = NULL;
        If (part->GetContentDisposition() != NULL) {
            value = ToIsoString(part->GetContentDisposition());
            values->Put(Part.CONTENT_DISPOSITION, (String) value);
        }

        If (part->GetContentId() != NULL) {
            value = ToIsoString(part->GetContentId());
            values->Put(Part.CONTENT_ID, (String) value);
        }

        If (part->GetContentLocation() != NULL) {
            value = ToIsoString(part->GetContentLocation());
            values->Put(Part.CONTENT_LOCATION, (String) value);
        }

        Uri res = SqliteWrapper->Insert(mContext, mContentResolver, uri, values);
        If (res == NULL) {
            throw new MmsException("Failed to persist part, return NULL.");
        }

        PersistData(part, res, contentType, preOpenedFiles);
        // After successfully store the data, we should update
        // the dataUri of the part.
        part->SetDataUri(res);

        return res;
    }

    /**
     * Save data of the part into storage. The source data may be given
     * by a Byte[] or a Uri. If it's a Byte[], directly save it
     * into storage, otherwise load source data from the dataUri and then
     * save it. If the data is an image, we may scale down it according
     * to user preference.
     *
     * @param part The PDU part which contains data to be saved.
     * @param uri The URI of the part.
     * @param contentType The MIME type of the part.
     * @param preOpenedFiles if not NULL, a map of preopened InputStreams for the parts.
     * @throws MmsException Cannot find source data or error occurred
     *         while saving the data.
     */
    private void PersistData(PduPart part, Uri uri,
            String contentType, HashMap<Uri, InputStream> preOpenedFiles)
            throws MmsException {
        OutputStream os = NULL;
        InputStream is = NULL;
        DrmConvertSession drmConvertSession = NULL;
        Uri dataUri = NULL;
        String path = NULL;

        try {
            Byte[] data = part->GetData();
            If (ContentType.TEXT_PLAIN->Equals(contentType)
                    || ContentType.APP_SMIL->Equals(contentType)
                    || ContentType.TEXT_HTML->Equals(contentType)) {
                ContentValues cv = new ContentValues();
                If (data == NULL) {
                    data = new String("").GetBytes(CharacterSets.DEFAULT_CHARSET_NAME);
                }
                cv->Put(Telephony.Mms.Part.TEXT, new EncodedStringValue(data).GetString());
                If (mContentResolver->Update(uri, cv, NULL, NULL) != 1) {
                    throw new MmsException("unable to update " + uri->ToString());
                }
            } else {
                Boolean isDrm = DownloadDrmHelper->IsDrmConvertNeeded(contentType);
                If (isDrm) {
                    If (uri != NULL) {
                        try {
                            path = ConvertUriToPath(mContext, uri);
                            If (LOCAL_LOGV) {
                                Logger::V(TAG, "drm uri: " + uri + " path: " + path);
                            }
                            File f = new File(path);
                            Int64 len = f->Length();
                            If (LOCAL_LOGV) {
                                Logger::V(TAG, "drm path: " + path + " len: " + len);
                            }
                            If (len > 0) {
                                // we're not going to re-persist and re-encrypt an already
                                // converted drm file
                                return;
                            }
                        } Catch (Exception e) {
                            Logger::E(TAG, "Can't get file info for: " + part->GetDataUri(), e);
                        }
                    }
                    // We haven't converted the file yet, start the conversion
                    drmConvertSession = DrmConvertSession->Open(mContext, contentType);
                    If (drmConvertSession == NULL) {
                        throw new MmsException("Mimetype " + contentType +
                                " can not be converted.");
                    }
                }
                // uri can look like:
                // content://mms/part/98
                os = mContentResolver->OpenOutputStream(uri);
                If (data == NULL) {
                    dataUri = part->GetDataUri();
                    If ((dataUri == NULL) || (dataUri == uri)) {
                        Logger::W(TAG, "Can't find data for this part.");
                        return;
                    }
                    // dataUri can look like:
                    // content://com.google.android.gallery3d.provider/picasa/item/5720646660183715586
                    If (preOpenedFiles != NULL && preOpenedFiles->ContainsKey(dataUri)) {
                        is = preOpenedFiles->Get(dataUri);
                    }
                    If (is == NULL) {
                        is = mContentResolver->OpenInputStream(dataUri);
                    }

                    If (LOCAL_LOGV) {
                        Logger::V(TAG, "Saving data to: " + uri);
                    }

                    Byte[] buffer = new Byte[8192];
                    For (Int32 len = 0; (len = is->Read(buffer)) != -1; ) {
                        If (!isDrm) {
                            os->Write(buffer, 0, len);
                        } else {
                            Byte[] convertedData = drmConvertSession->Convert(buffer, len);
                            If (convertedData != NULL) {
                                os->Write(convertedData, 0, convertedData.length);
                            } else {
                                throw new MmsException("Error converting drm data.");
                            }
                        }
                    }
                } else {
                    If (LOCAL_LOGV) {
                        Logger::V(TAG, "Saving data to: " + uri);
                    }
                    If (!isDrm) {
                        os->Write(data);
                    } else {
                        dataUri = uri;
                        Byte[] convertedData = drmConvertSession->Convert(data, data.length);
                        If (convertedData != NULL) {
                            os->Write(convertedData, 0, convertedData.length);
                        } else {
                            throw new MmsException("Error converting drm data.");
                        }
                    }
                }
            }
        } Catch (FileNotFoundException e) {
            Logger::E(TAG, "Failed to open Input/Output stream.", e);
            throw new MmsException(e);
        } Catch (IOException e) {
            Logger::E(TAG, "Failed to read/write data.", e);
            throw new MmsException(e);
        } finally {
            If (os != NULL) {
                try {
                    os->Close();
                } Catch (IOException e) {
                    Logger::E(TAG, "IOException while closing: " + os, e);
                } // Ignore
            }
            If (is != NULL) {
                try {
                    is->Close();
                } Catch (IOException e) {
                    Logger::E(TAG, "IOException while closing: " + is, e);
                } // Ignore
            }
            If (drmConvertSession != NULL) {
                drmConvertSession->Close(path);

                // Reset the permissions on the encrypted part file so everyone has only read
                // permission.
                File f = new File(path);
                ContentValues values = new ContentValues(0);
                SqliteWrapper->Update(mContext, mContentResolver,
                                     Uri->Parse("content://mms/resetFilePerm/" + f->GetName()),
                                     values, NULL, NULL);
            }
        }
    }

    /**
     * This method expects uri in the following format
     *     content://media/<table_name>/<row_index> (or)
     *     file://sdcard/test.mp4
     *     http://test.com/test.mp4
     *
     * Here <table_name> shall be "video" or "audio" or "images"
     * <row_index> the index of the content in given table
     */
    static public String ConvertUriToPath(Context context, Uri uri) {
        String path = NULL;
        If (NULL != uri) {
            String scheme = uri->GetScheme();
            If (NULL == scheme || scheme->Equals("") ||
                    scheme->Equals(ContentResolver.SCHEME_FILE)) {
                path = uri->GetPath();

            } else If (scheme->Equals("http")) {
                path = uri->ToString();

            } else If (scheme->Equals(ContentResolver.SCHEME_CONTENT)) {
                String[] projection = new String[] {MediaStore.MediaColumns.DATA};
                Cursor cursor = NULL;
                try {
                    cursor = context->GetContentResolver()->Query(uri, projection, NULL,
                            NULL, NULL);
                    If (NULL == cursor || 0 == cursor->GetCount() || !cursor->MoveToFirst()) {
                        throw new IllegalArgumentException("Given Uri could not be found" +
                                " in media store");
                    }
                    Int32 pathIndex = cursor->GetColumnIndexOrThrow(MediaStore.MediaColumns.DATA);
                    path = cursor->GetString(pathIndex);
                } Catch (SQLiteException e) {
                    throw new IllegalArgumentException("Given Uri is not formatted in a way " +
                            "so that it can be found in media store.");
                } finally {
                    If (NULL != cursor) {
                        cursor->Close();
                    }
                }
            } else {
                throw new IllegalArgumentException("Given Uri scheme is not supported");
            }
        }
        return path;
    }

    private void UpdateAddress(
            Int64 msgId, Int32 type, EncodedStringValue[] array) {
        // Delete old address information and then insert new ones.
        SqliteWrapper->Delete(mContext, mContentResolver,
                Uri->Parse("content://mms/" + msgId + "/addr"),
                Addr.TYPE + "=" + type, NULL);

        PersistAddress(msgId, type, array);
    }

    /**
     * Update headers of a SendReq.
     *
     * @param uri The PDU which need to be updated.
     * @param pdu New headers.
     * @throws MmsException Bad URI or updating failed.
     */
    CARAPI UpdateHeaders(Uri uri, SendReq sendReq) {
        {    AutoLock syncLock(PDU_CACHE_INSTANCE);
            // If the cache item is getting updated, wait until it's done updating before
            // purging it.
            If (PDU_CACHE_INSTANCE->IsUpdating(uri)) {
                If (LOCAL_LOGV) {
                    Logger::V(TAG, "updateHeaders: " + uri + " blocked by IsUpdating()");
                }
                try {
                    PDU_CACHE_INSTANCE->Wait();
                } Catch (InterruptedException e) {
                    Logger::E(TAG, "updateHeaders: ", e);
                }
            }
        }
        PDU_CACHE_INSTANCE->Purge(uri);

        ContentValues values = new ContentValues(10);
        Byte[] contentType = sendReq->GetContentType();
        If (contentType != NULL) {
            values->Put(Mms.CONTENT_TYPE, ToIsoString(contentType));
        }

        Int64 date = sendReq->GetDate();
        If (date != -1) {
            values->Put(Mms.DATE, date);
        }

        Int32 deliveryReport = sendReq->GetDeliveryReport();
        If (deliveryReport != 0) {
            values->Put(Mms.DELIVERY_REPORT, deliveryReport);
        }

        Int64 expiry = sendReq->GetExpiry();
        If (expiry != -1) {
            values->Put(Mms.EXPIRY, expiry);
        }

        Byte[] msgClass = sendReq->GetMessageClass();
        If (msgClass != NULL) {
            values->Put(Mms.MESSAGE_CLASS, ToIsoString(msgClass));
        }

        Int32 priority = sendReq->GetPriority();
        If (priority != 0) {
            values->Put(Mms.PRIORITY, priority);
        }

        Int32 readReport = sendReq->GetReadReport();
        If (readReport != 0) {
            values->Put(Mms.READ_REPORT, readReport);
        }

        Byte[] transId = sendReq->GetTransactionId();
        If (transId != NULL) {
            values->Put(Mms.TRANSACTION_ID, ToIsoString(transId));
        }

        EncodedStringValue subject = sendReq->GetSubject();
        If (subject != NULL) {
            values->Put(Mms.SUBJECT, ToIsoString(subject->GetTextString()));
            values->Put(Mms.SUBJECT_CHARSET, subject->GetCharacterSet());
        } else {
            values->Put(Mms.SUBJECT, "");
        }

        Int64 messageSize = sendReq->GetMessageSize();
        If (messageSize > 0) {
            values->Put(Mms.MESSAGE_SIZE, messageSize);
        }

        PduHeaders headers = sendReq->GetPduHeaders();
        HashSet<String> recipients = new HashSet<String>();
        For (Int32 addrType : ADDRESS_FIELDS) {
            EncodedStringValue[] array = NULL;
            If (addrType == PduHeaders.FROM) {
                EncodedStringValue v = headers->GetEncodedStringValue(addrType);
                If (v != NULL) {
                    array = new EncodedStringValue[1];
                    array[0] = v;
                }
            } else {
                array = headers->GetEncodedStringValues(addrType);
            }

            If (array != NULL) {
                Int64 msgId = ContentUris->ParseId(uri);
                UpdateAddress(msgId, addrType, array);
                If (addrType == PduHeaders.TO) {
                    For (EncodedStringValue v : array) {
                        If (v != NULL) {
                            recipients->Add(v->GetString());
                        }
                    }
                }
            }
        }
        If (!recipients->IsEmpty()) {
            Int64 threadId = Threads->GetOrCreateThreadId(mContext, recipients);
            values->Put(Mms.THREAD_ID, threadId);
        }

        SqliteWrapper->Update(mContext, mContentResolver, uri, values, NULL, NULL);
    }

    private void UpdatePart(Uri uri, PduPart part, HashMap<Uri, InputStream> preOpenedFiles)
            throws MmsException {
        ContentValues values = new ContentValues(7);

        Int32 charset = part->GetCharset();
        If (charset != 0 ) {
            values->Put(Part.CHARSET, charset);
        }

        String contentType = NULL;
        If (part->GetContentType() != NULL) {
            contentType = ToIsoString(part->GetContentType());
            values->Put(Part.CONTENT_TYPE, contentType);
        } else {
            throw new MmsException("MIME type of the part must be set.");
        }

        If (part->GetFilename() != NULL) {
            String fileName = new String(part->GetFilename());
            values->Put(Part.FILENAME, fileName);
        }

        If (part->GetName() != NULL) {
            String name = new String(part->GetName());
            values->Put(Part.NAME, name);
        }

        Object value = NULL;
        If (part->GetContentDisposition() != NULL) {
            value = ToIsoString(part->GetContentDisposition());
            values->Put(Part.CONTENT_DISPOSITION, (String) value);
        }

        If (part->GetContentId() != NULL) {
            value = ToIsoString(part->GetContentId());
            values->Put(Part.CONTENT_ID, (String) value);
        }

        If (part->GetContentLocation() != NULL) {
            value = ToIsoString(part->GetContentLocation());
            values->Put(Part.CONTENT_LOCATION, (String) value);
        }

        SqliteWrapper->Update(mContext, mContentResolver, uri, values, NULL, NULL);

        // Only update the data when:
        // 1. New binary data supplied or
        // 2. The Uri of the part is different from the current one.
        If ((part->GetData() != NULL)
                || (uri != part->GetDataUri())) {
            PersistData(part, uri, contentType, preOpenedFiles);
        }
    }

    /**
     * Update all parts of a PDU.
     *
     * @param uri The PDU which need to be updated.
     * @param body New message body of the PDU.
     * @param preOpenedFiles if not NULL, a map of preopened InputStreams for the parts.
     * @throws MmsException Bad URI or updating failed.
     */
    CARAPI UpdateParts(Uri uri, PduBody body, HashMap<Uri, InputStream> preOpenedFiles)
            throws MmsException {
        try {
            PduCacheEntry cacheEntry;
            {    AutoLock syncLock(PDU_CACHE_INSTANCE);
                If (PDU_CACHE_INSTANCE->IsUpdating(uri)) {
                    If (LOCAL_LOGV) {
                        Logger::V(TAG, "updateParts: " + uri + " blocked by IsUpdating()");
                    }
                    try {
                        PDU_CACHE_INSTANCE->Wait();
                    } Catch (InterruptedException e) {
                        Logger::E(TAG, "updateParts: ", e);
                    }
                    cacheEntry = PDU_CACHE_INSTANCE->Get(uri);
                    If (cacheEntry != NULL) {
                        ((MultimediaMessagePdu) cacheEntry->GetPdu()).SetBody(body);
                    }
                }
                // Tell the cache to indicate to other callers that this item
                // is currently being updated.
                PDU_CACHE_INSTANCE->SetUpdating(uri, TRUE);
            }

            ArrayList<PduPart> toBeCreated = new ArrayList<PduPart>();
            HashMap<Uri, PduPart> toBeUpdated = new HashMap<Uri, PduPart>();

            Int32 partsNum = body->GetPartsNum();
            StringBuilder filter = new StringBuilder()->Append('(');
            For (Int32 i = 0; i < partsNum; i++) {
                PduPart part = body->GetPart(i);
                Uri partUri = part->GetDataUri();
                If ((partUri == NULL) || TextUtils->IsEmpty(partUri->GetAuthority())
                        || !partUri->GetAuthority()->StartsWith("mms")) {
                    toBeCreated->Add(part);
                } else {
                    toBeUpdated->Put(partUri, part);

                    // Don't use 'i > 0' to determine whether we should append
                    // 'AND' since 'i = 0' may be skipped in another branch.
                    If (filter->Length() > 1) {
                        filter->Append(" AND ");
                    }

                    filter->Append(Part._ID);
                    filter->Append("!=");
                    DatabaseUtils->AppendEscapedSQLString(filter, partUri->GetLastPathSegment());
                }
            }
            filter->Append(')');

            Int64 msgId = ContentUris->ParseId(uri);

            // Remove the parts which doesn't exist anymore.
            SqliteWrapper->Delete(mContext, mContentResolver,
                    Uri->Parse(Mms.CONTENT_URI + "/" + msgId + "/part"),
                    filter->Length() > 2 ? filter->ToString() : NULL, NULL);

            // Create new parts which didn't exist before.
            For (PduPart part : toBeCreated) {
                PersistPart(part, msgId, preOpenedFiles);
            }

            // Update the modified parts.
            For (Map.Entry<Uri, PduPart> e : toBeUpdated->EntrySet()) {
                UpdatePart(e->GetKey(), e->GetValue(), preOpenedFiles);
            }
        } finally {
            {    AutoLock syncLock(PDU_CACHE_INSTANCE);
                PDU_CACHE_INSTANCE->SetUpdating(uri, FALSE);
                PDU_CACHE_INSTANCE->NotifyAll();
            }
        }
    }

    /**
     * Persist a PDU object to specific location in the storage.
     *
     * @param pdu The PDU object to be stored.
     * @param uri Where to store the given PDU object.
     * @param createThreadId if TRUE, this function may create a thread id for the recipients
     * @param groupMmsEnabled if TRUE, all of the recipients addressed in the PDU will be used
     *  to create the associated thread. When FALSE, only the sender will be used in finding or
     *  creating the appropriate thread or conversation.
     * @param preOpenedFiles if not NULL, a map of preopened InputStreams for the parts.
     * @return A Uri which can be used to access the stored PDU.
     */

    public Uri Persist(GenericPdu pdu, Uri uri, Boolean createThreadId, Boolean groupMmsEnabled,
            HashMap<Uri, InputStream> preOpenedFiles)
            throws MmsException {
        If (uri == NULL) {
            throw new MmsException("Uri may not be NULL.");
        }
        Int64 msgId = -1;
        try {
            msgId = ContentUris->ParseId(uri);
        } Catch (NumberFormatException e) {
            // the uri ends with "inbox" or something else like that
        }
        Boolean existingUri = msgId != -1;

        If (!existingUri && MESSAGE_BOX_MAP->Get(uri) == NULL) {
            throw new MmsException(
                    "Bad destination, must be one of "
                    + "content://mms/inbox, content://mms/sent, "
                    + "content://mms/drafts, content://mms/outbox, "
                    + "content://mms/temp.");
        }
        {    AutoLock syncLock(PDU_CACHE_INSTANCE);
            // If the cache item is getting updated, wait until it's done updating before
            // purging it.
            If (PDU_CACHE_INSTANCE->IsUpdating(uri)) {
                If (LOCAL_LOGV) {
                    Logger::V(TAG, "persist: " + uri + " blocked by IsUpdating()");
                }
                try {
                    PDU_CACHE_INSTANCE->Wait();
                } Catch (InterruptedException e) {
                    Logger::E(TAG, "persist1: ", e);
                }
            }
        }
        PDU_CACHE_INSTANCE->Purge(uri);

        PduHeaders header = pdu->GetPduHeaders();
        PduBody body = NULL;
        ContentValues values = new ContentValues();
        Set<Entry<Integer, String>> set;

        set = ENCODED_STRING_COLUMN_NAME_MAP->EntrySet();
        For (Entry<Integer, String> e : set) {
            Int32 field = e->GetKey();
            EncodedStringValue encodedString = header->GetEncodedStringValue(field);
            If (encodedString != NULL) {
                String charsetColumn = CHARSET_COLUMN_NAME_MAP->Get(field);
                values->Put(e->GetValue(), ToIsoString(encodedString->GetTextString()));
                values->Put(charsetColumn, encodedString->GetCharacterSet());
            }
        }

        set = TEXT_STRING_COLUMN_NAME_MAP->EntrySet();
        For (Entry<Integer, String> e : set){
            Byte[] text = header->GetTextString(e->GetKey());
            If (text != NULL) {
                values->Put(e->GetValue(), ToIsoString(text));
            }
        }

        set = OCTET_COLUMN_NAME_MAP->EntrySet();
        For (Entry<Integer, String> e : set){
            Int32 b = header->GetOctet(e->GetKey());
            If (b != 0) {
                values->Put(e->GetValue(), b);
            }
        }

        set = LONG_COLUMN_NAME_MAP->EntrySet();
        For (Entry<Integer, String> e : set){
            Int64 l = header->GetLongInteger(e->GetKey());
            If (l != -1L) {
                values->Put(e->GetValue(), l);
            }
        }

        HashMap<Integer, EncodedStringValue[]> addressMap =
                new HashMap<Integer, EncodedStringValue[]>(ADDRESS_FIELDS.length);
        // Save address information.
        For (Int32 addrType : ADDRESS_FIELDS) {
            EncodedStringValue[] array = NULL;
            If (addrType == PduHeaders.FROM) {
                EncodedStringValue v = header->GetEncodedStringValue(addrType);
                If (v != NULL) {
                    array = new EncodedStringValue[1];
                    array[0] = v;
                }
            } else {
                array = header->GetEncodedStringValues(addrType);
            }
            addressMap->Put(addrType, array);
        }

        HashSet<String> recipients = new HashSet<String>();
        Int32 msgType = pdu->GetMessageType();
        // Here we only allocate thread ID for M-Notification.ind,
        // M-Retrieve.conf and M-Send.req.
        // Some of other PDU types may be allocated a thread ID outside
        // this scope.
        If ((msgType == PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND)
                || (msgType == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF)
                || (msgType == PduHeaders.MESSAGE_TYPE_SEND_REQ)) {
            Switch (msgType) {
                case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
                case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
                    LoadRecipients(PduHeaders.FROM, recipients, addressMap, FALSE);

                    // For received messages when group MMS is enabled, we want to associate this
                    // message with the thread composed of all the recipients -- all but our own
                    // number, that is. This includes the person who sent the
                    // message or the FROM Field (above) in addition to the other people the message
                    // was addressed to or the TO field. Our own number is in that TO field and
                    // we have to ignore it in loadRecipients.
                    If (groupMmsEnabled) {
                        LoadRecipients(PduHeaders.TO, recipients, addressMap, TRUE);

                        // Also load any numbers in the CC field to address group messaging
                        // compatibility issues with devices that place numbers in this field
                        // for group messages.
                        LoadRecipients(PduHeaders.CC, recipients, addressMap, TRUE);
                    }
                    break;
                case PduHeaders.MESSAGE_TYPE_SEND_REQ:
                    LoadRecipients(PduHeaders.TO, recipients, addressMap, FALSE);
                    break;
            }
            Int64 threadId = 0;
            If (createThreadId && !recipients->IsEmpty()) {
                // Given all the recipients associated with this message, Find (or create) the
                // correct thread.
                threadId = Threads->GetOrCreateThreadId(mContext, recipients);
            }
            values->Put(Mms.THREAD_ID, threadId);
        }

        // Save parts first to avoid inconsistent message is loaded
        // while saving the parts.
        Int64 dummyId = System->CurrentTimeMillis(); // Dummy ID of the msg.

        // Figure out if this PDU is a text-only message
        Boolean textOnly = TRUE;

        // Sum up the total message size
        Int32 messageSize = 0;

        // Get body if the PDU is a RetrieveConf or SendReq.
        If (pdu instanceof MultimediaMessagePdu) {
            body = ((MultimediaMessagePdu) pdu).GetBody();
            // Start saving parts if necessary.
            If (body != NULL) {
                Int32 partsNum = body->GetPartsNum();
                If (partsNum > 2) {
                    // For a text-only message there will be two parts: 1-the SMIL, 2-the text.
                    // Down a few lines below we're checking to make sure we've only got SMIL or
                    // text. We also have to check then we don't have more than two parts.
                    // Otherwise, a slideshow with two text slides would be marked as textOnly.
                    textOnly = FALSE;
                }
                For (Int32 i = 0; i < partsNum; i++) {
                    PduPart part = body->GetPart(i);
                    messageSize += part->GetDataLength();
                    PersistPart(part, dummyId, preOpenedFiles);

                    // If we've got anything besides text/plain or SMIL part, then we've got
                    // an mms message with some other type of attachment.
                    String contentType = GetPartContentType(part);
                    If (contentType != NULL && !ContentType.APP_SMIL->Equals(contentType)
                            && !ContentType.TEXT_PLAIN->Equals(contentType)) {
                        textOnly = FALSE;
                    }
                }
            }
        }
        // Record whether this mms message is a simple plain text or not. This is a hint for the
        // UI.
        values->Put(Mms.TEXT_ONLY, textOnly ? 1 : 0);
        // The message-size might already have been inserted when parsing the
        // PDU header. If not, then we insert the message size as well.
        If (values->GetAsInteger(Mms.MESSAGE_SIZE) == NULL) {
            values->Put(Mms.MESSAGE_SIZE, messageSize);
        }

        Uri res = NULL;
        If (existingUri) {
            res = uri;
            SqliteWrapper->Update(mContext, mContentResolver, res, values, NULL, NULL);
        } else {
            res = SqliteWrapper->Insert(mContext, mContentResolver, uri, values);
            If (res == NULL) {
                throw new MmsException("Persist() failed: return NULL.");
            }
            // Get the real ID of the PDU and update all parts which were
            // saved with the dummy ID.
            msgId = ContentUris->ParseId(res);
        }

        values = new ContentValues(1);
        values->Put(Part.MSG_ID, msgId);
        SqliteWrapper->Update(mContext, mContentResolver,
                             Uri->Parse("content://mms/" + dummyId + "/part"),
                             values, NULL, NULL);
        // We should return the longest URI of the persisted PDU, for
        // example, if input URI is "content://mms/inbox" and the _ID of
        // persisted PDU is '8', we should return "content://mms/inbox/8"
        // instead of "content://mms/8".
        // FIXME: Should the MmsProvider be responsible for this???
        If (!existingUri) {
            res = Uri->Parse(uri + "/" + msgId);
        }

        // Save address information.
        For (Int32 addrType : ADDRESS_FIELDS) {
            EncodedStringValue[] array = addressMap->Get(addrType);
            If (array != NULL) {
                PersistAddress(msgId, addrType, array);
            }
        }

        return res;
    }

    /**
     * For a given address type, extract the recipients from the headers.
     *
     * @param addressType can be PduHeaders.FROM, PduHeaders.TO or PduHeaders.CC
     * @param recipients a HashSet that is loaded with the recipients from the FROM, TO or CC headers
     * @param addressMap a HashMap of the addresses from the ADDRESS_FIELDS header
     * @param excludeMyNumber if TRUE, the number of this phone will be excluded from recipients
     */
    private void LoadRecipients(Int32 addressType, HashSet<String> recipients,
            HashMap<Integer, EncodedStringValue[]> addressMap, Boolean excludeMyNumber) {
        EncodedStringValue[] array = addressMap->Get(addressType);
        If (array == NULL) {
            return;
        }
        // If the TO recipients is only a single address, then we can skip loadRecipients when
        // we're excluding our own number because we know that address is our own.
        If (excludeMyNumber && array.length == 1) {
            return;
        }
        String myNumber = excludeMyNumber ? mTelephonyManager->GetLine1Number() : NULL;
        For (EncodedStringValue v : array) {
            If (v != NULL) {
                String number = v->GetString();
                If ((myNumber == NULL || !PhoneNumberUtils->Compare(number, myNumber)) &&
                        !recipients->Contains(number)) {
                    // Only add numbers which aren't my own number.
                    recipients->Add(number);
                }
            }
        }
    }

    /**
     * Move a PDU object from one location to another.
     *
     * @param from Specify the PDU object to be moved.
     * @param to The destination location, should be one of the following:
     *        "content://mms/inbox", "content://mms/sent",
     *        "content://mms/drafts", "content://mms/outbox",
     *        "content://mms/trash".
     * @return New Uri of the moved PDU.
     * @throws MmsException Error occurred while moving the message.
     */
    public Uri Move(Uri from, Uri to) throws MmsException {
        // Check whether the 'msgId' has been assigned a valid value.
        Int64 msgId = ContentUris->ParseId(from);
        If (msgId == -1L) {
            throw new MmsException("Error! ID of the message: -1.");
        }

        // Get corresponding Int32 value of destination box.
        Integer msgBox = MESSAGE_BOX_MAP->Get(to);
        If (msgBox == NULL) {
            throw new MmsException(
                    "Bad destination, must be one of "
                    + "content://mms/inbox, content://mms/sent, "
                    + "content://mms/drafts, content://mms/outbox, "
                    + "content://mms/temp.");
        }

        ContentValues values = new ContentValues(1);
        values->Put(Mms.MESSAGE_BOX, msgBox);
        SqliteWrapper->Update(mContext, mContentResolver, from, values, NULL, NULL);
        return ContentUris->WithAppendedId(to, msgId);
    }

    /**
     * Wrap a Byte[] into a String.
     */
    public static String ToIsoString(Byte[] bytes) {
        try {
            return new String(bytes, CharacterSets.MIMENAME_ISO_8859_1);
        } Catch (UnsupportedEncodingException e) {
            // Impossible to reach here!
            Logger::E(TAG, "ISO_8859_1 must be supported!", e);
            return "";
        }
    }

    /**
     * Unpack a given String into a Byte[].
     */
    public static Byte[] GetBytes(String data) {
        try {
            return data->GetBytes(CharacterSets.MIMENAME_ISO_8859_1);
        } Catch (UnsupportedEncodingException e) {
            // Impossible to reach here!
            Logger::E(TAG, "ISO_8859_1 must be supported!", e);
            return new Byte[0];
        }
    }

    /**
     * Remove all objects in the temporary path.
     */
    CARAPI Release() {
        Uri uri = Uri->Parse(TEMPORARY_DRM_OBJECT_URI);
        SqliteWrapper->Delete(mContext, mContentResolver, uri, NULL, NULL);
    }

    /**
     * Find all messages to be sent or downloaded before certain time.
     */
    public Cursor GetPendingMessages(Int64 dueTime) {
        Uri.Builder uriBuilder = PendingMessages.CONTENT_URI->BuildUpon();
        uriBuilder->AppendQueryParameter("protocol", "mms");

        String selection = PendingMessages.ERROR_TYPE + " < ?"
                + " AND " + PendingMessages.DUE_TIME + " <= ?";

        String[] selectionArgs = new String[] {
                String->ValueOf(MmsSms.ERR_TYPE_GENERIC_PERMANENT),
                String->ValueOf(dueTime)
        };

        return SqliteWrapper->Query(mContext, mContentResolver,
                uriBuilder->Build(), NULL, selection, selectionArgs,
                PendingMessages.DUE_TIME);
    }
}
