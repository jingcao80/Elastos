#include "elastos/droid/internal/telephony/gsm/CGsmSmsCbMessage.h"
#include "elastos/droid/internal/telephony/gsm/CSmsCbHeader.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/telephony/CSmsCbMessage.h"
#include "elastos/droid/utility/CPair.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::Telephony::CSmsCbMessage;
using Elastos::Droid::Utility::CPair;
using Elastos::Core::CoreUtils;
using Elastos::Core::IByte;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

static AutoPtr<ArrayOf<String> > InitGROUP0() {
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(16);

    (*array)[0] = String("de");
    (*array)[1] = String("en");
    (*array)[2] = String("it");
    (*array)[3] = String("fr");
    (*array)[4] = String("es");
    (*array)[5] = String("nl");
    (*array)[6] = String("sv");
    (*array)[7] = String("da");
    (*array)[8] = String("pt");
    (*array)[9] = String("fi");
    (*array)[10] = String("no");
    (*array)[11] = String("el");
    (*array)[12] = String("tr");
    (*array)[13] = String("hu");
    (*array)[14] = String("pl");
    (*array)[15] = String(NULL);

    return array;
}

AutoPtr<ArrayOf<String> > CGsmSmsCbMessage::LANGUAGE_CODES_GROUP_0 = InitGROUP0();

static AutoPtr<ArrayOf<String> > InitGROUP2() {
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(16);

    (*array)[0] = String("cs");
    (*array)[1] = String("he");
    (*array)[2] = String("ar");
    (*array)[3] = String("ru");
    (*array)[4] = String("is");
    (*array)[5] = String(NULL);
    (*array)[6] = String(NULL);
    (*array)[7] = String(NULL);
    (*array)[8] = String(NULL);
    (*array)[9] = String(NULL);
    (*array)[10] = String(NULL);
    (*array)[11] = String(NULL);
    (*array)[12] = String(NULL);
    (*array)[13] = String(NULL);
    (*array)[14] = String(NULL);
    (*array)[15] = String(NULL);

    return array;
}

AutoPtr<ArrayOf<String> > CGsmSmsCbMessage::LANGUAGE_CODES_GROUP_2 = InitGROUP2();

const Char32 CGsmSmsCbMessage::CARRIAGE_RETURN;

const Int32 CGsmSmsCbMessage::PDU_BODY_PAGE_LENGTH;

CAR_INTERFACE_IMPL(CGsmSmsCbMessage, Object, IGsmSmsCbMessage)

CAR_OBJECT_IMPL(CGsmSmsCbMessage)

ECode CGsmSmsCbMessage::constructor()
{
    return NOERROR;
}

ECode CGsmSmsCbMessage::CreateSmsCbMessage(
    /* [in] */ ISmsCbHeader* header,
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ IArrayOf* pdus,
    /* [out] */ ISmsCbMessage** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if (header->IsEtwsPrimaryNotification(&b), b) {
        Int32 scope;
        header->GetGeographicalScope(&scope);
        Int32 number;
        header->GetSerialNumber(&number);
        Int32 category;
        header->GetServiceCategory(&category);
        AutoPtr<ISmsCbEtwsInfo> etwsInfo;
        header->GetEtwsInfo((ISmsCbEtwsInfo**)&etwsInfo);
        AutoPtr<ISmsCbCmasInfo> cmasInfo;
        header->GetCmasInfo((ISmsCbCmasInfo**)&cmasInfo);
        return CSmsCbMessage::New(ISmsCbMessage::MESSAGE_FORMAT_3GPP,
                scope, number, location, category,
                String(NULL), String("ETWS"), ISmsCbMessage::MESSAGE_PRIORITY_EMERGENCY,
                etwsInfo, cmasInfo, result);
    }
    else {
        String language;
        StringBuilder sb;

        Int32 pdusSize;
        pdus->GetLength(&pdusSize);
        for (Int32 i = 0; i < pdusSize; i++) {
            AutoPtr<IInterface> obj;
            pdus->Get(i, (IInterface**)&obj);
            AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);
            Int32 size;
            iarray->GetLength(&size);
            AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);

            for (Int32 j = 0; j < size; j++) {
                AutoPtr<IInterface> obj;
                pdus->Get(j, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*pdu)[j]);
            }

            AutoPtr<IPair> p;
            ParseBody(header, pdu, (IPair**)&p);
            obj = NULL;
            p->GetFirst((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&language);
            obj = NULL;
            p->GetSecond((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            sb.Append(str);
        }
        Boolean b;
        Int32 priority = (header->IsEmergencyMessage(&b), b) ? ISmsCbMessage::MESSAGE_PRIORITY_EMERGENCY
                : ISmsCbMessage::MESSAGE_PRIORITY_NORMAL;

        Int32 scope;
        header->GetGeographicalScope(&scope);
        Int32 number;
        header->GetSerialNumber(&number);
        Int32 category;
        header->GetServiceCategory(&category);
        AutoPtr<ISmsCbEtwsInfo> etwsInfo;
        header->GetEtwsInfo((ISmsCbEtwsInfo**)&etwsInfo);
        AutoPtr<ISmsCbCmasInfo> cmasInfo;
        header->GetCmasInfo((ISmsCbCmasInfo**)&cmasInfo);
        String str;
        return CSmsCbMessage::New(ISmsCbMessage::MESSAGE_FORMAT_3GPP,
                scope, number, location,
                category, language, (sb.ToString(&str), str), priority,
                etwsInfo, cmasInfo, result);
    }
    return NOERROR;
}

ECode CGsmSmsCbMessage::CreateSmsCbMessage(
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ IArrayOf* pdus,
    /* [out] */ ISmsCbMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    pdus->Get(0, (IInterface**)&obj);
    AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);
    Int32 size;
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        pdus->Get(0, (IInterface**)&obj);
        IByte::Probe(obj)->GetValue(&(*bytes)[i]);
    }
    AutoPtr<ISmsCbHeader> header;
    CSmsCbHeader::New(bytes, (ISmsCbHeader**)&header);

    return CreateSmsCbMessage(header, location, pdus, result);
}

ECode CGsmSmsCbMessage::ParseBody(
    /* [in] */ ISmsCbHeader* header,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ IPair** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 encoding;
    String language;
    Boolean hasLanguageIndicator = FALSE;
    Int32 dataCodingScheme;
    header->GetDataCodingScheme(&dataCodingScheme);

    // Extract encoding and language from DCS, as defined in 3gpp TS 23.038,
    // section 5.
    switch ((dataCodingScheme & 0xf0) >> 4) {
        case 0x00:
            encoding = ISmsConstants::ENCODING_7BIT;
            language = (*LANGUAGE_CODES_GROUP_0)[dataCodingScheme & 0x0f];
            break;

        case 0x01:
            hasLanguageIndicator = true;
            if ((dataCodingScheme & 0x0f) == 0x01) {
                encoding = ISmsConstants::ENCODING_16BIT;
            }
            else {
                encoding = ISmsConstants::ENCODING_7BIT;
            }
            break;

        case 0x02:
            encoding = ISmsConstants::ENCODING_7BIT;
            language = (*LANGUAGE_CODES_GROUP_2)[dataCodingScheme & 0x0f];
            break;

        case 0x03:
            encoding = ISmsConstants::ENCODING_7BIT;
            break;

        case 0x04:
        case 0x05:
            switch ((dataCodingScheme & 0x0c) >> 2) {
                case 0x01:
                    encoding = ISmsConstants::ENCODING_8BIT;
                    break;

                case 0x02:
                    encoding = ISmsConstants::ENCODING_16BIT;
                    break;

                case 0x00:
                default:
                    encoding = ISmsConstants::ENCODING_7BIT;
                    break;
            }
            break;

        case 0x06:
        case 0x07:
            // Compression not supported
        case 0x09:
            // UDH structure not supported
        case 0x0e:
            // Defined by the WAP forum not supported
            // throw new IllegalArgumentException("Unsupported GSM dataCodingScheme "
            //         + dataCodingScheme);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;

        case 0x0f:
            if (((dataCodingScheme & 0x04) >> 2) == 0x01) {
                encoding = ISmsConstants::ENCODING_8BIT;
            }
            else {
                encoding = ISmsConstants::ENCODING_7BIT;
            }
            break;

        default:
            // Reserved values are to be treated as 7-bit
            encoding = ISmsConstants::ENCODING_7BIT;
            break;
    }

    Boolean b;
    if (header->IsUmtsFormat(&b), b) {
        // Payload may contain multiple pages
        Int32 nrPages = (*pdu)[CSmsCbHeader::PDU_HEADER_LENGTH];

        if (pdu->GetLength() < CSmsCbHeader::PDU_HEADER_LENGTH + 1 + (PDU_BODY_PAGE_LENGTH + 1)
                * nrPages) {
            // throw new IllegalArgumentException("Pdu length " + pdu->GetLength() + " does not match "
            //         + nrPages + " pages");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        StringBuilder sb;

        for (Int32 i = 0; i < nrPages; i++) {
            // Each page is 82 bytes followed by a length octet indicating
            // the number of useful octets within those 82
            Int32 offset = CSmsCbHeader::PDU_HEADER_LENGTH + 1 + (PDU_BODY_PAGE_LENGTH + 1) * i;
            Int32 length = (*pdu)[offset + PDU_BODY_PAGE_LENGTH];

            if (length > PDU_BODY_PAGE_LENGTH) {
                // throw new IllegalArgumentException("Page length " + length
                //         + " exceeds maximum value " + PDU_BODY_PAGE_LENGTH);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<IPair> p;
            UnpackBody(pdu, encoding, offset, length,
                    hasLanguageIndicator, language, (IPair**)&p);
            AutoPtr<IInterface> obj;
            p->GetFirst((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&language);
            obj = NULL;
            p->GetSecond((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            sb.Append(str);
        }
        String str;
        sb.ToString(&str);
        CPair::New(CoreUtils::Convert(language), CoreUtils::Convert(str), result);
    }
    else {
        // Payload is one single page
        Int32 offset = CSmsCbHeader::PDU_HEADER_LENGTH;
        Int32 length = pdu->GetLength() - offset;

        UnpackBody(pdu, encoding, offset, length, hasLanguageIndicator, language, result);
    }

    return NOERROR;
}

ECode CGsmSmsCbMessage::UnpackBody(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Boolean hasLanguageIndicator,
    /* [in] */ const String& _language,
    /* [out] */ IPair** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    String body;
    String language = _language;

    switch (encoding) {
        case ISmsConstants::ENCODING_7BIT:
            GsmAlphabet::Gsm7BitPackedToString(pdu, offset, length * 8 / 7, &body);

            if (hasLanguageIndicator && body != NULL && body.GetLength() > 2) {
                // Language is two GSM characters followed by a CR.
                // The actual body text is offset by 3 characters.
                language = body.Substring(0, 2);
                body = body.Substring(3);
            }
            break;

        case ISmsConstants::ENCODING_16BIT:
            if (hasLanguageIndicator && pdu->GetLength() >= offset + 2) {
                // Language is two GSM characters.
                // The actual body text is offset by 2 bytes.
                GsmAlphabet::Gsm7BitPackedToString(pdu, offset, 2, &language);
                offset += 2;
                length -= 2;
            }

            // try {
            body = String(*pdu, offset, (length & 0xfffe)); // body = new String(pdu, offset, (length & 0xfffe), "utf-16");
            // } catch (UnsupportedEncodingException e) {
            //     // Apparently it wasn't valid UTF-16.
            //     throw new IllegalArgumentException("Error decoding UTF-16 message", e);
            // }
            break;

        default:
            break;
    }

    if (body != NULL) {
        // Remove trailing carriage return
        for (Int32 i = body.GetLength() - 1; i >= 0; i--) {
            if (body.GetChar(i) != CARRIAGE_RETURN) {
                body = body.Substring(0, i + 1);
                break;
            }
        }
    }
    else {
        body = String("");
    }

    return CPair::New(CoreUtils::Convert(language), CoreUtils::Convert(body), result);
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos