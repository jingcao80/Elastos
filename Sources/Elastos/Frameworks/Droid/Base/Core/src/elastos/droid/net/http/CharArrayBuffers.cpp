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

#include <Elastos.CoreLibrary.Apache.h>
#include "elastos/droid/net/http/CharArrayBuffers.h"

using Org::Apache::Http::Protocol::IHTTP;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

const Char32 CharArrayBuffers::uppercaseAddon = 'a' - 'A';

ECode CharArrayBuffers::ContainsIgnoreCaseTrimmed(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ Int32 beginIndex,
    /* [in] */ const String& str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 len;
    buffer->GetLength(&len);
    AutoPtr<ArrayOf<Char32> > chars;
    buffer->GetBuffer((ArrayOf<Char32>**)&chars);
    AutoPtr<IHTTP> helper;
    // TODO: Waiting for CHTTP
    assert(0);
    // CHTTP::AcquireSingleton((IHTTP**)&helper);
    while (beginIndex < len) {
        Boolean isWhitespace;
        helper->IsWhitespace((*chars)[beginIndex], &isWhitespace);
        if (!isWhitespace)
            break;
        beginIndex++;
    }
    Int32 size = str.GetLength();
    Boolean ok = len >= beginIndex + size;
    for (Int32 j=0; ok && (j<size); j++) {
        Char32 a = (*chars)[beginIndex+j];
        Char32 b = str.GetChar(j);
        if (a != b) {
            a = ToLower(a);
            b = ToLower(b);
            ok = a == b;
        }
    }
    *result = ok;
    return NOERROR;
}

ECode CharArrayBuffers::SetLowercaseIndexOf(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ Int32 ch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 beginIndex = 0;
    Int32 endIndex;
    buffer->GetLength(&endIndex);
    AutoPtr<ArrayOf<Char32> > chars;
    buffer->GetBuffer((ArrayOf<Char32>**)&chars);
    for (Int32 i = beginIndex; i < endIndex; i++) {
        Char32 current = (*chars)[i];
        if (current == ch) {
            *result = i;
            return NOERROR;
        }
        else if (current >= 'A' && current <= 'Z'){
            // make lower case
            current += uppercaseAddon;
            (*chars)[i] = current;
        }
    }
    *result = -1;
    return NOERROR;
}

Char32 CharArrayBuffers::ToLower(
    /* [in] */ Char32 c)
{
    if (c >= 'A' && c <= 'Z'){
        c += uppercaseAddon;
    }
    return c;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
