
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CCodecProfileLevel.h"

namespace Elastos {
namespace Droid {
namespace Media {

CCodecProfileLevel::CCodecProfileLevel()
    : mProfile(0)
    , mLevel(0)
{
}

ECode CCodecProfileLevel::GetPrfile(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mProfile;
    return NOERROR;
}

ECode CCodecProfileLevel::SetProfile(
    /* [in] */ Int32 value)
{
    mProfile = value;
    return NOERROR;
}

ECode CCodecProfileLevel::GetLevel(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mLevel;
    return NOERROR;
}

ECode CCodecProfileLevel::SetLevel(
    /* [in] */ Int32 value)
{
    mLevel = value;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
