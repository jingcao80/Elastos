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

#include "elastos/droid/hardware/input/KeyboardLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CAR_INTERFACE_IMPL_3(KeyboardLayout, Object, IKeyboardLayout, IComparable, IParcelable)

KeyboardLayout::KeyboardLayout()
    : mPriority(0)
{}

ECode KeyboardLayout::constructor()
{
    return NOERROR;
}

ECode KeyboardLayout::constructor(
    /* [in] */ const String& descriptor,
    /* [in] */ const String& label,
    /* [in] */ const String& collection,
    /* [in] */ Int32 priority)
{
    mDescriptor = descriptor;
    mLabel = label;
    mCollection = collection;
    mPriority = priority;
    return NOERROR;
}

ECode KeyboardLayout::GetDescriptor(
    /* [out] */ String* descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    *descriptor = mDescriptor;
    return NOERROR;
}

ECode KeyboardLayout::GetLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    return NOERROR;
}

ECode KeyboardLayout::GetCollection(
    /* [out] */ String* collection)
{
    VALIDATE_NOT_NULL(collection);
    *collection = mCollection;
    return NOERROR;
}

ECode KeyboardLayout::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mDescriptor));
    FAIL_RETURN(source->ReadString(&mLabel));
    FAIL_RETURN(source->ReadString(&mCollection));
    FAIL_RETURN(source->ReadInt32(&mPriority));

    return NOERROR;
}

ECode KeyboardLayout::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mDescriptor));
    FAIL_RETURN(dest->WriteString(mLabel));
    FAIL_RETURN(dest->WriteString(mCollection));
    FAIL_RETURN(dest->WriteInt32(mPriority));

    return NOERROR;
}

ECode KeyboardLayout::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(another);
    KeyboardLayout* temp = (KeyboardLayout*)IKeyboardLayout::Probe(another);
    if (!temp) {
        *result = -1;
        return NOERROR;
    }

    // Note that these arguments are intentionally flipped since you want higher priority
    // keyboards to be listed before lower priority keyboards.
    if (temp->mPriority == mPriority) {
        *result = mLabel.CompareIgnoreCase(temp->mLabel);
    }

    if (*result == 0) {
        *result = mCollection.CompareIgnoreCase(temp->mCollection);
    }

    return NOERROR;
}

ECode KeyboardLayout::ToString(
    /* [out] */ String* str)
{
    if (mCollection.IsEmpty()) {
        *str = mLabel;
        return NOERROR;
    }

    StringBuilder sb;
    sb += mLabel;
    sb += " - ";
    sb += mCollection;
    return sb.ToString(str);
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
