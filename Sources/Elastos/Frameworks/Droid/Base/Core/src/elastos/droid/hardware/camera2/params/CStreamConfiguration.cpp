
#include "elastos/droid/hardware/camera2/params/CStreamConfiguration.h"
//#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSize.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;

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

ECode CStreamConfiguration::constructor()
{
    return NOERROR;
}

ECode CStreamConfiguration::constructor(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean input)
{
    assert(0);
    //FAIL_RETURN(StreamConfigurationMap::CheckArgumentFormatInternal(format))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive")))
    mFormat = format;
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
    VALIDATE_NOT_NULL(size);

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

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IStreamConfiguration::Probe(obj) != NULL) {
        const AutoPtr<CStreamConfiguration> other = (CStreamConfiguration*)IStreamConfiguration::Probe(obj);
        *equal = mFormat == other->mFormat &&
                mWidth == other->mWidth &&
                mHeight == other->mHeight &&
                mInput == other->mInput;
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode CStreamConfiguration::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    assert(0 && "TODO: weit Hardware/Camera2/Utils/HashCodeHelpers");
    //*hashCode = HashCodeHelpers::GetHashCode(mFormat, mWidth, mHeight, mInput ? 1 : 0);
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
