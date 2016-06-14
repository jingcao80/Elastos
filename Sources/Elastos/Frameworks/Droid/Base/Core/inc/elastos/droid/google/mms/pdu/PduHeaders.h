#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.InvalidHeaderValueException;
// import java.util.ArrayList;
// import java.util.HashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduHeaders
    : public Object
    , public IPduHeaders
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Constructor of PduHeaders.
      */
    PduHeaders();

protected:
    /**
      * Get octet value by header field.
      *
      * @param field the field
      * @return the octet value of the pdu header
      *          with specified header field. Return 0 if
      *          the value is not set.
      */
    virtual CARAPI_(Int32) GetOctet(
        /* [in] */ Int32 field);

    /**
      * Set octet value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI_(void) SetOctet(
        /* [in] */ Int32 value,
        /* [in] */ Int32 field);

    /**
      * Get TextString value by header field.
      *
      * @param field the field
      * @return the TextString value of the pdu header
      *          with specified header field
      */
    virtual CARAPI_(AutoPtr< ArrayOf<Byte> >) GetTextString(
        /* [in] */ Int32 field);

    /**
      * Set TextString value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @return the TextString value of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI_(void) SetTextString(
        /* [in] */ ArrayOf<Byte>* value,
        /* [in] */ Int32 field);

    /**
      * Get EncodedStringValue value by header field.
      *
      * @param field the field
      * @return the EncodedStringValue value of the pdu header
      *          with specified header field
      */
    virtual CARAPI_(AutoPtr<IEncodedStringValue>) GetEncodedStringValue(
        /* [in] */ Int32 field);

    /**
      * Get TO, CC or BCC header value.
      *
      * @param field the field
      * @return the EncodeStringValue array of the pdu header
      *          with specified header field
      */
    virtual CARAPI_(AutoPtr<ArrayOf<IEncodedStringValue*> >) GetEncodedStringValues(
        /* [in] */ Int32 field);

    /**
      * Set EncodedStringValue value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @return the EncodedStringValue value of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI_(void) SetEncodedStringValue(
        /* [in] */ IEncodedStringValue* value,
        /* [in] */ Int32 field);

    /**
      * Set TO, CC or BCC header value.
      *
      * @param value the value
      * @param field the field
      * @return the EncodedStringValue value array of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI_(void) SetEncodedStringValues(
        /* [in] */ ArrayOf<IEncodedStringValue*>* value,
        /* [in] */ Int32 field);

    /**
      * Append one EncodedStringValue to another.
      *
      * @param value the EncodedStringValue to append
      * @param field the field
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI_(void) AppendEncodedStringValue(
        /* [in] */ IEncodedStringValue* value,
        /* [in] */ Int32 field);

    /**
      * Get LongInteger value by header field.
      *
      * @param field the field
      * @return the LongInteger value of the pdu header
      *          with specified header field. if return -1, the
      *          field is not existed in pdu header.
      */
    virtual CARAPI_(Int64) GetLongInteger(
        /* [in] */ Int32 field);

    /**
      * Set LongInteger value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      */
    virtual CARAPI_(void) SetLongInteger(
        /* [in] */ Int64 value,
        /* [in] */ Int32 field);

public:
    /**
      * All pdu header fields.
      */
    static const Int32 BCC = 0x81;
    static const Int32 CC = 0x82;
    static const Int32 CONTENT_LOCATION = 0x83;
    static const Int32 CONTENT_TYPE = 0x84;
    static const Int32 DATE = 0x85;
    static const Int32 DELIVERY_REPORT = 0x86;
    static const Int32 DELIVERY_TIME = 0x87;
    static const Int32 EXPIRY = 0x88;
    static const Int32 FROM = 0x89;
    static const Int32 MESSAGE_CLASS = 0x8A;
    static const Int32 MESSAGE_ID = 0x8B;
    static const Int32 MESSAGE_TYPE = 0x8C;
    static const Int32 MMS_VERSION = 0x8D;
    static const Int32 MESSAGE_SIZE = 0x8E;
    static const Int32 PRIORITY = 0x8F;
    static const Int32 READ_REPLY = 0x90;
    static const Int32 READ_REPORT = 0x90;
    static const Int32 REPORT_ALLOWED = 0x91;
    static const Int32 RESPONSE_STATUS = 0x92;
    static const Int32 RESPONSE_TEXT = 0x93;
    static const Int32 SENDER_VISIBILITY = 0x94;
    static const Int32 STATUS = 0x95;
    static const Int32 SUBJECT = 0x96;
    static const Int32 TO = 0x97;
    static const Int32 TRANSACTION_ID = 0x98;
    static const Int32 RETRIEVE_STATUS = 0x99;
    static const Int32 RETRIEVE_TEXT = 0x9A;
    static const Int32 READ_STATUS = 0x9B;
    static const Int32 REPLY_CHARGING = 0x9C;
    static const Int32 REPLY_CHARGING_DEADLINE = 0x9D;
    static const Int32 REPLY_CHARGING_ID = 0x9E;
    static const Int32 REPLY_CHARGING_SIZE = 0x9F;
    static const Int32 PREVIOUSLY_SENT_BY = 0xA0;
    static const Int32 PREVIOUSLY_SENT_DATE = 0xA1;
    static const Int32 STORE = 0xA2;
    static const Int32 MM_STATE = 0xA3;
    static const Int32 MM_FLAGS = 0xA4;
    static const Int32 STORE_STATUS = 0xA5;
    static const Int32 STORE_STATUS_TEXT = 0xA6;
    static const Int32 STORED = 0xA7;
    static const Int32 ATTRIBUTES = 0xA8;
    static const Int32 TOTALS = 0xA9;
    static const Int32 MBOX_TOTALS = 0xAA;
    static const Int32 QUOTAS = 0xAB;
    static const Int32 MBOX_QUOTAS = 0xAC;
    static const Int32 MESSAGE_COUNT = 0xAD;
    static const Int32 CONTENT = 0xAE;
    static const Int32 START = 0xAF;
    static const Int32 ADDITIONAL_HEADERS = 0xB0;
    static const Int32 DISTRIBUTION_INDICATOR = 0xB1;
    static const Int32 ELEMENT_DESCRIPTOR = 0xB2;
    static const Int32 LIMIT = 0xB3;
    static const Int32 RECOMMENDED_RETRIEVAL_MODE = 0xB4;
    static const Int32 RECOMMENDED_RETRIEVAL_MODE_TEXT = 0xB5;
    static const Int32 STATUS_TEXT = 0xB6;
    static const Int32 APPLIC_ID = 0xB7;
    static const Int32 REPLY_APPLIC_ID = 0xB8;
    static const Int32 AUX_APPLIC_ID = 0xB9;
    static const Int32 CONTENT_CLASS = 0xBA;
    static const Int32 DRM_CONTENT = 0xBB;
    static const Int32 ADAPTATION_ALLOWED = 0xBC;
    static const Int32 REPLACE_ID = 0xBD;
    static const Int32 CANCEL_ID = 0xBE;
    static const Int32 CANCEL_STATUS = 0xBF;
    /**
      * X-Mms-Message-Type field types.
      */
    static const Int32 MESSAGE_TYPE_SEND_REQ = 0x80;
    static const Int32 MESSAGE_TYPE_SEND_CONF = 0x81;
    static const Int32 MESSAGE_TYPE_NOTIFICATION_IND = 0x82;
    static const Int32 MESSAGE_TYPE_NOTIFYRESP_IND = 0x83;
    static const Int32 MESSAGE_TYPE_RETRIEVE_CONF = 0x84;
    static const Int32 MESSAGE_TYPE_ACKNOWLEDGE_IND = 0x85;
    static const Int32 MESSAGE_TYPE_DELIVERY_IND = 0x86;
    static const Int32 MESSAGE_TYPE_READ_REC_IND = 0x87;
    static const Int32 MESSAGE_TYPE_READ_ORIG_IND = 0x88;
    static const Int32 MESSAGE_TYPE_FORWARD_REQ = 0x89;
    static const Int32 MESSAGE_TYPE_FORWARD_CONF = 0x8A;
    static const Int32 MESSAGE_TYPE_MBOX_STORE_REQ = 0x8B;
    static const Int32 MESSAGE_TYPE_MBOX_STORE_CONF = 0x8C;
    static const Int32 MESSAGE_TYPE_MBOX_VIEW_REQ = 0x8D;
    static const Int32 MESSAGE_TYPE_MBOX_VIEW_CONF = 0x8E;
    static const Int32 MESSAGE_TYPE_MBOX_UPLOAD_REQ = 0x8F;
    static const Int32 MESSAGE_TYPE_MBOX_UPLOAD_CONF = 0x90;
    static const Int32 MESSAGE_TYPE_MBOX_DELETE_REQ = 0x91;
    static const Int32 MESSAGE_TYPE_MBOX_DELETE_CONF = 0x92;
    static const Int32 MESSAGE_TYPE_MBOX_DESCR = 0x93;
    static const Int32 MESSAGE_TYPE_DELETE_REQ = 0x94;
    static const Int32 MESSAGE_TYPE_DELETE_CONF = 0x95;
    static const Int32 MESSAGE_TYPE_CANCEL_REQ = 0x96;
    static const Int32 MESSAGE_TYPE_CANCEL_CONF = 0x97;
    /**
      *  X-Mms-Delivery-Report |
      *  X-Mms-Read-Report |
      *  X-Mms-Report-Allowed |
      *  X-Mms-Sender-Visibility |
      *  X-Mms-Store |
      *  X-Mms-Stored |
      *  X-Mms-Totals |
      *  X-Mms-Quotas |
      *  X-Mms-Distribution-Indicator |
      *  X-Mms-DRM-Content |
      *  X-Mms-Adaptation-Allowed |
      *  field types.
      */
    static const Int32 VALUE_YES = 0x80;
    static const Int32 VALUE_NO = 0x81;
    /**
      *  Delivery-Time |
      *  Expiry and Reply-Charging-Deadline |
      *  field type components.
      */
    static const Int32 VALUE_ABSOLUTE_TOKEN = 0x80;
    static const Int32 VALUE_RELATIVE_TOKEN = 0x81;
    /**
      * X-Mms-MMS-Version field types.
      */
    static const Int32 MMS_VERSION_1_3 = ((1 << 4) | 3);
    static const Int32 MMS_VERSION_1_2 = ((1 << 4) | 2);
    static const Int32 MMS_VERSION_1_1 = ((1 << 4) | 1);
    static const Int32 MMS_VERSION_1_0 = ((1 << 4) | 0);
    // Current version is 1.2.
    static const Int32 CURRENT_MMS_VERSION = MMS_VERSION_1_2;
    /**
      *  From field type components.
      */
    static const Int32 FROM_ADDRESS_PRESENT_TOKEN = 0x80;
    static const Int32 FROM_INSERT_ADDRESS_TOKEN = 0x81;
    static const String FROM_ADDRESS_PRESENT_TOKEN_STR;
    static const String FROM_INSERT_ADDRESS_TOKEN_STR;
    /**
      *  X-Mms-Status Field.
      */
    static const Int32 STATUS_EXPIRED = 0x80;
    static const Int32 STATUS_RETRIEVED = 0x81;
    static const Int32 STATUS_REJECTED = 0x82;
    static const Int32 STATUS_DEFERRED = 0x83;
    static const Int32 STATUS_UNRECOGNIZED = 0x84;
    static const Int32 STATUS_INDETERMINATE = 0x85;
    static const Int32 STATUS_FORWARDED = 0x86;
    static const Int32 STATUS_UNREACHABLE = 0x87;
    /**
      *  MM-Flags field type components.
      */
    static const Int32 MM_FLAGS_ADD_TOKEN = 0x80;
    static const Int32 MM_FLAGS_REMOVE_TOKEN = 0x81;
    static const Int32 MM_FLAGS_FILTER_TOKEN = 0x82;
    /**
      *  X-Mms-Message-Class field types.
      */
    static const Int32 MESSAGE_CLASS_PERSONAL = 0x80;
    static const Int32 MESSAGE_CLASS_ADVERTISEMENT = 0x81;
    static const Int32 MESSAGE_CLASS_INFORMATIONAL = 0x82;
    static const Int32 MESSAGE_CLASS_AUTO = 0x83;
    static const String MESSAGE_CLASS_PERSONAL_STR;
    static const String MESSAGE_CLASS_ADVERTISEMENT_STR;
    static const String MESSAGE_CLASS_INFORMATIONAL_STR;
    static const String MESSAGE_CLASS_AUTO_STR;
    /**
      *  X-Mms-Priority field types.
      */
    static const Int32 PRIORITY_LOW = 0x80;
    static const Int32 PRIORITY_NORMAL = 0x81;
    static const Int32 PRIORITY_HIGH = 0x82;
    /**
      *  X-Mms-Response-Status field types.
      */
    static const Int32 RESPONSE_STATUS_OK = 0x80;
    static const Int32 RESPONSE_STATUS_ERROR_UNSPECIFIED = 0x81;
    static const Int32 RESPONSE_STATUS_ERROR_SERVICE_DENIED = 0x82;
    static const Int32 RESPONSE_STATUS_ERROR_MESSAGE_FORMAT_CORRUPT = 0x83;
    static const Int32 RESPONSE_STATUS_ERROR_SENDING_ADDRESS_UNRESOLVED = 0x84;
    static const Int32 RESPONSE_STATUS_ERROR_MESSAGE_NOT_FOUND = 0x85;
    static const Int32 RESPONSE_STATUS_ERROR_NETWORK_PROBLEM = 0x86;
    static const Int32 RESPONSE_STATUS_ERROR_CONTENT_NOT_ACCEPTED = 0x87;
    static const Int32 RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE = 0x88;
    static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE = 0xC0;
    static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_SENDNG_ADDRESS_UNRESOLVED = 0xC1;
    static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND = 0xC2;
    static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM = 0xC3;
    static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS = 0xC4;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_FAILURE = 0xE0;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_SERVICE_DENIED = 0xE1;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT = 0xE2;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_SENDING_ADDRESS_UNRESOLVED = 0xE3;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND = 0xE4;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_CONTENT_NOT_ACCEPTED = 0xE5;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_LIMITATIONS_NOT_MET = 0xE6;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_REQUEST_NOT_ACCEPTED = 0xE6;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_FORWARDING_DENIED = 0xE8;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_NOT_SUPPORTED = 0xE9;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_ADDRESS_HIDING_NOT_SUPPORTED = 0xEA;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID = 0xEB;
    static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_END = 0xFF;
    /**
      *  X-Mms-Retrieve-Status field types.
      */
    static const Int32 RETRIEVE_STATUS_OK = 0x80;
    static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE = 0xC0;
    static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND = 0xC1;
    static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM = 0xC2;
    static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE = 0xE0;
    static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_SERVICE_DENIED = 0xE1;
    static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND = 0xE2;
    static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED = 0xE3;
    static const Int32 RETRIEVE_STATUS_ERROR_END = 0xFF;
    /**
      *  X-Mms-Sender-Visibility field types.
      */
    static const Int32 SENDER_VISIBILITY_HIDE = 0x80;
    static const Int32 SENDER_VISIBILITY_SHOW = 0x81;
    /**
      *  X-Mms-Read-Status field types.
      */
    static const Int32 READ_STATUS_READ = 0x80;
    static const Int32 READ_STATUS__DELETED_WITHOUT_BEING_READ = 0x81;
    /**
      *  X-Mms-Cancel-Status field types.
      */
    static const Int32 CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED = 0x80;
    static const Int32 CANCEL_STATUS_REQUEST_CORRUPTED = 0x81;
    /**
      *  X-Mms-Reply-Charging field types.
      */
    static const Int32 REPLY_CHARGING_REQUESTED = 0x80;
    static const Int32 REPLY_CHARGING_REQUESTED_TEXT_ONLY = 0x81;
    static const Int32 REPLY_CHARGING_ACCEPTED = 0x82;
    static const Int32 REPLY_CHARGING_ACCEPTED_TEXT_ONLY = 0x83;
    /**
      *  X-Mms-MM-State field types.
      */
    static const Int32 MM_STATE_DRAFT = 0x80;
    static const Int32 MM_STATE_SENT = 0x81;
    static const Int32 MM_STATE_NEW = 0x82;
    static const Int32 MM_STATE_RETRIEVED = 0x83;
    static const Int32 MM_STATE_FORWARDED = 0x84;
    /**
      * X-Mms-Recommended-Retrieval-Mode field types.
      */
    static const Int32 RECOMMENDED_RETRIEVAL_MODE_MANUAL = 0x80;
    /**
      *  X-Mms-Content-Class field types.
      */
    static const Int32 CONTENT_CLASS_TEXT = 0x80;
    static const Int32 CONTENT_CLASS_IMAGE_BASIC = 0x81;
    static const Int32 CONTENT_CLASS_IMAGE_RICH = 0x82;
    static const Int32 CONTENT_CLASS_VIDEO_BASIC = 0x83;
    static const Int32 CONTENT_CLASS_VIDEO_RICH = 0x84;
    static const Int32 CONTENT_CLASS_MEGAPIXEL = 0x85;
    static const Int32 CONTENT_CLASS_CONTENT_BASIC = 0x86;
    static const Int32 CONTENT_CLASS_CONTENT_RICH = 0x87;
    /**
      *  X-Mms-Store-Status field types.
      */
    static const Int32 STORE_STATUS_SUCCESS = 0x80;
    static const Int32 STORE_STATUS_ERROR_TRANSIENT_FAILURE = 0xC0;
    static const Int32 STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM = 0xC1;
    static const Int32 STORE_STATUS_ERROR_PERMANENT_FAILURE = 0xE0;
    static const Int32 STORE_STATUS_ERROR_PERMANENT_SERVICE_DENIED = 0xE1;
    static const Int32 STORE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT = 0xE2;
    static const Int32 STORE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND = 0xE3;
    static const Int32 STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL = 0xE4;
    static const Int32 STORE_STATUS_ERROR_END = 0xFF;

private:
    /**
      * The map contains the value of all headers.
      */
    AutoPtr<IHashMap> mHeaderMap; // Integer, Object
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__

