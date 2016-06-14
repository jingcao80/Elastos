#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              PduHeaders
//=====================================================================
CAR_INTERFACE_IMPL(PduHeaders, Object, IPduHeaders);

const Int32 PduHeaders::BCC;
const Int32 PduHeaders::CC;
const Int32 PduHeaders::CONTENT_LOCATION;
const Int32 PduHeaders::CONTENT_TYPE;
const Int32 PduHeaders::DATE;
const Int32 PduHeaders::DELIVERY_REPORT;
const Int32 PduHeaders::DELIVERY_TIME;
const Int32 PduHeaders::EXPIRY;
const Int32 PduHeaders::FROM;
const Int32 PduHeaders::MESSAGE_CLASS;
const Int32 PduHeaders::MESSAGE_ID;
const Int32 PduHeaders::MESSAGE_TYPE;
const Int32 PduHeaders::MMS_VERSION;
const Int32 PduHeaders::MESSAGE_SIZE;
const Int32 PduHeaders::PRIORITY;
const Int32 PduHeaders::READ_REPLY;
const Int32 PduHeaders::READ_REPORT;
const Int32 PduHeaders::REPORT_ALLOWED;
const Int32 PduHeaders::RESPONSE_STATUS;
const Int32 PduHeaders::RESPONSE_TEXT;
const Int32 PduHeaders::SENDER_VISIBILITY;
const Int32 PduHeaders::STATUS;
const Int32 PduHeaders::SUBJECT;
const Int32 PduHeaders::TO;
const Int32 PduHeaders::TRANSACTION_ID;
const Int32 PduHeaders::RETRIEVE_STATUS;
const Int32 PduHeaders::RETRIEVE_TEXT;
const Int32 PduHeaders::READ_STATUS;
const Int32 PduHeaders::REPLY_CHARGING;
const Int32 PduHeaders::REPLY_CHARGING_DEADLINE;
const Int32 PduHeaders::REPLY_CHARGING_ID;
const Int32 PduHeaders::REPLY_CHARGING_SIZE;
const Int32 PduHeaders::PREVIOUSLY_SENT_BY;
const Int32 PduHeaders::PREVIOUSLY_SENT_DATE;
const Int32 PduHeaders::STORE;
const Int32 PduHeaders::MM_STATE;
const Int32 PduHeaders::MM_FLAGS;
const Int32 PduHeaders::STORE_STATUS;
const Int32 PduHeaders::STORE_STATUS_TEXT;
const Int32 PduHeaders::STORED;
const Int32 PduHeaders::ATTRIBUTES;
const Int32 PduHeaders::TOTALS;
const Int32 PduHeaders::MBOX_TOTALS;
const Int32 PduHeaders::QUOTAS;
const Int32 PduHeaders::MBOX_QUOTAS;
const Int32 PduHeaders::MESSAGE_COUNT;
const Int32 PduHeaders::CONTENT;
const Int32 PduHeaders::START;
const Int32 PduHeaders::ADDITIONAL_HEADERS;
const Int32 PduHeaders::DISTRIBUTION_INDICATOR;
const Int32 PduHeaders::ELEMENT_DESCRIPTOR;
const Int32 PduHeaders::LIMIT;
const Int32 PduHeaders::RECOMMENDED_RETRIEVAL_MODE;
const Int32 PduHeaders::RECOMMENDED_RETRIEVAL_MODE_TEXT;
const Int32 PduHeaders::STATUS_TEXT;
const Int32 PduHeaders::APPLIC_ID;
const Int32 PduHeaders::REPLY_APPLIC_ID;
const Int32 PduHeaders::AUX_APPLIC_ID;
const Int32 PduHeaders::CONTENT_CLASS;
const Int32 PduHeaders::DRM_CONTENT;
const Int32 PduHeaders::ADAPTATION_ALLOWED;
const Int32 PduHeaders::REPLACE_ID;
const Int32 PduHeaders::CANCEL_ID;
const Int32 PduHeaders::CANCEL_STATUS;
const Int32 PduHeaders::MESSAGE_TYPE_SEND_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_SEND_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_NOTIFICATION_IND;
const Int32 PduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND;
const Int32 PduHeaders::MESSAGE_TYPE_RETRIEVE_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND;
const Int32 PduHeaders::MESSAGE_TYPE_DELIVERY_IND;
const Int32 PduHeaders::MESSAGE_TYPE_READ_REC_IND;
const Int32 PduHeaders::MESSAGE_TYPE_READ_ORIG_IND;
const Int32 PduHeaders::MESSAGE_TYPE_FORWARD_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_FORWARD_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_STORE_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_STORE_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_VIEW_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_VIEW_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_DELETE_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_DELETE_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_MBOX_DESCR;
const Int32 PduHeaders::MESSAGE_TYPE_DELETE_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_DELETE_CONF;
const Int32 PduHeaders::MESSAGE_TYPE_CANCEL_REQ;
const Int32 PduHeaders::MESSAGE_TYPE_CANCEL_CONF;
const Int32 PduHeaders::VALUE_YES;
const Int32 PduHeaders::VALUE_NO;
const Int32 PduHeaders::VALUE_ABSOLUTE_TOKEN;
const Int32 PduHeaders::VALUE_RELATIVE_TOKEN;
const Int32 PduHeaders::MMS_VERSION_1_3;
const Int32 PduHeaders::MMS_VERSION_1_2;
const Int32 PduHeaders::MMS_VERSION_1_1;
const Int32 PduHeaders::MMS_VERSION_1_0;
const Int32 PduHeaders::CURRENT_MMS_VERSION;
const Int32 PduHeaders::FROM_ADDRESS_PRESENT_TOKEN;
const Int32 PduHeaders::FROM_INSERT_ADDRESS_TOKEN;
const String PduHeaders::FROM_ADDRESS_PRESENT_TOKEN_STR("address-present-token");
const String PduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR("insert-address-token");
const Int32 PduHeaders::STATUS_EXPIRED;
const Int32 PduHeaders::STATUS_RETRIEVED;
const Int32 PduHeaders::STATUS_REJECTED;
const Int32 PduHeaders::STATUS_DEFERRED;
const Int32 PduHeaders::STATUS_UNRECOGNIZED;
const Int32 PduHeaders::STATUS_INDETERMINATE;
const Int32 PduHeaders::STATUS_FORWARDED;
const Int32 PduHeaders::STATUS_UNREACHABLE;
const Int32 PduHeaders::MM_FLAGS_ADD_TOKEN;
const Int32 PduHeaders::MM_FLAGS_REMOVE_TOKEN;
const Int32 PduHeaders::MM_FLAGS_FILTER_TOKEN;
const Int32 PduHeaders::MESSAGE_CLASS_PERSONAL;
const Int32 PduHeaders::MESSAGE_CLASS_ADVERTISEMENT;
const Int32 PduHeaders::MESSAGE_CLASS_INFORMATIONAL;
const Int32 PduHeaders::MESSAGE_CLASS_AUTO;
const String PduHeaders::MESSAGE_CLASS_PERSONAL_STR("personal");
const String PduHeaders::MESSAGE_CLASS_ADVERTISEMENT_STR("advertisement");
const String PduHeaders::MESSAGE_CLASS_INFORMATIONAL_STR("informational");
const String PduHeaders::MESSAGE_CLASS_AUTO_STR("auto");
const Int32 PduHeaders::PRIORITY_LOW;
const Int32 PduHeaders::PRIORITY_NORMAL;
const Int32 PduHeaders::PRIORITY_HIGH;
const Int32 PduHeaders::RESPONSE_STATUS_OK;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_UNSPECIFIED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_SERVICE_DENIED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_MESSAGE_FORMAT_CORRUPT;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_SENDING_ADDRESS_UNRESOLVED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_NETWORK_PROBLEM;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_CONTENT_NOT_ACCEPTED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_TRANSIENT_SENDNG_ADDRESS_UNRESOLVED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_FAILURE;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_SERVICE_DENIED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_SENDING_ADDRESS_UNRESOLVED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_CONTENT_NOT_ACCEPTED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_LIMITATIONS_NOT_MET;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_REQUEST_NOT_ACCEPTED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_FORWARDING_DENIED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_NOT_SUPPORTED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_ADDRESS_HIDING_NOT_SUPPORTED;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID;
const Int32 PduHeaders::RESPONSE_STATUS_ERROR_PERMANENT_END;
const Int32 PduHeaders::RETRIEVE_STATUS_OK;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_PERMANENT_SERVICE_DENIED;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED;
const Int32 PduHeaders::RETRIEVE_STATUS_ERROR_END;
const Int32 PduHeaders::SENDER_VISIBILITY_HIDE;
const Int32 PduHeaders::SENDER_VISIBILITY_SHOW;
const Int32 PduHeaders::READ_STATUS_READ;
const Int32 PduHeaders::READ_STATUS__DELETED_WITHOUT_BEING_READ;
const Int32 PduHeaders::CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED;
const Int32 PduHeaders::CANCEL_STATUS_REQUEST_CORRUPTED;
const Int32 PduHeaders::REPLY_CHARGING_REQUESTED;
const Int32 PduHeaders::REPLY_CHARGING_REQUESTED_TEXT_ONLY;
const Int32 PduHeaders::REPLY_CHARGING_ACCEPTED;
const Int32 PduHeaders::REPLY_CHARGING_ACCEPTED_TEXT_ONLY;
const Int32 PduHeaders::MM_STATE_DRAFT;
const Int32 PduHeaders::MM_STATE_SENT;
const Int32 PduHeaders::MM_STATE_NEW;
const Int32 PduHeaders::MM_STATE_RETRIEVED;
const Int32 PduHeaders::MM_STATE_FORWARDED;
const Int32 PduHeaders::RECOMMENDED_RETRIEVAL_MODE_MANUAL;
const Int32 PduHeaders::CONTENT_CLASS_TEXT;
const Int32 PduHeaders::CONTENT_CLASS_IMAGE_BASIC;
const Int32 PduHeaders::CONTENT_CLASS_IMAGE_RICH;
const Int32 PduHeaders::CONTENT_CLASS_VIDEO_BASIC;
const Int32 PduHeaders::CONTENT_CLASS_VIDEO_RICH;
const Int32 PduHeaders::CONTENT_CLASS_MEGAPIXEL;
const Int32 PduHeaders::CONTENT_CLASS_CONTENT_BASIC;
const Int32 PduHeaders::CONTENT_CLASS_CONTENT_RICH;
const Int32 PduHeaders::STORE_STATUS_SUCCESS;
const Int32 PduHeaders::STORE_STATUS_ERROR_TRANSIENT_FAILURE;
const Int32 PduHeaders::STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM;
const Int32 PduHeaders::STORE_STATUS_ERROR_PERMANENT_FAILURE;
const Int32 PduHeaders::STORE_STATUS_ERROR_PERMANENT_SERVICE_DENIED;
const Int32 PduHeaders::STORE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT;
const Int32 PduHeaders::STORE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND;
const Int32 PduHeaders::STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL;
const Int32 PduHeaders::STORE_STATUS_ERROR_END;

PduHeaders::PduHeaders()
{
    // ==================before translated======================
    // mHeaderMap = new HashMap<Integer, Object>();
}

Int32 PduHeaders::GetOctet(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // Integer octet = (Integer) mHeaderMap.get(field);
    // if (null == octet) {
    //     return 0;
    // }
    //
    // return octet;
    assert(0);
    return 0;
}

void PduHeaders::SetOctet(
    /* [in] */ Int32 value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // /**
    //  * Check whether this field can be set for specific
    //  * header and check validity of the field.
    //  */
    // switch (field) {
    //     case REPORT_ALLOWED:
    //     case ADAPTATION_ALLOWED:
    //     case DELIVERY_REPORT:
    //     case DRM_CONTENT:
    //     case DISTRIBUTION_INDICATOR:
    //     case QUOTAS:
    //     case READ_REPORT:
    //     case STORE:
    //     case STORED:
    //     case TOTALS:
    //     case SENDER_VISIBILITY:
    //         if ((VALUE_YES != value) && (VALUE_NO != value)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case READ_STATUS:
    //         if ((READ_STATUS_READ != value) &&
    //                 (READ_STATUS__DELETED_WITHOUT_BEING_READ != value)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case CANCEL_STATUS:
    //         if ((CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED != value) &&
    //                 (CANCEL_STATUS_REQUEST_CORRUPTED != value)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case PRIORITY:
    //         if ((value < PRIORITY_LOW) || (value > PRIORITY_HIGH)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case STATUS:
    //         break;
    //     case REPLY_CHARGING:
    //         if ((value < REPLY_CHARGING_REQUESTED)
    //                 || (value > REPLY_CHARGING_ACCEPTED_TEXT_ONLY)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case MM_STATE:
    //         if ((value < MM_STATE_DRAFT) || (value > MM_STATE_FORWARDED)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case RECOMMENDED_RETRIEVAL_MODE:
    //         if (RECOMMENDED_RETRIEVAL_MODE_MANUAL != value) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case CONTENT_CLASS:
    //         if ((value < CONTENT_CLASS_TEXT)
    //                 || (value > CONTENT_CLASS_CONTENT_RICH)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     case RETRIEVE_STATUS:
    //         // According to oma-ts-mms-enc-v1_3, section 7.3.50, we modify the invalid value.
    //         if ((value > RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
    //                 (value < RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE)) {
    //             value = RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE;
    //         } else if ((value > RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED) &&
    //                 (value <= RETRIEVE_STATUS_ERROR_END)) {
    //             value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
    //         } else if ((value < RETRIEVE_STATUS_OK) ||
    //                 ((value > RETRIEVE_STATUS_OK) &&
    //                         (value < RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
    //                         (value > RETRIEVE_STATUS_ERROR_END)) {
    //             value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
    //         }
    //         break;
    //     case STORE_STATUS:
    //         // According to oma-ts-mms-enc-v1_3, section 7.3.58, we modify the invalid value.
    //         if ((value > STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
    //                 (value < STORE_STATUS_ERROR_PERMANENT_FAILURE)) {
    //             value = STORE_STATUS_ERROR_TRANSIENT_FAILURE;
    //         } else if ((value > STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL) &&
    //                 (value <= STORE_STATUS_ERROR_END)) {
    //             value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
    //         } else if ((value < STORE_STATUS_SUCCESS) ||
    //                 ((value > STORE_STATUS_SUCCESS) &&
    //                         (value < STORE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
    //                         (value > STORE_STATUS_ERROR_END)) {
    //             value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
    //         }
    //         break;
    //     case RESPONSE_STATUS:
    //         // According to oma-ts-mms-enc-v1_3, section 7.3.48, we modify the invalid value.
    //         if ((value > RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS) &&
    //                 (value < RESPONSE_STATUS_ERROR_PERMANENT_FAILURE)) {
    //             value = RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE;
    //         } else if (((value > RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID) &&
    //                 (value <= RESPONSE_STATUS_ERROR_PERMANENT_END)) ||
    //                 (value < RESPONSE_STATUS_OK) ||
    //                 ((value > RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE) &&
    //                         (value < RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
    //                         (value > RESPONSE_STATUS_ERROR_PERMANENT_END)) {
    //             value = RESPONSE_STATUS_ERROR_PERMANENT_FAILURE;
    //         }
    //         break;
    //     case MMS_VERSION:
    //         if ((value < MMS_VERSION_1_0)|| (value > MMS_VERSION_1_3)) {
    //             value = CURRENT_MMS_VERSION; // Current version is the default value.
    //         }
    //         break;
    //     case MESSAGE_TYPE:
    //         if ((value < MESSAGE_TYPE_SEND_REQ) || (value > MESSAGE_TYPE_CANCEL_CONF)) {
    //             // Invalid value.
    //             throw new InvalidHeaderValueException("Invalid Octet value!");
    //         }
    //         break;
    //     default:
    //         // This header value should not be Octect.
    //         throw new RuntimeException("Invalid header field!");
    // }
    // mHeaderMap.put(field, value);
    assert(0);
}

AutoPtr<ArrayOf<Byte> > PduHeaders::GetTextString(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // return (byte[]) mHeaderMap.get(field);
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

void PduHeaders::SetTextString(
    /* [in] */ ArrayOf<Byte>* value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // /**
    //  * Check whether this field can be set for specific
    //  * header and check validity of the field.
    //  */
    // if (null == value) {
    //     throw new NullPointerException();
    // }
    //
    // switch (field) {
    //     case TRANSACTION_ID:
    //     case REPLY_CHARGING_ID:
    //     case AUX_APPLIC_ID:
    //     case APPLIC_ID:
    //     case REPLY_APPLIC_ID:
    //     case MESSAGE_ID:
    //     case REPLACE_ID:
    //     case CANCEL_ID:
    //     case CONTENT_LOCATION:
    //     case MESSAGE_CLASS:
    //     case CONTENT_TYPE:
    //         break;
    //     default:
    //         // This header value should not be Text-String.
    //         throw new RuntimeException("Invalid header field!");
    // }
    // mHeaderMap.put(field, value);
    assert(0);
}

AutoPtr<IEncodedStringValue> PduHeaders::GetEncodedStringValue(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // return (EncodedStringValue) mHeaderMap.get(field);
    assert(0);
    AutoPtr<IEncodedStringValue> empty;
    return empty;
}

AutoPtr<ArrayOf<IEncodedStringValue*> > PduHeaders::GetEncodedStringValues(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // ArrayList<EncodedStringValue> list =
    //         (ArrayList<EncodedStringValue>) mHeaderMap.get(field);
    // if (null == list) {
    //     return null;
    // }
    // EncodedStringValue[] values = new EncodedStringValue[list.size()];
    // return list.toArray(values);
    assert(0);
    AutoPtr< ArrayOf<IEncodedStringValue*> > empty;
    return empty;
}

void PduHeaders::SetEncodedStringValue(
    /* [in] */ IEncodedStringValue* value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // /**
    //  * Check whether this field can be set for specific
    //  * header and check validity of the field.
    //  */
    // if (null == value) {
    //     throw new NullPointerException();
    // }
    //
    // switch (field) {
    //     case SUBJECT:
    //     case RECOMMENDED_RETRIEVAL_MODE_TEXT:
    //     case RETRIEVE_TEXT:
    //     case STATUS_TEXT:
    //     case STORE_STATUS_TEXT:
    //     case RESPONSE_TEXT:
    //     case FROM:
    //     case PREVIOUSLY_SENT_BY:
    //     case MM_FLAGS:
    //         break;
    //     default:
    //         // This header value should not be Encoded-String-Value.
    //         throw new RuntimeException("Invalid header field!");
    // }
    //
    // mHeaderMap.put(field, value);
    assert(0);
}

void PduHeaders::SetEncodedStringValues(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // /**
    //  * Check whether this field can be set for specific
    //  * header and check validity of the field.
    //  */
    // if (null == value) {
    //     throw new NullPointerException();
    // }
    //
    // switch (field) {
    //     case BCC:
    //     case CC:
    //     case TO:
    //         break;
    //     default:
    //         // This header value should not be Encoded-String-Value.
    //         throw new RuntimeException("Invalid header field!");
    // }
    //
    // ArrayList<EncodedStringValue> list = new ArrayList<EncodedStringValue>();
    // for (int i = 0; i < value.length; i++) {
    //     list.add(value[i]);
    // }
    // mHeaderMap.put(field, list);
    assert(0);
}

void PduHeaders::AppendEncodedStringValue(
    /* [in] */ IEncodedStringValue* value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // if (null == value) {
    //     throw new NullPointerException();
    // }
    //
    // switch (field) {
    //     case BCC:
    //     case CC:
    //     case TO:
    //         break;
    //     default:
    //         throw new RuntimeException("Invalid header field!");
    // }
    //
    // ArrayList<EncodedStringValue> list =
    //     (ArrayList<EncodedStringValue>) mHeaderMap.get(field);
    // if (null == list) {
    //     list  = new ArrayList<EncodedStringValue>();
    // }
    // list.add(value);
    // mHeaderMap.put(field, list);
    assert(0);
}

Int64 PduHeaders::GetLongInteger(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // Long longInteger = (Long) mHeaderMap.get(field);
    // if (null == longInteger) {
    //     return -1;
    // }
    //
    // return longInteger.longValue();
    assert(0);
    return 0;
}

void PduHeaders::SetLongInteger(
    /* [in] */ Int64 value,
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // /**
    //  * Check whether this field can be set for specific
    //  * header and check validity of the field.
    //  */
    // switch (field) {
    //     case DATE:
    //     case REPLY_CHARGING_SIZE:
    //     case MESSAGE_SIZE:
    //     case MESSAGE_COUNT:
    //     case START:
    //     case LIMIT:
    //     case DELIVERY_TIME:
    //     case EXPIRY:
    //     case REPLY_CHARGING_DEADLINE:
    //     case PREVIOUSLY_SENT_DATE:
    //         break;
    //     default:
    //         // This header value should not be LongInteger.
    //         throw new RuntimeException("Invalid header field!");
    // }
    // mHeaderMap.put(field, value);
    assert(0);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
