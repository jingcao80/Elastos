#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduComposer.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/google/mms/pdu/CEncodedStringValueHelper.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                     PduComposer::PositionMarker
//=====================================================================
PduComposer::PositionMarker::PositionMarker(
    /* [in] */ PduComposer* host)
    : mHost(host)
{
}

ECode PduComposer::PositionMarker::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // If these assert fails, likely that you are finding the
    // size of buffer that is deep in BufferStack you can only
    // find the length of the buffer that is on top
    if (mCurrentStackSize != mHost->mStack->mStackSize) {
        // throw new RuntimeException("BUG: Invalid call to getLength()");
        return E_RUNTIME_EXCEPTION;
    }

    *result = mHost->mPosition - mC_pos;
    return NOERROR;
}

//=====================================================================
//                       PduComposer::BufferStack
//=====================================================================
PduComposer::BufferStack::BufferStack(
    /* [in] */ PduComposer* host)
    : mHost(host)
{
}

ECode PduComposer::BufferStack::Newbuf()
{
    // You can't create a new buff when toCopy != null
    // That is after calling pop() and before calling copy()
    // If you do, it is a bug
    if (mToCopy != NULL) {
        // throw new RuntimeException("BUG: Invalid newbuf() before copy()");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<LengthRecordNode> temp = new LengthRecordNode();

    temp->mCurrentMessage = mHost->mMessage;
    temp->mCurrentPosition = mHost->mPosition;

    temp->mNext = mStack;
    mStack = temp;

    mStackSize = mStackSize + 1;

    CByteArrayOutputStream::New((IByteArrayOutputStream**)&(mHost->mMessage));
    mHost->mPosition = 0;
    return NOERROR;
}

ECode PduComposer::BufferStack::Pop()
{
    AutoPtr<IByteArrayOutputStream> currentMessage = mHost->mMessage;
    Int32 currentPosition = mHost->mPosition;

    mHost->mMessage = mStack->mCurrentMessage;
    mHost->mPosition = mStack->mCurrentPosition;

    mToCopy = mStack;
    // Re using the top element of the stack to avoid memory allocation

    mStack = mStack->mNext;
    mStackSize = mStackSize - 1;

    mToCopy->mCurrentMessage = currentMessage;
    mToCopy->mCurrentPosition = currentPosition;
    return NOERROR;
}

ECode PduComposer::BufferStack::Copy()
{
    AutoPtr<ArrayOf<Byte> > arr;
    mToCopy->mCurrentMessage->ToByteArray((ArrayOf<Byte>**)&arr);
    mHost->Arraycopy(arr, 0,
            mToCopy->mCurrentPosition);

    mToCopy = NULL;
    return NOERROR;
}

ECode PduComposer::BufferStack::Mark(
    /* [out] */ PositionMarker** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<PositionMarker> m = new PositionMarker(mHost);

    m->mC_pos = mHost->mPosition;
    m->mCurrentStackSize = mStackSize;

    *result = m;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=====================================================================
//                             PduComposer
//=====================================================================
CAR_INTERFACE_IMPL(PduComposer, Object, IPduComposer);

const String PduComposer::REGEXP_PHONE_NUMBER_ADDRESS_TYPE("\\+?[0-9|\\.|\\-]+");
const String PduComposer::REGEXP_EMAIL_ADDRESS_TYPE("[a-zA-Z| ]*\\<{0,1}[a-zA-Z| ]+@{1}[a-zA-Z| ]+\\.{1}[a-zA-Z| ]+\\>{0,1}");
const String PduComposer::REGEXP_IPV6_ADDRESS_TYPE("[a-fA-F]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}");
const String PduComposer::REGEXP_IPV4_ADDRESS_TYPE("[0-9]{1,3}\\.{1}[0-9]{1,3}\\.{1}[0-9]{1,3}\\.{1}[0-9]{1,3}");

const String PduComposer::STRING_PHONE_NUMBER_ADDRESS_TYPE("/TYPE=PLMN");
const String PduComposer::STRING_IPV4_ADDRESS_TYPE("/TYPE=IPV4");
const String PduComposer::STRING_IPV6_ADDRESS_TYPE("/TYPE=IPV6");
const Int32 PduComposer::PDU_PHONE_NUMBER_ADDRESS_TYPE;
const Int32 PduComposer::PDU_EMAIL_ADDRESS_TYPE;
const Int32 PduComposer::PDU_IPV4_ADDRESS_TYPE;
const Int32 PduComposer::PDU_IPV6_ADDRESS_TYPE;
const Int32 PduComposer::PDU_UNKNOWN_ADDRESS_TYPE;
const Int32 PduComposer::PDU_COMPOSE_SUCCESS;
const Int32 PduComposer::PDU_COMPOSE_CONTENT_ERROR;
const Int32 PduComposer::PDU_COMPOSE_FIELD_NOT_SET;
const Int32 PduComposer::PDU_COMPOSE_FIELD_NOT_SUPPORTED;
const Int32 PduComposer::QUOTED_STRING_FLAG;
const Int32 PduComposer::END_STRING_FLAG;
const Int32 PduComposer::LENGTH_QUOTE;
const Int32 PduComposer::TEXT_MAX;
const Int32 PduComposer::SHORT_INTEGER_MAX;
const Int32 PduComposer::LONG_INTEGER_LENGTH_MAX;
const Int32 PduComposer::PDU_COMPOSER_BLOCK_SIZE;
AutoPtr<IHashMap> PduComposer::mContentTypeMap = NULL;

PduComposer::PduComposer()
{
}

ECode PduComposer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IGenericPdu* pdu)
{
    mPdu = pdu;
    context->GetContentResolver((IContentResolver**)&mResolver);
    pdu->GetPduHeaders((IPduHeaders**)&mPduHeader);
    mStack = new BufferStack(this);
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&mMessage);
    mPosition = 0;
    return NOERROR;
}

ECode PduComposer::Make(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // Get Message-type.
    Int32 type = 0;
    mPdu->GetMessageType(&type);

    /* make the message */
    switch (type) {
        case IPduHeaders::MESSAGE_TYPE_SEND_REQ:
        case IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF:
            if (MakeSendRetrievePdu(type) != PDU_COMPOSE_SUCCESS) {
                *result = NULL;
                return NOERROR;
            }
            break;
        case IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND:
            if (MakeNotifyResp() != PDU_COMPOSE_SUCCESS) {
                *result = NULL;
                return NOERROR;
            }
            break;
        case IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND:
            if (MakeAckInd() != PDU_COMPOSE_SUCCESS) {
                *result = NULL;
                return NOERROR;
            }
            break;
        case IPduHeaders::MESSAGE_TYPE_READ_REC_IND:
            if (MakeReadRecInd() != PDU_COMPOSE_SUCCESS) {
                *result = NULL;
                return NOERROR;
            }
            break;
        default: {
            *result = NULL;
            return NOERROR;
        }
    }

    return mMessage->ToByteArray(result);
}

void PduComposer::Arraycopy(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 length)
{
    IOutputStream::Probe(mMessage)->Write(buf, pos, length);
    mPosition = mPosition + length;
}

void PduComposer::Append(
    /* [in] */ Int32 value)
{
    IOutputStream::Probe(mMessage)->Write(value);
    mPosition++;
}

void PduComposer::AppendShortInteger(
    /* [in] */ Int32 value)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Short-integer = OCTET
     * ; Integers in range 0-127 shall be encoded as a one octet value
     * ; with the most significant bit set to one (1xxx xxxx) and with
     * ; the value in the remaining least significant bits.
     * In our implementation, only low 7 bits are stored and otherwise
     * bits are ignored.
     */
    Append((value | 0x80) & 0xff);
}

void PduComposer::AppendOctet(
    /* [in] */ Int32 number)
{
    Append(number);
}

void PduComposer::AppendShortLength(
    /* [in] */ Int32 value)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Short-length = <Any octet 0-30>
     */
    Append(value);
}

void PduComposer::AppendLongInteger(
    /* [in] */ Int64 longInt)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Long-integer = Short-length Multi-octet-integer
     * ; The Short-length indicates the length of the Multi-octet-integer
     * Multi-octet-integer = 1*30 OCTET
     * ; The content octets shall be an unsigned integer value with the
     * ; most significant octet encoded first (big-endian representation).
     * ; The minimum number of octets must be used to encode the value.
     */
    Int32 size = 0;
    Int64 temp = longInt;

    // Count the length of the long integer.
    for (size = 0; (temp != 0) && (size < LONG_INTEGER_LENGTH_MAX); size++) {
        temp = (temp >> 8);
    }

    // Set Length.
    AppendShortLength(size);

    // Count and set the long integer.
    Int32 i = 0;
    Int32 shift = (size -1) * 8;

    for (i = 0; i < size; i++) {
        Append((Int32)((longInt >> shift) & 0xff));
        shift = shift - 8;
    }
}

void PduComposer::AppendTextString(
    /* [in] */ ArrayOf<Byte>* text)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Text-string = [Quote] *TEXT End-of-string
     * ; If the first character in the TEXT is in the range of 128-255,
     * ; a Quote character must precede it. Otherwise the Quote character
     * ;must be omitted. The Quote is not part of the contents.
     */
    if ((((*text)[0]) & 0xff) > TEXT_MAX) { // No need to check for <= 255
        Append(TEXT_MAX);
    }

    Arraycopy(text, 0, text->GetLength());
    Append(0);
}

void PduComposer::AppendTextString(
    /* [in] */ const String& str)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Text-string = [Quote] *TEXT End-of-string
     * ; If the first character in the TEXT is in the range of 128-255,
     * ; a Quote character must precede it. Otherwise the Quote character
     * ;must be omitted. The Quote is not part of the contents.
     */
    AppendTextString(str.GetBytes());
}

void PduComposer::AppendEncodedString(
    /* [in] */ IEncodedStringValue* enStr)
{
    /*
     * From OMA-TS-MMS-ENC-V1_3-20050927-C:
     * Encoded-string-value = Text-string | Value-length Char-set Text-string
     */
    assert(enStr != NULL);

    Int32 charset = 0;
    enStr->GetCharacterSet(&charset);
    AutoPtr<ArrayOf<Byte> > textString;
    enStr->GetTextString((ArrayOf<Byte>**)&textString);
    if (NULL == textString) {
        return;
    }

    /*
     * In the implementation of EncodedStringValue, the charset field will
     * never be 0. It will always be composed as
     * Encoded-string-value = Value-length Char-set Text-string
     */
    mStack->Newbuf();
    AutoPtr<PositionMarker> start;
    mStack->Mark((PositionMarker**)&start);

    AppendShortInteger(charset);
    AppendTextString(textString);

    Int32 len = 0;
    start->GetLength(&len);
    mStack->Pop();
    AppendValueLength(len);
    mStack->Copy();
}

void PduComposer::AppendUintvarInteger(
    /* [in] */ Int64 value)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * To encode a large unsigned integer, split it into 7-bit fragments
     * and place them in the payloads of multiple octets. The most significant
     * bits are placed in the first octets with the least significant bits
     * ending up in the last octet. All octets MUST set the Continue bit to 1
     * except the last octet, which MUST set the Continue bit to 0.
     */
    Int32 i = 0;
    Int64 max = SHORT_INTEGER_MAX;

    for (i = 0; i < 5; i++) {
        if (value < max) {
            break;
        }

        max = (max << 7) | 0x7fl;
    }

    while (i > 0) {
        Int64 temp = value >> (i * 7);
        temp = temp & 0x7f;

        Append((Int32)((temp | 0x80) & 0xff));

        i--;
    }

    Append((Int32)(value & 0x7f));
}

void PduComposer::AppendDateValue(
    /* [in] */ Int64 date)
{
    /*
     * From OMA-TS-MMS-ENC-V1_3-20050927-C:
     * Date-value = Long-integer
     */
    AppendLongInteger(date);
}

void PduComposer::AppendValueLength(
    /* [in] */ Int64 value)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Value-length = Short-length | (Length-quote Length)
     * ; Value length is used to indicate the length of the value to follow
     * Short-length = <Any octet 0-30>
     * Length-quote = <Octet 31>
     * Length = Uintvar-integer
     */
    if (value < LENGTH_QUOTE) {
        AppendShortLength((Int32) value);
        return;
    }

    Append(LENGTH_QUOTE);
    AppendUintvarInteger(value);
}

void PduComposer::AppendQuotedString(
    /* [in] */ ArrayOf<Byte>* text)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Quoted-string = <Octet 34> *TEXT End-of-string
     * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
     * ;quotation-marks <"> removed.
     */
    Append(QUOTED_STRING_FLAG);
    Arraycopy(text, 0, text->GetLength());
    Append(END_STRING_FLAG);
}

void PduComposer::AppendQuotedString(
    /* [in] */ const String& str)
{
    /*
     * From WAP-230-WSP-20010705-a:
     * Quoted-string = <Octet 34> *TEXT End-of-string
     * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
     * ;quotation-marks <"> removed.
     */
    AppendQuotedString(str.GetBytes());
}

Int32 PduComposer::CheckAddressType(
    /* [in] */ const String& address)
{
    /**
     * From OMA-TS-MMS-ENC-V1_3-20050927-C.pdf, section 8.
     * address = ( e-mail / device-address / alphanum-shortcode / num-shortcode)
     * e-mail = mailbox; to the definition of mailbox as described in
     * section 3.4 of [RFC2822], but excluding the
     * obsolete definitions as indicated by the "obs-" prefix.
     * device-address = ( global-phone-number "/TYPE=PLMN" )
     * / ( ipv4 "/TYPE=IPv4" ) / ( ipv6 "/TYPE=IPv6" )
     * / ( escaped-value "/TYPE=" address-type )
     *
     * global-phone-number = ["+"] 1*( DIGIT / written-sep )
     * written-sep =("-"/".")
     *
     * ipv4 = 1*3DIGIT 3( "." 1*3DIGIT ) ; IPv4 address value
     *
     * ipv6 = 4HEXDIG 7( ":" 4HEXDIG ) ; IPv6 address per RFC 2373
     */

    if (address.IsNull()) {
        return PDU_UNKNOWN_ADDRESS_TYPE;
    }

    Boolean b = FALSE;
    if ((StringUtils::Matches(address, REGEXP_IPV4_ADDRESS_TYPE, &b), b)) {
        // Ipv4 address.
        return PDU_IPV4_ADDRESS_TYPE;
    }
    else if ((StringUtils::Matches(address, REGEXP_PHONE_NUMBER_ADDRESS_TYPE, &b), b)) {
        // Phone number.
        return PDU_PHONE_NUMBER_ADDRESS_TYPE;
    }
    else if ((StringUtils::Matches(address, REGEXP_EMAIL_ADDRESS_TYPE, &b), b)) {
        // Email address.
        return PDU_EMAIL_ADDRESS_TYPE;
    }
    else if ((StringUtils::Matches(address, REGEXP_IPV6_ADDRESS_TYPE, &b), b)) {
        // Ipv6 address.
        return PDU_IPV6_ADDRESS_TYPE;
    }
    else {
        // Unknown address.
        return PDU_UNKNOWN_ADDRESS_TYPE;
    }
}

AutoPtr<IEncodedStringValue> PduComposer::AppendAddressType(
    /* [in] */ IEncodedStringValue* address)
{
    AutoPtr<IEncodedStringValue> temp;

    // try {
    String str;
    address->GetString(&str);
    Int32 addressType = CheckAddressType(str);
    AutoPtr<IEncodedStringValueHelper> hlp;
    CEncodedStringValueHelper::AcquireSingleton((IEncodedStringValueHelper**)&hlp);
    hlp->Copy(address, (IEncodedStringValue**)&temp);
    if (PDU_PHONE_NUMBER_ADDRESS_TYPE == addressType) {
        // Phone number.
        temp->AppendTextString(STRING_PHONE_NUMBER_ADDRESS_TYPE.GetBytes());
    }
    else if (PDU_IPV4_ADDRESS_TYPE == addressType) {
        // Ipv4 address.
        temp->AppendTextString(STRING_IPV4_ADDRESS_TYPE.GetBytes());
    }
    else if (PDU_IPV6_ADDRESS_TYPE == addressType) {
        // Ipv6 address.
        temp->AppendTextString(STRING_IPV6_ADDRESS_TYPE.GetBytes());
    }
    // } catch (NullPointerException e) {
    //     return NULL;
    // }

    return temp;
}

Int32 PduComposer::AppendHeader(
    /* [in] */ Int32 field)
{
    switch (field) {
        case IPduHeaders::MMS_VERSION: {
            AppendOctet(field);

            Int32 version = 0;
            mPduHeader->GetOctet(field, &version);
            if (0 == version) {
                AppendShortInteger(IPduHeaders::CURRENT_MMS_VERSION);
            }
            else {
                AppendShortInteger(version);
            }

            break;
        }
        case IPduHeaders::MESSAGE_ID:
        case IPduHeaders::TRANSACTION_ID: {
            AutoPtr<ArrayOf<Byte> > textString;
            mPduHeader->GetTextString(field, (ArrayOf<Byte>**)&textString);
            if (NULL == textString) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);
            AppendTextString(textString);
            break;
        }

        case IPduHeaders::TO:
        case IPduHeaders::BCC:
        case IPduHeaders::CC: {
            AutoPtr<ArrayOf<IEncodedStringValue*> > addr;
            mPduHeader->GetEncodedStringValues(field, (ArrayOf<IEncodedStringValue*>**)&addr);

            if (NULL == addr) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AutoPtr<IEncodedStringValue> temp;
            for (Int32 i = 0; i < addr->GetLength(); i++) {
                temp = AppendAddressType((*addr)[i]);
                if (temp == NULL) {
                    return PDU_COMPOSE_CONTENT_ERROR;
                }

                AppendOctet(field);
                AppendEncodedString(temp);
            }
            break;
        }

        case IPduHeaders::FROM: {
            // Value-length (Address-present-token Encoded-string-value | Insert-address-token)
            AppendOctet(field);

            AutoPtr<IEncodedStringValue> from;
            mPduHeader->GetEncodedStringValue(field, (IEncodedStringValue**)&from);
            String str;
            AutoPtr<ArrayOf<Byte> > arr;
            if ((from == NULL)
                    || TextUtils::IsEmpty((from->GetString(&str), str))
                    || String(*(from->GetTextString((ArrayOf<Byte>**)&arr), arr)).Equals(
                            IPduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR)) {
                // Length of from = 1
                Append(1);
                // Insert-address-token = <Octet 129>
                Append(IPduHeaders::FROM_INSERT_ADDRESS_TOKEN);
            }
            else {
                mStack->Newbuf();
                AutoPtr<PositionMarker> fstart;
                mStack->Mark((PositionMarker**)&fstart);

                // Address-present-token = <Octet 128>
                Append(IPduHeaders::FROM_ADDRESS_PRESENT_TOKEN);

                AutoPtr<IEncodedStringValue> temp = AppendAddressType(from);
                if (temp == NULL) {
                    return PDU_COMPOSE_CONTENT_ERROR;
                }

                AppendEncodedString(temp);

                Int32 flen = 0;
                fstart->GetLength(&flen);
                mStack->Pop();
                AppendValueLength(flen);
                mStack->Copy();
            }
            break;
        }

        case IPduHeaders::READ_STATUS:
        case IPduHeaders::STATUS:
        case IPduHeaders::REPORT_ALLOWED:
        case IPduHeaders::PRIORITY:
        case IPduHeaders::DELIVERY_REPORT:
        case IPduHeaders::READ_REPORT:
        case IPduHeaders::RETRIEVE_STATUS: {
            Int32 octet = 0;
            mPduHeader->GetOctet(field, &octet);
            if (0 == octet) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);
            AppendOctet(octet);
            break;
        }

        case IPduHeaders::DATE: {
            Int64 date = 0;
            mPduHeader->GetLongInteger(field, &date);
            if (-1 == date) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);
            AppendDateValue(date);
            break;
        }

        case IPduHeaders::SUBJECT:
        case IPduHeaders::RETRIEVE_TEXT: {
            AutoPtr<IEncodedStringValue> enString;
            mPduHeader->GetEncodedStringValue(field, (IEncodedStringValue**)&enString);
            if (NULL == enString) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);
            AppendEncodedString(enString);
            break;
        }

        case IPduHeaders::MESSAGE_CLASS: {
            AutoPtr<ArrayOf<Byte> > messageClass;
            mPduHeader->GetTextString(field, (ArrayOf<Byte>**)&messageClass);
            if (NULL == messageClass) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);
            if (Arrays::Equals(messageClass,
                    IPduHeaders::MESSAGE_CLASS_ADVERTISEMENT_STR.GetBytes())) {
                AppendOctet(IPduHeaders::MESSAGE_CLASS_ADVERTISEMENT);
            }
            else if (Arrays::Equals(messageClass,
                    IPduHeaders::MESSAGE_CLASS_AUTO_STR.GetBytes())) {
                AppendOctet(IPduHeaders::MESSAGE_CLASS_AUTO);
            }
            else if (Arrays::Equals(messageClass,
                    IPduHeaders::MESSAGE_CLASS_PERSONAL_STR.GetBytes())) {
                AppendOctet(IPduHeaders::MESSAGE_CLASS_PERSONAL);
            }
            else if (Arrays::Equals(messageClass,
                    IPduHeaders::MESSAGE_CLASS_INFORMATIONAL_STR.GetBytes())) {
                AppendOctet(IPduHeaders::MESSAGE_CLASS_INFORMATIONAL);
            }
            else {
                AppendTextString(messageClass);
            }
            break;
        }

        case IPduHeaders::EXPIRY: {
            Int64 expiry = 0;
            mPduHeader->GetLongInteger(field, &expiry);
            if (-1 == expiry) {
                return PDU_COMPOSE_FIELD_NOT_SET;
            }

            AppendOctet(field);

            mStack->Newbuf();
            AutoPtr<PositionMarker> expiryStart;
            mStack->Mark((PositionMarker**)&expiryStart);

            Append(IPduHeaders::VALUE_RELATIVE_TOKEN);
            AppendLongInteger(expiry);

            Int32 expiryLength = 0;
            expiryStart->GetLength(&expiryLength);
            mStack->Pop();
            AppendValueLength(expiryLength);
            mStack->Copy();
            break;
        }

        default:
            return PDU_COMPOSE_FIELD_NOT_SUPPORTED;
    }

    return PDU_COMPOSE_SUCCESS;
}

Int32 PduComposer::MakeReadRecInd()
{
    if (mMessage == NULL) {
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&mMessage);
        mPosition = 0;
    }

    // X-Mms-Message-Type
    AppendOctet(IPduHeaders::MESSAGE_TYPE);
    AppendOctet(IPduHeaders::MESSAGE_TYPE_READ_REC_IND);

    // X-Mms-MMS-Version
    if (AppendHeader(IPduHeaders::MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // Message-ID
    if (AppendHeader(IPduHeaders::MESSAGE_ID) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // To
    if (AppendHeader(IPduHeaders::TO) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // From
    if (AppendHeader(IPduHeaders::FROM) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // Date Optional
    AppendHeader(IPduHeaders::DATE);

    // X-Mms-Read-Status
    if (AppendHeader(IPduHeaders::READ_STATUS) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // X-Mms-Applic-ID Optional(not support)
    // X-Mms-Reply-Applic-ID Optional(not support)
    // X-Mms-Aux-Applic-Info Optional(not support)

    return PDU_COMPOSE_SUCCESS;
}

Int32 PduComposer::MakeNotifyResp()
{
    if (mMessage == NULL) {
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&mMessage);
        mPosition = 0;
    }

    //    X-Mms-Message-Type
    AppendOctet(IPduHeaders::MESSAGE_TYPE);
    AppendOctet(IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND);

    // X-Mms-Transaction-ID
    if (AppendHeader(IPduHeaders::TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // X-Mms-MMS-Version
    if (AppendHeader(IPduHeaders::MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    //  X-Mms-Status
    if (AppendHeader(IPduHeaders::STATUS) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // X-Mms-Report-Allowed Optional (not support)
    return PDU_COMPOSE_SUCCESS;
}

Int32 PduComposer::MakeAckInd()
{
    if (mMessage == NULL) {
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&mMessage);
        mPosition = 0;
    }

    //    X-Mms-Message-Type
    AppendOctet(IPduHeaders::MESSAGE_TYPE);
    AppendOctet(IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND);

    // X-Mms-Transaction-ID
    if (AppendHeader(IPduHeaders::TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    //     X-Mms-MMS-Version
    if (AppendHeader(IPduHeaders::MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // X-Mms-Report-Allowed Optional
    AppendHeader(IPduHeaders::REPORT_ALLOWED);

    return PDU_COMPOSE_SUCCESS;
}

Int32 PduComposer::MakeSendRetrievePdu(
    /* [in] */ Int32 type)
{
    if (mMessage == NULL) {
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&mMessage);
        mPosition = 0;
    }

    // X-Mms-Message-Type
    AppendOctet(IPduHeaders::MESSAGE_TYPE);
    AppendOctet(type);

    // X-Mms-Transaction-ID
    AppendOctet(IPduHeaders::TRANSACTION_ID);

    AutoPtr<ArrayOf<Byte> > trid;
    mPduHeader->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&trid);
    if (trid == NULL) {
        // Transaction-ID should be set(by Transaction) before make().
        // throw new IllegalArgumentException("Transaction-ID is NULL.");
        return -1;
    }
    AppendTextString(trid);

    //  X-Mms-MMS-Version
    if (AppendHeader(IPduHeaders::MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // Date Date-value Optional.
    AppendHeader(IPduHeaders::DATE);

    // From
    if (AppendHeader(IPduHeaders::FROM) != PDU_COMPOSE_SUCCESS) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    Boolean recipient = FALSE;

    // To
    if (AppendHeader(IPduHeaders::TO) != PDU_COMPOSE_CONTENT_ERROR) {
        recipient = true;
    }

    // Cc
    if (AppendHeader(IPduHeaders::CC) != PDU_COMPOSE_CONTENT_ERROR) {
        recipient = true;
    }

    // Bcc
    if (AppendHeader(IPduHeaders::BCC) != PDU_COMPOSE_CONTENT_ERROR) {
        recipient = true;
    }

    // Need at least one of "cc", "bcc" and "to".
    if (FALSE == recipient) {
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    // Subject Optional
    AppendHeader(IPduHeaders::SUBJECT);

    // X-Mms-Message-Class Optional
    // Message-class-value = Class-identifier | Token-text
    AppendHeader(IPduHeaders::MESSAGE_CLASS);

    // X-Mms-Expiry Optional
    AppendHeader(IPduHeaders::EXPIRY);

    // X-Mms-Priority Optional
    AppendHeader(IPduHeaders::PRIORITY);

    // X-Mms-Delivery-Report Optional
    AppendHeader(IPduHeaders::DELIVERY_REPORT);

    // X-Mms-Read-Report Optional
    AppendHeader(IPduHeaders::READ_REPORT);

    if (type == IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF) {
        // X-Mms-Retrieve-Status Optional
        AppendHeader(IPduHeaders::RETRIEVE_STATUS);
        // X-Mms-Retrieve-Text Optional
        AppendHeader(IPduHeaders::RETRIEVE_TEXT);
    }

    //    Content-Type
    AppendOctet(IPduHeaders::CONTENT_TYPE);

    //  Message body
    return MakeMessageBody(type);
}

Int32 PduComposer::MakeMessageBody(
    /* [in] */ Int32 type)
{
    // 1. add body informations
    mStack->Newbuf();  // Switching buffer because we need to

    AutoPtr<PositionMarker> ctStart;
    mStack->Mark((PositionMarker**)&ctStart);

    // This contentTypeIdentifier should be used for type of attachment...
    AutoPtr<ArrayOf<Byte> > arr;
    mPduHeader->GetTextString(IPduHeaders::CONTENT_TYPE, (ArrayOf<Byte>**)&arr);
    String contentType(*arr);
    AutoPtr<IInterface> _contentTypeIdentifier;
    mContentTypeMap->Get(CoreUtils::Convert(contentType), (IInterface**)&_contentTypeIdentifier);
    AutoPtr<IInteger32> contentTypeIdentifier = IInteger32::Probe(_contentTypeIdentifier);
    if (contentTypeIdentifier == NULL) {
        // content type is mandatory
        return PDU_COMPOSE_CONTENT_ERROR;
    }

    Int32 value = 0;
    contentTypeIdentifier->GetValue(&value);
    AppendShortInteger(value);

    // content-type parameter: start
    AutoPtr<IPduBody> body;
    if (type == IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF) {
        // IRetrieveConf::Probe(mPdu)->GetBody((IPduBody**)&body);
        IMultimediaMessagePdu::Probe(mPdu)->GetBody((IPduBody**)&body);
    }
    else {
        // ISendReq::Probe(mPdu)->GetBody((IPduBody**)&body);
        IMultimediaMessagePdu::Probe(mPdu)->GetBody((IPduBody**)&body);
    }
    Int32 num = 0;
    if (NULL == body || (body->GetPartsNum(&num), num) == 0) {
        // empty message
        AppendUintvarInteger(0);
        mStack->Pop();
        mStack->Copy();
        return PDU_COMPOSE_SUCCESS;
    }

    AutoPtr<IPduPart> part;
    // try {
        body->GetPart(0, (IPduPart**)&part);

        AutoPtr<ArrayOf<Byte> > start;
        part->GetContentId((ArrayOf<Byte>**)&start);
        if (start != NULL) {
            AppendOctet(IPduPart::P_DEP_START);
            if (('<' == (*start)[0]) && ('>' == (*start)[start->GetLength() - 1])) {
                AppendTextString(start);
            }
            else {
                AppendTextString(String("<") + String(*start) + String(">"));
            }
        }

        // content-type parameter: type
        AppendOctet(IPduPart::P_CT_MR_TYPE);
        AutoPtr<ArrayOf<Byte> > arrByte;
        part->GetContentType((ArrayOf<Byte>**)&arrByte);
        AppendTextString(arrByte);
    // }
    // catch (ArrayIndexOutOfBoundsException e){
    //     e.printStackTrace();
    // }

    Int32 ctLength = 0;
    ctStart->GetLength(&ctLength);
    mStack->Pop();
    AppendValueLength(ctLength);
    mStack->Copy();

    // 3. add content
    Int32 partNum = 0;
    body->GetPartsNum(&partNum);
    AppendUintvarInteger(partNum);
    for (Int32 i = 0; i < partNum; i++) {
        body->GetPart(i, (IPduPart**)&part);
        mStack->Newbuf();  // Leaving space for header lengh and data length
        AutoPtr<PositionMarker> attachment;
        mStack->Mark((PositionMarker**)&attachment);

        mStack->Newbuf();  // Leaving space for Content-Type length
        AutoPtr<PositionMarker> contentTypeBegin;
        mStack->Mark((PositionMarker**)&contentTypeBegin);

        AutoPtr<ArrayOf<Byte> > partContentType;
        part->GetContentType((ArrayOf<Byte>**)&partContentType);

        if (partContentType == NULL) {
            // content type is mandatory
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // content-type value
        AutoPtr<IInterface> _partContentTypeIdentifier;
        mContentTypeMap->Get(CoreUtils::Convert(String(*partContentType)), (IInterface**)&_partContentTypeIdentifier);
        AutoPtr<IInteger32> partContentTypeIdentifier = IInteger32::Probe(_partContentTypeIdentifier);
        if (partContentTypeIdentifier == NULL) {
            AppendTextString(partContentType);
        }
        else {
            Int32 value = 0;
            partContentTypeIdentifier->GetValue(&value);
            AppendShortInteger(value);
        }

        /* Content-type parameter : name.
         * The value of name, filename, content-location is the same.
         * Just one of them is enough for this PDU.
         */
        AutoPtr<ArrayOf<Byte> > name;
        part->GetName((ArrayOf<Byte>**)&name);

        if (NULL == name) {
            part->GetFilename((ArrayOf<Byte>**)&name);

            if (NULL == name) {
                part->GetContentLocation((ArrayOf<Byte>**)&name);

                if (NULL == name) {
                    part->GetContentId((ArrayOf<Byte>**)&name);

                    if (NULL == name) {
                        // At lease one of name, filename, Content-location, Content id
                        // should be available.
                        return PDU_COMPOSE_CONTENT_ERROR;
                    }
                }
            }
        }
        AppendOctet(IPduPart::P_DEP_NAME);
        AppendTextString(name);

        // content-type parameter : charset
        Int32 charset = 0;
        part->GetCharset(&charset);
        if (charset != 0) {
            AppendOctet(IPduPart::P_CHARSET);
            AppendShortInteger(charset);
        }

        Int32 contentTypeLength = 0;
        contentTypeBegin->GetLength(&contentTypeLength);
        mStack->Pop();
        AppendValueLength(contentTypeLength);
        mStack->Copy();

        // content id
        AutoPtr<ArrayOf<Byte> > contentId;
        part->GetContentId((ArrayOf<Byte>**)&contentId);

        if (NULL != contentId) {
            AppendOctet(IPduPart::P_CONTENT_ID);
            if (('<' == (*contentId)[0]) && ('>' == (*contentId)[contentId->GetLength() - 1])) {
                AppendQuotedString(contentId);
            }
            else {
                AppendQuotedString(String("<") + String(*contentId) + String(">"));
            }
        }

        // content-location
        AutoPtr<ArrayOf<Byte> > contentLocation;
        part->GetContentLocation((ArrayOf<Byte>**)&contentLocation);
        if (NULL != contentLocation) {
        	AppendOctet(IPduPart::P_CONTENT_LOCATION);
        	AppendTextString(contentLocation);
        }

        // content
        Int32 headerLength = 0;
        attachment->GetLength(&headerLength);

        Int32 dataLength = 0; // Just for safety...
        AutoPtr<ArrayOf<Byte> > partData;
        part->GetData((ArrayOf<Byte>**)&partData);

        if (partData != NULL) {
            Arraycopy(partData, 0, partData->GetLength());
            dataLength = partData->GetLength();
        }
        else {
            AutoPtr<IInputStream> cr;
            // try {
                AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(PDU_COMPOSER_BLOCK_SIZE);
                AutoPtr<IUri> dataUri;
                part->GetDataUri((IUri**)&dataUri);
                mResolver->OpenInputStream(dataUri, (IInputStream**)&cr);
                Int32 len = 0;
                while ((cr->Read(buffer, &len), len) != -1) {
                    IOutputStream::Probe(mMessage)->Write(buffer, 0, len);
                    mPosition += len;
                    dataLength += len;
                }
            // } catch (FileNotFoundException e) {
            //     return PDU_COMPOSE_CONTENT_ERROR;
            // } catch (IOException e) {
            //     return PDU_COMPOSE_CONTENT_ERROR;
            // } catch (RuntimeException e) {
            //     return PDU_COMPOSE_CONTENT_ERROR;
            // } finally {
                if (cr != NULL) {
                    // try {
                    cr->Close();
                    // } catch (IOException e) {
                    // }
                }
            // }
        }

        Int32 attachmentLength = 0;
        attachment->GetLength(&attachmentLength);
        if (dataLength != (attachmentLength - headerLength)) {
            // throw new RuntimeException("BUG: Length sanity check failed");
            return -1;
        }

        mStack->Pop();
        AppendUintvarInteger(headerLength);
        AppendUintvarInteger(dataLength);
        mStack->Copy();
    }

    return PDU_COMPOSE_SUCCESS;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
