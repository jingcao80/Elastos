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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULEBUILDER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULEBUILDER_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioMixingRuleBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * Builder class for {@link AudioMixingRule} objects
 *
 */
CarClass(CAudioMixingRuleBuilder)
    , public Object
    , public IAudioMixingRuleBuilder
{
public:
    CAudioMixingRuleBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioMixingRuleBuilder();

    /**
     * Constructs a new Builder with no rules.
     */
    CARAPI constructor();

    /**
     * Add a rule for the selection of which streams are mixed together.
     * @param attrToMatch a non-null AudioAttributes instance for which a contradictory
     *     rule hasn't been set yet.
     * @param rule one of {@link AudioMixingRule#RULE_EXCLUDE_ATTRIBUTE_USAGE},
     *     {@link AudioMixingRule#RULE_MATCH_ATTRIBUTE_USAGE}.
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI AddRule(
        /* [in] */ IAudioAttributes * attrToMatch,
        /* [in] */ Int32 rule,
        /* [out] */ IAudioMixingRuleBuilder** builder);

    /**
     * Combines all of the matching and exclusion rules that have been set and return a new
     * {@link AudioMixingRule} object.
     * @return a new {@link AudioMixingRule} object
     */
    CARAPI Build(
        /* [out] */ IAudioMixingRule** result);

private:
    AutoPtr<IArrayList> mCriteria;
};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULEBUILDER_H__
