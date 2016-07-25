
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/internal/telephony/uicc/UiccIccUtils.h"
#include "elastos/droid/graphics/CBitmapHelper.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::R;

using Elastos::Core::StringBuilder;
using Elastos::IO::Charset::ICharset;
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
    StringBuilder ret(length*2);

    for (Int32 i = offset; i < offset + length; i++) {
        Int32 v = 0;

        v = (*data)[i] & 0xf;
        if (v > 9)  break;
        ret.Append((Int32)('0' + v));

        v = ((*data)[i] >> 4) & 0xf;
        // Some PLMNs have 'f' as high nibble, ignore it
        if (v == 0xf) continue;
        if (v > 9) break;
        ret.Append((Int32)('0' + v));
    }

    return ret.ToString();
}

String UiccIccUtils::CdmaBcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    StringBuilder ret(length);

    Int32 count = 0;
    for (Int32 i = offset; count < length; i++) {
        Int32 v = 0;
        v = (*data)[i] & 0xf;
        if (v > 9)  v = 0;
        ret.Append((Int32)('0' + v));

        if (++count == length) break;

        v = ((*data)[i] >> 4) & 0xf;
        if (v > 9) v = 0;
        ret.Append((Int32)('0' + v));
        ++count;
    }
    return ret.ToString();
}

Int32 UiccIccUtils::GsmBcdByteToInt(
    /* [in] */ Byte b)
{
    Int32 ret = 0;

    // treat out-of-range BCD values as 0
    if ((b & 0xf0) <= 0x90) {
        ret = (b >> 4) & 0xf;
    }

    if ((b & 0x0f) <= 0x09) {
        ret += (b & 0xf) * 10;
    }

    return ret;
}

Int32 UiccIccUtils::CdmaBcdByteToInt(
    /* [in] */ Byte b)
{
    Int32 ret = 0;

    // treat out-of-range BCD values as 0
    if ((b & 0xf0) <= 0x90) {
        ret = ((b >> 4) & 0xf) * 10;
    }

    if ((b & 0x0f) <= 0x09) {
        ret += (b & 0xf);
    }

    return ret;
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
        if ((*data)[offset] == (Byte) 0x80) {
            Int32 ucslen = (length - 1) / 2;
            String ret(NULL);

            // try {
                ret = String(*data, offset + 1, ucslen * 2); // String("utf-16be")
            // } catch (UnsupportedEncodingException ex) {
            //     Rlog.e(LOGTAG, "implausible UnsupportedEncodingException",
            //           ex);
            // }

            if (ret != NULL) {
                // trim off trailing FFFF characters

                ucslen = ret.GetLength();
                while (ucslen > 0 && ret.GetChar(ucslen - 1) == '\uFFFF') {
                    ucslen--;
                }

                return ret.Substring(0, ucslen);
            }
        }
    }

    Boolean isucs2 = FALSE;
    Char32 base = '\0';
    Int32 len = 0;

    if (length >= 3 && (*data)[offset] == (Byte) 0x81) {
        len = (*data)[offset + 1] & 0xFF;
        if (len > length - 3)
            len = length - 3;

        base = (Char32) (((*data)[offset + 2] & 0xFF) << 7);
        offset += 3;
        isucs2 = TRUE;
    }
    else if (length >= 4 && (*data)[offset] == (Byte) 0x82) {
        len = (*data)[offset + 1] & 0xFF;
        if (len > length - 4)
            len = length - 4;

        base = (Char32) ((((*data)[offset + 2] & 0xFF) << 8) |
                        ((*data)[offset + 3] & 0xFF));
        offset += 4;
        isucs2 = TRUE;
    }

    AutoPtr<IGsmAlphabet> gsmhlp;
    CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsmhlp);
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
            while (count < len && (*data)[offset + count] >= 0) {
                count++;
            }

            String str;
            gsmhlp->Gsm8BitUnpackedToString(data,
                       offset, count, &str);
            ret.Append(str);

            offset += count;
            len -= count;
        }

        return ret.ToString();
    }

    AutoPtr<IResourcesHelper> reshlp;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&reshlp);
    AutoPtr<IResources> resource;
    reshlp->GetSystem((IResources**)&resource);
    String defaultCharset("");
    // try {
    resource->GetString(R::string::gsm_alphabet_default_charset, &defaultCharset);
    // } catch (NotFoundException e) {
    //     // Ignore Exception and defaultCharset is set to a empty string.
    // }
    String result;
    gsmhlp->Gsm8BitUnpackedToString(data, offset, length, defaultCharset.Trim(), &result);
    return result;
}

Int32 UiccIccUtils::HexCharToInt32(
    /* [in] */ Char16 c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);

    // throw new RuntimeException ("invalid hex Char32 '" + c + "'");
    assert(0 && "RuntimeException");
    return 0;
}

AutoPtr<ArrayOf<Byte> > UiccIccUtils::HexStringToBytes(
    /* [in] */ const String& s)
{
    AutoPtr<ArrayOf<Byte> > ret;

    if (s.IsNull()) return NULL;

    Int32 sz = s.GetLength();

    ret = ArrayOf<Byte>::Alloc(sz/2);

    for (Int32 i = 0; i < sz; i += 2) {
        (*ret)[i/2] = (Byte) ((HexCharToInt32(s.GetChar(i)) << 4)
                            | HexCharToInt32(s.GetChar(i + 1)));
    }

    return ret;
}

String UiccIccUtils::BytesToHexString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    if (bytes == NULL) return String(NULL);

    StringBuilder ret(2 * bytes->GetLength());

    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        Int32 b = 0;

        b = 0x0f & ((*bytes)[i] >> 4);

        ret.Append((Int32)(String("0123456789abcdef").GetChar(b)));

        b = 0x0f & (*bytes)[i];

        ret.Append((Int32)(String("0123456789abcdef").GetChar(b)));
    }

    return ret.ToString();
}

String UiccIccUtils::NetworkNameToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    String ret;

    if (((*data)[offset] & 0x80) != 0x80 || length < 1) {
        return String("");
    }

    switch (((*data)[offset] >> 4) & 0x7) {
        case 0: {
            // SMS character set
            Int32 countSeptets;
            Int32 unusedBits = (*data)[offset] & 7;
            countSeptets = (((length - 1) * 8) - unusedBits) / 7 ;
            AutoPtr<IGsmAlphabet> gsmhlp;
            CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsmhlp);
            gsmhlp->Gsm7BitPackedToString(data, offset + 1, countSeptets, &ret);
        break;
        }
        case 1: {
            // UCS2
            // try {
            ret = String(*data,
                    offset + 1, length - 1); // "utf-16"
            // } catch (UnsupportedEncodingException ex) {
            //     ret = "";
            //     Rlog.e(LOGTAG,"implausible UnsupportedEncodingException", ex);
            // }
        break;
        }
        // unsupported encoding
        default: {
            ret = String("");
        break;
        }
    }

    // "Add CI"
    // "The MS should add the letters for the Country's Initials and
    //  a separator (e.g. a space) to the text string"

    if (((*data)[offset] & 0x40) != 0) {
        // FIXME(mkf) add country initials here

    }

    return ret;
}

AutoPtr<IBitmap> UiccIccUtils::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 numOfPixels = width * height;

    AutoPtr<ArrayOf<Int32> > pixels = ArrayOf<Int32>::Alloc(numOfPixels);

    Int32 pixelIndex = 0;
    Int32 bitIndex = 7;
    Byte currentByte = 0x00;
    while (pixelIndex < numOfPixels) {
        // reassign data and index for every Byte (8 bits).
        if (pixelIndex % 8 == 0) {
            currentByte = (*data)[valueIndex++];
            bitIndex = 7;
        }
        (*pixels)[pixelIndex++] = BitToRGB((currentByte >> bitIndex-- ) & 0x01);
    }

    if (pixelIndex != numOfPixels) {
        Logger::E(LOGTAG, String("parse end and size error"));
    }
    AutoPtr<IBitmapHelper> bmhlp;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bmhlp);
    AutoPtr<IBitmap> bm;
    bmhlp->CreateBitmap(pixels, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bm);
    return bm;
}

AutoPtr<IBitmap> UiccIccUtils::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 bits = (*data)[valueIndex++] & 0xFF;
    Int32 colorNumber = (*data)[valueIndex++] & 0xFF;
    Int32 num1 = (((*data)[valueIndex++] & 0xFF) << 8);
    Int32 num2 = ((*data)[valueIndex++] & 0xFF);
    Int32 clutOffset = num1 | num2;

    AutoPtr<ArrayOf<Int32> > colorIndexArray = GetCLUT(data, clutOffset, colorNumber);
    if (TRUE == transparency) {
        (*colorIndexArray)[colorNumber - 1] = IColor::TRANSPARENT;
    }

    AutoPtr<ArrayOf<Int32> > resultArray = NULL;
    if (0 == (8 % bits)) {
        resultArray = MapTo2OrderBitColor(data, valueIndex,
                (width * height), colorIndexArray, bits);
    }
    else {
        resultArray = MapToNon2OrderBitColor(data, valueIndex,
                (width * height), colorIndexArray, bits);
    }

    AutoPtr<IBitmapHelper> bmhlp;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bmhlp);
    AutoPtr<IBitmap> bm;
    bmhlp->CreateBitmap(resultArray, width, height, BitmapConfig_RGB_565, (IBitmap**)&bm);
    return bm;
}

AutoPtr<ArrayOf<Byte> > UiccIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag)
{
    Boolean isUcs2 = FALSE;
    // try {
        for (Int32 i = 0; i < alphaTag.GetLength(); i++) {
            AutoPtr<IGsmAlphabet> gsmhlp;
            CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsmhlp);
            Int32 res = 0;
            gsmhlp->CountGsmSeptets(alphaTag.GetChar(i), TRUE, &res);
        }
    // } catch (EncodeException e) {
    //     isUcs2 = TRUE;
    // }
    return StringToAdnStringField(alphaTag, isUcs2);
}

AutoPtr<ArrayOf<Byte> > UiccIccUtils::StringToAdnStringField(
    /* [in] */ const String& alphaTag,
    /* [in] */ Boolean isUcs2)
{
    AutoPtr<IGsmAlphabet> gsmhlp;
    CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsmhlp);
    if (!isUcs2) {
        AutoPtr<ArrayOf<Byte> > res;
        gsmhlp->StringToGsm8BitPacked(alphaTag, (ArrayOf<Byte>**)&res);
        return res;
    }
    AutoPtr<ICharset> cs;
    assert(0 && "TODO");
    // Elastos::IO::Charset::Charset::ForName(String("UTF-16BE"), (ICharset**)&cs);
    AutoPtr<ArrayOf<Byte> > alphaTagBytes = alphaTag.GetBytes(); // cs
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(1 + alphaTagBytes->GetLength());
    (*ret)[0] = (Byte)0x80;
    ret->Copy(1, alphaTagBytes, 0, alphaTagBytes->GetLength());
    return ret;
}

Int32 UiccIccUtils::BitToRGB(
    /* [in] */ Int32 bit)
{
    if (bit == 1){
        return IColor::WHITE;
    }
    else {
        return IColor::BLACK;
    }
}

AutoPtr<ArrayOf<Int32> > UiccIccUtils::MapTo2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    if (0 != (8 % bits)) {
        Logger::E(LOGTAG, String("not event number of color"));
        return MapToNon2OrderBitColor(data, valueIndex, length, colorArray,
                bits);
    }

    Int32 mask = 0x01;
    switch (bits) {
    case 1:
        mask = 0x01;
        break;
    case 2:
        mask = 0x03;
        break;
    case 4:
        mask = 0x0F;
        break;
    case 8:
        mask = 0xFF;
        break;
    }

    AutoPtr<ArrayOf<Int32> > resultArray = ArrayOf<Int32>::Alloc(length);
    Int32 resultIndex = 0;
    Int32 run = 8 / bits;
    while (resultIndex < length) {
        Byte tempByte = (*data)[valueIndex++];
        for (Int32 runIndex = 0; runIndex < run; ++runIndex) {
            Int32 offset = run - runIndex - 1;
            (*resultArray)[resultIndex++] = (*colorArray)[(tempByte >> (offset * bits))
                    & mask];
        }
    }
    return resultArray;
}

AutoPtr<ArrayOf<Int32> > UiccIccUtils::MapToNon2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    if (0 == (8 % bits)) {
        Logger::E(LOGTAG, String("not odd number of color"));
        return MapTo2OrderBitColor(data, valueIndex, length, colorArray,
                bits);
    }

    AutoPtr<ArrayOf<Int32> > resultArray = ArrayOf<Int32>::Alloc(length);
    // TODO fix me:
    return resultArray;
}

AutoPtr<ArrayOf<Int32> > UiccIccUtils::GetCLUT(
    /* [in] */ ArrayOf<Byte>* rawData,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 number)
{
    if (NULL == rawData) {
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(number);
    Int32 endIndex = offset + (number * 3); // 1 color use 3 bytes
    Int32 valueIndex = offset;
    Int32 colorIndex = 0;
    Int32 alpha = 0xff << 24;
    do {
        (*result)[colorIndex++] = alpha
                | (((*result)[valueIndex++] & 0xFF) << 16)
                | (((*result)[valueIndex++] & 0xFF) << 8)
                | (((*result)[valueIndex++] & 0xFF));
    } while (valueIndex < endIndex);
    return result;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
