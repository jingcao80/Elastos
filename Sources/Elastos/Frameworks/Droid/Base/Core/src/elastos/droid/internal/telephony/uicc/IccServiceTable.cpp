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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccServiceTable.h"

#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IccServiceTable
//=====================================================================
CAR_INTERFACE_IMPL(IccServiceTable, Object, IIccServiceTable);

IccServiceTable::IccServiceTable()
{
}

ECode IccServiceTable::constructor(
    /* [in] */ ArrayOf<Byte>* table)
{
    mServiceTable = table;
    return NOERROR;
}

ECode IccServiceTable::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IInterface*> > values = GetValues();
    Int32 numBytes = mServiceTable->GetLength();
    StringBuilder builder(GetTag());
    builder.Append('[');
    builder.Append(numBytes * 8);
    builder.Append(String("]={ "));

    Boolean addComma = FALSE;
    for (Int32 i = 0; i < numBytes; i++) {
        Byte currentByte = (*mServiceTable)[i];
        for (Int32 bit = 0; bit < 8; bit++) {
            if ((currentByte & (1 << bit)) != 0) {
                if (addComma) {
                    builder.Append(", ");
                }
                else {
                    addComma = TRUE;
                }
                Int32 ordinal = (i * 8) + bit;
                if (ordinal < values->GetLength()) {
                    builder.Append((*values)[ordinal]);
                }
                else {
                    builder.Append('#');
                    builder.Append(ordinal + 1);    // service number (one-based)
                }
            }
        }
    }
    builder.Append(" }");
    builder.ToString(result);
    return NOERROR;
}

Boolean IccServiceTable::IsAvailable(
    /* [in] */ Int32 service)
{
    Int32 offset = service / 8;
    if (offset >= mServiceTable->GetLength()) {
        // Note: Enums are zero-based, but the TS service numbering is one-based
        Logger::E(GetTag(), String("isAvailable for service ") + StringUtils::ToString(service + 1)
                + String(" fails, max service is ")
                + StringUtils::ToString(mServiceTable->GetLength() * 8));
        return FALSE;
    }
    Int32 bit = service % 8;
    return ((*mServiceTable)[offset] & (1 << bit)) != 0;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
