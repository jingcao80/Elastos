
#include "elastos/droid/media/CAudioManagerHelper.h"
#include "elastos/droid/media/CAudioManager.h"

namespace Elastos {
namespace Droid {
namespace Media {

ECode CAudioManagerHelper::IsValidRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [out] */ Boolean* result)
{
    return CAudioManager::IsValidRingerMode(ringerMode, result);
}

ECode CAudioManagerHelper::SetHdmiAvailable(
    /* [in] */ Boolean val)
{
    return CAudioManager::SetHdmiAvailable(val) ;
}

ECode CAudioManagerHelper::GetHdmiAvailable(
    /* [out] */ Boolean* result)
{
    return CAudioManager::GetHdmiAvailable(result) ;
}

ECode CAudioManagerHelper::GetHdmiExpected(
    /* [out] */ Boolean* result)
{
    return CAudioManager::GetHdmiExpected(result);
}

ECode CAudioManagerHelper::SetHdmiExpected(
    /* [in] */ Boolean val)
{
    return CAudioManager::SetHdmiExpected(val);
}

ECode CAudioManagerHelper::GetDefaultStreamVolume(
    /* [out, callee] */ ArrayOf<Int32>** defaultStreamVolume)
{
    VALIDATE_NOT_NULL(defaultStreamVolume)
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(const_cast<Int32*>(CAudioManager::DEFAULT_STREAM_VOLUME), 10);

    *defaultStreamVolume = array;
    REFCOUNT_ADD(*defaultStreamVolume);
    return NOERROR;
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
