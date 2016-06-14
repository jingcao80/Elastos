#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduPersister.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::StringUtils;

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
AutoPtr< ArrayOf<Int32> > PduPersister::ADDRESS_FIELDS = PduPersister::MiddleInitAddressFields();
AutoPtr< ArrayOf<String> > PduPersister::PDU_PROJECTION = PduPersister::MiddleInitPduProjection();
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
AutoPtr< ArrayOf<String> > PduPersister::PART_PROJECTION = PduPersister::MiddleInitPartProjection();
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
    // ==================before translated======================
    // if ((sPersister == null)) {
    //     sPersister = new PduPersister(context);
    // } else if (!context.equals(sPersister.mContext)) {
    //     sPersister.release();
    //     sPersister = new PduPersister(context);
    // }
    //
    // return sPersister;
    assert(0);
    AutoPtr<IPduPersister> empty;
    return empty;
}

ECode PduPersister::Load(
    /* [in] */ IUri* uri,
    /* [out] */ IGenericPdu** result)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // GenericPdu pdu = null;
    // PduCacheEntry cacheEntry = null;
    // int msgBox = 0;
    // long threadId = -1;
    // try {
    //     synchronized(PDU_CACHE_INSTANCE) {
    //         if (PDU_CACHE_INSTANCE.isUpdating(uri)) {
    //             if (LOCAL_LOGV) {
    //                 Log.v(TAG, "load: " + uri + " blocked by isUpdating()");
    //             }
    //             try {
    //                 PDU_CACHE_INSTANCE.wait();
    //             } catch (InterruptedException e) {
    //                 Log.e(TAG, "load: ", e);
    //             }
    //             cacheEntry = PDU_CACHE_INSTANCE.get(uri);
    //             if (cacheEntry != null) {
    //                 return cacheEntry.getPdu();
    //             }
    //         }
    //         // Tell the cache to indicate to other callers that this item
    //         // is currently being updated.
    //         PDU_CACHE_INSTANCE.setUpdating(uri, true);
    //     }
    //
    //     Cursor c = SqliteWrapper.query(mContext, mContentResolver, uri,
    //             PDU_PROJECTION, null, null, null);
    //     PduHeaders headers = new PduHeaders();
    //     Set<Entry<Integer, Integer>> set;
    //     long msgId = ContentUris.parseId(uri);
    //
    //     try {
    //         if ((c == null) || (c.getCount() != 1) || !c.moveToFirst()) {
    //             throw new MmsException("Bad uri: " + uri);
    //         }
    //
    //         msgBox = c.getInt(PDU_COLUMN_MESSAGE_BOX);
    //         threadId = c.getLong(PDU_COLUMN_THREAD_ID);
    //
    //         set = ENCODED_STRING_COLUMN_INDEX_MAP.entrySet();
    //         for (Entry<Integer, Integer> e : set) {
    //             setEncodedStringValueToHeaders(
    //                     c, e.getValue(), headers, e.getKey());
    //         }
    //
    //         set = TEXT_STRING_COLUMN_INDEX_MAP.entrySet();
    //         for (Entry<Integer, Integer> e : set) {
    //             setTextStringToHeaders(
    //                     c, e.getValue(), headers, e.getKey());
    //         }
    //
    //         set = OCTET_COLUMN_INDEX_MAP.entrySet();
    //         for (Entry<Integer, Integer> e : set) {
    //             setOctetToHeaders(
    //                     c, e.getValue(), headers, e.getKey());
    //         }
    //
    //         set = LONG_COLUMN_INDEX_MAP.entrySet();
    //         for (Entry<Integer, Integer> e : set) {
    //             setLongToHeaders(
    //                     c, e.getValue(), headers, e.getKey());
    //         }
    //     } finally {
    //         if (c != null) {
    //             c.close();
    //         }
    //     }
    //
    //     // Check whether 'msgId' has been assigned a valid value.
    //     if (msgId == -1L) {
    //         throw new MmsException("Error! ID of the message: -1.");
    //     }
    //
    //     // Load address information of the MM.
    //     loadAddress(msgId, headers);
    //
    //     int msgType = headers.getOctet(PduHeaders.MESSAGE_TYPE);
    //     PduBody body = new PduBody();
    //
    //     // For PDU which type is M_retrieve.conf or Send.req, we should
    //     // load multiparts and put them into the body of the PDU.
    //     if ((msgType == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF)
    //             || (msgType == PduHeaders.MESSAGE_TYPE_SEND_REQ)) {
    //         PduPart[] parts = loadParts(msgId);
    //         if (parts != null) {
    //             int partsNum = parts.length;
    //             for (int i = 0; i < partsNum; i++) {
    //                 body.addPart(parts[i]);
    //             }
    //         }
    //     }
    //
    //     switch (msgType) {
    //     case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
    //         pdu = new NotificationInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_DELIVERY_IND:
    //         pdu = new DeliveryInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_READ_ORIG_IND:
    //         pdu = new ReadOrigInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
    //         pdu = new RetrieveConf(headers, body);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_SEND_REQ:
    //         pdu = new SendReq(headers, body);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
    //         pdu = new AcknowledgeInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
    //         pdu = new NotifyRespInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
    //         pdu = new ReadRecInd(headers);
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_SEND_CONF:
    //     case PduHeaders.MESSAGE_TYPE_FORWARD_REQ:
    //     case PduHeaders.MESSAGE_TYPE_FORWARD_CONF:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_STORE_REQ:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_STORE_CONF:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_REQ:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_CONF:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_REQ:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_CONF:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_REQ:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_CONF:
    //     case PduHeaders.MESSAGE_TYPE_MBOX_DESCR:
    //     case PduHeaders.MESSAGE_TYPE_DELETE_REQ:
    //     case PduHeaders.MESSAGE_TYPE_DELETE_CONF:
    //     case PduHeaders.MESSAGE_TYPE_CANCEL_REQ:
    //     case PduHeaders.MESSAGE_TYPE_CANCEL_CONF:
    //         throw new MmsException(
    //                 "Unsupported PDU type: " + Integer.toHexString(msgType));
    //
    //     default:
    //         throw new MmsException(
    //                 "Unrecognized PDU type: " + Integer.toHexString(msgType));
    //     }
    // } finally {
    //     synchronized(PDU_CACHE_INSTANCE) {
    //         if (pdu != null) {
    //             assert(PDU_CACHE_INSTANCE.get(uri) == null);
    //             // Update the cache entry with the real info
    //             cacheEntry = new PduCacheEntry(pdu, msgBox, threadId);
    //             PDU_CACHE_INSTANCE.put(uri, cacheEntry);
    //         }
    //         PDU_CACHE_INSTANCE.setUpdating(uri, false);
    //         PDU_CACHE_INSTANCE.notifyAll(); // tell anybody waiting on this entry to go ahead
    //     }
    // }
    // return pdu;
    assert(0);
    return NOERROR;
}

ECode PduPersister::PersistPart(
    /* [in] */ IPduPart* part,
    /* [in] */ Int64 msgId,
    /* [in] */ IHashMap* preOpenedFiles,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Uri uri = Uri.parse("content://mms/" + msgId + "/part");
    // ContentValues values = new ContentValues(8);
    //
    // int charset = part.getCharset();
    // if (charset != 0 ) {
    //     values.put(Part.CHARSET, charset);
    // }
    //
    // String contentType = getPartContentType(part);
    // if (contentType != null) {
    //     // There is no "image/jpg" in Android (and it's an invalid mimetype).
    //     // Change it to "image/jpeg"
    //     if (ContentType.IMAGE_JPG.equals(contentType)) {
    //         contentType = ContentType.IMAGE_JPEG;
    //     }
    //
    //     values.put(Part.CONTENT_TYPE, contentType);
    //     // To ensure the SMIL part is always the first part.
    //     if (ContentType.APP_SMIL.equals(contentType)) {
    //         values.put(Part.SEQ, -1);
    //     }
    // } else {
    //     throw new MmsException("MIME type of the part must be set.");
    // }
    //
    // if (part.getFilename() != null) {
    //     String fileName = new String(part.getFilename());
    //     values.put(Part.FILENAME, fileName);
    // }
    //
    // if (part.getName() != null) {
    //     String name = new String(part.getName());
    //     values.put(Part.NAME, name);
    // }
    //
    // Object value = null;
    // if (part.getContentDisposition() != null) {
    //     value = toIsoString(part.getContentDisposition());
    //     values.put(Part.CONTENT_DISPOSITION, (String) value);
    // }
    //
    // if (part.getContentId() != null) {
    //     value = toIsoString(part.getContentId());
    //     values.put(Part.CONTENT_ID, (String) value);
    // }
    //
    // if (part.getContentLocation() != null) {
    //     value = toIsoString(part.getContentLocation());
    //     values.put(Part.CONTENT_LOCATION, (String) value);
    // }
    //
    // Uri res = SqliteWrapper.insert(mContext, mContentResolver, uri, values);
    // if (res == null) {
    //     throw new MmsException("Failed to persist part, return null.");
    // }
    //
    // persistData(part, res, contentType, preOpenedFiles);
    // // After successfully store the data, we should update
    // // the dataUri of the part.
    // part.setDataUri(res);
    //
    // return res;
    assert(0);
    return NOERROR;
}

String PduPersister::ConvertUriToPath(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    // ==================before translated======================
    // String path = null;
    // if (null != uri) {
    //     String scheme = uri.getScheme();
    //     if (null == scheme || scheme.equals("") ||
    //             scheme.equals(ContentResolver.SCHEME_FILE)) {
    //         path = uri.getPath();
    //
    //     } else if (scheme.equals("http")) {
    //         path = uri.toString();
    //
    //     } else if (scheme.equals(ContentResolver.SCHEME_CONTENT)) {
    //         String[] projection = new String[] {MediaStore.MediaColumns.DATA};
    //         Cursor cursor = null;
    //         try {
    //             cursor = context.getContentResolver().query(uri, projection, null,
    //                     null, null);
    //             if (null == cursor || 0 == cursor.getCount() || !cursor.moveToFirst()) {
    //                 throw new IllegalArgumentException("Given Uri could not be found" +
    //                         " in media store");
    //             }
    //             int pathIndex = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DATA);
    //             path = cursor.getString(pathIndex);
    //         } catch (SQLiteException e) {
    //             throw new IllegalArgumentException("Given Uri is not formatted in a way " +
    //                     "so that it can be found in media store.");
    //         } finally {
    //             if (null != cursor) {
    //                 cursor.close();
    //             }
    //         }
    //     } else {
    //         throw new IllegalArgumentException("Given Uri scheme is not supported");
    //     }
    // }
    // return path;
    assert(0);
    return String("");
}

ECode PduPersister::UpdateHeaders(
    /* [in] */ IUri* uri,
    /* [in] */ ISendReq* sendReq)
{
    VALIDATE_NOT_NULL(sendReq);
    // ==================before translated======================
    // synchronized(PDU_CACHE_INSTANCE) {
    //     // If the cache item is getting updated, wait until it's done updating before
    //     // purging it.
    //     if (PDU_CACHE_INSTANCE.isUpdating(uri)) {
    //         if (LOCAL_LOGV) {
    //             Log.v(TAG, "updateHeaders: " + uri + " blocked by isUpdating()");
    //         }
    //         try {
    //             PDU_CACHE_INSTANCE.wait();
    //         } catch (InterruptedException e) {
    //             Log.e(TAG, "updateHeaders: ", e);
    //         }
    //     }
    // }
    // PDU_CACHE_INSTANCE.purge(uri);
    //
    // ContentValues values = new ContentValues(10);
    // byte[] contentType = sendReq.getContentType();
    // if (contentType != null) {
    //     values.put(Mms.CONTENT_TYPE, toIsoString(contentType));
    // }
    //
    // long date = sendReq.getDate();
    // if (date != -1) {
    //     values.put(Mms.DATE, date);
    // }
    //
    // int deliveryReport = sendReq.getDeliveryReport();
    // if (deliveryReport != 0) {
    //     values.put(Mms.DELIVERY_REPORT, deliveryReport);
    // }
    //
    // long expiry = sendReq.getExpiry();
    // if (expiry != -1) {
    //     values.put(Mms.EXPIRY, expiry);
    // }
    //
    // byte[] msgClass = sendReq.getMessageClass();
    // if (msgClass != null) {
    //     values.put(Mms.MESSAGE_CLASS, toIsoString(msgClass));
    // }
    //
    // int priority = sendReq.getPriority();
    // if (priority != 0) {
    //     values.put(Mms.PRIORITY, priority);
    // }
    //
    // int readReport = sendReq.getReadReport();
    // if (readReport != 0) {
    //     values.put(Mms.READ_REPORT, readReport);
    // }
    //
    // byte[] transId = sendReq.getTransactionId();
    // if (transId != null) {
    //     values.put(Mms.TRANSACTION_ID, toIsoString(transId));
    // }
    //
    // EncodedStringValue subject = sendReq.getSubject();
    // if (subject != null) {
    //     values.put(Mms.SUBJECT, toIsoString(subject.getTextString()));
    //     values.put(Mms.SUBJECT_CHARSET, subject.getCharacterSet());
    // } else {
    //     values.put(Mms.SUBJECT, "");
    // }
    //
    // long messageSize = sendReq.getMessageSize();
    // if (messageSize > 0) {
    //     values.put(Mms.MESSAGE_SIZE, messageSize);
    // }
    //
    // PduHeaders headers = sendReq.getPduHeaders();
    // HashSet<String> recipients = new HashSet<String>();
    // for (int addrType : ADDRESS_FIELDS) {
    //     EncodedStringValue[] array = null;
    //     if (addrType == PduHeaders.FROM) {
    //         EncodedStringValue v = headers.getEncodedStringValue(addrType);
    //         if (v != null) {
    //             array = new EncodedStringValue[1];
    //             array[0] = v;
    //         }
    //     } else {
    //         array = headers.getEncodedStringValues(addrType);
    //     }
    //
    //     if (array != null) {
    //         long msgId = ContentUris.parseId(uri);
    //         updateAddress(msgId, addrType, array);
    //         if (addrType == PduHeaders.TO) {
    //             for (EncodedStringValue v : array) {
    //                 if (v != null) {
    //                     recipients.add(v.getString());
    //                 }
    //             }
    //         }
    //     }
    // }
    // if (!recipients.isEmpty()) {
    //     long threadId = Threads.getOrCreateThreadId(mContext, recipients);
    //     values.put(Mms.THREAD_ID, threadId);
    // }
    //
    // SqliteWrapper.update(mContext, mContentResolver, uri, values, null, null);
    assert(0);
    return NOERROR;
}

ECode PduPersister::UpdateParts(
    /* [in] */ IUri* uri,
    /* [in] */ IPduBody* body,
    /* [in] */ IHashMap* preOpenedFiles)
{
    VALIDATE_NOT_NULL(preOpenedFiles);
    // ==================before translated======================
    // try {
    //     PduCacheEntry cacheEntry;
    //     synchronized(PDU_CACHE_INSTANCE) {
    //         if (PDU_CACHE_INSTANCE.isUpdating(uri)) {
    //             if (LOCAL_LOGV) {
    //                 Log.v(TAG, "updateParts: " + uri + " blocked by isUpdating()");
    //             }
    //             try {
    //                 PDU_CACHE_INSTANCE.wait();
    //             } catch (InterruptedException e) {
    //                 Log.e(TAG, "updateParts: ", e);
    //             }
    //             cacheEntry = PDU_CACHE_INSTANCE.get(uri);
    //             if (cacheEntry != null) {
    //                 ((MultimediaMessagePdu) cacheEntry.getPdu()).setBody(body);
    //             }
    //         }
    //         // Tell the cache to indicate to other callers that this item
    //         // is currently being updated.
    //         PDU_CACHE_INSTANCE.setUpdating(uri, true);
    //     }
    //
    //     ArrayList<PduPart> toBeCreated = new ArrayList<PduPart>();
    //     HashMap<Uri, PduPart> toBeUpdated = new HashMap<Uri, PduPart>();
    //
    //     int partsNum = body.getPartsNum();
    //     StringBuilder filter = new StringBuilder().append('(');
    //     for (int i = 0; i < partsNum; i++) {
    //         PduPart part = body.getPart(i);
    //         Uri partUri = part.getDataUri();
    //         if ((partUri == null) || TextUtils.isEmpty(partUri.getAuthority())
    //                 || !partUri.getAuthority().startsWith("mms")) {
    //             toBeCreated.add(part);
    //         } else {
    //             toBeUpdated.put(partUri, part);
    //
    //             // Don't use 'i > 0' to determine whether we should append
    //             // 'AND' since 'i = 0' may be skipped in another branch.
    //             if (filter.length() > 1) {
    //                 filter.append(" AND ");
    //             }
    //
    //             filter.append(Part._ID);
    //             filter.append("!=");
    //             DatabaseUtils.appendEscapedSQLString(filter, partUri.getLastPathSegment());
    //         }
    //     }
    //     filter.append(')');
    //
    //     long msgId = ContentUris.parseId(uri);
    //
    //     // Remove the parts which doesn't exist anymore.
    //     SqliteWrapper.delete(mContext, mContentResolver,
    //             Uri.parse(Mms.CONTENT_URI + "/" + msgId + "/part"),
    //             filter.length() > 2 ? filter.toString() : null, null);
    //
    //     // Create new parts which didn't exist before.
    //     for (PduPart part : toBeCreated) {
    //         persistPart(part, msgId, preOpenedFiles);
    //     }
    //
    //     // Update the modified parts.
    //     for (Map.Entry<Uri, PduPart> e : toBeUpdated.entrySet()) {
    //         updatePart(e.getKey(), e.getValue(), preOpenedFiles);
    //     }
    // } finally {
    //     synchronized(PDU_CACHE_INSTANCE) {
    //         PDU_CACHE_INSTANCE.setUpdating(uri, false);
    //         PDU_CACHE_INSTANCE.notifyAll();
    //     }
    // }
    assert(0);
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
    // ==================before translated======================
    // if (uri == null) {
    //     throw new MmsException("Uri may not be null.");
    // }
    // long msgId = -1;
    // try {
    //     msgId = ContentUris.parseId(uri);
    // } catch (NumberFormatException e) {
    //     // the uri ends with "inbox" or something else like that
    // }
    // boolean existingUri = msgId != -1;
    //
    // if (!existingUri && MESSAGE_BOX_MAP.get(uri) == null) {
    //     throw new MmsException(
    //             "Bad destination, must be one of "
    //             + "content://mms/inbox, content://mms/sent, "
    //             + "content://mms/drafts, content://mms/outbox, "
    //             + "content://mms/temp.");
    // }
    // synchronized(PDU_CACHE_INSTANCE) {
    //     // If the cache item is getting updated, wait until it's done updating before
    //     // purging it.
    //     if (PDU_CACHE_INSTANCE.isUpdating(uri)) {
    //         if (LOCAL_LOGV) {
    //             Log.v(TAG, "persist: " + uri + " blocked by isUpdating()");
    //         }
    //         try {
    //             PDU_CACHE_INSTANCE.wait();
    //         } catch (InterruptedException e) {
    //             Log.e(TAG, "persist1: ", e);
    //         }
    //     }
    // }
    // PDU_CACHE_INSTANCE.purge(uri);
    //
    // PduHeaders header = pdu.getPduHeaders();
    // PduBody body = null;
    // ContentValues values = new ContentValues();
    // Set<Entry<Integer, String>> set;
    //
    // set = ENCODED_STRING_COLUMN_NAME_MAP.entrySet();
    // for (Entry<Integer, String> e : set) {
    //     int field = e.getKey();
    //     EncodedStringValue encodedString = header.getEncodedStringValue(field);
    //     if (encodedString != null) {
    //         String charsetColumn = CHARSET_COLUMN_NAME_MAP.get(field);
    //         values.put(e.getValue(), toIsoString(encodedString.getTextString()));
    //         values.put(charsetColumn, encodedString.getCharacterSet());
    //     }
    // }
    //
    // set = TEXT_STRING_COLUMN_NAME_MAP.entrySet();
    // for (Entry<Integer, String> e : set){
    //     byte[] text = header.getTextString(e.getKey());
    //     if (text != null) {
    //         values.put(e.getValue(), toIsoString(text));
    //     }
    // }
    //
    // set = OCTET_COLUMN_NAME_MAP.entrySet();
    // for (Entry<Integer, String> e : set){
    //     int b = header.getOctet(e.getKey());
    //     if (b != 0) {
    //         values.put(e.getValue(), b);
    //     }
    // }
    //
    // set = LONG_COLUMN_NAME_MAP.entrySet();
    // for (Entry<Integer, String> e : set){
    //     long l = header.getLongInteger(e.getKey());
    //     if (l != -1L) {
    //         values.put(e.getValue(), l);
    //     }
    // }
    //
    // HashMap<Integer, EncodedStringValue[]> addressMap =
    //         new HashMap<Integer, EncodedStringValue[]>(ADDRESS_FIELDS.length);
    // // Save address information.
    // for (int addrType : ADDRESS_FIELDS) {
    //     EncodedStringValue[] array = null;
    //     if (addrType == PduHeaders.FROM) {
    //         EncodedStringValue v = header.getEncodedStringValue(addrType);
    //         if (v != null) {
    //             array = new EncodedStringValue[1];
    //             array[0] = v;
    //         }
    //     } else {
    //         array = header.getEncodedStringValues(addrType);
    //     }
    //     addressMap.put(addrType, array);
    // }
    //
    // HashSet<String> recipients = new HashSet<String>();
    // int msgType = pdu.getMessageType();
    // // Here we only allocate thread ID for M-Notification.ind,
    // // M-Retrieve.conf and M-Send.req.
    // // Some of other PDU types may be allocated a thread ID outside
    // // this scope.
    // if ((msgType == PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND)
    //         || (msgType == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF)
    //         || (msgType == PduHeaders.MESSAGE_TYPE_SEND_REQ)) {
    //     switch (msgType) {
    //         case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
    //         case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
    //             loadRecipients(PduHeaders.FROM, recipients, addressMap, false);
    //
    //             // For received messages when group MMS is enabled, we want to associate this
    //             // message with the thread composed of all the recipients -- all but our own
    //             // number, that is. This includes the person who sent the
    //             // message or the FROM field (above) in addition to the other people the message
    //             // was addressed to or the TO field. Our own number is in that TO field and
    //             // we have to ignore it in loadRecipients.
    //             if (groupMmsEnabled) {
    //                 loadRecipients(PduHeaders.TO, recipients, addressMap, true);
    //
    //                 // Also load any numbers in the CC field to address group messaging
    //                 // compatibility issues with devices that place numbers in this field
    //                 // for group messages.
    //                 loadRecipients(PduHeaders.CC, recipients, addressMap, true);
    //             }
    //             break;
    //         case PduHeaders.MESSAGE_TYPE_SEND_REQ:
    //             loadRecipients(PduHeaders.TO, recipients, addressMap, false);
    //             break;
    //     }
    //     long threadId = 0;
    //     if (createThreadId && !recipients.isEmpty()) {
    //         // Given all the recipients associated with this message, find (or create) the
    //         // correct thread.
    //         threadId = Threads.getOrCreateThreadId(mContext, recipients);
    //     }
    //     values.put(Mms.THREAD_ID, threadId);
    // }
    //
    // // Save parts first to avoid inconsistent message is loaded
    // // while saving the parts.
    // long dummyId = System.currentTimeMillis(); // Dummy ID of the msg.
    //
    // // Figure out if this PDU is a text-only message
    // boolean textOnly = true;
    //
    // // Sum up the total message size
    // int messageSize = 0;
    //
    // // Get body if the PDU is a RetrieveConf or SendReq.
    // if (pdu instanceof MultimediaMessagePdu) {
    //     body = ((MultimediaMessagePdu) pdu).getBody();
    //     // Start saving parts if necessary.
    //     if (body != null) {
    //         int partsNum = body.getPartsNum();
    //         if (partsNum > 2) {
    //             // For a text-only message there will be two parts: 1-the SMIL, 2-the text.
    //             // Down a few lines below we're checking to make sure we've only got SMIL or
    //             // text. We also have to check then we don't have more than two parts.
    //             // Otherwise, a slideshow with two text slides would be marked as textOnly.
    //             textOnly = false;
    //         }
    //         for (int i = 0; i < partsNum; i++) {
    //             PduPart part = body.getPart(i);
    //             messageSize += part.getDataLength();
    //             persistPart(part, dummyId, preOpenedFiles);
    //
    //             // If we've got anything besides text/plain or SMIL part, then we've got
    //             // an mms message with some other type of attachment.
    //             String contentType = getPartContentType(part);
    //             if (contentType != null && !ContentType.APP_SMIL.equals(contentType)
    //                     && !ContentType.TEXT_PLAIN.equals(contentType)) {
    //                 textOnly = false;
    //             }
    //         }
    //     }
    // }
    // // Record whether this mms message is a simple plain text or not. This is a hint for the
    // // UI.
    // values.put(Mms.TEXT_ONLY, textOnly ? 1 : 0);
    // // The message-size might already have been inserted when parsing the
    // // PDU header. If not, then we insert the message size as well.
    // if (values.getAsInteger(Mms.MESSAGE_SIZE) == null) {
    //     values.put(Mms.MESSAGE_SIZE, messageSize);
    // }
    //
    // Uri res = null;
    // if (existingUri) {
    //     res = uri;
    //     SqliteWrapper.update(mContext, mContentResolver, res, values, null, null);
    // } else {
    //     res = SqliteWrapper.insert(mContext, mContentResolver, uri, values);
    //     if (res == null) {
    //         throw new MmsException("persist() failed: return null.");
    //     }
    //     // Get the real ID of the PDU and update all parts which were
    //     // saved with the dummy ID.
    //     msgId = ContentUris.parseId(res);
    // }
    //
    // values = new ContentValues(1);
    // values.put(Part.MSG_ID, msgId);
    // SqliteWrapper.update(mContext, mContentResolver,
    //                      Uri.parse("content://mms/" + dummyId + "/part"),
    //                      values, null, null);
    // // We should return the longest URI of the persisted PDU, for
    // // example, if input URI is "content://mms/inbox" and the _ID of
    // // persisted PDU is '8', we should return "content://mms/inbox/8"
    // // instead of "content://mms/8".
    // // FIXME: Should the MmsProvider be responsible for this???
    // if (!existingUri) {
    //     res = Uri.parse(uri + "/" + msgId);
    // }
    //
    // // Save address information.
    // for (int addrType : ADDRESS_FIELDS) {
    //     EncodedStringValue[] array = addressMap.get(addrType);
    //     if (array != null) {
    //         persistAddress(msgId, addrType, array);
    //     }
    // }
    //
    // return res;
    assert(0);
    return NOERROR;
}

ECode PduPersister::Move(
    /* [in] */ IUri* from,
    /* [in] */ IUri* to,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Check whether the 'msgId' has been assigned a valid value.
    // long msgId = ContentUris.parseId(from);
    // if (msgId == -1L) {
    //     throw new MmsException("Error! ID of the message: -1.");
    // }
    //
    // // Get corresponding int value of destination box.
    // Integer msgBox = MESSAGE_BOX_MAP.get(to);
    // if (msgBox == null) {
    //     throw new MmsException(
    //             "Bad destination, must be one of "
    //             + "content://mms/inbox, content://mms/sent, "
    //             + "content://mms/drafts, content://mms/outbox, "
    //             + "content://mms/temp.");
    // }
    //
    // ContentValues values = new ContentValues(1);
    // values.put(Mms.MESSAGE_BOX, msgBox);
    // SqliteWrapper.update(mContext, mContentResolver, from, values, null, null);
    // return ContentUris.withAppendedId(to, msgId);
    assert(0);
    return NOERROR;
}

String PduPersister::ToIsoString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // ==================before translated======================
    // try {
    //     return new String(bytes, CharacterSets.MIMENAME_ISO_8859_1);
    // } catch (UnsupportedEncodingException e) {
    //     // Impossible to reach here!
    //     Log.e(TAG, "ISO_8859_1 must be supported!", e);
    //     return "";
    // }
    assert(0);
    return String("");
}

AutoPtr< ArrayOf<Byte> > PduPersister::GetBytes(
    /* [in] */ const String& data)
{
    // ==================before translated======================
    // try {
    //     return data.getBytes(CharacterSets.MIMENAME_ISO_8859_1);
    // } catch (UnsupportedEncodingException e) {
    //     // Impossible to reach here!
    //     Log.e(TAG, "ISO_8859_1 must be supported!", e);
    //     return new byte[0];
    // }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

ECode PduPersister::ReleaseResources()
{
    // ==================before translated======================
    // Uri uri = Uri.parse(TEMPORARY_DRM_OBJECT_URI);
    // SqliteWrapper.delete(mContext, mContentResolver, uri, null, null);
    assert(0);
    return NOERROR;
}

ECode PduPersister::GetPendingMessages(
    /* [in] */ Int64 dueTime,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Uri.Builder uriBuilder = PendingMessages.CONTENT_URI.buildUpon();
    // uriBuilder.appendQueryParameter("protocol", "mms");
    //
    // String selection = PendingMessages.ERROR_TYPE + " < ?"
    //         + " AND " + PendingMessages.DUE_TIME + " <= ?";
    //
    // String[] selectionArgs = new String[] {
    //         String.valueOf(MmsSms.ERR_TYPE_GENERIC_PERMANENT),
    //         String.valueOf(dueTime)
    // };
    //
    // return SqliteWrapper.query(mContext, mContentResolver,
    //         uriBuilder.build(), null, selection, selectionArgs,
    //         PendingMessages.DUE_TIME);
    assert(0);
    return NOERROR;
}

PduPersister::PduPersister(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context;
    // mContentResolver = context.getContentResolver();
    // mDrmManagerClient = new DrmManagerClient(context);
    // mTelephonyManager = (TelephonyManager)context
    //         .getSystemService(Context.TELEPHONY_SERVICE);
}

AutoPtr< ArrayOf<Int32> > PduPersister::MiddleInitAddressFields()
{
    // ==================before translated======================
    // int[] result = new int[] {
    //              PduHeaders.BCC,
    //              PduHeaders.CC,
    //              PduHeaders.FROM,
    //              PduHeaders.TO
    //      };
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr< ArrayOf<String> > PduPersister::MiddleInitPduProjection()
{
    // ==================before translated======================
    // String[] result = new String[] {
    //          Mms._ID,
    //          Mms.MESSAGE_BOX,
    //          Mms.THREAD_ID,
    //          Mms.RETRIEVE_TEXT,
    //          Mms.SUBJECT,
    //          Mms.CONTENT_LOCATION,
    //          Mms.CONTENT_TYPE,
    //          Mms.MESSAGE_CLASS,
    //          Mms.MESSAGE_ID,
    //          Mms.RESPONSE_TEXT,
    //          Mms.TRANSACTION_ID,
    //          Mms.CONTENT_CLASS,
    //          Mms.DELIVERY_REPORT,
    //          Mms.MESSAGE_TYPE,
    //          Mms.MMS_VERSION,
    //          Mms.PRIORITY,
    //          Mms.READ_REPORT,
    //          Mms.READ_STATUS,
    //          Mms.REPORT_ALLOWED,
    //          Mms.RETRIEVE_STATUS,
    //          Mms.STATUS,
    //          Mms.DATE,
    //          Mms.DELIVERY_TIME,
    //          Mms.EXPIRY,
    //          Mms.MESSAGE_SIZE,
    //          Mms.SUBJECT_CHARSET,
    //          Mms.RETRIEVE_TEXT_CHARSET,
    //      };
    assert(0);
    AutoPtr< ArrayOf<String> > empty;
    return empty;
}

AutoPtr< ArrayOf<String> > PduPersister::MiddleInitPartProjection()
{
    // ==================before translated======================
    // String[] result = new String[] {
    //          Part._ID,
    //          Part.CHARSET,
    //          Part.CONTENT_DISPOSITION,
    //          Part.CONTENT_ID,
    //          Part.CONTENT_LOCATION,
    //          Part.CONTENT_TYPE,
    //          Part.FILENAME,
    //          Part.NAME,
    //          Part.TEXT
    //      };
    assert(0);
    AutoPtr< ArrayOf<String> > empty;
    return empty;
}

void PduPersister::SetEncodedStringValueToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    // ==================before translated======================
    // String s = c.getString(columnIndex);
    // if ((s != null) && (s.length() > 0)) {
    //     int charsetColumnIndex = CHARSET_COLUMN_INDEX_MAP.get(mapColumn);
    //     int charset = c.getInt(charsetColumnIndex);
    //     EncodedStringValue value = new EncodedStringValue(
    //             charset, getBytes(s));
    //     headers.setEncodedStringValue(value, mapColumn);
    // }
    assert(0);
}

void PduPersister::SetTextStringToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    // ==================before translated======================
    // String s = c.getString(columnIndex);
    // if (s != null) {
    //     headers.setTextString(getBytes(s), mapColumn);
    // }
    assert(0);
}

void PduPersister::SetOctetToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    // ==================before translated======================
    // if (!c.isNull(columnIndex)) {
    //     int b = c.getInt(columnIndex);
    //     headers.setOctet(b, mapColumn);
    // }
    assert(0);
}

void PduPersister::SetLongToHeaders(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [in] */ IPduHeaders* headers,
    /* [in] */ Int32 mapColumn)
{
    // ==================before translated======================
    // if (!c.isNull(columnIndex)) {
    //     long l = c.getLong(columnIndex);
    //     headers.setLongInteger(l, mapColumn);
    // }
    assert(0);
}

AutoPtr<IInteger32> PduPersister::GetIntegerFromPartColumn(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex)
{
    // ==================before translated======================
    // if (!c.isNull(columnIndex)) {
    //     return c.getInt(columnIndex);
    // }
    // return null;
    assert(0);
    AutoPtr<IInteger32> empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > PduPersister::GetByteArrayFromPartColumn(
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex)
{
    // ==================before translated======================
    // if (!c.isNull(columnIndex)) {
    //     return getBytes(c.getString(columnIndex));
    // }
    // return null;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<IPduPart*> > PduPersister::LoadParts(
    /* [in] */ Int64 msgId)
{
    // ==================before translated======================
    // Cursor c = SqliteWrapper.query(mContext, mContentResolver,
    //         Uri.parse("content://mms/" + msgId + "/part"),
    //         PART_PROJECTION, null, null, null);
    //
    // PduPart[] parts = null;
    //
    // try {
    //     if ((c == null) || (c.getCount() == 0)) {
    //         if (LOCAL_LOGV) {
    //             Log.v(TAG, "loadParts(" + msgId + "): no part to load.");
    //         }
    //         return null;
    //     }
    //
    //     int partCount = c.getCount();
    //     int partIdx = 0;
    //     parts = new PduPart[partCount];
    //     while (c.moveToNext()) {
    //         PduPart part = new PduPart();
    //         Integer charset = getIntegerFromPartColumn(
    //                 c, PART_COLUMN_CHARSET);
    //         if (charset != null) {
    //             part.setCharset(charset);
    //         }
    //
    //         byte[] contentDisposition = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_CONTENT_DISPOSITION);
    //         if (contentDisposition != null) {
    //             part.setContentDisposition(contentDisposition);
    //         }
    //
    //         byte[] contentId = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_CONTENT_ID);
    //         if (contentId != null) {
    //             part.setContentId(contentId);
    //         }
    //
    //         byte[] contentLocation = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_CONTENT_LOCATION);
    //         if (contentLocation != null) {
    //             part.setContentLocation(contentLocation);
    //         }
    //
    //         byte[] contentType = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_CONTENT_TYPE);
    //         if (contentType != null) {
    //             part.setContentType(contentType);
    //         } else {
    //             throw new MmsException("Content-Type must be set.");
    //         }
    //
    //         byte[] fileName = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_FILENAME);
    //         if (fileName != null) {
    //             part.setFilename(fileName);
    //         }
    //
    //         byte[] name = getByteArrayFromPartColumn(
    //                 c, PART_COLUMN_NAME);
    //         if (name != null) {
    //             part.setName(name);
    //         }
    //
    //         // Construct a Uri for this part.
    //         long partId = c.getLong(PART_COLUMN_ID);
    //         Uri partURI = Uri.parse("content://mms/part/" + partId);
    //         part.setDataUri(partURI);
    //
    //         // For images/audio/video, we won't keep their data in Part
    //         // because their renderer accept Uri as source.
    //         String type = toIsoString(contentType);
    //         if (!ContentType.isImageType(type)
    //                 && !ContentType.isAudioType(type)
    //                 && !ContentType.isVideoType(type)) {
    //             ByteArrayOutputStream baos = new ByteArrayOutputStream();
    //             InputStream is = null;
    //
    //             // Store simple string values directly in the database instead of an
    //             // external file.  This makes the text searchable and retrieval slightly
    //             // faster.
    //             if (ContentType.TEXT_PLAIN.equals(type) || ContentType.APP_SMIL.equals(type)
    //                     || ContentType.TEXT_HTML.equals(type)) {
    //                 String text = c.getString(PART_COLUMN_TEXT);
    //                 byte [] blob = new EncodedStringValue(text != null ? text : "")
    //                     .getTextString();
    //                 baos.write(blob, 0, blob.length);
    //             } else {
    //
    //                 try {
    //                     is = mContentResolver.openInputStream(partURI);
    //
    //                     byte[] buffer = new byte[256];
    //                     int len = is.read(buffer);
    //                     while (len >= 0) {
    //                         baos.write(buffer, 0, len);
    //                         len = is.read(buffer);
    //                     }
    //                 } catch (IOException e) {
    //                     Log.e(TAG, "Failed to load part data", e);
    //                     c.close();
    //                     throw new MmsException(e);
    //                 } finally {
    //                     if (is != null) {
    //                         try {
    //                             is.close();
    //                         } catch (IOException e) {
    //                             Log.e(TAG, "Failed to close stream", e);
    //                         } // Ignore
    //                     }
    //                 }
    //             }
    //             part.setData(baos.toByteArray());
    //         }
    //         parts[partIdx++] = part;
    //     }
    // } finally {
    //     if (c != null) {
    //         c.close();
    //     }
    // }
    //
    // return parts;
    assert(0);
    AutoPtr< ArrayOf<IPduPart*> > empty;
    return empty;
}

void PduPersister::LoadAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // Cursor c = SqliteWrapper.query(mContext, mContentResolver,
    //         Uri.parse("content://mms/" + msgId + "/addr"),
    //         new String[] { Addr.ADDRESS, Addr.CHARSET, Addr.TYPE },
    //         null, null, null);
    //
    // if (c != null) {
    //     try {
    //         while (c.moveToNext()) {
    //             String addr = c.getString(0);
    //             if (!TextUtils.isEmpty(addr)) {
    //                 int addrType = c.getInt(2);
    //                 switch (addrType) {
    //                     case PduHeaders.FROM:
    //                         headers.setEncodedStringValue(
    //                                 new EncodedStringValue(c.getInt(1), getBytes(addr)),
    //                                 addrType);
    //                         break;
    //                     case PduHeaders.TO:
    //                     case PduHeaders.CC:
    //                     case PduHeaders.BCC:
    //                         headers.appendEncodedStringValue(
    //                                 new EncodedStringValue(c.getInt(1), getBytes(addr)),
    //                                 addrType);
    //                         break;
    //                     default:
    //                         Log.e(TAG, "Unknown address type: " + addrType);
    //                         break;
    //                 }
    //             }
    //         }
    //     } finally {
    //         c.close();
    //     }
    // }
    assert(0);
}

void PduPersister::PersistAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<IEncodedStringValue*>* array)
{
    // ==================before translated======================
    // ContentValues values = new ContentValues(3);
    //
    // for (EncodedStringValue addr : array) {
    //     values.clear(); // Clear all values first.
    //     values.put(Addr.ADDRESS, toIsoString(addr.getTextString()));
    //     values.put(Addr.CHARSET, addr.getCharacterSet());
    //     values.put(Addr.TYPE, type);
    //
    //     Uri uri = Uri.parse("content://mms/" + msgId + "/addr");
    //     SqliteWrapper.insert(mContext, mContentResolver, uri, values);
    // }
    assert(0);
}

String PduPersister::GetPartContentType(
    /* [in] */ IPduPart* part)
{
    // ==================before translated======================
    // return part.getContentType() == null ? null : toIsoString(part.getContentType());
    assert(0);
    return String("");
}

void PduPersister::PersistData(
    /* [in] */ IPduPart* part,
    /* [in] */ IUri* uri,
    /* [in] */ const String& contentType,
    /* [in] */ IHashMap* preOpenedFiles)
{
    // ==================before translated======================
    // OutputStream os = null;
    // InputStream is = null;
    // DrmConvertSession drmConvertSession = null;
    // Uri dataUri = null;
    // String path = null;
    //
    // try {
    //     byte[] data = part.getData();
    //     if (ContentType.TEXT_PLAIN.equals(contentType)
    //             || ContentType.APP_SMIL.equals(contentType)
    //             || ContentType.TEXT_HTML.equals(contentType)) {
    //         ContentValues cv = new ContentValues();
    //         if (data == null) {
    //             data = new String("").getBytes(CharacterSets.DEFAULT_CHARSET_NAME);
    //         }
    //         cv.put(Telephony.Mms.Part.TEXT, new EncodedStringValue(data).getString());
    //         if (mContentResolver.update(uri, cv, null, null) != 1) {
    //             throw new MmsException("unable to update " + uri.toString());
    //         }
    //     } else {
    //         boolean isDrm = DownloadDrmHelper.isDrmConvertNeeded(contentType);
    //         if (isDrm) {
    //             if (uri != null) {
    //                 try {
    //                     path = convertUriToPath(mContext, uri);
    //                     if (LOCAL_LOGV) {
    //                         Log.v(TAG, "drm uri: " + uri + " path: " + path);
    //                     }
    //                     File f = new File(path);
    //                     long len = f.length();
    //                     if (LOCAL_LOGV) {
    //                         Log.v(TAG, "drm path: " + path + " len: " + len);
    //                     }
    //                     if (len > 0) {
    //                         // we're not going to re-persist and re-encrypt an already
    //                         // converted drm file
    //                         return;
    //                     }
    //                 } catch (Exception e) {
    //                     Log.e(TAG, "Can't get file info for: " + part.getDataUri(), e);
    //                 }
    //             }
    //             // We haven't converted the file yet, start the conversion
    //             drmConvertSession = DrmConvertSession.open(mContext, contentType);
    //             if (drmConvertSession == null) {
    //                 throw new MmsException("Mimetype " + contentType +
    //                         " can not be converted.");
    //             }
    //         }
    //         // uri can look like:
    //         // content://mms/part/98
    //         os = mContentResolver.openOutputStream(uri);
    //         if (data == null) {
    //             dataUri = part.getDataUri();
    //             if ((dataUri == null) || (dataUri == uri)) {
    //                 Log.w(TAG, "Can't find data for this part.");
    //                 return;
    //             }
    //             // dataUri can look like:
    //             // content://com.google.android.gallery3d.provider/picasa/item/5720646660183715586
    //             if (preOpenedFiles != null && preOpenedFiles.containsKey(dataUri)) {
    //                 is = preOpenedFiles.get(dataUri);
    //             }
    //             if (is == null) {
    //                 is = mContentResolver.openInputStream(dataUri);
    //             }
    //
    //             if (LOCAL_LOGV) {
    //                 Log.v(TAG, "Saving data to: " + uri);
    //             }
    //
    //             byte[] buffer = new byte[8192];
    //             for (int len = 0; (len = is.read(buffer)) != -1; ) {
    //                 if (!isDrm) {
    //                     os.write(buffer, 0, len);
    //                 } else {
    //                     byte[] convertedData = drmConvertSession.convert(buffer, len);
    //                     if (convertedData != null) {
    //                         os.write(convertedData, 0, convertedData.length);
    //                     } else {
    //                         throw new MmsException("Error converting drm data.");
    //                     }
    //                 }
    //             }
    //         } else {
    //             if (LOCAL_LOGV) {
    //                 Log.v(TAG, "Saving data to: " + uri);
    //             }
    //             if (!isDrm) {
    //                 os.write(data);
    //             } else {
    //                 dataUri = uri;
    //                 byte[] convertedData = drmConvertSession.convert(data, data.length);
    //                 if (convertedData != null) {
    //                     os.write(convertedData, 0, convertedData.length);
    //                 } else {
    //                     throw new MmsException("Error converting drm data.");
    //                 }
    //             }
    //         }
    //     }
    // } catch (FileNotFoundException e) {
    //     Log.e(TAG, "Failed to open Input/Output stream.", e);
    //     throw new MmsException(e);
    // } catch (IOException e) {
    //     Log.e(TAG, "Failed to read/write data.", e);
    //     throw new MmsException(e);
    // } finally {
    //     if (os != null) {
    //         try {
    //             os.close();
    //         } catch (IOException e) {
    //             Log.e(TAG, "IOException while closing: " + os, e);
    //         } // Ignore
    //     }
    //     if (is != null) {
    //         try {
    //             is.close();
    //         } catch (IOException e) {
    //             Log.e(TAG, "IOException while closing: " + is, e);
    //         } // Ignore
    //     }
    //     if (drmConvertSession != null) {
    //         drmConvertSession.close(path);
    //
    //         // Reset the permissions on the encrypted part file so everyone has only read
    //         // permission.
    //         File f = new File(path);
    //         ContentValues values = new ContentValues(0);
    //         SqliteWrapper.update(mContext, mContentResolver,
    //                              Uri.parse("content://mms/resetFilePerm/" + f.getName()),
    //                              values, null, null);
    //     }
    // }
    assert(0);
}

void PduPersister::UpdateAddress(
    /* [in] */ Int64 msgId,
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<IEncodedStringValue*>* array)
{
    // ==================before translated======================
    // // Delete old address information and then insert new ones.
    // SqliteWrapper.delete(mContext, mContentResolver,
    //         Uri.parse("content://mms/" + msgId + "/addr"),
    //         Addr.TYPE + "=" + type, null);
    //
    // persistAddress(msgId, type, array);
    assert(0);
}

void PduPersister::UpdatePart(
    /* [in] */ IUri* uri,
    /* [in] */ IPduPart* part,
    /* [in] */ IHashMap* preOpenedFiles)
{
    // ==================before translated======================
    // ContentValues values = new ContentValues(7);
    //
    // int charset = part.getCharset();
    // if (charset != 0 ) {
    //     values.put(Part.CHARSET, charset);
    // }
    //
    // String contentType = null;
    // if (part.getContentType() != null) {
    //     contentType = toIsoString(part.getContentType());
    //     values.put(Part.CONTENT_TYPE, contentType);
    // } else {
    //     throw new MmsException("MIME type of the part must be set.");
    // }
    //
    // if (part.getFilename() != null) {
    //     String fileName = new String(part.getFilename());
    //     values.put(Part.FILENAME, fileName);
    // }
    //
    // if (part.getName() != null) {
    //     String name = new String(part.getName());
    //     values.put(Part.NAME, name);
    // }
    //
    // Object value = null;
    // if (part.getContentDisposition() != null) {
    //     value = toIsoString(part.getContentDisposition());
    //     values.put(Part.CONTENT_DISPOSITION, (String) value);
    // }
    //
    // if (part.getContentId() != null) {
    //     value = toIsoString(part.getContentId());
    //     values.put(Part.CONTENT_ID, (String) value);
    // }
    //
    // if (part.getContentLocation() != null) {
    //     value = toIsoString(part.getContentLocation());
    //     values.put(Part.CONTENT_LOCATION, (String) value);
    // }
    //
    // SqliteWrapper.update(mContext, mContentResolver, uri, values, null, null);
    //
    // // Only update the data when:
    // // 1. New binary data supplied or
    // // 2. The Uri of the part is different from the current one.
    // if ((part.getData() != null)
    //         || (uri != part.getDataUri())) {
    //     persistData(part, uri, contentType, preOpenedFiles);
    // }
    assert(0);
}

void PduPersister::LoadRecipients(
    /* [in] */ Int32 addressType,
    /* [in] */ IHashSet* recipients,
    /* [in] */ IHashMap* addressMap,
    /* [in] */ Boolean excludeMyNumber)
{
    // ==================before translated======================
    // EncodedStringValue[] array = addressMap.get(addressType);
    // if (array == null) {
    //     return;
    // }
    // // If the TO recipients is only a single address, then we can skip loadRecipients when
    // // we're excluding our own number because we know that address is our own.
    // if (excludeMyNumber && array.length == 1) {
    //     return;
    // }
    // String myNumber = excludeMyNumber ? mTelephonyManager.getLine1Number() : null;
    // for (EncodedStringValue v : array) {
    //     if (v != null) {
    //         String number = v.getString();
    //         if ((myNumber == null || !PhoneNumberUtils.compare(number, myNumber)) &&
    //                 !recipients.contains(number)) {
    //             // Only add numbers which aren't my own number.
    //             recipients.add(number);
    //         }
    //     }
    // }
    assert(0);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
