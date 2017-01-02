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
#include "elastos/droid/text/CLayoutHelper.h"
#include "elastos/droid/text/Layout.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CLayoutHelper, Singleton, ILayoutHelper)

CAR_SINGLETON_IMPL(CLayoutHelper)

ECode CLayoutHelper::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [out] */ Float* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = Layout::GetDesiredWidth(source, paint);
    return NOERROR;
}

ECode CLayoutHelper::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ITextPaint* paint,
    /* [out] */ Float* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = Layout::GetDesiredWidth(source, start, end, paint);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos