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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/text/CBidiFormatterBuilder.h"
#include "elastos/droid/text/CBidiFormatter.h"

using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBidiFormatterBuilder, Object, IBidiFormatterBuilder)

CAR_OBJECT_IMPL(CBidiFormatterBuilder)

CBidiFormatterBuilder::CBidiFormatterBuilder()
    : mIsRtlContext(FALSE)
    , mFlags(0)
{}

CBidiFormatterBuilder::~CBidiFormatterBuilder()
{}

ECode CBidiFormatterBuilder::constructor()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Initialize(CBidiFormatter::IsRtlLocale(locale));
    return NOERROR;
}

ECode CBidiFormatterBuilder::constructor(
    /* [in] */ Boolean rtlContext)
{
    Initialize(rtlContext);
    return NOERROR;
}

ECode CBidiFormatterBuilder::constructor(
    /* [in] */ ILocale* locale)
{
    Initialize(CBidiFormatter::IsRtlLocale(locale));
    return NOERROR;
}

void CBidiFormatterBuilder::Initialize(
    /* [in] */ Boolean isRtlContext)
{
    mIsRtlContext = isRtlContext;
    mTextDirectionHeuristic = CBidiFormatter::DEFAULT_TEXT_DIRECTION_HEURISTIC;
    mFlags = CBidiFormatter::DEFAULT_FLAGS;
}

ECode CBidiFormatterBuilder::StereoReset(
    /* [in] */ Boolean stereoReset)
{
    if (stereoReset) {
        mFlags |= CBidiFormatter::FLAG_STEREO_RESET;
    }
    else {
        mFlags &= ~CBidiFormatter::FLAG_STEREO_RESET;
    }
    return NOERROR;
}

ECode CBidiFormatterBuilder::SetTextDirectionHeuristic(
    /* [in] */ ITextDirectionHeuristic* heuristic)
{
    mTextDirectionHeuristic = heuristic;
    return NOERROR;
}

AutoPtr<IBidiFormatter> CBidiFormatterBuilder::GetDefaultInstanceFromContext(
    /* [in] */ Boolean isRtlContext)
{
    return isRtlContext ? CBidiFormatter::DEFAULT_RTL_INSTANCE : CBidiFormatter::DEFAULT_LTR_INSTANCE;
}

ECode CBidiFormatterBuilder::Build(
    /* [out] */ IBidiFormatter** ret)
{
    VALIDATE_NOT_NULL(ret)

    if (mFlags == CBidiFormatter::DEFAULT_FLAGS &&
            mTextDirectionHeuristic == CBidiFormatter::DEFAULT_TEXT_DIRECTION_HEURISTIC) {
        AutoPtr<IBidiFormatter> bf = GetDefaultInstanceFromContext(mIsRtlContext);
        *ret = bf;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }

    return CBidiFormatter::New(mIsRtlContext, mFlags, mTextDirectionHeuristic, ret);
}

} // namespace Text
} // namespace Droid
} // namespace Elastos