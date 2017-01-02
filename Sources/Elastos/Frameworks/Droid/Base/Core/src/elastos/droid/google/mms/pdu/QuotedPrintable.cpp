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

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/google/mms/pdu/QuotedPrintable.h"

using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                           QuotedPrintable
//=====================================================================
Byte QuotedPrintable::ESCAPE_CHAR = '=';

AutoPtr<ArrayOf<Byte> > QuotedPrintable::DecodeQuotedPrintable(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    if (bytes == NULL) {
        return NULL;
    }
    AutoPtr<IByteArrayOutputStream> buffer;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&buffer);
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        Int32 b = (*bytes)[i];
        if (b == ESCAPE_CHAR) {
            // try {
                if ('\r' == (Char32)(*bytes)[i + 1] &&
                        '\n' == (Char32)(*bytes)[i + 2]) {
                    i += 2;
                    continue;
                }
                Int32 u = (*bytes)[++i]; // Character::Digit((Char32) (*bytes)[++i], 16);
                Int32 l = (*bytes)[++i]; // Character::Digit((Char32) (*bytes)[++i], 16);
                if (u == -1 || l == -1) {
                    return NULL;
                }
                IOutputStream::Probe(buffer)->Write((Char32) ((u << 4) + l));
            // } catch (ArrayIndexOutOfBoundsException e) {
            //     return NULL;
            // }
        }
        else {
            IOutputStream::Probe(buffer)->Write(b);
        }
    }
    AutoPtr<ArrayOf<Byte> > res;
    buffer->ToByteArray((ArrayOf<Byte>**)&res);
    return res;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
