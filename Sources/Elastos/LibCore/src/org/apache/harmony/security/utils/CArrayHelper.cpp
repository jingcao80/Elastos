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

#include "CArrayHelper.h"
#include "Character.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_SINGLETON_IMPL(CArrayHelper)
CAR_INTERFACE_IMPL(CArrayHelper, Singleton, IArrayHelper)

ECode CArrayHelper::GetBytesAsString(
    /* [in] */ ArrayOf<Byte> * data,
    /* [out] */ String* retVal)
{
    VALIDATE_NOT_NULL(retVal);
    StringBuilder result(data->GetLength() * 3);
    for (Int32 i = 0; i < data->GetLength(); ++i) {
        result.Append(StringUtils::ToHexString((*data)[i], FALSE));
        result.AppendChar(' ');
    }
    return result.ToString(retVal);
}

ECode CArrayHelper::ToString(
    /* [in] */ ArrayOf<Byte> * array,
    /* [in] */ const String& prefix,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    // Prefixes to be added to the offset values
    // in <code>String toString(byte[], String)</code> method
    const String offsetPrefix[] = {
            String(""),
            String("000"),
            String("00"),
            String("0"),
            String("")
    };
    StringBuilder sb;
    StringBuilder charForm;
    Int32 i = 0;
    for (i = 0; i < array->GetLength(); i++) {
        if (i%16 == 0) {
            sb.Append(prefix);
            // put offset
            String offset = StringUtils::ToHexString(i);
            sb.Append(offsetPrefix[offset.GetLength()]);
            sb.Append(offset);
            // clear char form for new line
            charForm.Delete(0, charForm.GetLength());
        }
        // put delimiter
        sb.AppendChar(' ');
        // put current byte
        sb.Append(StringUtils::ToHexString((*array)[i], FALSE));
        // form character representation part
        Int32 currentByte = (0xff & (*array)[i]);
        Char32 currentChar = (Char32)(currentByte & 0xffff);
        // FIXME if needed (how to distinguish PRINTABLE chars?)
        charForm.AppendChar(
                (Character::IsISOControl(currentChar) ? '.' : currentChar));
        // Add additional delimiter for each 8 values
        if ((i+1)%8 == 0) {
            sb.AppendChar(' ');
        }
        // Add character representation for each line
        if ((i+1)%16 == 0) {
            sb.AppendChar(' ');
            sb.Append(charForm.ToString());
            sb.AppendChar('\n');
        }
    }
    // form last line
    if (i%16 != 0) {
        Int32 ws2add = 16 - i%16;
        for (Int32 j=0; j<ws2add; j++) {
            sb.Append("   ");
        }
        if (ws2add > 8) {
            sb.AppendChar(' ');
        }
        sb.Append("  ");
        sb.Append(charForm.ToString());
        sb.AppendChar('\n');
    }
    return sb.ToString(result);
}

}
}
}
}
}

