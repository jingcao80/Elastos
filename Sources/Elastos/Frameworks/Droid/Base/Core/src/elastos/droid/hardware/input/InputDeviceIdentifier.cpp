
#include "elastos/droid/hardware/input/InputDeviceIdentifier.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CAR_INTERFACE_IMPL_2(InputDeviceIdentifier, Object, IInputDeviceIdentifier, IParcelable)

InputDeviceIdentifier::InputDeviceIdentifier(
    /* [in] */ const String& descriptor,
    /* [in] */ Int32 vendorId,
    /* [in] */ Int32 productId)
    : mDescriptor(descriptor)
    , mVendorId(vendorId)
    , mProductId(productId)
{
}

InputDeviceIdentifier::InputDeviceIdentifier(
   /* [in] */ IParcel* src)
{
    src->ReadString(&mDescriptor);
    src->ReadInt32(&mVendorId);
    src->ReadInt32(&mProductId);
}

ECode InputDeviceIdentifier::GetDescriptor(
    /* [out] */ String* descriptor)
{
    VALIDATE_NOT_NULL(descriptor);

    *descriptor = mDescriptor;
    return NOERROR;
}

ECode InputDeviceIdentifier::GetVendorId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mVendorId;
    return NOERROR;
}

ECode InputDeviceIdentifier::GetProductId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mProductId;
    return NOERROR;
}

ECode InputDeviceIdentifier::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mDescriptor))
    FAIL_RETURN(source->ReadInt32(&mVendorId))
    FAIL_RETURN(source->ReadInt32(&mProductId))

    return NOERROR;
}

ECode InputDeviceIdentifier::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mDescriptor));
    FAIL_RETURN(dest->WriteInt32(mVendorId));
    FAIL_RETURN(dest->WriteInt32(mProductId));

    return NOERROR;
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
