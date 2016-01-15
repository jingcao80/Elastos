
#include "elastos/droid/internal/utility/CParcelableString.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CParcelableString, Object, IParcelableString)

CAR_OBJECT_IMPL(CParcelableString)

ECode CParcelableString::constructor()
{
    return NOERROR;
}

ECode CParcelableString::SetString(
    /* [in] */ const String& str)
{
    mString = str;
    return NOERROR;
}

ECode CParcelableString::GetString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mString;
    return NOERROR;
}

ECode CParcelableString::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadString(&mString);
}

ECode CParcelableString::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteString(mString);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
