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
#include "elastos/droid/text/utility/CRfc822TokenizerHelper.h"
#include "elastos/droid/text/utility/CRfc822Tokenizer.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(CRfc822TokenizerHelper, Singleton, IRfc822TokenizerHelper)

CAR_SINGLETON_IMPL(CRfc822TokenizerHelper)

ECode CRfc822TokenizerHelper::Tokenize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ICollection* ret)
{
    return CRfc822Tokenizer::Tokenize(text, ret);
}

ECode CRfc822TokenizerHelper::Tokenize(
    /* [in] */ ICharSequence* text,
    /* [out, callee] */ ArrayOf<IRfc822Token*>** ret)
{
    VALIDATE_NOT_NULL(ret);
    return CRfc822Tokenizer::Tokenize(text, ret);
}

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos
