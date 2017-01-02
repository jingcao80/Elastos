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

#include "elastos/droid/text/CHtml.h"
#include "elastos/droid/text/Html.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CHtml, Singleton, IHtml)

CAR_SINGLETON_IMPL(CHtml)

ECode CHtml::FromHtml(
    /* [in] */ const String& source,
    /* [out] */ ISpanned** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ISpanned> spanned = Html::FromHtml(source);
    *ret = spanned;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CHtml::FromHtml(
    /* [in] */ const String& source,
    /* [in] */ IHtmlImageGetter* imageGetter,
    /* [in] */ IHtmlTagHandler* tagHandler,
    /* [out] */ ISpanned** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ISpanned> spanned = Html::FromHtml(source, imageGetter, tagHandler);
    *ret = spanned;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CHtml::ToHtml(
    /* [in] */ ISpanned* text,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Html::ToHtml(text);
    return NOERROR;
}

ECode CHtml::EscapeHtml(
    /* [in] */ ICharSequence* text,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Html::EscapeHtml(text);
    return NOERROR;
}

} // namespace Text
} // namespace Droid
} // namespace Elastos

