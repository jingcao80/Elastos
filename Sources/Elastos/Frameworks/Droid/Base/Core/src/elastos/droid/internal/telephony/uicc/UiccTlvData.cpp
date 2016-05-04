package com.android.internal.telephony.uicc;

using Elastos::Droid::Telephony::IRlog;

/**
 * UICC TLV Data Parser according to ETSI TS 102 221 spec.
 */
public class UiccTlvData {
    private static const String LOG_TAG = "UiccTlvData";

    private static const Int32 TLV_FORMAT_ID = 0x62;

    private static const Int32 TAG_FILE_DESCRIPTOR = 0x82;
    private static const Int32 TAG_FILE_IDENTIFIER = 0x83;
    private static const Int32 TAG_PROPRIETARY_INFO = 0xA5;
    private static const Int32 TAG_LIFECYCLE_STATUS = 0x8A;
    private static const Int32 TAG_SECURITY_ATTR_1 = 0x8B;
    private static const Int32 TAG_SECURITY_ATTR_2 = 0x8C;
    private static const Int32 TAG_SECURITY_ATTR_3 = 0xAB;
    private static const Int32 TAG_FILE_SIZE = 0x80;
    private static const Int32 TAG_TOTAL_FILE_SIZE = 0x81;
    private static const Int32 TAG_SHORT_FILE_IDENTIFIER = 0x88;

    private static const Int32 TYPE_5 = 5;
    private static const Int32 TYPE_2 = 2;

    Int32 mRecordSize;
    Int32 mFileSize;
    Int32 mNumRecords;
    Boolean mIsDataEnough;

    private Int32 mFileType = -1;

    private UiccTlvData() {
        mNumRecords = -1;
        mFileSize = -1;
        mRecordSize = -1;
    }

    public Boolean IsIncomplete() {
        return mNumRecords == -1 || mFileSize == -1 || mRecordSize == -1 || mFileType == -1;
    }

    public static Boolean IsUiccTlvData(Byte[] data) {
        If(data != NULL && data.length > 0 && TLV_FORMAT_ID == (data[0] & 0xFF)) {
            return TRUE;
        }
        return FALSE;
    }

    public static UiccTlvData Parse(Byte[] data) throws IccFileTypeMismatch{

        UiccTlvData parsedData = new UiccTlvData();

        If (data == NULL || data.length == 0 || TLV_FORMAT_ID != (data[0] & 0xFF)) {
            throw new IccFileTypeMismatch();
        }

        try {

            Int32 currentLocation = 2; //Ignore FCP size
            Int32 currentTag;

            While (currentLocation < data.length) {
                currentTag = data[currentLocation++] & 0xFF;

                Switch (currentTag) {
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
                        Rlog->D(LOG_TAG, "Unknown tag 0x" + String->Format("%02X", currentTag));
                        currentLocation = parsedData->ParseSomeTag(data, currentLocation);
                        break;
                }
            }

        } Catch (ArrayIndexOutOfBoundsException e) {

            //We might be looking at incomplete data but we might have what we need.
            //Ignore this  and let caller handle it by checking isIncomplete
        }

        return parsedData;
    }

    private Int32 ParseFileSize(Byte[] data, Int32 currentLocation) {
        Int32 length = data[currentLocation++] & 0xFF;

        Int32 fileSize = 0;
        For (Int32 i = 0; i < length; i++) {
            fileSize += ((data[currentLocation + i] & 0xFF) << ( 8 * (length - i - 1)));
        }

        mFileSize = fileSize;

        If (mFileType == TYPE_2) {
            mRecordSize = fileSize;
        }

        return currentLocation + length;
    }

    private Int32 ParseSomeTag(Byte[] data, Int32 currentLocation) {
        //Just skip unwanted tags;
        Int32 length = data[currentLocation++] & 0xFF;
        return currentLocation + length;
    }

    private Int32 ParseFileDescriptor(Byte[] data, Int32 currentLocation) throws IccFileTypeMismatch {
        Int32 length = data[currentLocation++] & 0xFF;
        If (length == 5) {

            mRecordSize = ((data[currentLocation + 2] & 0xFF) << 8) +
                    (data[currentLocation + 3] & 0xFF); // Length of 1 record
            mNumRecords = data[currentLocation + 4] & 0xFF; // Number of records

            mFileSize = mRecordSize * mNumRecords;

            mFileType = TYPE_5;

            return currentLocation + 5;
        } else If (length == 2) {

            Int32 descriptorByte = data[currentLocation + 1] & 0xFF;

            //Ignore descriptorByte for now

            mNumRecords = 1;

            mFileType = TYPE_2;

            return currentLocation + 2;

        } else {
            throw new IccFileTypeMismatch();
        }
    }

}

