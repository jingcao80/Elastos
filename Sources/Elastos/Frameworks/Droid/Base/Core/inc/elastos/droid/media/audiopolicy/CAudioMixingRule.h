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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULE_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULE_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioMixingRule.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * @hide CANDIDATE FOR PUBLIC API
 *
 * Here's an example of creating a mixing rule for all media playback:
 * <pre>
 * AudioAttributes mediaAttr = new AudioAttributes.Builder()
 *         .setUsage(AudioAttributes.USAGE_MEDIA)
 *         .build();
 * AudioMixingRule mediaRule = new AudioMixingRule.Builder()
 *         .addRule(mediaAttr, AudioMixingRule.RULE_MATCH_ATTRIBUTE_USAGE)
 *         .build();
 * </pre>
 */
CarClass(CAudioMixingRule)
    , public Object
    , public IAudioMixingRule
{
public:
    class AttributeMatchCriterion
        : public Object
    {
    public:
        AttributeMatchCriterion(
            /* [in] */ IAudioAttributes * attributes,
            /* [in] */ Int32 rule);

    public:
        AutoPtr<IAudioAttributes> mAttr;
        Int32 mRule;
    };

public:
    CAudioMixingRule();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioMixingRule();

    CARAPI constructor(
        /* [in] */ IArrayList * criteria);

    CARAPI GetCriteria(
        /* [out] */ IArrayList ** result);

private:
    AutoPtr<IArrayList> mCriteria;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIXINGRULE_H__
