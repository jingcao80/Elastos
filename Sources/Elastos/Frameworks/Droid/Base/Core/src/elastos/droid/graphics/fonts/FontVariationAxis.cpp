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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/graphics/fonts/FontVariationAxis.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Fonts {

const AutoPtr<IPattern> FontVariationAxis::TAG_PATTERN = Init_TAG_PATTERN();

CAR_INTERFACE_IMPL(FontVariationAxis, Object, IFontVariationAxis);

ECode FontVariationAxis::constructor(
    /* [in] */ const String& tagString,
    /* [in] */ Float styleValue)
{
    if (!IsValidTag(tagString)) {
        // throw new IllegalArgumentException("Illegal tag pattern: " + tagString);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTag = MakeTag(tagString);
    mTagString = tagString;
    mStyleValue = styleValue;
    return NOERROR;
}

ECode FontVariationAxis::GetOpenTypeTagValue(
    /* [out] */ Int32* tagValue)
{
    VALIDATE_NOT_NULL(tagValue);
    *tagValue = mTag;
    return NOERROR;
}

ECode FontVariationAxis::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTagString;
    return NOERROR;
}

ECode FontVariationAxis::GetStyleValue(
    /* [out] */ Float* styleValue)
{
    VALIDATE_NOT_NULL(styleValue);
    *styleValue = mStyleValue;
    return NOERROR;
}

ECode FontVariationAxis::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("'") + mTagString + String("' ")
            + StringUtils::ToString(mStyleValue);
    return NOERROR;
}

Boolean FontVariationAxis::IsValidTag(
    /* [in] */ const String& tagString)
{
    if (tagString.IsNull()) return FALSE;

    AutoPtr<IMatcher> matcher;
    TAG_PATTERN->Matcher(tagString, (IMatcher**)&matcher);
    Boolean match;
    matcher->Matches(&match);
    return match;
}

Int32 FontVariationAxis::MakeTag(
    /* [in] */ const String& tagString)
{
    Char32 c1 = tagString.GetChar(0);
    Char32 c2 = tagString.GetChar(1);
    Char32 c3 = tagString.GetChar(2);
    Char32 c4 = tagString.GetChar(3);
    return (c1 << 24) | (c2 << 16) | (c3 << 8) | c4;
}

AutoPtr<IPattern> FontVariationAxis::Init_TAG_PATTERN()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("[\u0020-\u007E]{4}"), (IPattern**)&pattern);
    return pattern;
}

} // namespace Fonts
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
