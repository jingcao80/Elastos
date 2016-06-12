
#include "elastos/droid/internal/telephony/InboundSmsTracker.h"
#include "elastos/droid/internal/telephony/InboundSmsHandler.h"
#include "elastos/droid/internal/utility/HexDump.h"
#include "elastos/droid/content/CContentValues.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Internal::Utility::HexDump;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 InboundSmsTracker::DEST_PORT_FLAG_NO_PORT = (1 << 16);
const Int32 InboundSmsTracker::DEST_PORT_FLAG_3GPP = (1 << 17);
const Int32 InboundSmsTracker::DEST_PORT_FLAG_3GPP2 = (1 << 18);
const Int32 InboundSmsTracker::DEST_PORT_FLAG_3GPP2_WAP_PDU = (1 << 19);
const Int32 InboundSmsTracker::DEST_PORT_MASK = 0xffff;
InboundSmsTracker::InboundSmsTracker(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 destPort,
    /* [in] */ Boolean is3gpp2,
    /* [in] */ Boolean is3gpp2WapPdu)
{
    mPdu = pdu;
    mTimestamp = timestamp;
    mDestPort = destPort;
    mIs3gpp2 = is3gpp2;
    mIs3gpp2WapPdu = is3gpp2WapPdu;
    // fields for multi-part SMS
    mAddress = NULL;
    mReferenceNumber = -1;
    mSequenceNumber = GetIndexOffset();     // 0 or 1, depending on type
    mMessageCount = 1;
}

InboundSmsTracker::InboundSmsTracker(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 destPort,
    /* [in] */ Boolean is3gpp2,
    /* [in] */ const String& address,
    /* [in] */ Int32 referenceNumber,
    /* [in] */ Int32 sequenceNumber,
    /* [in] */ Int32 messageCount,
    /* [in] */ Boolean is3gpp2WapPdu)
{
    mPdu = pdu;
    mTimestamp = timestamp;
    mDestPort = destPort;
    mIs3gpp2 = is3gpp2;
    mIs3gpp2WapPdu = is3gpp2WapPdu;
    // fields for multi-part SMS
    mAddress = address;
    mReferenceNumber = referenceNumber;
    mSequenceNumber = sequenceNumber;
    mMessageCount = messageCount;
}

InboundSmsTracker::InboundSmsTracker(
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isCurrentFormat3gpp2)
    : mTimestamp(0)
    , mDestPort(0)
    , mIs3gpp2(FALSE)
    , mIs3gpp2WapPdu(FALSE)
    , mReferenceNumber(0)
    , mSequenceNumber(0)
    , mMessageCount(0)
{
    String s;
    cursor->GetString(InboundSmsHandler::PDU_COLUMN, &s);
    mPdu = HexDump::HexStringToByteArray(s);

    Boolean tmp = FALSE;
    if (cursor->IsNull(InboundSmsHandler::DESTINATION_PORT_COLUMN, &tmp), tmp) {
        mDestPort = -1;
        mIs3gpp2 = isCurrentFormat3gpp2;
        mIs3gpp2WapPdu = FALSE;
    }
    else {
        Int32 destPort = 0;
        cursor->GetInt32(InboundSmsHandler::DESTINATION_PORT_COLUMN, &destPort);
        if ((destPort & DEST_PORT_FLAG_3GPP) != 0) {
            mIs3gpp2 = FALSE;
        }
        else if ((destPort & DEST_PORT_FLAG_3GPP2) != 0) {
            mIs3gpp2 = TRUE;
        }
        else {
            mIs3gpp2 = isCurrentFormat3gpp2;
        }
        mIs3gpp2WapPdu = ((destPort & DEST_PORT_FLAG_3GPP2_WAP_PDU) != 0);
        mDestPort = GetRealDestPort(destPort);
    }

    cursor->GetInt64(InboundSmsHandler::DATE_COLUMN, &mTimestamp);

    if (cursor->IsNull(InboundSmsHandler::COUNT_COLUMN, &tmp), tmp) {
        // single-part message
        Int64 rowId = 0;
        cursor->GetInt64(InboundSmsHandler::ID_COLUMN, &rowId);
        mAddress = String(NULL);
        mReferenceNumber = -1;
        mSequenceNumber = GetIndexOffset();     // 0 or 1, depending on type
        mMessageCount = 1;
        mDeleteWhere = InboundSmsHandler::SELECT_BY_ID;
        mDeleteWhereArgs = ArrayOf<String>::Alloc(1);
        (*mDeleteWhereArgs)[0] = StringUtils::ToString(rowId);
    }
    else {
        // multi-part message
        cursor->GetString(InboundSmsHandler::ADDRESS_COLUMN, &mAddress);
        cursor->GetInt32(InboundSmsHandler::REFERENCE_NUMBER_COLUMN, &mReferenceNumber);
        cursor->GetInt32(InboundSmsHandler::COUNT_COLUMN, &mMessageCount);

        // GSM sequence numbers start at 1; CDMA WDP datagram sequence numbers start at 0
        cursor->GetInt32(InboundSmsHandler::SEQUENCE_COLUMN, &mSequenceNumber);
        Int32 index = mSequenceNumber - GetIndexOffset();

        if (index < 0 || index >= mMessageCount) {
            assert(0 && "TODO");
            // throw new IllegalArgumentException("invalid PDU sequence " + mSequenceNumber
            //         + " of " + mMessageCount);
        }

        mDeleteWhere = InboundSmsHandler::SELECT_BY_REFERENCE;
        mDeleteWhereArgs = ArrayOf<String>::Alloc(3);
        (*mDeleteWhereArgs)[0] = mAddress;
        (*mDeleteWhereArgs)[1] = StringUtils::ToString(mReferenceNumber);
        (*mDeleteWhereArgs)[2] = StringUtils::ToString(mMessageCount);
    }
}

AutoPtr<IContentValues> InboundSmsTracker::GetContentValues()
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(String("pdu"), HexDump::ToHexString(mPdu));
    values->Put(String("date"), mTimestamp);
    // Always set the destination port, since it now contains message format flags.
    // Port is a 16-bit value, or -1, so clear the upper bits before setting flags.
    Int32 destPort;
    if (mDestPort == -1) {
        destPort = DEST_PORT_FLAG_NO_PORT;
    }
    else {
        destPort = mDestPort & DEST_PORT_MASK;
    }
    if (mIs3gpp2) {
        destPort |= DEST_PORT_FLAG_3GPP2;
    }
    else {
        destPort |= DEST_PORT_FLAG_3GPP;
    }
    if (mIs3gpp2WapPdu) {
        destPort |= DEST_PORT_FLAG_3GPP2_WAP_PDU;
    }
    values->Put(String("destination_port"), destPort);
    if (mAddress != NULL) {
        values->Put(String("address"), mAddress);
        values->Put(String("reference_number"), mReferenceNumber);
        values->Put(String("sequence"), mSequenceNumber);
        values->Put(String("count"), mMessageCount);
    }
    return values;
}

Int32 InboundSmsTracker::GetRealDestPort(
    /* [in] */ Int32 destPort)
{
    if ((destPort & DEST_PORT_FLAG_NO_PORT) != 0) {
        return -1;
    }

    return destPort & DEST_PORT_MASK;
}

void InboundSmsTracker::SetDeleteWhere(
    /* [in] */ const String& deleteWhere,
    /* [in] */ ArrayOf<String>* deleteWhereArgs)
{
    mDeleteWhere = deleteWhere;
    mDeleteWhereArgs = deleteWhereArgs;
}

ECode InboundSmsTracker::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder builder("SmsTracker{timestamp=");
    AutoPtr<IDate> d;
    CDate::New(mTimestamp, (IDate**)&d);
    builder.Append(TO_CSTR(d));
    builder.Append(" destPort=");
    builder.Append(mDestPort);
    builder.Append(" is3gpp2=");
    builder.Append(mIs3gpp2);
    if (mAddress != NULL) {
        builder.Append(" address=");
        builder.Append(mAddress);
        builder.Append(" refNumber=");
        builder.Append(mReferenceNumber);
        builder.Append(" seqNumber=");
        builder.Append(mSequenceNumber);
        builder.Append(" msgCount=");
        builder.Append(mMessageCount);
    }
    if (mDeleteWhere != NULL) {
        builder.Append(" deleteWhere(");
        builder.Append(mDeleteWhere);
        builder.Append(") deleteArgs=(");
        builder.Append(Arrays::ToString(mDeleteWhereArgs));
        builder.AppendChar(')');
    }
    builder.AppendChar('}');
    return builder.ToString(result);
}

AutoPtr<ArrayOf<Byte> > InboundSmsTracker::GetPdu()
{
    return mPdu;
}

Int64 InboundSmsTracker::GetTimestamp()
{
    return mTimestamp;
}

Int32 InboundSmsTracker::GetDestPort()
{
    return mDestPort;
}

Boolean InboundSmsTracker::Is3gpp2()
{
    return mIs3gpp2;
}

String InboundSmsTracker::GetFormat()
{
    return mIs3gpp2 ? ISmsConstants::FORMAT_3GPP2 : ISmsConstants::FORMAT_3GPP;
}

Int32 InboundSmsTracker::GetIndexOffset()
{
    return (mIs3gpp2 && mIs3gpp2WapPdu) ? 0 : 1;
}

String InboundSmsTracker::GetAddress()
{
    return mAddress;
}

Int32 InboundSmsTracker::GetReferenceNumber()
{
    return mReferenceNumber;
}

Int32 InboundSmsTracker::GetSequenceNumber()
{
    return mSequenceNumber;
}

Int32 InboundSmsTracker::GetMessageCount()
{
    return mMessageCount;
}

String InboundSmsTracker::GetDeleteWhere()
{
    return mDeleteWhere;
}

AutoPtr<ArrayOf<String> > InboundSmsTracker::GetDeleteWhereArgs()
{
    return mDeleteWhereArgs;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
