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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CLengthFilter.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_2(CLengthFilter, Object, ILengthFilter, IInputFilter)

CAR_OBJECT_IMPL(CLengthFilter)

CLengthFilter::CLengthFilter()
    : mMax(0)
{}

ECode CLengthFilter::constructor(
    /* [in] */ Int32 max)
{
    mMax = max;
    return NOERROR;
}

ECode CLengthFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs);

    Int32 len;
    ICharSequence::Probe(dest)->GetLength(&len);
    Int32 keep = mMax - (len - (dend - dstart));

    if (keep <= 0) {
        return CString::New(String(""), cs);
    }
    else if (keep >= end - start) {
        *cs = NULL;
        return NOERROR; // keep original
    }
    else {
        keep += start;
        Char32 sourceChar;
        if (Character::IsHighSurrogate((source->GetCharAt(keep - 1, &sourceChar), sourceChar))) {
            --keep;
            if (keep == start) {
                return CString::New(String(""), cs);
            }
        }
        return source->SubSequence(start, keep, cs);
    }
}

ECode CLengthFilter::GetMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max)
    *max = mMax;
    return NOERROR;
}

}//namespace Text
}//namespace Droid
}//namespace Elastos