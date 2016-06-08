
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/ResponseData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                             ResponseData
//=====================================================================
ECode ResponseData::WriteLength(
    /* [in] */ IByteArrayOutputStream* buf,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // // As per ETSI 102.220 Sec7.1.2, if the total length is greater
    // // than 0x7F, it should be coded in two bytes and the first byte
    // // should be 0x81.
    // if (length > 0x7F) {
    //     buf.write(0x81);
    // }
    // buf.write(length);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                        SelectItemResponseData
//=====================================================================
SelectItemResponseData::SelectItemResponseData(
    /* [in] */ Int32 id)
{
    // ==================before translated======================
    // super();
    // mId = id;
}

ECode SelectItemResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // ==================before translated======================
    // // Item identifier object
    // int tag = 0x80 | ComprehensionTlvTag.ITEM_ID.value();
    // buf.write(tag); // tag
    // buf.write(1); // length
    // buf.write(mId); // identifier of item chosen
    assert(0);
    return NOERROR;
}

//=====================================================================
//                      GetInkeyInputResponseData
//=====================================================================
const Byte GetInkeyInputResponseData::GET_INKEY_YES = 0x01;
const Byte GetInkeyInputResponseData::GET_INKEY_NO = 0x00;

GetInkeyInputResponseData::GetInkeyInputResponseData(
    /* [in] */ const String& inData,
    /* [in] */ Boolean ucs2,
    /* [in] */ Boolean packed)
{
    // ==================before translated======================
    // super();
    // mIsUcs2 = ucs2;
    // mIsPacked = packed;
    // mInData = inData;
    // mIsYesNo = false;
}

GetInkeyInputResponseData::GetInkeyInputResponseData(
    /* [in] */ Boolean yesNoResponse)
{
    // ==================before translated======================
    // super();
    // mIsUcs2 = false;
    // mIsPacked = false;
    // mInData = "";
    // mIsYesNo = true;
    // mYesNoResponse = yesNoResponse;
}

ECode GetInkeyInputResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // ==================before translated======================
    // if (buf == null) {
    //     return;
    // }
    //
    // // Text string object
    // int tag = 0x80 | ComprehensionTlvTag.TEXT_STRING.value();
    // buf.write(tag); // tag
    //
    // byte[] data;
    //
    // if (mIsYesNo) {
    //     data = new byte[1];
    //     data[0] = mYesNoResponse ? GET_INKEY_YES : GET_INKEY_NO;
    // } else if (mInData != null && mInData.length() > 0) {
    //     try {
    //         // ETSI TS 102 223 8.15, should use the same format as in SMS messages
    //         // on the network.
    //         if (mIsUcs2) {
    //             // ucs2 is by definition big endian.
    //             data = mInData.getBytes("UTF-16BE");
    //         } else if (mIsPacked) {
    //             int size = mInData.length();
    //
    //             byte[] tempData = GsmAlphabet
    //                     .stringToGsm7BitPacked(mInData, 0, 0);
    //             data = new byte[size];
    //             // Since stringToGsm7BitPacked() set byte 0 in the
    //             // returned byte array to the count of septets used...
    //             // copy to a new array without byte 0.
    //             System.arraycopy(tempData, 1, data, 0, size);
    //         } else {
    //             data = GsmAlphabet.stringToGsm8BitPacked(mInData);
    //         }
    //     } catch (UnsupportedEncodingException e) {
    //         data = new byte[0];
    //     } catch (EncodeException e) {
    //         data = new byte[0];
    //     }
    // } else {
    //     data = new byte[0];
    // }
    //
    // // length - one more for data coding scheme.
    // writeLength(buf, data.length + 1);
    //
    // // data coding scheme
    // if (mIsUcs2) {
    //     buf.write(0x08); // UCS2
    // } else if (mIsPacked) {
    //     buf.write(0x00); // 7 bit packed
    // } else {
    //     buf.write(0x04); // 8 bit unpacked
    // }
    //
    // for (byte b : data) {
    //     buf.write(b);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                         LanguageResponseData
//=====================================================================
LanguageResponseData::LanguageResponseData(
    /* [in] */ const String& lang)
{
    // ==================before translated======================
    // super();
    // mLang = lang;
}

ECode LanguageResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // ==================before translated======================
    // if (buf == null) {
    //     return;
    // }
    //
    // // Text string object
    // int tag = 0x80 | ComprehensionTlvTag.LANGUAGE.value();
    // buf.write(tag); // tag
    //
    // byte[] data;
    //
    // if (mLang != null && mLang.length() > 0) {
    //     data = GsmAlphabet.stringToGsm8BitPacked(mLang);
    // }
    // else {
    //     data = new byte[0];
    // }
    //
    // buf.write(data.length);
    //
    // for (byte b : data) {
    //     buf.write(b);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                           DTTZResponseData
//=====================================================================
DTTZResponseData::DTTZResponseData(
    /* [in] */ ICalendar* cal)
{
    // ==================before translated======================
    // super();
    // mCalendar = cal;
}

ECode DTTZResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // ==================before translated======================
    // if (buf == null) {
    //     return;
    // }
    //
    // // DTTZ object
    // int tag = 0x80 | CommandType.PROVIDE_LOCAL_INFORMATION.value();
    // buf.write(tag); // tag
    //
    // byte[] data = new byte[8];
    //
    // data[0] = 0x07; // Write length of DTTZ data
    //
    // if (mCalendar == null) {
    //     mCalendar = Calendar.getInstance();
    // }
    // // Fill year byte
    // data[1] = byteToBCD(mCalendar.get(java.util.Calendar.YEAR) % 100);
    //
    // // Fill month byte
    // data[2] = byteToBCD(mCalendar.get(java.util.Calendar.MONTH) + 1);
    //
    // // Fill day byte
    // data[3] = byteToBCD(mCalendar.get(java.util.Calendar.DATE));
    //
    // // Fill hour byte
    // data[4] = byteToBCD(mCalendar.get(java.util.Calendar.HOUR_OF_DAY));
    //
    // // Fill minute byte
    // data[5] = byteToBCD(mCalendar.get(java.util.Calendar.MINUTE));
    //
    // // Fill second byte
    // data[6] = byteToBCD(mCalendar.get(java.util.Calendar.SECOND));
    //
    // String tz = SystemProperties.get("persist.sys.timezone", "");
    // if (TextUtils.isEmpty(tz)) {
    //     data[7] = (byte) 0xFF;    // set FF in terminal response
    // } else {
    //     TimeZone zone = TimeZone.getTimeZone(tz);
    //     int zoneOffset = zone.getRawOffset() + zone.getDSTSavings();
    //     data[7] = getTZOffSetByte(zoneOffset);
    // }
    //
    // for (byte b : data) {
    //     buf.write(b);
    // }
    assert(0);
    return NOERROR;
}

Byte DTTZResponseData::ByteToBCD(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // if (value < 0 && value > 99) {
    //     CatLog.d(this, "Err: byteToBCD conversion Value is " + value +
    //                    " Value has to be between 0 and 99");
    //     return 0;
    // }
    //
    // return (byte) ((value / 10) | ((value % 10) << 4));
    assert(0);
    return 0;
}

Byte DTTZResponseData::GetTZOffSetByte(
    /* [in] */ Int64 offSetVal)
{
    // ==================before translated======================
    // boolean isNegative = (offSetVal < 0);
    //
    // /*
    //  * The 'offSetVal' is in milliseconds. Convert it to hours and compute
    //  * offset While sending T.R to UICC, offset is expressed is 'quarters of
    //  * hours'
    //  */
    //
    //  long tzOffset = offSetVal / (15 * 60 * 1000);
    //  tzOffset = (isNegative ? -1 : 1) * tzOffset;
    //  byte bcdVal = byteToBCD((int) tzOffset);
    //  // For negative offsets, put '1' in the msb
    //  return isNegative ?  (bcdVal |= 0x08) : bcdVal;
    assert(0);
    return 0;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
