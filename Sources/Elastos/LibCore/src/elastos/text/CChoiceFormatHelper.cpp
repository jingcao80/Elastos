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

#include "CChoiceFormatHelper.h"
#include "ChoiceFormat.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CChoiceFormatHelper, Singleton, IChoiceFormatHelper)

CAR_SINGLETON_IMPL(CChoiceFormatHelper)

ECode CChoiceFormatHelper::NextDouble(
    /* [in] */ Double value,
    /* [out] */ Double* nextValue)
{
    VALIDATE_NOT_NULL(nextValue)
    *nextValue = ChoiceFormat::NextDouble(value);
    return NOERROR;
}

ECode CChoiceFormatHelper::NextDouble(
    /* [in] */ Double value,
    /* [in] */ Boolean increment,
    /* [out] */ Double* nextValue)
{
    VALIDATE_NOT_NULL(nextValue)
    *nextValue = ChoiceFormat::NextDouble(value,increment);
    return NOERROR;
}

ECode CChoiceFormatHelper::PreviousDouble(
    /* [in] */ Double value,
    /* [out] */ Double* previousValue)
{
    VALIDATE_NOT_NULL(previousValue)
    *previousValue = ChoiceFormat::PreviousDouble(value);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos