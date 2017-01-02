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
#include "elastos/droid/media/audiopolicy/CAudioMixingRule.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

CAR_INTERFACE_IMPL(CAudioMixingRule, Object, IAudioMixingRule)

CAR_OBJECT_IMPL(CAudioMixingRule)

CAudioMixingRule::AttributeMatchCriterion::AttributeMatchCriterion(
    /* [in] */ IAudioAttributes * attributes,
    /* [in] */ Int32 rule)
{
    mAttr = attributes;
    mRule = rule;
}

CAudioMixingRule::CAudioMixingRule()
{
}

CAudioMixingRule::~CAudioMixingRule()
{
}

ECode CAudioMixingRule::constructor(
    /* [in] */ IArrayList * criteria)
{
    mCriteria = criteria;
    return NOERROR;
}

ECode CAudioMixingRule::GetCriteria(
    /* [out] */ IArrayList ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCriteria;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
