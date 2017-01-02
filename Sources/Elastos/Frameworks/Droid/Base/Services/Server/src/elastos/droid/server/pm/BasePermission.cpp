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

#include "elastos/droid/server/pm/BasePermission.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const Int32 BasePermission::TYPE_NORMAL;
const Int32 BasePermission::TYPE_BUILTIN;
const Int32 BasePermission::TYPE_DYNAMIC;

BasePermission::BasePermission(
    /* [in] */ const String& name,
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 type)
    : mName(name)
    , mSourcePackage(sourcePackage)
    , mType(type)
    // Default to most conservative protection level.
    , mProtectionLevel(IPermissionInfo::PROTECTION_SIGNATURE)
    , mUid(0)
{}

ECode BasePermission::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb += "name=";
    sb += mName;
    sb += " package=";
    sb += mSourcePackage;
    sb += " type=";
    sb += mType;
    sb += " rotectionLevel=";
    sb += mProtectionLevel;
    sb += " uid=";
    sb += mUid;
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
