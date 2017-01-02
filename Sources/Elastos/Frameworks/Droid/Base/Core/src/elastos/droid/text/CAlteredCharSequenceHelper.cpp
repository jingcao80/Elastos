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

#include "elastos/droid/text/CAlteredCharSequenceHelper.h"
#include "elastos/droid/text/AlteredCharSequence.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CAlteredCharSequenceHelper, Object, IAlteredCharSequenceHelper)

CAR_SINGLETON_IMPL(CAlteredCharSequenceHelper)

ECode CAlteredCharSequenceHelper::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend,
    /* [out] */ IAlteredCharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IAlteredCharSequence> acs = AlteredCharSequence::Make(source, sub, substart, subend);
    *ret = acs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos

