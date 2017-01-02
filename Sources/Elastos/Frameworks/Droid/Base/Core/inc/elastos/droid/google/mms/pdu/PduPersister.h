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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPERSISTER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPERSISTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Google::Mms::Utility::IPduCache;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * This class is the high-level manager of PDU storage.
  */
class PduPersister
    : public Object
    , public IPduPersister
{
public:
    CAR_INTERFACE_DECL();

    /** Get(or create if not exist) an instance of PduPersister */
    static CARAPI_(AutoPtr<IPduPersister>) GetPduPersister(
        /* [in] */ IContext* context);

    /**
      * Load a PDU from storage by given Uri.
      *
      * @param uri The Uri of the PDU to be loaded.
      * @return A generic PDU object, it may be cast to dedicated PDU.
      * @throws MmsException Failed to load some fields of a PDU.
      */
    virtual CARAPI Load(
        /* [in] */ IUri* uri,
        /* [out] */ IGenericPdu** result);

    virtual CARAPI PersistPart(
        /* [in] */ IPduPart* part,
        /* [in] */ Int64 msgId,
        /* [in] */ IHashMap* preOpenedFiles, // IUri, IInputStream
        /* [out] */ IUri** result);

    /**
      * This method expects uri in the following format
      *     content://media/<table_name>/<row_index> (or)
      *     file://sdcard/test.mp4
      *     http://test.com/test.mp4
      *
      * Here <table_name> shall be "video" or "audio" or "images"
      * <row_index> the index of the content in given table
      */
    static CARAPI_(String) ConvertUriToPath(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    /**
      * Update headers of a SendReq.
      *
      * @param uri The PDU which need to be updated.
      * @param pdu New headers.
      * @throws MmsException Bad URI or updating failed.
      */
    virtual CARAPI UpdateHeaders(
        /* [in] */ IUri* uri,
        /* [in] */ ISendReq* sendReq);

    /**
      * Update all parts of a PDU.
      *
      * @param uri The PDU which need to be updated.
      * @param body New message body of the PDU.
      * @param preOpenedFiles if not null, a map of preopened InputStreams for the parts.
      * @throws MmsException Bad URI or updating failed.
      */
    virtual CARAPI UpdateParts(
        /* [in] */ IUri* uri,
        /* [in] */ IPduBody* body,
        /* [in] */ IHashMap* preOpenedFiles); // IUri, IInputStream

    /**
      * Persist a PDU object to specific location in the storage.
      *
      * @param pdu The PDU object to be stored.
      * @param uri Where to store the given PDU object.
      * @param createThreadId if true, this function may create a thread id for the recipients
      * @param groupMmsEnabled if true, all of the recipients addressed in the PDU will be used
      *  to create the associated thread. When false, only the sender will be used in finding or
      *  creating the appropriate thread or conversation.
      * @param preOpenedFiles if not null, a map of preopened InputStreams for the parts.
      * @return A Uri which can be used to access the stored PDU.
      */
    virtual CARAPI Persist(
        /* [in] */ IGenericPdu* pdu,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean createThreadId,
        /* [in] */ Boolean groupMmsEnabled,
        /* [in] */ IHashMap* preOpenedFiles, // IUri, IInputStream
        /* [out] */ IUri** result);

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
    virtual CARAPI Move(
        /* [in] */ IUri* from,
        /* [in] */ IUri* to,
        /* [out] */ IUri** result);

    /**
      * Wrap a byte[] into a String.
      */
    static CARAPI_(String) ToIsoString(
        /* [in] */ ArrayOf<Byte>* bytes);

    /**
      * Unpack a given String into a byte[].
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetBytes(
        /* [in] */ const String& data);

    /**
      * Remove all objects in the temporary path.
      */
    virtual CARAPI ReleaseResources();

    /**
      * Find all messages to be sent or downloaded before certain time.
      */
    virtual CARAPI GetPendingMessages(
        /* [in] */ Int64 dueTime,
        /* [out] */ ICursor** result);

private:
    PduPersister(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) MiddleInitAddressFields();

    static CARAPI_(AutoPtr<ArrayOf<String> >) MiddleInitPduProjection();

    static CARAPI_(AutoPtr<ArrayOf<String> >) MiddleInitPartProjection();

    CARAPI_(void) SetEncodedStringValueToHeaders(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex,
        /* [in] */ IPduHeaders* headers,
        /* [in] */ Int32 mapColumn);

    CARAPI_(void) SetTextStringToHeaders(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex,
        /* [in] */ IPduHeaders* headers,
        /* [in] */ Int32 mapColumn);

    CARAPI_(void) SetOctetToHeaders(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex,
        /* [in] */ IPduHeaders* headers,
        /* [in] */ Int32 mapColumn);

    CARAPI_(void) SetLongToHeaders(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex,
        /* [in] */ IPduHeaders* headers,
        /* [in] */ Int32 mapColumn);

    CARAPI_(AutoPtr<IInteger32>) GetIntegerFromPartColumn(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetByteArrayFromPartColumn(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 columnIndex);

    CARAPI_(AutoPtr<ArrayOf<IPduPart*> >) LoadParts(
        /* [in] */ Int64 msgId);

    CARAPI_(void) LoadAddress(
        /* [in] */ Int64 msgId,
        /* [in] */ IPduHeaders* headers);

    CARAPI_(void) PersistAddress(
        /* [in] */ Int64 msgId,
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<IEncodedStringValue*>* array);

    static CARAPI_(String) GetPartContentType(
        /* [in] */ IPduPart* part);

    /**
      * Save data of the part into storage. The source data may be given
      * by a byte[] or a Uri. If it's a byte[], directly save it
      * into storage, otherwise load source data from the dataUri and then
      * save it. If the data is an image, we may scale down it according
      * to user preference.
      *
      * @param part The PDU part which contains data to be saved.
      * @param uri The URI of the part.
      * @param contentType The MIME type of the part.
      * @param preOpenedFiles if not null, a map of preopened InputStreams for the parts.
      * @throws MmsException Cannot find source data or error occurred
      *         while saving the data.
      */
    CARAPI_(void) PersistData(
        /* [in] */ IPduPart* part,
        /* [in] */ IUri* uri,
        /* [in] */ const String& contentType,
        /* [in] */ IHashMap* preOpenedFiles); // IUri, IInputStream

    CARAPI_(void) UpdateAddress(
        /* [in] */ Int64 msgId,
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<IEncodedStringValue*>* array);

    CARAPI_(void) UpdatePart(
        /* [in] */ IUri* uri,
        /* [in] */ IPduPart* part,
        /* [in] */ IHashMap* preOpenedFiles); // IUri, IInputStream

    /**
      * For a given address type, extract the recipients from the headers.
      *
      * @param addressType can be PduHeaders.FROM, PduHeaders.TO or PduHeaders.CC
      * @param recipients a HashSet that is loaded with the recipients from the FROM, TO or CC headers
      * @param addressMap a HashMap of the addresses from the ADDRESS_FIELDS header
      * @param excludeMyNumber if true, the number of this phone will be excluded from recipients
      */
    CARAPI_(void) LoadRecipients(
        /* [in] */ Int32 addressType,
        /* [in] */ IHashSet* recipients, // String
        /* [in] */ IHashMap* addressMap, // Integer*, ArrayOf<IEncodedStringValue>
        /* [in] */ Boolean excludeMyNumber);

public:
    /**
      * The uri of temporary drm objects.
      */
    static const String TEMPORARY_DRM_OBJECT_URI;
    /**
      * Indicate that we transiently failed to process a MM.
      */
    static const Int32 PROC_STATUS_TRANSIENT_FAILURE = 1;
    /**
      * Indicate that we permanently failed to process a MM.
      */
    static const Int32 PROC_STATUS_PERMANENTLY_FAILURE = 2;
    /**
      * Indicate that we have successfully processed a MM.
      */
    static const Int32 PROC_STATUS_COMPLETED = 3;

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean LOCAL_LOGV;
    static const Int64 DUMMY_THREAD_ID = 0x7FFFFFFFFFFFFFFFLL;//TODO Elastos::Core::Math::INT64_MAX_VALUE;
    static AutoPtr<IPduPersister> sPersister;
    static const AutoPtr<IPduCache> PDU_CACHE_INSTANCE;
    Object cache_lock;
    static AutoPtr<ArrayOf<Int32> > ADDRESS_FIELDS;
    static AutoPtr<ArrayOf<String> > PDU_PROJECTION;
    static const Int32 PDU_COLUMN_ID = 0;
    static const Int32 PDU_COLUMN_MESSAGE_BOX = 1;
    static const Int32 PDU_COLUMN_THREAD_ID = 2;
    static const Int32 PDU_COLUMN_RETRIEVE_TEXT = 3;
    static const Int32 PDU_COLUMN_SUBJECT = 4;
    static const Int32 PDU_COLUMN_CONTENT_LOCATION = 5;
    static const Int32 PDU_COLUMN_CONTENT_TYPE = 6;
    static const Int32 PDU_COLUMN_MESSAGE_CLASS = 7;
    static const Int32 PDU_COLUMN_MESSAGE_ID = 8;
    static const Int32 PDU_COLUMN_RESPONSE_TEXT = 9;
    static const Int32 PDU_COLUMN_TRANSACTION_ID = 10;
    static const Int32 PDU_COLUMN_CONTENT_CLASS = 11;
    static const Int32 PDU_COLUMN_DELIVERY_REPORT = 12;
    static const Int32 PDU_COLUMN_MESSAGE_TYPE = 13;
    static const Int32 PDU_COLUMN_MMS_VERSION = 14;
    static const Int32 PDU_COLUMN_PRIORITY = 15;
    static const Int32 PDU_COLUMN_READ_REPORT = 16;
    static const Int32 PDU_COLUMN_READ_STATUS = 17;
    static const Int32 PDU_COLUMN_REPORT_ALLOWED = 18;
    static const Int32 PDU_COLUMN_RETRIEVE_STATUS = 19;
    static const Int32 PDU_COLUMN_STATUS = 20;
    static const Int32 PDU_COLUMN_DATE = 21;
    static const Int32 PDU_COLUMN_DELIVERY_TIME = 22;
    static const Int32 PDU_COLUMN_EXPIRY = 23;
    static const Int32 PDU_COLUMN_MESSAGE_SIZE = 24;
    static const Int32 PDU_COLUMN_SUBJECT_CHARSET = 25;
    static const Int32 PDU_COLUMN_RETRIEVE_TEXT_CHARSET = 26;
    static AutoPtr<ArrayOf<String> > PART_PROJECTION;
    static const Int32 PART_COLUMN_ID = 0;
    static const Int32 PART_COLUMN_CHARSET = 1;
    static const Int32 PART_COLUMN_CONTENT_DISPOSITION = 2;
    static const Int32 PART_COLUMN_CONTENT_ID = 3;
    static const Int32 PART_COLUMN_CONTENT_LOCATION = 4;
    static const Int32 PART_COLUMN_CONTENT_TYPE = 5;
    static const Int32 PART_COLUMN_FILENAME = 6;
    static const Int32 PART_COLUMN_NAME = 7;
    static const Int32 PART_COLUMN_TEXT = 8;
    static AutoPtr<IHashMap> MESSAGE_BOX_MAP; // Uri, Integer
    // These map are used for convenience in persist() and load().
    static AutoPtr<IHashMap> CHARSET_COLUMN_INDEX_MAP; // Integer, Integer
    static AutoPtr<IHashMap> ENCODED_STRING_COLUMN_INDEX_MAP; // Integer, Integer
    static AutoPtr<IHashMap> TEXT_STRING_COLUMN_INDEX_MAP; // Integer, Integer
    static AutoPtr<IHashMap> OCTET_COLUMN_INDEX_MAP; // Integer, Integer
    static AutoPtr<IHashMap> LONG_COLUMN_INDEX_MAP; // Integer, Integer
    static AutoPtr<IHashMap> CHARSET_COLUMN_NAME_MAP; // Integer, String
    static AutoPtr<IHashMap> ENCODED_STRING_COLUMN_NAME_MAP; // Integer, String
    static AutoPtr<IHashMap> TEXT_STRING_COLUMN_NAME_MAP; // Integer, String
    static AutoPtr<IHashMap> OCTET_COLUMN_NAME_MAP; // Integer, String
    static AutoPtr<IHashMap> LONG_COLUMN_NAME_MAP; // Integer, String
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr<IContentResolver> mContentResolver;
    /*const*/ AutoPtr<IInterface/*TODO IDrmManagerClient*/> mDrmManagerClient;
    /*const*/ AutoPtr<ITelephonyManager> mTelephonyManager;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPERSISTER_H__
