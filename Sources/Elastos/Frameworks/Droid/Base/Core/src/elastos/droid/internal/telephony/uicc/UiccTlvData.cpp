
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccTlvData.h"

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
    // ==================before translated======================
    // return mNumRecords == -1 || mFileSize == -1 || mRecordSize == -1 || mFileType == -1;
    assert(0);
    return NOERROR;
}

Boolean UiccTlvData::IsUiccTlvData(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // if(data != null && data.length > 0 && TLV_FORMAT_ID == (data[0] & 0xFF)) {
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

AutoPtr<IUiccTlvData> UiccTlvData::Parse(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    //
    // UiccTlvData parsedData = new UiccTlvData();
    //
    // if (data == null || data.length == 0 || TLV_FORMAT_ID != (data[0] & 0xFF)) {
    //     throw new IccFileTypeMismatch();
    // }
    //
    // try {
    //
    //     int currentLocation = 2; //Ignore FCP size
    //     int currentTag;
    //
    //     while (currentLocation < data.length) {
    //         currentTag = data[currentLocation++] & 0xFF;
    //
    //         switch (currentTag) {
    //             case TAG_FILE_DESCRIPTOR:
    //                 currentLocation = parsedData.parseFileDescriptor(data, currentLocation);
    //                 break;
    //
    //             case TAG_FILE_SIZE:
    //                 currentLocation = parsedData.parseFileSize(data, currentLocation);
    //                 break;
    //
    //             case TAG_FILE_IDENTIFIER:
    //             case TAG_PROPRIETARY_INFO:
    //             case TAG_LIFECYCLE_STATUS:
    //             case TAG_SECURITY_ATTR_1:
    //             case TAG_SECURITY_ATTR_2:
    //             case TAG_SECURITY_ATTR_3:
    //             case TAG_TOTAL_FILE_SIZE:
    //             case TAG_SHORT_FILE_IDENTIFIER:
    //                 currentLocation = parsedData.parseSomeTag(data, currentLocation);
    //                 break;
    //
    //             default:
    //                 Rlog.d(LOGTAG, "Unknown tag 0x" + String.format("%02X", currentTag));
    //                 currentLocation = parsedData.parseSomeTag(data, currentLocation);
    //                 break;
    //         }
    //     }
    //
    // } catch (ArrayIndexOutOfBoundsException e) {
    //
    //     //We might be looking at incomplete data but we might have what we need.
    //     //Ignore this  and let caller handle it by checking isIncomplete
    // }
    //
    // return parsedData;
    assert(0);
    AutoPtr<IUiccTlvData> empty;
    return empty;
}

UiccTlvData::UiccTlvData()
{
    // ==================before translated======================
    // mNumRecords = -1;
    // mFileSize = -1;
    // mRecordSize = -1;
}

ECode UiccTlvData::constructor()
{
    return NOERROR;
}

Int32 UiccTlvData::ParseFileSize(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    // ==================before translated======================
    // int length = data[currentLocation++] & 0xFF;
    //
    // int fileSize = 0;
    // for (int i = 0; i < length; i++) {
    //     fileSize += ((data[currentLocation + i] & 0xFF) << ( 8 * (length - i - 1)));
    // }
    //
    // mFileSize = fileSize;
    //
    // if (mFileType == TYPE_2) {
    //     mRecordSize = fileSize;
    // }
    //
    // return currentLocation + length;
    assert(0);
    return 0;
}

Int32 UiccTlvData::ParseSomeTag(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    // ==================before translated======================
    // //Just skip unwanted tags;
    // int length = data[currentLocation++] & 0xFF;
    // return currentLocation + length;
    assert(0);
    return 0;
}

Int32 UiccTlvData::ParseFileDescriptor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 currentLocation)
{
    // ==================before translated======================
    // int length = data[currentLocation++] & 0xFF;
    // if (length == 5) {
    //
    //     mRecordSize = ((data[currentLocation + 2] & 0xFF) << 8) +
    //             (data[currentLocation + 3] & 0xFF); // Length of 1 record
    //     mNumRecords = data[currentLocation + 4] & 0xFF; // Number of records
    //
    //     mFileSize = mRecordSize * mNumRecords;
    //
    //     mFileType = TYPE_5;
    //
    //     return currentLocation + 5;
    // } else if (length == 2) {
    //
    //     int descriptorByte = data[currentLocation + 1] & 0xFF;
    //
    //     //Ignore descriptorByte for now
    //
    //     mNumRecords = 1;
    //
    //     mFileType = TYPE_2;
    //
    //     return currentLocation + 2;
    //
    // } else {
    //     throw new IccFileTypeMismatch();
    // }
    assert(0);
    return 0;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
