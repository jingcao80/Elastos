//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::IColor;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String IccUtils::TAG("IccUtils");
String IccUtils::BcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    StringBuilder ret(length * 2);

    for (Int32 i = offset ; i < offset + length ; i++) {
        // Byte b;
        Int32 v;

        v = (*data)[i] & 0xf;
        if (v > 9)  break;
        ret.AppendChar((Char32)('0' + v));

        v = ((*data)[i] >> 4) & 0xf;
        // Some PLMNs have 'f' as high nibble, ignore it
        if (v == 0xf) continue;
        if (v > 9)  break;
        ret.AppendChar((Char32)('0' + v));
    }

    return ret.ToString();
}

String IccUtils::CdmaBcdToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    StringBuilder ret(length);

    Int32 count = 0;
    for (Int32 i = offset; count < length; i++) {
        Int32 v;
        v = (*data)[i] & 0xf;
        if (v > 9)  v = 0;
        ret.AppendChar((Char32)('0' + v));

        if (++count == length) break;

        v = ((*data)[i] >> 4) & 0xf;
        if (v > 9)  v = 0;
        ret.AppendChar((Char32)('0' + v));
        ++count;
    }
    return ret.ToString();
}

Int32 IccUtils::GsmBcdByteToInt(
    /* [in] */ Byte b)
{
    Int32 ret = 0;

    // treat out-of-range BCD values as 0
    if ((b & 0xf0) <= 0x90) {
        ret = (b >> 4) & 0xf;
    }

    if ((b & 0x0f) <= 0x09) {
        ret +=  (b & 0xf) * 10;
    }

    return ret;
}

Int32 IccUtils::CdmaBcdByteToInt(
    /* [in] */ Byte b)
{
    Int32 ret = 0;

    // treat out-of-range BCD values as 0
    if ((b & 0xf0) <= 0x90) {
        ret = ((b >> 4) & 0xf) * 10;
    }

    if ((b & 0x0f) <= 0x09) {
        ret +=  (b & 0xf);
    }

    return ret;
}

String IccUtils::AdnStringFieldToString(
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
            String ret;

            // try {
            Logger::D(TAG, "TODO : has charset problem???");
            // ret = new String(data, offset + 1, ucslen * 2, "utf-16be");
            ret = String(*data, offset + 1, ucslen * 2);
            // } catch (UnsupportedEncodingException ex) {
            //     Rlog.e(TAG, "implausible UnsupportedEncodingException",
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

            String s;
            GsmAlphabet::Gsm8BitUnpackedToString(data, offset, count, &s);
            ret.Append(s);

            offset += count;
            len -= count;
        }

        return ret.ToString();
    }

    AutoPtr<IResources> resource = CResources::GetSystem();
    String defaultCharset("");
    // try {
    resource->GetString(Elastos::Droid::R::string::gsm_alphabet_default_charset, &defaultCharset);
    // } catch (NotFoundException e) {
    //     // Ignore Exception and defaultCharset is set to a empty string.
    // }
    String s;
    GsmAlphabet::Gsm8BitUnpackedToString(data, offset, length, defaultCharset.Trim(), &s);
    return s;
}

Int32 IccUtils::HexCharToInt32(
    /* [in] */ Char32 c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);

    assert(0 && "invalid hex Char32");
    // throw new RuntimeException ("invalid hex Char32 '" + c + "'");
    return -1;
}

AutoPtr<ArrayOf<Byte> > IccUtils::HexStringToBytes(
    /* [in] */ const String& s)
{
    if (s == NULL) return NULL;

    Int32 sz = s.GetLength();

    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(sz / 2);

    for (Int32 i = 0 ; i < sz ; i += 2) {
        (*ret)[i / 2] = (Byte) ((HexCharToInt32(s.GetChar(i)) << 4)
                            | HexCharToInt32(s.GetChar(i + 1)));
    }

    return ret;
}

String IccUtils::BytesToHexString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    if (bytes == NULL) return String(NULL);

    Int32 len = bytes->GetLength();
    StringBuilder ret(2 * len);

    for (Int32 i = 0 ; i < len ; i++) {
        Int32 b;

        b = 0x0f & ((*bytes)[i] >> 4);

        ret.AppendChar(String("0123456789abcdef").GetChar(b));

        b = 0x0f & (*bytes)[i];

        ret.AppendChar(String("0123456789abcdef").GetChar(b));
    }

    return ret.ToString();
}

String IccUtils::NetworkNameToString(
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
            GsmAlphabet::Gsm7BitPackedToString(data, offset + 1, countSeptets, &ret);
            break;
        }
        case 1: {
            // UCS2
            // try {
            Logger::D(TAG, "TODO : has charset problem???");
            // ret = new String(data, offset + 1, length - 1, "utf-16");
            ret = String(*data, offset + 1, length - 1);
            // } catch (UnsupportedEncodingException ex) {
            //     ret = "";
            //     Rlog.e(TAG,"implausible UnsupportedEncodingException", ex);
            // }
            break;
        }

        // unsupported encoding
        default:
            ret = String("");
            break;
    }

    // "Add CI"
    // "The MS should add the letters for the Country's Initials and
    //  a separator (e.g. a space) to the text string"

    if (((*data)[offset] & 0x40) != 0) {
        // FIXME(mkf) add country initials here
    }

    return ret;
}

AutoPtr<IBitmap> IccUtils::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 numOfPixels = width*height;

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
        Logger::E(TAG, "parse end and size error");
    }

    AutoPtr<IBitmap> b;
    CBitmap::CreateBitmap(pixels, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&b);
    return b;
}

Int32 IccUtils::BitToRGB(
    /* [in] */ Int32 bit)
{
    if(bit == 1){
        return IColor::WHITE;
    }
    return IColor::BLACK;
}

AutoPtr<IBitmap> IccUtils::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 bits = (*data)[valueIndex++] & 0xFF;
    Int32 colorNumber = (*data)[valueIndex++] & 0xFF;
    Byte b1 = (*data)[valueIndex++];
    Byte b2 = (*data)[valueIndex++];
    Int32 clutOffset = (((b1) & 0xFF) << 8) | ((b2) & 0xFF);

    AutoPtr<ArrayOf<Int32> > colorIndexArray = GetCLUT(data, clutOffset, colorNumber);
    if (TRUE == transparency) {
        (*colorIndexArray)[colorNumber - 1] = IColor::TRANSPARENT;
    }

    AutoPtr<ArrayOf<Int32> > resultArray;
    if (0 == (8 % bits)) {
        resultArray = MapTo2OrderBitColor(data, valueIndex,
                (width * height), colorIndexArray, bits);
    }
    else {
        resultArray = MapToNon2OrderBitColor(data, valueIndex,
                (width * height), colorIndexArray, bits);
    }

    AutoPtr<IBitmap> b;
    CBitmap::CreateBitmap(resultArray, width, height, BitmapConfig_RGB_565, (IBitmap**)&b);
    return b;
}

AutoPtr<ArrayOf<Int32> > IccUtils::MapTo2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    if (0 != (8 % bits)) {
        Logger::E(TAG, "not event number of color");
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

AutoPtr<ArrayOf<Int32> > IccUtils::MapToNon2OrderBitColor(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* colorArray,
    /* [in] */ Int32 bits)
{
    if (0 == (8 % bits)) {
        Logger::E(TAG, "not odd number of color");
        return MapTo2OrderBitColor(data, valueIndex, length, colorArray,
                bits);
    }

    AutoPtr<ArrayOf<Int32> > resultArray = ArrayOf<Int32>::Alloc(length);
    // TODO fix me:
    return resultArray;
}

AutoPtr<ArrayOf<Int32> > IccUtils::GetCLUT(
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
        Byte b1 = (*rawData)[valueIndex++];
        Byte b2 = (*rawData)[valueIndex++];
        Byte b3 = (*rawData)[valueIndex++];
        (*result)[colorIndex++] = alpha
                | ((b1 & 0xFF) << 16)
                | ((b2 & 0xFF) << 8)
                | ((b3 & 0xFF));
    } while (valueIndex < endIndex);
    return result;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
