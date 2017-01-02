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

#include "elastos/droid/internal/utility/HexDump.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

const Char32 HexDump::HEX_DIGITS[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

String HexDump::DumpHexString(
    /* [in] */ ArrayOf<Byte>* array)
{
    return DumpHexString(array, 0, array->GetLength());
}

String HexDump::DumpHexString(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    StringBuilder result;

    AutoPtr<ArrayOf<Byte> > line = ArrayOf<Byte>::Alloc(16);
    Int32 lineIndex = 0;

    result.Append("\n0x");
    result.Append(ToHexString(offset));

    for (Int32 i = offset; i < offset + length; i++) {
        if (lineIndex == 16) {
            result.Append(" ");

            for (Int32 j = 0; j < 16; j++) {
                if ((*line)[j] > ' ' && (*line)[j] < '~') {
                    result.Append(String(*line, j, 1));
                }
                else {
                    result.Append(".");
                }
            }

            result.Append("\n0x");
            result.Append(ToHexString(i));
            lineIndex = 0;
        }

        Byte b = (*array)[i];
        result.Append(" ");
        result.AppendChar(HEX_DIGITS[((0xFF & b) >> 4) & 0x0F]);
        result.AppendChar(HEX_DIGITS[b & 0x0F]);

        (*line)[lineIndex++] = b;
    }

    if (lineIndex != 16) {
        Int32 count = (16 - lineIndex) * 3;
        count++;
        for (Int32 i = 0; i < count; i++) {
            result.Append(" ");
        }

        for (Int32 i = 0; i < lineIndex; i++) {
            if ((*line)[i] > ' ' && (*line)[i] < '~') {
                result.Append(String(*line, i, 1));
            }
            else {
                result.Append(".");
            }
        }
    }

    return result.ToString();
}

String HexDump::ToHexString(
    /* [in] */ Byte b)
{
    return ToHexString(ToByteArray(b));
}

String HexDump::ToHexString(
    /* [in] */ ArrayOf<Byte>* array)
{
    return ToHexString(array, 0, array->GetLength());
}

String HexDump::ToHexString(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    AutoPtr<ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(length * 2);

    Int32 bufIndex = 0;
    for (Int32 i = offset; i < offset + length; i++) {
        Byte b = (*array)[i];
        (*buf)[bufIndex++] = HEX_DIGITS[((0xFF & b) >> 4) & 0x0F];
        (*buf)[bufIndex++] = HEX_DIGITS[b & 0x0F];
    }

    return String(*buf);
}

String HexDump::ToHexString(
    /* [in] */ Int32 i)
{
    return ToHexString(ToByteArray(i));
}

AutoPtr<ArrayOf<Byte> > HexDump::ToByteArray(
    /* [in] */ Byte b)
{
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(1);
    (*array)[0] = b;
    return array;
}

AutoPtr<ArrayOf<Byte> > HexDump::ToByteArray(
    /* [in] */ Int32 i)
{
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(4);

    (*array)[3] = (Byte)(i & 0xFF);
    (*array)[2] = (Byte)((i >> 8) & 0xFF);
    (*array)[1] = (Byte)((i >> 16) & 0xFF);
    (*array)[0] = (Byte)((i >> 24) & 0xFF);

    return array;
}

Int32 HexDump::ToByte(
    /* [in] */ Char32 c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);

    Logger::E("HexDump", "Invalid hex char '%c'", c);
    // return E_RUNTIME_EXCEPTION;
    return 0;
}

AutoPtr<ArrayOf<Byte> > HexDump::HexStringToByteArray(
    /* [in] */ const String& hexString)
{
    Int32 length = hexString.GetLength();
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(length / 2);

    for (Int32 i = 0; i < length; i += 2) {
        (*buffer)[i / 2] = (Byte)((ToByte(hexString.GetChar(i)) << 4) | ToByte(hexString.GetChar(i+1)));
    }

    return buffer;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos