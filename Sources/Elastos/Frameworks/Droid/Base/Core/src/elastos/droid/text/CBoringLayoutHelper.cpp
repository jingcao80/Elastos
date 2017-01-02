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

#include "elastos/droid/text/CBoringLayoutHelper.h"
#include "elastos/droid/text/CBoringLayout.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBoringLayoutHelper, Singleton, IBoringLayoutHelper)

CAR_SINGLETON_IMPL(CBoringLayoutHelper)

CARAPI CBoringLayoutHelper::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [out] */ IBoringLayout** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayout> bl = CBoringLayout::Make(source, paint, outerwidth, align, spacingmult, spacingadd, metrics, includepad);
    *ret = bl;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth,
    /* [out] */ IBoringLayout** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayout> bl = CBoringLayout::Make(source, paint, outerwidth, align, spacingmult, spacingadd, metrics, includepad, ellipsize, ellipsizedWidth);
    *ret = bl;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [out] */ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint);
    *ret = blm;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [out] */ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, textDir);
    *ret = blm;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [out] */ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, metrics);
    *ret = blm;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CARAPI CBoringLayoutHelper::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [out] */ IBoringLayoutMetrics** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IBoringLayoutMetrics> blm = CBoringLayout::IsBoring(text, paint, textDir, metrics);
    *ret = blm;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos