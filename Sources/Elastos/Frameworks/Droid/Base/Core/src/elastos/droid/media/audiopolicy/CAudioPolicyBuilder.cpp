#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiopolicy/CAudioPolicyBuilder.h"
#include "elastos/droid/media/audiopolicy/CAudioPolicy.h"
#include "elastos/droid/media/audiopolicy/CAudioPolicyConfig.h"

using Elastos::Utility::CArrayList;
using Elastos::Droid::Media::AudioPolicy::CAudioPolicy;
using Elastos::Droid::Media::AudioPolicy::CAudioPolicyConfig;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

CAR_INTERFACE_IMPL(CAudioPolicyBuilder, Object, IAudioPolicyBuilder)

CAR_OBJECT_IMPL(CAudioPolicyBuilder)

CAudioPolicyBuilder::CAudioPolicyBuilder()
{
}

CAudioPolicyBuilder::~CAudioPolicyBuilder()
{
}

ECode CAudioPolicyBuilder::constructor()
{
    return CArrayList::New((IArrayList**)&mMixes);
}

ECode CAudioPolicyBuilder::AddMix(
    /* [in] */ IAudioMix * mix,
    /* [out] */ IAudioPolicyBuilder ** builder)
{
    VALIDATE_NOT_NULL(builder)
    *builder = NULL;
    if (mix == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioMix argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMixes->Add(mix);
    *builder = this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CAudioPolicyBuilder::Build(
    /* [out] */ IAudioPolicy** result)
{
    AutoPtr<IAudioPolicyConfig> config;
    CAudioPolicyConfig::New(mMixes, (IAudioPolicyConfig**)&config);
    return CAudioPolicy::New(config, result);
}


} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
