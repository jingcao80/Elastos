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

#include "elastos/droid/telephony/cdma/CCdmaSmsCbProgramResults.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL_2(CCdmaSmsCbProgramResults, Object, ICdmaSmsCbProgramResults, IParcelable)

CAR_OBJECT_IMPL(CCdmaSmsCbProgramResults)

CCdmaSmsCbProgramResults::CCdmaSmsCbProgramResults()
    : mCategory(0)
    , mLanguage(0)
    , mCategoryResult(0)
{
}

CCdmaSmsCbProgramResults::~CCdmaSmsCbProgramResults()
{
}

ECode CCdmaSmsCbProgramResults::constructor()
{
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::constructor(
    /* [in] */ Int32 category,
    /* [in] */ Int32 language,
    /* [in] */ Int32 categoryResult)
{
    mCategory = category;
    mLanguage = language;
    mCategoryResult = categoryResult;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCategory);
    source->ReadInt32(&mLanguage);
    source->ReadInt32(&mCategoryResult);
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCategory);
    dest->WriteInt32(mLanguage);
    dest->WriteInt32(mCategoryResult);
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::GetCategory(
    /* [out] */ Int32* category)
{
    VALIDATE_NOT_NULL(category);
    *category = mCategory;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::GetLanguage(
    /* [out] */ Int32* language)
{
    VALIDATE_NOT_NULL(language);
    *language = mLanguage;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::GetCategoryResult(
    /* [out] */ Int32* categoryresult)
{
    VALIDATE_NOT_NULL(categoryresult);
    *categoryresult = mCategoryResult;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::SetCategory(
    /* [in] */ Int32 category)
{
    mCategory = category;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::SetLanguage(
    /* [in] */ Int32 language)
{
    mLanguage = language;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::SetCategoryResult(
    /* [in] */ Int32 categoryresult)
{
    mCategoryResult = categoryresult;
    return NOERROR;
}

ECode CCdmaSmsCbProgramResults::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CdmaSmsCbProgramResults{category=");
    sb.Append(mCategory);
    sb.Append(", language=");
    sb.Append(mLanguage);
    sb.Append(", result=");
    sb.Append(mCategoryResult);
    sb.Append("}");
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos
