
#include "elastos/droid/hardware/camera2/params/CStreamConfiguration.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSize.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CStreamConfiguration, Object, IStreamConfiguration)

CAR_OBJECT_IMPL(CStreamConfiguration)

CStreamConfiguration::CStreamConfiguration()
    : mFormat(0)
    , mWidth(0)
    , mHeight(0)
    , mInput(FALSE)
{
}

CStreamConfiguration::~CStreamConfiguration()
{
}

ECode CStreamConfiguration::constructor(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean input)
{
    FAIL_RETURN(StreamConfigurationMap::CheckArgumentFormatInternal(format, &mFormat))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive")))
    mWidth = width;
    mHeight = height;
    mInput = input;
    return NOERROR;
}

ECode CStreamConfiguration::GetFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);
    *format = mFormat;
    return NOERROR;
}

ECode CStreamConfiguration::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode CStreamConfiguration::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode CStreamConfiguration::GetSize(
    /* [out] */ ISize** size)
{
    return CSize::New(mWidth, mHeight, size);
}

ECode CStreamConfiguration::IsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInput;
    return NOERROR;
}

ECode CStreamConfiguration::IsOutput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mInput;
    return NOERROR;
}

ECode CStreamConfiguration::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    IStreamConfiguration* sc = IStreamConfiguration::Probe(obj);
    if (obj == NULL) {
        return NOERROR;
    }

    CStreamConfiguration* other = (CStreamConfiguration*)sc;
    *equal = mFormat == other->mFormat &&
            mWidth == other->mWidth &&
            mHeight == other->mHeight &&
            mInput == other->mInput;
    return NOERROR;

    return NOERROR;
}

ECode CStreamConfiguration::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return HashCodeHelpers::GetHashCode(mFormat, mWidth, mHeight, mInput ? 1 : 0, hashCode);
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
