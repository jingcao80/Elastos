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
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/google/mms/pdu/PduPersister.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/database/CDatabaseUtils.h"
#include "elastos/droid/provider/CTelephonySms.h"
#include "elastos/droid/provider/CTelephonyThreads.h"
#include "elastos/droid/provider/CTelephonyMmsSmsPendingMessages.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"
#include "elastos/droid/google/mms/pdu/CNotificationInd.h"
#include "elastos/droid/google/mms/pdu/CDeliveryInd.h"
#include "elastos/droid/google/mms/pdu/CReadOrigInd.h"
#include "elastos/droid/google/mms/pdu/CRetrieveConf.h"
#include "elastos/droid/google/mms/pdu/CSendReq.h"
#include "elastos/droid/google/mms/pdu/CAcknowledgeInd.h"
#include "elastos/droid/google/mms/pdu/CNotifyRespInd.h"
#include "elastos/droid/google/mms/pdu/CReadRecInd.h"
#include "elastos/droid/google/mms/pdu/CPduBody.h"
#include "elastos/droid/google/mms/pdu/CPduPart.h"
#include "elastos/droid/google/mms/pdu/CEncodedStringValue.h"
#include "elastos/droid/google/mms/utility/CSqliteWrapper.h"
#include "elastos/droid/google/mms/utility/CPduCacheEntry.h"
#include "elastos/droid/google/mms/utility/CDrmConvertSessionHelper.h"
#include "elastos/droid/google/mms/utility/CDownloadDrmHelper.h"
#include "elastos/droid/google/mms/CContentTypeHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/telephony/CPhoneNumberUtils.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::ITelephonySms;
using Elastos::Droid::Provider::CTelephonySms;
using Elastos::Droid::Provider::ITelephonyMmsSms;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::ITelephonyMmsPart;
using Elastos::Droid::Provider::ITelephonyBaseMmsColumns;
using Elastos::Droid::Provider::ITelephonyMmsSmsPendingMessages;
using Elastos::Droid::Provider::CTelephonyMmsSmsPendingMessages;
using Elastos::Droid::Provider::ITelephonyMmsAddr;
using Elastos::Droid::Provider::CTelephonyThreads;
using Elastos::Droid::Provider::ITelephonyThreads;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Google::Mms::IContentTypeHelper;
using Elastos::Droid::Google::Mms::CContentTypeHelper;
using Elastos::Droid::Google::Mms::Utility::IAbstractCache;
using Elastos::Droid::Google::Mms::Utility::IPduCacheEntry;
using Elastos::Droid::Google::Mms::Utility::CPduCacheEntry;
using Elastos::Droid::Google::Mms::Utility::ISqliteWrapper;
using Elastos::Droid::Google::Mms::Utility::CSqliteWrapper;
using Elastos::Droid::Google::Mms::Utility::IDrmConvertSession;
using Elastos::Droid::Google::Mms::Utility::IDrmConvertSessionHelper;
using Elastos::Droid::Google::Mms::Utility::CDrmConvertSessionHelper;
using Elastos::Droid::Google::Mms::Utility::IDownloadDrmHelper;
using Elastos::Droid::Google::Mms::Utility::CDownloadDrmHelper;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CFile;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                             PduPersister
//=====================================================================
CAR_INTERFACE_IMPL(PduPersister, Object, IPduPersister);

const String PduPersister::TEMPORARY_DRM_OBJECT_URI = String("content://mms/") + StringUtils::ToString(Elastos::Core::Math::INT64_MAX_VALUE) + String("/part");
const Int32 PduPersister::PROC_STATUS_TRANSIENT_FAILURE;
const Int32 PduPersister::PROC_STATUS_PERMANENTLY_FAILURE;
const Int32 PduPersister::PROC_STATUS_COMPLETED;
const String PduPersister::TAG("PduPersister");
const Boolean PduPersister::DEBUG = FALSE;
const Boolean PduPersister::LOCAL_LOGV = FALSE;
const Int64 PduPersister::DUMMY_THREAD_ID;
AutoPtr<IPduPersister> PduPersister::sPersister;
const AutoPtr<IPduCache> PduPersister::PDU_CACHE_INSTANCE;
AutoPtr<ArrayOf<Int32> > PduPersister::ADDRESS_FIELDS = PduPersister::MiddleInitAddressFields();
AutoPtr<ArrayOf<String> > PduPersister::PDU_PROJECTION = PduPersister::MiddleInitPduProjection();
const Int32 PduPersister::PDU_COLUMN_ID;
const Int32 PduPersister::PDU_COLUMN_MESSAGE_BOX;
const Int32 PduPersister::PDU_COLUMN_THREAD_ID;
const Int32 PduPersister::PDU_COLUMN_RETRIEVE_TEXT;
const Int32 PduPersister::PDU_COLUMN_SUBJECT;
const Int32 PduPersister::PDU_COLUMN_CONTENT_LOCATION;
const Int32 PduPersister::PDU_COLUMN_CONTENT_TYPE;
const Int32 PduPersister::PDU_COLUMN_MESSAGE_CLASS;
const Int32 PduPersister::PDU_COLUMN_MESSAGE_ID;
const Int32 PduPersister::PDU_COLUMN_RESPONSE_TEXT;
const Int32 PduPersister::PDU_COLUMN_TRANSACTION_ID;
const Int32 PduPersister::PDU_COLUMN_CONTENT_CLASS;
const Int32 PduPersister::PDU_COLUMN_DELIVERY_REPORT;
const Int32 PduPersister::PDU_COLUMN_MESSAGE_TYPE;
const Int32 PduPersister::PDU_COLUMN_MMS_VERSION;
const Int32 PduPersister::PDU_COLUMN_PRIORITY;
const Int32 PduPersister::PDU_COLUMN_READ_REPORT;
const Int32 PduPersister::PDU_COLUMN_READ_STATUS;
const Int32 PduPersister::PDU_COLUMN_REPORT_ALLOWED;
const Int32 PduPersister::PDU_COLUMN_RETRIEVE_STATUS;
const Int32 PduPersister::PDU_COLUMN_STATUS;
const Int32 PduPersister::PDU_COLUMN_DATE;
const Int32 PduPersister::PDU_COLUMN_DELIVERY_TIME;
const Int32 PduPersister::PDU_COLUMN_EXPIRY;
const Int32 PduPersister::PDU_COLUMN_MESSAGE_SIZE;
const Int32 PduPersister::PDU_COLUMN_SUBJECT_CHARSET;
const Int32 PduPersister::PDU_COLUMN_RETRIEVE_TEXT_CHARSET;
AutoPtr<ArrayOf<String> > PduPersister::PART_PROJECTION = PduPersister::MiddleInitPartProjection();
const Int32 PduPersister::PART_COLUMN_ID;
const Int32 PduPersister::PART_COLUMN_CHARSET;
const Int32 PduPersister::PART_COLUMN_CONTENT_DISPOSITION;
const Int32 PduPersister::PART_COLUMN_CONTENT_ID;
const Int32 PduPersister::PART_COLUMN_CONTENT_LOCATION;
const Int32 PduPersister::PART_COLUMN_CONTENT_TYPE;
const Int32 PduPersister::PART_COLUMN_FILENAME;
const Int32 PduPersister::PART_COLUMN_NAME;
const Int32 PduPersister::PART_COLUMN_TEXT;
AutoPtr<IHashMap> PduPersister::MESSAGE_BOX_MAP;
AutoPtr<IHashMap> PduPersister::CHARSET_COLUMN_INDEX_MAP;
AutoPtr<IHashMap> PduPersister::ENCODED_STRING_COLUMN_INDEX_MAP;
AutoPtr<IHashMap> PduPersister::TEXT_STRING_COLUMN_INDEX_MAP;
AutoPtr<IHashMap> PduPersister::OCTET_COLUMN_INDEX_MAP;
AutoPtr<IHashMap> PduPersister::LONG_COLUMN_INDEX_MAP;
AutoPtr<IHashMap> PduPersister::CHARSET_COLUMN_NAME_MAP;
AutoPtr<IHashMap> PduPersister::ENCODED_STRING_COLUMN_NAME_MAP;
AutoPtr<IHashMap> PduPersister::TEXT_STRING_COLUMN_NAME_MAP;
AutoPtr<IHashMap> PduPersister::OCTET_COLUMN_NAME_MAP;
AutoPtr<IHashMap> PduPersister::LONG_COLUMN_NAME_MAP;

AutoPtr<IPduPersister> PduPersister::GetPduPersister(
    /* [in] */ IContext* context)
{
    if ((sPersister == NULL)) {
        sPersister = new PduPersister(context);
    }
    else {
        AutoPtr<PduPersister> p = (PduPersister*)sPersister.Get();
        if (!(Object::Equals(context, p->mContext))) {
            p->Release();
            sPersister = new PduPersister(context);
        }
    }

    return sPersister;
}

ECode PduPersister::Load(
    /* [in] */ IUri* uri,
    /* [out] */ IGenericPdu** result)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGenericPdu> pdu;
    AutoPtr<IPduCacheEntry> cacheEntry;
    Int32 msgBox = 0;
    Int64 threadId = -1;
    // try {
        {
            AutoLock lock(cache_lock);
            Boolean b = FALSE;
            PDU_CACHE_INSTANCE->IsUpdating(uri, &b);
            if (b) {
                if (LOCAL_LOGV) {
                    Logger::V(TAG, String("load: ") + TO_CSTR(uri) + String(" blocked by isUpdating()"));
                }
                cache_lock.Wait();
                AutoPtr<IInterface> p;
                IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Get(uri, (IInterface**)&p);
                cacheEntry = IPduCacheEntry::Probe(p);
                if (cacheEntry != NULL) {
                    return cacheEntry->GetPdu(result);
                }
            }
            // Tell the cache to indicate to other callers that this item
            // is currently being updated.
            PDU_CACHE_INSTANCE->SetUpdating(uri, TRUE);
        }

        AutoPtr<ISqliteWrapper> sw;
        CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
        AutoPtr<ICursor> c;
        sw->Query(mContext, mContentResolver, uri,
                PDU_PROJECTION, String(NULL), NULL, String(NULL),
                (ICursor**)&c);
        AutoPtr<IPduHeaders> headers = new PduHeaders();
        AutoPtr<ISet> set;
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        Int64 msgId = 0;
        cu->ParseId(uri, &msgId);

        // try {
            if (c == NULL) {
                // throw new MmsException("Bad uri: " + uri);
                // return E_MMS_EXCEPTION;
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            Int32 count = 0;
            c->GetCount(&count);
            if (count != 1) {
                // throw new MmsException("Bad uri: " + uri);
                // return E_MMS_EXCEPTION;
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            Boolean bMF = FALSE;
            c->MoveToFirst(&bMF);
            if (!bMF) {
                // throw new MmsException("Bad uri: " + uri);
                // return E_MMS_EXCEPTION;
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            c->GetInt32(PDU_COLUMN_MESSAGE_BOX, &msgBox);
            c->GetInt64(PDU_COLUMN_THREAD_ID, &threadId);

            set = NULL;
            ENCODED_STRING_COLUMN_INDEX_MAP->GetEntrySet((ISet**)&set);
            Boolean bHasNext = FALSE;
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IMapEntry> e = IMapEntry::Probe(p);
                AutoPtr<IInterface> value;
                e->GetValue((IInterface**)&value);
                AutoPtr<IInterface> key;
                e->GetKey((IInterface**)&key);
                Int32 iValue = 0;
                IInteger32::Probe(value)->GetValue(&iValue);
                Int32 iKey = 0;
                IInteger32::Probe(key)->GetValue(&iKey);
                SetEncodedStringValueToHeaders(
                        c, iValue, headers, iKey);
            }

            set = NULL;
            TEXT_STRING_COLUMN_INDEX_MAP->GetEntrySet((ISet**)&set);
            it = NULL;
            set->GetIterator((IIterator**)&it);
            bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IMapEntry> e = IMapEntry::Probe(p);
                AutoPtr<IInterface> value;
                e->GetValue((IInterface**)&value);
                AutoPtr<IInterface> key;
                e->GetKey((IInterface**)&key);
                Int32 iValue = 0;
                IInteger32::Probe(value)->GetValue(&iValue);
                Int32 iKey = 0;
                IInteger32::Probe(key)->GetValue(&iKey);
                SetTextStringToHeaders(
                        c, iValue, headers, iKey);
            }

            set = NULL;
            OCTET_COLUMN_INDEX_MAP->GetEntrySet((ISet**)&set);
            it = NULL;
            set->GetIterator((IIterator**)&it);
            bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IMapEntry> e = IMapEntry::Probe(p);
                AutoPtr<IInterface> value;
                e->GetValue((IInterface**)&value);
                AutoPtr<IInterface> key;
                e->GetKey((IInterface**)&key);
                Int32 iValue = 0;
                IInteger32::Probe(value)->GetValue(&iValue);
                Int32 iKey = 0;
                IInteger32::Probe(key)->GetValue(&iKey);
                SetOctetToHeaders(
                        c, iValue, headers, iKey);
            }

            set = NULL;
            LONG_COLUMN_INDEX_MAP->GetEntrySet((ISet**)&set);
            it = NULL;
            set->GetIterator((IIterator**)&it);
            bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IMapEntry> e = IMapEntry::Probe(p);
                AutoPtr<IInterface> value;
                e->GetValue((IInterface**)&value);
                AutoPtr<IInterface> key;
                e->GetKey((IInterface**)&key);
                Int32 iValue = 0;
                IInteger32::Probe(value)->GetValue(&iValue);
                Int32 iKey = 0;
                IInteger32::Probe(key)->GetValue(&iKey);
                SetLongToHeaders(
                        c, iValue, headers, iKey);
            }
        // } finally {
            if (c != NULL) {
                ICloseable::Probe(c)->Close();
            }
        // }

        // Check whether 'msgId' has been assigned a valid value.
        if (msgId == -1L) {
            // throw new MmsException("Error! ID of the message: -1.");
            // return E_MMS_EXCEPTION;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // Load address information of the MM.
        LoadAddress(msgId, headers);

        Int32 msgType = 0;
        headers->GetOctet(IPduHeaders::MESSAGE_TYPE, &msgType);
        AutoPtr<IPduBody> body;
        CPduBody::New((IPduBody**)&body);

        // For PDU which type is M_retrieve.conf or Send.req, we should
        // load multiparts and put them into the body of the PDU.
        if ((msgType == IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF)
                || (msgType == IPduHeaders::MESSAGE_TYPE_SEND_REQ)) {
            AutoPtr<ArrayOf<IPduPart*> > parts = LoadParts(msgId);
            if (parts != NULL) {
                Int32 partsNum = parts->GetLength();
                for (Int32 i = 0; i < partsNum; i++) {
                    Boolean b = FALSE;
                    body->AddPart((*parts)[i], &b);
                }
            }
        }

        switch (msgType) {
        case IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND: {
            CNotificationInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_DELIVERY_IND: {
            CDeliveryInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_ORIG_IND: {
            CReadOrigInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF: {
            CRetrieveConf::New(headers, body, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_SEND_REQ: {
            CSendReq::New(headers, body, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND: {
            CAcknowledgeInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND: {
            CNotifyRespInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_REC_IND: {
            CReadRecInd::New(headers, (IGenericPdu**)&pdu);
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_SEND_CONF:
        case IPduHeaders::MESSAGE_TYPE_FORWARD_REQ:
        case IPduHeaders::MESSAGE_TYPE_FORWARD_CONF:
        case IPduHeaders::MESSAGE_TYPE_MBOX_STORE_REQ:
        case IPduHeaders::MESSAGE_TYPE_MBOX_STORE_CONF:
        case IPduHeaders::MESSAGE_TYPE_MBOX_VIEW_REQ:
        case IPduHeaders::MESSAGE_TYPE_MBOX_VIEW_CONF:
        case IPduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_REQ:
        case IPduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_CONF:
        case IPduHeaders::MESSAGE_TYPE_MBOX_DELETE_REQ:
        case IPduHeaders::MESSAGE_TYPE_MBOX_DELETE_CONF:
        case IPduHeaders::MESSAGE_TYPE_MBOX_DESCR:
        case IPduHeaders::MESSAGE_TYPE_DELETE_REQ:
        case IPduHeaders::MESSAGE_TYPE_DELETE_CONF:
        case IPduHeaders::MESSAGE_TYPE_CANCEL_REQ:
        case IPduHeaders::MESSAGE_TYPE_CANCEL_CONF: {
            // throw new MmsException(
            //         "Unsupported PDU type: " + Integer.toHexString(msgType));
            // return E_MMS_EXCEPTION;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        default: {
            // throw new MmsException(
            //         "Unrecognized PDU type: " + Integer.toHexString(msgType));
            // return E_MMS_EXCEPTION;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        }
    // } finally {
        {
            AutoLock lock(cache_lock);
            if (pdu != NULL) {
                AutoPtr<IInterface> p;
                IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Get(uri, (IInterface**)&p);
                assert(p == NULL);
                // Update the cache entry with the real info
                CPduCacheEntry::New(pdu, msgBox, threadId, (IPduCacheEntry**)&cacheEntry);
                Boolean res = FALSE;
                IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Put(uri, cacheEntry, &res);
            }
            PDU_CACHE_INSTANCE->SetUpdating(uri, FALSE);
            cache_lock.NotifyAll(); // tell anybody waiting on this entry to go ahead
        }
    // }
    *result = pdu;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduPersister::PersistPart(
    /* [in] */ IPduPart* part,
    /* [in] */ Int64 msgId,
    /* [in] */ IHashMap* preOpenedFiles,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->Parse(String("content://mms/") + StringUtils::ToString(msgId) + String("/part"), (IUri**)&uri);
    AutoPtr<IContentValues> values;
    CContentValues::New(8, (IContentValues**)&values);

    Int32 charset = 0;
    part->GetCharset(&charset);
    if (charset != 0 ) {
        values->Put(ITelephonyMmsPart::CHARSET, charset);
    }

    String contentType = GetPartContentType(part);
    if (!contentType.IsNull()) {
        // There is no "image/jpg" in Android (and it's an invalid mimetype).
        // Change it to "image/jpeg"
        if (IContentType::IMAGE_JPG.Equals(contentType)) {
            contentType = IContentType::IMAGE_JPEG;
        }

        values->Put(ITelephonyMmsPart::CONTENT_TYPE, contentType);
        // To ensure the SMIL part is always the first part.
        if (IContentType::APP_SMIL.Equals(contentType)) {
            values->Put(ITelephonyMmsPart::SEQ, -1);
        }
    }
    else {
        // throw new MmsException("MIME type of the part must be set.");
        // return E_MMS_EXCEPTION;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > fileName;
    part->GetFilename((ArrayOf<Byte>**)&fileName);
    if (fileName != NULL) {
        String str(*fileName);
        values->Put(ITelephonyMmsPart::FILENAME, str);
    }

    AutoPtr<ArrayOf<Byte> > name;
    part->GetName((ArrayOf<Byte>**)&name);
    if (name != NULL) {
        String str(*name);
        values->Put(ITelephonyMmsPart::NAME, name);
    }

    AutoPtr<ArrayOf<Byte> > pos;
    part->GetContentDisposition((ArrayOf<Byte>**)&pos);
    String value;
    if (pos != NULL) {
        value = ToIsoString(pos);
        values->Put(ITelephonyMmsPart::CONTENT_DISPOSITION, (String) value);
    }

    AutoPtr<ArrayOf<Byte> > contentId;
    part->GetContentId((ArrayOf<Byte>**)&contentId);
    if (contentId != NULL) {
        value = ToIsoString(contentId);
        values->Put(ITelephonyMmsPart::CONTENT_ID, (String) value);
    }

    AutoPtr<ArrayOf<Byte> > location;
    part->GetContentLocation((ArrayOf<Byte>**)&location);
    if (location != NULL) {
        value = ToIsoString(location);
        values->Put(ITelephonyMmsPart::CONTENT_LOCATION, (String) value);
    }

    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUri> res;
    sw->Insert(mContext, mContentResolver, uri, values, (IUri**)&res);
    if (res == NULL) {
        // throw new MmsException("Failed to persist part, return NULL.");
        // return E_MMS_EXCEPTION;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    PersistData(part, res, contentType, preOpenedFiles);
    // After successfully store the data, we should update
    // the dataUri of the part.
    part->SetDataUri(res);

    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

String PduPersister::ConvertUriToPath(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    String path(NULL);
    if (NULL != uri) {
        String scheme;
        uri->GetScheme(&scheme);
        if (scheme.IsNull() || scheme.Equals("") ||
                scheme.Equals(IContentResolver::SCHEME_FILE)) {
            uri->GetPath(&path);

        }
        else if (scheme.Equals("http")) {
            assert(0 && "TODO");
            // uri->ToString(&path);

        }
        else if (scheme.Equals(IContentResolver::SCHEME_CONTENT)) {
            AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
            (*projection)[0] = IMediaStoreMediaColumns::DATA;
            AutoPtr<ICursor> cursor;
            // try {
                AutoPtr<IContentResolver> cr;
                context->GetContentResolver((IContentResolver**)&cr);
                cr->Query(uri, projection, String(NULL),
                        NULL, String(NULL), (ICursor**)&cursor);
                if (NULL == cursor) {
                    // throw new IllegalArgumentException("Given Uri could not be found" +
                    //         " in media store");
                    return String(NULL);
                }
                Int32 count = 0;
                cursor->GetCount(&count);
                if (0 == count) {
                    // throw new IllegalArgumentException("Given Uri could not be found" +
                    //         " in media store");
                    return String(NULL);
                }
                Boolean bMF = FALSE;
                cursor->MoveToFirst(&bMF);
                if (!bMF) {
                    // throw new IllegalArgumentException("Given Uri could not be found" +
                    //         " in media store");
                    return String(NULL);
                }
                Int32 pathIndex = 0;
                cursor->GetColumnIndexOrThrow(IMediaStoreMediaColumns::DATA, &pathIndex);
                cursor->GetString(pathIndex, &path);
            // } catch (SQLiteException e) {
            //     throw new IllegalArgumentException("Given Uri is not formatted in a way " +
            //             "so that it can be found in media store.");
            // } finally {
                if (NULL != cursor) {
                    ICloseable::Probe(cursor)->Close();
                }
            // }
        }
        else {
            // throw new IllegalArgumentException("Given Uri scheme is not supported");
            return String(NULL);
        }
    }
    return path;
}

ECode PduPersister::UpdateHeaders(
    /* [in] */ IUri* uri,
    /* [in] */ ISendReq* sendReq)
{
    VALIDATE_NOT_NULL(sendReq);
    {
        AutoLock lock(cache_lock);
        // If the cache item is getting updated, wait until it's done updating before
        // purging it.
        Boolean b = FALSE;
        PDU_CACHE_INSTANCE->IsUpdating(uri, &b);
        if (b) {
            if (LOCAL_LOGV) {
                Logger::V(TAG, String("updateHeaders: ") + TO_CSTR(uri) + String(" blocked by isUpdating()"));
            }
            // try {
                cache_lock.Wait();
            // } catch (InterruptedException e) {
            //     Log.e(TAG, "updateHeaders: ", e);
            // }
        }
    }
    AutoPtr<IInterface> resPurge;
    IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Purge(uri, (IInterface**)&resPurge);

    AutoPtr<IContentValues> values;
    CContentValues::New(10, (IContentValues**)&values);
    AutoPtr<ArrayOf<Byte> > contentType;
    sendReq->GetContentType((ArrayOf<Byte>**)&contentType);
    if (contentType != NULL) {
        values->Put(ITelephonyBaseMmsColumns::CONTENT_TYPE, ToIsoString(contentType));
    }

    Int64 date = 0;
    IMultimediaMessagePdu::Probe(sendReq)->GetDate(&date);
    if (date != -1) {
        values->Put(ITelephonyBaseMmsColumns::DATE, date);
    }

    Int32 deliveryReport = 0;
    sendReq->GetDeliveryReport(&deliveryReport);
    if (deliveryReport != 0) {
        values->Put(ITelephonyBaseMmsColumns::DELIVERY_REPORT, deliveryReport);
    }

    Int64 expiry = 0;
    sendReq->GetExpiry(&expiry);
    if (expiry != -1) {
        values->Put(ITelephonyBaseMmsColumns::EXPIRY, expiry);
    }

    AutoPtr<ArrayOf<Byte> > msgClass;
    sendReq->GetMessageClass((ArrayOf<Byte>**)&msgClass);
    if (msgClass != NULL) {
        values->Put(ITelephonyBaseMmsColumns::MESSAGE_CLASS, ToIsoString(msgClass));
    }

    Int32 priority = 0;
    IMultimediaMessagePdu::Probe(sendReq)->GetPriority(&priority);
    if (priority != 0) {
        values->Put(ITelephonyBaseMmsColumns::PRIORITY, priority);
    }

    Int32 readReport = 0;
    sendReq->GetReadReport(&readReport);
    if (readReport != 0) {
        values->Put(ITelephonyBaseMmsColumns::READ_REPORT, readReport);
    }

    AutoPtr<ArrayOf<Byte> > transId;
    sendReq->GetTransactionId((ArrayOf<Byte>**)&transId);
    if (transId != NULL) {
        values->Put(ITelephonyBaseMmsColumns::TRANSACTION_ID, ToIsoString(transId));
    }

    AutoPtr<IEncodedStringValue> subject;
    IMultimediaMessagePdu::Probe(sendReq)->GetSubject((IEncodedStringValue**)&subject);
    if (subject != NULL) {
        AutoPtr<ArrayOf<Byte> > arr;
        subject->GetTextString((ArrayOf<Byte>**)&arr);
        values->Put(ITelephonyBaseMmsColumns::SUBJECT, ToIsoString(arr));
        Int32 cs = 0;
        subject->GetCharacterSet(&cs);
        values->Put(ITelephonyBaseMmsColumns::SUBJECT_CHARSET, cs);
    }
    else {
        values->Put(ITelephonyBaseMmsColumns::SUBJECT, String(""));
    }

    Int64 messageSize = 0;
    sendReq->GetMessageSize(&messageSize);
    if (messageSize > 0) {
        values->Put(ITelephonyBaseMmsColumns::MESSAGE_SIZE, messageSize);
    }

    AutoPtr<IPduHeaders> headers;
    IGenericPdu::Probe(sendReq)->GetPduHeaders((IPduHeaders**)&headers);
    AutoPtr<IHashSet> recipients;
    CHashSet::New((IHashSet**)&recipients);
    for (Int32 i = 0; i < ADDRESS_FIELDS->GetLength(); i++) {
        Int32 addrType = (*ADDRESS_FIELDS)[i];
        AutoPtr<ArrayOf<IEncodedStringValue*> > array;
        if (addrType == IPduHeaders::FROM) {
            AutoPtr<IEncodedStringValue> v;
            headers->GetEncodedStringValue(addrType, (IEncodedStringValue**)&v);
            if (v != NULL) {
                array = ArrayOf<IEncodedStringValue*>::Alloc(1);
                (*array)[0] = v;
            }
        }
        else {
            headers->GetEncodedStringValues(addrType, (ArrayOf<IEncodedStringValue*>**)&array);
        }

        if (array != NULL) {
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            Int64 msgId = 0;
            cu->ParseId(uri, &msgId);
            UpdateAddress(msgId, addrType, array);
            if (addrType == IPduHeaders::TO) {
                for (Int32 i = 0; i < array->GetLength(); i++) {
                    AutoPtr<IEncodedStringValue> v = (*array)[i];
                    if (v != NULL) {
                        String str;
                        v->GetString(&str);
                        recipients->Add(CoreUtils::Convert(str));
                    }
                }
            }
        }
    }
    Boolean bEmp = FALSE;
    recipients->IsEmpty(&bEmp);
    if (!bEmp) {
        AutoPtr<ITelephonyThreads> t;
        CTelephonyThreads::AcquireSingleton((ITelephonyThreads**)&t);
        Int64 threadId = 0;
        t->GetOrCreateThreadId(mContext, ISet::Probe(recipients), &threadId);
        values->Put(ITelephonyBaseMmsColumns::THREAD_ID, threadId);
    }

    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    Int32 res = 0;
    sw->Update(mContext, mContentResolver, uri, values, String(NULL), NULL, &res);
    return NOERROR;
}

ECode PduPersister::UpdateParts(
    /* [in] */ IUri* uri,
    /* [in] */ IPduBody* body,
    /* [in] */ IHashMap* preOpenedFiles)
{
    VALIDATE_NOT_NULL(preOpenedFiles);
    // try {
        AutoPtr<IPduCacheEntry> cacheEntry;
        {
            AutoLock lock(cache_lock);
            Boolean bUpdating = FALSE;
            PDU_CACHE_INSTANCE->IsUpdating(uri, &bUpdating);
            if (bUpdating) {
                if (LOCAL_LOGV) {
                    Logger::V(TAG, String("updateParts: ") + TO_CSTR(uri) + String(" blocked by isUpdating()"));
                }
                // try {
                    cache_lock.Wait();
                // } catch (InterruptedException e) {
                //     Log.e(TAG, "updateParts: ", e);
                // }
                AutoPtr<IInterface> p;
                IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Get(uri, (IInterface**)&p);
                cacheEntry = IPduCacheEntry::Probe(p);
                if (cacheEntry != NULL) {
                    AutoPtr<IGenericPdu> pdu;
                    cacheEntry->GetPdu((IGenericPdu**)&pdu);
                    ((MultimediaMessagePdu*)pdu.Get())->SetBody(body);
                }
            }
            // Tell the cache to indicate to other callers that this item
            // is currently being updated.
            PDU_CACHE_INSTANCE->SetUpdating(uri, TRUE);
        }

        AutoPtr<IArrayList> toBeCreated;
        CArrayList::New((IArrayList**)&toBeCreated);
        AutoPtr<IHashMap> toBeUpdated;
        CHashMap::New((IHashMap**)&toBeUpdated);

        Int32 partsNum = 0;
        body->GetPartsNum(&partsNum);
        AutoPtr<IStringBuilder> filter = new StringBuilder();
        filter->Append('(');
        for (Int32 i = 0; i < partsNum; i++) {
            AutoPtr<IPduPart> part;
            body->GetPart(i, (IPduPart**)&part);
            AutoPtr<IUri> partUri;
            part->GetDataUri((IUri**)&partUri);
            String authority;
            partUri->GetAuthority(&authority);
            if ((partUri == NULL) || TextUtils::IsEmpty(authority)
                    || !authority.StartWith("mms")) {
                toBeCreated->Add(part);
            }
            else {
                toBeUpdated->Put(partUri, part);

                // Don't use 'i > 0' to determine whether we should append
                // 'AND' since 'i = 0' may be skipped in another branch.
                Int32 length = 0;
                ICharSequence::Probe(filter)->GetLength(&length);
                if (length > 1) {
                    filter->Append(String(" AND "));
                }

                filter->Append(ITelephonyMmsPart::MSG_ID);
                filter->Append(String("!="));
                AutoPtr<IDatabaseUtils> du;
                CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&du);
                String strPath;
                partUri->GetLastPathSegment(&strPath);
                du->AppendEscapedSQLString(filter, strPath);
            }
        }
        filter->Append(')');

        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        Int64 msgId = 0;
        cu->ParseId(uri, &msgId);

        // Remove the parts which doesn't exist anymore.
        AutoPtr<ISqliteWrapper> sw;
        CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
        AutoPtr<IUriHelper> urihlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
        AutoPtr<ITelephonySms> sms;
        CTelephonySms::AcquireSingleton((ITelephonySms**)&sms);
        AutoPtr<IUri> contenturi;
        sms->GetCONTENT_URI((IUri**)&contenturi);
        AutoPtr<IUri> uriInner;
        urihlp->Parse(String(TO_CSTR(contenturi)) + String("/") + StringUtils::ToString(msgId) + String("/part"), (IUri**)&uriInner);
        Int32 length = 0;
        ICharSequence::Probe(filter)->GetLength(&length);
        String strFilter;
        ICharSequence::Probe(filter)->ToString(&strFilter);
        Int32 res = 0;
        sw->Delete(mContext, mContentResolver,
                uriInner,
                length > 2 ? strFilter : String(NULL), NULL, &res);

        // Create new parts which didn't exist before.
        AutoPtr<IIterator> it;
        toBeCreated->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IPduPart> part = IPduPart::Probe(p);
            AutoPtr<IUri> resUri;
            PersistPart(part, msgId, preOpenedFiles, (IUri**)&resUri);
        }

        // Update the modified parts.
        AutoPtr<ISet> st;
        toBeUpdated->GetEntrySet((ISet**)&st);
        bHasNext = FALSE;
        it = NULL;
        st->GetIterator((IIterator**)&it);
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IMapEntry> e = IMapEntry::Probe(p);
            AutoPtr<IInterface> key;
            e->GetKey((IInterface**)&key);
            AutoPtr<IInterface> value;
            e->GetValue((IInterface**)&value);
            UpdatePart(IUri::Probe(key), IPduPart::Probe(value), preOpenedFiles);
        }
    // } finally {
        {
            AutoLock lock(cache_lock);
            PDU_CACHE_INSTANCE->SetUpdating(uri, FALSE);
            cache_lock.NotifyAll();
        }
    // }
    return NOERROR;
}

ECode PduPersister::Persist(
    /* [in] */ IGenericPdu* pdu,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean createThreadId,
    /* [in] */ Boolean groupMmsEnabled,
    /* [in] */ IHashMap* preOpenedFiles,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    if (uri == NULL) {
        // throw new MmsException("Uri may not be NULL.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 msgId = -1;
    // try {
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    cu->ParseId(uri, &msgId);
    // } catch (NumberFormatException e) {
    //     // the uri ends with "inbox" or something else like that
    // }
    Boolean existingUri = msgId != -1;

    AutoPtr<IInterface> p;
    MESSAGE_BOX_MAP->Get(uri, (IInterface**)&p);
    if (!existingUri && p == NULL) {
        // throw new MmsException(
        //         "Bad destination, must be one of "
        //         + "content://mms/inbox, content://mms/sent, "
        //         + "content://mms/drafts, content://mms/outbox, "
        //         + "content://mms/temp.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock lock(cache_lock);
        // If the cache item is getting updated, wait until it's done updating before
        // purging it.
        Boolean bUpdating = FALSE;
        PDU_CACHE_INSTANCE->IsUpdating(uri, &bUpdating);
        if (bUpdating) {
            if (LOCAL_LOGV) {
                Logger::V(TAG, String("persist: ") + TO_CSTR(uri) + String(" blocked by isUpdating()"));
            }
            // try {
                cache_lock.Wait();
            // } catch (InterruptedException e) {
            //     Log.e(TAG, "persist1: ", e);
            // }
        }
    }
    AutoPtr<IInterface> pPurge;
    IAbstractCache::Probe(PDU_CACHE_INSTANCE)->Purge(uri, (IInterface**)&pPurge);

    AutoPtr<IPduHeaders> header;
    pdu->GetPduHeaders((IPduHeaders**)&header);
    AutoPtr<IPduBody> body;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<ISet> set;
    ENCODED_STRING_COLUMN_NAME_MAP->GetEntrySet((ISet**)&set);
    Boolean bHasNext = FALSE;
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> _e;
        it->GetNext((IInterface**)&_e);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(_e);
        AutoPtr<IInterface> key;
        e->GetKey((IInterface**)&key);
        Int32 field = 0;
        IInteger32::Probe(key)->GetValue(&field);
        AutoPtr<IEncodedStringValue> encodedString;
        header->GetEncodedStringValue(field, (IEncodedStringValue**)&encodedString);
        if (encodedString != NULL) {
            AutoPtr<IInterface> p;
            CHARSET_COLUMN_NAME_MAP->Get(CoreUtils::Convert(field), (IInterface**)&p);
            String charsetColumn;
            ICharSequence::Probe(p)->ToString(&charsetColumn);
            AutoPtr<IInterface> value;
            e->GetValue((IInterface**)&value);
            AutoPtr<ArrayOf<Byte> > arr;
            encodedString->GetTextString((ArrayOf<Byte>**)&arr);
            String strValue;
            ICharSequence::Probe(value)->ToString(&strValue);
            values->Put(strValue, ToIsoString(arr));
            Int32 charset = 0;
            encodedString->GetCharacterSet(&charset);
            values->Put(charsetColumn, charset);
        }
    }

    set = NULL;
    TEXT_STRING_COLUMN_NAME_MAP->GetEntrySet((ISet**)&set);
    bHasNext = FALSE;
    it = NULL;
    set->GetIterator((IIterator**)&it);
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> _e;
        it->GetNext((IInterface**)&_e);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(_e);
        AutoPtr<IInterface> key;
        e->GetKey((IInterface**)&key);
        Int32 iKey = 0;
        IInteger32::Probe(key)->GetValue(&iKey);
        AutoPtr<ArrayOf<Byte> > text;
        header->GetTextString(iKey, (ArrayOf<Byte>**)&text);
        if (text != NULL) {
            AutoPtr<IInterface> value;
            e->GetValue((IInterface**)&value);
            String strValue;
            ICharSequence::Probe(value)->ToString(&strValue);
            values->Put(strValue, ToIsoString(text));
        }
    }

    set = NULL;
    OCTET_COLUMN_NAME_MAP->GetEntrySet((ISet**)&set);
    bHasNext = FALSE;
    it = NULL;
    set->GetIterator((IIterator**)&it);
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> _e;
        it->GetNext((IInterface**)&_e);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(_e);
        AutoPtr<IInterface> key;
        e->GetKey((IInterface**)&key);
        Int32 iKey = 0;
        IInteger32::Probe(key)->GetValue(&iKey);
        Int32 b = 0;
        header->GetOctet(iKey, &b);
        if (b != 0) {
            AutoPtr<IInterface> value;
            e->GetValue((IInterface**)&value);
            String strValue;
            ICharSequence::Probe(value)->ToString(&strValue);
            values->Put(strValue, CoreUtils::Convert(b));
        }
    }

    set = NULL;
    LONG_COLUMN_NAME_MAP->GetEntrySet((ISet**)&set);
    bHasNext = FALSE;
    it = NULL;
    set->GetIterator((IIterator**)&it);
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> _e;
        it->GetNext((IInterface**)&_e);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(_e);
        AutoPtr<IInterface> key;
        e->GetKey((IInterface**)&key);
        Int32 iKey = 0;
        IInteger32::Probe(key)->GetValue(&iKey);
        Int64 l = 0;
        header->GetLongInteger(iKey, &l);
        if (l != -1L) {
            AutoPtr<IInterface> value;
            e->GetValue((IInterface**)&value);
            String strValue;
            ICharSequence::Probe(value)->ToString(&strValue);
            values->Put(strValue, CoreUtils::Convert(l));
        }
    }

    AutoPtr<IHashMap> addressMap;
    CHashMap::New(ADDRESS_FIELDS->GetLength(), (IHashMap**)&addressMap);
    // Save address information.
    for (Int32 i = 0; i < ADDRESS_FIELDS->GetLength(); i++) {
        Int32 addrType = (*ADDRESS_FIELDS)[i];
        AutoPtr<ArrayOf<IEncodedStringValue*> > array;
        if (addrType == IPduHeaders::FROM) {
            AutoPtr<IEncodedStringValue> v;
            header->GetEncodedStringValue(addrType, (IEncodedStringValue**)&v);
            if (v != NULL) {
                array = ArrayOf<IEncodedStringValue*>::Alloc(1);
                (*array)[0] = v;
            }
        }
        else {
            header->GetEncodedStringValues(addrType, (ArrayOf<IEncodedStringValue*>**)&array);
        }
        AutoPtr<IArrayList> pArray;
        CArrayList::New((IArrayList**)&pArray);
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            pArray->Add((*array)[i]);
        }
        addressMap->Put(CoreUtils::Convert(addrType), pArray);
    }

    AutoPtr<IHashSet> recipients;
    CHashSet::New((IHashSet**)&recipients);
    Int32 msgType = 0;
    pdu->GetMessageType(&msgType);
    // Here we only allocate thread ID for M-Notification.ind,
    // M-Retrieve.conf and M-Send.req.
    // Some of other PDU types may be allocated a thread ID outside
    // this scope.
    if ((msgType == IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND)
            || (msgType == IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF)
            || (msgType == IPduHeaders::MESSAGE_TYPE_SEND_REQ)) {
        switch (msgType) {
            case IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND:
            case IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF: {
                LoadRecipients(IPduHeaders::FROM, recipients, addressMap, FALSE);

                // For received messages when group MMS is enabled, we want to associate this
                // message with the thread composed of all the recipients -- all but our own
                // number, that is. This includes the person who sent the
                // message or the FROM field (above) in addition to the other people the message
                // was addressed to or the TO field. Our own number is in that TO field and
                // we have to ignore it in loadRecipients.
                if (groupMmsEnabled) {
                    LoadRecipients(IPduHeaders::TO, recipients, addressMap, TRUE);

                    // Also load any numbers in the CC field to address group messaging
                    // compatibility issues with devices that place numbers in this field
                    // for group messages.
                    LoadRecipients(IPduHeaders::CC, recipients, addressMap, TRUE);
                }
                break;
            }
            case IPduHeaders::MESSAGE_TYPE_SEND_REQ: {
                LoadRecipients(IPduHeaders::TO, recipients, addressMap, FALSE);
                break;
            }
        }
        Int64 threadId = 0;
        Boolean bEmp = FALSE;
        recipients->IsEmpty(&bEmp);
        if (createThreadId && !bEmp) {
            // Given all the recipients associated with this message, find (or create) the
            // correct thread.
            AutoPtr<ITelephonyThreads> t;
            CTelephonyThreads::AcquireSingleton((ITelephonyThreads**)&t);
            t->GetOrCreateThreadId(mContext, ISet::Probe(recipients), &threadId);
        }
        values->Put(ITelephonyBaseMmsColumns::THREAD_ID, threadId);
    }

    // Save parts first to avoid inconsistent message is loaded
    // while saving the parts.
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 dummyId = 0;
    sys->GetCurrentTimeMillis(&dummyId); // Dummy ID of the msg.

    // Figure out if this PDU is a text-only message
    Boolean textOnly = TRUE;

    // Sum up the total message size
    Int32 messageSize = 0;

    // Get body if the PDU is a RetrieveConf or SendReq.
    if (IMultimediaMessagePdu::Probe(pdu) != NULL) {
        ((MultimediaMessagePdu*) pdu)->GetBody((IPduBody**)&body);
        // Start saving parts if necessary.
        if (body != NULL) {
            Int32 partsNum = 0;
            body->GetPartsNum(&partsNum);
            if (partsNum > 2) {
                // For a text-only message there will be two parts: 1-the SMIL, 2-the text.
                // Down a few lines below we're checking to make sure we've only got SMIL or
                // text. We also have to check then we don't have more than two parts.
                // Otherwise, a slideshow with two text slides would be marked as textOnly.
                textOnly = FALSE;
            }
            for (Int32 i = 0; i < partsNum; i++) {
                AutoPtr<IPduPart> part;
                body->GetPart(i, (IPduPart**)&part);
                Int32 len = 0;
                part->GetDataLength(&len);
                messageSize += len;
                AutoPtr<IUri> resUri;
                PersistPart(part, dummyId, preOpenedFiles, (IUri**)&resUri);

                // If we've got anything besides text/plain or SMIL part, then we've got
                // an mms message with some other type of attachment.
                String contentType = GetPartContentType(part);
                if (contentType != NULL && !IContentType::APP_SMIL.Equals(contentType)
                        && !IContentType::TEXT_PLAIN.Equals(contentType)) {
                    textOnly = FALSE;
                }
            }
        }
    }
    // Record whether this mms message is a simple plain text or not. This is a hint for the
    // UI.
    values->Put(ITelephonyBaseMmsColumns::TEXT_ONLY, textOnly ? 1 : 0);
    // The message-size might already have been inserted when parsing the
    // PDU header. If not, then we insert the message size as well.
    AutoPtr<IInteger32> pInteger;
    values->GetAsInteger32(ITelephonyBaseMmsColumns::MESSAGE_SIZE, (IInteger32**)&pInteger);
    if (pInteger == NULL) {
        values->Put(ITelephonyBaseMmsColumns::MESSAGE_SIZE, messageSize);
    }

    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUri> res;
    if (existingUri) {
        res = uri;
        Int32 UpdateResult = 0;
        sw->Update(mContext, mContentResolver, res, values, String(NULL), NULL, &UpdateResult);
    }
    else {
        sw->Insert(mContext, mContentResolver, uri, values, (IUri**)&res);
        if (res == NULL) {
            // throw new MmsException("persist() failed: return NULL.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Get the real ID of the PDU and update all parts which were
        // saved with the dummy ID.
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        cu->ParseId(res, &msgId);
    }

    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ITelephonyMmsPart::MSG_ID, msgId);
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> newUri;
    urihlp->Parse(String("content://mms/") + StringUtils::ToString(dummyId) + String("/part"), (IUri**)&newUri);
    Int32 UpdateResult = 0;
    sw->Update(mContext, mContentResolver,
                        newUri,
                        values, String(NULL), NULL, &UpdateResult);
    // We should return the longest URI of the persisted PDU, for
    // example, if input URI is "content://mms/inbox" and the _ID of
    // persisted PDU is '8', we should return "content://mms/inbox/8"
    // instead of "content://mms/8".
    // FIXME: Should the MmsProvider be responsible for this???
    if (!existingUri) {
        urihlp->Parse(String(TO_CSTR(uri)) + String("/") + StringUtils::ToString(msgId), (IUri**)&res);
    }

    // Save address information.
    for (Int32 i = 0; i < ADDRESS_FIELDS->GetLength(); i++) {
        Int32 addrType = (*ADDRESS_FIELDS)[i];
        AutoPtr<IInterface> p;
        addressMap->Get(CoreUtils::Convert(addrType), (IInterface**)&p);
        AutoPtr<IArrayOf> pArray = IArrayOf::Probe(p);
        Int32 len = 0;
        pArray->GetLength(&len);
        AutoPtr<ArrayOf<IEncodedStringValue*> > array = ArrayOf<IEncodedStringValue*>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            pArray->Get(i, (IInterface**)&obj);
            (*array)[i] = IEncodedStringValue::Probe(obj);
        }
        if (array != NULL) {
            PersistAddress(msgId, addrType, array);
        }
    }

    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduPersister::Move(
    /* [in] */ IUri* from,
    /* [in] */ IUri* to,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // Check whether the 'msgId' has been assigned a valid value.
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    Int64 msgId = 0;
    cu->ParseId(from, &msgId);
    if (msgId == -1L) {
        // throw new MmsException("Error! ID of the message: -1.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Get corresponding Int32 value of destination box.
    AutoPtr<IInterface> p;
    MESSAGE_BOX_MAP->Get(to, (IInterface**)&p);
    AutoPtr<IInteger32> msgBox = IInteger32::Probe(p);
    if (msgBox == NULL) {
        // throw new MmsException(
        //         "Bad destination, must be one of "
        //         + "content://mms/inbox, content://mms/sent, "
        //         + "content://mms/drafts, content://mms/outbox, "
        //         + "content://mms/temp.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ITelephonyBaseMmsColumns::MESSAGE_BOX, msgBox);
    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    Int32 res = 0;
    sw->Update(mContext, mContentResolver, from, values, String(NULL), NULL, &res);
    return cu->WithAppendedId(to, msgId, result);
}

String PduPersister::ToIsoString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // try {
    return String(*bytes); // String(bytes, ICharacterSets::MIMENAME_ISO_8859_1);
    // } catch (UnsupportedEncodingException e) {
    //     // Impossible to reach here!
    //     Log.e(TAG, "ISO_8859_1 must be supported!", e);
    //     return "";
    // }
}

AutoPtr<ArrayOf<Byte> > PduPersister::GetBytes(
    /* [in] */ const String& data)
{
    // try {
    return data.GetBytes(); // ICharacterSets::MIMENAME_ISO_8859_1);
    // } catch (UnsupportedEncodingException e) {
    //     // Impossible to reach here!
    //     Log.e(TAG, "ISO_8859_1 must be supported!", e);
    //     return new byte[0];
    // }
}

ECode PduPersister::ReleaseResources()
{
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->Parse(TEMPORARY_DRM_OBJECT_URI, (IUri**)&uri);
    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    Int32 res = 0;
    sw->Delete(mContext, mContentResolver, uri, String(NULL), NULL, &res);
    return NOERROR;
}

ECode PduPersister::GetPendingMessages(
    /* [in] */ Int64 dueTime,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ITelephonyMmsSmsPendingMessages> pending;
    CTelephonyMmsSmsPendingMessages::AcquireSingleton((ITelephonyMmsSmsPendingMessages**)&pending);
    AutoPtr<IUri> uri;
    pending->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(String("protocol"), String("mms"));

    String selection = ITelephonyMmsSmsPendingMessages::ERROR_TYPE + String(" < ?")
            + String(" AND ") + ITelephonyMmsSmsPendingMessages::DUE_TIME + String(" <= ?");

    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(2);
    (*selectionArgs)[0] = StringUtils::ToString(ITelephonyMmsSms::ERR_TYPE_GENERIC_PERMANENT);
    (*selectionArgs)[1] = StringUtils::ToString(dueTime);

    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUri> buildUri;
    uriBuilder->Build((IUri**)&buildUri);
    return sw->Query(mContext, mContentResolver,
            buildUri, NULL, selection, selectionArgs,
            ITelephonyMmsSmsPendingMessages::DUE_TIME, result);
}

PduPersister::PduPersister(
    /* [in] */ IContext* context)
{
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    assert(0 && "TODO");
    // mDrmManagerClient = new DrmManagerClient(context);
    AutoPtr<IInterface> p;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&p);
    mTelephonyManager = ITelephonyManager::Probe(p);
}

AutoPtr<ArrayOf<Int32> > PduPersister::MiddleInitAddressFields()
{
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(4);
    (*result)[0] = IPduHeaders::BCC;
    (*result)[1] = IPduHeaders::CC;
    (*result)[2] = IPduHeaders::FROM;
    (*result)[3] = IPduHeaders::TO;
    return result;
}

AutoPtr<ArrayOf<String> > PduPersister::MiddleInitPduProjection()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(27);
    (*result)[0] = ITelephonyBaseMmsColumns::THREAD_ID;
    (*result)[1] = ITelephonyBaseMmsColumns::MESSAGE_BOX;
    (*result)[2] = ITelephonyBaseMmsColumns::THREAD_ID;
    (*result)[3] = ITelephonyBaseMmsColumns::RETRIEVE_TEXT;
    (*result)[4] = ITelephonyBaseMmsColumns::SUBJECT;
    (*result)[5] = ITelephonyBaseMmsColumns::CONTENT_LOCATION;
    (*result)[6] = ITelephonyBaseMmsColumns::CONTENT_TYPE;
    (*result)[7] = ITelephonyBaseMmsColumns::MESSAGE_CLASS;
    (*result)[8] = ITelephonyBaseMmsColumns::MESSAGE_ID;
    (*result)[9] = ITelephonyBaseMmsColumns::RESPONSE_TEXT;
    (*result)[10] = ITelephonyBaseMmsColumns::TRANSACTION_ID;
    (*result)[11] = ITelephonyBaseMmsColumns::CONTENT_CLASS;
    (*result)[12] = ITelephonyBaseMmsColumns::DELIVERY_REPORT;
    (*result)[13] = ITelephonyBaseMmsColumns::MESSAGE_TYPE;
    (*result)[14] = ITelephonyBaseMmsColumns::MMS_VERSION;
    (*result)[15] = ITelephonyBaseMmsColumns::PRIORITY;
    (*result)[16] = ITelephonyBaseMmsColumns::READ_REPORT;
    (*result)[17] = ITelephonyBaseMmsColumns::READ_STATUS;
    (*result)[18] = ITelephonyBaseMmsColumns::REPORT_ALLOWED;
    (*result)[19] = ITelephonyBaseMmsColumns::RETRIEVE_STATUS;
    (*result)[20] = ITelephonyBaseMmsColumns::STATUS;
    (*result)[21] = ITelephonyBaseMmsColumns::DATE;
    (*result)[22] = ITelephonyBaseMmsColumns::DELIVERY_TIME;
    (*result)[23] = ITelephonyBaseMmsColumns::EXPIRY;
    (*result)[24] = ITelephonyBaseMmsColumns::MESSAGE_SIZE;
    (*result)[25] = ITelephonyBaseMmsColumns::SUBJECT_CHARSET;
    (*result)[26] = ITelephonyBaseMmsColumns::RETRIEVE_TEXT_CHARSET;
    return result;
}

AutoPtr<ArrayOf<String> > PduPersister::MiddleInitPartProjection()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(9);
    (*result)[0] = ITelephonyMmsPart::MSG_ID;
    (*result)[1] = ITelephonyMmsPart::CHARSET;
    (*result)[2] = ITelephonyMmsPart::CONTENT_DISPOSITION;
    (*result)[3] = ITelephonyMmsPart::CONTENT_ID;
    (*result)[4] = ITelephonyMmsPart::CONTENT_LOCATION;
    (*result)[5] = ITelephonyMmsPart::CONTENT_TYPE;
    (*result)[6] = ITelephonyMmsPart::FILENAME;
    (*result)[7] = ITelephonyMmsPart::NAME;
    (*result)[8] = ITelephonyMmsPart::TEXT;
    return result;
}

void PduPersister::SetEncodedStringValueToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    String s;
    c->GetString(columnIndex, &s);
    if ((!s.IsNull()) && (s.GetLength() > 0)) {
        AutoPtr<IInterface> p;
        CHARSET_COLUMN_INDEX_MAP->Get(CoreUtils::Convert(mapColumn), (IInterface**)&p);
        Int32 charsetColumnIndex = 0;
        IInteger32::Probe(p)->GetValue(&charsetColumnIndex);
        Int32 charset = 0;
        c->GetInt32(charsetColumnIndex, &charset);
        AutoPtr<IEncodedStringValue> value;
        CEncodedStringValue::New(
                charset, GetBytes(s), (IEncodedStringValue**)&value);
        headers->SetEncodedStringValue(value, mapColumn);
    }
}

void PduPersister::SetTextStringToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    String s;
    c->GetString(columnIndex, &s);
    if (!s.IsNull()) {
        headers->SetTextString(GetBytes(s), mapColumn);
    }
}

void PduPersister::SetOctetToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    Boolean bNull = FALSE;
    c->IsNull(columnIndex, &bNull);
    if (!bNull) {
        Int32 b = 0;
        c->GetInt32(columnIndex, &b);
        headers->SetOctet(b, mapColumn);
    }
}

void PduPersister::SetLongToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    Boolean bNull = FALSE;
    c->IsNull(columnIndex, &bNull);
    if (!bNull) {
        Int64 l = 0;
        c->GetInt64(columnIndex, &l);
        headers->SetLongInteger(l, mapColumn);
    }
}

AutoPtr<IInteger32> PduPersister::GetIntegerFromPartColumn(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex)
{
    Boolean bNull = FALSE;
    c->IsNull(columnIndex, &bNull);
    if (!bNull) {
        Int32 res = 0;
        c->GetInt32(columnIndex, &res);
        return CoreUtils::Convert(res);
    }
    return NULL;
}

AutoPtr<ArrayOf<Byte> > PduPersister::GetByteArrayFromPartColumn(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex)
{
    Boolean bNull = FALSE;
    c->IsNull(columnIndex, &bNull);
    if (!bNull) {
        String str;
        c->GetString(columnIndex, &str);
        return GetBytes(str);
    }
    return NULL;
}

AutoPtr<ArrayOf<IPduPart*> > PduPersister::LoadParts(
    /* [in] */ Int64 msgId)
{
    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->Parse(String("content://mms/") + StringUtils::ToString(msgId) + String("/part"), (IUri**)&uri);
    AutoPtr<ICursor> c;
    sw->Query(mContext, mContentResolver,
            uri,
            PART_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&c);

    AutoPtr<ArrayOf<IPduPart*> > parts;

    // try {
        Int32 count = 0;
        if ((c == NULL) || ((c->GetCount(&count), count) == 0)) {
            if (LOCAL_LOGV) {
                Logger::V(TAG, String("loadParts(") + StringUtils::ToString(msgId) + String("): no part to load."));
            }
            return NULL;
        }

        Int32 partCount = 0;
        c->GetCount(&partCount);
        Int32 partIdx = 0;
        parts = ArrayOf<IPduPart*>::Alloc(partCount);
        Boolean bMN = FALSE;
        while ((c->MoveToNext(&bMN), bMN)) {
            AutoPtr<IPduPart> part;
            CPduPart::New((IPduPart**)&part);
            AutoPtr<IInteger32> charset = GetIntegerFromPartColumn(
                    c, PART_COLUMN_CHARSET);
            if (charset != NULL) {
                Int32 num = 0;
                charset->GetValue(&num);
                part->SetCharset(num);
            }

            AutoPtr<ArrayOf<Byte> > contentDisposition = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_CONTENT_DISPOSITION);
            if (contentDisposition != NULL) {
                part->SetContentDisposition(contentDisposition);
            }

            AutoPtr<ArrayOf<Byte> > contentId = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_CONTENT_ID);
            if (contentId != NULL) {
                part->SetContentId(contentId);
            }

            AutoPtr<ArrayOf<Byte> > contentLocation = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_CONTENT_LOCATION);
            if (contentLocation != NULL) {
                part->SetContentLocation(contentLocation);
            }

            AutoPtr<ArrayOf<Byte> > contentType = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_CONTENT_TYPE);
            if (contentType != NULL) {
                part->SetContentType(contentType);
            }
            else {
                // throw new MmsException("Content-Type must be set.");
                return NULL;
            }

            AutoPtr<ArrayOf<Byte> > fileName = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_FILENAME);
            if (fileName != NULL) {
                part->SetFilename(fileName);
            }

            AutoPtr<ArrayOf<Byte> > name = GetByteArrayFromPartColumn(
                    c, PART_COLUMN_NAME);
            if (name != NULL) {
                part->SetName(name);
            }

            // Construct a Uri for this part.
            Int64 partId = 0;
            c->GetInt64(PART_COLUMN_ID, &partId);
            AutoPtr<IUriHelper> urihlp;
            CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
            AutoPtr<IUri> partURI;
            urihlp->Parse(String("content://mms/part/") + StringUtils::ToString(partId), (IUri**)&partURI);
            part->SetDataUri(partURI);

            // For images/audio/video, we won't keep their data in Part
            // because their renderer accept Uri as source.
            AutoPtr<IContentTypeHelper> cthlp;
            CContentTypeHelper::AcquireSingleton((IContentTypeHelper**)&cthlp);
            String type = ToIsoString(contentType);
            Boolean bImgType = FALSE, bAudioType = FALSE, bVideoType = FALSE;
            cthlp->IsImageType(type, &bImgType);
            cthlp->IsAudioType(type, &bAudioType);
            cthlp->IsVideoType(type, &bVideoType);
            if (!bImgType && !bAudioType && !bVideoType) {
                AutoPtr<IByteArrayOutputStream> baos;
                CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
                AutoPtr<IInputStream> is;

                // Store simple string values directly in the database instead of an
                // external file.  This makes the text searchable and retrieval slightly
                // faster.
                if (IContentType::TEXT_PLAIN.Equals(type) || IContentType::APP_SMIL.Equals(type)
                        || IContentType::TEXT_HTML.Equals(type)) {
                    String text;
                    c->GetString(PART_COLUMN_TEXT, &text);
                    AutoPtr<IEncodedStringValue> p;
                    CEncodedStringValue::New(!text.IsNull() ? text : String(""), (IEncodedStringValue**)&p);
                    AutoPtr<ArrayOf<Byte> > blob;
                    p->GetTextString((ArrayOf<Byte>**)&blob);
                    IOutputStream::Probe(baos)->Write(blob, 0, blob->GetLength());
                }
                else {
                    // try {
                        mContentResolver->OpenInputStream(partURI, (IInputStream**)&is);

                        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(256);
                        Int32 len = 0;
                        is->Read(buffer, &len);
                        while (len >= 0) {
                            IOutputStream::Probe(baos)->Write(buffer, 0, len);
                            is->Read(buffer, &len);
                        }
                    // } catch (IOException e) {
                    //     Log.e(TAG, "Failed to load part data", e);
                    //     c.close();
                    //     throw new MmsException(e);
                    // } finally {
                        if (is != NULL) {
                            // try {
                                ICloseable::Probe(is)->Close();
                            // } catch (IOException e) {
                            //     Log.e(TAG, "Failed to close stream", e);
                            // } // Ignore
                        }
                    // }
                }
                AutoPtr<ArrayOf<Byte> > byteArr;
                baos->ToByteArray((ArrayOf<Byte>**)&byteArr);
                part->SetData(byteArr);
            }
            (*parts)[partIdx++] = part;
        }
    // } finally {
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
    // }

    return parts;
}

void PduPersister::LoadAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ IPduHeaders* headers)
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = ITelephonyMmsAddr::ADDRESS;
    (*arr)[1] = ITelephonyMmsAddr::CHARSET;
    (*arr)[2] = ITelephonyMmsAddr::TYPE;
    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->Parse(String("content://mms/") + StringUtils::ToString(msgId) + String("/addr"), (IUri**)&uri);
    AutoPtr<ICursor> c;
    sw->Query(mContext, mContentResolver,
            uri,
            arr,
            String(NULL), NULL, String(NULL),
            (ICursor**)&c);

    if (c != NULL) {
        // try {
            Boolean bMN = FALSE;
            while ((c->MoveToNext(&bMN), bMN)) {
                String addr;
                c->GetString(0, &addr);
                if (!TextUtils::IsEmpty(addr)) {
                    Int32 addrType = 0;
                    c->GetInt32(2, &addrType);
                    switch (addrType) {
                        case IPduHeaders::FROM: {
                            Int32 num = 0;
                            c->GetInt32(1, &num);
                            AutoPtr<IEncodedStringValue> p;
                            CEncodedStringValue::New(num, GetBytes(addr), (IEncodedStringValue**)&p);
                            headers->SetEncodedStringValue(p, addrType);
                            break;
                        }
                        case IPduHeaders::TO:
                        case IPduHeaders::CC:
                        case IPduHeaders::BCC: {
                            Int32 num = 0;
                            c->GetInt32(1, &num);
                            AutoPtr<IEncodedStringValue> p;
                            CEncodedStringValue::New(num, GetBytes(addr), (IEncodedStringValue**)&p);
                            headers->AppendEncodedStringValue(p, addrType);
                            break;
                        }
                        default: {
                            Logger::E(TAG, String("Unknown address type: ") + StringUtils::ToString(addrType));
                            break;
                        }
                    }
                }
            }
        // } finally {
            ICloseable::Probe(c)->Close();
        // }
    }
}

void PduPersister::PersistAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<IEncodedStringValue*>* array)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(3, (IContentValues**)&values);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IEncodedStringValue> addr = (*array)[i];
        values->Clear(); // Clear all values first.
        AutoPtr<ArrayOf<Byte> > text;
        addr->GetTextString((ArrayOf<Byte>**)&text);
        values->Put(ITelephonyMmsAddr::ADDRESS, ToIsoString(text));
        Int32 cs = 0;
        addr->GetCharacterSet(&cs);
        values->Put(ITelephonyMmsAddr::CHARSET, cs);
        values->Put(ITelephonyMmsAddr::TYPE, type);

        AutoPtr<IUriHelper> urihlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
        AutoPtr<IUri> uri;
        urihlp->Parse(String("content://mms/") + StringUtils::ToString(msgId) + String("/addr"), (IUri**)&uri);
        AutoPtr<ISqliteWrapper> sw;
        CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
        AutoPtr<IUri> result;
        sw->Insert(mContext, mContentResolver, uri, values, (IUri**)&result);
    }
}

String PduPersister::GetPartContentType(
    /* [in] */ IPduPart* part)
{
    AutoPtr<ArrayOf<Byte> > arr;
    part->GetContentType((ArrayOf<Byte>**)&arr);
    return arr == NULL ? String(NULL) : ToIsoString(arr);
}

void PduPersister::PersistData(
    /* [in] */ IPduPart* part,
    /* [in] */ IUri* uri,
    /* [in] */ const String& contentType,
    /* [in] */ IHashMap* preOpenedFiles)
{
    AutoPtr<IOutputStream> os;
    AutoPtr<IInputStream> is;
    AutoPtr<IDrmConvertSession> drmConvertSession;
    AutoPtr<IUri> dataUri;
    String path(NULL);

    // try {
        AutoPtr<ArrayOf<Byte> > data;
        part->GetData((ArrayOf<Byte>**)&data);
        if (IContentType::TEXT_PLAIN.Equals(contentType)
                || IContentType::APP_SMIL.Equals(contentType)
                || IContentType::TEXT_HTML.Equals(contentType)) {
            AutoPtr<IContentValues> cv;
            CContentValues::New((IContentValues**)&cv);
            if (data == NULL) {
                data = String("").GetBytes(); // ICharacterSets::DEFAULT_CHARSET_NAME);
            }
            AutoPtr<IEncodedStringValue> p;
            CEncodedStringValue::New(data, (IEncodedStringValue**)&p);
            String str;
            p->GetString(&str);
            cv->Put(ITelephonyMmsPart::TEXT, str);
            Int32 resUpdate = 0;
            mContentResolver->Update(uri, cv, String(NULL), NULL, &resUpdate);
            if (resUpdate != 1) {
                // throw new MmsException("unable to update " + uri.toString());
                return;
            }
        }
        else {
            AutoPtr<IDownloadDrmHelper> ddhlp;
            CDownloadDrmHelper::AcquireSingleton((IDownloadDrmHelper**)&ddhlp);
            Boolean isDrm = FALSE;
            ddhlp->IsDrmConvertNeeded(contentType, &isDrm);
            if (isDrm) {
                if (uri != NULL) {
                    // try {
                        path = ConvertUriToPath(mContext, uri);
                        if (LOCAL_LOGV) {
                            Logger::V(TAG, String("drm uri: ") + TO_CSTR(uri) + String(" path: ") + path);
                        }
                        AutoPtr<IFile> f;
                        CFile::New(path, (IFile**)&f);
                        Int64 len = 0;
                        f->GetLength(&len);
                        if (LOCAL_LOGV) {
                            Logger::V(TAG, String("drm path: ") + path + String(" len: ") + StringUtils::ToString(len));
                        }
                        if (len > 0) {
                            // we're not going to re-persist and re-encrypt an already
                            // converted drm file
                            return;
                        }
                    // } catch (Exception e) {
                    //     Log.e(TAG, "Can't get file info for: " + part.getDataUri(), e);
                    // }
                }
                // We haven't converted the file yet, start the conversion
                AutoPtr<IDrmConvertSessionHelper> dshlp;
                CDrmConvertSessionHelper::AcquireSingleton((IDrmConvertSessionHelper**)&dshlp);
                dshlp->Open(mContext, contentType, (IDrmConvertSession**)&drmConvertSession);
                if (drmConvertSession == NULL) {
                    // throw new MmsException("Mimetype " + contentType +
                    //         " can not be converted.");
                    return;
                }
            }
            // uri can look like:
            // content://mms/part/98
            mContentResolver->OpenOutputStream(uri, (IOutputStream**)&os);
            if (data == NULL) {
                part->GetDataUri((IUri**)&dataUri);
                if ((dataUri == NULL) || (dataUri.Get() == uri)) {
                    Logger::W(TAG, String("Can't find data for this part."));
                    return;
                }
                // dataUri can look like:
                // content://com.google.android.gallery3d.provider/picasa/item/5720646660183715586
                Boolean bCK = FALSE;
                if (preOpenedFiles != NULL && (preOpenedFiles->ContainsKey(dataUri, &bCK), bCK)) {
                    AutoPtr<IInterface> p;
                    preOpenedFiles->Get(dataUri, (IInterface**)&p);
                    is = IInputStream::Probe(p);
                }
                if (is == NULL) {
                    mContentResolver->OpenInputStream(dataUri, (IInputStream**)&is);
                }

                if (LOCAL_LOGV) {
                    Logger::V(TAG, String("Saving data to: ") + TO_CSTR(uri));
                }

                AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(8192);
                for (Int32 len = 0; (is->Read(buffer, &len), len) != -1; ) {
                    if (!isDrm) {
                        os->Write(buffer, 0, len);
                    }
                    else {
                        AutoPtr<ArrayOf<Byte> > convertedData;
                        drmConvertSession->Convert(buffer, len, (ArrayOf<Byte>**)&convertedData);
                        if (convertedData != NULL) {
                            os->Write(convertedData, 0, convertedData->GetLength());
                        }
                        else {
                            // throw new MmsException("Error converting drm data.");
                            return;
                        }
                    }
                }
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::V(TAG, String("Saving data to: ") + TO_CSTR(uri));
                }
                if (!isDrm) {
                    os->Write(data);
                }
                else {
                    dataUri = uri;
                    AutoPtr<ArrayOf<Byte> > convertedData;
                    drmConvertSession->Convert(data, data->GetLength(), (ArrayOf<Byte>**)&convertedData);
                    if (convertedData != NULL) {
                        os->Write(convertedData, 0, convertedData->GetLength());
                    }
                    else {
                        // throw new MmsException("Error converting drm data.");
                        return;
                    }
                }
            }
        }
    // } catch (FileNotFoundException e) {
    //     Log.e(TAG, "Failed to open Input/Output stream.", e);
    //     throw new MmsException(e);
    // } catch (IOException e) {
    //     Log.e(TAG, "Failed to read/write data.", e);
    //     throw new MmsException(e);
    // } finally {
        if (os != NULL) {
            // try {
                ICloseable::Probe(os)->Close();
            // } catch (IOException e) {
            //     Log.e(TAG, "IOException while closing: " + os, e);
            // } // Ignore
        }
        if (is != NULL) {
            // try {
                ICloseable::Probe(is)->Close();
            // } catch (IOException e) {
            //     Log.e(TAG, "IOException while closing: " + is, e);
            // } // Ignore
        }
        if (drmConvertSession != NULL) {
            Int32 resClose = 0;
            drmConvertSession->Close(path, &resClose);

            // Reset the permissions on the encrypted part file so everyone has only read
            // permission.
            AutoPtr<IFile> f;
            CFile::New(path, (IFile**)&f);
            AutoPtr<IContentValues> values;
            CContentValues::New(0, (IContentValues**)&values);
            AutoPtr<ISqliteWrapper> sw;
            CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
            AutoPtr<IUriHelper> urihlp;
            CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
            String name;
            f->GetName(&name);
            AutoPtr<IUri> uri;
            urihlp->Parse(String("content://mms/resetFilePerm/") + name, (IUri**)&uri);
            Int32 resUpdate = 0;
            sw->Update(mContext, mContentResolver,
                                uri,
                                values, String(NULL), NULL, &resUpdate);
        }
    // }
}

void PduPersister::UpdateAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<IEncodedStringValue*>* array)
{
    // Delete old address information and then insert new ones.
    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->Parse(String("content://mms/") + StringUtils::ToString(msgId) + String("/addr"), (IUri**)&uri);
    Int32 resDel = 0;
    sw->Delete(mContext, mContentResolver,
            uri,
            ITelephonyMmsAddr::TYPE + String("=") + StringUtils::ToString(type), NULL, &resDel);

    PersistAddress(msgId, type, array);
}

void PduPersister::UpdatePart(
    /* [in] */ IUri* uri,
    /* [in] */ IPduPart* part,
    /* [in] */ IHashMap* preOpenedFiles)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(7, (IContentValues**)&values);

    Int32 charset = 0;
    part->GetCharset(&charset);
    if (charset != 0 ) {
        values->Put(ITelephonyMmsPart::CHARSET, charset);
    }

    String contentType(NULL);
    AutoPtr<ArrayOf<Byte> > type;
    part->GetContentType((ArrayOf<Byte>**)&type);
    if (type != NULL) {
        contentType = ToIsoString(type);
        values->Put(ITelephonyMmsPart::CONTENT_TYPE, contentType);
    }
    else {
        // throw new MmsException("MIME type of the part must be set.");
        return;
    }

    AutoPtr<ArrayOf<Byte> > fileName;
    part->GetFilename((ArrayOf<Byte>**)&fileName);
    if (fileName != NULL) {
        String fileNameStr(*fileName);
        values->Put(ITelephonyMmsPart::FILENAME, fileNameStr);
    }

    AutoPtr<ArrayOf<Byte> > name;
    part->GetName((ArrayOf<Byte>**)&name);
    if (name != NULL) {
        String nameStr(*name);
        values->Put(ITelephonyMmsPart::NAME, nameStr);
    }

    String value;
    AutoPtr<ArrayOf<Byte> > pos;
    part->GetContentDisposition((ArrayOf<Byte>**)&pos);
    if (pos != NULL) {
        value = ToIsoString(pos);
        values->Put(ITelephonyMmsPart::CONTENT_DISPOSITION, value);
    }

    AutoPtr<ArrayOf<Byte> > contentId;
    part->GetContentId((ArrayOf<Byte>**)&contentId);
    if (contentId != NULL) {
        value = ToIsoString(contentId);
        values->Put(ITelephonyMmsPart::CONTENT_ID, value);
    }

    AutoPtr<ArrayOf<Byte> > contentLoc;
    part->GetContentLocation((ArrayOf<Byte>**)&contentLoc);
    if (contentLoc != NULL) {
        value = ToIsoString(contentLoc);
        values->Put(ITelephonyMmsPart::CONTENT_LOCATION, value);
    }

    AutoPtr<ISqliteWrapper> sw;
    CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    Int32 resUpdate = 0;
    sw->Update(mContext, mContentResolver, uri, values, String(NULL), NULL, &resUpdate);

    // Only update the data when:
    // 1. New binary data supplied or
    // 2. The Uri of the part is different from the current one.
    AutoPtr<ArrayOf<Byte> > data;
    part->GetData((ArrayOf<Byte>**)&data);
    AutoPtr<IUri> dataUri;
    part->GetDataUri((IUri**)&dataUri);
    if ((data != NULL)
            || (uri != dataUri)) {
        PersistData(part, uri, contentType, preOpenedFiles);
    }
}

void PduPersister::LoadRecipients(
    /* [in] */ Int32 addressType,
    /* [in] */ IHashSet* recipients,
    /* [in] */ IHashMap* addressMap,
    /* [in] */ Boolean excludeMyNumber)
{
    AutoPtr<IInterface> pArr;
    addressMap->Get(CoreUtils::Convert(addressType), (IInterface**)&pArr);
    AutoPtr<IArrayOf> pArray = IArrayOf::Probe(pArr);
    Int32 len = 0;
    pArray->GetLength(&len);
    AutoPtr<ArrayOf<IEncodedStringValue*> > array = ArrayOf<IEncodedStringValue*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        pArray->Get(i, (IInterface**)&obj);
        (*array)[i] = IEncodedStringValue::Probe(obj);
    }
    if (array == NULL) {
        return;
    }
    // If the TO recipients is only a single address, then we can skip loadRecipients when
    // we're excluding our own number because we know that address is our own.
    if (excludeMyNumber && array->GetLength() == 1) {
        return;
    }
    String myNumber(NULL);
    if (excludeMyNumber) {
        mTelephonyManager->GetLine1Number(&myNumber);
    }
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IEncodedStringValue> v = (*array)[i];
        if (v != NULL) {
            String number;
            v->GetString(&number);
            AutoPtr<IPhoneNumberUtils> pu;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
            Boolean bCompare = FALSE, bContains = FALSE;;
            if ((myNumber == NULL || !(pu->Compare(number, myNumber, &bCompare), bCompare)) &&
                    !(recipients->Contains(CoreUtils::Convert(number), &bContains), bContains)) {
                // Only add numbers which aren't my own number.
                recipients->Add(CoreUtils::Convert(number));
            }
        }
    }
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
