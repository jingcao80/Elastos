#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              PduHeaders
//=====================================================================
CAR_INTERFACE_IMPL(PduHeaders, Object, IPduHeaders);

PduHeaders::PduHeaders()
{
    CHashMap::New((IHashMap**)&mHeaderMap);
}

ECode PduHeaders::GetOctet(
    /* [in] */ Int32 field,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    AutoPtr<IInteger32> octet = IInteger32::Probe(p);
    if (NULL == octet) {
        *result = 0;
        return NOERROR;
    }
    return octet->GetValue(result);
}

ECode PduHeaders::SetOctet(
    /* [in] */ Int32 value,
    /* [in] */ Int32 field)
{
    /**
     * Check whether this field can be set for specific
     * header and check validity of the field.
     */
    switch (field) {
        case REPORT_ALLOWED:
        case ADAPTATION_ALLOWED:
        case DELIVERY_REPORT:
        case DRM_CONTENT:
        case DISTRIBUTION_INDICATOR:
        case QUOTAS:
        case READ_REPORT:
        case STORE:
        case STORED:
        case TOTALS:
        case SENDER_VISIBILITY: {
            if ((VALUE_YES != value) && (VALUE_NO != value)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case READ_STATUS: {
            if ((READ_STATUS_READ != value) &&
                    (READ_STATUS__DELETED_WITHOUT_BEING_READ != value)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case CANCEL_STATUS: {
            if ((CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED != value) &&
                    (CANCEL_STATUS_REQUEST_CORRUPTED != value)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case PRIORITY: {
            if ((value < PRIORITY_LOW) || (value > PRIORITY_HIGH)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case STATUS: {
            break;
        }
        case REPLY_CHARGING: {
            if ((value < REPLY_CHARGING_REQUESTED)
                    || (value > REPLY_CHARGING_ACCEPTED_TEXT_ONLY)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case MM_STATE: {
            if ((value < MM_STATE_DRAFT) || (value > MM_STATE_FORWARDED)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case RECOMMENDED_RETRIEVAL_MODE: {
            if (RECOMMENDED_RETRIEVAL_MODE_MANUAL != value) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case CONTENT_CLASS: {
            if ((value < CONTENT_CLASS_TEXT)
                    || (value > CONTENT_CLASS_CONTENT_RICH)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case RETRIEVE_STATUS: {
            // According to oma-ts-mms-enc-v1_3, section 7.3.50, we modify the invalid value.
            if ((value > RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
                    (value < RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE)) {
                value = RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE;
            }
            else if ((value > RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED) &&
                    (value <= RETRIEVE_STATUS_ERROR_END)) {
                value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
            }
            else if ((value < RETRIEVE_STATUS_OK) ||
                    ((value > RETRIEVE_STATUS_OK) &&
                            (value < RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                            (value > RETRIEVE_STATUS_ERROR_END)) {
                value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
            }
            break;
        }
        case STORE_STATUS: {
            // According to oma-ts-mms-enc-v1_3, section 7.3.58, we modify the invalid value.
            if ((value > STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
                    (value < STORE_STATUS_ERROR_PERMANENT_FAILURE)) {
                value = STORE_STATUS_ERROR_TRANSIENT_FAILURE;
            }
            else if ((value > STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL) &&
                    (value <= STORE_STATUS_ERROR_END)) {
                value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
            }
            else if ((value < STORE_STATUS_SUCCESS) ||
                    ((value > STORE_STATUS_SUCCESS) &&
                            (value < STORE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                            (value > STORE_STATUS_ERROR_END)) {
                value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
            }
            break;
        }
        case RESPONSE_STATUS: {
            // According to oma-ts-mms-enc-v1_3, section 7.3.48, we modify the invalid value.
            if ((value > RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS) &&
                    (value < RESPONSE_STATUS_ERROR_PERMANENT_FAILURE)) {
                value = RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE;
            }
            else if (((value > RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID) &&
                    (value <= RESPONSE_STATUS_ERROR_PERMANENT_END)) ||
                    (value < RESPONSE_STATUS_OK) ||
                    ((value > RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE) &&
                            (value < RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                            (value > RESPONSE_STATUS_ERROR_PERMANENT_END)) {
                value = RESPONSE_STATUS_ERROR_PERMANENT_FAILURE;
            }
            break;
        }
        case MMS_VERSION: {
            if ((value < MMS_VERSION_1_0)|| (value > MMS_VERSION_1_3)) {
                value = CURRENT_MMS_VERSION; // Current version is the default value.
            }
            break;
        }
        case MESSAGE_TYPE: {
            if ((value < MESSAGE_TYPE_SEND_REQ) || (value > MESSAGE_TYPE_CANCEL_CONF)) {
                // Invalid value.
                // throw new InvalidHeaderValueException("Invalid Octet value!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        default: {
            // This header value should not be Octect.
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
        }
    }
    mHeaderMap->Put(CoreUtils::Convert(field), CoreUtils::Convert(value));
    return NOERROR;
}

ECode PduHeaders::GetTextString(
    /* [in] */ Int32 field,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(p);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IByte::Probe(obj)->GetValue(&(*bytes)[i]);
    }
    *result = bytes;
    return NOERROR;
}

ECode PduHeaders::SetTextString(
    /* [in] */ ArrayOf<Byte>* value,
    /* [in] */ Int32 field)
{
    /**
     * Check whether this field can be set for specific
     * header and check validity of the field.
     */
    if (NULL == value) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    switch (field) {
        case TRANSACTION_ID:
        case REPLY_CHARGING_ID:
        case AUX_APPLIC_ID:
        case APPLIC_ID:
        case REPLY_APPLIC_ID:
        case MESSAGE_ID:
        case REPLACE_ID:
        case CANCEL_ID:
        case CONTENT_LOCATION:
        case MESSAGE_CLASS:
        case CONTENT_TYPE:
            break;
        default:
            // This header value should not be Text-String.
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
    }
    mHeaderMap->Put(CoreUtils::Convert(field), CoreUtils::Convert(String(*value)));
    return NOERROR;
}

ECode PduHeaders::GetEncodedStringValue(
    /* [in] */ Int32 field,
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    *result = IEncodedStringValue::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduHeaders::GetEncodedStringValues(
    /* [in] */ Int32 field,
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    AutoPtr<IArrayList> list = IArrayList::Probe(p);
    if (NULL == list) {
        *result = NULL;
        return NOERROR;
    }
    Int32 size = 0;
    list->GetSize(&size);
    AutoPtr<ArrayOf<IEncodedStringValue*> > values = ArrayOf<IEncodedStringValue*>::Alloc(size);
    AutoPtr<ArrayOf<IEncodedStringValue*> > res;
    list->ToArray((ArrayOf<IInterface*>*)values.Get(), (ArrayOf<IInterface*>**)&res);
    *result = res;
    return NOERROR;
}

ECode PduHeaders::SetEncodedStringValue(
    /* [in] */ IEncodedStringValue* value,
    /* [in] */ Int32 field)
{
    /**
     * Check whether this field can be set for specific
     * header and check validity of the field.
     */
    if (NULL == value) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    switch (field) {
        case SUBJECT:
        case RECOMMENDED_RETRIEVAL_MODE_TEXT:
        case RETRIEVE_TEXT:
        case STATUS_TEXT:
        case STORE_STATUS_TEXT:
        case RESPONSE_TEXT:
        case FROM:
        case PREVIOUSLY_SENT_BY:
        case MM_FLAGS:
            break;
        default:
            // This header value should not be Encoded-String-Value.
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
    }

    mHeaderMap->Put(CoreUtils::Convert(field), value);
    return NOERROR;
}

ECode PduHeaders::SetEncodedStringValues(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value,
    /* [in] */ Int32 field)
{
    /**
     * Check whether this field can be set for specific
     * header and check validity of the field.
     */
    if (NULL == value) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    switch (field) {
        case BCC:
        case CC:
        case TO:
            break;
        default:
            // This header value should not be Encoded-String-Value.
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    for (Int32 i = 0; i < value->GetLength(); i++) {
        list->Add((*value)[i]);
    }
    mHeaderMap->Put(CoreUtils::Convert(field), list);
    return NOERROR;
}

ECode PduHeaders::AppendEncodedStringValue(
    /* [in] */ IEncodedStringValue* value,
    /* [in] */ Int32 field)
{
    if (NULL == value) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    switch (field) {
        case BCC:
        case CC:
        case TO:
            break;
        default:
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    AutoPtr<IArrayList> list = IArrayList::Probe(p);
    if (NULL == list) {
        CArrayList::New((IArrayList**)&list);
    }
    list->Add(value);
    mHeaderMap->Put(CoreUtils::Convert(field), list);
    return NOERROR;
}

ECode PduHeaders::GetLongInteger(
    /* [in] */ Int32 field,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mHeaderMap->Get(CoreUtils::Convert(field), (IInterface**)&p);
    AutoPtr<IInteger64> longInteger = IInteger64::Probe(p);
    if (NULL == longInteger) {
        *result = -1;
        return NOERROR;
    }

    return longInteger->GetValue(result);
}

ECode PduHeaders::SetLongInteger(
    /* [in] */ Int64 value,
    /* [in] */ Int32 field)
{
    /**
     * Check whether this field can be set for specific
     * header and check validity of the field.
     */
    switch (field) {
        case DATE:
        case REPLY_CHARGING_SIZE:
        case MESSAGE_SIZE:
        case MESSAGE_COUNT:
        case START:
        case LIMIT:
        case DELIVERY_TIME:
        case EXPIRY:
        case REPLY_CHARGING_DEADLINE:
        case PREVIOUSLY_SENT_DATE:
            break;
        default:
            // This header value should not be LongInteger.
            // throw new RuntimeException("Invalid header field!");
            return E_RUNTIME_EXCEPTION;
    }
    mHeaderMap->Put(CoreUtils::Convert(field), CoreUtils::Convert(value));
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
