#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/internal/utility/HexDump.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Utility::HexDump;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {


/**
 * SMS user data header, as specified in TS 23.040 9.2.3.24.
 */
CAR_INTERFACE_IMPL(SmsHeader, Object, ISmsHeader)

SmsHeader::SmsHeader()
{
    CArrayList::New((IArrayList**)&specialSmsMsgList);
    CArrayList::New((IArrayList**)&miscEltList);
}

ECode SmsHeader::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsHeader** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IByteArrayInputStream> baInStream;
    CByteArrayInputStream::New(data, (IByteArrayInputStream**)&baInStream);
    AutoPtr<SmsHeader> smsHeader = new SmsHeader();
    Int32 num = 0;
    IInputStream* inStream = IInputStream::Probe(baInStream);
    while ((inStream->Available(&num), num) > 0) {
        /**
         * NOTE: as defined in the spec, ConcatRef and PortAddr
         * fields should not reoccur, but if they do the last
         * occurrence is to be used.  Also, for ConcatRef
         * elements, if the count is zero, sequence is zero, or
         * sequence is larger than count, the entire element is to
         * be ignored.
         */
        Int32 id;
        inStream->Read(&id);
        Int32 length;
        inStream->Read(&length);
        AutoPtr<ConcatRef> concatRef;
        AutoPtr<PortAddrs> portAddrs;
        switch (id) {
            case ELT_ID_CONCATENATED_8_BIT_REFERENCE: {
                concatRef = new ConcatRef();
                Int32 value;
                inStream->Read(&value);
                concatRef->refNumber = value;
                inStream->Read(&value);
                concatRef->msgCount = value;
                inStream->Read(&value);
                concatRef->seqNumber = value;
                concatRef->isEightBits = TRUE;
                if (concatRef->msgCount != 0 && concatRef->seqNumber != 0 &&
                        concatRef->seqNumber <= concatRef->msgCount) {
                    smsHeader->concatRef = concatRef;
                }
                break;
            }
            case ELT_ID_CONCATENATED_16_BIT_REFERENCE: {
                concatRef = new ConcatRef();
                Int32 value, v2;
                inStream->Read(&value);
                inStream->Read(&v2);
                concatRef->refNumber = (value << 8) | v2;
                inStream->Read(&value);
                concatRef->msgCount = value;
                inStream->Read(&value);
                concatRef->seqNumber = value;
                concatRef->isEightBits = FALSE;
                if (concatRef->msgCount != 0 && concatRef->seqNumber != 0 &&
                        concatRef->seqNumber <= concatRef->msgCount) {
                    smsHeader->concatRef = concatRef;
                }
                break;
            }
            case ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT: {
                portAddrs = new PortAddrs();
                Int32 value;
                inStream->Read(&value);
                portAddrs->destPort = value;
                inStream->Read(&value);
                portAddrs->origPort = value;
                portAddrs->areEightBits = TRUE;
                smsHeader->portAddrs = portAddrs;
                break;
            }
            case ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT: {
                portAddrs = new PortAddrs();
                Int32 value, v2;
                inStream->Read(&value);
                inStream->Read(&v2);
                portAddrs->destPort = (value << 8) | v2;
                inStream->Read(&value);
                inStream->Read(&v2);
                portAddrs->origPort = (value << 8) | v2;
                portAddrs->areEightBits = FALSE;
                smsHeader->portAddrs = portAddrs;
                break;
            }
            case ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT: {
                Int32 value;
                inStream->Read(&value);
                smsHeader->languageShiftTable = value;
                break;
            }
            case ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT:
                Int32 value;
                inStream->Read(&value);
                smsHeader->languageTable = value;
                break;
            case ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION: {
                AutoPtr<SpecialSmsMsg> specialSmsMsg = new SpecialSmsMsg();
                Int32 value;
                inStream->Read(&value);
                specialSmsMsg->msgIndType = value;
                inStream->Read(&value);
                specialSmsMsg->msgCount = value;
                smsHeader->specialSmsMsgList->Add(TO_IINTERFACE(specialSmsMsg));
                break;
            }
            default: {
                AutoPtr<MiscElt> miscElt = new MiscElt();
                miscElt->id = id;
                miscElt->data = ArrayOf<Byte>::Alloc(length);
                Int32 num;
                inStream->Read(miscElt->data, 0, length, &num);
                smsHeader->miscEltList->Add(TO_IINTERFACE(miscElt));
            }
        }
    }
    *result = smsHeader;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


ECode SmsHeader::ToByteArray(
    /* [in] */ ISmsHeader* _smsHeader,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(*result);
    *result = NULL;

    SmsHeader* smsHeader = (SmsHeader*)(IObject::Probe(_smsHeader));
    Boolean isEmpty;
    if ((smsHeader->portAddrs == NULL) &&
            (smsHeader->concatRef == NULL) &&
            (smsHeader->specialSmsMsgList->IsEmpty(&isEmpty), isEmpty) &&
            (smsHeader->miscEltList->IsEmpty(&isEmpty), isEmpty) &&
            (smsHeader->languageShiftTable == 0) &&
            (smsHeader->languageTable == 0)) {
        return NOERROR;
    }

    AutoPtr<IByteArrayOutputStream> baOutStream;
    CByteArrayOutputStream::New(ISmsConstants::MAX_USER_DATA_BYTES, (IByteArrayOutputStream**)&baOutStream);
    AutoPtr<ConcatRef> concatRef = smsHeader->concatRef;
    IOutputStream* outStream = IOutputStream::Probe(baOutStream);
    if (concatRef != NULL) {
        if (concatRef->isEightBits) {
            outStream->Write(ELT_ID_CONCATENATED_8_BIT_REFERENCE);
            outStream->Write(3);
            outStream->Write(concatRef->refNumber);
        } else {
            outStream->Write(ELT_ID_CONCATENATED_16_BIT_REFERENCE);
            outStream->Write(4);
            UInt32 value = (UInt32)concatRef->refNumber;
            outStream->Write(value >> 8);//TODO is this ok?? in java >>>
            outStream->Write(concatRef->refNumber & 0x00FF);
        }
        outStream->Write(concatRef->msgCount);
        outStream->Write(concatRef->seqNumber);
    }
    AutoPtr<PortAddrs> portAddrs = smsHeader->portAddrs;
    if (portAddrs != NULL) {
        if (portAddrs->areEightBits) {
            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT);
            outStream->Write(2);
            outStream->Write(portAddrs->destPort);
            outStream->Write(portAddrs->origPort);
        } else {
            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT);
            outStream->Write(4);
            UInt32 value = (UInt32)portAddrs->destPort;
            outStream->Write(portAddrs->destPort >> 8);// in java >>>
            outStream->Write(portAddrs->destPort & 0x00FF);
            value = (UInt32)portAddrs->origPort;
            outStream->Write(portAddrs->origPort >> 8); // in java >>>
            outStream->Write(portAddrs->origPort & 0x00FF);
        }
    }
    if (smsHeader->languageShiftTable != 0) {
        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT);
        outStream->Write(1);
        outStream->Write(smsHeader->languageShiftTable);
    }
    if (smsHeader->languageTable != 0) {
        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT);
        outStream->Write(1);
        outStream->Write(smsHeader->languageTable);
    }
    Int32 size;
    smsHeader->specialSmsMsgList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        smsHeader->specialSmsMsgList->Get(i, (IInterface**)&obj);
        SpecialSmsMsg* specialSmsMsg = (SpecialSmsMsg*)(IObject::Probe(obj));
        outStream->Write(ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION);
        outStream->Write(2);
        outStream->Write(specialSmsMsg->msgIndType & 0xFF);
        outStream->Write(specialSmsMsg->msgCount & 0xFF);
    }
    smsHeader->miscEltList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        smsHeader->miscEltList->Get(i, (IInterface**)&obj);
        MiscElt* miscElt = (MiscElt*)(IObject::Probe(obj));
        outStream->Write(miscElt->id);
        Int32 len = miscElt->data->GetLength();
        outStream->Write(len);
        outStream->Write(miscElt->data, 0, len);
    }
    return baOutStream->ToByteArray(result);
}

//@Override
CARAPI SmsHeader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<StringBuilder> builder = new StringBuilder();
    builder->Append("UserDataHeader ");
    builder->Append("{ ConcatRef ");
    if (concatRef == NULL) {
        builder->Append("unset");
    } else {
        builder->Append("{ refNumber=");
        builder->Append(concatRef->refNumber);
        builder->Append(", msgCount=");
        builder->Append(concatRef->msgCount);
        builder->Append(", seqNumber=");
        builder->Append(concatRef->seqNumber);
        builder->Append(", isEightBits=");
        builder->Append(concatRef->isEightBits);
        builder->Append(" }");
    }
    builder->Append(", PortAddrs ");
    if (portAddrs == NULL) {
        builder->Append("unset");
    } else {
        builder->Append("{ destPort=");
        builder->Append(portAddrs->destPort);
        builder->Append(", origPort=");
        builder->Append(portAddrs->origPort);
        builder->Append(", areEightBits=");
        builder->Append(portAddrs->areEightBits);
        builder->Append(" }");
    }
    if (languageShiftTable != 0) {
        builder->Append(", languageShiftTable=");
        builder->Append(languageShiftTable);
    }
    if (languageTable != 0) {
        builder->Append(", languageTable=");
        builder->Append(languageTable);
    }
    Int32 size;
    specialSmsMsgList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        specialSmsMsgList->Get(i, (IInterface**)&obj);
        SpecialSmsMsg* specialSmsMsg = (SpecialSmsMsg*)(IObject::Probe(obj));
        builder->Append(", SpecialSmsMsg ");
        builder->Append("{ msgIndType=");
        builder->Append(specialSmsMsg->msgIndType);
        builder->Append(", msgCount=");
        builder->Append(specialSmsMsg->msgCount);
        builder->Append(" }");
    }
    miscEltList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        miscEltList->Get(i, (IInterface**)&obj);
        MiscElt* miscElt = (MiscElt*)(IObject::Probe(obj));
        builder->Append(", MiscElt ");
        builder->Append("{ id=");
        builder->Append(miscElt->id);
        builder->Append(", length=");
        Int32 len = miscElt->data->GetLength();
        builder->Append(len);
        builder->Append(", data=");
        builder->Append(HexDump::ToHexString(miscElt->data));
        builder->Append(" }");
    }
    builder->Append(" }");
    *str = builder->ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
