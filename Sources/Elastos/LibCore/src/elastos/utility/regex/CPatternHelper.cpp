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

#include "CPatternHelper.h"
#include "Pattern.h"

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(CPatternHelper, Singleton, IPatternHelper)

CAR_SINGLETON_IMPL(CPatternHelper)

ECode CPatternHelper::Compile(
    /* [in] */ const String& regularExpression,
    /* [in] */ Int32 flags,
    /* [out] */ IPattern** obj)
{
    VALIDATE_NOT_NULL(obj);
    return Pattern::Compile(regularExpression, flags, obj);
}

ECode CPatternHelper::Compile(
    /* [in] */ const String& pattern,
    /* [in] */ IPattern** obj)
{
    return Pattern::Compile(pattern, obj);
}

ECode CPatternHelper::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ const String& input,
    /* [out] */ Boolean* isMatch)
{

    VALIDATE_NOT_NULL(isMatch);
    *isMatch = Pattern::Matches(regularExpression, input);
    return NOERROR;
}

ECode CPatternHelper::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ ICharSequence* input,
    /* [out] */ Boolean* isMatch)
{

    VALIDATE_NOT_NULL(isMatch);
    *isMatch = Pattern::Matches(regularExpression, input);
    return NOERROR;
}

ECode CPatternHelper::Quote(
    /* [in] */ const String& string,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Pattern::Quote(string);
    return NOERROR;
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos
