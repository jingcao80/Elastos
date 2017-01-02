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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/text/CAllCapsFilter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CSpannableString.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_2(CAllCapsFilter, Object, IAllCapsFilter, IAllCapsFilter)

CAR_OBJECT_IMPL(CAllCapsFilter)

ECode CAllCapsFilter::constructor()
{
    return NOERROR;
}

ECode CAllCapsFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs);

    String s;
    Char32 c;
    for (Int32 i = start; i < end; i++) {
        source->GetCharAt(i, &c);
        if (Character::IsLowerCase(c)) {
            AutoPtr< ArrayOf<Char32> > v = ArrayOf<Char32>::Alloc(end - start);
            TextUtils::GetChars(source, start, end, v, 0);
            s = String(*v, 0, end - start);
            s = s.ToUpperCase();

            AutoPtr<ICharSequence> charS;
            FAIL_RETURN(CString::New(s, (ICharSequence**)&charS));
            if (ISpanned::Probe(source) != NULL) {
                AutoPtr<ISpannableString> sp;
                FAIL_RETURN(CSpannableString::New(charS, (ISpannableString**)&sp));
                TextUtils::CopySpansFrom(ISpanned::Probe(source),
                        start, end, EIID_IInterface, ISpannable::Probe(sp), 0);
                *cs = ICharSequence::Probe(sp);
                REFCOUNT_ADD(*cs);
                return NOERROR;
            }
            else {
                *cs = charS;
                REFCOUNT_ADD(*cs);
                return NOERROR;
            }
        }
    }

    *cs = NULL;
    return NOERROR;
}

}//namespace Text
}//namespace Droid
}//namespace Elastos
