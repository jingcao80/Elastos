
#include "elastos/droid/hardware/camera2/params/CHighSpeedVideoConfiguration.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSize.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::CInteger32;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CHighSpeedVideoConfiguration, Object, IHighSpeedVideoConfiguration)

CAR_OBJECT_IMPL(CHighSpeedVideoConfiguration)

CHighSpeedVideoConfiguration::CHighSpeedVideoConfiguration()
    : mWidth(0)
    , mHeight(0)
    , mFpsMin(0)
    , mFpsMax(0)
{
}

CHighSpeedVideoConfiguration::~CHighSpeedVideoConfiguration()
{
}

ECode CHighSpeedVideoConfiguration::constructor()
{
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 fpsMin,
    /* [in] */ Int32 fpsMax)
{
    if (fpsMax < 60) {
        //throw new IllegalArgumentException("fpsMax must be at least 60");
        Slogger::E("CHighSpeedVideoConfiguration", "fpsMax must be at least 60");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFpsMax = fpsMax;
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(fpsMin, String("fpsMin must be positive")))
    mWidth = width;
    mHeight = height;
    mFpsMin = fpsMin;

    CSize::New(mWidth, mHeight, (ISize**)&mSize);
    AutoPtr<IInteger32> lower;
    CInteger32::New(mFpsMin, (IInteger32**)&lower);
    AutoPtr<IInteger32> upper;
    CInteger32::New(mFpsMax, (IInteger32**)&upper);
    mFpsRange = new Range<IInteger32>(lower, upper);
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetFpsMin(
    /* [out] */ Int32* min)
{
    VALIDATE_NOT_NULL(min);

    *min = mFpsMin;
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetFpsMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max);

    *max = mFpsMax;
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetSize(
    /* [out] */ ISize** size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    REFCOUNT_ADD(*size);
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetFpsRange(
    /* [out] */ IInterface** range)
{
    VALIDATE_NOT_NULL(range);

    *range = TO_IINTERFACE(mFpsRange);
    REFCOUNT_ADD(*range);
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (THIS_PROBE(IInterface) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IHighSpeedVideoConfiguration::Probe(obj) != NULL) {
        const AutoPtr<CHighSpeedVideoConfiguration> other = (CHighSpeedVideoConfiguration*)IHighSpeedVideoConfiguration::Probe(obj);
        *equal = mWidth == other->mWidth &&
                mHeight == other->mHeight &&
                mFpsMin == other->mFpsMin &&
                mFpsMax == other->mFpsMax;
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode CHighSpeedVideoConfiguration::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    assert(0 && "TODO: weit Hardware/Camera2/Utils/HashCodeHelpers");
    //*hashCode = HashCodeHelpers::GetHashCode(mWidth, mHeight, mFpsMin, mFpsMax);
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
