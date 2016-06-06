
#include "elastos/droid/internal/telephony/SmsHeader.h"

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Utility::IHexDump;

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {


/**
 * SMS user data header, as specified in TS 23.040 9.2.3.24.
 */
CAR_INTERFACE_IMPL(SmsHeader, Object, ISmsHeader)

//TODO
//public static class PortAddrs {
//    public Int32 destPort;
//    public Int32 origPort;
//    public Boolean areEightBits;
//}
//
//public static class ConcatRef {
//    public Int32 refNumber;
//    public Int32 seqNumber;
//    public Int32 msgCount;
//    public Boolean isEightBits;
//}
//
//public static class SpecialSmsMsg {
//    public Int32 msgIndType;
//    public Int32 msgCount;
//}
//
///**
// * A header element that is not explicitly parsed, meaning not
// * PortAddrs or ConcatRef or SpecialSmsMsg.
// */
//public static class MiscElt {
//    public Int32 id;
//    public Byte[] data;
//}
//
//public PortAddrs portAddrs;
//public ConcatRef concatRef;
//public ArrayList<SpecialSmsMsg> specialSmsMsgList = new ArrayList<SpecialSmsMsg>();
//public ArrayList<MiscElt> miscEltList = new ArrayList<MiscElt>();
//
///** 7 bit national language locking shift table, or 0 for GSM default 7 bit alphabet. */
//public Int32 languageTable;
//
///** 7 bit national language single shift table, or 0 for GSM default 7 bit extension table. */
//public Int32 languageShiftTable;
//
//public SmsHeader() {}


ECode SmsHeader::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsHeader** result)
{
    assert(0 && "TODO");
    return NOERROR;
//    ByteArrayInputStream inStream = new ByteArrayInputStream(data);
//    SmsHeader smsHeader = new SmsHeader();
//    While (inStream->Available() > 0) {
//        /**
//         * NOTE: as defined in the spec, ConcatRef and PortAddr
//         * fields should not reoccur, but if they do the last
//         * occurrence is to be used.  Also, for ConcatRef
//         * elements, if the count is zero, sequence is zero, or
//         * sequence is larger than count, the entire element is to
//         * be ignored.
//         */
//        Int32 id = inStream->Read();
//        Int32 length = inStream->Read();
//        ConcatRef concatRef;
//        PortAddrs portAddrs;
//        Switch (id) {
//        case ELT_ID_CONCATENATED_8_BIT_REFERENCE:
//            concatRef = new ConcatRef();
//            concatRef.refNumber = inStream->Read();
//            concatRef.msgCount = inStream->Read();
//            concatRef.seqNumber = inStream->Read();
//            concatRef.isEightBits = TRUE;
//            if (concatRef.msgCount != 0 && concatRef.seqNumber != 0 &&
//                    concatRef.seqNumber <= concatRef.msgCount) {
//                smsHeader.concatRef = concatRef;
//            }
//            break;
//        case ELT_ID_CONCATENATED_16_BIT_REFERENCE:
//            concatRef = new ConcatRef();
//            concatRef.refNumber = (inStream->Read() << 8) | inStream->Read();
//            concatRef.msgCount = inStream->Read();
//            concatRef.seqNumber = inStream->Read();
//            concatRef.isEightBits = FALSE;
//            if (concatRef.msgCount != 0 && concatRef.seqNumber != 0 &&
//                    concatRef.seqNumber <= concatRef.msgCount) {
//                smsHeader.concatRef = concatRef;
//            }
//            break;
//        case ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT:
//            portAddrs = new PortAddrs();
//            portAddrs.destPort = inStream->Read();
//            portAddrs.origPort = inStream->Read();
//            portAddrs.areEightBits = TRUE;
//            smsHeader.portAddrs = portAddrs;
//            break;
//        case ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT:
//            portAddrs = new PortAddrs();
//            portAddrs.destPort = (inStream->Read() << 8) | inStream->Read();
//            portAddrs.origPort = (inStream->Read() << 8) | inStream->Read();
//            portAddrs.areEightBits = FALSE;
//            smsHeader.portAddrs = portAddrs;
//            break;
//        case ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHifT:
//            smsHeader.languageShiftTable = inStream->Read();
//            break;
//        case ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHifT:
//            smsHeader.languageTable = inStream->Read();
//            break;
//        case ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION:
//            SpecialSmsMsg specialSmsMsg = new SpecialSmsMsg();
//            specialSmsMsg.msgIndType = inStream->Read();
//            specialSmsMsg.msgCount = inStream->Read();
//            smsHeader.specialSmsMsgList->Add(specialSmsMsg);
//            break;
//        default:
//            MiscElt miscElt = new MiscElt();
//            miscElt.id = id;
//            miscElt.data = new Byte[length];
//            inStream->Read(miscElt.data, 0, length);
//            smsHeader.miscEltList->Add(miscElt);
//        }
//    }
//    return smsHeader;
}


ECode SmsHeader::ToByteArray(
    /* [in] */ ISmsHeader* smsHeader,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    assert(0 && "TODO");
    return NOERROR;
//    if ((smsHeader.portAddrs == NULL) &&
//        (smsHeader.concatRef == NULL) &&
//        (smsHeader.specialSmsMsgList->IsEmpty()) &&
//        (smsHeader.miscEltList->IsEmpty()) &&
//        (smsHeader.languageShiftTable == 0) &&
//        (smsHeader.languageTable == 0)) {
//        return NULL;
//    }
//
//    ByteArrayOutputStream outStream =
//            new ByteArrayOutputStream(SmsConstants.MAX_USER_DATA_BYTES);
//    ConcatRef concatRef = smsHeader.concatRef;
//    if (concatRef != NULL) {
//        if (concatRef.isEightBits) {
//            outStream->Write(ELT_ID_CONCATENATED_8_BIT_REFERENCE);
//            outStream->Write(3);
//            outStream->Write(concatRef.refNumber);
//        } else {
//            outStream->Write(ELT_ID_CONCATENATED_16_BIT_REFERENCE);
//            outStream->Write(4);
//            outStream->Write(concatRef.refNumber >>> 8);
//            outStream->Write(concatRef.refNumber & 0x00FF);
//        }
//        outStream->Write(concatRef.msgCount);
//        outStream->Write(concatRef.seqNumber);
//    }
//    PortAddrs portAddrs = smsHeader.portAddrs;
//    if (portAddrs != NULL) {
//        if (portAddrs.areEightBits) {
//            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT);
//            outStream->Write(2);
//            outStream->Write(portAddrs.destPort);
//            outStream->Write(portAddrs.origPort);
//        } else {
//            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT);
//            outStream->Write(4);
//            outStream->Write(portAddrs.destPort >>> 8);
//            outStream->Write(portAddrs.destPort & 0x00FF);
//            outStream->Write(portAddrs.origPort >>> 8);
//            outStream->Write(portAddrs.origPort & 0x00FF);
//        }
//    }
//    if (smsHeader.languageShiftTable != 0) {
//        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHifT);
//        outStream->Write(1);
//        outStream->Write(smsHeader.languageShiftTable);
//    }
//    if (smsHeader.languageTable != 0) {
//        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHifT);
//        outStream->Write(1);
//        outStream->Write(smsHeader.languageTable);
//    }
//    For (SpecialSmsMsg specialSmsMsg : smsHeader.specialSmsMsgList) {
//        outStream->Write(ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION);
//        outStream->Write(2);
//        outStream->Write(specialSmsMsg.msgIndType & 0xFF);
//        outStream->Write(specialSmsMsg.msgCount & 0xFF);
//    }
//    For (MiscElt miscElt : smsHeader.miscEltList) {
//        outStream->Write(miscElt.id);
//        outStream->Write(miscElt.data.length);
//        outStream->Write(miscElt.data, 0, miscElt.data.length);
//    }
//    return outStream->ToByteArray();
}

//@Override
CARAPI SmsHeader::ToString(
    /* [out] */ String* str)
{
    assert(0 && "TODO");
    return NOERROR;
//    StringBuilder builder = new StringBuilder();
//    builder->Append("UserDataHeader ");
//    builder->Append("{ ConcatRef ");
//    if (concatRef == NULL) {
//        builder->Append("unset");
//    } else {
//        builder->Append("{ refNumber=" + concatRef.refNumber);
//        builder->Append(", msgCount=" + concatRef.msgCount);
//        builder->Append(", seqNumber=" + concatRef.seqNumber);
//        builder->Append(", isEightBits=" + concatRef.isEightBits);
//        builder->Append(" }");
//    }
//    builder->Append(", PortAddrs ");
//    if (portAddrs == NULL) {
//        builder->Append("unset");
//    } else {
//        builder->Append("{ destPort=" + portAddrs.destPort);
//        builder->Append(", origPort=" + portAddrs.origPort);
//        builder->Append(", areEightBits=" + portAddrs.areEightBits);
//        builder->Append(" }");
//    }
//    if (languageShiftTable != 0) {
//        builder->Append(", languageShiftTable=" + languageShiftTable);
//    }
//    if (languageTable != 0) {
//        builder->Append(", languageTable=" + languageTable);
//    }
//    For (SpecialSmsMsg specialSmsMsg : specialSmsMsgList) {
//        builder->Append(", SpecialSmsMsg ");
//        builder->Append("{ msgIndType=" + specialSmsMsg.msgIndType);
//        builder->Append(", msgCount=" + specialSmsMsg.msgCount);
//        builder->Append(" }");
//    }
//    For (MiscElt miscElt : miscEltList) {
//        builder->Append(", MiscElt ");
//        builder->Append("{ id=" + miscElt.id);
//        builder->Append(", length=" + miscElt.data.length);
//        builder->Append(", data=" + HexDump->ToHexString(miscElt.data));
//        builder->Append(" }");
//    }
//    builder->Append(" }");
//    return builder->ToString();
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
