#include "elastos/droid/media/CAudioPatch.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioPatch, Object, IAudioPatch)

CAR_OBJECT_IMPL(CAudioPatch)

CAudioPatch::CAudioPatch()
{
}

CAudioPatch::~CAudioPatch()
{
}

ECode CAudioPatch::constructor(
    /* [in] */ IAudioHandle* patchHandle,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sinks)
{
    mHandle = patchHandle;
    mSources = sources;
    mSinks = sinks;
    return NOERROR;
}

ECode CAudioPatch::Sources(
    /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSources;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioPatch::Sinks(
    /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSinks;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
