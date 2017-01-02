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

#include "elastos/droid/text/CSpannableStringBuilderHelper.h"
#include "elastos/droid/text/SpannableStringBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_SINGLETON_IMPL(CSpannableStringBuilderHelper)

CAR_INTERFACE_IMPL(CSpannableStringBuilderHelper, Singleton, ISpannableStringBuilderHelper)

ECode  CSpannableStringBuilderHelper::ValueOf(
    /* [in] */ ICharSequence* source,
    /* [out] */ ISpannableStringBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    AutoPtr<ISpannableStringBuilder> b = SpannableStringBuilder::ValueOf(source);
    *builder= b;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos