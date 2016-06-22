
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccIccUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                               UiccIccUtils
//=====================================================================
CAR_INTERFACE_IMPL(UiccIccUtils, Object, IIccUtils);

const String UiccIccUtils::LOGTAG("UiccIccUtils");

String UiccIccUtils::BcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // StringBuilder ret = new StringBuilder(length*2);
    //
    // for (int i = offset ; i < offset + length ; i++) {
    //     byte b;
    //     int v;
    //
    //     v = data[i] & 0xf;
    //     if (v > 9)  break;
    //     ret.append((char)('0' + v));
    //
    //     v = (data[i] >> 4) & 0xf;
    //     // Some PLMNs have 'f' as high nibble, ignore it
    //     if (v == 0xf) continue;
    //     if (v > 9)  break;
    //     ret.append((char)('0' + v));
    // }
    //
    // return ret.toString();
    assert(0);
    return String("");
}

String UiccIccUtils::CdmaBcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // StringBuilder ret = new StringBuilder(length);
    //
    // int count = 0;
    // for (int i = offset; count < length; i++) {
    //     int v;
    //     v = data[i] & 0xf;
    //     if (v > 9)  v = 0;
    //     ret.append((char)('0' + v));
    //
    //     if (++count == length) break;
    //
    //     v = (data[i] >> 4) & 0xf;
    //     if (v > 9)  v = 0;
    //     ret.append((char)('0' + v));
    //     ++count;
    // }
    // return ret.toString();
    assert(0);
    return String("");
}

Int32 UiccIccUtils::GsmBcdByteToInt(
    /* [in] */ Byte b)
{
    // ==================before translated======================
    // int ret = 0;
    //
    // // treat out-of-range BCD values as 0
    // if ((b & 0xf0) <= 0x90) {
    //     ret = (b >> 4) & 0xf;
    // }
    //
    // if ((b & 0x0f) <= 0x09) {
    //     ret +=  (b & 0xf) * 10;
    // }
    //
    // return ret;
    assert(0);
    return 0;
}

Int32 UiccIccUtils::CdmaBcdByteToInt(
    /* [in] */ Byte b)
{
    // ==================before translated======================
    // int ret = 0;
    //
    // // treat out-of-range BCD values as 0
    // if ((b & 0xf0) <= 0x90) {
    //     ret = ((b >> 4) & 0xf) * 10;
    // }
    //
    // if ((b & 0x0f) <= 0x09) {
    //     ret +=  (b & 0xf);
    // }
    //
    // return ret;
    assert(0);
    return 0;
}

String UiccIccUtils::AdnStringFieldToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // if (length == 0) {
    //     return "";
    // }
    // if (length >= 1) {
    //     if (data[offset] == (byte) 0x80) {
    //         int ucslen = (length - 1) / 2;
    //         String ret = null;
    //
    //         try {
    //             ret = new String(data, offset + 1, ucslen * 2, "utf-16be");
    //         } catch (UnsupportedEncodingException ex) {
    //             Rlog.e(LOGTAG, "implausible UnsupportedEncodingException",
    //                   ex);
    //         }
    //
    //         if (ret != null) {
    //             // trim off trailing FFFF characters
    //
    //             ucslen = ret.length();
    //             while (ucslen > 0 && ret.charAt(ucslen - 1) == '\uFFFF')
    //                 ucslen--;
    //
    //             return ret.substring(0, ucslen);
    //         }
    //     }
    // }
    //
    // boolean isucs2 = false;
    // char base = '\0';
    // int len = 0;
    //
    // if (length >= 3 && data[offset] == (byte) 0x81) {
    //     len = data[offset + 1] & 0xFF;
    //     if (len > length - 3)
    //         len = length - 3;
    //
    //     base = (char) ((data[offset + 2] & 0xFF) << 7);
    //     offset += 3;
    //     isucs2 = true;
    // } else if (length >= 4 && data[offset] == (byte) 0x82) {
    //     len = data[offset + 1] & 0xFF;
    //     if (len > length - 4)
    //         len = length - 4;
    //
    //     base = (char) (((data[offset + 2] & 0xFF) << 8) |
    //                     (data[offset + 3] & 0xFF));
    //     offset += 4;
    //     isucs2 = true;
    // }
    //
    // if (isucs2) {
    //     StringBuilder ret = new StringBuilder();
    //
    //     while (len > 0) {
    //         // UCS2 subset case
    //
    //         if (data[offset] < 0) {
    //             ret.append((char) (base + (data[offset] & 0x7F)));
    //             offset++;
    //             len--;
    //         }
    //
    //         // GSM character set case
    //
    //         int count = 0;
    //         while (count < len && data[offset + count] >= 0)
    //             count++;
    //
    //         ret.append(GsmAlphabet.gsm8BitUnpackedToString(data,
    //                    offset, count));
    //
    //         offset += count;
    //         len -= count;
    //     }
    //
    //     return ret.toString();
    // }
    //
    // Resources resource = Resources.getSystem();
    // String defaultCharset = "";
    // try {
    //     defaultCharset =
    //             resource.getString(com.android.internal.R.string.gsm_alphabet_default_charset);
    // } catch (NotFoundException e) {
    //     // Ignore Exception and defaultCharset is set to a empty string.
    // }
    // return GsmAlphabet.gsm8BitUnpackedToString(data, offset, length, defaultCharset.trim());
    assert(0);
    return String("");
}

Int32 UiccIccUtils::HexCharToInt(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (c >= '0' && c <= '9') return (c - '0');
    // if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    // if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    //
    // throw new RuntimeException ("invalid hex char '" + c + "'");
    assert(0);
    return 0;
}

AutoPtr< ArrayOf<Byte> > UiccIccUtils::HexStringToBytes(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // byte[] ret;
    //
    // if (s == null) return null;
    //
    // int sz = s.length();
    //
    // ret = new byte[sz/2];
    //
    // for (int i=0 ; i <sz ; i+=2) {
    //     ret[i/2] = (byte) ((hexCharToInt(s.charAt(i)) << 4)
    //                         | hexCharToInt(s.charAt(i+1)));
    // }
    //
    // return ret;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

String UiccIccUtils::BytesToHexString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // ==================before translated======================
    // if (bytes == null) return null;
    //
    // StringBuilder ret = new StringBuilder(2*bytes.length);
    //
    // for (int i = 0 ; i < bytes.length ; i++) {
    //     int b;
    //
    //     b = 0x0f & (bytes[i] >> 4);
    //
    //     ret.append("0123456789abcdef".charAt(b));
    //
    //     b = 0x0f & bytes[i];
    //
    //     ret.append("0123456789abcdef".charAt(b));
    // }
    //
    // return ret.toString();
    assert(0);
    return String("");
}

String UiccIccUtils::NetworkNameToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // String ret;
    //
    // if ((data[offset] & 0x80) != 0x80 || length < 1) {
    //     return "";
    // }
    //
    // switch ((data[offset] >>> 4) & 0x7) {
    //     case 0:
    //         // SMS character set
    //         int countSeptets;
    //         int unusedBits = data[offset] & 7;
    //         countSeptets = (((length - 1) * 8) - unusedBits) / 7 ;
    //         ret =  GsmAlphabet.gsm7BitPackedToString(data, offset + 1, countSeptets);
    //     break;
    //     case 1:
    //         // UCS2
    //         try {
    //             ret = new String(data,
    //                     offset + 1, length - 1, "utf-16");
    //         } catch (UnsupportedEncodingException ex) {
    //             ret = "";
    //             Rlog.e(LOGTAG,"implausible UnsupportedEncodingException", ex);
    //         }
    //     break;
    //
    //     // unsupported encoding
    //     default:
    //         ret = "";
    //     break;
    // }
    //
    // // "Add CI"
    // // "The MS should add the letters for the Country's Initials and
    // //  a separator (e.g. a space) to the text string"
    //
    // if ((data[offset] & 0x40) != 0) {
    //     // FIXME(mkf) add country initials here
    //
    // }
    //
    // return ret;
    assert(0);
    return String("");
}

AutoPtr<IBitmap> UiccIccUtils::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // int valueIndex = 0;
    // int width = data[valueIndex++] & 0xFF;
    // int height = data[valueIndex++] & 0xFF;
    // int numOfPixels = width*height;
    //
    // int[] pixels = new int[numOfPixels];
    //
    // int pixelIndex = 0;
    // int bitIndex = 7;
    // byte currentByte = 0x00;
    // while (pixelIndex < numOfPixels) {
    //     // reassign data and index for every byte (8 bits).
    //     if (pixelIndex % 8 == 0) {
    //         currentByte = data[valueIndex++];
    //         bitIndex = 7;
    //     }
    //     pixels[pixelIndex++] = bitToRGB((currentByte >> bitIndex-- ) & 0x01);
    // }
    //
    // if (pixelIndex != numOfPixels) {
    //     Rlog.e(LOGTAG, "parse end and size error");
    // }
    // return Bitmap.createBitmap(pixels, width, height, Bitmap.Config.ARGB_8888);
    assert(0);
    AutoPtr<IBitmap> empty;
    return empty;
}

AutoPtr<IBitmap> UiccIccUtils::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency)
{
    // ==================before translated======================
    // int valueIndex = 0;
    // int width = data[valueIndex++] & 0xFF;
    // int height = data[valueIndex++] & 0xFF;
    // int bits = data[valueIndex++] & 0xFF;
    // int colorNumber = data[valueIndex++] & 0xFF;
    // int clutOffset = ((data[valueIndex++] & 0xFF) << 8)
    //         | (data[valueIndex++] & 0xFF);
    //
    // int[] colorIndexArray = getCLUT(data, clutOffset, colorNumber);
    // if (true == transparency) {
    //     colorIndexArray[colorNumber - 1] = Color.TRANSPARENT;
    // }
    //
    // int[] resultArray = null;
    // if (0 == (8 % bits)) {
    //     resultArray = mapTo2OrderBitColor(data, valueIndex,
    //             (width * height), colorIndexArray, bits);
    // } else {
    //     resultArray = mapToNon2OrderBitColor(data, valueIndex,
    //             (width * height), colorIndexArray, bits);
    // }
    //
    // return Bitmap.createBitmap(resultArray, width, height,
    //         Bitmap.Config.RGB_565);
    assert(0);
    AutoPtr<IBitmap> empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > UiccIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag)
{
    // ==================before translated======================
    // boolean isUcs2 = false;
    // try {
    //    for(int i = 0; i < alphaTag.length(); i++) {
    //        GsmAlphabet.countGsmSeptets(alphaTag.charAt(i), true);
    //    }
    // } catch (EncodeException e) {
    //     isUcs2 = true;
    // }
    // return stringToAdnStringField(alphaTag, isUcs2);
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > UiccIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag,
    /* [in] */ Boolean isUcs2)
{
    // ==================before translated======================
    // if (!isUcs2) {
    //     return GsmAlphabet.stringToGsm8BitPacked(alphaTag);
    // }
    // byte[] alphaTagBytes = alphaTag.getBytes(Charset.forName("UTF-16BE"));
    // byte[] ret = new byte[1 + alphaTagBytes.length];
    // ret[0] = (byte)0x80;
    // System.arraycopy(alphaTagBytes, 0, ret, 1, alphaTagBytes.length);
    //
    // return ret;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

Int32 UiccIccUtils::BitToRGB(
    /* [in] */ Int32 bit)
{
    // ==================before translated======================
    // if(bit == 1){
    //     return Color.WHITE;
    // } else {
    //     return Color.BLACK;
    // }
    assert(0);
    return 0;
}

AutoPtr< ArrayOf<Int32> > UiccIccUtils::MapTo2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    // ==================before translated======================
    // if (0 != (8 % bits)) {
    //     Rlog.e(LOGTAG, "not event number of color");
    //     return mapToNon2OrderBitColor(data, valueIndex, length, colorArray,
    //             bits);
    // }
    //
    // int mask = 0x01;
    // switch (bits) {
    // case 1:
    //     mask = 0x01;
    //     break;
    // case 2:
    //     mask = 0x03;
    //     break;
    // case 4:
    //     mask = 0x0F;
    //     break;
    // case 8:
    //     mask = 0xFF;
    //     break;
    // }
    //
    // int[] resultArray = new int[length];
    // int resultIndex = 0;
    // int run = 8 / bits;
    // while (resultIndex < length) {
    //     byte tempByte = data[valueIndex++];
    //     for (int runIndex = 0; runIndex < run; ++runIndex) {
    //         int offset = run - runIndex - 1;
    //         resultArray[resultIndex++] = colorArray[(tempByte >> (offset * bits))
    //                 & mask];
    //     }
    // }
    // return resultArray;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr< ArrayOf<Int32> > UiccIccUtils::MapToNon2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    // ==================before translated======================
    // if (0 == (8 % bits)) {
    //     Rlog.e(LOGTAG, "not odd number of color");
    //     return mapTo2OrderBitColor(data, valueIndex, length, colorArray,
    //             bits);
    // }
    //
    // int[] resultArray = new int[length];
    // // TODO fix me:
    // return resultArray;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr< ArrayOf<Int32> > UiccIccUtils::GetCLUT(
    /* [in] */ ArrayOf<Byte>* rawData,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 number)
{
    // ==================before translated======================
    // if (null == rawData) {
    //     return null;
    // }
    //
    // int[] result = new int[number];
    // int endIndex = offset + (number * 3); // 1 color use 3 bytes
    // int valueIndex = offset;
    // int colorIndex = 0;
    // int alpha = 0xff << 24;
    // do {
    //     result[colorIndex++] = alpha
    //             | ((rawData[valueIndex++] & 0xFF) << 16)
    //             | ((rawData[valueIndex++] & 0xFF) << 8)
    //             | ((rawData[valueIndex++] & 0xFF));
    // } while (valueIndex < endIndex);
    // return result;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
