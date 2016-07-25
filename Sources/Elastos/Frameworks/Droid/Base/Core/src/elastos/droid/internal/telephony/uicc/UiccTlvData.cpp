
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccTlvData.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             UiccTlvData
//=====================================================================
CAR_INTERFACE_IMPL(UiccTlvData, Object, IUiccTlvData);

const String UiccTlvData::LOGTAG("UiccTlvData");
const Int32 UiccTlvData::TLV_FORMAT_ID;
const Int32 UiccTlvData::TAG_FILE_DESCRIPTOR;
const Int32 UiccTlvData::TAG_FILE_IDENTIFIER;
const Int32 UiccTlvData::TAG_PROPRIETARY_INFO;
const Int32 UiccTlvData::TAG_LIFECYCLE_STATUS;
const Int32 UiccTlvData::TAG_SECURITY_ATTR_1;
const Int32 UiccTlvData::TAG_SECURITY_ATTR_2;
const Int32 UiccTlvData::TAG_SECURITY_ATTR_3;
const Int32 UiccTlvData::TAG_FILE_SIZE;
const Int32 UiccTlvData::TAG_TOTAL_FILE_SIZE;
const Int32 UiccTlvData::TAG_SHORT_FILE_IDENTIFIER;
const Int32 UiccTlvData::TYPE_5;
const Int32 UiccTlvData::TYPE_2;

ECode UiccTlvData::IsIncomplete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumRecords == -1 || mFileSize == -1 || mRecordSize == -1 || mFileType == -1;
    return NOERROR;
}

Boolean UiccTlvData::IsUiccTlvData(
    /* [in] */ ArrayOf<Byte>* data)
{
    if(data != NULL && data->GetLength() > 0 && TLV_FORMAT_ID == ((*data)[0] & 0xFF)) {
        return TRUE;
    }
    return FALSE;
}

AutoPtr<IUiccTlvData> UiccTlvData::Parse(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<UiccTlvData> parsedData = new UiccTlvData();

    if (data == NULL || data->GetLength() == 0 || TLV_FORMAT_ID != ((*data)[0] & 0xFF)) {
        // throw new IccFileTypeMismatch();
        assert(0 && "Error");
    }

    // try {
    Int32 currentLocation = 2; //Ignore FCP size
    Int32 currentTag;

    while (currentLocation < data->GetLength()) {
        currentTag = (*data)[currentLocation++] & 0xFF;

        switch (currentTag) {
            case TAG_FILE_DESCRIPTOR:
                currentLocation = parsedData->ParseFileDescriptor(data, currentLocation);
                break;

            case TAG_FILE_SIZE:
                currentLocation = parsedData->ParseFileSize(data, currentLocation);
                break;

            case TAG_FILE_IDENTIFIER:
            case TAG_PROPRIETARY_INFO:
            case TAG_LIFECYCLE_STATUS:
            case TAG_SECURITY_ATTR_1:
            case TAG_SECURITY_ATTR_2:
            case TAG_SECURITY_ATTR_3:
            case TAG_TOTAL_FILE_SIZE:
            case TAG_SHORT_FILE_IDENTIFIER:
                currentLocation = parsedData->ParseSomeTag(data, currentLocation);
                break;

            default:
                Logger::D(LOGTAG, "Unknown tag 0x%02X", currentTag);
                currentLocation = parsedData->ParseSomeTag(data, currentLocation);
                break;
        }
    }
    // } catch (ArrayIndexOutOfBoundsException e) {
    //     //We might be looking at incomplete data but we might have what we need.
    //     //Ignore this  and let caller handle it by checking isIncomplete
    // }

    return parsedData;
}

UiccTlvData::UiccTlvData()
    : mRecordSize(0)
    , mFileSize(0)
    , mNumRecords(0)
    , mIsDataEnough(FALSE)
    , mFileType(-1)
{
    mNumRecords = -1;
    mFileSize = -1;
    mRecordSize = -1;
}

ECode UiccTlvData::constructor()
{
    return NOERROR;
}

Int32 UiccTlvData::ParseFileSize(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    Int32 length = (*data)[currentLocation++] & 0xFF;

    Int32 fileSize = 0;
    for (Int32 i = 0; i < length; i++) {
        fileSize += (((*data)[currentLocation + i] & 0xFF) << ( 8 * (length - i - 1)));
    }

    mFileSize = fileSize;

    if (mFileType == TYPE_2) {
        mRecordSize = fileSize;
    }

    return currentLocation + length;
}

Int32 UiccTlvData::ParseSomeTag(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    //Just skip unwanted tags;
    Int32 length = (*data)[currentLocation++] & 0xFF;
    return currentLocation + length;
}

Int32 UiccTlvData::ParseFileDescriptor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    Int32 length = (*data)[currentLocation++] & 0xFF;
    if (length == 5) {

        mRecordSize = (((*data)[currentLocation + 2] & 0xFF) << 8) +
                ((*data)[currentLocation + 3] & 0xFF); // Length of 1 record
        mNumRecords = (*data)[currentLocation + 4] & 0xFF; // Number of records

        mFileSize = mRecordSize * mNumRecords;

        mFileType = TYPE_5;

        return currentLocation + 5;
    }
    else if (length == 2) {
        Int32 descriptorByte = (*data)[currentLocation + 1] & 0xFF;

        //Ignore descriptorByte for now

        mNumRecords = 1;

        mFileType = TYPE_2;

        return currentLocation + 2;

    }
    else {
        assert(0 && "IccFileTypeMismatch");
        // throw new IccFileTypeMismatch();
    }

    return 0;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
