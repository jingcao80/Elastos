
#include "elastos/droid/hardware/camera2/params/CStreamConfigurationDuration.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSize.h"
#include <elastos/utility/Arrays.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CStreamConfigurationDuration, Object, IStreamConfigurationDuration)

CAR_OBJECT_IMPL(CStreamConfigurationDuration)

CStreamConfigurationDuration::CStreamConfigurationDuration()
    : mFormat(0)
    , mWidth(0)
    , mHeight(0)
    , mDurationNs(0)
{
}

CStreamConfigurationDuration::~CStreamConfigurationDuration()
{
}

ECode CStreamConfigurationDuration::constructor()
{
    return NOERROR;
}

ECode CStreamConfigurationDuration::constructor(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int64 durationNs)
{
    FAIL_RETURN(StreamConfigurationMap::CheckArgumentFormatInternal(format, &mFormat))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(durationNs, String("durationNs must be non-negative")))

    mWidth = width;
    mHeight = height;
    mDurationNs = durationNs;
    return NOERROR;
}

ECode CStreamConfigurationDuration::GetFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);

    *format = mFormat;
    return NOERROR;
}

ECode CStreamConfigurationDuration::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode CStreamConfigurationDuration::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

ECode CStreamConfigurationDuration::GetSize(
    /* [out] */ ISize** size)
{
    VALIDATE_NOT_NULL(size);

    return CSize::New(mWidth, mHeight, size);
}

ECode CStreamConfigurationDuration::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);

    *duration = mDurationNs;
    return NOERROR;
}

ECode CStreamConfigurationDuration::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    IStreamConfigurationDuration* scd = IStreamConfigurationDuration::Probe(obj);
    if (scd == NULL) {
        return NOERROR;
    }

    CStreamConfigurationDuration* other = (CStreamConfigurationDuration*)scd;
    *equal = mFormat == other->mFormat &&
                mWidth == other->mWidth &&
                mHeight == other->mHeight &&
                mDurationNs == other->mDurationNs;
    return NOERROR;
}

ECode CStreamConfigurationDuration::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return HashCodeHelpers::GetHashCode(mFormat, mWidth, mHeight,
        (Int32) mDurationNs, (Int32)((0xFFFFFFFF & mDurationNs) >> 4), hashCode);
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
