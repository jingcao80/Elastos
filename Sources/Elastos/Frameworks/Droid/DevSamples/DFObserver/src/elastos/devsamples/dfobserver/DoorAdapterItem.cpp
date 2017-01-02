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

#include "DoorAdapterItem.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {


CAR_INTERFACE_IMPL(DoorAdapterItem, Object, IDoorAdapterItem)

DoorAdapterItem::DoorAdapterItem()
    : mIsOpen(FALSE)
{}

DoorAdapterItem::DoorAdapterItem(
    /* [in] */ const String& name,
    /* [in] */ Boolean isOpen)
    : mName(name)
    , mIsOpen(isOpen)
{
}

ECode DoorAdapterItem::GetIsOpen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mIsOpen;
    return NOERROR;
}

ECode DoorAdapterItem::SetIsOpen(
    /* [in] */ Boolean value)
{
    mIsOpen = value;
    return NOERROR;
}

ECode DoorAdapterItem::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode DoorAdapterItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("Door{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", name:";    sb += mName;
    sb += ", ";         sb += mIsOpen ? "Open" : "Closed";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos