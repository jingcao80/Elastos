
#include "elastos/droid/hardware/camera2/utils/LongParcelable.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(LongParcelable, Object, ILongParcelable, IParcelable)

ECode LongParcelable::constructor()
{
    mNumber = 0;
    return NOERROR;
}

ECode LongParcelable::constructor(
        /* [in] */ Int64 number)
{
    mNumber = number;
    return NOERROR;
}

ECode LongParcelable::constructor(
    /* [in] */ IParcel* _in)
{
    return ReadFromParcel(_in);
}

ECode LongParcelable::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt64(mNumber);
}

ECode LongParcelable::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt64(&mNumber);
}

ECode LongParcelable::GetNumber(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mNumber;
    return NOERROR;
}

ECode LongParcelable::SetNumber(
    /* [in] */ Int64 number)
{
    mNumber = number;
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
