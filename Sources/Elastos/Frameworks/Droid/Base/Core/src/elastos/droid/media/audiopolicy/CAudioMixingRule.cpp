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
