
#include "elastos/droid/media/CCamcorderProfileHelper.h"
#include "elastos/droid/media/CCamcorderProfile.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CCamcorderProfileHelper, Singleton, ICamcorderProfileHelper)

CAR_SINGLETON_IMPL(CCamcorderProfileHelper)

/*static*/
ECode CCamcorderProfileHelper::Get(
    /* [in] */ Int32 quality,
    /* [out] */ ICamcorderProfile** result)
{
    return CCamcorderProfile::Get(quality, result);
}

/*static*/
ECode CCamcorderProfileHelper::Get(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ ICamcorderProfile** result)
{
    return CCamcorderProfile::Get(cameraId, quality, result);
}

/*static*/
ECode CCamcorderProfileHelper::HasProfile(
    /* [in] */ Int32 quality,
    /* [out] */ Boolean* result)
{
    return CCamcorderProfile::HasProfile(quality, result);
}

/*static*/
ECode CCamcorderProfileHelper::HasProfile(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ Boolean* result)
{
    return CCamcorderProfile::HasProfile(cameraId, quality, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
