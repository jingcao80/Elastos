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
