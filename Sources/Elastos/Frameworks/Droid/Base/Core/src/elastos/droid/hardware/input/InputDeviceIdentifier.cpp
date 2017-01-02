//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/hardware/input/InputDeviceIdentifier.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CAR_INTERFACE_IMPL_2(InputDeviceIdentifier, Object, IInputDeviceIdentifier, IParcelable)

InputDeviceIdentifier::InputDeviceIdentifier()
    : mVendorId(0)
    , mProductId(0)
{}

ECode InputDeviceIdentifier::constructor()
{
    return NOERROR;
}

ECode InputDeviceIdentifier::constructor(
    /* [in] */ const String& descriptor,
    /* [in] */ Int32 vendorId,
    /* [in] */ Int32 productId)
{
    mDescriptor = descriptor;
    mVendorId = vendorId;
    mProductId = productId;
    return NOERROR;
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
