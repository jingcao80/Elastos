#include "elastos/droid/internal/telephony/gsm/CSmsMessageHelper.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CSmsMessageHelper, Singleton, ISmsMessageHelper)

CAR_SINGLETON_IMPL(CSmsMessageHelper)

ECode CSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    return CSmsMessage::CreateFromPdu(pdu, result);
}

ECode CSmsMessageHelper::NewFromCMT(
    /* [in] */ ArrayOf<String>* lines,
    /* [out] */ ISmsMessage** result)
{
    return CSmsMessage::NewFromCMT(lines, result);
}

ECode CSmsMessageHelper::NewFromCDS(
    /* [in] */ const String& line,
    /* [out] */ ISmsMessage** result)
{
    return CSmsMessage::NewFromCDS(line, result);
}

ECode CSmsMessageHelper::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsMessage** result)
{
    return CSmsMessage::CreateFromEfRecord(index, data, result);
}

ECode CSmsMessageHelper::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    return CSmsMessage::GetTPLayerLengthForPDU(pdu, result);
}

ECode CSmsMessageHelper::GetRelativeValidityPeriod(
    /* [in] */ Int32 validityPeriod,
    /* [out] */ Int32* result)
{
    return CSmsMessage::GetRelativeValidityPeriod(validityPeriod, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, header, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, header, encoding, languageTable,
            languageShiftTable, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [in] */ Int32 validityPeriod,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, header, encoding, languageTable,
            languageShiftTable, validityPeriod, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int32 validityPeriod,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, validityPeriod, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int32 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, destinationPort,
            data, statusReportRequested, result);
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int32 destinationPort,
    /* [in] */ Int32 originatorPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return CSmsMessage::GetSubmitPdu(scAddress, destinationAddress, destinationPort,
            originatorPort, data, statusReportRequested, result);
}

ECode CSmsMessageHelper::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    return CSmsMessage::CalculateLength(msgBody, use7bitOnly, result);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos