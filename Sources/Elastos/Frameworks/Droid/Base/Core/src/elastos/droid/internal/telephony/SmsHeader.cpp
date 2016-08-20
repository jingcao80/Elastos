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
    : mLanguageTable(0)
    , mLanguageShiftTable(0)
{
    CArrayList::New((IArrayList**)&mSpecialSmsMsgList);
    CArrayList::New((IArrayList**)&mMiscEltList);
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
        AutoPtr<PortAddrs> mPortAddrs;
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
                    smsHeader->mConcatRef = concatRef;
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
                    smsHeader->mConcatRef = concatRef;
                }
                break;
            }
            case ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT: {
                mPortAddrs = new PortAddrs();
                Int32 value;
                inStream->Read(&value);
                mPortAddrs->destPort = value;
                inStream->Read(&value);
                mPortAddrs->origPort = value;
                mPortAddrs->areEightBits = TRUE;
                smsHeader->mPortAddrs = mPortAddrs;
                break;
            }
            case ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT: {
                mPortAddrs = new PortAddrs();
                Int32 value, v2;
                inStream->Read(&value);
                inStream->Read(&v2);
                mPortAddrs->destPort = (value << 8) | v2;
                inStream->Read(&value);
                inStream->Read(&v2);
                mPortAddrs->origPort = (value << 8) | v2;
                mPortAddrs->areEightBits = FALSE;
                smsHeader->mPortAddrs = mPortAddrs;
                break;
            }
            case ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT: {
                Int32 value;
                inStream->Read(&value);
                smsHeader->mLanguageShiftTable = value;
                break;
            }
            case ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT:
                Int32 value;
                inStream->Read(&value);
                smsHeader->mLanguageTable = value;
                break;
            case ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION: {
                AutoPtr<SpecialSmsMsg> specialSmsMsg = new SpecialSmsMsg();
                Int32 value;
                inStream->Read(&value);
                specialSmsMsg->msgIndType = value;
                inStream->Read(&value);
                specialSmsMsg->msgCount = value;
                smsHeader->mSpecialSmsMsgList->Add(TO_IINTERFACE(specialSmsMsg));
                break;
            }
            default: {
                AutoPtr<MiscElt> miscElt = new MiscElt();
                miscElt->id = id;
                miscElt->data = ArrayOf<Byte>::Alloc(length);
                Int32 num;
                inStream->Read(miscElt->data, 0, length, &num);
                smsHeader->mMiscEltList->Add(TO_IINTERFACE(miscElt));
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
    if ((smsHeader->mPortAddrs == NULL) &&
            (smsHeader->mConcatRef == NULL) &&
            (smsHeader->mSpecialSmsMsgList->IsEmpty(&isEmpty), isEmpty) &&
            (smsHeader->mMiscEltList->IsEmpty(&isEmpty), isEmpty) &&
            (smsHeader->mLanguageShiftTable == 0) &&
            (smsHeader->mLanguageTable == 0)) {
        return NOERROR;
    }

    AutoPtr<IByteArrayOutputStream> baOutStream;
    CByteArrayOutputStream::New(ISmsConstants::MAX_USER_DATA_BYTES, (IByteArrayOutputStream**)&baOutStream);
    AutoPtr<ConcatRef> concatRef = smsHeader->mConcatRef;
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
    AutoPtr<PortAddrs> mPortAddrs = smsHeader->mPortAddrs;
    if (mPortAddrs != NULL) {
        if (mPortAddrs->areEightBits) {
            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT);
            outStream->Write(2);
            outStream->Write(mPortAddrs->destPort);
            outStream->Write(mPortAddrs->origPort);
        } else {
            outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT);
            outStream->Write(4);
            UInt32 value = (UInt32)mPortAddrs->destPort;
            outStream->Write(mPortAddrs->destPort >> 8);// in java >>>
            outStream->Write(mPortAddrs->destPort & 0x00FF);
            value = (UInt32)mPortAddrs->origPort;
            outStream->Write(mPortAddrs->origPort >> 8); // in java >>>
            outStream->Write(mPortAddrs->origPort & 0x00FF);
        }
    }
    if (smsHeader->mLanguageShiftTable != 0) {
        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT);
        outStream->Write(1);
        outStream->Write(smsHeader->mLanguageShiftTable);
    }
    if (smsHeader->mLanguageTable != 0) {
        outStream->Write(ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT);
        outStream->Write(1);
        outStream->Write(smsHeader->mLanguageTable);
    }
    Int32 size;
    smsHeader->mSpecialSmsMsgList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        smsHeader->mSpecialSmsMsgList->Get(i, (IInterface**)&obj);
        SpecialSmsMsg* specialSmsMsg = (SpecialSmsMsg*)(IObject::Probe(obj));
        outStream->Write(ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION);
        outStream->Write(2);
        outStream->Write(specialSmsMsg->msgIndType & 0xFF);
        outStream->Write(specialSmsMsg->msgCount & 0xFF);
    }
    smsHeader->mMiscEltList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        smsHeader->mMiscEltList->Get(i, (IInterface**)&obj);
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
    if (mConcatRef == NULL) {
        builder->Append("unset");
    } else {
        builder->Append("{ refNumber=");
        builder->Append(mConcatRef->refNumber);
        builder->Append(", msgCount=");
        builder->Append(mConcatRef->msgCount);
        builder->Append(", seqNumber=");
        builder->Append(mConcatRef->seqNumber);
        builder->Append(", isEightBits=");
        builder->Append(mConcatRef->isEightBits);
        builder->Append(" }");
    }
    builder->Append(", PortAddrs ");
    if (mPortAddrs == NULL) {
        builder->Append("unset");
    } else {
        builder->Append("{ destPort=");
        builder->Append(mPortAddrs->destPort);
        builder->Append(", origPort=");
        builder->Append(mPortAddrs->origPort);
        builder->Append(", areEightBits=");
        builder->Append(mPortAddrs->areEightBits);
        builder->Append(" }");
    }
    if (mLanguageShiftTable != 0) {
        builder->Append(", mLanguageShiftTable=");
        builder->Append(mLanguageShiftTable);
    }
    if (mLanguageTable != 0) {
        builder->Append(", mLanguageTable=");
        builder->Append(mLanguageTable);
    }
    Int32 size;
    mSpecialSmsMsgList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mSpecialSmsMsgList->Get(i, (IInterface**)&obj);
        SpecialSmsMsg* specialSmsMsg = (SpecialSmsMsg*)(IObject::Probe(obj));
        builder->Append(", SpecialSmsMsg ");
        builder->Append("{ msgIndType=");
        builder->Append(specialSmsMsg->msgIndType);
        builder->Append(", msgCount=");
        builder->Append(specialSmsMsg->msgCount);
        builder->Append(" }");
    }
    mMiscEltList->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mMiscEltList->Get(i, (IInterface**)&obj);
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
