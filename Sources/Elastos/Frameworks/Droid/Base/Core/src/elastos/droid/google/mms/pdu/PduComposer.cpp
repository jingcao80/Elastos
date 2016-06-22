#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduComposer.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                     PduComposer::PositionMarker
//=====================================================================
ECode PduComposer::PositionMarker::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // If these assert fails, likely that you are finding the
    // // size of buffer that is deep in BufferStack you can only
    // // find the length of the buffer that is on top
    // if (currentStackSize != mStack.stackSize) {
    //     throw new RuntimeException("BUG: Invalid call to getLength()");
    // }
    //
    // return mPosition - c_pos;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                       PduComposer::BufferStack
//=====================================================================
ECode PduComposer::BufferStack::Newbuf()
{
    // ==================before translated======================
    // // You can't create a new buff when toCopy != null
    // // That is after calling pop() and before calling copy()
    // // If you do, it is a bug
    // if (toCopy != null) {
    //     throw new RuntimeException("BUG: Invalid newbuf() before copy()");
    // }
    //
    // LengthRecordNode temp = new LengthRecordNode();
    //
    // temp.currentMessage = mMessage;
    // temp.currentPosition = mPosition;
    //
    // temp.next = stack;
    // stack = temp;
    //
    // stackSize = stackSize + 1;
    //
    // mMessage = new ByteArrayOutputStream();
    // mPosition = 0;
    assert(0);
    return NOERROR;
}

ECode PduComposer::BufferStack::Pop()
{
    // ==================before translated======================
    // ByteArrayOutputStream currentMessage = mMessage;
    // int currentPosition = mPosition;
    //
    // mMessage = stack.currentMessage;
    // mPosition = stack.currentPosition;
    //
    // toCopy = stack;
    // // Re using the top element of the stack to avoid memory allocation
    //
    // stack = stack.next;
    // stackSize = stackSize - 1;
    //
    // toCopy.currentMessage = currentMessage;
    // toCopy.currentPosition = currentPosition;
    assert(0);
    return NOERROR;
}

ECode PduComposer::BufferStack::Copy()
{
    // ==================before translated======================
    // arraycopy(toCopy.currentMessage.toByteArray(), 0,
    //         toCopy.currentPosition);
    //
    // toCopy = null;
    assert(0);
    return NOERROR;
}

ECode PduComposer::BufferStack::Mark(
    /* [out] */ PositionMarker** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // PositionMarker m = new PositionMarker();
    //
    // m.c_pos = mPosition;
    // m.currentStackSize = stackSize;
    //
    // return m;
    assert(0);
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
    // ==================before translated======================
    // mPdu = pdu;
    // mResolver = context.getContentResolver();
    // mPduHeader = pdu.getPduHeaders();
    // mStack = new BufferStack();
    // mMessage = new ByteArrayOutputStream();
    // mPosition = 0;
    return NOERROR;
}

ECode PduComposer::Make(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Get Message-type.
    // int type = mPdu.getMessageType();
    //
    // /* make the message */
    // switch (type) {
    //     case PduHeaders.MESSAGE_TYPE_SEND_REQ:
    //     case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
    //         if (makeSendRetrievePdu(type) != PDU_COMPOSE_SUCCESS) {
    //             return null;
    //         }
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
    //         if (makeNotifyResp() != PDU_COMPOSE_SUCCESS) {
    //             return null;
    //         }
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
    //         if (makeAckInd() != PDU_COMPOSE_SUCCESS) {
    //             return null;
    //         }
    //         break;
    //     case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
    //         if (makeReadRecInd() != PDU_COMPOSE_SUCCESS) {
    //             return null;
    //         }
    //         break;
    //     default:
    //         return null;
    // }
    //
    // return mMessage.toByteArray();
    assert(0);
    return NOERROR;
}

void PduComposer::Arraycopy(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // mMessage.write(buf, pos, length);
    // mPosition = mPosition + length;
    assert(0);
}

void PduComposer::Append(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mMessage.write(value);
    // mPosition ++;
    assert(0);
}

void PduComposer::AppendShortInteger(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Short-integer = OCTET
    //  * ; Integers in range 0-127 shall be encoded as a one octet value
    //  * ; with the most significant bit set to one (1xxx xxxx) and with
    //  * ; the value in the remaining least significant bits.
    //  * In our implementation, only low 7 bits are stored and otherwise
    //  * bits are ignored.
    //  */
    // append((value | 0x80) & 0xff);
    assert(0);
}

void PduComposer::AppendOctet(
    /* [in] */ Int32 number)
{
    // ==================before translated======================
    // append(number);
    assert(0);
}

void PduComposer::AppendShortLength(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Short-length = <Any octet 0-30>
    //  */
    // append(value);
    assert(0);
}

void PduComposer::AppendLongInteger(
    /* [in] */ Int64 longInt)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Long-integer = Short-length Multi-octet-integer
    //  * ; The Short-length indicates the length of the Multi-octet-integer
    //  * Multi-octet-integer = 1*30 OCTET
    //  * ; The content octets shall be an unsigned integer value with the
    //  * ; most significant octet encoded first (big-endian representation).
    //  * ; The minimum number of octets must be used to encode the value.
    //  */
    // int size;
    // long temp = longInt;
    //
    // // Count the length of the long integer.
    // for(size = 0; (temp != 0) && (size < LONG_INTEGER_LENGTH_MAX); size++) {
    //     temp = (temp >>> 8);
    // }
    //
    // // Set Length.
    // appendShortLength(size);
    //
    // // Count and set the long integer.
    // int i;
    // int shift = (size -1) * 8;
    //
    // for (i = 0; i < size; i++) {
    //     append((int)((longInt >>> shift) & 0xff));
    //     shift = shift - 8;
    // }
    assert(0);
}

void PduComposer::AppendTextString(
    /* [in] */ ArrayOf<Byte>* text)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Text-string = [Quote] *TEXT End-of-string
    //  * ; If the first character in the TEXT is in the range of 128-255,
    //  * ; a Quote character must precede it. Otherwise the Quote character
    //  * ;must be omitted. The Quote is not part of the contents.
    //  */
    // if (((text[0])&0xff) > TEXT_MAX) { // No need to check for <= 255
    //     append(TEXT_MAX);
    // }
    //
    // arraycopy(text, 0, text.length);
    // append(0);
    assert(0);
}

void PduComposer::AppendTextString(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Text-string = [Quote] *TEXT End-of-string
    //  * ; If the first character in the TEXT is in the range of 128-255,
    //  * ; a Quote character must precede it. Otherwise the Quote character
    //  * ;must be omitted. The Quote is not part of the contents.
    //  */
    // appendTextString(str.getBytes());
    assert(0);
}

void PduComposer::AppendEncodedString(
    /* [in] */ IEncodedStringValue* enStr)
{
    // ==================before translated======================
    // /*
    //  * From OMA-TS-MMS-ENC-V1_3-20050927-C:
    //  * Encoded-string-value = Text-string | Value-length Char-set Text-string
    //  */
    // assert(enStr != null);
    //
    // int charset = enStr.getCharacterSet();
    // byte[] textString = enStr.getTextString();
    // if (null == textString) {
    //     return;
    // }
    //
    // /*
    //  * In the implementation of EncodedStringValue, the charset field will
    //  * never be 0. It will always be composed as
    //  * Encoded-string-value = Value-length Char-set Text-string
    //  */
    // mStack.newbuf();
    // PositionMarker start = mStack.mark();
    //
    // appendShortInteger(charset);
    // appendTextString(textString);
    //
    // int len = start.getLength();
    // mStack.pop();
    // appendValueLength(len);
    // mStack.copy();
    assert(0);
}

void PduComposer::AppendUintvarInteger(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * To encode a large unsigned integer, split it into 7-bit fragments
    //  * and place them in the payloads of multiple octets. The most significant
    //  * bits are placed in the first octets with the least significant bits
    //  * ending up in the last octet. All octets MUST set the Continue bit to 1
    //  * except the last octet, which MUST set the Continue bit to 0.
    //  */
    // int i;
    // long max = SHORT_INTEGER_MAX;
    //
    // for (i = 0; i < 5; i++) {
    //     if (value < max) {
    //         break;
    //     }
    //
    //     max = (max << 7) | 0x7fl;
    // }
    //
    // while(i > 0) {
    //     long temp = value >>> (i * 7);
    //     temp = temp & 0x7f;
    //
    //     append((int)((temp | 0x80) & 0xff));
    //
    //     i--;
    // }
    //
    // append((int)(value & 0x7f));
    assert(0);
}

void PduComposer::AppendDateValue(
    /* [in] */ Int64 date)
{
    // ==================before translated======================
    // /*
    //  * From OMA-TS-MMS-ENC-V1_3-20050927-C:
    //  * Date-value = Long-integer
    //  */
    // appendLongInteger(date);
    assert(0);
}

void PduComposer::AppendValueLength(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Value-length = Short-length | (Length-quote Length)
    //  * ; Value length is used to indicate the length of the value to follow
    //  * Short-length = <Any octet 0-30>
    //  * Length-quote = <Octet 31>
    //  * Length = Uintvar-integer
    //  */
    // if (value < LENGTH_QUOTE) {
    //     appendShortLength((int) value);
    //     return;
    // }
    //
    // append(LENGTH_QUOTE);
    // appendUintvarInteger(value);
    assert(0);
}

void PduComposer::AppendQuotedString(
    /* [in] */ ArrayOf<Byte>* text)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Quoted-string = <Octet 34> *TEXT End-of-string
    //  * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
    //  * ;quotation-marks <"> removed.
    //  */
    // append(QUOTED_STRING_FLAG);
    // arraycopy(text, 0, text.length);
    // append(END_STRING_FLAG);
    assert(0);
}

void PduComposer::AppendQuotedString(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // /*
    //  * From WAP-230-WSP-20010705-a:
    //  * Quoted-string = <Octet 34> *TEXT End-of-string
    //  * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
    //  * ;quotation-marks <"> removed.
    //  */
    // appendQuotedString(str.getBytes());
    assert(0);
}

Int32 PduComposer::CheckAddressType(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // /**
    //  * From OMA-TS-MMS-ENC-V1_3-20050927-C.pdf, section 8.
    //  * address = ( e-mail / device-address / alphanum-shortcode / num-shortcode)
    //  * e-mail = mailbox; to the definition of mailbox as described in
    //  * section 3.4 of [RFC2822], but excluding the
    //  * obsolete definitions as indicated by the "obs-" prefix.
    //  * device-address = ( global-phone-number "/TYPE=PLMN" )
    //  * / ( ipv4 "/TYPE=IPv4" ) / ( ipv6 "/TYPE=IPv6" )
    //  * / ( escaped-value "/TYPE=" address-type )
    //  *
    //  * global-phone-number = ["+"] 1*( DIGIT / written-sep )
    //  * written-sep =("-"/".")
    //  *
    //  * ipv4 = 1*3DIGIT 3( "." 1*3DIGIT ) ; IPv4 address value
    //  *
    //  * ipv6 = 4HEXDIG 7( ":" 4HEXDIG ) ; IPv6 address per RFC 2373
    //  */
    //
    // if (null == address) {
    //     return PDU_UNKNOWN_ADDRESS_TYPE;
    // }
    //
    // if (address.matches(REGEXP_IPV4_ADDRESS_TYPE)) {
    //     // Ipv4 address.
    //     return PDU_IPV4_ADDRESS_TYPE;
    // }else if (address.matches(REGEXP_PHONE_NUMBER_ADDRESS_TYPE)) {
    //     // Phone number.
    //     return PDU_PHONE_NUMBER_ADDRESS_TYPE;
    // } else if (address.matches(REGEXP_EMAIL_ADDRESS_TYPE)) {
    //     // Email address.
    //     return PDU_EMAIL_ADDRESS_TYPE;
    // } else if (address.matches(REGEXP_IPV6_ADDRESS_TYPE)) {
    //     // Ipv6 address.
    //     return PDU_IPV6_ADDRESS_TYPE;
    // } else {
    //     // Unknown address.
    //     return PDU_UNKNOWN_ADDRESS_TYPE;
    // }
    assert(0);
    return 0;
}

AutoPtr<IEncodedStringValue> PduComposer::AppendAddressType(
    /* [in] */ IEncodedStringValue* address)
{
    // ==================before translated======================
    // EncodedStringValue temp = null;
    //
    // try {
    //     int addressType = checkAddressType(address.getString());
    //     temp = EncodedStringValue.copy(address);
    //     if (PDU_PHONE_NUMBER_ADDRESS_TYPE == addressType) {
    //         // Phone number.
    //         temp.appendTextString(STRING_PHONE_NUMBER_ADDRESS_TYPE.getBytes());
    //     } else if (PDU_IPV4_ADDRESS_TYPE == addressType) {
    //         // Ipv4 address.
    //         temp.appendTextString(STRING_IPV4_ADDRESS_TYPE.getBytes());
    //     } else if (PDU_IPV6_ADDRESS_TYPE == addressType) {
    //         // Ipv6 address.
    //         temp.appendTextString(STRING_IPV6_ADDRESS_TYPE.getBytes());
    //     }
    // } catch (NullPointerException e) {
    //     return null;
    // }
    //
    // return temp;
    assert(0);
    AutoPtr<IEncodedStringValue> empty;
    return empty;
}

Int32 PduComposer::AppendHeader(
    /* [in] */ Int32 field)
{
    // ==================before translated======================
    // switch (field) {
    //     case PduHeaders.MMS_VERSION:
    //         appendOctet(field);
    //
    //         int version = mPduHeader.getOctet(field);
    //         if (0 == version) {
    //             appendShortInteger(PduHeaders.CURRENT_MMS_VERSION);
    //         } else {
    //             appendShortInteger(version);
    //         }
    //
    //         break;
    //
    //     case PduHeaders.MESSAGE_ID:
    //     case PduHeaders.TRANSACTION_ID:
    //         byte[] textString = mPduHeader.getTextString(field);
    //         if (null == textString) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //         appendTextString(textString);
    //         break;
    //
    //     case PduHeaders.TO:
    //     case PduHeaders.BCC:
    //     case PduHeaders.CC:
    //         EncodedStringValue[] addr = mPduHeader.getEncodedStringValues(field);
    //
    //         if (null == addr) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         EncodedStringValue temp;
    //         for (int i = 0; i < addr.length; i++) {
    //             temp = appendAddressType(addr[i]);
    //             if (temp == null) {
    //                 return PDU_COMPOSE_CONTENT_ERROR;
    //             }
    //
    //             appendOctet(field);
    //             appendEncodedString(temp);
    //         }
    //         break;
    //
    //     case PduHeaders.FROM:
    //         // Value-length (Address-present-token Encoded-string-value | Insert-address-token)
    //         appendOctet(field);
    //
    //         EncodedStringValue from = mPduHeader.getEncodedStringValue(field);
    //         if ((from == null)
    //                 || TextUtils.isEmpty(from.getString())
    //                 || new String(from.getTextString()).equals(
    //                         PduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR)) {
    //             // Length of from = 1
    //             append(1);
    //             // Insert-address-token = <Octet 129>
    //             append(PduHeaders.FROM_INSERT_ADDRESS_TOKEN);
    //         } else {
    //             mStack.newbuf();
    //             PositionMarker fstart = mStack.mark();
    //
    //             // Address-present-token = <Octet 128>
    //             append(PduHeaders.FROM_ADDRESS_PRESENT_TOKEN);
    //
    //             temp = appendAddressType(from);
    //             if (temp == null) {
    //                 return PDU_COMPOSE_CONTENT_ERROR;
    //             }
    //
    //             appendEncodedString(temp);
    //
    //             int flen = fstart.getLength();
    //             mStack.pop();
    //             appendValueLength(flen);
    //             mStack.copy();
    //         }
    //         break;
    //
    //     case PduHeaders.READ_STATUS:
    //     case PduHeaders.STATUS:
    //     case PduHeaders.REPORT_ALLOWED:
    //     case PduHeaders.PRIORITY:
    //     case PduHeaders.DELIVERY_REPORT:
    //     case PduHeaders.READ_REPORT:
    //     case PduHeaders.RETRIEVE_STATUS:
    //         int octet = mPduHeader.getOctet(field);
    //         if (0 == octet) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //         appendOctet(octet);
    //         break;
    //
    //     case PduHeaders.DATE:
    //         long date = mPduHeader.getLongInteger(field);
    //         if (-1 == date) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //         appendDateValue(date);
    //         break;
    //
    //     case PduHeaders.SUBJECT:
    //     case PduHeaders.RETRIEVE_TEXT:
    //         EncodedStringValue enString =
    //             mPduHeader.getEncodedStringValue(field);
    //         if (null == enString) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //         appendEncodedString(enString);
    //         break;
    //
    //     case PduHeaders.MESSAGE_CLASS:
    //         byte[] messageClass = mPduHeader.getTextString(field);
    //         if (null == messageClass) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //         if (Arrays.equals(messageClass,
    //                 PduHeaders.MESSAGE_CLASS_ADVERTISEMENT_STR.getBytes())) {
    //             appendOctet(PduHeaders.MESSAGE_CLASS_ADVERTISEMENT);
    //         } else if (Arrays.equals(messageClass,
    //                 PduHeaders.MESSAGE_CLASS_AUTO_STR.getBytes())) {
    //             appendOctet(PduHeaders.MESSAGE_CLASS_AUTO);
    //         } else if (Arrays.equals(messageClass,
    //                 PduHeaders.MESSAGE_CLASS_PERSONAL_STR.getBytes())) {
    //             appendOctet(PduHeaders.MESSAGE_CLASS_PERSONAL);
    //         } else if (Arrays.equals(messageClass,
    //                 PduHeaders.MESSAGE_CLASS_INFORMATIONAL_STR.getBytes())) {
    //             appendOctet(PduHeaders.MESSAGE_CLASS_INFORMATIONAL);
    //         } else {
    //             appendTextString(messageClass);
    //         }
    //         break;
    //
    //     case PduHeaders.EXPIRY:
    //         long expiry = mPduHeader.getLongInteger(field);
    //         if (-1 == expiry) {
    //             return PDU_COMPOSE_FIELD_NOT_SET;
    //         }
    //
    //         appendOctet(field);
    //
    //         mStack.newbuf();
    //         PositionMarker expiryStart = mStack.mark();
    //
    //         append(PduHeaders.VALUE_RELATIVE_TOKEN);
    //         appendLongInteger(expiry);
    //
    //         int expiryLength = expiryStart.getLength();
    //         mStack.pop();
    //         appendValueLength(expiryLength);
    //         mStack.copy();
    //         break;
    //
    //     default:
    //         return PDU_COMPOSE_FIELD_NOT_SUPPORTED;
    // }
    //
    // return PDU_COMPOSE_SUCCESS;
    assert(0);
    return 0;
}

Int32 PduComposer::MakeReadRecInd()
{
    // ==================before translated======================
    // if (mMessage == null) {
    //     mMessage = new ByteArrayOutputStream();
    //     mPosition = 0;
    // }
    //
    // // X-Mms-Message-Type
    // appendOctet(PduHeaders.MESSAGE_TYPE);
    // appendOctet(PduHeaders.MESSAGE_TYPE_READ_REC_IND);
    //
    // // X-Mms-MMS-Version
    // if (appendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // Message-ID
    // if (appendHeader(PduHeaders.MESSAGE_ID) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // To
    // if (appendHeader(PduHeaders.TO) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // From
    // if (appendHeader(PduHeaders.FROM) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // Date Optional
    // appendHeader(PduHeaders.DATE);
    //
    // // X-Mms-Read-Status
    // if (appendHeader(PduHeaders.READ_STATUS) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // X-Mms-Applic-ID Optional(not support)
    // // X-Mms-Reply-Applic-ID Optional(not support)
    // // X-Mms-Aux-Applic-Info Optional(not support)
    //
    // return PDU_COMPOSE_SUCCESS;
    assert(0);
    return 0;
}

Int32 PduComposer::MakeNotifyResp()
{
    // ==================before translated======================
    // if (mMessage == null) {
    //     mMessage = new ByteArrayOutputStream();
    //     mPosition = 0;
    // }
    //
    // //    X-Mms-Message-Type
    // appendOctet(PduHeaders.MESSAGE_TYPE);
    // appendOctet(PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND);
    //
    // // X-Mms-Transaction-ID
    // if (appendHeader(PduHeaders.TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // X-Mms-MMS-Version
    // if (appendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // //  X-Mms-Status
    // if (appendHeader(PduHeaders.STATUS) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // X-Mms-Report-Allowed Optional (not support)
    // return PDU_COMPOSE_SUCCESS;
    assert(0);
    return 0;
}

Int32 PduComposer::MakeAckInd()
{
    // ==================before translated======================
    // if (mMessage == null) {
    //     mMessage = new ByteArrayOutputStream();
    //     mPosition = 0;
    // }
    //
    // //    X-Mms-Message-Type
    // appendOctet(PduHeaders.MESSAGE_TYPE);
    // appendOctet(PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND);
    //
    // // X-Mms-Transaction-ID
    // if (appendHeader(PduHeaders.TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // //     X-Mms-MMS-Version
    // if (appendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // X-Mms-Report-Allowed Optional
    // appendHeader(PduHeaders.REPORT_ALLOWED);
    //
    // return PDU_COMPOSE_SUCCESS;
    assert(0);
    return 0;
}

Int32 PduComposer::MakeSendRetrievePdu(
    /* [in] */ Int32 type)
{
    // ==================before translated======================
    // if (mMessage == null) {
    //     mMessage = new ByteArrayOutputStream();
    //     mPosition = 0;
    // }
    //
    // // X-Mms-Message-Type
    // appendOctet(PduHeaders.MESSAGE_TYPE);
    // appendOctet(type);
    //
    // // X-Mms-Transaction-ID
    // appendOctet(PduHeaders.TRANSACTION_ID);
    //
    // byte[] trid = mPduHeader.getTextString(PduHeaders.TRANSACTION_ID);
    // if (trid == null) {
    //     // Transaction-ID should be set(by Transaction) before make().
    //     throw new IllegalArgumentException("Transaction-ID is null.");
    // }
    // appendTextString(trid);
    //
    // //  X-Mms-MMS-Version
    // if (appendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // Date Date-value Optional.
    // appendHeader(PduHeaders.DATE);
    //
    // // From
    // if (appendHeader(PduHeaders.FROM) != PDU_COMPOSE_SUCCESS) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // boolean recipient = false;
    //
    // // To
    // if (appendHeader(PduHeaders.TO) != PDU_COMPOSE_CONTENT_ERROR) {
    //     recipient = true;
    // }
    //
    // // Cc
    // if (appendHeader(PduHeaders.CC) != PDU_COMPOSE_CONTENT_ERROR) {
    //     recipient = true;
    // }
    //
    // // Bcc
    // if (appendHeader(PduHeaders.BCC) != PDU_COMPOSE_CONTENT_ERROR) {
    //     recipient = true;
    // }
    //
    // // Need at least one of "cc", "bcc" and "to".
    // if (false == recipient) {
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // // Subject Optional
    // appendHeader(PduHeaders.SUBJECT);
    //
    // // X-Mms-Message-Class Optional
    // // Message-class-value = Class-identifier | Token-text
    // appendHeader(PduHeaders.MESSAGE_CLASS);
    //
    // // X-Mms-Expiry Optional
    // appendHeader(PduHeaders.EXPIRY);
    //
    // // X-Mms-Priority Optional
    // appendHeader(PduHeaders.PRIORITY);
    //
    // // X-Mms-Delivery-Report Optional
    // appendHeader(PduHeaders.DELIVERY_REPORT);
    //
    // // X-Mms-Read-Report Optional
    // appendHeader(PduHeaders.READ_REPORT);
    //
    // if (type == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF) {
    //     // X-Mms-Retrieve-Status Optional
    //     appendHeader(PduHeaders.RETRIEVE_STATUS);
    //     // X-Mms-Retrieve-Text Optional
    //     appendHeader(PduHeaders.RETRIEVE_TEXT);
    // }
    //
    // //    Content-Type
    // appendOctet(PduHeaders.CONTENT_TYPE);
    //
    // //  Message body
    // return makeMessageBody(type);
    assert(0);
    return 0;
}

Int32 PduComposer::MakeMessageBody(
    /* [in] */ Int32 type)
{
    // ==================before translated======================
    // // 1. add body informations
    // mStack.newbuf();  // Switching buffer because we need to
    //
    // PositionMarker ctStart = mStack.mark();
    //
    // // This contentTypeIdentifier should be used for type of attachment...
    // String contentType = new String(mPduHeader.getTextString(PduHeaders.CONTENT_TYPE));
    // Integer contentTypeIdentifier = mContentTypeMap.get(contentType);
    // if (contentTypeIdentifier == null) {
    //     // content type is mandatory
    //     return PDU_COMPOSE_CONTENT_ERROR;
    // }
    //
    // appendShortInteger(contentTypeIdentifier.intValue());
    //
    // // content-type parameter: start
    // PduBody body;
    // if (type == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF) {
    //     body = ((RetrieveConf) mPdu).getBody();
    // } else {
    //     body = ((SendReq) mPdu).getBody();
    // }
    // if (null == body || body.getPartsNum() == 0) {
    //     // empty message
    //     appendUintvarInteger(0);
    //     mStack.pop();
    //     mStack.copy();
    //     return PDU_COMPOSE_SUCCESS;
    // }
    //
    // PduPart part;
    // try {
    //     part = body.getPart(0);
    //
    //     byte[] start = part.getContentId();
    //     if (start != null) {
    //         appendOctet(PduPart.P_DEP_START);
    //         if (('<' == start[0]) && ('>' == start[start.length - 1])) {
    //             appendTextString(start);
    //         } else {
    //             appendTextString("<" + new String(start) + ">");
    //         }
    //     }
    //
    //     // content-type parameter: type
    //     appendOctet(PduPart.P_CT_MR_TYPE);
    //     appendTextString(part.getContentType());
    // }
    // catch (ArrayIndexOutOfBoundsException e){
    //     e.printStackTrace();
    // }
    //
    // int ctLength = ctStart.getLength();
    // mStack.pop();
    // appendValueLength(ctLength);
    // mStack.copy();
    //
    // // 3. add content
    // int partNum = body.getPartsNum();
    // appendUintvarInteger(partNum);
    // for (int i = 0; i < partNum; i++) {
    //     part = body.getPart(i);
    //     mStack.newbuf();  // Leaving space for header lengh and data length
    //     PositionMarker attachment = mStack.mark();
    //
    //     mStack.newbuf();  // Leaving space for Content-Type length
    //     PositionMarker contentTypeBegin = mStack.mark();
    //
    //     byte[] partContentType = part.getContentType();
    //
    //     if (partContentType == null) {
    //         // content type is mandatory
    //         return PDU_COMPOSE_CONTENT_ERROR;
    //     }
    //
    //     // content-type value
    //     Integer partContentTypeIdentifier =
    //         mContentTypeMap.get(new String(partContentType));
    //     if (partContentTypeIdentifier == null) {
    //         appendTextString(partContentType);
    //     } else {
    //         appendShortInteger(partContentTypeIdentifier.intValue());
    //     }
    //
    //     /* Content-type parameter : name.
    //      * The value of name, filename, content-location is the same.
    //      * Just one of them is enough for this PDU.
    //      */
    //     byte[] name = part.getName();
    //
    //     if (null == name) {
    //         name = part.getFilename();
    //
    //         if (null == name) {
    //             name = part.getContentLocation();
    //
    //             if (null == name) {
    //                 name = part.getContentId();
    //
    //                 if (null == name) {
    //                     // At lease one of name, filename, Content-location, Content id
    //                     // should be available.
    //                     return PDU_COMPOSE_CONTENT_ERROR;
    //                 }
    //             }
    //         }
    //     }
    //     appendOctet(PduPart.P_DEP_NAME);
    //     appendTextString(name);
    //
    //     // content-type parameter : charset
    //     int charset = part.getCharset();
    //     if (charset != 0) {
    //         appendOctet(PduPart.P_CHARSET);
    //         appendShortInteger(charset);
    //     }
    //
    //     int contentTypeLength = contentTypeBegin.getLength();
    //     mStack.pop();
    //     appendValueLength(contentTypeLength);
    //     mStack.copy();
    //
    //     // content id
    //     byte[] contentId = part.getContentId();
    //
    //     if (null != contentId) {
    //         appendOctet(PduPart.P_CONTENT_ID);
    //         if (('<' == contentId[0]) && ('>' == contentId[contentId.length - 1])) {
    //             appendQuotedString(contentId);
    //         } else {
    //             appendQuotedString("<" + new String(contentId) + ">");
    //         }
    //     }
    //
    //     // content-location
    //     byte[] contentLocation = part.getContentLocation();
    //     if (null != contentLocation) {
    //     	appendOctet(PduPart.P_CONTENT_LOCATION);
    //     	appendTextString(contentLocation);
    //     }
    //
    //     // content
    //     int headerLength = attachment.getLength();
    //
    //     int dataLength = 0; // Just for safety...
    //     byte[] partData = part.getData();
    //
    //     if (partData != null) {
    //         arraycopy(partData, 0, partData.length);
    //         dataLength = partData.length;
    //     } else {
    //         InputStream cr = null;
    //         try {
    //             byte[] buffer = new byte[PDU_COMPOSER_BLOCK_SIZE];
    //             cr = mResolver.openInputStream(part.getDataUri());
    //             int len = 0;
    //             while ((len = cr.read(buffer)) != -1) {
    //                 mMessage.write(buffer, 0, len);
    //                 mPosition += len;
    //                 dataLength += len;
    //             }
    //         } catch (FileNotFoundException e) {
    //             return PDU_COMPOSE_CONTENT_ERROR;
    //         } catch (IOException e) {
    //             return PDU_COMPOSE_CONTENT_ERROR;
    //         } catch (RuntimeException e) {
    //             return PDU_COMPOSE_CONTENT_ERROR;
    //         } finally {
    //             if (cr != null) {
    //                 try {
    //                     cr.close();
    //                 } catch (IOException e) {
    //                 }
    //             }
    //         }
    //     }
    //
    //     if (dataLength != (attachment.getLength() - headerLength)) {
    //         throw new RuntimeException("BUG: Length sanity check failed");
    //     }
    //
    //     mStack.pop();
    //     appendUintvarInteger(headerLength);
    //     appendUintvarInteger(dataLength);
    //     mStack.copy();
    // }
    //
    // return PDU_COMPOSE_SUCCESS;
    assert(0);
    return 0;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
