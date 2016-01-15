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
