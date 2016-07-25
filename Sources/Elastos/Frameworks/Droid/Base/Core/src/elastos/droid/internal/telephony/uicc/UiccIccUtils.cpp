
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccIccUtils.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/content/res/CResources.h"
#include <elastos/droid/R.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

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
    // for (Int32 i = offset ; i < offset + length ; i++) {
    //     byte b;
    //     Int32 v;
    //
    //     v = data[i] & 0xf;
    //     if (v > 9)  break;
    //     ret.append((Char32)('0' + v));
    //
    //     v = (data[i] >> 4) & 0xf;
    //     // Some PLMNs have 'f' as high nibble, ignore it
    //     if (v == 0xf) continue;
    //     if (v > 9)  break;
    //     ret.append((Char32)('0' + v));
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
    // Int32 count = 0;
    // for (Int32 i = offset; count < length; i++) {
    //     Int32 v;
    //     v = data[i] & 0xf;
    //     if (v > 9)  v = 0;
    //     ret.append((Char32)('0' + v));
    //
    //     if (++count == length) break;
    //
    //     v = (data[i] >> 4) & 0xf;
    //     if (v > 9)  v = 0;
    //     ret.append((Char32)('0' + v));
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
    // Int32 ret = 0;
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
    // Int32 ret = 0;
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
    if (length == 0) {
        return String("");
    }
    if (length >= 1) {
        if ((*data)[offset] == (byte) 0x80) {
            Int32 ucslen = (length - 1) / 2;
            String ret;

            // try {
            // ret = new String(data, offset + 1, ucslen * 2, "utf-16be");
            ret = String(*data, offset + 1, ucslen * 2);
            // } catch (UnsupportedEncodingException ex) {
            //     Rlog.e(LOGTAG, "implausible UnsupportedEncodingException",
            //           ex);
            // }

            if (ret != NULL) {
                // trim off trailing FFFF characters
                ucslen = ret.GetLength();
                while (ucslen > 0 && ret.GetChar(ucslen - 1) == 0xFFFF)
                    ucslen--;

                return ret.Substring(0, ucslen);
            }
        }
    }

    Boolean isucs2 = false;
    Char32 base = '\0';
    Int32 len = 0;

    if (length >= 3 && (*data)[offset] == (byte) 0x81) {
        len = (*data)[offset + 1] & 0xFF;
        if (len > length - 3)
            len = length - 3;

        base = (Char32) (((*data)[offset + 2] & 0xFF) << 7);
        offset += 3;
        isucs2 = TRUE;
    }
    else if (length >= 4 && (*data)[offset] == (byte) 0x82) {
        len = (*data)[offset + 1] & 0xFF;
        if (len > length - 4)
            len = length - 4;

        base = (Char32) ((((*data)[offset + 2] & 0xFF) << 8) |
                        ((*data)[offset + 3] & 0xFF));
        offset += 4;
        isucs2 = TRUE;
    }

    if (isucs2) {
        StringBuilder ret;

        while (len > 0) {
            // UCS2 subset case

            if ((*data)[offset] < 0) {
                ret.AppendChar((Char32) (base + ((*data)[offset] & 0x7F)));
                offset++;
                len--;
            }

            // GSM character set case

            Int32 count = 0;
            while (count < len && (*data)[offset + count] >= 0)
                count++;

            String v;
            GsmAlphabet::Gsm8BitUnpackedToString(data, offset, count, &v);
            ret.Append(v);

            offset += count;
            len -= count;
        }

        return ret.ToString();
    }

    AutoPtr<IResources> resource = CResources::GetSystem();
    String defaultCharset("");
    // try {
    resource->GetString(R::string::gsm_alphabet_default_charset, &defaultCharset);
    // } catch (NotFoundException e) {
    //     // Ignore Exception and defaultCharset is set to a empty string.
    // }
    String v;
    GsmAlphabet::Gsm8BitUnpackedToString(data, offset, length, defaultCharset.Trim(), &v);
    return v;
}

Int32 UiccIccUtils::HexCharToInt(
    /* [in] */ Char32 c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);

    // throw new RuntimeException ("invalid hex Char32 '" + c + "'");
    assert(0 && "RuntimeException");
    return 0;
}

AutoPtr< ArrayOf<Byte> > UiccIccUtils::HexStringToBytes(
    /* [in] */ const String& s)
{
    AutoPtr< ArrayOf<Byte> > ret;

    if (s == NULL) return NULL;

    Int32 sz = s.GetLength();

    ret = ArrayOf<Byte>::Alloc(sz / 2);

    for (Int32 i = 0 ; i < sz ; i += 2) {
        (*ret)[i / 2] = (Byte) ((HexCharToInt(s.GetChar(i)) << 4)
                            | HexCharToInt(s.GetChar(i + 1)));
    }

    return ret;
}

String UiccIccUtils::BytesToHexString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // ==================before translated======================
    // if (bytes == NULL) return NULL;
    //
    // StringBuilder ret = new StringBuilder(2*bytes.length);
    //
    // for (Int32 i = 0 ; i < bytes.length ; i++) {
    //     Int32 b;
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
    //         Int32 countSeptets;
    //         Int32 unusedBits = data[offset] & 7;
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
    // Int32 valueIndex = 0;
    // Int32 width = data[valueIndex++] & 0xFF;
    // Int32 height = data[valueIndex++] & 0xFF;
    // Int32 numOfPixels = width*height;
    //
    // Int32[] pixels = new Int32[numOfPixels];
    //
    // Int32 pixelIndex = 0;
    // Int32 bitIndex = 7;
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
    // Int32 valueIndex = 0;
    // Int32 width = data[valueIndex++] & 0xFF;
    // Int32 height = data[valueIndex++] & 0xFF;
    // Int32 bits = data[valueIndex++] & 0xFF;
    // Int32 colorNumber = data[valueIndex++] & 0xFF;
    // Int32 clutOffset = ((data[valueIndex++] & 0xFF) << 8)
    //         | (data[valueIndex++] & 0xFF);
    //
    // Int32[] colorIndexArray = getCLUT(data, clutOffset, colorNumber);
    // if (TRUE == transparency) {
    //     colorIndexArray[colorNumber - 1] = Color.TRANSPARENT;
    // }
    //
    // Int32[] resultArray = NULL;
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
    //    for(Int32 i = 0; i < alphaTag.length(); i++) {
    //        GsmAlphabet.countGsmSeptets(alphaTag.charAt(i), TRUE);
    //    }
    // } catch (EncodeException e) {
    //     isUcs2 = TRUE;
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
    // Int32 mask = 0x01;
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
    // Int32[] resultArray = new Int32[length];
    // Int32 resultIndex = 0;
    // Int32 run = 8 / bits;
    // while (resultIndex < length) {
    //     byte tempByte = data[valueIndex++];
    //     for (Int32 runIndex = 0; runIndex < run; ++runIndex) {
    //         Int32 offset = run - runIndex - 1;
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
    // Int32[] resultArray = new Int32[length];
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
    // if (NULL == rawData) {
    //     return NULL;
    // }
    //
    // Int32[] result = new Int32[number];
    // Int32 endIndex = offset + (number * 3); // 1 color use 3 bytes
    // Int32 valueIndex = offset;
    // Int32 colorIndex = 0;
    // Int32 alpha = 0xff << 24;
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
